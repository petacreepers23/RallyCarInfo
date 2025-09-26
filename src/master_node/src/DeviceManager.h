#pragma once
#include <vector>
#include "IDevice.h"

class DeviceManager {
    std::vector<IDevice*> devices;
public:
    void addDevice(IDevice* device) { devices.push_back(device); }
    void beginAll() { for (auto d : devices) d->begin(); }
    void updateAll() { for (auto d : devices) d->update(); }
};
