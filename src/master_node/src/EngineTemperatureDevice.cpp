#include "EngineTemperatureDevice.h"
#include "DeviceDebug.h"
#include <Arduino.h>

void EngineTemperatureDevice::begin() {
    pinMode(r_ntcPin, INPUT);
    THIS_DEBUG_PRINTLN("Engine Temperature Device initialized");
}

void EngineTemperatureDevice::update() {
    unsigned long currentTime = millis();
    
    // Check if it's time to update
    if (currentTime - r_lastUpdateMs >= r_updateIntervalMs) {
        r_lastUpdateMs = currentTime;
        
        int temperature = readTemperature();
        
        // Only update if temperature changed significantly (±1°C)
        if (abs(temperature - lastTemperature) >= 1) {
            lastTemperature = temperature;
            r_tempVar.setValue(temperature);
            
            // Debug output
            THIS_DEBUG_PRINTLN("Engine Temperature: ", temperature, "°C");
        }
    }
}

int EngineTemperatureDevice::readTemperature() {
    // Read ADC value (0-4095 for ESP32)
    int adcValue = analogRead(r_ntcPin);
    
    // Convert ADC to voltage (0-3.3V)
    float voltage = (adcValue / 4095.0) * 3.3;
    
    // Calculate NTC resistance using voltage divider
    float ntcResistance = SERIES_RESISTOR * (3.3 - voltage) / voltage;
    
    // Convert resistance to temperature using Steinhart-Hart equation
    float temperature = ntcResistanceToTemperature(ntcResistance);
    
    return (int)temperature;
}

float EngineTemperatureDevice::ntcResistanceToTemperature(float resistance) {
    // Steinhart-Hart equation simplified for B-parameter
    float steinhart;
    steinhart = resistance / NOMINAL_RESISTANCE;           // (R/Ro)
    steinhart = log(steinhart);                           // ln(R/Ro)
    steinhart /= B_COEFFICIENT;                           // 1/B * ln(R/Ro)
    steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15);   // + (1/To)
    steinhart = 1.0 / steinhart;                         // Invert
    steinhart -= 273.15;                                 // Convert to Celsius
    
    return steinhart;
}