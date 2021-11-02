// Main File for NordenBombsight Addon
// Created for the National Museum of World War II Aviation
// Created by the Bombgineers of the University of Colorado, Colorado Springs
//---------------------------------------------------------------------------
// Project Lead: Dylan McWharter
// Hardware Lead: Jadon Wilson
// Software Lead: Zack Dunn
// Systems Lead: Haider Khan
//---------------------------------------------------------------------------
//----------------------------Main File Structure----------------------------
//---------------------------------------------------------------------------
//--Begin
//--Connect to Arduino via Serial Port
//--Detect and Connect to Prepar3d
//--Init Custom Autopilot
//----Via Elevator Trim
//----Via Throttle Control
//--Control Norden Camera
//--Drop Bomb
//--Detect distance from target
//--end

//--------------------------------------
//-------------TODO LIST----------------
//--------------------------------------
//--Distance from target/Score

#include "SerialPort.hpp"
#include "SimConnect.h"
#include <WinUser.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>

using namespace std;
INPUT drop = {0};

//--------------------------------------
//-------SimConnect Variable Init-------
//--------------------------------------
int quit = 0;
HANDLE hSimConnect = NULL;
HRESULT hr;

enum GROUP_ID {
  GROUP0,
};

enum EVENT_ID {
  EVENT_PAUSE_TOGGLE,
  EVENT_CAMERA_UP,
  EVENT_CAMERA_DOWN,
};

enum DATA_DEFINE_ID {
  DEFINITION_1,
};

enum DATA_REQUEST_ID {
  REQUEST_1,
};

enum INPUT_ID {
  INPUT0,
};

struct PlaneData {
  double planePitch;
  double planeTrim;
  double throttlePercent;
  double planeBank;
  double planeHeading;
};

PlaneData pd;
static float cameraPitch = 16.5f;
static float cameraHeading = 0.0f;
static double cameraRate = 0.0f;
static double cameraDisplacement = 0.0f;
static double dropAngle = 80.0f;

float normalize90(float v) {
  while (v < -90.0f)
    v += 180.0f;
  while (v > 90.0f)
    v -= 180.0f;
  return v;
}

void simulationConnect() {
  cout << "Searching for Prepar3d" << endl;
  while (1) {
    if (SUCCEEDED(
            SimConnect_Open(&hSimConnect, "MainFile", NULL, 0, NULL, 0))) {
      cout << "Connected to Prepar3d." << endl;
      break;
    }
  }
}

void dropBomb() {
  // Drop Bomb
  // Simulates Shift+d keypresses
  cout << "Dropping Bomb" << endl;
  drop.type = INPUT_KEYBOARD;
  drop.ki.wVk = VK_SHIFT; // shift
  drop.ki.dwFlags = 0;
  SendInput(1, &drop, sizeof(drop));
  ZeroMemory(&drop, sizeof(drop));
  drop.type = INPUT_KEYBOARD;
  drop.ki.wVk = 0x44; // d
  drop.ki.dwFlags = 0;
  SendInput(1, &drop, sizeof(drop));
  ZeroMemory(&drop, sizeof(drop));
  drop.type = INPUT_KEYBOARD;
  drop.ki.wVk = VK_SHIFT; // shift
  drop.ki.dwFlags = KEYEVENTF_KEYUP;
  SendInput(1, &drop, sizeof(drop));
  ZeroMemory(&drop, sizeof(drop));
  drop.type = INPUT_KEYBOARD;
  drop.ki.wVk = 0x44; // d
  drop.ki.dwFlags = KEYEVENTF_KEYUP;
  SendInput(1, &drop, sizeof(drop));
  ZeroMemory(&drop, sizeof(drop));
  cout << "Bomb Dropped" << endl;

  /*
  Other Keyboard Press variation
  keybd_event(VK_SHIFT, 0x10, KEYEVENTF_EXTENDEDKEY | 0, 0);//shift down
  keybd_event(0x44, 0x44, KEYEVENTF_EXTENDEDKEY | 0, 0);//d down
  keybd_event(VK_SHIFT, 0x10, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
  0);//shift up keybd_event(0x44, 0x44, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
  0);//d up
  */
}

