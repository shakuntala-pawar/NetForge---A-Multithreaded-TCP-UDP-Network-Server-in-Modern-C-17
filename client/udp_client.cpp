#include <iostream>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif


int main()
{
#ifdef _WIN32

    WSADATA wsa_data{};

    int result = WSAStartup(
        MAKEWORD(2, 2),
        &wsa_data
    );

    if (result != 0)
    {
        std::cerr
            << "WSAStartup failed.\n";

        return 1;
    }


    SOCKET socket_fd =
        socket(
            AF_INET,
            SOCK_DGRAM,
            IPPROTO_UDP
        );


    if (socket_fd == INVALID_SOCKET)
    {
        std::cerr
            << "UDP socket creation failed.\n";

        WSACleanup();

        return 1;
    }


    sockaddr_in server_address{};

    server_address.sin_family =
        AF_INET;

    server_address.sin_port =
        htons(9090);


    inet_pton(
        AF_INET,
        "127.0.0.1",
        &server_address.sin_addr
    );


    std::cout
        << "Connected to UDP server.\n";

    std::cout
        << "Enter message: ";


    std::string message;

    std::getline(
        std::cin,
        message
    );


    sendto(
        socket_fd,
        message.c_str(),
        static_cast<int>(
            message.size()
        ),
        0,
        reinterpret_cast<sockaddr*>(
            &server_address
        ),
        sizeof(server_address)
    );


    char buffer[4096]{};

    sockaddr_in response_address{};

    int response_address_length =
        sizeof(response_address);


    int bytes_received =
        recvfrom(
            socket_fd,
            buffer,
            sizeof(buffer) - 1,
            0,
            reinterpret_cast<sockaddr*>(
                &response_address
            ),
            &response_address_length
        );


    if (bytes_received > 0)
    {
        buffer[bytes_received] = '\0';

        std::cout
            << "Server response: "
            << buffer
            << '\n';
    }


    closesocket(socket_fd);

    WSACleanup();

    return 0;

#else

    std::cerr
        << "This client currently supports Windows only.\n";

    return 1;

#endif
}