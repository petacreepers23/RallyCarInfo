
// BLE Service
#include "Arduino.h"
#include "BLEDevice.h"
#include "TyresDevice.h"
#include "DeviceDebug.h"


void TyresDevice::begin() { 
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  callback = new CbBtDeviceAdvertising(this);
  pBLEScan->setAdvertisedDeviceCallbacks(callback);
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
}

void TyresDevice::update() { 
  unsigned long now = millis();
  if (now - r_lastUpdateMs < r_updateIntervalMs) return;
  r_lastUpdateMs = now;

  THIS_DEBUG_PRINTLN("Starting BLE scan...");
  // Keep scan duration short to reduce blocking; 1 second is a compromise.
  BLEScanResults foundDevices = pBLEScan->start(1, false);
  
  THIS_DEBUG_PRINTLN("Scan completed, found ", foundDevices.getCount(), " devices");
  
  pBLEScan->clearResults();   // delete results from BLEScan buffer to release memory
}

// --- Inner callback implementation ---
void TyresDevice::CbBtDeviceAdvertising::decodeManufacturerData(uint8_t *data, uint8_t len, TyreScreenFields& fields) {
  if (len < 7) {
    DEVICE_DEBUG_PRINT(parent, "TyresDevice: Insufficient data length: ");
    DEVICE_DEBUG_PRINTLN(parent, len);
    return;
  }
  
  uint8_t nibbles[14];
  for (int i = 0; i < 7; i++) {
    nibbles[i * 2] = (data[i] >> 4) & 0x0F;
    nibbles[i * 2 + 1] = data[i] & 0x0F;
  }
  
  float battery = ((nibbles[2] << 4) | nibbles[3]) / 10.0;
  int temperature = (nibbles[4] << 4) | nibbles[5];
  int raw_pressure = (nibbles[7] << 8) + (nibbles[8] << 4) + nibbles[9];
  float pressure_psi = (raw_pressure - 145) / 10.0;
  float pressure = pressure_psi * 0.0689476;

  // Debug output
  DEVICE_DEBUG_PRINT(parent, "TyresDevice: Decoded - Temp: ");
  DEVICE_DEBUG_PRINT(parent, temperature);
  DEVICE_DEBUG_PRINT(parent, "°C, Battery: ");
  DEVICE_DEBUG_PRINT(parent, battery);
  DEVICE_DEBUG_PRINT(parent, "V, Pressure: ");
  DEVICE_DEBUG_PRINT(parent, pressure);
  DEVICE_DEBUG_PRINTLN(parent, " bar");

  // Update Nextion global variables
  fields.temperatureVar.setValue(temperature);
  fields.voltageVar.setValue((int)(battery * 10)); // Convert to mV (e.g., 3.2V -> 32)
  fields.pressureVar.setValue((int)(pressure * 100)); // Convert to centibars (e.g., 2.5 bar -> 250)
}

void TyresDevice::CbBtDeviceAdvertising::onResult(BLEAdvertisedDevice dev) {
  BLEAddress *pServerAddress = new BLEAddress(dev.getAddress());
  std::string addrStr = pServerAddress->toString();
  
  auto it = parent->addressMap.find(addrStr);
  if (it != parent->addressMap.end() && it->second) {
    DEVICE_DEBUG_PRINT(parent, "TyresDevice: Found known device: ");
    DEVICE_DEBUG_PRINT(parent, addrStr.c_str());
    DEVICE_DEBUG_PRINTLN(parent, " - Decoding data...");
    decodeManufacturerData((uint8_t *)dev.getManufacturerData().data(), dev.getManufacturerData().length(), *(it->second));
    dev.getScan()->stop();
  }
  delete pServerAddress;
}