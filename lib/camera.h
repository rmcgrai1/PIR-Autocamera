/* CAMERA METHODS AND VARIABLES, camera.h
 * Max Kennard, Ryan McGrail, Shaquille Johnson
 * 2/2/16
 *
 * Provides methods to write commands to Vaddio camera through serial port.
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <Arduino.h>
#include <SoftwareSerial\SoftwareSerial.h> // Allows for Serial Communication over pins 2 and 3
#include "math2.h"
#include "camvars.h"
#include "pir.h"

/*****************************  VARIABLES  *****************************/

// Prebuilt Camera Commands
byte COMMAND_IR_RECEIVE[6] = { 0x81, 0x01, 0x06, 0x08, 0x02, 0xFF }, // Toggle IR Input
		COMMAND_PRESET_RECALL[7] = { 0x81, 0x01, 0x04, 0x3F, 0x02, 0x00, 0xFF }, // Move to Preset
		COMMAND_PRESET_SET[7] = { 0x81, 0x01, 0x04, 0x3F, 0x01, 0x00, 0xFF }, // Change Preset
		COMMAND_CONFIG_MOTOR[8] = { 0x81, 0x01, 0x7E, 0x01, 0x70, 0x00, 0x00,
				0xFF },                     // Set Hard/Soft Stops
		COMMAND_PAN_STOP[9] = { 0x81, 0x01, 0x06, 0x01, 0x09, 0x09, 0x03, 0x03,
				0xFF },               // Camera Pan Stop
		COMMAND_PAN[24] = { 0x81, 0x01, 0x06, 0x02, 0x00,
				0x00,                                  // Pan Camera
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF },

		// PAGE 1
		COMMAND_SET_ADDRESS[4] = { 0x88, 0x30, 0x01, 0xFF }, // Sets camera address (Needed for Daisy Chaining)
		COMMAND_IF_CLEAR[5] = { 0x88, 0x01, 0x00, 0x01, 0xFF }, // Clear Command Buffer

		COMMAND_POWER[6] = { 0x81, 0x01, 0x04, 0x00, 0x00, 0xFF }, // Clear Command Buffer
		COMMAND_ZOOM[11] = { 0x81, 0x01, 0x7E, 0x01, 0x4A, 0x07, 0x00, 0x00,
				0x00, 0x00, 0xFF },   // Zoom Camera
		COMMAND_FOCUS[6] = { 0x81, 0x01, 0x04, 0x38, 0x02, 0xFF }, // Enable Autofocusing

		COMMAND_IRIS[9] =
				{ 0x81, 0x01, 0x04, 0x4B, 0x00, 0x00, 0x00, 0x00, 0xFF }, // Set Iris
		COMMAND_GAIN[9] =
				{ 0x81, 0x01, 0x04, 0x4C, 0x00, 0x00, 0x00, 0x00, 0xFF }, // Set Gain
		COMMAND_BRIGHTNESS[9] = { 0x81, 0x01, 0x04, 0x4D, 0x00, 0x00, 0x00,
				0x00, 0xFF },               // Set Brightness

		// PAGE 2
		COMMAND_BACKLIGHT[9] = { 0x81, 0x01, 0x04, 0x33, 0x00, 0xFF }, // Set Backlight
		COMMAND_APERTURE[9] = { 0x81, 0x01, 0x04, 0x42, 0x00, 0x00, 0x00, 0x00,
				0xFF };               // Set Aperture

// Do not use 0 and 1 if you want to be able to upload code while shield is attached to Arduino
SoftwareSerial CAMERA_SERIAL(2, 3); //2 is TX, 3 is RX

void setupCamera();
void sendCameraCommand(byte command[], int si);
void powerCamera(bool on);
void configCameraMotor(boolean softStops);
void setCameraFocus(boolean autoFocus);

/*****************************  CAMERA FUNCTIONS  *****************************/

// Checks the camera is online and the channel is clear
void setupCamera() {
	Serial.println("Setting up camera...");

	// Begin Serial Communication w/ Camera w/ Baud Rate of 9600
	CAMERA_SERIAL.begin(9600);

	//Send Address command
	sendCameraCommand(COMMAND_SET_ADDRESS, 4);
	delay(50);
	configCameraMotor(true);
	delay(50);
	//Auto focus
	setCameraFocus(false);
	delay(50);
	//Send IF_clear command
	clearCameraBuffer();
	delay(50);

	// Initialize Variables
	panPosition = toPanPosition = PIR_PAN[PIR_2], tiltPosition =
			PIR_TILT[PIR_2], zoomPosition = toZoomPosition = PIR_ZOOM[PIR_2];

	Serial.println("\tDone!");
}

void sendCameraCommand(byte command[], int si) {
	// Send bytes sequentially to camera
	for (int i = 0; i < si; i++)
		CAMERA_SERIAL.write(command[i]);
}

void powerCamera(bool on) {
	/* Message: 8x 01 04 00 0p FF
	 *  p: power, [2 (on), 3 (no)
	 */

	COMMAND_POWER[4] = on ? 0x2 : 0x3;

	// Send Command to Camera
	sendCameraCommand(COMMAND_POWER, 6);
}

void clearCameraBuffer() {
	/* Message: 88 01 00 01 FF */

	// Send Command to Camera
	sendCameraCommand(COMMAND_IF_CLEAR, 5);
}

