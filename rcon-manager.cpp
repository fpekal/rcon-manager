#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "protocol.hpp"
#include "server-entry.hpp"

int connect_to_server(const ServerEntry &entry) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in servaddr;
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(25575);

  inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

  if (connect(sockfd, reinterpret_cast<sockaddr *>(&servaddr),
              sizeof(servaddr))) {
    std::cerr << "Failed to connect" << std::endl;
    return 1;
  }

  auto send_packet = [](int sockfd, const Packet &packet) {
    auto packet_raw = packet.to_raw_bytes();
    write(sockfd, packet_raw.data(), packet_raw.size());
  };

  auto get_packet = [](int sockfd) {
    std::byte buffer[1024];
    size_t length = read(sockfd, buffer, sizeof(buffer));
    return Packet::from_raw_bytes({buffer, buffer + length});
  };

  {
    auto login = Packet::login("amogus");
    send_packet(sockfd, login);

    auto response = get_packet(sockfd);
    std::cout << response.payload << std::endl;
  }

  while (true) {
    std::string line;
    std::getline(std::cin, line);
    auto command = Packet::command(line);
    send_packet(sockfd, command);

    auto response = get_packet(sockfd);
    std::cout << response.payload << std::endl;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  auto entries = ServerEntry::load_entries_from_file("servers.txt");

  if (argc <= 1) {
    std::cout << "Available servers:\n";

    for (auto &entry : entries) {
      std::cout << entry.name << '\n';
    }
  } else if (argc == 2) {
    std::string server_name = argv[1];

    for (auto &entry : entries) {
      if (entry.name == server_name) {
        return connect_to_server(entry);
      }
    }

    std::cerr << "Server not found" << std::endl;
    return 2;
  } else {
    std::cerr << "Invalid arguments" << std::endl;
    return 3;
  }
}
