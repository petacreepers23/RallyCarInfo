#include "AmbientSensorDevice.h"
#include "NextionGlobalVariable.h" // if your project uses a wrapper for Nextion variables
// (include only if needed in your project layout)
#include "DeviceDebug.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_HTU21DF.h>

static Adafruit_HTU21DF htu; // single sensor instance shared by device

void AmbientSensorDevice::begin() {
    THIS_DEBUG_PRINTLN("AmbientSensorDevice: initializing sensor...");
    Wire.begin();
    if (!htu.begin()) {
        THIS_DEBUG_PRINTLN("AmbientSensorDevice: sensor not found - will retry on updates");
        r_initialized = false;
    } else {
        THIS_DEBUG_PRINTLN("AmbientSensorDevice: sensor found");
        r_initialized = true;
    }
}

void AmbientSensorDevice::update() {
    unsigned long now = millis();
    if (now - r_lastUpdateMs < r_updateIntervalMs) return;
    r_lastUpdateMs = now;

    if (!r_initialized) {
        if (htu.begin()) {
            THIS_DEBUG_PRINTLN("AmbientSensorDevice: sensor init OK");
            r_initialized = true;
        } else {
            // still not initialized, skip this cycle
            return;
        }
    }

    int32_t temperature = static_cast<int32_t>(htu.readTemperature()); // °C
    int32_t humidity = static_cast<int32_t>(htu.readHumidity());       // %RH

    if (isnan(temperature) || isnan(humidity)) {
        THIS_DEBUG_PRINTLN("AmbientSensorDevice: read failed (NaN)");
        return;
    }

    // Update Nextion via screen fields (use setValue as other devices do)
    r_screenFields.temperatureVar.setValue(temperature);
    r_screenFields.humidityVar.setValue(humidity);

    // Debug output using project logger
    THIS_DEBUG_PRINTLN("AmbientSensorDevice: T=", temperature, " C H=", humidity, " %");
}