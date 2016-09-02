
#ifndef ETHERNET2_H
#define ETHERNET2_H

#include <Arduino.h>
#include <SPI\SPI.h>
#include <Ethernet\Ethernet.h> // Ethernet library
#include "sd2.h"      // SD library
#include "camvars.h"
#include "pir.h"
#include "log.h"
#include "json_arduino.h"

typedef struct actionHandler {
	char* action;
	boolean (*handler)(token_list_t*, char*, char*&);
} actionHandler;

typedef struct typeHandler {
	char* type;
	boolean (*defaultHandler)(token_list_t*, char*, char*&);
	int aHandlerNum;
	int aHandlerCurrent;
	actionHandler* aHandlers;
} typeHandler;

/*****************************  VARIABLES  *****************************/

// Ethernet Communication Channels
byte mac[6] = { 0x90, 0xA2, 0xDA, 0x60, 0xC8, 0x20 }; //physical mac address
EthernetServer server(80); //server port

// Server Files
// variable for hosted file
char S_T[5] = "true", S_F[6] = "false";

/*****************************  PROTOTYPES  *****************************/

void enableEthernet();
void disableEthernet();

void setupEthernet();

typeHandler& addTypeHandler(char* type,
		boolean (*defaultHandler)(token_list_t*, char*, char*&),
		int aHandlerNum);
actionHandler& addActionHandler(typeHandler& tHandler, char* action,
		boolean (*handler)(token_list_t*, char*, char*&));
boolean handlerNull(token_list_t* jsonTokens, char* responseString,
		char* &curPos);

boolean handlerLogGet(token_list_t* jsonTokens, char* responseString,
		char* &curPos);
boolean handlerLogClear(token_list_t* jsonTokens, char* responseString,
		char* &curPos);

boolean handlerCamGet(token_list_t* jsonTokens, char* responseString,
		char* &curPos);
boolean handlerCamSetFocus(token_list_t* jsonTokens, char* responseString,
		char* &curPos);

/*****************************  ETHERNET FUNCTIONS  *****************************/

void enableEthernet() {
	digitalWrite(10, LOW);
}

void disableEthernet() {
	digitalWrite(10, HIGH);
}

void setupEthernet() {
	Serial.println("Setting up ethernet connection...");

	//pinMode(10, OUTPUT);

	//enableEthernet();
	Ethernet.begin(mac);
	//disableEthernet();

	Serial.println("\tDone!");

	typeHandler t;
	t = addTypeHandler("log", handlerNull, 2);
	addActionHandler(t, "get", handlerLogGet);
	addActionHandler(t, "clear", handlerLogClear);

	t = addTypeHandler("cam", handlerNull, 2);
	addActionHandler(t, "get", handlerCamGet);
	addActionHandler(t, "set-focus", handlerCamSetFocus);
}

void parseAction(String var, String val) {
	int varID = var.toInt();
	float value = val.toFloat();

	if (val == "")
		return;

	switch (varID) {
	case 1:
		Serial.println(val);
		break;
	case 2:
		Serial.println(val);
		break;
	case 3:
		Serial.println(val);
		break;
	case 4:
		setFOV(value);
		break;
	}
}

void println(EthernetClient client, bool val) {
	client.println(val ? S_T : S_F);
}

char eatChar(String& str, char& c, int& i, int& len) {
	c = (++i < len) ? str.charAt(i) : -1;
	return c;
}

int parsei(String str) {
	if (str == "")
		return -1;

	int len = str.length() + 1;
	char buff[len];

	str.toCharArray(buff, len);

	return atoi(buff);
}

unsigned long parsel(String str) {
	if (str == "")
		return -1;

	int len = str.length() + 1;
	char buff[len];

	str.toCharArray(buff, len);

	return atol(buff);
}

float parsef(String str) {
	if (str == "")
		return -1;

	int len = str.length() + 1;
	char buff[len];

	str.toCharArray(buff, len);

	return atof(buff);
}

