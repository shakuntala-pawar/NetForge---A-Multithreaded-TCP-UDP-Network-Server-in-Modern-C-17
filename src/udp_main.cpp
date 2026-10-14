
#include "UdpServer.hpp"

#include <atomic>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace
{
    std::atomic<bool> shutdown_requested{false};

#ifdef _WIN32
    BOOL WINAPI consoleControlHandler(DWORD control_type)
    {
        switch (control_type)
        {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
            shutdown_requested.store(true);
            return TRUE;

        default:
            return FALSE;
        }
    }
#endif
}

int main()
{
#ifdef _WIN32
    if (!SetConsoleCtrlHandler(consoleControlHandler, TRUE))
    {
        std::cerr << "[UDP][ERROR] Could not register console handler.\n";
        return 1;
    }
#endif

    UdpServer server("127.0.0.1", 9090);

    if (!server.start())
    {
#ifdef _WIN32
        SetConsoleCtrlHandler(consoleControlHandler, FALSE);
#endif
        std::cerr << "[UDP][ERROR] Failed to start server.\n";
        return 1;
    }

    server.run(shutdown_requested);

    // Called after the receive loop exits, including on Ctrl+C.
    server.stop();

#ifdef _WIN32
    SetConsoleCtrlHandler(consoleControlHandler, FALSE);
#endif

    return 0;
}
