#include "Nextion.h" 
#include "TyresDevice.h"
#include "OrientationDevice.h"
#include "DeviceManager.h"
#include "AmbientSensorDevice.h"
// #include "EngineTemperatureDevice.h"

// ESP32 built-in LED pin
#define LED_BUILTIN 2

DeviceManager deviceManager;

// // Nextion global screen components
NexPage mainPage = NexPage(0, 0, "main");
NexPage orientationPage = NexPage(1, 0, "inclination");

// Create tyre screen fields with Nextion global variables
TyreScreenFields fl("tempFL", "voltFL", "barFL");
TyreScreenFields fr("tempFR", "voltFR", "barFR");
TyreScreenFields bl("tempBL", "voltBL", "barBL");
TyreScreenFields br("tempBR", "voltBR", "barBR");

TyresDevice tyresDevice(fl, fr, bl, br);

// Create orientation device with explicit variable names
OrientationScreenFields orientation("pitchDeg", "rollDeg", "compassDeg");
OrientationDevice orientationDevice(orientation);

// Create ambient sensor device
AmbientScreenFields ambientFields("humi", "temp");
AmbientSensorDevice ambientSensor(ambientFields);

// // Create engine temperature device (pin 34 for NTC sensor)
// #define ENGINE_TEMP_PIN 34
// EngineTemperatureDevice engineTemp(ENGINE_TEMP_PIN, "tempMotor");

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
  
  deviceManager.addDevice(&tyresDevice);
  deviceManager.addDevice(&orientationDevice);
  deviceManager.addDevice(&ambientSensor);
  // deviceManager.addDevice(&engineTemp);

  // Configure per-device refresh intervals (ms)
  orientationDevice.setRefreshInterval(5);   // ~20Hz updates
  orientationDevice.setDebugEnabled(false);
  ambientSensor.setRefreshInterval(1000);     // 1Hz updates
  ambientSensor.setDebugEnabled(false);
  // engineTemp.setRefreshInterval(2000);        // 0.5Hz updates (slow for temperature)
  tyresDevice.setRefreshInterval(1000);    // scan every 3s (when enabled)
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