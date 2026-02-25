#include <algorithm>



#include "utils.hpp"

void workload(std::string& message)
{
    DEBUG_PRINT("Received message: " << message);
    std::sort(message.begin(), message.end());
}

void workload_pointer(void* pointer)
{
    DEBUG_PRINT("Received pointer: " << *static_cast<std::string*>(pointer));
    std::string* message = static_cast<std::string*>(pointer);
    std::sort(message->begin(), message->end());
}

std::vector<std::string> messages;
void generate_messages()
{
    messages.clear();
    for (int i = 0; i < NUM_MESSAGES; ++i) {
        messages.push_back("Hello, world! " + std::to_string(i));
    }
}

int main()
{
    {
        sbench::SBench bench("test sending string over:");
        test_string_socket_queue_packet(bench);
        test_string_socket_queue_stream(bench);
        test_string_thread_safe_queue(bench);
        test_string_thread_safe_queue_optimized(bench);
    }

    {
        sbench::SBench bench("test sending pointer over:");
        test_pointer_socket_queue_stream(bench);
        test_pointer_thread_safe_queue(bench);
        test_pointer_thread_safe_queue_optimized(bench);
    }

    {
        sbench::SBench bench("test sending pointer (over socket) vs string (over message queue)");
        test_pointer_socket_queue_stream(bench);
        test_string_thread_safe_queue(bench);
        test_string_thread_safe_queue_optimized(bench);
    }

    return 0;
}
