#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

// Source: https://minecraft.wiki/w/RCON

enum PacketType : uint32_t {
  RESPONSE = 0,
  COMMAND = 2,
  LOGIN = 3,
};

class Packet {
public:
  uint32_t id;
  PacketType type;
  std::string payload;

  Packet();
  Packet(uint32_t id, PacketType type, const std::string &payload);

  std::vector<std::byte> to_raw_bytes() const;
  static Packet from_raw_bytes(const std::vector<std::byte> &raw_bytes);

  static Packet login(const std::string &password);
  static Packet command(const std::string &text);

private:
  static uint32_t generate_id();
};