void configCameraMotor(boolean softStops) {
	/* Message: 8x 01 7E 01 70 00 0p FF
	 *  x: camera number
	 *  p: soft stops, [0 (no), 1 (yes)]
	 */

	COMMAND_CONFIG_MOTOR[6] = softStops ? 0x0 : 0x1;

	// Send Command to Camera
	sendCameraCommand(COMMAND_CONFIG_MOTOR, 8);
}

void setCameraIRReceive(boolean on) {
	/* Message: 8x 01 06 08 0v FF
	 *  x: camera number
	 *  v: on/off, [2 (on), 3 (off)]
	 */

	COMMAND_IR_RECEIVE[4] = on ? 0x02 : 0x03;

	// Send Command to Camera
	sendCameraCommand(COMMAND_IR_RECEIVE, 6);
}

void setCameraFocus(boolean autoFocus) {
	/* Message: 8x 01 04 38 0p FF
	 *  p: auto/manual, [2 (auto), 3 (manual)]
	 */

	COMMAND_FOCUS[4] = autoFocus ? 0x02 : 0x03;

	//logData("Enabled autofocus.");

	// Send Command to Camera
	sendCameraCommand(COMMAND_FOCUS, 6);
}

// Sets the camera to the desired preset
void setCameraPreset(byte newPresetNum) {
	/* Message: 8x 01 04 3F 02 0p FF
	 *  x: camera number
	 *  p: preset number [0, E]
	 */

	String txt = "Changed from preset \"";
	txt += getPresetName(camPreset);
	txt += "\" to \"";
	txt += getPresetName(newPresetNum);
	txt += "\".";
	logData(txt);

	camPreset = newPresetNum;

	COMMAND_PRESET_RECALL[5] = 0x0 + newPresetNum;

	// Send Command to Camera
	sendCameraCommand(COMMAND_PRESET_RECALL, 7);
}

// Changes the desired preset to the camera's current orientation
void changeCameraPreset(byte newPresetNum) {
	/* Message: 8x 01 04 3F 01 0p FF
	 *  x: camera number
	 *  p: preset number [0, E]
	 */

	String txt = "Updated preset \"";
	txt += getPresetName(newPresetNum);
	txt += "\".";
	logData(txt);

	camPreset = newPresetNum;

	COMMAND_PRESET_SET[5] = 0x0 + newPresetNum;

	// Send Command to Camera
	sendCameraCommand(COMMAND_PRESET_SET, 7);
}

void setCameraZoom(float newZoom) {
	/* Message: 8x 01 7E 01 4A 0v 0p 0q 0r 0s FF
	 *  x: camera number
	 *  v: speed, [0, 7]
	 *  pqrs: zoom, [000, 6B3]
	 */

	String txt = "Zoomed from ";
	txt += (100 * zoomPosition);
	txt += "% to ";
	txt += (100 * newZoom);
	txt += "%.";
	logData(txt);

	zoomPosition = newZoom;

	// Insert Zoom Amount into COMMAND_ZOOM Array
	convertShort2Bytes(constrain(newZoom,0,1) * 1715., COMMAND_ZOOM, 6);

	// Send Command to Camera
	sendCameraCommand(COMMAND_ZOOM, 11);
}

void setCameraPosition(byte panSpeed, byte tiltSpeed, float panPosF,
		float tiltPosF) {
	String txt = "Panned from ";
	txt += panPosition;
	txt += " to ";
	txt += panPosF;
	txt += ".";
	logData(txt);

	panPosition = panPosF;

	int panPos = mapf(constrain(panPosF, -170, 170), -170, 170, -32700, 32700),
			tiltPos = mapf(constrain(tiltPosF, -30, 90), -30, 90, -5999, 19499);

	// Insert Speeds into COMMAND_PAN
	COMMAND_PAN[4] = tiltSpeed;
	COMMAND_PAN[5] = panSpeed;

	// Separate Pan Int and Tilt Int into 4 bytes, and Insert Them into COMMAND_PAN
	convertShort2Bytes(panPos, COMMAND_PAN, 6, 15);
	convertShort2Bytes(tiltPos, COMMAND_PAN, 10, 19);

	// Send Command to Camera
	sendCameraCommand(COMMAND_PAN, 24);
}

/*****************************  CAMERA FUNCTIONS, PAGE 2  *****************************/

void setCameraBacklight(boolean on) {
	/* Message: 8x 01 04 00 0p FF
	 *  p: backlight, [2 (on), 3 (no)]
	 */

	COMMAND_BACKLIGHT[4] = on ? 0x02 : 0x03;

	// Send Command to Camera
	sendCameraCommand(COMMAND_BACKLIGHT, 6);
}

void setCameraAperture(float fraction) {
	/* Message: 8x 01 04 00 0p FF
	 *  p: power, [2 (on), 3 (no)
	 */

	convertFraction2Bytes(constrain(fraction, 0, 1), 0, 31, COMMAND_APERTURE,
			6);

	Serial.println("APERTURE!");
	Serial.println(COMMAND_APERTURE[6]);
	Serial.println(COMMAND_APERTURE[7]);

	// Send Command to Camera
	sendCameraCommand(COMMAND_APERTURE, 9);
}

#endif
