#include "../includes/Protocol.hpp"

// conversion to known format
sf::Packet& operator<<(sf::Packet& packet, Command cmd) {
    return packet << static_cast<sf::Uint8>(cmd);
}

sf::Packet& operator>>(sf::Packet& packet, Command& cmd) {
    sf::Uint8 value;
    packet >> value;
    cmd = static_cast<Command>(value);
    return packet;
}