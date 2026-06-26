#include <iostream>

#include "protocol.hpp"

int main(int argc, char *argv[]) {
  Packet p{123, PacketType::COMMAND, "say Hello, World!"};
  auto raw_bytes = p.to_raw_bytes();

  for (auto b : raw_bytes) {
    std::cout << static_cast<int>(b) << " ";
  }
  std::cout << std::endl;

  Packet p2 = Packet::from_raw_bytes(raw_bytes);
  std::cout << p2.id << std::endl;
  std::cout << p2.type << std::endl;
  std::cout << p2.payload << std::endl;
}