void CALLBACK MyDispatchProcDataReq(SIMCONNECT_RECV *pData, DWORD cbData,
                                    void *pContext) {
  switch (pData->dwID) {
  case SIMCONNECT_RECV_ID_EVENT: {
    SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT *)pData;
    switch (evt->uEventID) {
      // Debug Camera Controls
    case EVENT_CAMERA_DOWN: {
      cameraPitch = normalize90(cameraPitch + 0.5f);
      // SimConnect_CameraSetRelative6DOF(Handle, fDeltaX, fDeltaY, fDeltaZ,
      // fPitchDeg, fBankDeg, fHeadingDeg) hr =
      // SimConnect_CameraSetRelative6DofByName(hSimConnect, "Norden Camera",
      // 0.0f, 0.0f, 0.0f, cameraPitch, SIMCONNECT_CAMERA_IGNORE_FIELD,
      // cameraHeading);
      cout << "Down" << endl;
      break;
    }
    case EVENT_CAMERA_UP: {
      cameraPitch = normalize90(cameraPitch - 0.5f);
      // SimConnect_CameraSetRelative6DOF(Handle, fDeltaX, fDeltaY, fDeltaZ,
      // fPitchDeg, fBankDeg, fHeadingDeg) hr =
      // SimConnect_CameraSetRelative6DofByName(hSimConnect, "Norden Camera",
      // 0.0f, 0.0f, 0.0f, cameraPitch, SIMCONNECT_CAMERA_IGNORE_FIELD,
      // cameraHeading);
      cout << "Up" << endl;
      break;
    }
    default:
      // nothing
      cout << "No Change in Cam" << endl;
      break;
    }
    break;
  }
  case SIMCONNECT_RECV_ID_QUIT: {
    quit = 1;
    break;
  }
  }

  // Autopilot Setup
  SIMCONNECT_RECV_SIMOBJECT_DATA *pObjData =
      (SIMCONNECT_RECV_SIMOBJECT_DATA *)pData;
  PlaneData *pS = (PlaneData *)&pObjData->dwData;
  hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_1, DEFINITION_1,
                                         SIMCONNECT_OBJECT_ID_USER,
                                         SIMCONNECT_PERIOD_ONCE);

  // Autopilot Values
  pd.planePitch = 0;
  pd.throttlePercent = 90.0f;
  pd.planeTrim = 0.3; // edit value for altitude
  pd.planeBank = 0;
  pd.planeHeading = 0;

  // Send Autopilot values to Prepar3d
  hr = SimConnect_SetDataOnSimObject(hSimConnect, DEFINITION_1,
                                     SIMCONNECT_OBJECT_ID_USER, 0, 0,
                                     sizeof(pd), &pd);
  // SimConnect_CameraSetRelative6DOF(Handle, fDeltaX, fDeltaY, fDeltaZ,
  // fPitchDeg, fBankDeg, fHeadingDeg)
  hr = SimConnect_CameraSetRelative6DofByName(
      hSimConnect, "Norden Camera", 0.0f, 0.0f, 0.0f, cameraPitch,
      SIMCONNECT_CAMERA_IGNORE_FIELD, cameraHeading);
  cameraPitch = cameraPitch + (1 * cameraRate) + cameraDisplacement;
  cameraPitch = normalize90(cameraPitch);
  hr = SimConnect_CameraSetRelative6DofByName(
      hSimConnect, "Norden Camera", 0.0f, 0.0f, 0.0f, cameraPitch,
      SIMCONNECT_CAMERA_IGNORE_FIELD, SIMCONNECT_CAMERA_IGNORE_FIELD);
  cameraDisplacement = 0.0f;
  // cout << "Cam Pitch: "<< cameraPitch << "Cam Rate: "<< cameraRate << endl;
  // Drop angle needs to be set to match specific bomb. 80.0f is a dummy value
  if (cameraPitch > dropAngle) {
    dropBomb();
  }
}

