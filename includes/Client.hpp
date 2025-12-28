#pragma once

#include <iostream>
#include "SFML/Network.hpp"


class Client {
public:
    Client() = default;

    void connect(const std::string& server_ip, int server_port){
        sf::Socket::Status status = socket.connect(server_ip, server_port);

        if(status == sf::Socket::Status::Done){
            char buffer[1024];
            std::size_t received;
            auto receive_status = socket.receive(buffer, sizeof(buffer), received);

            if(receive_status == sf::Socket::Status::Done){
              std::cout << buffer << std::endl;
            }
        }
    }

    void sendCommand() { /* отправляет команды */ }

private:
    sf::TcpSocket socket{};
    bool connected = false;
};