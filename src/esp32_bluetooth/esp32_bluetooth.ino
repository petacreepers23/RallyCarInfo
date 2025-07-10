// TODO: not work
// #include <WiFi.h>
// #include <car_data.h>
// #include <now.h>
//
//
// void setup() {
//   Serial.begin(115200);
//   Serial.println("Setup...");
//
//   if (!now::setup()){
//     while(1){
//
//     Serial.println("Failed...");
//     }
//   }
// }
//
// void loop() {
//   Serial.println("Sending tire data...");
//   TireDto tireDto{0, 0, 20, 28, 31};
//   now::sendTire(tireDto);
//
//   delay(2000);
// }


#include <esp_now.h>
#include <WiFi.h>
#include "../common/dto.h"

uint8_t broadcastAddress[] = {0x80, 0xF3, 0xDA, 0x41, 0xB8, 0xD4};

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  DataPacket packet; 

  if (random(0,2) == 0){
    TireDto tireDto;
    tireDto.id = random(0,4);
    tireDto.pressure = random(0,30);
    tireDto.temperature = random(0,100);
    tireDto.battery = random(0,100); 

    tireDto.id = 1;
    tireDto.pressure = 12;
    tireDto.temperature = 32;
    tireDto.battery = 98;

    packet.type = PackageType::Tire;
    packet.data.tire = tireDto;
  }
  else {
    PositionDto positionDto;
    positionDto.latitude = random(0, 90) + random(0, 100) / 100.0;
    positionDto.longitude = random(0, 180) + random(0, 100) / 100.0;
    positionDto.altitude = random(0, 5000) + random(0, 100) / 100.0;

    positionDto.latitude = 52.5200; // Example latitude
    positionDto.longitude = 13.4050; // Example longitude
    positionDto.altitude = 34.0; // Example altitude in meters
    packet.type = PackageType::Position;
    packet.data.position = positionDto;
  }

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &packet, sizeof(packet));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}