char* strcat(char* s, char c) {
	int len = strlen(s);
	s[len] = c;
	s[len + 1] = '\0';

	return s;
}

#define RESPONSE_LENGTH 2000
#define HEADER_LENGTH 500
#define BODY_LENGTH RESPONSE_LENGTH-HEADER_LENGTH
#define LINE_LENGTH 90
#define LINE_MAX BODY_LENGTH/LINE_LENGTH

int decimal(float value, int digits) {
	value = (value < 0) ? -value : value;
	float out = (value - floor(value)) * (int) pow(10, digits);

	return (int) floor(out);
}

boolean handlerNull(token_list_t* jsonTokens, char *responseString,
		char* &curPos) {
	return false;
}

boolean handlerCamGet(token_list_t* jsonTokens, char *responseString,
		char* &curPos) {
	curPos +=
			sprintf(curPos,
					"\"pan\":%d.%d,\"tilt\":%d.%d,\"zoom\":%d.%d,\"auto\":%d,\"pir1\":%d,\"pir2\":%d,\"pir3\":%d,\"fov\":%d.%d,\"mode\":%d,\"preset\":%d,",
					(int) panPosition, decimal(panPosition, 3),
					(int) tiltPosition, decimal(tiltPosition, 3),
					(int) zoomPosition, decimal(zoomPosition, 3), isAutoCam,
					checkPIR(PIR_1), checkPIR(PIR_2), checkPIR(PIR_3),
					(int) CAMERA_FOV, decimal(CAMERA_FOV, 3), isPreset,
					camPreset);

	return true;
}

boolean handlerLogGet(token_list_t* jsonTokens, char *responseString,
		char* &curPos) {
	unsigned long startpos = parsel(json_get_value(jsonTokens, "pos")),
			cNum = 0, lineNum = 0, totalSize;

	char cc;
	File webFile = SD.open(LOG_FILENAME);        // open web page file
	if (webFile) {
		totalSize = webFile.size();

		if (!webFile.seek(startpos))
			curPos += sprintf(curPos, "\"progress\":\"done\",");
		else {
			curPos += sprintf(curPos,
					"\"progress\":\"incomplete\",\"text\": \"");

			while (webFile.available()) {
				cc = webFile.read();
				cNum++;

				if (cc == '\"')
					curPos += sprintf(curPos, "\\\"");
				else if (cc == '\r') {
				} else if (cc == '\n') {
					lineNum++;
					curPos += sprintf(curPos, "<br>");

					if (lineNum >= LINE_MAX
							|| cNum >= RESPONSE_LENGTH - HEADER_LENGTH)
						break;
				} else
					curPos += sprintf(curPos, "%c", cc);
			}

			if (cNum > 0 && lineNum == 0) {
				curPos += sprintf(curPos, "<br>");
				lineNum++;
			}

			webFile.close();

			curPos +=
					sprintf(curPos,
							"\",\"startPos\":%lu,\"pos\":%lu,\"total\":%lu,\"lineNum\":%lu,",
							startpos, cNum, totalSize, lineNum);
		}

		return true;
	} else
		return false;
}

boolean handlerCamSetFocus(token_list_t* jsonTokens, char* responseString,
		char* &curPos) {
	setCameraFocus(parsei(json_get_value(jsonTokens, "value")) ? true : false);
	return true;
}

boolean handlerLogClear(token_list_t* jsonTokens, char* responseString,
		char* &curPos) {
	clearLogASAP = true;
	return true;
}

const int TYPE_HANDLER_NUM = 10;
int typeHandlerCurrent = 0;
typeHandler* tHandlers = (typeHandler*) malloc(
		TYPE_HANDLER_NUM * sizeof(typeHandler));

