#pragma once
#include <Arduino.h>
#include "Nextion.h"

class NextionGlobalVariable {
private:
    String variableName;
    int lastValue;
    bool hasChanged;
    
public:
    // Constructor
    NextionGlobalVariable(const String& name) 
        : variableName(name), lastValue(0), hasChanged(true) {}
    
    // Set value (only sends command if value changed)
    void setValue(int value) {
        if (value != lastValue || hasChanged) {
            String command = variableName + "=" + String(value);
            sendCommand(command.c_str());
            lastValue = value;
            hasChanged = false;
        }
    }
    
    // Force update (sends command even if value hasn't changed)
    void forceUpdate(int value) {
        String command = variableName + "=" + String(value);
        sendCommand(command.c_str());
        lastValue = value;
        hasChanged = false;
    }
    
    // Get last set value (note: this is the last value sent, not read from Nextion)
    int getValue() const {
        return lastValue;
    }
    
    // Get variable name
    String getName() const {
        return variableName;
    }
    
    // Mark as changed (will force update on next setValue)
    void markChanged() {
        hasChanged = true;
    }
};