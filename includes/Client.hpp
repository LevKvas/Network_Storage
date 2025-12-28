#pragma once

#include <iostream>
#include "Protocol.hpp"
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

    // user interface
    bool set_value(std::string& key, std::string& value);
    std::string get_value(std::string& key);
    bool del_value(std::string& key);

    void sendCommand() { /* отправляет команды */ }

private:
    sf::TcpSocket socket{};
    bool connected = false;

    sf::Packet form_set_value_packet(std::string& key, std::string& value);
    sf::Packet form_get_value_packet(std::string& key);
    sf::Packet form_del_value_packet(std::string& key);
};