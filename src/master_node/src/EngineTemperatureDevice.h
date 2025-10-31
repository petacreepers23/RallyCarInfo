#pragma once

#include "IDevice.h"
#include "NextionGlobalVariable.h"

class EngineTemperatureDevice : public IDevice {
public:
    EngineTemperatureDevice(int ntcPin, const String& tempVarName)
        : r_ntcPin(ntcPin), r_tempVar(tempVarName), lastTemperature(-999)
    {}

    void begin() override;
    void update() override;
    void setRefreshInterval(uint32_t intervalMs) override { r_updateIntervalMs = intervalMs; }
    uint32_t getRefreshInterval() const override { return r_updateIntervalMs; }
    
    const char* getDeviceName() const override { return "EngineTemperatureDevice"; }

private:
    int readTemperature();
    float ntcResistanceToTemperature(float resistance);
    
    int r_ntcPin;
    NextionGlobalVariable r_tempVar;
    int lastTemperature;
    
    // NTC thermistor constants
    static constexpr float SERIES_RESISTOR = 10000.0f;  // 10kΩ series resistor
    static constexpr float NOMINAL_RESISTANCE = 10000.0f;  // 10kΩ at 25°C
    static constexpr float NOMINAL_TEMPERATURE = 25.0f;   // 25°C
    static constexpr float B_COEFFICIENT = 3950.0f;       // Typical B value for automotive NTC
    
    // Scheduling
    unsigned long r_lastUpdateMs = 0;
    unsigned long r_updateIntervalMs = 1000; // Default 1Hz update
};