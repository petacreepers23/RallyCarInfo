#include "Nextion.h" 
#include "TyresDevice.h"
#include "OrientationDevice.h"
#include "DeviceManager.h"
#include "AmbientSensorDevice.h"

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

// Create ambient sensor device
AmbientScreenFields ambientFields("humi", "temp");
AmbientSensorDevice ambientSensor(ambientFields);

// Calibration button/hotspot
NexButton calibrateInclinationButton(2, 8, "calRollPitch");
NexButton compassDecButton(2, 10, "compassDec");
NexButton compassIncButton(2, 7, "compassInc");

// Callback function for calibration
void calibrateInclinationCallback(void *ptr) {
  dbSerialPrintln("Calibration button pressed");
  orientationDevice.calibrateInclination();
}

void compassDecCallback(void *ptr) {
  dbSerialPrintln("Compass decrement button pressed");
  orientationDevice.decrementCompassOffset();
}

void compassIncCallback(void *ptr) {
  dbSerialPrintln("Compass increment button pressed");
  orientationDevice.incrementCompassOffset();
}

void setup() {
  // Initialize LED pin
  pinMode(LED_BUILTIN, OUTPUT);
  
  nexInit();
  // Register calibration callback
  calibrateInclinationButton.attachPop(calibrateInclinationCallback, &calibrateInclinationButton);
  compassDecButton.attachPop(compassDecCallback, &compassDecButton);
  compassIncButton.attachPop(compassIncCallback, &compassIncButton);
  
  // deviceManager.addDevice(&tyresDevice);
  deviceManager.addDevice(&orientationDevice);
  deviceManager.addDevice(&ambientSensor);

  // Configure per-device refresh intervals (ms)
  orientationDevice.setRefreshInterval(5);   // ~20Hz updates
  ambientSensor.setRefreshInterval(1000);     // 1Hz updates
  // tyresDevice.setRefreshInterval(3000);    // scan every 3s (when enabled)
  deviceManager.beginAll();
}

// Touch event list for Nextion callbacks
NexTouch *nex_listen_list[] = {
  &calibrateInclinationButton,
  &compassDecButton,
  &compassIncButton,
  NULL
};

void loop() {
  // Handle Nextion touch events
  nexLoop(nex_listen_list);
  
  deviceManager.updateAll();
  // No delay to keep loop responsive; each device self-schedules via refresh interval
}