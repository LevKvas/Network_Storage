#pragma once

#include "SFML/Network.hpp"
#include <unordered_map>
#include <thread>
#include <chrono>
#include <mutex>


class Server {
public:
    explicit Server(int port_): port(port_) {}

    void run() const{
        sf::TcpListener listener; // one listener, socket

        std::cout << "Start running " << std::endl;

        if (listener.listen(port) != sf::Socket::Status::Done) {
            std::cout << "=== ERROR: Cannot listen on port " << port << "! ===" << std::endl;
            std::cout << "Is another program using this port?" << std::endl;
            return;
        }

        while(true){
            auto* socket_for_client = new sf::TcpSocket();
            // waiting for someone
            std::cout << "=== Waiting for client... ===" << std::endl;
            if(listener.accept(*socket_for_client) == sf::Socket::Status::Done) {
                std::cout << "=== CLIENT CONNECTED! ===" << std::endl;
                auto status = socket_for_client->send("Hello", 5);

                if (status == sf::Socket::Status::Done) {
                    std::cout << "Data sent successfully" << std::endl;
                } else if (status == sf::Socket::Status::Partial) {
                    std::cout << "Partially sent" << std::endl;
                } else if (status == sf::Socket::Status::NotReady) {
                    std::cout << "The socket is not ready to send." << std::endl;
                } else if (status == sf::Socket::Status::Disconnected) {
                    std::cout << "The connection was broken." << std::endl;
                } else if (status == sf::Socket::Status::Error) {
                    std::cout << "Sent error" << std::endl;
                }


                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                socket_for_client->disconnect();
                delete socket_for_client;
            }
            else {
                delete socket_for_client;
            }
        }
    }

    int get_port() const { return port; }

private:
    int port{};
    std::unordered_map<std::string, std::string> inf{};
    std::mutex mtx{};
};