#include <esp_now.h>
#include <WiFi.h>
#include "../common/dto.h"


TireDto tireDto;



// callback function that will be executed when data is received
// void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  // memcpy(&tireDto, incomingData, sizeof(tireDto));
  // Serial.printf("- ID: %d\n- Pressure: %d\n- Temperature: %d\n- Battery: %d\n\n",
  //             tireDto.id,
  //             tireDto.pressure,
  //             tireDto.temperature,
  //             tireDto.battery);

void OnDataRecv(const uint8_t* mac, const uint8_t* data, int len) {
  DataPacket packet = DataPacket::fromBytes(data, len);

  switch (packet.type) {
    case PackageType::Tire: {
      const TireDto* tire = packet.asTire();
      if (tire) {
        Serial.printf("Tire ID %d: %.1f bar, %d°C, %.1fV\n",
          tire->id, tire->pressure / 10.0, tire->temperature, tire->battery / 10.0);
      }
      break;
    }
    case PackageType::Position: {
      const PositionDto* pos = packet.asPosition();
      if (pos) {
        Serial.printf("Position: lat %.6f, lon %.6f, alt %.1f m\n",
          pos->latitude, pos->longitude, pos->altitude);
      }
      break;
    }
    default:
      Serial.println("Unknown packet type");
  }
}



 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}
 
void loop() {
  delay(1000);
}
