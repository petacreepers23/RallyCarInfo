#pragma once
#include <Arduino.h>

enum class PackageType : uint8_t {
  Tire = 0,
  Position = 1,
};

struct TireDto {
  PackageType packageType = PackageType::Tire;
  byte id;
  byte pressure;    // e.g., 20 = 2.0 bar
  byte temperature; // Celsius
  byte battery;     // 31 = 3.1V
};

struct PositionDto {
  PackageType packageType = PackageType::Position;
  float latitude;
  float longitude;
  float altitude;
};

class DataPacket;

class DataPacket {
public:
  PackageType type;

  union DataUnion{
    TireDto tire;
    PositionDto position;

    DataUnion() {} // Required for union
    ~DataUnion() {} // Optional, no dynamic memory
  } data;

  DataPacket() : type(PackageType::Tire) {
    new (&data.tire) TireDto();  // Placement new for default type
  }

  static DataPacket fromBytes(const uint8_t* buffer, size_t length) {
    DataPacket packet;
    if (length < 1) return packet; // invalid

    packet.type = static_cast<PackageType>(buffer[0]);
    switch (packet.type) {
      case PackageType::Tire:
        if (length >= sizeof(TireDto)) {
          memcpy(&packet.data.tire, buffer, sizeof(TireDto));
        }
        break;
      case PackageType::Position:
        if (length >= sizeof(PositionDto)) {
          memcpy(&packet.data.position, buffer, sizeof(PositionDto));
        }
        break;
    }
    return packet;
  }

  void toBytes(uint8_t* buffer) const {
    switch (type) {
      case PackageType::Tire:
        memcpy(buffer, &data.tire, sizeof(TireDto));
        break;
      case PackageType::Position:
        memcpy(buffer, &data.position, sizeof(PositionDto));
        break;
    }
  }

  size_t size() const {
    switch (type) {
      case PackageType::Tire: return sizeof(TireDto);
      case PackageType::Position: return sizeof(PositionDto);
      default: return 0;
    }
  }

  // Convenience accessors
  const TireDto* asTire() const {
    return (type == PackageType::Tire) ? &data.tire : nullptr;
  }

  const PositionDto* asPosition() const {
    return (type == PackageType::Position) ? &data.position : nullptr;
  }
};
