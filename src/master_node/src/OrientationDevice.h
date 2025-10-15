#pragma once

#include "IDevice.h"
#include "screenfields.h"


class OrientationDevice : public IDevice {
public:

    OrientationDevice(const OrientationScreenFields& fields)
        : r_screenFields(fields), lastValidReading(0), lastResetAttempt(0), sensorConnected(false)
    {}

    void begin() override; 
    void update() override; 

private:
    void initializeSensor();
    void resetSensor();
    OrientationScreenFields r_screenFields;
    unsigned long lastValidReading;
    unsigned long lastResetAttempt;
    static const unsigned long TIMEOUT_MS = 5000;  // 5 seconds timeout
    static const unsigned long RESET_INTERVAL_MS = 2000;  // Try reset every 2 seconds
    bool sensorConnected;
};