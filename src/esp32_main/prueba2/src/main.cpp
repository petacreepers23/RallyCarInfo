
// BLE Service
#include "Arduino.h"
#include "BLEDevice.h"

// Variables globales
BLEScan* pBLEScan; //init in setup() used in loop()

// TPMS BLE SENSORS known addresses
String knownAddresses[] = { 
  "49:18:00:00:04:71", // front left
  "4a:89:00:00:05:6b", // front right
  "49:14:00:00:4b:03", // back left
  "49:18:00:00:86:0a"  // back right
};   

// Función para decodificar los datos del sensor
void decodeManufacturerData2(uint8_t* data, uint8_t len) {
  if (len < 7) {
    Serial.println("Manufacturer data too short!");
    return;
  }

  // Convertimos los primeros 7 bytes en nibbles (medio byte, 4 bits)
  // Cada byte contiene dos nibbles → 14 nibbles en total
  uint8_t nibbles[14];
  for (int i = 0; i < 7; i++) {
    nibbles[i * 2]     = (data[i] >> 4) & 0x0F;
    nibbles[i * 2 + 1] = data[i] & 0x0F;
  }

  // Extraer batería (en 1/10 V)
  float battery = ((nibbles[2] << 4) | nibbles[3]) / 10.0;

  // Extraer temperatura (en °C)
  int temperature = (nibbles[4] << 4) | nibbles[5];

  // Extraer presión (en 1/10 PSI), con el ajuste -145
  int raw_pressure = (nibbles[7] << 8) + (nibbles[8] << 4) + nibbles[9];
  float pressure_psi = (raw_pressure - 145) / 10.0;
  float pressure = pressure_psi * 0.0689476;

  // Extraer estado
  uint8_t status_byte1 = (nibbles[0] << 4) | nibbles[1];
  uint8_t status_byte2 = (nibbles[1]);  // También se usa por separado

  // Mostrar datos
  Serial.print("Battery: ");
  Serial.print(battery, 1);
  Serial.print(" V, Temp: ");
  Serial.print(temperature);
  Serial.print(" °C, Pressure: ");
  Serial.print(pressure, 1);
  Serial.println(" bar");

  // Estado (bits)
  Serial.print("Status bits: ");
  Serial.print("Byte1: ");
  Serial.print(status_byte1, BIN);
  Serial.print("  Byte2: ");
  Serial.println(status_byte2, BIN);

  // Interpretar los bits del estado (basado en ARSB2H1y)
  if (nibbles[0] & 0x8) Serial.print(" ALARM");
  if (nibbles[0] & 0x4) Serial.print(" ROTATING");
  if (nibbles[0] & 0x2) Serial.print(" STILL");
  if (nibbles[0] & 0x1) Serial.print(" BGROT");

  if (nibbles[1] & 0x8) Serial.print(" DECR2");
  if (nibbles[1] & 0x4) Serial.print(" RISING");
  if (nibbles[1] & 0x2) Serial.print(" DECR1");

  // Batería baja
  if (((nibbles[0] << 4) | nibbles[1]) == 0xFF) Serial.print(" LBATT");

  Serial.println();
}

class CbBtDeviceAdvertising: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice dev){

    //Comprobamos si address esta en la lista
    BLEAddress *pServerAddress = new BLEAddress(dev.getAddress());
    bool known = false;
    for (int i = 0; i < (sizeof(knownAddresses) / sizeof(knownAddresses[0])); i++) {
      if (strcmp(pServerAddress->toString().c_str(), knownAddresses[i].c_str()) == 0)
        known = true;
    }

    //Si esta en la lista obtenemos datos y paramos escaneo
    if (known) {
      Serial.println("DATOS");
      decodeManufacturerData2((uint8_t*)dev.getManufacturerData().data(),
                       dev.getManufacturerData().length());

      dev.getScan()->stop();
    }

    //TODO: estudiar bien si este es el modo correcto de usar los dispositivos bt
  }
};


void setup() {
  // Opening serial port
  Serial.begin(115200);
  delay(100);
  Serial.print("Init BLE. ");
  BLEDevice::init("");

  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new CbBtDeviceAdvertising());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(5, false);
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(2000);
}