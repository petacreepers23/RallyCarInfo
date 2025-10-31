#pragma once

#include <Arduino.h>
#include <utility>

// Modern C++17 debug system for IDevice implementations
class IDevice; // Forward declaration

namespace DeviceDebug {
    // Helper function to safely get device name
    inline const char* getDeviceNameSafe(const IDevice* device) {
        return device ? device->getDeviceName() : "Unknown";
    }
    
    // C++17 fold expressions for variadic template printing with device name
    template<typename... Args>
    void print(const IDevice* device, Args&&... args) {
        if (device && device->isDebugEnabled()) {
            Serial.print("[");
            Serial.print(getDeviceNameSafe(device));
            Serial.print("] ");
            (Serial.print(std::forward<Args>(args)), ...);
        }
    }
    
    template<typename... Args>
    void println(const IDevice* device, Args&&... args) {
        if (device && device->isDebugEnabled()) {
            Serial.print("[");
            Serial.print(getDeviceNameSafe(device));
            Serial.print("] ");
            (Serial.print(std::forward<Args>(args)), ...);
            Serial.println();
        }
    }
}

// Modern convenience macros (minimal, just for 'this' pointer)
#define THIS_DEBUG_PRINT(...) DeviceDebug::print(this, __VA_ARGS__)
#define THIS_DEBUG_PRINTLN(...) DeviceDebug::println(this, __VA_ARGS__)
#define DEVICE_DEBUG_PRINT(device, ...) DeviceDebug::print(device, __VA_ARGS__)
#define DEVICE_DEBUG_PRINTLN(device, ...) DeviceDebug::println(device, __VA_ARGS__)