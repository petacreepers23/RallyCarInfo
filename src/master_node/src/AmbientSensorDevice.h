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

private:
    AmbientScreenFields r_screenFields;
    unsigned long r_lastUpdateMs;
    const unsigned long r_updateIntervalMs = 1000; // ms
    bool r_initialized = false;
};
