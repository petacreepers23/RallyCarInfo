#pragma once

#include "IDevice.h"
#include "Nextion.h"
#include "screenfields.h"

class AmbientSensorDevice : public IDevice {
public:
    AmbientSensorDevice(AmbientScreenFields& screenFields)
        : r_screenFields(screenFields), r_lastUpdateMs(0) {};

    void begin() override;
    void update() override;
    void setRefreshInterval(uint32_t intervalMs) override { r_updateIntervalMs = intervalMs; }
    uint32_t getRefreshInterval() const override { return r_updateIntervalMs; }
    
    const char* getDeviceName() const override { return "AmbientSensorDevice"; }

private:
    AmbientScreenFields r_screenFields;
    unsigned long r_lastUpdateMs;
    unsigned long r_updateIntervalMs = 1000; // ms, configurable
    bool r_initialized = false;
};
