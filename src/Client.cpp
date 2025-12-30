#include "../includes/Client.hpp"
#include <cstdint>


sf::Packet Client::form_set_value_packet(const std::string& key, const std::string& value){
  sf::Packet packet;
  packet << Command::SetValue << key << value;

  return packet;
}

sf::Packet Client::form_get_value_packet(const std::string& key){
  sf::Packet packet;
  packet << Command::GetValue << key;

  return packet;
}

sf::Packet Client::form_del_value_packet(const std::string& key){
  sf::Packet packet;
  packet << Command::DelValue << key;

  return packet;
}

/*
  key, value -> bool, status execution
 */
bool Client::set_value(const std::string& key, const std::string& value){
  // form packet
  auto packet = form_set_value_packet(key, value);
  // send packet to server

  if (socket.send(packet) != sf::Socket::Done) {
    return false;
  }

  return true;
}

/*
  key -> std::string - value
 */
std::string Client::get_value(const std::string& key){
  // form packet
  auto packet = form_get_value_packet(key);

  // send packet to server
  if (socket.send(packet) != sf::Socket::Done) {
    throw PacketErrorSend();
  }

  // get answer from server
  sf::Packet response;
  std::string response_str{};

  if (socket.receive(response) != sf::Socket::Done) {
    throw ErrorReceiving();
  }

  response >> response_str;

  return response_str;
}

/*
 * key -> bool, true - if the val was deleted, false - if not.
 *
 */
bool Client::del_value(const std::string &key) {
  auto packet = form_del_value_packet(key);

  // send packet to server
  if (socket.send(packet) != sf::Socket::Done) {
    throw PacketErrorSend();
  }

  sf::Packet status_deletion;

  if (socket.receive(status_deletion) != sf::Socket::Done) {
    throw ErrorReceiving();
  }

  sf::Uint8 status;
  packet >> status;
  bool success = status != 0;

  return success;
}