

#include "UdpServer.hpp"
#include "Protocol.hpp"

#include <iostream>
#include <string>

#ifdef _WIN32
#include <ws2tcpip.h>
#endif

UdpServer::UdpServer(
    const std::string& host,
    unsigned short port
)
    : host_(host),
      port_(port),
      socket_(INVALID_SOCKET),
      running_(false)
{
}

UdpServer::~UdpServer()
{
    stop();
}

bool UdpServer::start()
{
#ifdef _WIN32

    WSADATA wsa_data{};

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        std::cerr << "[UDP][ERROR] WSAStartup failed.\n";
        return false;
    }

    socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (socket_ == INVALID_SOCKET)
    {
        const int error_code = WSAGetLastError();

        std::cerr << "[UDP][ERROR] Socket creation failed. Code: "
                  << error_code << '\n';

        WSACleanup();
        return false;
    }

    sockaddr_in server_address{};

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_);

    if (inet_pton(
            AF_INET,
            host_.c_str(),
            &server_address.sin_addr
        ) != 1)
    {
        std::cerr << "[UDP][ERROR] Invalid host address: "
                  << host_ << '\n';

        closesocket(socket_);
        socket_ = INVALID_SOCKET;

        WSACleanup();
        return false;
    }

    if (bind(
            socket_,
            reinterpret_cast<sockaddr*>(&server_address),
            sizeof(server_address)
        ) == SOCKET_ERROR)
    {
        const int error_code = WSAGetLastError();

        std::cerr << "[UDP][ERROR] Bind failed. Code: "
                  << error_code << '\n';

        closesocket(socket_);
        socket_ = INVALID_SOCKET;

        WSACleanup();
        return false;
    }

    // Allow the receive loop to periodically check for Ctrl+C.
    DWORD timeout_ms = 500;

    if (setsockopt(
            socket_,
            SOL_SOCKET,
            SO_RCVTIMEO,
            reinterpret_cast<const char*>(&timeout_ms),
            sizeof(timeout_ms)
        ) == SOCKET_ERROR)
    {
        std::cerr << "[UDP][WARNING] Could not set receive timeout. Code: "
                  << WSAGetLastError() << '\n';
    }

    running_ = true;

    std::cout << "\n";
    std::cout << "============================================================\n";
    std::cout << "                    UDP SERVER\n";
    std::cout << "============================================================\n";
    std::cout << "Status        : RUNNING\n";
    std::cout << "Host          : " << host_ << '\n';
    std::cout << "Port          : " << port_ << '\n';
    std::cout << "Protocol      : UDP\n";
    std::cout << "============================================================\n";
    std::cout << "\n[UDP] Waiting for datagrams...\n";

    return true;

#else

    std::cerr << "[UDP] This implementation currently supports Windows only.\n";
    return false;

#endif
}

void UdpServer::run(
    const std::atomic<bool>& shutdown_requested
)
{
#ifdef _WIN32

    while (running_ && !shutdown_requested.load())
    {
        char buffer[4096]{};

        sockaddr_in client_address{};

        int client_address_length =
            sizeof(client_address);

        const int bytes_received = recvfrom(
            socket_,
            buffer,
            static_cast<int>(sizeof(buffer) - 1),
            0,
            reinterpret_cast<sockaddr*>(&client_address),
            &client_address_length
        );

        if (bytes_received == SOCKET_ERROR)
        {
            if (!running_ || shutdown_requested.load())
            {
                break;
            }

            const int error_code = WSAGetLastError();

            // A timeout is expected; it allows shutdown checks.
            if (error_code == WSAETIMEDOUT ||
                error_code == WSAEWOULDBLOCK)
            {
                continue;
            }

            ++socket_errors_;

            std::cerr << "\n[UDP][ERROR] recvfrom failed. Code: "
                      << error_code << '\n';

            continue;
        }

        ++datagrams_received_;

        buffer[bytes_received] = '\0';

        char client_ip[INET_ADDRSTRLEN] = "unknown";

        if (inet_ntop(
                AF_INET,
                &client_address.sin_addr,
                client_ip,
                sizeof(client_ip)
            ) == nullptr)
        {
            std::cerr << "[UDP][WARNING] Could not format client IP.\n";
        }

        const unsigned short client_port =
            ntohs(client_address.sin_port);

        const std::string request(buffer);

        std::cout << "\n[UDP] Datagram received\n";
        std::cout << "      Address : " << client_ip << '\n';
        std::cout << "      Port    : " << client_port << '\n';
        std::cout << "      Message : " << request << '\n';

        std::string response;

        // This monitoring command belongs to UDP only.
        // The shared Protocol class and TCP behavior stay unchanged.
        if (request == "UDP_STATS")
        {
            response =
                "UDP_STATS received=" +
                std::to_string(datagrams_received_) +
                " sent=" +
                std::to_string(responses_sent_) +
                " invalid=" +
                std::to_string(invalid_commands_) +
                " errors=" +
                std::to_string(socket_errors_);
        }
        else
        {
            response = Protocol::processRequest(request);

            if (response == "ERROR Unknown command")
            {
                ++invalid_commands_;
            }
        }

        std::cout << "      Response: " << response << '\n';

        const int bytes_sent = sendto(
            socket_,
            response.c_str(),
            static_cast<int>(response.size()),
            0,
            reinterpret_cast<sockaddr*>(&client_address),
            client_address_length
        );

        if (bytes_sent == SOCKET_ERROR)
        {
            ++socket_errors_;

            const int error_code = WSAGetLastError();

            std::cerr << "[UDP][ERROR] sendto failed. Code: "
                      << error_code << '\n';

            continue;
        }

        ++responses_sent_;
    }

#endif
}

void UdpServer::stop()
{
#ifdef _WIN32

    running_ = false;

    if (socket_ == INVALID_SOCKET)
    {
        return;
    }

    std::cout << "\n[UDP] Shutting down...\n";

    closesocket(socket_);
    socket_ = INVALID_SOCKET;

    std::cout << "[UDP] Socket closed.\n";

    std::cout << "\n";
    std::cout << "================ UDP STATISTICS ================\n";
    std::cout << "Datagrams received : " << datagrams_received_ << '\n';
    std::cout << "Responses sent     : " << responses_sent_ << '\n';
    std::cout << "Invalid commands   : " << invalid_commands_ << '\n';
    std::cout << "Socket errors      : " << socket_errors_ << '\n';
    std::cout << "=================================================\n";

    WSACleanup();

    std::cout << "[UDP] Server stopped.\n";

#endif
}
