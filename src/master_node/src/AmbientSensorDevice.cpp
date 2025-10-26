#include "AmbientSensorDevice.h"
#include "NextionGlobalVariable.h" // if your project uses a wrapper for Nextion variables
// (include only if needed in your project layout)

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_HTU21DF.h>

static Adafruit_HTU21DF htu; // single sensor instance shared by device

void AmbientSensorDevice::begin() {
    dbSerialPrintln("AmbientSensorDevice: initializing sensor...");
    Wire.begin();
    if (!htu.begin()) {
        dbSerialPrintln("AmbientSensorDevice: sensor not found - will retry on updates");
        r_initialized = false;
    } else {
        dbSerialPrintln("AmbientSensorDevice: sensor found");
        r_initialized = true;
    }
}

void AmbientSensorDevice::update() {
    unsigned long now = millis();
    if (now - r_lastUpdateMs < r_updateIntervalMs) return;
    r_lastUpdateMs = now;

    if (!r_initialized) {
        if (htu.begin()) {
            dbSerialPrintln("AmbientSensorDevice: sensor init OK");
            r_initialized = true;
        } else {
            // still not initialized, skip this cycle
            return;
        }
    }

    float temperature = htu.readTemperature(); // °C
    float humidity = htu.readHumidity();       // %RH

    if (isnan(temperature) || isnan(humidity)) {
        dbSerialPrintln("AmbientSensorDevice: read failed (NaN)");
        return;
    }

    // Scale by 100 to keep two decimal places in integer Nextion vars (follow project pattern)
    int32_t tempScaled = (int32_t)round(temperature * 100.0f);
    int32_t humScaled  = (int32_t)round(humidity * 100.0f);

    // Update Nextion via screen fields (use setValue as other devices do)
    r_screenFields.temperatureVar.setValue(tempScaled);
    r_screenFields.humidityVar.setValue(humScaled);

    // Debug output using project logger
    dbSerialPrint("AmbientSensorDevice: T=");
    dbSerialPrint(temperature);
    dbSerialPrint(" C H=");
    dbSerialPrint(humidity);
    dbSerialPrintln(" %");
}