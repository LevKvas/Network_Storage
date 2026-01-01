#include <gtest/gtest.h>
#include <thread>

#include "../includes/Client.hpp"
#include "../includes/Server.hpp"

using namespace std;

class Network: public ::testing::Test {
public:
    // connection details
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

    ASSERT_TRUE(client.connect(server_ip, port));

    auto status_set = client.set_value("car", "12345");
    if (status_set) {
        EXPECT_EQ(client.get_value("car"), "12345");
    }
}

TEST_F(Network, OneClientDelNonExist) {
    Client client;

    ASSERT_TRUE(client.connect(server_ip, port));

    auto status_del = client.del_value("car");
    EXPECT_EQ(status_del, 0);
}


TEST_F(Network, OneClientDel) {
    Client client;

    ASSERT_TRUE(client.connect(server_ip, port));

    auto status_set = client.set_value("car", "12345");
    if (status_set) {
        auto status_del = client.del_value("car");

        if (status_del) {
            auto get_status = client.get_value("car");
            EXPECT_FALSE(get_status);
        }
    }
}

TEST_F(Network, OneClientGet) {
    Client client;

    ASSERT_TRUE(client.connect(server_ip, port));

    auto status_1 = client.set_value("car", "12345");
    auto status_2 = client.set_value("house", "6789");

    if (status_1 && status_2) {
        EXPECT_EQ(client.get_value("car"), "12345");
        EXPECT_EQ(client.get_value("house"), "6789");
    }
}

TEST_F(Network, SeveralClientsSet) {
    Client client_1;
    Client client_2;
    Client client_3;

    ASSERT_TRUE(client_1.connect(server_ip, port));
    ASSERT_TRUE(client_2.connect(server_ip, port));
    ASSERT_TRUE(client_3.connect(server_ip, port));

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

TEST_F(Network, SeveralClientsDel) {
    Client client_1;
    Client client_2;
    Client client_3;

    ASSERT_TRUE(client_1.connect(server_ip, port));
    ASSERT_TRUE(client_2.connect(server_ip, port));
    ASSERT_TRUE(client_3.connect(server_ip, port));

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
        auto get_1 = client_1.get_value("car");
        auto get_2 = client_1.get_value("house");
        auto get_3 = client_1.get_value("rode");

        EXPECT_FALSE(get_1);
        EXPECT_FALSE(get_2);
        EXPECT_FALSE(get_3);
    }
}

TEST_F(Network, SeveralClientGet) {
    Client client_1;
    Client client_2;
    Client client_3;

    ASSERT_TRUE(client_1.connect(server_ip, port));
    ASSERT_TRUE(client_2.connect(server_ip, port));
    ASSERT_TRUE(client_3.connect(server_ip, port));

    auto status_set_1 = client_1.set_value("car", "12345");
    auto status_set_2 = client_2.set_value("house", "45668");
    auto status_set_3 = client_3.set_value("rode", "54321");

    if (status_set_1 && status_set_2 && status_set_3) {
        // reading one key in diff threads
        thread get_client_1([&client_1]() {
            EXPECT_EQ(client_1.get_value("car"), "12345");
        });

        thread get_client_2([&client_2]() {
            EXPECT_EQ(client_2.get_value("car"), "12345");
        });

        thread get_client_3([&client_3]() {
            EXPECT_EQ(client_3.get_value("car"), "12345");
        });

        get_client_1.join();
        get_client_2.join();
        get_client_3.join();
    }
}

TEST_F(Network, EmptyKeyValue) {
    Client client;
    ASSERT_TRUE(client.connect(server_ip, port));

    ASSERT_TRUE(client.set_value("", "empty_value"));
    auto val = client.get_value("");
    ASSERT_TRUE(val);
    EXPECT_EQ(*val, "empty_value");

    ASSERT_TRUE(client.set_value("empty_val", ""));
    val = client.get_value("empty_val");
    ASSERT_TRUE(val);
    EXPECT_EQ(*val, "");
}

TEST_F(Network, OverwriteValue) {
    Client client;
    ASSERT_TRUE(client.connect(server_ip, port));

    ASSERT_TRUE(client.set_value("key", "old"));
    ASSERT_TRUE(client.set_value("key", "new"));

    auto val = client.get_value("key");
    ASSERT_TRUE(val);
    EXPECT_EQ(*val, "new");
}

TEST_F(Network, ManyOperations) {
    Client client;
    ASSERT_TRUE(client.connect(server_ip, port));

    const int N = 1000;
    for (int i = 0; i < N; ++i) {
        std::string key = "key_" + std::to_string(i);
        std::string val = "val_" + std::to_string(i);
        ASSERT_TRUE(client.set_value(key, val));
    }

    for (int i = 0; i < N; ++i) {
        std::string key = "key_" + std::to_string(i);
        auto val = client.get_value(key);
        ASSERT_TRUE(val);
        EXPECT_EQ(*val, "val_" + std::to_string(i));
    }
}

TEST_F(Network, MixedOperations) {
    Client client;
    ASSERT_TRUE(client.connect(server_ip, port));

    ASSERT_TRUE(client.set_value("a", "1"));
    ASSERT_TRUE(client.set_value("b", "2"));

    EXPECT_EQ(*client.get_value("a"), "1");
    EXPECT_EQ(*client.get_value("b"), "2");

    ASSERT_TRUE(client.del_value("a"));
    EXPECT_FALSE(client.get_value("a"));
    EXPECT_EQ(*client.get_value("b"), "2");
}

TEST_F(Network, SuddenShutdown) {
    {
        Client client;
        ASSERT_TRUE(client.connect(server_ip, port));
        ASSERT_TRUE(client.set_value("temp", "data"));
    } // disconnect

    // include a new client
    Client client_2;
    ASSERT_TRUE(client_2.connect(server_ip, port));
    EXPECT_EQ(client_2.get_value("temp"), "data");
}

TEST(ServerManualTest, IntentionalStop) {
    const int port = 53001;
    auto server = std::make_unique<Server>(port);
    std::thread server_thread([&]() { server->run(); });

    Client client_1;
    ASSERT_TRUE(client_1.connect("127.0.0.1", port));
    ASSERT_TRUE(client_1.set_value("test", "data"));

    // intentional stop
    server->stop();
    server_thread.join();

    // try to connect a new client
    Client client_2;
    EXPECT_FALSE(client_2.connect("127.0.0.1", port));

    // old client still works
    EXPECT_EQ(client_1.get_value("test"), "data");
}