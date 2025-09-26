#pragma once
#include "Nextion.h"
#include <string>

struct TyreScreenFields {
    NexNumber pressure;
    NexNumber temperature;
    NexNumber battery;
    TyreScreenFields(const std::string &pressureName, const std::string &tempName, const std::string &batteryName)
        : pressure(0, 1, pressureName.c_str()),
          temperature(0, 1, tempName.c_str()),
          battery(0, 1, batteryName.c_str()) {}
};
