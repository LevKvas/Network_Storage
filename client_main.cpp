#include <iostream>
#include "includes/Client.hpp"

using namespace std;

int main(){
  Client client;
  auto status = client.connect("192.168.3.46", 53000);

  if(status){
    auto status_set = client.set_value("car", "12345");
    cout << "Added was successfully" << endl;
  }

  try {
    auto get_inf = client.get_value("car");
    cout << "Got data is: " << get_inf << endl;
  }
  catch(const std::exception& e) {
    cerr << e.what() << endl;
  }

  return 0;
}