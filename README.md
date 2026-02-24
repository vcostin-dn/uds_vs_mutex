# uds_vs_mutex

## TL;DR

`./run.sh` - build and run the benchmark

A benchmark comparing two inter-thread communication strategies in C++:

- **ThreadSafeQueue** — a mutex/condition-variable-based in-process message queue.
- **SocketQueue** — a Unix Domain Socket (UDS) `SOCK_SEQPACKET`-based IPC queue.

Both approaches pass messages from a producer thread to a consumer thread that applies a sorting workload to each message. The benchmark measures throughput across 5 iterations using [SBench](https://github.com/CostinV92/SBench).

## Project Structure

```
uds_vs_mutex/
├── uds_vs_mutex.cpp          # Entry point and benchmark harness
├── CMakeLists.txt
├── build.sh                  # Convenience build script
├── run.sh                    # Convenience run script
├── ThreadSafeQueue/
│   ├── ThreadSafeQueue.hpp
│   ├── ThreadSafeQueue.cpp   # Mutex + condition variable queue
│   └── CMakeLists.txt
└── SocketQueue/
    ├── SocketQueue.hpp
    └── SocketQueue.cpp       # Unix Domain Socket queue
```

## Requirements

- CMake >= 3.22.1
- A C++17-capable compiler (GCC or Clang)
- Internet access on first build (CMake fetches SBench via `FetchContent`)

## Build

Use the provided script (defaults to `Release`):

```bash
./build.sh
```

An optional build type can be passed as the first argument:

```bash
./build.sh Debug
```

Alternatively, invoke CMake directly:

```bash
cmake -S . -B build
cmake --build build
```

### Build types

The default build applies `-O3 -DNDEBUG` to the main executable. You can also pass a `CMAKE_BUILD_TYPE`:

| Build type       | Flags                                   | Notes                          |
|------------------|-----------------------------------------|--------------------------------|
| `Release`        | `-O3 -DNDEBUG`                          | Fastest, no debug output       |
| `Debug`          | `-g -O0 -DDEBUG -fno-omit-frame-pointer`| Enables verbose console output, runs only 5 messages |
| `RelWithDebInfo` | `-O2 -g -DNDEBUG`                       |                                |
| `ASan`           | `-fsanitize=address,undefined`          | Address/UB sanitizer           |

Example (Debug build):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Run

Use the provided script:

```bash
./run.sh
```

It will print an error and exit if the binary has not been built yet.

Alternatively, run the binary directly:

```bash
./build/bin/uds_vs_mutex
```

Or use the convenience CMake target:

```bash
cmake --build build --target run
```

The benchmark runs both queues for 5 iterations and prints timing results produced by SBench.
