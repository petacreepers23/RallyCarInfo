// // 1: Central: 80:f3:da:41:b8:d4
// // 2: Sensor: 80:f3:da:41:51:a4

#include "now.h"
#include <WiFi.h>
#include "../common/dto.h"

namespace {
esp_now_peer_info_t peerInfo;
uint8_t kBroadcastAddress[6] = {0x80, 0xF3, 0xDA, 0x41, 0xB8, 0xD4};
} // namespace

namespace now {

// Callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print(F("\r\nLast Packet Send Status:\t"));
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? F("Delivery Success")
                                                : F("Delivery Fail"));
}

bool setup() {
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println(F("Error initializing ESP-NOW"));
    return false;
  }

  esp_now_register_send_cb(onDataSent);

  memcpy(peerInfo.peer_addr, kBroadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println(F("Failed to add peer"));
    return false;
  }
  return true;
}

bool sendTire(TireDto &tireData) {
  // esp_err_t result = esp_now_send( kBroadcastAddress,
  // reinterpret_cast<uint8_t *>(&tireData), sizeof(tireData));
  esp_err_t result =
      esp_now_send(kBroadcastAddress, (uint8_t *)&tireData, sizeof(tireData));
  if (result == ESP_OK) {
    Serial.println(F("Tire data sent successfully"));
  } else {
    Serial.println(F("Failed to send tire data"));
  }
}
} // namespace now