typeHandler& addTypeHandler(char* type,
		boolean (*defaultHandler)(token_list_t*, char*, char*&),
		int aHandlerNum) {
	typeHandler tHandler = tHandlers[typeHandlerCurrent++];

	Serial.print("\t");
	Serial.println(type);

	tHandler.type = type;
	tHandler.defaultHandler = defaultHandler;
	tHandler.aHandlerNum = aHandlerNum;
	tHandler.aHandlerCurrent = 0;
	tHandler.aHandlers = (actionHandler*) malloc(
			aHandlerNum * sizeof(actionHandler));

	return tHandler;
}

actionHandler& addActionHandler(typeHandler& tHandler, char* action,
		boolean (*handler)(token_list_t*, char*, char*&)) {
	actionHandler aHandler = tHandler.aHandlers[tHandler.aHandlerCurrent++];

	Serial.print("\t\t");
	Serial.println(action);

	aHandler.action = action;
	aHandler.handler = handler;

	return aHandler;
}

boolean runActionHandler(actionHandler& aHandler, token_list_t* jsonTokens,
		char* responseString, char* &curPos) {
	Serial.print("FOUND ACTION! ");
	Serial.println(aHandler.action);

	return aHandler.handler(jsonTokens, responseString, curPos);
}
boolean runTypeHandler(typeHandler& tHandler, token_list_t* jsonTokens,
		char* responseString, char* &curPos) {
	Serial.print("FOUND TYPE! ");
	Serial.println(tHandler.type);

	char* action = json_get_value(jsonTokens, "action");

	actionHandler aHandler;
	for (int i = 0; i < tHandler.aHandlerNum; i++) {
		Serial.print("\t\t");
		Serial.println(aHandler.action);

		aHandler = tHandler.aHandlers[i];
		if (!strcmp(aHandler.action, action))
			return runActionHandler(aHandler, jsonTokens, responseString,
					curPos);
	}

	return tHandler.defaultHandler(jsonTokens, responseString, curPos);
}
boolean runHandler(token_list_t* jsonTokens, char* responseString,
		char* &curPos) {
	char* type = json_get_value(jsonTokens, "type");

	typeHandler tHandler;
	for (int i = 0; i < typeHandlerCurrent; i++) {
		Serial.print("\t");
		Serial.println(tHandler.type);

		tHandler = tHandlers[i];
		if (!strcmp(tHandler.type, type))
			return runTypeHandler(tHandler, jsonTokens, responseString, curPos);
	}

	return false;
}

