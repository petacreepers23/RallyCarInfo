
// BLE Service
#include "Arduino.h"
#include "BLEDevice.h"
#include "TyresDevice.h"


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

  // Keep scan duration short to reduce blocking; 1 second is a compromise.
  BLEScanResults foundDevices = pBLEScan->start(1, false);
  pBLEScan->clearResults();   // delete results from BLEScan buffer to release memory
}

// --- Inner callback implementation ---
void TyresDevice::CbBtDeviceAdvertising::decodeManufacturerData(uint8_t *data, uint8_t len, TyreScreenFields& fields) {
  if (len < 7) return;
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

  fields.pressure.setValue(pressure);
  fields.temperature.setValue(temperature);
  fields.battery.setValue(battery);
}

void TyresDevice::CbBtDeviceAdvertising::onResult(BLEAdvertisedDevice dev) {
  BLEAddress *pServerAddress = new BLEAddress(dev.getAddress());
  std::string addrStr = pServerAddress->toString();
  auto it = parent->addressMap.find(addrStr);
  if (it != parent->addressMap.end() && it->second) {
    decodeManufacturerData((uint8_t *)dev.getManufacturerData().data(), dev.getManufacturerData().length(), *(it->second));
    dev.getScan()->stop();
  }
  delete pServerAddress;
}