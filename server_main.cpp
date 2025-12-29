#include "includes/Server.hpp"


int main() {
    Server server(53000);
    server.run();

    return 0;
}