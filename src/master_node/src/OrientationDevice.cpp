#include <Arduino.h>
#include "OrientationDevice.h"
#include <Wire.h>
#include "SparkFun_BNO08x_Arduino_Library.h"
#include <EEPROM.h>

#define EEPROM_SIZE 512
#define ROLL_OFFSET_ADDR 0
#define PITCH_OFFSET_ADDR 4
#define CALIBRATED_FLAG_ADDR 8
#define COMPASS_OFFSET_ADDR 12
#define COMPASS_CALIBRATED_FLAG_ADDR 16


// BNO085 sensor object
BNO08x myIMU;


void OrientationDevice::begin() {
    EEPROM.begin(EEPROM_SIZE);
    loadCalibration();
    Wire.begin();
    initializeSensor();
}

void OrientationDevice::initializeSensor() {
    sensorConnected = false;
    int attempts = 0;
    const int maxAttempts = 10;
    
    while (attempts < maxAttempts && !sensorConnected) {
        if (myIMU.begin() == true) {
            // Enable the rotation vector output (quaternion)
            myIMU.enableRotationVector(50); // Send data update every 50ms (20Hz)
            sensorConnected = true;
            lastValidReading = millis();
            dbSerialPrintln("BNO085 initialized successfully");
        } else {
            attempts++;
            dbSerialPrintln("BNO085 initialization attempt failed\n");
            delay(500);
        }
    }
    
    if (!sensorConnected) {
        // Serial.println("Failed to initialize BNO085 after multiple attempts");
    }
}

void OrientationDevice::resetSensor() {
    // Serial.println("Attempting to reset BNO085 sensor...");
    
    // Try to reinitialize the sensor
    initializeSensor();
    
    lastResetAttempt = millis();
}


// Function to convert quaternion to Euler angles (roll, pitch, yaw) - FACE DOWN sensor
void quaternionToEuler(float qr, float qi, float qj, float qk, float* roll, float* pitch, float* yaw) {
    float sqr = sq(qr);
    float sqi = sq(qi);
    float sqj = sq(qj);
    float sqk = sq(qk);

    // Roll (x-axis rotation) - FLIPPED for face-down sensor
    *roll = atan2(2.0 * (qr * qi + qj * qk), sqr - sqi - sqj + sqk);

    // Pitch (y-axis rotation)
    float sinp = 2.0 * (qr * qj - qk * qi);
    if (abs(sinp) >= 1)
        *pitch = copysign(PI / 2, sinp);
    else
        *pitch = asin(sinp);

    // Yaw (z-axis rotation) - this is our compass heading
    *yaw = atan2(2.0 * (qr * qk + qi * qj), sqr + sqi - sqj - sqk);

    // Convert from radians to degrees
    *roll = *roll * 180.0 / PI;
    *pitch = *pitch * 180.0 / PI;
    *yaw = *yaw * 180.0 / PI;

    // Fix roll for face-down sensor: flip to opposite side
    *roll = *roll + 180.0;
    if (*roll > 180.0) *roll -= 360.0;

    // Normalize yaw to 0-360 degrees
    if (*yaw < 0) *yaw += 360.0;
}

void OrientationDevice::calibrateInclination() {
    if (!sensorConnected) {
        // Serial.println("Cannot calibrate: sensor not connected");
        return;
    }
    
    // Try to get current sensor reading for calibration
    if (myIMU.getSensorEvent() == true) {
        if (myIMU.getSensorEventID() == SENSOR_REPORTID_ROTATION_VECTOR) {
            float quatI = myIMU.getQuatI();
            float quatJ = myIMU.getQuatJ();
            float quatK = myIMU.getQuatK();
            float quatReal = myIMU.getQuatReal();
            
            // Convert quaternion to Euler angles
            float roll, pitch, compass;
            quaternionToEuler(quatReal, quatI, quatJ, quatK, &roll, &pitch, &compass);
            
            // Store current angles as offsets (these represent the "level" position)
            rollOffset = roll;
            pitchOffset = pitch;
            isCalibrated = true;
            saveCalibration();
        }
    } else {
        // Serial.println("Cannot calibrate: no sensor data available");
    }
}

void OrientationDevice::incrementCompassOffset() {
    compassOffset -= 1;  // Decrease offset = increase displayed compass value
    if (compassOffset < 0) compassOffset += 360;
    dbSerialPrint("Compass offset incremented to: ");
    dbSerialPrintln(compassOffset);
    saveCalibration();
}

void OrientationDevice::decrementCompassOffset() {
    compassOffset += 1;  // Increase offset = decrease displayed compass value
    if (compassOffset >= 360) compassOffset -= 360;
    dbSerialPrint("Compass offset decremented to: ");
    dbSerialPrintln(compassOffset);
    saveCalibration();
}



