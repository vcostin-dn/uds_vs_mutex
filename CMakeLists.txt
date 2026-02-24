cmake_minimum_required(VERSION 3.22.1)
project(mvi-cpp VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_COLOR_DIAGNOSTICS ON)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -DNDEBUG")

# Configure debug and release flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_ASAN "-fsanitize=address,undefined -fno-omit-frame-pointer")

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(FetchContent)

FetchContent_Declare(
    sbench
    GIT_REPOSITORY https://github.com/CostinV92/SBench
    GIT_TAG main
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(
    sbench
)

add_subdirectory(ThreadSafeQueue)
add_subdirectory(SocketQueue)

add_executable(uds_vs_mutex uds_vs_mutex.cpp)
target_link_libraries(uds_vs_mutex PRIVATE sbench socket_queue thread_safe_queue)
target_compile_options(uds_vs_mutex PRIVATE -O3 -DNDEBUG)

add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/bin/uds_vs_mutex
    DEPENDS uds_vs_mutex
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMENT "Running uds_vs_mutex..."
)