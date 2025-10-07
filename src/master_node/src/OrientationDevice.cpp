#include <Arduino.h>
#include "OrientationDevice.h"
#include <Wire.h>
#include "SparkFun_BNO08x_Arduino_Library.h"


// BNO085 sensor object
BNO08x myIMU;


void OrientationDevice::begin() {
    // Initialize I2C
    Wire.begin();
    
    // Initialize BNO085
    while (myIMU.begin() == false) {
        // Serial.println("BNO085 not detected at default I2C address. Check your jumpers and the hookup guide");
    }
    
    // Enable the rotation vector output (quaternion)
    myIMU.enableRotationVector(50); // Send data update every 50ms (20Hz)
}


// Function to convert quaternion to Euler angles (roll, pitch, yaw)
void quaternionToEuler(float qr, float qi, float qj, float qk, float* roll, float* pitch, float* yaw) {
    float sqr = sq(qr);
    float sqi = sq(qi);
    float sqj = sq(qj);
    float sqk = sq(qk);

    // Roll (x-axis rotation)
    *roll = atan2(2.0 * (qr * qi + qj * qk), sqr - sqi - sqj + sqk);

    // Pitch (y-axis rotation)
    float sinp = 2.0 * (qr * qj - qk * qi);
    if (abs(sinp) >= 1)
        *pitch = copysign(PI / 2, sinp); // Use 90 degrees if out of range
    else
        *pitch = asin(sinp);

    // Yaw (z-axis rotation) - this is our compass heading
    *yaw = atan2(2.0 * (qr * qk + qi * qj), sqr + sqi - sqj - sqk);

    // Convert from radians to degrees
    *roll = *roll * 180.0 / PI;
    *pitch = *pitch * 180.0 / PI;
    *yaw = *yaw * 180.0 / PI;

    // Normalize yaw to 0-360 degrees
    if (*yaw < 0) *yaw += 360.0;
}



void OrientationDevice::update() {
        
    // Check if new data is available
    if (myIMU.getSensorEvent() == true) {
        // Check if we have rotation vector data
        if (myIMU.getSensorEventID() == SENSOR_REPORTID_ROTATION_VECTOR) {
            float quatI = myIMU.getQuatI();
            float quatJ = myIMU.getQuatJ();
            float quatK = myIMU.getQuatK();
            float quatReal = myIMU.getQuatReal();
            
            // Convert quaternion to Euler angles
            float roll, pitch, compass;
            quaternionToEuler(quatReal, quatI, quatJ, quatK, &roll, &pitch, &compass);

            // Convert roll to 0-180° range (90° = level)
            // Clamp roll to ±90° then map to 0-180°
            if (roll > 90.0) roll = 90.0;       // Clamp to +90° max
            if (roll < -90.0) roll = -90.0;     // Clamp to -90° min
            float roll180 = roll + 90.0;        // Map -90°→0°, 0°→90°, +90°→180°
            
            // Convert pitch to 0-180° range (90° = level)
            // Clamp pitch to ±90° then map to 0-180°
            if (pitch > 90.0) pitch = 90.0;     // Clamp to +90° max
            if (pitch < -90.0) pitch = -90.0;   // Clamp to -90° min
            float pitch180 = pitch + 90.0;      // Map -90°→0°, 0°→90°, +90°→180°
            
            // Print the values
            // Serial.printf("Compass: %.1f°, Roll: %.1f°, Pitch: %.1f°\n", compass, roll180, pitch180);

            // Update compass as number
            r_screenFields.compass.setValue(compass);
            
            // Convert pitch (0-180) to degrees (-90 to +90), then map to image IDs (1-17)
            // 0 -> -90°, 90 -> 0°, 180 -> +90°
            int pitchDegrees = pitch180 - 90;  // Convert to -90 to +90
            // Clamp to -40 to +40 range and map to IDs 1-17
            if (pitchDegrees < -40) pitchDegrees = -40;
            if (pitchDegrees > 40) pitchDegrees = 40;
            uint8_t pitchImageId = 9 + (pitchDegrees * 8) / 40;  // Center at ID 9, ±8 IDs for ±40°
            
            // Convert roll (0-180) to degrees (-90 to +90), then map to image IDs (18-34)
            int rollDegrees = roll180 - 90;  // Convert to -90 to +90
            // Clamp to -40 to +40 range and map to IDs 18-34
            if (rollDegrees < -40) rollDegrees = -40;
            if (rollDegrees > 40) rollDegrees = 40;
            uint8_t rollImageId = 26 + (rollDegrees * 8) / 40;  // Center at ID 26, ±8 IDs for ±40°
            
            const uint8_t MIN_PIC_ID = 36;
            // r_screenFields.rollBig.setPic(rollImageId);
            // r_screenFields.pitchBig.setPic(pitchImageId);
            
            r_screenFields.rollSmall.setPic(rollImageId + MIN_PIC_ID);
            r_screenFields.pitchSmall.setPic(pitchImageId + MIN_PIC_ID);
            
        
    }

}