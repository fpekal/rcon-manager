#include "protocol.hpp"

Packet::Packet() {
  id = 0;
  type = PacketType::COMMAND;
  payload = "";
}

Packet::Packet(uint32_t id, PacketType type, const std::string &payload)
    : id(id), type(type), payload(payload) {}

std::vector<std::byte> Packet::to_raw_bytes() const {
  std::vector<std::byte> raw_bytes;

  int32_t length = payload.size() + 9; // 4 - request ID
                                       // 4 - type
                                       // 1 - 1-byte pad

  auto push_uint32 = [&raw_bytes](int32_t data) {
    raw_bytes.push_back(static_cast<std::byte>(data & 0xFF));
    raw_bytes.push_back(static_cast<std::byte>((data >> 8) & 0xFF));
    raw_bytes.push_back(static_cast<std::byte>((data >> 16) & 0xFF));
    raw_bytes.push_back(static_cast<std::byte>((data >> 24) & 0xFF));
  };

  push_uint32(length);
  push_uint32(id);
  push_uint32(static_cast<uint32_t>(type));

  // std::byte is incompatible with char
  for (auto c : payload) {
    raw_bytes.push_back(static_cast<std::byte>(c));
  }

  raw_bytes.push_back(std::byte{0x00});
  return raw_bytes;
}

Packet Packet::from_raw_bytes(const std::vector<std::byte> &raw_bytes) {
  Packet packet;
  auto bytes_iter = raw_bytes.begin();

  auto get_uint32 = [&bytes_iter]() {
    std::byte b1 = *bytes_iter++;
    std::byte b2 = *bytes_iter++;
    std::byte b3 = *bytes_iter++;
    std::byte b4 = *bytes_iter++;
    return static_cast<uint32_t>(b1) | (static_cast<uint32_t>(b2) << 8) |
           (static_cast<uint32_t>(b3) << 16) |
           (static_cast<uint32_t>(b4) << 24);
  };

  uint32_t length = get_uint32();
  packet.id = get_uint32();
  packet.type = static_cast<PacketType>(get_uint32());

  uint32_t payload_length = length - 9;
  packet.payload = std::string(bytes_iter, bytes_iter + payload_length);

  return packet;
}

Packet Packet::login(const std::string &password) {
  return Packet(Packet::generate_id(), PacketType::LOGIN, password);
}

Packet Packet::command(const std::string &text) {
  return Packet(Packet::generate_id(), PacketType::COMMAND, text);
}
