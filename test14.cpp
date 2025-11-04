#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>
#include <memory>
#include <vector>

std::vector<std::shared_ptr<thread>> thread_list;

void self_join_test(uintptr_t arg)
{
    // Intentionally joining the thread on itself
    std::cout << "Thread " << arg << " is joining itself.\n";
    while (true) {
        std::cout << "Thread " << arg << " is running.\n";
        if (thread_list.size() > 0) {
            thread_list[0]->join();
        }
    }
}

void boot_function(uintptr_t arg)
{
    std::shared_ptr<thread> t = std::make_shared<thread>(self_join_test, 0);
    thread_list.push_back(t);
}

int main()
{
    cpu::boot(1, boot_function, 0, false, false, 0);
    return 0;
}
