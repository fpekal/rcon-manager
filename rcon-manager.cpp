#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "protocol.hpp"

int main(int argc, char *argv[]) {
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
}
