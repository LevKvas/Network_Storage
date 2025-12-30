#pragma once

#include <iostream>
#include "Protocol.hpp"
#include "SFML/Network.hpp"

class PacketErrorSend final: public std::exception{
public:
    const char* what() const noexcept override{return "Cannot sent this packet";}
};

class ErrorReceiving final: public std::exception{
public:
    const char* what() const noexcept override{return "Cannot receive this packet";}
};

class KeyNotFound final: public std::exception{
public:
    const char* what() const noexcept override{return "Key not found";}
};


class Client {
public:
    Client() = default;

    bool connect(const std::string& server_ip_, int server_port_){
        sf::Socket::Status status = socket.connect(server_ip_, server_port_);

        if(status == sf::Socket::Status::Done){
          	server_ip = server_ip_;
            server_port = server_port_;
            connected = true;
        }

        return connected;
    }

    // user interface
    bool set_value(const std::string& key, const std::string& value);
    std::string get_value(const std::string& key);
    bool del_value(const std::string& key);

private:
    sf::TcpSocket socket{};
    bool connected = false;

    std::string server_ip{};
    int server_port{};

    sf::Packet static form_set_value_packet(const std::string& key, const std::string& value);
    sf::Packet static form_get_value_packet(const std::string& key);
    sf::Packet static form_del_value_packet(const std::string& key);
};