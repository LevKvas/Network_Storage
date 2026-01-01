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

        server_thread = std::thread([this]() {
            server->run();
        });
    }

    void TearDown() override {
        server->stop();
        server_thread.join();
    }

private:
    unique_ptr<Server> server;
    std::thread server_thread;
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

TEST_F(Network, OneClientDelNonExist) {
    Client client;

    auto status = client.connect(server_ip, port);

    if (status) {
        auto status_del = client.del_value("car");
        EXPECT_EQ(status_del, 0);
    }
}


TEST_F(Network, OneClientDel) {
    Client client;

    auto status = client.connect(server_ip, port);
    if (status) {
        auto status_set = client.set_value("car", "12345");
        if (status_set) {
            auto status_del = client.del_value("car");

            if (status_del) {
                EXPECT_THROW(client.get_value("car"), KeyNotFound);
            }
        }
    }
}

TEST_F(Network, OneClientGet) {
    Client client;

    auto status = client.connect(server_ip, port);
    if (status) {
        auto status_1 = client.set_value("car", "12345");
        auto status_2 = client.set_value("house", "6789");

        if (status_1 && status_2) {
            auto inf_1 = client.get_value("car");
            auto inf_2 = client.get_value("house");

            EXPECT_EQ(inf_1, "12345");
            EXPECT_EQ(inf_2, "6789");
        }
    }
}

TEST_F(Network, SeveralClientsSet) {
    Client client_1;
    Client client_2;
    Client client_3;

    // connect to one server
    auto status_1 = client_1.connect(server_ip, port);
    auto status_2 = client_2.connect(server_ip, port);
    auto status_3 = client_3.connect(server_ip, port);

    if (status_1 && status_2 && status_3) {
        auto status_set_1 = client_1.set_value("car", "12345");
        auto status_set_2 = client_2.set_value("house", "45668");
        auto status_set_3 = client_3.set_value("rode", "54321");

        // read all clients
        if (status_set_1 && status_set_2 && status_set_3) {
            EXPECT_EQ(client_1.get_value("car"), "12345");
            EXPECT_EQ(client_2.get_value("house"), "45668");
            EXPECT_EQ(client_3.get_value("rode"), "54321");
        }

        // read one client
        if (status_set_1 && status_set_2 && status_set_3) {
            EXPECT_EQ(client_1.get_value("car"), "12345");
            EXPECT_EQ(client_1.get_value("house"), "45668");
            EXPECT_EQ(client_1.get_value("rode"), "54321");
        }
    }
}

TEST_F(Network, SeveralClientsDel) {
    Client client_1;
    Client client_2;
    Client client_3;

    // connect to one server
    auto status_1 = client_1.connect(server_ip, port);
    auto status_2 = client_2.connect(server_ip, port);
    auto status_3 = client_3.connect(server_ip, port);

    if (status_1 && status_2 && status_3) {
        auto status_set_1 = client_1.set_value("car", "12345");
        auto status_set_2 = client_2.set_value("house", "45668");
        auto status_set_3 = client_3.set_value("rode", "54321");

        if (status_set_1 && status_set_2 && status_set_3) {
            // check multithreading
            thread del_client_1([&client_1]() {
                auto status_del = client_1.del_value("car");
                EXPECT_EQ(status_del, 1);
            });

            thread del_client_2([&client_2]() {
                auto status_del = client_2.del_value("house");
                EXPECT_EQ(status_del, 1);
            });

            thread del_client_3([&client_3]() {
                auto status_del = client_3.del_value("rode");
                EXPECT_EQ(status_del, 1);
            });

            del_client_1.join();
            del_client_2.join();
            del_client_3.join();

            // read one client
            EXPECT_THROW(client_1.get_value("car"), KeyNotFound);
            EXPECT_THROW(client_1.get_value("house"), KeyNotFound);
            EXPECT_THROW(client_1.get_value("house"), KeyNotFound);
        }
    }
}