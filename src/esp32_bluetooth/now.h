// // 1: Central: 80:f3:da:41:b8:d4
// // 2: Sensor: 80:f3:da:41:51:a4
#pragma once

#include <WiFi.h>
#include "../common/dto.h"
#include <esp_now.h>

namespace now {

bool setup();
bool sendTire(TireDto &tireData);
} // namespace now