void hostData() {
	EthernetClient client = server.available();  // try to get client

	if (client)
		if (client.connected()) {
			bool inJunk = true, inVar = true;
			String var = "", val = "", file = "", clientText;

			boolean currentLineIsBlank = true;
			while (client.connected()) {
				if (client.available()) {   // client data available to read
					char cc; // read 1 byte (character) from client
					// last line of client request is blank and ends with \n
					// respond to client only after last line received

					clientText = "";
					do {
						cc = client.read();
						clientText += cc;
					} while (client.available());

					Serial.println(clientText);

					char responseString[RESPONSE_LENGTH] = "", *curPos =
							responseString;

					if (true) {
						client.println("HTTP/1.1 200 OK");
						client.println("Content-Type: json"); // plain text allows for reading from text file
						client.println("Access-Control-Allow-Origin: *");
						client.println();

						char c;
						int len, i, startingLine, lineNum, jsonNum = 0;
						String txt;

						char jsonStr[256] = "";
						token_list_t *jsonTokens = create_token_list(
								10 * 2 + 1);

						strcat(jsonStr, "{");
						//Find Start of JSON
						i = clientText.indexOf("type=") - 1;
						len = clientText.length();
						while (i < len) {
							if (jsonNum > 0)
								strcat(jsonStr, ",");

							while (eatChar(clientText, c, i, len) != '='
									&& c != -1)
								strcat(jsonStr, c);
							strcat(jsonStr, ":");
							while (eatChar(clientText, c, i, len) != '&'
									&& c != -1)
								strcat(jsonStr, c);
							jsonNum++;
						}
						strcat(jsonStr, "}");

						json_to_token_list(jsonStr, jsonTokens);

						bool success = false;

						unsigned long startpos, pos, cNum;

						String type, action, line, logTxt = "";
						type = json_get_value(jsonTokens, "type");
						action = json_get_value(jsonTokens, "action");

						Serial.println(
								"**************************************************");
						Serial.println(type + " " + action);

						curPos += sprintf(curPos,
								"{\"type\":\"%s\",\"action\":\"%s\",",
								json_get_value(jsonTokens, "type"),
								json_get_value(jsonTokens, "action"));

						success = runHandler(jsonTokens, responseString,
								curPos);

						if (!success) {
							if (type == "log") {
								if (action == "deleteLines") {
									success = true;
									deleteUpTo = parsei(
											json_get_value(jsonTokens, "upto"));
								}
							} else if (type == "cam") {
								success = true;

								if (action == "move-ptz") {
									String txt =
											"Client requested PTZ camera movement.";
									logData(txt);

									toPanPosition = parsef(
											json_get_value(jsonTokens, "pan"));
									tiltPosition = parsef(
											json_get_value(jsonTokens, "tilt"));
									toZoomPosition = parsef(
											json_get_value(jsonTokens, "zoom"));

									clearCameraBuffer();

									canPanCamera = true;
									setCameraPosition(0x18, 0x14, toPanPosition,
											tiltPosition);
									canZoomCamera = true;
									setCameraZoom(toZoomPosition);
								} else if (action == "move-preset") {
									canPanCamera = true;
									canZoomCamera = true;

									clearCameraBuffer();
									setCameraPreset(
											parsei(
													json_get_value(jsonTokens,
															"preset")));
								} else if (action == "set-aperture")
									setCameraAperture(
											parsef(
													json_get_value(jsonTokens,
															"value")));
								else if (action == "set-backlight")
									setCameraBacklight(
											parsei(
													json_get_value(jsonTokens,
															"value")) ?
											true :
																		false);
								else
									success = false;

							} else if (type == "pir") {
								int id = parsei(
										json_get_value(jsonTokens, "id"));
								if (action == "on") {
									success = true;
									activatePIR(id, true);
								} else if (action == "off") {
									success = true;
									activatePIR(id, false);
								}
							} else if (type == "auto") {
								if (action == "on") {
									success = true;
									isAutoCam = true;

									logData("Automatic control set to \"on\".");
								} else if (action == "off") {
									success = true;
									isAutoCam = false;

									logData(
											"Automatic control set to \"off\".");
								}
							} else if (type == "mode") {
								if (action == "preset") {
									success = true;
									isPreset = true;

									logData("Control mode set to \"preset\".");
								} else if (action == "fov") {
									success = true;
									isPreset = false;

									logData("Control mode set to \"fov\".");
								}
							} else if (type == "preset") {
								byte preset;
								String presetStr = json_get_value(jsonTokens,
										"preset");

								if (action == "set") {
									success = true;

									preset = parsei(presetStr);

									txt = "Updated preset \"";
									txt += getPresetName(preset);
									txt += "\".";
									logData(txt);

									changeCameraPreset(preset);
								}
							}
						}

						release_token_list(jsonTokens);

						curPos += sprintf(curPos, "\"result\":\"%s\"}",
								success ? "success" : "error");
						Serial.println(responseString);
						client.print(responseString);

						if (!success) {
							txt += "ERROR, Invalid command with type \"";
							txt += type;
							txt += "\" and value \"";
							txt += action;
							txt += "\".";
							logData(txt);
						}

						client.stop();

						return;
					}
				}
			}
		}
}

// Checks the SD Card is Installed
void webFileCheck() {
	// check for index.htm file
	//if (!SD.exists("index.htm"))
	//{
//        Serial.println("ERROR - Can't find index.htm file!");
	return;// can't find index file
	//}
//    Serial.println("SUCCESS - Found index.htm file.");
}

#endif
