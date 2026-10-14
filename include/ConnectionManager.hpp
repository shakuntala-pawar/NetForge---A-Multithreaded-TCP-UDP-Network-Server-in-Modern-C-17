#pragma once

#include <mutex>
#include <string>
#include <unordered_map>

struct ClientInfo
{
    unsigned long client_id;

    std::string address;

    bool active;
};


class ConnectionManager
{
public:

    void addClient(
        unsigned long client_id,
        const std::string& address
    );

    void removeClient(
        unsigned long client_id
    );

    bool isClientActive(
        unsigned long client_id
    );

    std::size_t activeClientCount();

    void printActiveClients();

    std::string getActiveClients();

private:

    std::unordered_map<
        unsigned long,
        ClientInfo
    > clients_;

    std::mutex mutex_;
};