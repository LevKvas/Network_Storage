#pragma once

#include <unordered_map>
#include <chrono>
#include <future>
#include <mutex>
#include <iostream>

#include "SFML/Network.hpp"
#include "Protocol.hpp"


class ErrorListen final: public std::exception{
public:
    const char* what() const noexcept override{return "Cannot listen on this port";}
};

class ErrorCommand final: public std::exception{
public:
    const char* what() const noexcept override{return "Failed to read command";}
};

class DataErrorSend final: public std::exception{
public:
    const char* what() const noexcept override{return "Failed to send data";}
};

class UnknownCommand final: public std::exception {
public:
    const char* what() const noexcept override{return "This type of command is not known.";}
};

class KeyNotExist final: public std::exception{
public:
    const char* what() const noexcept override{return "Such a key does not exist";}
};


class Server {
public:
    explicit Server(int port_): port(port_) {}

    void run() {
        sf::TcpListener listener; // one listener, socket

        std::cout << "Start running " << std::endl;

        if (listener.listen(port) != sf::Socket::Status::Done) {
            throw ErrorListen();
        }

        while(true){
            auto* socket_for_client = new sf::TcpSocket();
            // waiting for someone
            std::cout << "=== Waiting for client... ===" << std::endl;
            if(listener.accept(*socket_for_client) == sf::Socket::Status::Done) {
                std::cout << "=== CLIENT CONNECTED! ===" << std::endl;

                // work with this client in another thread
                auto future = std::async(std::launch::async, &Server::handle_client, this,
                    std::ref(*socket_for_client));

            }
            else {
                delete socket_for_client;
            }
        }
    }

    int get_port() const { return port; }

private:
    int port{};
    std::unordered_map<std::string, std::string> data{};
    std::mutex mtx{};

    void handle_client(sf::TcpSocket& client){
        // get inf from client
        while (true) {
            sf::Packet packet;

            if (client.receive(packet) != sf::Socket::Done) {
                break;
            }

            Command cmd;
            std::string key, value{};

            // must check command
            if (!(packet >> cmd)) {
                std::cerr << "Missing command byte" << std::endl;

                sf::Packet response;
                response << static_cast<sf::Uint8>(0);
                client.send(response);
                return;
            }

            if (cmd == Command::SetValue) {
                std::lock_guard lock(mtx);

                bool set_flag = false;

                try {
                    data[key] = value;
                    set_flag = true;
                }
                catch (...) {set_flag = false;}

                packet << static_cast<sf::Uint8>(set_flag ? 1 : 0);
                client.send(packet);
            }

            else if (cmd == Command::GetValue) {
                // get value to client
                sf::Packet response{};

                auto it = data.find(key);

                if (it != data.end()) {
                    response << static_cast<sf::Uint8>(1); // status
                    response << it->second;
                } else {
                    response << static_cast<sf::Uint8>(0);
                }

                client.send(response) != sf::Socket::Done;
            }
            else if (cmd == Command::DelValue) {
                std::lock_guard lock(mtx);
                auto num_del = data.erase(key);

                packet << static_cast<sf::Uint8>(num_del == 1 ? 1 : 0);
                client.send(packet);
            }
            else { // will almost never happen
                std::cerr << "Received unknown command: " << static_cast<int>(cmd) << std::endl;
                sf::Packet response;
                response << static_cast<sf::Uint8>(0);
                client.send(response);
            }
        }
    }

};