void OrientationDevice::update() {
    unsigned long currentTime = millis();

    // Respect configured refresh interval (non-blocking)
    if (currentTime - r_lastUpdateMs < r_updateIntervalMs) {
        // Still check for timeouts to keep reconnection logic active
        if (sensorConnected && (currentTime - lastValidReading > TIMEOUT_MS)) {
            sensorConnected = false;
        }
        if (!sensorConnected && (currentTime - lastResetAttempt > RESET_INTERVAL_MS)) {
            resetSensor();
        }
        return;
    }
    r_lastUpdateMs = currentTime;
    
    // Check if sensor is connected and try to get data
    if (sensorConnected && myIMU.getSensorEvent() == true) {
        // Check if we have rotation vector data
        if (myIMU.getSensorEventID() == SENSOR_REPORTID_ROTATION_VECTOR) {
            lastValidReading = currentTime;  // Update last valid reading timestamp
            
            float quatI = myIMU.getQuatI();
            float quatJ = myIMU.getQuatJ();
            float quatK = myIMU.getQuatK();
            float quatReal = myIMU.getQuatReal();
            
            // Convert quaternion to Euler angles
            float roll, pitch, compass;
            quaternionToEuler(quatReal, quatI, quatJ, quatK, &roll, &pitch, &compass);


            // Apply calibration offsets if calibrated
            if (isCalibrated) {
                roll -= rollOffset;
                pitch -= pitchOffset;
            }
            
            // Apply compass offset (always apply the offset)
            int compassInt = (int)compass;
            compassInt -= compassOffset;
            if (compassInt < 0) compassInt += 360;
            if (compassInt >= 360) compassInt -= 360;


            // Clamp to display range ±40°
            if (roll > 40.0) roll = 40.0;
            if (roll < -40.0) roll = -40.0;
            if (pitch > 40.0) pitch = 40.0;
            if (pitch < -40.0) pitch = -40.0;
            
            // Convert to 0-80 range (40 = level, 0 = -40°, 80 = +40°)
            int roll80 = (int)(roll + 40.0);
            int pitch80 = (int)(pitch + 40.0);

            // // Print the values
            dbSerialPrint("Compass: ");
            dbSerialPrint(compassInt);
            // offset compass
            dbSerialPrint(" (offset ");
            dbSerialPrint(compassOffset);
            dbSerialPrint(") ");
            dbSerialPrint(" Roll: ");
            dbSerialPrint(roll80);
            dbSerialPrint(" Pitch: ");
            dbSerialPrint(pitch80);
            dbSerialPrintln(); 
            
            // Send to Nextion variables
            r_screenFields.compassDegVar.setValue(compassInt);
            r_screenFields.pitchDegVar.setValue(pitch80);
            r_screenFields.rollDegVar.setValue(roll80);
        }
    }
    
    // Check for sensor timeout or disconnection
    if (sensorConnected && (currentTime - lastValidReading > TIMEOUT_MS)) {
        // Serial.println("IMU sensor timeout detected - no readings for 5 seconds");
        sensorConnected = false;
    }
    
    // If sensor is not connected and enough time has passed since last reset attempt
    if (!sensorConnected && (currentTime - lastResetAttempt > RESET_INTERVAL_MS)) {
        resetSensor();
    }
}

void OrientationDevice::saveCalibration() {
    EEPROM.put(ROLL_OFFSET_ADDR, rollOffset);
    EEPROM.put(PITCH_OFFSET_ADDR, pitchOffset);
    EEPROM.put(CALIBRATED_FLAG_ADDR, isCalibrated);
    EEPROM.put(COMPASS_OFFSET_ADDR, compassOffset);
    EEPROM.commit();
}

void OrientationDevice::loadCalibration() {
    EEPROM.get(ROLL_OFFSET_ADDR, rollOffset);
    EEPROM.get(PITCH_OFFSET_ADDR, pitchOffset);
    EEPROM.get(CALIBRATED_FLAG_ADDR, isCalibrated);
    EEPROM.get(COMPASS_OFFSET_ADDR, compassOffset);
    
    // Check for uninitialized EEPROM (NaN values) and set defaults
    if (isnan(rollOffset)) rollOffset = 0.0;
    if (isnan(pitchOffset)) pitchOffset = 0.0;
    if (compassOffset < 0 || compassOffset >= 360) compassOffset = 0;
    if (isCalibrated != 0 && isCalibrated != 1) isCalibrated = false;
    
    // Debug print loaded values
    dbSerialPrint("Loaded calibration - Roll: ");
    dbSerialPrint(rollOffset);
    dbSerialPrint(" Pitch: ");
    dbSerialPrint(pitchOffset);
    dbSerialPrint(" Compass: ");
    dbSerialPrint(compassOffset);
    dbSerialPrint(" Calibrated: ");
    dbSerialPrintln(isCalibrated);
}