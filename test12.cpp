#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>

// test Deadlock
bool ready = false;
static mutex m1;
static mutex m2;
static cv condition_var;
void worker(uintptr_t arg)
{
    // Lock the mutex
    m1.lock();
    thread::yield();
    m2.lock();
    m2.unlock();
    m1.unlock();
}

void notifier(uintptr_t arg)
{
    m2.lock();
    thread::yield();
    m1.lock();
    m1.unlock();
    m2.unlock();
}

void boot_function(uintptr_t arg)
{
    thread worker_thread(worker, 0);
    thread notifier_thread(notifier, 0);
    // Join both threads
    worker_thread.join();
    notifier_thread.join();
}

int main()
{
    cpu::boot(1, boot_function, 0, false, false, 0);
    return 0;
}
