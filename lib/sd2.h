/* SD METHODS AND VARIABLES, sd2.h
 * Max Kennard, Ryan McGrail, Shaquille Johnson
 * 1/26/16
 *
 * Provides methods to access SD and write log files to it.
 */

#ifndef SD2_H
#define SD2_H

#include <Arduino.h>
#include <SD\SD.h>             // SD library
#include <time2.h>
#include "log.h"

bool hasLogged = false, clearLogASAP = false;
int deleteUpTo = -1;

/*****************************  CONSTANTS  *****************************/

const int SD_PIN = 4;

/*****************************  PROTOTYPES  *****************************/

void setupSD();
void logData(String, String);
bool clearFile(const char[]);
bool copyFile(const char[], unsigned long, const char[]);

/*****************************  SD FUNCTIONS  *****************************/

void enableSD() {
	digitalWrite(SD_PIN, LOW);
}

void disableSD() {
	digitalWrite(SD_PIN, HIGH);
}

// Checks the SD Card is Installed
void setupSD() {
	Serial.println("Setting up SD card...");

	// see if the card is present and can be initialized:
	//pinMode(SD_PIN, OUTPUT);

	//enableSD();
	if (!SD.begin(SD_PIN)) {
		Serial.println(
				"************FAILED TO LOAD SD CARD!!!!!***************");
		// don't do anything more:
		return;
	}
	//disableSD();

	Serial.println("\tDone!");
}

void logData(String exportData) {

	// open the file. note that only one file can be open at a time,
	// so you have to close this one before opening another.
	File dataFile = SD.open(LOG_FILENAME, FILE_WRITE);

	String logStr = timeString + " | " + exportData;

	int numBytes;

	// if the file is available, write to it:
	if (dataFile) {
		numBytes = dataFile.println(logStr);

		Serial.print("Original size: ");
		Serial.println(dataFile.size());

		Serial.print("Printed ");
		Serial.print(numBytes);
		Serial.print("bytes, ");

		//pushLog(logStr);
		// print to the serial port too:
		Serial.println(exportData);

		Serial.print("After size: ");
		Serial.println(dataFile.size());

		dataFile.close();
	}
	// if the file isn't open, pop up an error:
	else
		Serial.println("Error opening datalog.txt!");
}

void loopSD() {
	File dataFile;

	if (clearLogASAP) {
		if (clearFile(LOG_FILENAME)) {
			clearLogASAP = false;
		}
	} else if (deleteUpTo != -1) {
		clearFile(TMP_FILENAME);

		copyFile(LOG_FILENAME, 0, BKP_FILENAME);
		copyFile(LOG_FILENAME, deleteUpTo, TMP_FILENAME);
		clearFile(LOG_FILENAME);
		copyFile(TMP_FILENAME, 0, LOG_FILENAME);

		deleteUpTo = -1;
	}
}

bool clearFile(const char fileName[]) {
	File file;
	if (file = SD.openOverwrite(fileName)) {
		file.close();
		return true;
	} else
		return false;
}

bool printFile(const char fileName[]) {
	String txt = "";
	unsigned long cNum = 0, lineNum = 0;
	char c;

	File file;

	Serial.print("Printing (");
	Serial.print(fileName);
	Serial.println(").");

	// ONLY ONE FILE CAN BE OPEN AT A TIME!!!!!!!!!!!
	file = SD.open(fileName);        // open web page file
	if (file) {
		Serial.println("\tOpened source file!");

		while (file.available()) {
			c = file.read();

			if (c == '\r') {
			} else if (c == '\n') {
				if (lineNum >= 40) {
					Serial.println(txt);
					txt = "";
					lineNum = 0;
					cNum = 0;
				} else {
					txt += "\r\n";
					lineNum++;
				}
			} else {
				if (cNum >= 2000) {
					Serial.println(txt);
					txt = "";
					lineNum = 0;
					cNum = 0;
				} else {
					txt += c;
					cNum++;
				}
			}
		}

		if (txt != "")
			Serial.println(txt);

		file.close();
	} else {
		Serial.println("\tFailed to open source file.");
		return false;
	}
}

bool copyFile(const char srcFileName[], unsigned long srcOffset,
		const char dstFileName[]) {
	String transferStr, line, info;
	File file;
	char c;

	Serial.print("Copying from (");
	Serial.print(srcFileName);
	Serial.print(", ");
	Serial.print(srcOffset);
	Serial.print(") to (");
	Serial.print(dstFileName);
	Serial.println(").");

	// ONLY ONE FILE CAN BE OPEN AT A TIME!!!!!!!!!!!
	while (true) {
		transferStr = "";
		line = "";

		file = SD.open(srcFileName);        // open web page file
		if (file) {
			//Serial.println("\tOpened source file!");

			unsigned long lineNum = 0, cNum = 0;

			if (!file.seek(srcOffset)) {
				Serial.println("\tDone reading source file!");
				file.close();
				return true;
			} else {
				while (file.available()) {
					c = file.read();
					cNum++;
					srcOffset++;

					if (c == '\r') {
					} else if (c == '\n') {
						lineNum++;

						transferStr += line + "\r\n";
						line = "";

						if (lineNum >= 40 || cNum >= 2000)
							break;
					} else
						line += c;
				}

				if (cNum > 0 && lineNum == 0) {
					transferStr += line + "\r\n";
					lineNum++;
				}
			}

			file.close();
		} else {
			Serial.println("\tFailed to open source file.");
			return false;
		}

		if (transferStr == "") {
			Serial.println("\tDone reading source file!");
			return true;
		}

		file = SD.open(dstFileName, FILE_WRITE);
		if (file) {
			file.println(transferStr);
			file.close();
		} else {
			Serial.println("\tFailed to open destination file.");
			return false;
		}
	}

	return true;
}

#endif
