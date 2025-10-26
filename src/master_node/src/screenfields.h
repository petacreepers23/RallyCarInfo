#pragma once
#include "Nextion.h"
#include "NextionGlobalVariable.h"
#include <string>

struct TyreScreenFields {
    NexNumber pressure;
    NexNumber temperature;
    NexNumber battery;
    TyreScreenFields(NexNumber pressure, NexNumber temperature, NexNumber battery)
        : pressure(pressure), temperature(temperature), battery(battery) {}
};


struct OrientationScreenFields {
    NextionGlobalVariable pitchDegVar;
    NextionGlobalVariable rollDegVar;
    NextionGlobalVariable compassDegVar;
    
    OrientationScreenFields(const String& pitchVarName, const String& rollVarName, const String& compassVarName) 
        : pitchDegVar(pitchVarName), rollDegVar(rollVarName), compassDegVar(compassVarName) {}
};

struct AmbientScreenFields {
    NextionGlobalVariable humidityVar;
    NextionGlobalVariable temperatureVar;

    AmbientScreenFields(const String& humidityVarName, const String& temperatureVarName)
        : humidityVar(humidityVarName), temperatureVar(temperatureVarName) {}
};