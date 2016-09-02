#ifndef PIR_H
#define PIR_H

#include <time2.h>
#include "log.h"
#include "camera.h"
#include "led.h"

/****************************  CONSTANTS  *****************************/

const int PIR_1 = 0, PIR_2 = 1, PIR_3 = 2, PIR_PIN[] = { 37, 39, 41 },
		PIR_LIFETIME = 5 * 1000;
int inactiveTimer = 10 * 1000;
const byte PIR_PRESET[] = { PRESET_RIGHT, PRESET_MIDDLE, PRESET_LEFT };
float PIR_TILT[] = { 4, 5, 4 }, PIR_PAN[] =
		{ CAMERA_FOV / 2, 0, -CAMERA_FOV / 2 }, PIR_ZOOM[] = { CAMERA_ZOOM_MAX,
		CAMERA_ZOOM_MAX * .8, CAMERA_ZOOM_MAX };

/***************************  VARIABLES  **************************/

int pirState[] = { LOW, LOW, LOW };
bool pirActivated[] = { false, false, false };
long pirTimeOn[] = { -1, -1, -1 }, pirTimeOff[] = { -1, -1, -1 };

/*****************************  PROTOTYPES  *****************************/

/*PIRChange(int);
 int getPIR();
 long getTime();*/
void addPIRChange(int);
void popPIRChanges();

void setupPIRS();
void loopPIRS();
bool analyzePIRS();

/*****************************  CLASSES *****************************/

class PIRChange {
public:
	PIRChange(int pir) :
			pir(pir), time(timer) {
	}
	;

	int getPIR() const {
		return pir;
	}
	long getTime() const {
		return time;
	}

private:
	int pir;
	long time;
};

int pirChangeLen = 0, pirChangeMaxLen = 20;
long maxPIRChangeLifetime = 10 * 1000;
PIRChange** pirChangeArray = new PIRChange*[pirChangeMaxLen];

void addPIRChange(int pir) {

	// Only Add if There's Space
	if (pirChangeLen < pirChangeMaxLen) {
		// Move All Elements Back One
		for (int i = pirChangeLen - 1; i >= 0; i--)
			pirChangeArray[i + 1] = pirChangeArray[i];

		pirChangeArray[0] = new PIRChange(pir);
		pirChangeLen++;
	}
}

void popPIRChanges() {
	PIRChange* curChange;
	boolean popRest = false;
	int newLen = pirChangeLen;

	// Loop through all PIRChange Objects
	for (int i = 0; i < pirChangeLen; i++) {
		curChange = pirChangeArray[i];

		// If None Have Previously Popped, Check to See if Time is Up
		if (!popRest) {
			if (timer - curChange->getTime() > maxPIRChangeLifetime) {

				// If So, Pop and Delete
				popRest = true;
				newLen--;
				delete curChange;
			}
		}
		// If a Previous PIRChange Object's Time was Up, this One Must Pop Too
		else {
			newLen--;
			delete curChange;
		}
	}

	pirChangeLen = newLen;
}

/*****************************  PIR FUNCTIONS  *****************************/

void setFOV(float newFOV) {
	CAMERA_FOV = newFOV;
	PIR_PAN[0] = CAMERA_FOV / 2;
	PIR_PAN[2] = -CAMERA_FOV / 2;
}

void setupPIRs() {
	// Initialize PIR Pins
	for (int i = 0; i < 3; i++)
		pinMode(PIR_PIN[i], INPUT);

	// Turn Off LEDS
	writeAllLEDS(LOW);

	// Wait for 10 Seconds, So PIRS can Warm Up
	int calibrationTime = 10 * 1000;
	int k = 0, maxK = 0, advanceTime = calibrationTime / 9, blinkTime = 30, d =
			1;
	for (int i = calibrationTime; i >= 0; i -= advanceTime) {
		for (int n = advanceTime; n >= 0; n -= blinkTime) {

			// If Reach Either End, Turn Around!
			d = (k == maxK - 1) ? -d : (k == 0) ? -d : d;

			// Turn Off Previous LED
			writeLED(k, LOW);

			// Move & Turn On New LED
			writeLED(k = constrain(k + d, 0, maxK - 1), HIGH);

			// Pause Before Next LED
			delay(blinkTime);
		}

		maxK++;
	}

	// Turn Off All LEDS for Moment
	writeAllLEDS(LOW);
	delay(200);

	// Blink LEDS Twice
	blinkAllLEDS(50, 200, 2);
}

int checkPIR(int id) {
	return (pirState[id] == HIGH) ? 1 : 0 + 2 * pirActivated[id];
}

