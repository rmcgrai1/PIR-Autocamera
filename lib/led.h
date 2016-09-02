#ifndef LED_H
#define LED_H

/*****************************  CONSTANTS  *****************************/

const int

// LED Pins
LED_PIN_11 = 31, LED_PIN_12 = 33, LED_PIN_13 = 35, LED_PIN_21 = 30, LED_PIN_22 =
		32, LED_PIN_23 = 34, LED_PIN_24 = 36, LED_PIN_25 = 38, LED_PIN_26 = 40,

// Array of All LEDS
		LED_PINS[] = { LED_PIN_11, LED_PIN_12, LED_PIN_13, LED_PIN_21,
				LED_PIN_22, LED_PIN_23, LED_PIN_24, LED_PIN_25, LED_PIN_26 };

/*****************************  PROTOTYPES  *****************************/

void setupLEDS();

void writeLED(int, int);
void writeAllLEDS(int);
void writeLEDPos(float, float);
void blinkAllLEDS(int, int, int);

/*****************************  LED FUNCTIONS  *****************************/

// Initialize All LED Pins as Outputs
void setupLEDS() {
	Serial.println("Setting up LEDs...");

	// Enable LED Pins
	for (int i = 0; i < 9; i++)
		pinMode(LED_PINS[i], OUTPUT);

	Serial.println("\tDone!");
}

// Write LED Brightness
void writeLED(int led, int brightness) {

	// If LOW or HIGH, set to 0 or 255 respectively; Also, Constrain Between 0 and 255!
	brightness = (brightness == LOW) ? 0 :
					(brightness == HIGH) ? 255 : constrain(brightness, 0, 255);

	// Write Brightness to Pin
	digitalWrite(LED_PINS[led], brightness);
}

// Write ALL LEDS to Same Brightness
void writeAllLEDS(int brightness) {
	for (int i = 0; i < 9; i++)
		writeLED(i, brightness);
}

// Write Camera Position to Green LEDS
void writeLEDPos(float angle, float fov) {
	float ledDir;
	int brightness;

	// Loop Through Green LEDS
	for (int i = 3; i < 9; i++) {

		// Map Current LED to Relative Direction in Field of View (FOV)
		ledDir = map(i, 3, 8, fov / 2, -fov / 2);

		// Determine Brightness Based on Distance from LED Direction to Camera Direction
		brightness = (int) constrain(
				255 - 40 * fabs(calcAngleDiff(angle, ledDir)), 0, 255);

		// Write LED Brightness
		writeLED(i, brightness);
	}
}

// Blink LEDs
void blinkAllLEDS(int timeOn, int timeOff, int numBlinks) {

	// Blink "numBlinks" Times
	for (int n = 0; n < numBlinks; n++) {

		// Turn On All LEDs and Wait
		writeAllLEDS (HIGH);
		delay(timeOn);

		// Turn Off All LEDs and Wait
		writeAllLEDS (LOW);
		delay(timeOff);
	}
}

#endif
