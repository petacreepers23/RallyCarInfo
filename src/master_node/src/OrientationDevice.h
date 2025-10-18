#pragma once

#include "IDevice.h"
#include "screenfields.h"


class OrientationDevice : public IDevice {
public:

    OrientationDevice(const OrientationScreenFields& fields)
        : r_screenFields(fields), lastValidReading(0), lastResetAttempt(0), sensorConnected(false),
          rollOffset(0.0), pitchOffset(0.0), isCalibrated(false)
    {}

    void begin() override; 
    void update() override; 
    
    // Calibration function to be called from Nextion callback
    void calibrateInclination();

private:
    void initializeSensor();
    void resetSensor();
    void saveCalibration();
    void loadCalibration();
    OrientationScreenFields r_screenFields;
    unsigned long lastValidReading;
    unsigned long lastResetAttempt;
    static const unsigned long TIMEOUT_MS = 5000;  // 5 seconds timeout
    static const unsigned long RESET_INTERVAL_MS = 2000;  // Try reset every 2 seconds
    bool sensorConnected;
    
    // Calibration offsets
    float rollOffset;
    float pitchOffset;
    bool isCalibrated;
};