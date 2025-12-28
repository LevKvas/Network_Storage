#pragma once

#include <unordered_map>
#include <chrono>
#include <future>
#include <mutex>

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

class Server {
public:
    explicit Server(int port_): port(port_) {}

    void run() const{
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
                auto future = std::async(std::launch::async, handle_client, this, std::ref(*socket_for_client));

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

    void handle_client(sf::TcpSocket& client) const{
        sf::Packet packet;

        // get inf from client
        if (client.receive(packet) == sf::Socket::Done) {
            Command cmd;
            if (!(packet >> cmd)) {
                throw ErrorCommand();
            }

        }
    }

};