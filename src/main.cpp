#include "TcpServer.hpp"

#include <iostream>

int main()
{
    TcpServer server("127.0.0.1", 8080);

    if (!server.start())
    {
        std::cerr << "Failed to start server.\n";

        return 1;
    }

    server.run();

    return 0;
}