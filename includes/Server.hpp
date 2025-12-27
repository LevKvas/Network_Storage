#pragma once

#include "SFML/Network.hpp"
#include <unordered_map>
#include <mutex>


class Server {
  public:
    explicit Server(int port_): port(port_) {}

    void run() const{
      sf::TcpListener listener; // one listener, socket
      listener.listen(port);

      while(true){
        auto* socket_for_client = new sf::TcpSocket();
        // waiting for someone
        if(listener.accept(*socket_for_client) == sf::SocketStatus::Done) {
			socket_for_client->send("Hello", 5);
            return;
        }
      }
    }

    int get_port() const { return port; }

  private:
    int port{};
    std::unordered_map<std::string, std::string> inf{};
    std::mutex mtx{}; // for inf
};