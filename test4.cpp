#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>

static mutex mu;

void lock_and_unlock(uintptr_t arg)
{
    std::cout << "Running lock_and_unlock...\n";
    std::cout << "Thread " << arg << " attempting to lock mutex.\n";
    mu.lock();
    std::cout << "Thread " << arg << " acquired mutex.\n";

    // Simulate some work in the critical section
    for (int i = 0; i < 3; ++i) {
        std::cout << "Thread " << arg << " working in critical section... step "
                  << i + 1 << std::endl;
        thread::yield(); // Yield within the critical section
    }

    mu.unlock();
    std::cout << "Thread " << arg << " released mutex.\n";
}

void boot_function(uintptr_t arg)
{
    thread t5(lock_and_unlock, 5);
    thread t6(lock_and_unlock, 6);

    t5.join();
    t6.join();
}

int main()
{
    cpu::boot(1, boot_function, 0, false, false, 0);
    return 0;
}
