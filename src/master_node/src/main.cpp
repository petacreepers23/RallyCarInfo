#include "Nextion.h" 
#include "TyresDevice.h"
#include "OrientationDevice.h"
#include "DeviceManager.h"

DeviceManager deviceManager;

// Nextion global screen components
NexPage mainPage = NexPage(0, 0, "main");
NexPage orientationPage = NexPage(1, 0, "inclination");

NexNumber flPressure(0, 1, "flPressure");
NexNumber flTemp(0, 1, "flTemp");
NexNumber flBattery(0, 1, "flBattery");
TyreScreenFields fl(flPressure, flTemp, flBattery);

NexNumber frPressure(0, 2, "frPressure");
NexNumber frTemp(0, 2, "frTemp"); 
NexNumber frBattery(0, 2, "frBattery");
TyreScreenFields fr(frPressure, frTemp, frBattery);

NexNumber blPressure(0, 3, "blPressure");
NexNumber blTemp(0, 3, "blTemp");
NexNumber blBattery(0, 3, "blBattery");
TyreScreenFields bl(blPressure, blTemp, blBattery);

NexNumber brPressure(0, 4, "brPressure");
NexNumber brTemp(0, 4, "brTemp");
NexNumber brBattery(0, 4, "brBattery");
TyreScreenFields br(brPressure, brTemp, brBattery);

TyresDevice tyresDevice(fl, fr, bl, br);

NexNumber compass(0, 5, "compass");
NexPicture rollBig(1, 1, "rollBig");
NexPicture pitchBig(1, 2, "pitchBig");
NexPicture rollSmall(0, 3, "rollSmall");
NexPicture pitchSmall(0, 4, "pitchSmall");
OrientationScreenFields orientation(compass, rollBig, pitchBig, rollSmall, pitchSmall);
OrientationDevice orientationDevice(orientation);

void setup() {
  nexInit();
  // deviceManager.addDevice(&tyresDevice);
  deviceManager.addDevice(&orientationDevice);
  deviceManager.beginAll();
}

void loop() {
  deviceManager.updateAll();
  delay(50);
}
