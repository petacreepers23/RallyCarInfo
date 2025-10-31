#pragma once

#include <unordered_map>
#include "Arduino.h"
#include "BLEDevice.h"
#include "IDevice.h"
#include "screenfields.h"


class TyresDevice : public IDevice {
public:
    TyresDevice(const TyreScreenFields& fl, const TyreScreenFields& fr, const TyreScreenFields& bl, const TyreScreenFields& br)
        : tyreFields{fl, fr, bl, br}, callback(nullptr), pBLEScan(nullptr) {
        addressMap[FL_ADDR] = &tyreFields[0];
        addressMap[FR_ADDR] = &tyreFields[1];
        addressMap[BL_ADDR] = &tyreFields[2];
        addressMap[BR_ADDR] = &tyreFields[3];
    }

    void begin() override; 
    void update() override; 
    void setRefreshInterval(uint32_t intervalMs) override { r_updateIntervalMs = intervalMs; }
    uint32_t getRefreshInterval() const override { return r_updateIntervalMs; }
    
    const char* getDeviceName() const override { return "TyresDevice"; }

private:
    TyreScreenFields tyreFields[4];
    BLEScan *pBLEScan;

    // BLE addresses for mapping
    static constexpr const char* FL_ADDR = "49:18:00:00:04:71";
    static constexpr const char* FR_ADDR = "4a:89:00:00:05:6b";
    static constexpr const char* BL_ADDR = "49:14:00:00:4b:03";
    static constexpr const char* BR_ADDR = "49:18:00:00:86:0a";

    std::unordered_map<std::string, TyreScreenFields*> addressMap;

    class CbBtDeviceAdvertising : public BLEAdvertisedDeviceCallbacks {
    public:
        CbBtDeviceAdvertising(TyresDevice* parent) : parent(parent) {}
        void decodeManufacturerData(uint8_t *data, uint8_t len, TyreScreenFields& fields);
        void onResult(BLEAdvertisedDevice dev) override;
    private:
        TyresDevice* parent;
    };
    CbBtDeviceAdvertising* callback;

    // Scheduling (note: BLE scanning can block; keep intervals modest)
    unsigned long r_lastUpdateMs = 0;
    unsigned long r_updateIntervalMs = 3000; // default 3s scan cadence
};