void simSetup() {
  // Setup data definition
  hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1,
                                      "plane pitch degrees", "radians");
  hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1,
                                      "elevator trim position", "radians");
  hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1,
                                      "GENERAL ENG THROTTLE LEVER POSITION:1",
                                      "percent");
  hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1,
                                      "plane bank degrees", "radians");
  hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1,
                                      "plane heading degrees true", "radians");

  hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_CAMERA_DOWN);
  hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_CAMERA_UP);

  hr = SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP0,
                                                    EVENT_CAMERA_DOWN);
  hr = SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP0,
                                                    EVENT_CAMERA_UP);

  hr = SimConnect_SetNotificationGroupPriority(
      hSimConnect, GROUP0, SIMCONNECT_GROUP_PRIORITY_HIGHEST);

  hr = SimConnect_MapInputEventToClientEvent(hSimConnect, INPUT0,
                                             "shift+ctrl+1", EVENT_CAMERA_DOWN);
  hr = SimConnect_MapInputEventToClientEvent(hSimConnect, INPUT0,
                                             "shift+ctrl+2", EVENT_CAMERA_UP);

  hr = SimConnect_SetInputGroupState(hSimConnect, INPUT0, SIMCONNECT_STATE_ON);
}

//--------------------------------------
//--Serial Communication Variable Init--
//--------------------------------------
const char *portName = "COM5";
#define MAX_DATA_LENGTH 255
char incomingData[MAX_DATA_LENGTH];
char outgoingData[MAX_DATA_LENGTH] = "Outgoing Test";
int dot_variable = 0;

// Arduino SerialPort object
SerialPort *arduino;

// Receive Data
// Testing the char of incomingData was not woring
// Now testing readResult
void ReceiveData(void) {
  int readResult = arduino->readSerialPort(incomingData, MAX_DATA_LENGTH);
  Sleep(10);
  // cout << readResult << endl;
  switch (readResult) {
  case 3: {
    cout << "Case 3" << endl;
    cameraDisplacement = cameraDisplacement + 0.001f; // Down
    break;
  }
  case 4: {
    cout << "Case 4" << endl;
    cameraDisplacement = cameraDisplacement - 0.001f; // Up
    break;
  }
  case 7: {
    cout << "Case 7" << endl;
    cameraRate = cameraRate + 0.0002f; // Down
    break;
  }
  case 8: {
    cout << "Case 8" << endl;
    cameraRate = cameraRate - 0.0002f; // Up
    break;
  }
  default: {
    // Nothing
    break;
  }
  }
}

// Send Data
// Currently Unused
void SendData(void) {
  if (outgoingData != " ") {
    arduino->writeSerialPort(outgoingData, MAX_DATA_LENGTH);
    memset(outgoingData, 0, sizeof(outgoingData));
  }
  Sleep(10);
}

// Serial AutoConnection
void arduinoConnect(void) {
  while (1) {
    // ui - searching
    cout << "Searching for Norden Bombsight(arduino)";
    // wait connection
    while (!arduino->isConnected()) {
      Sleep(500);
      if (dot_variable <= 2) {
        cout << ".";
        dot_variable++;
      } else {
        cout << "\b\b\b   \b\b\b";
        dot_variable = 0;
      }
      arduino = new SerialPort(portName);
    }
    // Checking if arduino is connected or not
    if (arduino->isConnected()) {
      cout << endl << "Connection established at port " << portName << endl;
      break;
    }
  }
}

//--------------------------------------
//--------Main Function and Loop--------
//--------------------------------------
int main() { // Rename temp to main when ready to implement
  HRESULT hr;
  // Connect to Arduino
  arduino = new SerialPort(portName);
  arduinoConnect();

  // Connect to Prepar3d/Setup Prepar3d
  simulationConnect();
  simSetup();

  // Main Loop
  cout << "\nDebug: Starting main loop..." << endl;
  while (quit == 0) {
    // Custom Autopilot and Camera Control
    SimConnect_CallDispatch(hSimConnect, MyDispatchProcDataReq, NULL);

    if (arduino->isConnected()) {
      ReceiveData();
    } else {
      cout << "Error: Norden(arduino) disconnected! Please restart Norden.exe "
              "and Prepar3d Scenario after reconnecting Norden(arduino)"
           << endl;
      break;
    }
    Sleep(10);
  }

  // Distance From Target
  // TODO

  // Disconnect from Prepar3d
  hr = SimConnect_Close(hSimConnect);
  // Serial Port Close handled in SerialPort.cpp
  return 0;
}
