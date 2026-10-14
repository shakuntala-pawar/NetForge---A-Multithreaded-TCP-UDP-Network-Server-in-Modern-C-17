#pragma once

#ifdef _WIN32
#include <winsock2.h>
#endif

#include <string>

class ConnectionManager;

class ClientSession
{
public:

    ClientSession(
        SOCKET client_socket,
        unsigned long client_id,
        ConnectionManager& connection_manager
    );

    ~ClientSession();

    void handle();

private:

    bool receiveMessage();

    void sendResponse(
        const std::string& response
    );

private:

    SOCKET client_socket_;

    unsigned long client_id_;

    ConnectionManager& connection_manager_;
};