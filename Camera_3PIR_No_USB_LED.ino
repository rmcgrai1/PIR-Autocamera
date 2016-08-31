#include <Time.h>
#include <TimeLib.h>

/* Camera Control, Data Logging, Server Posting
 * Max Kennard, Ryan McGrail, Shaquille Johnson
 * December 9, 2015
 * Purpose: Controls the position of the camera based on input from multiple PIR Sensors
 *          and keeps a log file of the times when the camera position was changed. 
 *          Data is posted to server from an external html file that reads the data file.
 * Status: Working
 * 
 * http://b003atardeth1.campus.nd.edu/
 * 
 */
 
/* Reserved Pins - DO NOT USE
 SD card attached to SPI bus as follows:
 Terminal - pin 0 & 1
 Camera Serial - pin 2 & 3
 CS - pin 4
 Ethernet - pin 10
 MOSI - pin 11
 MISO - pin 12
 CLK - pin 13
*/

// Necessary Libraries

#include <SPI.h> // Serial Peripheral Interface. Helps with Ethernet Shield
#include "camvars.h"
#include "math2.h"
#include "sd2.h"
#include "camera.h"
#include "ethernet2.h"
#include "led.h"
#include "pir.h"
#include "time.h"


void setup() {  
  // Open Communication Channels
  Serial.begin(230400); // Serial Communication w/ Computer for Debugging

  pinMode(4, OUTPUT);
  pinMode(10, OUTPUT);

  digitalWrite(4, LOW);
  digitalWrite(10, HIGH);
 
  // Initialize SD Card
  setupSD(); 

  logData("**BOOTED UP ARDUINO!**");
  
  digitalWrite(10, LOW);

  setupEthernet();
    logData(">>Successfully linked to internet!");

  
  setupTime();

    logData(">>Successfully obtained NTP time!");


  server.begin();
  
  // Initialize LED Pins
  setupLEDS();
 
  webFileCheck();

  
  // Initialize Camera
  setupCamera();
  
  // Initialize PIR Pins
  setupPIRs();

  // Reset Camera to Starting Position
  setCameraZoom(zoomPosition);
  setCameraPosition(0x18,0x14,panPosition, tiltPosition);
}


      
void loop() {
  loopTime();

  // Update 
  loopSD();

  // Update PIR Sensor Input
  loopPIRS();

  // Set Green LEDS to Match Camera Rotation
  writeLEDPos(estimatedPanPosition, CAMERA_FOV);

  // Iterate Estimated Pan Position
  if(estimatedPanPosition != panPosition) {
    int iniSign = sign(panPosition-estimatedPanPosition);
    estimatedPanPosition = estimatedPanPosition + iniSign * CAMERA_PAN_SPEED * (timer-timerPrevious)/1000;

    // If Passed Actual Pan Position, Done!
    if(sign(panPosition-estimatedPanPosition) != iniSign) {
      estimatedPanPosition = panPosition;
      canPanCamera = true;
    }
  }

  // Iterate Estimated Zoom Position
  if(estimatedZoomPosition != zoomPosition) {
    int iniSign = sign(zoomPosition-estimatedZoomPosition);
    estimatedZoomPosition = estimatedZoomPosition + iniSign * CAMERA_ZOOM_SPEED * (timer-timerPrevious)/1000;

    // If Passed Actual Zoom Position, Done!
    if(sign(zoomPosition-estimatedZoomPosition) != iniSign) {
      estimatedZoomPosition = zoomPosition;
      canZoomCamera = true;
    }
  }

  if((canPanCamera && canZoomCamera) || timer-lastPresetTime >= 10000) {   
    if(analyzePIRS() && (camPreset != toCamPreset || panPosition != toPanPosition || zoomPosition != toZoomPosition)) {
      lastPresetTime = timer;

      // Clear Camera Action Buffer, and Delay for 50 ms to Provide Clear Operation Time
      clearCameraBuffer();

      // If Got New Preset, Pan to that Preset!
      if(isPreset) {
        if(camPreset != toCamPreset) {
          setCameraPreset(toCamPreset);
  
          canPanCamera = true;
          panPosition = toPanPosition;
          canZoomCamera = true;
          zoomPosition = toZoomPosition;
        }        
      }
      else {
        // If Got New Pan Position, Pan Camera to that Location
        if(panPosition != toPanPosition) {
          canPanCamera = true;
          setCameraPosition(0x18,0x14,toPanPosition,tiltPosition);
        }
  
        // If Got New Zoom Position, Zoom Camera to that Location
        if(zoomPosition != toZoomPosition) {     
          canZoomCamera = true;
          setCameraZoom(toZoomPosition);
        }
      }
    }
  }

  hostData();
}
