#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>

static mutex mu;

void lock_and_exit(uintptr_t arg)
{
    std::cout << "Thread " << arg << " is acquiring the mutex and will exit.\n";
    mu.lock();
    std::cout << "Thread " << arg
              << " acquired the mutex and is now exiting without unlocking.\n";
    // Exiting while still holding the mutex
}

void blocking_thread(uintptr_t arg)
{
    std::cout << "Thread " << arg << " is attempting to lock the mutex.\n";
    mu.lock();
    std::cout << "Thread " << arg << " acquired the mutex.\n";
    mu.unlock();
    std::cout << "Thread " << arg << " released the mutex.\n";
}

void boot_function(uintptr_t arg)
{
    thread t1(lock_and_exit, 1);
    t1.join();

    thread t2(blocking_thread, 2);
    t2.join();
}

int main()
{
    cpu::boot(1, boot_function, 0, false, false, 0);
    return 0;
}
