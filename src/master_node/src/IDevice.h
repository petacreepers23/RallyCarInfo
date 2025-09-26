#pragma once

class IDevice {
public:
    virtual void begin() = 0;
    virtual void update() = 0;
};
