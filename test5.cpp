#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>

static mutex mu;

void recursive_lock(uintptr_t arg)
{
    std::cout << "Thread " << arg << " attempting first lock.\n";
    mu.lock();
    std::cout << "Thread " << arg << " acquired first lock.\n";

    // Attempt to lock the mutex again
    std::cout << "Thread " << arg << " attempting second lock.\n";
    mu.lock();
    std::cout << "Thread " << arg
              << " acquired second lock.\n"; // This line should not be reached
                                             // if non-recursive

    mu.unlock();
    std::cout << "Thread " << arg << " released second lock.\n";
    mu.unlock();
    std::cout << "Thread " << arg << " released first lock.\n";
}

void boot_function(uintptr_t arg)
{
    thread t(recursive_lock, 1);
    t.join();
}

int main()
{
    cpu::boot(1, boot_function, 0, false, false, 0);
    return 0;
}
