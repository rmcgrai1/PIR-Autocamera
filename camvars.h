/* CAMERA METHODS AND VARIABLES, camvars.h
 * Ryan McGrail
 * 1/26/16
 * 
 * Several camera-related constants and variables.
 */

#ifndef CAMVARS_H
#define CAMVARS_H



/*****************************  PROTOTYPES  *****************************/

void setupCamera();

void sendCameraCommand(byte[], int);

void clearCameraBuffer();
void configCameraMotor(boolean);
void setCameraIRReceive(boolean);
void setCameraBacklight(boolean);
void setCameraFocus(boolean);

void setCameraPreset(byte);
void changeCameraPreset(byte);
void setCameraZoom(float);
void setCameraPosition(byte, byte, float, float);


String getPresetName(byte);

/*****************************  CONSTANTS  *****************************/

float 
  CAMERA_FOV = 60,
  CAMERA_ZOOM_MIN = 0.0,
  CAMERA_ZOOM_MAX = 0.4,
  CAMERA_PAN_SPEED = 72.0,
  CAMERA_ZOOM_SPEED = .333;

byte
  PRESET_LEFT = 0x0,
  PRESET_MIDDLE_LEFT = 0x1,
  PRESET_MIDDLE = 0x2,
  PRESET_MIDDLE_RIGHT = 0x3,
  PRESET_RIGHT = 0x4,
  PRESET_ZOOMED_OUT = 0x5;


/*****************************  VARIABLES  *****************************/

float 
  panPosition, toPanPosition, estimatedPanPosition,
  tiltPosition,
  zoomPosition, toZoomPosition, estimatedZoomPosition;
bool 
  canPanCamera = true,
  canZoomCamera = true,
  isAutoCam = true,
  isPreset = false;
byte 
  camPreset = -1,
  toCamPreset;


String getPresetName(byte preset) {
  String name = "Invalid";

  switch(preset) {
    case 0x0: name = "Left";          break;
    case 0x1: name = "Middle-Left";   break;
    case 0x2: name = "Middle";        break;
    case 0x3: name = "Middle-Right";  break;
    case 0x4: name = "Right";         break;
    case 0x5: name = "Zoomed-Out";    break;
  }
  
  return name;
}


#endif
