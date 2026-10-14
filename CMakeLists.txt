cmake_minimum_required(VERSION 3.20)

project(CppNetworkServer
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ============================================================
# TCP + Thread Pool Server
# ============================================================

add_executable(server

    src/main.cpp

    src/TcpServer.cpp

    src/ClientSession.cpp

    src/Protocol.cpp

    src/ConnectionManager.cpp

    src/Logger.cpp

    src/ThreadPool.cpp

   
)


target_include_directories(server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)



# ============================================================
# TCP Client
# ============================================================

add_executable(tcp_client

    client/tcp_client.cpp
)


target_include_directories(tcp_client
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)


# ============================================================
# UDP Server
# ============================================================

add_executable(udp_server

    src/UdpServer.cpp

    src/udp_main.cpp

    src/Protocol.cpp

)

add_executable(protocol_test
    tests/ProtocolTest.cpp
    src/Protocol.cpp
)

target_include_directories(protocol_test PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)


target_include_directories(udp_server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

if(WIN32)
    target_link_libraries(udp_server PRIVATE ws2_32)
endif()
# ============================================================
# UDP Client
# ============================================================

add_executable(udp_client

    client/udp_client.cpp
)


target_include_directories(udp_client
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)



add_executable(udp_integration_test
    tests/UdpIntegrationTest.cpp
)

target_include_directories(udp_integration_test PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

if(WIN32)
    target_link_libraries(udp_integration_test PRIVATE ws2_32)
endif()





# Enable CTest
enable_testing()

add_test(
    NAME ConnectionManagerTests
    COMMAND connection_manager_test
)

add_executable(
    connection_manager_test
    tests/ConnectionManagerTest.cpp
    src/ConnectionManager.cpp
)

target_include_directories(
    connection_manager_test
    PRIVATE include
)

# Protocol unit tests
add_test(
    NAME ProtocolUnitTests
    COMMAND protocol_test
)

# Thread pool unit tests
add_test(
    NAME ThreadPoolTests
    COMMAND thread_pool_test
)

# ============================================================
# Thread Pool Test
# ============================================================

add_executable(thread_pool_test

    tests/ThreadPoolTest.cpp

    src/ThreadPool.cpp
)


target_include_directories(thread_pool_test

    PRIVATE

        ${PROJECT_SOURCE_DIR}/include
)


# ============================================================
# Windows Socket Library
# ============================================================

if(WIN32)

    target_link_libraries(server
        PRIVATE
            ws2_32
    )


    target_link_libraries(tcp_client
        PRIVATE
            ws2_32
    )


    target_link_libraries(udp_server
        PRIVATE
            ws2_32
    )


    target_link_libraries(udp_client
        PRIVATE
            ws2_32
    )

endif()

