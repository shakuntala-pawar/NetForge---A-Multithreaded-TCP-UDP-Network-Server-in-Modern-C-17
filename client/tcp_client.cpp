#include <iostream>
#include <string>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#endif


int main()
{
#ifdef _WIN32

    // ---------------------------------------------------------
    // Initialize Windows Sockets
    // ---------------------------------------------------------

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


    // ---------------------------------------------------------
    // Create TCP socket
    // ---------------------------------------------------------

    SOCKET client_socket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

    if (client_socket == INVALID_SOCKET)
    {
        std::cerr
            << "Socket creation failed.\n";

        WSACleanup();

        return 1;
    }


    // ---------------------------------------------------------
    // Configure server address
    // ---------------------------------------------------------

    sockaddr_in server_address{};

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);

    inet_pton(
        AF_INET,
        "127.0.0.1",
        &server_address.sin_addr
    );


    // ---------------------------------------------------------
    // Connect to server
    // ---------------------------------------------------------

    result = connect(
        client_socket,
        reinterpret_cast<sockaddr*>(&server_address),
        sizeof(server_address)
    );

    if (result == SOCKET_ERROR)
    {
        std::cerr
            << "Connection failed.\n";

        closesocket(client_socket);
        WSACleanup();

        return 1;
    }


    std::cout
        << "\n========================================\n";

    std::cout
        << "        C++ TCP NETWORK CLIENT\n";

    std::cout
        << "========================================\n";

    std::cout
        << "Connected to server.\n";

    std::cout
        << "Server: 127.0.0.1:8080\n";

    std::cout
        << "Type QUIT to disconnect.\n";

    std::cout
        << "========================================\n\n";


    // ---------------------------------------------------------
    // Persistent command loop
    // ---------------------------------------------------------

    while (true)
    {
        std::string command;

        std::cout
            << "Enter command: ";

        std::getline(
            std::cin,
            command
        );


        if (command.empty())
        {
            continue;
        }


        // -----------------------------------------------------
        // Send command
        // -----------------------------------------------------

        int bytes_sent = send(
            client_socket,
            command.c_str(),
            static_cast<int>(command.size()),
            0
        );

        if (bytes_sent == SOCKET_ERROR)
        {
            std::cerr
                << "Failed to send command.\n";

            break;
        }


        // -----------------------------------------------------
        // Receive server response
        // -----------------------------------------------------

        char buffer[4096]{};

        int bytes_received = recv(
            client_socket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        if (bytes_received <= 0)
        {
            std::cout
                << "\nServer disconnected.\n";

            break;
        }

        buffer[bytes_received] = '\0';


        std::cout
            << "\nServer response:\n";

        std::cout
            << buffer
            << "\n";


        // -----------------------------------------------------
        // QUIT
        // -----------------------------------------------------

        if (command == "QUIT")
        {
            break;
        }
    }


    // ---------------------------------------------------------
    // Cleanup
    // ---------------------------------------------------------

    closesocket(client_socket);

    WSACleanup();

    std::cout
        << "\nDisconnected from server.\n";

    return 0;

#else

    std::cerr
        << "This client currently supports Windows only.\n";

    return 1;

#endif
}