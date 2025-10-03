#include <Arduino.h>
#include "OrientationDevice.h"
#include <WiFi.h>
#include "ESPNowW.h"

OrientationDevice* OrientationDevice::instance = nullptr;


void OrientationDevice::onRecv(const uint8_t *macAddr, const uint8_t *data, int dataSize) {
    // Minimal processing in callback - just copy data and set flag
    if (dataSize == sizeof(OrientationData)) {
        r_currentOrientation = *(OrientationData*)data;
        r_dataReady = true;  // Signal that new data is available
    }
}

void OrientationDevice::begin() {
    // Serial.begin(115200);
    // Serial.println("Initializing OrientationDevice");
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();
    ESPNow.init();
    instance = this;
    ESPNow.reg_recv_cb(onRecvStatic);
}
void OrientationDevice::onRecvStatic(const uint8_t *macAddr, const uint8_t *data, int dataSize) {
    if (instance) {
        instance->onRecv(macAddr, data, dataSize);
    }
}

void OrientationDevice::update() {
    // Only process if new data is available (UDP-style - latest packet only)
    if (r_dataReady) {
        r_dataReady = false;  // Clear the flag
        
        // Update compass as number
        r_screenFields.compass.setValue(r_currentOrientation.compass);
        
        // Convert pitch (0-180) to degrees (-90 to +90), then map to image IDs (1-17)
        // 0 -> -90°, 90 -> 0°, 180 -> +90°
        int pitchDegrees = r_currentOrientation.pitch - 90;  // Convert to -90 to +90
        // Clamp to -40 to +40 range and map to IDs 1-17
        if (pitchDegrees < -40) pitchDegrees = -40;
        if (pitchDegrees > 40) pitchDegrees = 40;
        uint8_t pitchImageId = 9 + (pitchDegrees * 8) / 40;  // Center at ID 9, ±8 IDs for ±40°
        
        // Convert roll (0-180) to degrees (-90 to +90), then map to image IDs (18-34)
        int rollDegrees = r_currentOrientation.roll - 90;  // Convert to -90 to +90
        // Clamp to -40 to +40 range and map to IDs 18-34
        if (rollDegrees < -40) rollDegrees = -40;
        if (rollDegrees > 40) rollDegrees = 40;
        uint8_t rollImageId = 26 + (rollDegrees * 8) / 40;  // Center at ID 26, ±8 IDs for ±40°
        
        const uint8_t MIN_PIC_ID = 36;
        r_screenFields.rollBig.setPic(rollImageId);
        r_screenFields.pitchBig.setPic(pitchImageId);
        
        r_screenFields.rollSmall.setPic(rollImageId + MIN_PIC_ID);
        r_screenFields.pitchSmall.setPic(pitchImageId + MIN_PIC_ID);
    }

    // Optionally, print debug info
    // char macStr[18];
    // snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
    //          macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4],
    //          macAddr[5]);
    // Serial.print("Last Packet Recv from: ");
    // Serial.println(macStr);
}