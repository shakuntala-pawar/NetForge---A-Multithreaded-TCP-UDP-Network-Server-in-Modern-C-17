#pragma once

#include <mutex>
#include <string>


class Logger
{
public:

    static void info(
        const std::string& message
    );

    static void clientConnected(
        unsigned long client_id,
        const std::string& address,
        std::size_t active_clients
    );

    static void clientDisconnected(
        unsigned long client_id,
        std::size_t active_clients
    );

    static void request(
        unsigned long client_id,
        const std::string& command
    );

    static void response(
        unsigned long client_id,
        const std::string& response
    );

private:

    static std::mutex mutex_;

    static std::string timestamp();

    static void printSeparator();
};