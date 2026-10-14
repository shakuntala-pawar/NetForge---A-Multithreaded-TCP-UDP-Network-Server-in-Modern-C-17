#include "TcpServer.hpp"
#include "ClientSession.hpp"
#include "Logger.hpp"

#include <iostream>
#include <thread>

#ifdef _WIN32
#include <ws2tcpip.h>
#endif


// ============================================================
// Handle Individual Client
// ============================================================

void TcpServer::handleClient(
    SOCKET client_socket,
    unsigned long client_id
)
{
    ClientSession session(
        client_socket,
        client_id,
        connection_manager_
    );

    session.handle();
}


// ============================================================
// Constructor
// ============================================================

TcpServer::TcpServer(
    const std::string& host,
    unsigned short port
)
    : host_(host),
      port_(port),
      server_socket_(INVALID_SOCKET),
      running_(false),
      next_client_id_(1),
      thread_pool_(4)
{
}


// ============================================================
// Destructor
// ============================================================

TcpServer::~TcpServer()
{
    stop();
}


// ============================================================
// Start Server
// ============================================================

bool TcpServer::start()
{
#ifdef _WIN32

    // --------------------------------------------------------
    // Initialize Winsock
    // --------------------------------------------------------

    WSADATA wsa_data{};

    int result = WSAStartup(
        MAKEWORD(2, 2),
        &wsa_data
    );

    if (result != 0)
    {
        std::cerr
            << "WSAStartup failed: "
            << result
            << '\n';

        return false;
    }


    // --------------------------------------------------------
    // Create TCP socket
    // --------------------------------------------------------

    server_socket_ = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

    if (server_socket_ == INVALID_SOCKET)
    {
        std::cerr
            << "Socket creation failed.\n";

        WSACleanup();

        return false;
    }


    // --------------------------------------------------------
    // Configure server address
    // --------------------------------------------------------

    sockaddr_in server_address{};

    server_address.sin_family =
        AF_INET;

    server_address.sin_port =
        htons(port_);


    result = inet_pton(
        AF_INET,
        host_.c_str(),
        &server_address.sin_addr
    );

    if (result <= 0)
    {
        std::cerr
            << "Invalid server address.\n";

        closesocket(server_socket_);

        server_socket_ =
            INVALID_SOCKET;

        WSACleanup();

        return false;
    }


    // --------------------------------------------------------
    // Bind socket
    // --------------------------------------------------------

    result = bind(
        server_socket_,
        reinterpret_cast<sockaddr*>(
            &server_address
        ),
        sizeof(server_address)
    );

    if (result == SOCKET_ERROR)
    {
        std::cerr
            << "Bind failed.\n";

        closesocket(server_socket_);

        server_socket_ =
            INVALID_SOCKET;

        WSACleanup();

        return false;
    }


    // --------------------------------------------------------
    // Listen for clients
    // --------------------------------------------------------

    result = listen(
        server_socket_,
        SOMAXCONN
    );

    if (result == SOCKET_ERROR)
    {
        std::cerr
            << "Listen failed.\n";

        closesocket(server_socket_);

        server_socket_ =
            INVALID_SOCKET;

        WSACleanup();

        return false;
    }


    // --------------------------------------------------------
    // Server is running
    // --------------------------------------------------------

    running_ = true;


    std::cout
        << "\n"
        << "============================================================\n"
        << "                 C++ TCP NETWORK SERVER\n"
        << "============================================================\n"
        << "Status        : RUNNING\n"
        << "Host          : "
        << host_
        << '\n'
        << "Port          : "
        << port_
        << '\n'
        << "Protocol      : TCP\n"
        << "Client Mode   : MULTI-THREADED\n"
        << "============================================================\n"
        << '\n';


    std::cout
        << "Server is waiting for clients...\n";

    return true;

#else

    std::cerr
        << "Linux support will be added later.\n";

    return false;

#endif
}


// ============================================================
// Run Server
// ============================================================

void TcpServer::run()
{
#ifdef _WIN32

    if (!running_)
    {
        std::cerr
            << "Server is not running.\n";

        return;
    }


    while (running_)
    {
        // ----------------------------------------------------
        // Wait for client
        // ----------------------------------------------------

        sockaddr_in client_address{};

        int client_address_length =
            sizeof(client_address);


        SOCKET client_socket =
            accept(
                server_socket_,
                reinterpret_cast<sockaddr*>(
                    &client_address
                ),
                &client_address_length
            );


        // ----------------------------------------------------
        // Handle accept failure
        // ----------------------------------------------------

        if (client_socket == INVALID_SOCKET)
        {
            if (!running_)
            {
                break;
            }

            std::cerr
                << "Accept failed.\n";

            continue;
        }


        // ----------------------------------------------------
        // Generate client ID
        // ----------------------------------------------------

        unsigned long client_id =
            next_client_id_++;


        // ----------------------------------------------------
        // Get client IP address
        // ----------------------------------------------------

        char client_ip[INET_ADDRSTRLEN]{};

        inet_ntop(
            AF_INET,
            &client_address.sin_addr,
            client_ip,
            INET_ADDRSTRLEN
        );


        std::string client_address_string =
            client_ip;


        // ----------------------------------------------------
        // Register client
        // ----------------------------------------------------

        connection_manager_.addClient(
            client_id,
            client_address_string
        );


        // ----------------------------------------------------
        // Log client connection
        // ----------------------------------------------------

        Logger::clientConnected(
            client_id,
            client_address_string,
            connection_manager_.activeClientCount()
        );


        // ----------------------------------------------------
        // Create client thread
        // ----------------------------------------------------

        thread_pool_.enqueue(
        [this, client_socket, client_id]()
         {
            handleClient(
              client_socket,
              client_id
            );
         }
        );


        // ----------------------------------------------------
        // Detach thread
        // ----------------------------------------------------

        
    }

#else

    std::cerr
        << "Linux support will be added later.\n";

#endif
}


// ============================================================
// Stop Server
// ============================================================

void TcpServer::stop()
{
#ifdef _WIN32

    if (running_)
    {
        running_ = false;
    }

    thread_pool_.shutdown();

    // --------------------------------------------------------
    // Close server socket
    // --------------------------------------------------------

    if (server_socket_ != INVALID_SOCKET)
    {
        closesocket(
            server_socket_
        );

        server_socket_ =
            INVALID_SOCKET;
    }


    // --------------------------------------------------------
    // Cleanup Winsock
    // --------------------------------------------------------

    WSACleanup();

#endif
}