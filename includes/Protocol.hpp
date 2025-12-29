#pragma once
#include <SFML/Network.hpp>

enum class Command : sf::Uint8 {
    SetValue,
    GetValue,
    DelValue
};

// for sending
inline sf::Packet& operator<<(sf::Packet& packet, Command cmd) {
    return packet << static_cast<sf::Uint8>(cmd);
}

// for reception
inline sf::Packet& operator>>(sf::Packet& packet, Command& cmd) {
    sf::Uint8 value;
    packet >> value;
    cmd = static_cast<Command>(value);

    return packet;
}