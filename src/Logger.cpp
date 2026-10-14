#include "Logger.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>


std::mutex Logger::mutex_;


std::string Logger::timestamp()
{
    auto now =
        std::chrono::system_clock::now();

    std::time_t current_time =
        std::chrono::system_clock::to_time_t(now);

    std::tm local_time{};

#ifdef _WIN32

    localtime_s(
        &local_time,
        &current_time
    );

#else

    localtime_r(
        &current_time,
        &local_time
    );

#endif

    std::ostringstream output;

    output
        << std::put_time(
            &local_time,
            "%H:%M:%S"
        );

    return output.str();
}


void Logger::printSeparator()
{
    std::cout
        << "------------------------------------------------------------\n";
}


void Logger::info(
    const std::string& message
)
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::cout
        << "["
        << timestamp()
        << "] "
        << message
        << '\n';
}


void Logger::clientConnected(
    unsigned long client_id,
    const std::string& address,
    std::size_t active_clients
)
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::cout
        << "\n["
        << timestamp()
        << "] [CLIENT CONNECTED]\n";

    std::cout
        << "Client #"
        << client_id
        << '\n';

    std::cout
        << "Address       : "
        << address
        << '\n';

    std::cout
        << "Active Clients: "
        << active_clients
        << '\n';

    printSeparator();
}


void Logger::clientDisconnected(
    unsigned long client_id,
    std::size_t active_clients
)
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::cout
        << "\n["
        << timestamp()
        << "] [CLIENT DISCONNECTED]\n";

    std::cout
        << "Client #"
        << client_id
        << '\n';

    std::cout
        << "Active Clients: "
        << active_clients
        << '\n';

    printSeparator();
}


void Logger::request(
    unsigned long client_id,
    const std::string& command
)
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::cout
        << "\n["
        << timestamp()
        << "] [REQUEST]\n";

    std::cout
        << "Client #"
        << client_id
        << '\n';

    std::cout
        << "Command       : "
        << command
        << '\n';

    printSeparator();
}


void Logger::response(
    unsigned long client_id,
    const std::string& response
)
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::cout
        << "\n["
        << timestamp()
        << "] [RESPONSE]\n";

    std::cout
        << "Client #"
        << client_id
        << '\n';

    std::cout
        << "Response      : "
        << response
        << '\n';

    printSeparator();
}