#pragma once
#include "Nextion.h"
#include <string>

struct TyreScreenFields {
    NexNumber pressure;
    NexNumber temperature;
    NexNumber battery;
    TyreScreenFields(NexNumber pressure, NexNumber temperature, NexNumber battery)
        : pressure(pressure), temperature(temperature), battery(battery) {}
};


struct OrientationScreenFields {
    NexNumber compass;
    NexPicture rollBig;
    NexPicture pitchBig;
    NexPicture rollSmall;
    NexPicture pitchSmall;

    OrientationScreenFields(NexNumber compass, NexPicture rollBig, NexPicture pitchBig, NexPicture rollSmall, NexPicture pitchSmall)
        : compass(compass), rollBig(rollBig), pitchBig(pitchBig), rollSmall(rollSmall), pitchSmall(pitchSmall) {}
};