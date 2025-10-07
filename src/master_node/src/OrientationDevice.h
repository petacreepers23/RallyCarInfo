#pragma once

#include "IDevice.h"
#include "screenfields.h"


class OrientationDevice : public IDevice {
public:

    OrientationDevice(const OrientationScreenFields& fields)
        : r_screenFields(fields)
    {}

    void begin() override; 
    void update() override; 

private:
    OrientationScreenFields r_screenFields;
};