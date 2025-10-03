#pragma once

#include "IDevice.h"
#include "screenfields.h"


struct OrientationData {
    uint16_t compass;
    uint8_t roll;
    uint8_t pitch;
};

class OrientationDevice : public IDevice {
public:

    OrientationDevice(const OrientationScreenFields& fields)
        : r_currentOrientation{0, 0, 0}, r_screenFields(fields), r_dataReady(false)
    {}

    void begin() override; 
    void update() override; 

    static void onRecvStatic(const uint8_t *macAddr, const uint8_t *data, int dataSize);
    static OrientationDevice* instance;

private:
    void onRecv(const uint8_t *macAddr, const uint8_t *data, int dataSize);
    OrientationData r_currentOrientation;
    OrientationScreenFields r_screenFields;
    volatile bool r_dataReady;
};