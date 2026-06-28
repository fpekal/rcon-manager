#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <optional>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "protocol.hpp"
#include "server-entry.hpp"

int connect_to_server(const ServerEntry &entry) {
  sockaddr_in servaddr;
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(25575);

  inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (connect(sockfd, reinterpret_cast<sockaddr *>(&servaddr),
              sizeof(servaddr))) {
    std::cerr << "Failed to connect" << std::endl;
    return 1;
  }

  fcntl(sockfd, F_SETFL,
        fcntl(sockfd, F_GETFL) | O_NONBLOCK); // Make the socket non-blocking

  auto send_packet = [sockfd](const Packet &packet) {
    auto packet_raw = packet.to_raw_bytes();
    write(sockfd, packet_raw.data(), packet_raw.size());
  };

  auto get_packet = [sockfd]() -> std::optional<Packet> {
    std::byte buffer[1024];
    size_t length = read(sockfd, buffer, sizeof(buffer));

    if (length == -1) {
      // Should it really handle these errors differently?
      // if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return std::nullopt;
      // }
    }

    return Packet::from_raw_bytes({buffer, buffer + length});
  };

  {
    auto login = Packet::login(entry.password);
    send_packet(login);

    Packet response;
    while (true) {
      auto response_opt = get_packet();
      if (response_opt.has_value()) {
        response = response_opt.value();
        break;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    if (login.id != response.id) {
      std::cerr << "Login failed" << std::endl;
      return 6;
    }
  }

  auto recv_thread_handler = [&get_packet](std::stop_token token) {
    while (!token.stop_requested()) {
      auto response = get_packet();

      if (response.has_value()) {
        std::cout << response->payload << std::endl;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
  };

  {
    std::jthread recv_thread(recv_thread_handler);

    std::cout << ".exit or ^C to exit" << std::endl;

    while (true) {
      std::string line;
      std::getline(std::cin, line);

      if (line == ".exit") {
        recv_thread.request_stop();
        break;
      }

      auto command = Packet::command(line);
      send_packet(command);
    }
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
