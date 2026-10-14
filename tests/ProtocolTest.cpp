
#include "Protocol.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

namespace
{
    int tests_passed = 0;
    int tests_failed = 0;

    void check(
        const std::string& test_name,
        const std::string& actual,
        const std::string& expected
    )
    {
        if (actual == expected)
        {
            ++tests_passed;
            std::cout << "[PASS] " << test_name << '\n';
        }
        else
        {
            ++tests_failed;
            std::cout << "[FAIL] " << test_name
                      << "\n       Expected: " << expected
                      << "\n       Actual  : " << actual << '\n';
        }
    }

    void checkTimeResponse()
    {
        const std::string response = Protocol::processRequest("TIME");

        if (response.rfind("SERVER_TIME ", 0) == 0)
        {
            ++tests_passed;
            std::cout << "[PASS] TIME returns server time\n";
        }
        else
        {
            ++tests_failed;
            std::cout << "[FAIL] TIME response format\n"
                      << "       Actual: " << response << '\n';
        }
    }
}

int main()
{
    std::cout << "\n========== PROTOCOL UNIT TESTS ==========\n";

    check("PING command",
          Protocol::processRequest("PING"),
          "PONG");

    check("STATUS command",
          Protocol::processRequest("STATUS"),
          "SERVER_OK");

    check("ECHO command",
          Protocol::processRequest("ECHO Hello"),
          "Hello");

    check("QUIT command",
          Protocol::processRequest("QUIT"),
          "GOODBYE");

    check("Unknown command",
          Protocol::processRequest("UNKNOWN"),
          "ERROR Unknown command");

    check("Empty command",
          Protocol::processRequest(""),
          "ERROR Unknown command");

    checkTimeResponse();

    std::cout << "=========================================\n";
    std::cout << "Passed: " << tests_passed << '\n';
    std::cout << "Failed: " << tests_failed << '\n';

    return tests_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
