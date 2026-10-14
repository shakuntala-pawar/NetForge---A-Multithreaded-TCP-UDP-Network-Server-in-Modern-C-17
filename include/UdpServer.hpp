
#pragma once

#include <atomic>
#include <cstdint>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#endif

class UdpServer
{
public:
    UdpServer(const std::string& host, unsigned short port);
    ~UdpServer();

    bool start();

    // The server checks this flag to support graceful shutdown.
    void run(const std::atomic<bool>& shutdown_requested);

    void stop();

private:
    std::string host_;
    unsigned short port_;

#ifdef _WIN32
    SOCKET socket_;
#else
    int socket_;
#endif

    std::atomic<bool> running_;

    // UDP monitoring statistics
    std::uint64_t datagrams_received_ = 0;
    std::uint64_t responses_sent_ = 0;
    std::uint64_t invalid_commands_ = 0;
    std::uint64_t socket_errors_ = 0;
};

