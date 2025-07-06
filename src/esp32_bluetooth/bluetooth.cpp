// // TODO: cant connect
// #include <Arduino.h>
// #include "BluetoothSerial.h"
// #include "ELMduino.h"
//
//
// BluetoothSerial SerialBT;
// #define ELM_PORT SerialBT
//
//
//
//
// String name = "OBDII";
//
//
// const bool DEBUG = true;
// const int TIMEOUT = 2000;
// const bool HALT_ON_FAIL = true;
//
//
//
// ELM327 myELM327;
//
//
//
//
// typedef enum { ENG_RPM,
//                SPEED } obd_pid_states;
// obd_pid_states obd_state = ENG_RPM;
//
// float rpm = 0;
// float mph = 0;
//
//
//
//
// uint8_t address[6] = { 0x1C, 0xA1, 0x35, 0x69, 0x8D, 0xC5 };
// void setup() {
//   Serial.begin(115200);
//
//   ELM_PORT.begin();
//
//   if (!ELM_PORT.connect(name)) {
//     Serial.println("Couldn't connect to OBD scanner - Phase 1");
//   }
//   if (!myELM327.begin(ELM_PORT)) {
//     Serial.println("Couldn't connect to OBD scanner - Phase 2");
//     while (1)
//       ;
//   }
//   Serial.println("Attempting to connect to ELM327...");
//
//   if (!myELM327.begin(ELM_PORT, DEBUG, TIMEOUT)) {
//     Serial.println("Couldn't connect to OBD scanner");
//
//     if (HALT_ON_FAIL)
//       while (1)
//         ;
//   }
//
//   Serial.println("Connected to ELM327");
// }
//
//
//
//
// void loop() {
//   switch (obd_state) {
//     case ENG_RPM:
//       {
//         rpm = myELM327.rpm();
//
//         if (myELM327.nb_rx_state == ELM_SUCCESS) {
//           Serial.print("rpm: ");
//           Serial.println(rpm);
//           obd_state = SPEED;
//         } else if (myELM327.nb_rx_state != ELM_GETTING_MSG) {
//           myELM327.printError();
//           obd_state = SPEED;
//         }
//
//         break;
//       }
//
//     case SPEED:
//       {
//         mph = myELM327.mph();
//
//         if (myELM327.nb_rx_state == ELM_SUCCESS) {
//           Serial.print("mph: ");
//           Serial.println(mph);
//           obd_state = ENG_RPM;
//         } else if (myELM327.nb_rx_state != ELM_GETTING_MSG) {
//           myELM327.printError();
//           obd_state = ENG_RPM;
//         }
//
//         break;
//       }
//   }
// }
