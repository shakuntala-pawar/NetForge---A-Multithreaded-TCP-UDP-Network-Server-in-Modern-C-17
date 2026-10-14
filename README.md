# NetForge

### A Multithreaded TCP/UDP Network Server in Modern C++17

![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus&logoColor=white)
![CMake](https://img.shields.io/badge/build-CMake%20%2B%20Ninja-064F8C?logo=cmake&logoColor=white)
![Platform](https://img.shields.io/badge/platform-Windows%20(Winsock2)-0078D6?logo=windows&logoColor=white)
![Tests](https://img.shields.io/badge/tests-CTest-success)

NetForge is a modular, systems-level networking project that implements a **concurrent TCP server backed by a custom thread pool** and a **UDP request-response server with built-in runtime statistics**. Both servers share a single command protocol, and both come with dedicated command-line clients and automated tests.

The project explores core systems-programming topics: socket programming, concurrency and synchronization, resource management (RAII), graceful shutdown, and testable C++ architecture.

---

## Table of Contents

- [Highlights](#highlights)
- [Architecture](#architecture)
- [Protocol Reference](#protocol-reference)
- [Technology Stack](#technology-stack)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Testing](#testing)
- [Project Structure](#project-structure)
- [Design Decisions](#design-decisions)
- [Known Limitations](#known-limitations)
- [Roadmap](#roadmap)
- [License](#license)

---

## Highlights

| Area | What is implemented |
|---|---|
| **Concurrency** | TCP server dispatches each client session to a fixed-size worker thread pool (4 workers by default) using a mutex-guarded task queue and condition variable. |
| **Connection tracking** | Thread-safe `ConnectionManager` registers, removes, and lists active clients with unique IDs. |
| **Shared protocol layer** | A single `Protocol` component processes commands for both transports, keeping business logic independent of the network layer. |
| **UDP observability** | UDP server maintains counters for received datagrams, sent responses, invalid commands, and socket errors, exposed through the `UDP_STATS` command. |
| **Graceful shutdown** | UDP server uses a receive timeout and an atomic shutdown flag, driven by a Windows console control handler (Ctrl+C / Ctrl+Break / window close). The TCP server stops its worker pool during shutdown. |
| **Structured logging** | Mutex-protected `Logger` produces timestamped connection, request, and response logs without interleaved output. |
| **Testing** | CTest-registered unit tests for the protocol, thread pool, and connection manager, plus a UDP integration test executable. |
| **Build system** | CMake 3.20+ with Ninja; separate targets for servers, clients, and tests. |

---

## Architecture

```text
                         ┌───────────────────────────┐
   TCP client  ───────►  │         TcpServer         │
                         │  accept loop + client IDs │
                         └─────────────┬─────────────┘
                                       │ enqueue(session)
                         ┌─────────────▼─────────────┐
                         │         ThreadPool        │
                         │   N workers + task queue  │
                         └─────────────┬─────────────┘
                                       │
                         ┌─────────────▼─────────────┐      ┌───────────────────┐
                         │       ClientSession       │────► │ ConnectionManager │
                         │  recv → process → send    │      │  (mutex-guarded)  │
                         └─────────────┬─────────────┘      └───────────────────┘
                                       │
                                       ▼
                              ┌─────────────────┐
   UDP client  ─────────────► │    UdpServer    │ ──►  Protocol::processRequest()
   (datagram)                 │ recvfrom/sendto │ ◄──  shared by TCP and UDP
                              └─────────────────┘
                                       │
                                  Logger / UDP counters
```

### Components

| Component | Responsibility |
|---|---|
| `TcpServer` | Creates, binds, and listens on the TCP socket; accepts clients; assigns unique client IDs; submits sessions to the thread pool; coordinates shutdown. |
| `ClientSession` | Owns one client socket; receives requests, calls `Protocol`, sends responses, and deregisters the client on exit. |
| `ThreadPool` | Fixed-size worker pool with a task queue, condition variable, and clean shutdown. Non-copyable. |
| `ConnectionManager` | Thread-safe registry of connected clients (add, remove, active check, count, listing). |
| `Protocol` | Stateless command parser and response generator shared by both transports. |
| `UdpServer` | Receives datagrams, reports sender IP and port, executes commands, sends responses, and tracks runtime statistics. |
| `Logger` | Thread-safe, timestamped server-side logging. |
| `tcp_client` / `udp_client` | Interactive command-line clients for each server. |

### Request flow

- **TCP:** client → `TcpServer` → `ThreadPool` → `ClientSession` → `Protocol` → response
- **UDP:** client datagram → `UdpServer` → `Protocol` (or `UDP_STATS`) → response datagram

---

## Protocol Reference

Commands are plain-text and case-sensitive.

| Command | Description | Response |
|---|---|---|
| `PING` | Liveness check | `PONG` |
| `STATUS` | Server status | `SERVER_OK` |
| `TIME` | Current server time | `SERVER_TIME <timestamp>` |
| `ECHO <message>` | Returns the supplied message | `<message>` |
| `QUIT` | Requests session termination | `GOODBYE` |
| *anything else* | Unrecognized command | `ERROR Unknown command` |
| `UDP_STATS` *(UDP only)* | Server counters | `UDP_STATS received=<n> sent=<n> invalid=<n> errors=<n>` |

> **Transport note:** TCP is a byte stream while UDP preserves datagram boundaries. Over UDP, `QUIT` returns `GOODBYE` but does not stop the server, since UDP has no connection to close.

### Default endpoints

| Server | Address | Executable |
|---|---|---|
| TCP | `127.0.0.1:8080` | `server.exe` |
| UDP | `127.0.0.1:9090` | `udp_server.exe` |

---

## Technology Stack

- **Language:** C++17 (extensions disabled)
- **Networking:** TCP/IP and UDP via Windows Winsock2
- **Concurrency:** `std::thread`, `std::mutex`, `std::condition_variable`, `std::atomic`
- **Build:** CMake (3.20+) and Ninja
- **Toolchain:** GCC via MSYS2 UCRT64
- **Testing:** CTest with standalone C++ test executables
- **Version control:** Git

---

## Getting Started

### Prerequisites

- Windows (the networking layer currently uses Winsock2)
- A C++17-compatible compiler (GCC from MSYS2 UCRT64 is the reference toolchain)
- CMake 3.20 or newer
- Ninja
- Git

### Build

```powershell
git clone <YOUR_REPOSITORY_URL>
cd <YOUR_REPOSITORY_DIRECTORY>

cmake -S . -B build -G Ninja
cmake --build build
```

This produces `server.exe`, `tcp_client.exe`, `udp_server.exe`, `udp_client.exe`, and the test executables in `build/`.

---

## Usage

Run each component in its own terminal.

### TCP

```powershell
# Terminal 1
.\build\server.exe

# Terminal 2 (repeat in more terminals to test concurrent clients)
.\build\tcp_client.exe
```

### UDP

```powershell
# Terminal 1
.\build\udp_server.exe

# Terminal 2
.\build\udp_client.exe
```

### Example session

```text
> PING
PONG
> ECHO Hello, NetForge
Hello, NetForge
> STATUS
SERVER_OK
> FOO
ERROR Unknown command
> UDP_STATS            (UDP only)
UDP_STATS received=4 sent=4 invalid=1 errors=0
```

Stop the UDP server with `Ctrl+C`; it exits its receive loop and closes the socket cleanly.

---

## Testing

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

| Test suite | Coverage |
|---|---|
| `ProtocolUnitTests` | Command parsing and response behavior |
| `ThreadPoolTests` | Task execution and pool behavior |
| `ConnectionManagerTests` | Client registration, removal, duplicate IDs, active counts, and listing |

A separate `udp_integration_test` executable is built for end-to-end UDP request-response checks. It requires a running `udp_server` and is run manually rather than through CTest.

---

## Project Structure

```text
.
├── include/                 # Public headers
│   ├── ClientSession.hpp
│   ├── ConnectionManager.hpp
│   ├── Logger.hpp
│   ├── Protocol.hpp
│   ├── TcpServer.hpp
│   ├── ThreadPool.hpp
│   └── UdpServer.hpp
├── src/                     # Server implementation
│   ├── ClientSession.cpp
│   ├── ConnectionManager.cpp
│   ├── Logger.cpp
│   ├── Protocol.cpp
│   ├── TcpServer.cpp
│   ├── ThreadPool.cpp
│   ├── UdpServer.cpp
│   ├── main.cpp             # TCP server entry point
│   └── udp_main.cpp         # UDP server entry point
├── client/
│   ├── tcp_client.cpp
│   └── udp_client.cpp
├── tests/
│   ├── ConnectionManagerTest.cpp
│   ├── ProtocolTest.cpp
│   ├── ThreadPoolTest.cpp
│   └── UdpIntegrationTest.cpp
├── CMakeLists.txt
└── README.md
```

---

## Design Decisions

- **Separation of concerns:** socket handling, session logic, protocol parsing, connection tracking, logging, and concurrency each live in their own component.
- **Transport-independent protocol:** `Protocol` has no socket dependencies, which makes it trivially unit-testable and reusable across TCP and UDP.
- **Bounded concurrency:** a fixed thread pool avoids unbounded thread-per-connection growth and keeps resource usage predictable.
- **Explicit synchronization:** shared state (connection registry, task queue, log output) is guarded by mutexes; lifecycle flags use atomics.
- **Responsive shutdown:** the UDP receive timeout lets the blocking loop periodically observe the shutdown flag.
- **Observability:** UDP counters and structured logs make runtime behavior easy to inspect and debug.

---

## Known Limitations

- Networking currently targets **Windows / Winsock2** only.
- Host, port, and worker-thread count are compile-time defaults (`127.0.0.1:8080`, `127.0.0.1:9090`, 4 workers), not runtime options.
- Requests are read into fixed-size buffers (1 KB for TCP sessions, 4 KB for UDP datagrams), so oversized messages are not supported.
- UDP provides no delivery or ordering guarantees; statistics are in-memory only and reset on restart.
- Load, stress, and cross-platform testing have not been done yet.
- Intended for learning and demonstration, not as a hardened internet-facing service.

---

## Roadmap

- [ ] GitHub Actions CI (build and test on Windows)
- [ ] Cross-platform socket abstraction (POSIX sockets) and Linux support
- [ ] Runtime configuration for host, port, and thread count
- [ ] Register UDP integration tests in CTest with automatic server lifecycle
- [ ] Stress and concurrency tests, plus throughput/latency benchmarks
- [ ] Message framing and input-size validation for the TCP protocol
- [ ] Sanitizer (ASan/UBSan/TSan) builds and static analysis in CI
- [ ] Extended metrics for the TCP server

---

## License

No license has been specified yet. Add a `LICENSE` file (for example, MIT) before distributing or reusing this project.
