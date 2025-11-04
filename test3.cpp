#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>

void worker_thread(uintptr_t arg)
{
    std::cout << "Thread " << arg << " is doing work.\n";
    for (int i = 0; i < 3; ++i) {
        std::cout << "Thread " << arg << " working... step " << i + 1
                  << std::endl;
        thread::yield(); // Yield to allow other threads to attempt joining
    }
    std::cout << "Thread " << arg << " has finished work.\n";
}

void joiner_thread(uintptr_t arg)
{
    static thread t1(
        worker_thread, 1); // Ensure thread 1 is shared among joiners

    std::cout << "Thread " << arg << " is joining Thread 1.\n";
    t1.join(); // This thread will wait until Thread 1 completes
    std::cout << "Thread " << arg << " resumed after Thread 1 completed.\n";
}

void boot_function(uintptr_t arg)
{
    thread t2(joiner_thread, 2);
    thread t3(joiner_thread, 3);
    thread t4(joiner_thread, 4);

    t2.join();
    t3.join();
    t4.join();
}

int main()
{
    cpu::boot(1, boot_function, 0, false, false, 0);
    return 0;
}
