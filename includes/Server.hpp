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

class ErrorSending final: public std::exception{
public:
    const char* what() const noexcept override{return "Failed to send data";}
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
                auto future = std::async(std::launch::async, &Server::handle_client, this, std::ref(*socket_for_client));

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
        sf::Packet packet;

        // get inf from client
        if (client.receive(packet) == sf::Socket::Done) {
            Command cmd;
            std::string key, value{};

            // must contain command and key
            if (!(packet >> cmd) || !(packet >> key)) {
                throw ErrorCommand();
            }

            if (cmd == Command::SetValue) {
                if (!(packet >> value)) {throw ErrorCommand();}
                std::lock_guard lock(mtx);
                data[key] = value;
            }

            if (cmd == Command::GetValue) {
                std::lock_guard lock(mtx);

                // get value to client
                sf::Packet val_packet{};
                val_packet << data[key];
                if (!client.send(val_packet) == sf::Socket::Done) {
                    throw ErrorSending();
                }
            }
        }
    }

};