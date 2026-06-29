#pragma once
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
  load_entries_from_file(const std::string &filename);
};
