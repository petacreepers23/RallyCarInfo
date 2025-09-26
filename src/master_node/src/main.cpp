#include "Nextion.h" 
#include "tpms.h"
#include "DeviceManager.h"



TyreScreenFields fl("flPressure", "flTemp", "flBattery");
TyreScreenFields fr("frPressure", "frTemp", "frBattery");
TyreScreenFields bl("blPressure", "blTemp", "blBattery");
TyreScreenFields br("brPressure", "brTemp", "brBattery");

TPMS tyresDevice(fl, fr, bl, br);
DeviceManager deviceManager;

void setup() 
{
  nexInit();
  deviceManager.addDevice(&tyresDevice);
  deviceManager.beginAll();
}

void loop() 
{
  deviceManager.updateAll();
  delay(200); // Adjust as needed
}
