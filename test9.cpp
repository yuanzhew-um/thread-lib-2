#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>

static mutex mu;
static cv cv_var;
static bool ready = false;

void waiting_thread(uintptr_t arg)
{
    mu.lock();
    while (!ready) {
        std::cout << "Thread " << arg << " waiting on condition variable.\n";
        cv_var.wait(mu);
    }
    std::cout << "Thread " << arg << " resumed after broadcast.\n";
    mu.unlock();
}

void broadcaster_thread(uintptr_t arg)
{
    mu.lock();
    std::cout << "Broadcaster setting ready to true and broadcasting.\n";
    ready = true;
    cv_var.broadcast(); // Wakes up all waiting threads
    mu.unlock();
}

void boot_function(uintptr_t arg)
{
    thread t1(waiting_thread, 1);
    thread t2(waiting_thread, 2);
    thread t3(waiting_thread, 3);
    thread t4(broadcaster_thread, 4);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
}

int main()
{
    cpu::boot(1, boot_function, 0, false, false, 0);
    return 0;
}
