#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>

static mutex mu;
static cv cv_var;
static bool ready = false;

void waiter(uintptr_t arg)
{
    mu.lock();
    while (!ready) {
        std::cout << "Thread " << arg << " waiting on condition variable.\n";
        cv_var.wait(mu);
    }
    std::cout << "Thread " << arg << " resumed.\n";
    mu.unlock();
}

void signaler(uintptr_t arg)
{
    for (int i = 0; i < 3; ++i) {
        mu.lock();
        std::cout << "Signaler signaling thread.\n";
        ready = true;
        cv_var.signal();
        mu.unlock();
        thread::yield(); // Yield to let a waiter proceed
    }
}

void boot_function(uintptr_t arg)
{
    thread t1(waiter, 1);
    thread t2(waiter, 2);
    thread t3(waiter, 3);
    thread t4(signaler, 4);

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