void enablePIR(int id) {
	if (pirTimeOn[id] == -1) {
		pirTimeOn[id] = timer;
		pirTimeOff[id] = -1;
	}
}

void disablePIR(int id) {
	if (pirTimeOff[id] == -1) {
		pirTimeOn[id] = -1;
		pirTimeOff[id] = timer;
	}
}

void activatePIR(int id, bool isOn) {
	String txt = "Manually ";
	txt += isOn ? "activated" : "deactivated";
	txt += " PIR #";
	txt += (id + 1);
	txt += ".";
	logData(txt);

	pirActivated[id] = isOn;
}

void loopPIRS() {
	// Turn Off All LEDS
	writeAllLEDS(LOW);

	// Loop through PIR Sensors, Update On/Off Times
	int initialPIRState;
	for (int i = 0; i < 3; i++) {
		initialPIRState = pirState[i];

		// Read State of Current PIR, Enable Respective LED
		pirState[i] = digitalRead(PIR_PIN[i]);
		writeLED(i, pirState[i]);

		// Check PIR Switching from Off to On
		if (initialPIRState == LOW && checkPIR(i)) {
			if (pirState[i] == HIGH) {
				String txt = "Motion sensed at PIR #";
				txt += (i + 1);
				txt += ".";
				logData(txt);
			}

			enablePIR(i);
		}

		// Check PIR Switching from On to Off
		if (initialPIRState == HIGH) {
			if (pirState[i] == LOW) {
				String txt = "Motion ceased at PIR #";
				txt += (i + 1);
				txt += ".";
				logData(txt);
			}

			if (!checkPIR(i))
				disablePIR(i);
		}
	}

	// Remove All Old PIRChange Objects
	popPIRChanges();
}

bool analyzePIRS() {
	bool didUpdate = true;
	long maxOn = maxi(pirTimeOn, 3), maxOff = mina(pirTimeOff, 3);

	if (!isAutoCam)
		return false;

	toCamPreset = camPreset;

	// If No Lights are On...
	if (pirTimeOn[maxOn] == -1) {
		if (timerPrevious - maxOff <= inactiveTimer
				&& timer - maxOff > inactiveTimer) {
			String txt = "All PIRs inactive for ";
			txt += inactiveTimer;
			txt += " seconds. Resetting camera.";
			logData(txt);

			toPanPosition = 0;
			tiltPosition = PIR_TILT[PIR_2];
			toZoomPosition = CAMERA_ZOOM_MIN;

			toCamPreset = PRESET_ZOOMED_OUT;
		} else
			didUpdate = false;
	}

	// Otherwise, if At Least One Light is On...
	else {

		boolean maxed1, maxed2, maxed3, twoPIRS = false;
		maxed1 = pirTimeOn[PIR_1] > PIR_LIFETIME;
		maxed2 = pirTimeOn[PIR_2] > PIR_LIFETIME;
		maxed3 = pirTimeOn[PIR_3] > PIR_LIFETIME;

		if (maxed2) {
			if (maxed1 && maxed3) {
				toPanPosition = 0;
				tiltPosition = PIR_TILT[PIR_2];
				toZoomPosition = CAMERA_ZOOM_MIN;

				toCamPreset = PRESET_ZOOMED_OUT;

				twoPIRS = true;
			} else if (maxed1) {
				toZoomPosition = CAMERA_ZOOM_MAX * .5;
				toPanPosition = (PIR_PAN[PIR_2] + PIR_PAN[PIR_1]) / 2;
				tiltPosition = (PIR_TILT[PIR_2] + PIR_TILT[PIR_1]) / 2;

				toCamPreset = PRESET_MIDDLE_LEFT;

				twoPIRS = true;
			} else if (maxed3) {
				toZoomPosition = CAMERA_ZOOM_MAX * .5;
				toPanPosition = (PIR_PAN[PIR_2] + PIR_PAN[PIR_3]) / 2;
				tiltPosition = (PIR_TILT[PIR_2] + PIR_TILT[PIR_3]) / 2;

				toCamPreset = PRESET_MIDDLE_RIGHT;

				twoPIRS = true;
			}
		}

		if (!twoPIRS) {
			toZoomPosition = PIR_ZOOM[maxOn];
			toPanPosition = PIR_PAN[maxOn];
			tiltPosition = PIR_TILT[maxOn];
			toCamPreset = PIR_PRESET[maxOn];
		}

		if (toPanPosition != panPosition)
			addPIRChange(maxOn);
	}

	return didUpdate;
}

#endif
