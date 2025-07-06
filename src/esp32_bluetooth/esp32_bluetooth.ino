#include <WiFi.h>
#include <car_data.h>
#include <now.h>

namespace{
Now now;
}

void setup() {
  Serial.begin(115200);
  if (!now.setup()){
    while(1){}
  }
}

void loop() {
  Serial.println("Sending tire data...");
  TireData tireData{0, 0, 20, 28, 31};
  now.sendTire(tireData);

  delay(2000);
}
