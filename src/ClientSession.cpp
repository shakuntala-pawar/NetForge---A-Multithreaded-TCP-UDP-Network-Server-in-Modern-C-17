#include "ClientSession.hpp"
#include "ConnectionManager.hpp"
#include "Protocol.hpp"
#include "Logger.hpp"
#include <iostream>


ClientSession::ClientSession(
    SOCKET client_socket,
    unsigned long client_id,
    ConnectionManager& connection_manager
)
    : client_socket_(client_socket),
      client_id_(client_id),
      connection_manager_(connection_manager)
{
}
ClientSession::~ClientSession()
{
    if (client_socket_ != INVALID_SOCKET)
    {
        closesocket(client_socket_);

        client_socket_ = INVALID_SOCKET;
    }
}

void ClientSession::handle()
{
    

    while (true)
    {
        bool should_continue = receiveMessage();

        if (!should_continue)
        {
            break;
        }
    }

    
}

bool ClientSession::receiveMessage()
{
    char buffer[1024]{};

    int bytes_received = recv(
        client_socket_,
        buffer,
        sizeof(buffer) - 1,
        0
    );

    if (bytes_received <= 0)
    {
       

        return false;
    }

    buffer[bytes_received] = '\0';

    std::string request(buffer);

    Logger::request(
      client_id_,
      request
    );

    std::string response;

    if (request == "CLIENTS")
    {
        response =
            connection_manager_.getActiveClients();
    }
    else
   {
        response =
           Protocol::processRequest(request);
    }

    sendResponse(response);

    Logger::response(
      client_id_,
      response
    );

    if (request == "CLIENTS") 
   {
    std::cout
        << response
        << '\n';
    }
    else
   {
    std::cout
        << "Response      : "
        << response
        << '\n';
    }

    std::cout
      << "------------------------------------------------------------\n";

    if (request == "QUIT")
    {
        return false;
    }

    return true;
}

void ClientSession::sendResponse(
    const std::string& response
)
{
    send(
        client_socket_,
        response.c_str(),
        static_cast<int>(response.size()),
        0
    );
}