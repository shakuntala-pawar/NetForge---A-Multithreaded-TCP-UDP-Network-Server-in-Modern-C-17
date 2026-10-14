
#include "ConnectionManager.hpp"

#include <iostream>
#include <sstream>
#include <string>

namespace
{
    int tests_passed = 0;
    int tests_failed = 0;

    void check(bool condition, const std::string& test_name)
    {
        if (condition)
        {
            ++tests_passed;
            std::cout << "[PASS] " << test_name << '\n';
        }
        else
        {
            ++tests_failed;
            std::cerr << "[FAIL] " << test_name << '\n';
        }
    }

    void testInitiallyEmpty()
    {
        ConnectionManager manager;

        check(
            manager.activeClientCount() == 0,
            "New manager starts with zero active clients"
        );

        check(
            !manager.isClientActive(1),
            "Unknown client is not active"
        );
    }

    void testAddClient()
    {
        ConnectionManager manager;

        manager.addClient(1, "127.0.0.1:50001");

        check(
            manager.isClientActive(1),
            "Added client is active"
        );

        check(
            manager.activeClientCount() == 1,
            "Adding one client increments active count"
        );
    }

    void testRemoveClient()
    {
        ConnectionManager manager;

        manager.addClient(1, "127.0.0.1:50001");
        manager.removeClient(1);

        check(
            !manager.isClientActive(1),
            "Removed client is inactive"
        );

        check(
            manager.activeClientCount() == 0,
            "Removing a client decrements active count"
        );
    }

    void testRemoveUnknownClient()
    {
        ConnectionManager manager;

        manager.removeClient(999);

        check(
            manager.activeClientCount() == 0,
            "Removing an unknown client leaves count unchanged"
        );
    }

    void testMultipleClients()
    {
        ConnectionManager manager;

        manager.addClient(3, "127.0.0.1:50003");
        manager.addClient(1, "127.0.0.1:50001");
        manager.addClient(2, "127.0.0.1:50002");

        check(
            manager.activeClientCount() == 3,
            "Multiple clients are counted correctly"
        );

        manager.removeClient(2);

        check(
            manager.activeClientCount() == 2,
            "Only active clients contribute to the count"
        );

        check(
            manager.isClientActive(1) &&
            !manager.isClientActive(2) &&
            manager.isClientActive(3),
            "Removing one client preserves other clients"
        );
    }

    void testDuplicateClientId()
    {
        ConnectionManager manager;

        manager.addClient(7, "127.0.0.1:50007");
        manager.addClient(7, "127.0.0.1:60007");

        check(
            manager.activeClientCount() == 1,
            "Adding a duplicate ID does not create another map entry"
        );

        const std::string output = manager.getActiveClients();

        check(
            output.find("127.0.0.1:60007") != std::string::npos,
            "Duplicate ID updates the stored client address"
        );
    }

    void testActiveClientList()
    {
        ConnectionManager manager;

        manager.addClient(3, "127.0.0.1:50003");
        manager.addClient(1, "127.0.0.1:50001");
        manager.addClient(2, "127.0.0.1:50002");
        manager.removeClient(2);

        const std::string output = manager.getActiveClients();

        const std::size_t client1 = output.find("Client #1");
        const std::size_t client3 = output.find("Client #3");

        check(
            client1 != std::string::npos &&
            client3 != std::string::npos,
            "Active-client list contains active clients"
        );

        check(
            output.find("Client #2") == std::string::npos,
            "Active-client list excludes removed clients"
        );

        check(
            client1 < client3,
            "Active-client list is sorted by client ID"
        );

        check(
            output.find("Active clients: 2") != std::string::npos,
            "Active-client list reports the correct count"
        );
    }
}

int main()
{
    std::cout << "\n===== ConnectionManager Unit Tests =====\n\n";

    testInitiallyEmpty();
    testAddClient();
    testRemoveClient();
    testRemoveUnknownClient();
    testMultipleClients();
    testDuplicateClientId();
    testActiveClientList();

    std::cout << "\n========================================\n";
    std::cout << "Tests passed: " << tests_passed << '\n';
    std::cout << "Tests failed: " << tests_failed << '\n';
    std::cout << "========================================\n";

    return tests_failed == 0 ? 0 : 1;
}
