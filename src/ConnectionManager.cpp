#include "ConnectionManager.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>


void ConnectionManager::addClient(
    unsigned long client_id,
    const std::string& address
)
{
    std::lock_guard<std::mutex> lock(mutex_);

    ClientInfo client;

    client.client_id = client_id;
    client.address = address;
    client.active = true;

    clients_[client_id] = client;

    std::cout
        << "[Client #"
        << client_id
        << "] Added to connection manager.\n";
}


void ConnectionManager::removeClient(
    unsigned long client_id
)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto iterator = clients_.find(client_id);

    if (iterator != clients_.end())
    {
        iterator->second.active = false;

        std::cout
            << "[Client #"
            << client_id
            << "] Removed from active connections.\n";
    }
}


bool ConnectionManager::isClientActive(
    unsigned long client_id
)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto iterator = clients_.find(client_id);

    if (iterator == clients_.end())
    {
        return false;
    }

    return iterator->second.active;
}


std::size_t ConnectionManager::activeClientCount()
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::size_t count = 0;

    for (const auto& pair : clients_)
    {
        if (pair.second.active)
        {
            ++count;
        }
    }

    return count;
}


void ConnectionManager::printActiveClients()
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::cout
        << "\n========== ACTIVE CLIENTS ==========\n";

    std::vector<unsigned long> active_ids;

    for (const auto& pair : clients_)
    {
        if (pair.second.active)
        {
            active_ids.push_back(pair.first);
        }
    }

    std::sort(
        active_ids.begin(),
        active_ids.end()
    );

    for (unsigned long client_id : active_ids)
    {
        const ClientInfo& client =
            clients_.at(client_id);

        std::cout
            << "Client #"
            << client.client_id
            << " | Address: "
            << client.address
            << " | Status: ACTIVE\n";
    }

    std::cout
        << "Active clients: "
        << active_ids.size()
        << '\n';

    std::cout
        << "====================================\n\n";
}


std::string ConnectionManager::getActiveClients()
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::ostringstream output;

    output
        << "========== ACTIVE CLIENTS ==========\n";

    std::vector<unsigned long> active_ids;

    for (const auto& pair : clients_)
    {
        if (pair.second.active)
        {
            active_ids.push_back(pair.first);
        }
    }

    // Keep clients in numerical order:
    // Client #1, Client #2, Client #3, ...
    std::sort(
        active_ids.begin(),
        active_ids.end()
    );

    for (unsigned long client_id : active_ids)
    {
        const ClientInfo& client =
            clients_.at(client_id);

        output
            << "Client #"
            << client.client_id
            << " | Address: "
            << client.address
            << " | Status: ACTIVE\n";
    }

    output
        << "Active clients: "
        << active_ids.size()
        << '\n';

    output
        << "====================================";

    return output.str();
}