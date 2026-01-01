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

  auto get_inf = client.get_value("car");
  cout << "Got data is: " << *get_inf << endl;

  auto status_deletion = client.del_value("car");
  cout << status_deletion << endl;

  try {
    auto get_inf_2 = client.get_value("car");
    cout << *get_inf_2 << endl;
  }
  catch(const std::exception& e) {
    cout << e.what() << endl;
  }

  return 0;
}