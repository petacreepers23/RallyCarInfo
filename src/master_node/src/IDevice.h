#pragma once

#include <Arduino.h>
#include <cstdint>

class IDevice {
public:
    virtual ~IDevice() = default; // C++11: Explicit default destructor
    
    virtual void begin() = 0;
    virtual void update() = 0;
    
    // Configure how often this device should perform its heavy/visible update work.
    // Implementations should be non-blocking and use millis()-based scheduling internally.
    virtual void setRefreshInterval(std::uint32_t intervalMs) = 0;
    virtual std::uint32_t getRefreshInterval() const = 0;
    
    // Debug logging control - C++11: noexcept
    virtual void setDebugEnabled(bool enabled) noexcept { debugEnabled = enabled; }
    virtual bool isDebugEnabled() const noexcept { return debugEnabled; }
    
    // Device identification for debug output
    virtual const char* getDeviceName() const = 0;

protected:
    bool debugEnabled = true; // C++11: Default member initialization
};
