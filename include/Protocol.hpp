#pragma once

#include <string>

class Protocol
{
public:
    static std::string processRequest(
        const std::string& request
    );

private:
    static std::string handlePing();
    static std::string handleStatus();
    static std::string handleTime(
        const std::string& request
    );
    static std::string handleEcho(
        const std::string& request
    );
};