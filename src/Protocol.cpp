#include "Protocol.hpp"

#include <chrono>
#include <ctime>
#include <sstream>

std::string Protocol::processRequest(
    const std::string& request
)
{
    if (request == "QUIT")
    {
        return "GOODBYE";
    }

    if (request == "PING")
    {
        return handlePing();
    }

    if (request == "STATUS")
    {
        return handleStatus();
    }

    if (request == "TIME")
    {
        return handleTime(request);
    }

    if (request.rfind("ECHO ", 0) == 0)
    {
        return handleEcho(request);
    }

    return "ERROR Unknown command";
}

std::string Protocol::handlePing()
{
    return "PONG";
}

std::string Protocol::handleStatus()
{
    return "SERVER_OK";
}

std::string Protocol::handleTime(
    const std::string&
)
{
    auto now = std::chrono::system_clock::now();

    std::time_t current_time =
        std::chrono::system_clock::to_time_t(now);

    std::string result = std::ctime(&current_time);

    if (!result.empty() && result.back() == '\n')
    {
        result.pop_back();
    }

    return "SERVER_TIME " + result;
}

std::string Protocol::handleEcho(
    const std::string& request
)
{
    return request.substr(5);
}

