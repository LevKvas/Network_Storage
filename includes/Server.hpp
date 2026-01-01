#pragma once

#include <unordered_map>
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
        running.store(true);
        sf::TcpListener listener; // one listener, socket

        std::cout << "Start running " << std::endl;

        if (listener.listen(port) != sf::Socket::Status::Done) {
            std::cerr << "Cannot listen on port " << port << std::endl;
            return;
        }

        while(running){
            auto socket_for_client = std::make_unique<sf::TcpSocket>();

            std::cout << "=== Waiting for client... ===" << std::endl;
            // if new client has connected
            if(listener.accept(*socket_for_client) == sf::Socket::Status::Done) {
                if (!running){break;}
                std::cout << "=== Client Connected ===" << std::endl;

                // work with this client in another thread
                std::thread client_thread(&Server::handle_client, this, std::move(socket_for_client));
                client_thread.detach();
            }
        }
    }

    int get_port() const { return port; }

    void stop() {
        running.store(false);
        // connect to yourself
        try {
            sf::TcpSocket dummy;
            dummy.connect("192.168.3.46", port, sf::seconds(1));
            // if the server is no longer listening, terminate after 1 second.
            dummy.disconnect();
        }
        catch (...) {}
    }

private:
    int port{};
    std::atomic<bool> running{};

    std::unordered_map<std::string, std::string> data{};
    std::mutex mtx{};

    void handle_client(std::unique_ptr<sf::TcpSocket> client){
        // get inf from client
        while (true) {
            sf::Packet packet;

            if (client->receive(packet) != sf::Socket::Done) {
                break;
            }

            Command cmd;
            std::string key, value{};

            // must check command
            if (!(packet >> cmd)) {
                std::cerr << "Missing command byte" << std::endl;

                sf::Packet response;
                response << static_cast<sf::Uint8>(0);
                client->send(response);
                return;
            }

            // check key
            if (!(packet >> key)) {
                sf::Packet err;

                err << sf::Uint8(0);
                client->send(err);
                return;
            }

            if (cmd == Command::SetValue) {
                // check value
                if (!(packet >> value)) {
                    sf::Packet err;
                    err << sf::Uint8(0);
                    client->send(err);
                    return;
                }

                std::lock_guard lock(mtx);

                bool set_flag = false;

                try {
                    data[key] = value;
                    set_flag = true;
                }
                catch (...) {set_flag = false;}

                sf::Packet response;

                response << static_cast<sf::Uint8>(set_flag ? 1 : 0);
                client->send(response);
            }

            else if (cmd == Command::GetValue) {
                // get value to client
                sf::Packet response{};

                std::lock_guard lock(mtx);

                auto it = data.find(key);

                if (it != data.end()) {
                    response << static_cast<sf::Uint8>(1); // status
                    response << it->second;
                } else {
                    response << static_cast<sf::Uint8>(0);
                }

                client->send(response);
            }
            else if (cmd == Command::DelValue) {
                std::lock_guard lock(mtx);
                auto num_del = data.erase(key);

                sf::Packet response;
                response << static_cast<sf::Uint8>(num_del == 1 ? 1 : 0);
                client->send(response);
            }
            else { // will almost never happen
                std::cerr << "Received unknown command: " << static_cast<int>(cmd) << std::endl;
                sf::Packet response;
                response << static_cast<sf::Uint8>(0);
                client->send(response);
            }
        }
    }
};