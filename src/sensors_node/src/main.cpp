#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "ESPNowW.h"
#include "SparkFun_BNO08x_Arduino_Library.h"

// uint8_t receiver_mac[] = {0x36, 0x33, 0x33, 0x33, 0x33, 0x33};
uint8_t receiver_mac[] = {0x80, 0xF3, 0xDA, 0x41, 0xB8, 0xD4};

// BNO085 sensor object
BNO08x myIMU;

struct OrientationData {
    uint16_t compass;  // 0-360 degrees (2 bytes, full precision)
    uint8_t roll;     // 0-180 degrees (1 byte, full precision)
    uint8_t pitch;    // 0-180 degrees (1 byte, full precision)
}; // Total: 4 bytes with full 0-180° precision


void setup() {
    Serial.begin(115200);
    Serial.println("BNO085 + ESPNow Sensor Demo");
    
    // Initialize I2C
    Wire.begin();
    
    // Initialize BNO085
    while (myIMU.begin() == false) {
        Serial.println("BNO085 not detected at default I2C address. Check your jumpers and the hookup guide");
    }
    
    Serial.println("BNO085 found!");
    
    // Enable the rotation vector output (quaternion)
    myIMU.enableRotationVector(50); // Send data update every 50ms (20Hz)
    
    // Initialize WiFi and ESPNow
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();
    ESPNow.init();
    ESPNow.add_peer(receiver_mac);
    
    Serial.println("System initialized!");
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

void loop() {
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
            Serial.printf("Compass: %.1f°, Roll: %.1f°, Pitch: %.1f°\n", compass, roll180, pitch180);
            
            // Send data via ESPNow
            OrientationData currentOrientation{
                static_cast<uint16_t>(compass),   // 0-360°
                static_cast<uint8_t>(roll180),    // 0-180° 
                static_cast<uint8_t>(pitch180)    // 0-180°
            };
            ESPNow.send_message(receiver_mac, reinterpret_cast<uint8_t*>(&currentOrientation), sizeof(currentOrientation));
        }
    }
    
    delay(50); // Small delay to prevent overwhelming the serial output
}
