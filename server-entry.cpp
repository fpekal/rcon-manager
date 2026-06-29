#include "server-entry.hpp"

#include <fstream>
#include <iostream>
#include <ranges>

std::vector<ServerEntry>
ServerEntry::load_entries_from_file(const std::string &filename) {

  const char *server_config_path = std::getenv("RCON_SERVERS_CONFIG");
  if (server_config_path == nullptr) {
    std::cerr << "RCON_SERVERS_CONFIG not set" << std::endl;
    exit(5);
  }

  std::ifstream server_config_file(server_config_path);
  if (!server_config_file.is_open()) {
    std::cerr << "Failed to open server config file" << std::endl;
    exit(4);
  }

  std::vector<ServerEntry> entries;
  std::string line;
  while (std::getline(server_config_file, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }

    std::string name;
    std::string address;
    std::string password;
    int port;

    auto parts = line | std::views::split(':');
    auto parts_vec = std::ranges::to<std::vector<std::string>>(parts);

    try {
      name = parts_vec.at(0);
      address = parts_vec.at(1);
      port = std::stoi(parts_vec.at(2));
      password = parts_vec.at(3);
    } catch (...) {
      std::cerr << "Failed to parse server config file" << std::endl;
      std::cerr << "Wrong line: " << line << std::endl;
      exit(7);
    }

    entries.emplace_back(name, address, password, port);
  }
  return entries;
}
