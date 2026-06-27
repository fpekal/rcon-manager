#pragma once
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <unistd.h>
#include <vector>

class ServerEntry {
public:
  std::string name;
  std::string address;
  std::string password;
  int port;

  static std::vector<ServerEntry>
  load_entries_from_file(const std::string &filename) {
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

      name = parts_vec[0];
      address = parts_vec[1];
      port = std::stoi(parts_vec[2]);
      password = parts_vec[3];

      entries.emplace_back(name, address, password, port);
    }
    return entries;
  }
};
