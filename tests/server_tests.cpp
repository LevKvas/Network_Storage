#include <gtest/gtest.h>
#include <thread>

#include "../includes/Client.hpp"
#include "../includes/Server.hpp"

using namespace std;

class Network: public ::testing::Test {
public:
    int port = 53000;
    string server_ip = "192.168.3.46";

    void SetUp() override {
        server = std::make_unique<Server>(port);

        thread server_thread([this]() {
            server->run();
        });
        server_thread.detach();
    }

private:
    unique_ptr<Server> server;
};


TEST_F(Network, OneClientSet) {
    Client client;

    auto status = client.connect(server_ip, port);

    if (status) {
        auto status_set = client.set_value("car", "12345");
        if (status_set) {
            auto get_inf = client.get_value("car");
            EXPECT_EQ(get_inf, "12345");
        }
    }
}
