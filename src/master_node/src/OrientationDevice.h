#pragma once

#include "IDevice.h"
#include "screenfields.h"


class OrientationDevice : public IDevice {
public:

    OrientationDevice(const OrientationScreenFields& fields)
        : r_screenFields(fields), lastValidReading(0), lastResetAttempt(0), sensorConnected(false),
          rollOffset(0.0), pitchOffset(0.0), compassOffset(0), isCalibrated(false)
    {}

    void begin() override; 
    void update() override; 
    void setRefreshInterval(uint32_t intervalMs) override { r_updateIntervalMs = intervalMs; }
    uint32_t getRefreshInterval() const override { return r_updateIntervalMs; }
    
    const char* getDeviceName() const override { return "OrientationDevice"; }
    
    // Calibration function to be called from Nextion callback
    void calibrateInclination();
    void incrementCompassOffset();
    void decrementCompassOffset();

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
    int compassOffset;
    bool isCalibrated;
    // Scheduling
    unsigned long r_lastUpdateMs = 0;
    unsigned long r_updateIntervalMs = 50; // default ~20Hz to match sensor
};