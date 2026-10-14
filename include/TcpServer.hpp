#pragma once

#include <atomic>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#include "ConnectionManager.hpp"
#include "ThreadPool.hpp"


class TcpServer
{
public:

    TcpServer(
        const std::string& host,
        unsigned short port
    );

    ~TcpServer();


    bool start();

    void run();

    void stop();


private:

    void handleClient(
        SOCKET client_socket,
        unsigned long client_id
    );


private:

    std::string host_;

    unsigned short port_;


    SOCKET server_socket_;


    std::atomic<bool> running_;


    // --------------------------------------------------------
    // Client ID generation
    // --------------------------------------------------------

    std::atomic<unsigned long> next_client_id_;


    // --------------------------------------------------------
    // Active client management
    // --------------------------------------------------------

    ConnectionManager connection_manager_;


    // --------------------------------------------------------
    // Worker Thread Pool
    // --------------------------------------------------------

    ThreadPool thread_pool_;
};