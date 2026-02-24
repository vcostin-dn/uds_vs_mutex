# uds_vs_mutex

## TL;DR

`./run.sh (debug|asan)` - build and run the benchmark

A benchmark comparing three inter-thread message-passing strategies in C++:

- **ThreadSafeQueue** — single-buffer mutex + condition variable queue; processes one message per lock acquisition.
- **ThreadSafeQueueOptimized** — double-buffer mutex + condition variable queue; swaps buffers under a short lock then processes the entire batch without holding it.
- **SocketQueue** — Unix Domain Socket (`SOCK_SEQPACKET`-based) queue; one `sendmsg`/`recvmsg` syscall per message.

Each test sends 1,000,000 messages from a producer thread to a consumer thread that applies a sorting workload to each message. Timing is measured with [SBench](https://github.com/CostinV92/SBench), which reports total wall time and a relative comparison across all three.

## Project Structure

```
uds_vs_mutex/
├── uds_vs_mutex.cpp          # Entry point and benchmark harness
├── CMakeLists.txt
├── build.sh                  # Convenience build script
├── run.sh                    # Convenience run script
├── ThreadSafeQueue/
│   ├── ThreadSafeQueue.hpp   # ThreadSafeQueue + ThreadSafeQueueOptimized
│   ├── ThreadSafeQueue.cpp
│   └── CMakeLists.txt
└── SocketQueue/
    ├── SocketQueue.hpp
    └── SocketQueue.cpp
```

## Build

```bash
./build.sh          # Release (default)
./build.sh Debug    # Debug
./build.sh ASan     # ASan
./build.sh clean    # remove all build directories
```

### Build types

| Argument          | Build directory | Flags                                    | Notes                                        |
|-------------------|-----------------|------------------------------------------|----------------------------------------------|
| `Release` (default) | `build/`      | `-O3 -DNDEBUG`                           | Fastest, no debug output, 1,000,000 messages |
| `Debug`           | `debug/`        | `-g -O0 -DDEBUG -fno-omit-frame-pointer` | Verbose console output, 3 messages per run   |
| `ASan`            | `asan/`         | `-fsanitize=address,undefined`           | Address/UB sanitizer                         |

## Run

```bash
./run.sh            # Release
./run.sh Debug      # Debug
./run.sh ASan       # ASan
```

`run.sh` will print an error and exit if the binary for the requested build type has not been built yet.

