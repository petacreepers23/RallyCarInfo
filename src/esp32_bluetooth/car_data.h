#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

enum PackageType {
  Tire = 0,
};

struct TireData {
  byte packageType;
  byte id;
  byte pressure;    // e.g., 20 = 2.0 bar
  byte temperature; // Celsius
  byte battery;     // 31 = 3.1V
};
