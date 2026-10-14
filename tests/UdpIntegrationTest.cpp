
#include <winsock2.h>
#include <ws2tcpip.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

namespace
{
    int tests_passed = 0;
    int tests_failed = 0;

    bool sendAndCheck(
        SOCKET client_socket,
        const sockaddr_in& server_address,
        const std::string& request,
        const std::string& expected,
        bool check_prefix = false
    )
    {
        const int sent = sendto(
            client_socket,
            request.c_str(),
            static_cast<int>(request.size()),
            0,
            reinterpret_cast<const sockaddr*>(&server_address),
            sizeof(server_address)
        );

        if (sent == SOCKET_ERROR)
        {
            std::cout << "[FAIL] " << request
                      << " - sendto error: "
                      << WSAGetLastError() << '\n';
            ++tests_failed;
            return false;
        }

        char buffer[4096]{};

        sockaddr_in response_address{};
        int address_length = sizeof(response_address);

        const int received = recvfrom(
            client_socket,
            buffer,
            static_cast<int>(sizeof(buffer) - 1),
            0,
            reinterpret_cast<sockaddr*>(&response_address),
            &address_length
        );

        if (received == SOCKET_ERROR)
        {
            std::cout << "[FAIL] " << request
                      << " - receive error: "
                      << WSAGetLastError() << '\n';
            ++tests_failed;
            return false;
        }

        buffer[received] = '\0';

        const std::string response(buffer);

        const bool matches = check_prefix
            ? response.rfind(expected, 0) == 0
            : response == expected;

        if (matches)
        {
            std::cout << "[PASS] " << request
                      << " -> " << response << '\n';
            ++tests_passed;
            return true;
        }

        std::cout << "[FAIL] " << request
                  << "\n       Expected: " << expected
                  << "\n       Actual  : " << response << '\n';

        ++tests_failed;
        return false;
    }
}

int main()
{
    WSADATA wsa_data{};

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        std::cerr << "WSAStartup failed.\n";
        return EXIT_FAILURE;
    }

    SOCKET client_socket = socket(
        AF_INET,
        SOCK_DGRAM,
        IPPROTO_UDP
    );

    if (client_socket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed: "
                  << WSAGetLastError() << '\n';
        WSACleanup();
        return EXIT_FAILURE;
    }

    DWORD timeout_ms = 3000;

    if (setsockopt(
            client_socket,
            SOL_SOCKET,
            SO_RCVTIMEO,
            reinterpret_cast<const char*>(&timeout_ms),
            sizeof(timeout_ms)
        ) == SOCKET_ERROR)
    {
        std::cerr << "Could not configure receive timeout.\n";
        closesocket(client_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    sockaddr_in server_address{};

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9090);

    if (inet_pton(
            AF_INET,
            "127.0.0.1",
            &server_address.sin_addr
        ) != 1)
    {
        std::cerr << "Invalid server address.\n";
        closesocket(client_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    std::cout << "\n========== UDP INTEGRATION TESTS ==========\n";
    std::cout << "Target: 127.0.0.1:9090\n\n";

    sendAndCheck(
        client_socket, server_address,
        "PING", "PONG"
    );

    sendAndCheck(
        client_socket, server_address,
        "STATUS", "SERVER_OK"
    );

    sendAndCheck(
        client_socket, server_address,
        "ECHO Integration Test", "Integration Test"
    );

    sendAndCheck(
        client_socket, server_address,
        "UNKNOWN", "ERROR Unknown command"
    );

    sendAndCheck(
        client_socket, server_address,
        "TIME", "SERVER_TIME ", true
    );

    sendAndCheck(
        client_socket, server_address,
        "UDP_STATS", "UDP_STATS received=", true
    );

    std::cout << "\n===========================================\n";
    std::cout << "Passed: " << tests_passed << '\n';
    std::cout << "Failed: " << tests_failed << '\n';

    closesocket(client_socket);
    WSACleanup();

    return tests_failed == 0
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}
