#pragma once
#include "Nextion.h"
#include "NextionGlobalVariable.h"
#include <string>

struct TyreScreenFields {
    NextionGlobalVariable temperatureVar;
    NextionGlobalVariable voltageVar;
    NextionGlobalVariable pressureVar;
    
    TyreScreenFields(const String& tempVarName, const String& voltVarName, const String& pressureVarName)
        : temperatureVar(tempVarName), voltageVar(voltVarName), pressureVar(pressureVarName) {}
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