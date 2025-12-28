#pragma once
#include <SFML/Network.hpp>

enum class Command : sf::Uint8 {
    SetValue,
    GetValue,
    DeleteValue
};

// for sending
sf::Packet& operator<<(sf::Packet& packet, Command cmd);

// for reception
sf::Packet& operator>>(sf::Packet& packet, Command& cmd);