#pragma once

#include <Arduino.h>

class IDevice {
public:
    virtual void begin() = 0;
    virtual void update() = 0;
    // Configure how often this device should perform its heavy/visible update work.
    // Implementations should be non-blocking and use millis()-based scheduling internally.
    virtual void setRefreshInterval(uint32_t intervalMs) = 0;
    virtual uint32_t getRefreshInterval() const = 0;
};
