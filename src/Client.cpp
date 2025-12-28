#include "../includes/Client.hpp"


sf::Packet Client::form_set_value_packet(std::string& key, std::string& value){
  sf::Packet packet;
  packet << Command::SetValue << key << value;

  return packet;
}

sf::Packet Client::form_get_value_packet(std::string& key){
  sf::Packet packet;
  packet << Command::SetValue << key;

  return packet;
}

sf::Packet Client::form_del_value_packet(std::string& key){
  sf::Packet packet;
  packet << Command::SetValue << key;

  return packet;
}