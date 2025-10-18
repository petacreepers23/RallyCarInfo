#include "Nextion.h" 
#include "TyresDevice.h"
#include "OrientationDevice.h"
#include "DeviceManager.h"

// ESP32 built-in LED pin
#define LED_BUILTIN 2

DeviceManager deviceManager;

// // Nextion global screen components
NexPage mainPage = NexPage(0, 0, "main");
NexPage orientationPage = NexPage(1, 0, "inclination");

// NexNumber flPressure(0, 1, "flPressure");
// NexNumber flTemp(0, 1, "flTemp");
// NexNumber flBattery(0, 1, "flBattery");
// TyreScreenFields fl(flPressure, flTemp, flBattery);

// NexNumber frPressure(0, 2, "frPressure");
// NexNumber frTemp(0, 2, "frTemp"); 
// NexNumber frBattery(0, 2, "frBattery");
// TyreScreenFields fr(frPressure, frTemp, frBattery);

// NexNumber blPressure(0, 3, "blPressure");
// NexNumber blTemp(0, 3, "blTemp");
// NexNumber blBattery(0, 3, "blBattery");
// TyreScreenFields bl(blPressure, blTemp, blBattery);

// NexNumber brPressure(0, 4, "brPressure");
// NexNumber brTemp(0, 4, "brTemp");
// NexNumber brBattery(0, 4, "brBattery");
// TyreScreenFields br(brPressure, brTemp, brBattery);

// TyresDevice tyresDevice(fl, fr, bl, br);

// Create orientation device with explicit variable names
OrientationScreenFields orientation("pitchDeg", "rollDeg", "compassDeg");
OrientationDevice orientationDevice(orientation);

// // Calibration button/hotspot
NexButton calibrateButton(1, 4, "calRollPitch");

// Callback function for calibration
void calibrateInclinationCallback(void *ptr) {
  // Turn on LED to indicate button press detected
  digitalWrite(LED_BUILTIN, HIGH);
  dbSerialPrintln("Calibration button pressed");
  
  orientationDevice.calibrateInclination();
  
  // Keep LED on for 1 second, then turn off
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  
}

void setup() {
  // Initialize LED pin
  pinMode(LED_BUILTIN, OUTPUT);
  
  nexInit();
  // Register calibration callback
  calibrateButton.attachPop(calibrateInclinationCallback, &calibrateButton);
  
  // deviceManager.addDevice(&tyresDevice);
  deviceManager.addDevice(&orientationDevice);
  deviceManager.beginAll();
}

// Touch event list for Nextion callbacks
NexTouch *nex_listen_list[] = {
  &calibrateButton,
  NULL
};

void loop() {
  // Handle Nextion touch events
  nexLoop(nex_listen_list);
  
  deviceManager.updateAll();
  delay(50);
}
