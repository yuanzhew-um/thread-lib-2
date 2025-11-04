#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>

bool ready = false;
static mutex m1;
static mutex m2;
static cv condition_var;

void worker1(uintptr_t arg)
{
    m1.lock();
    std::cout << "Worker 1: Locked m1, waiting on condition variable.\n";

    while (!ready) {
        condition_var.wait(m1);
    }

    std::cout << "Worker 1: Proceeding after notification.\n";

    m1.unlock();
}

void worker2(uintptr_t arg)
{
    m2.lock();
    std::cout << "Worker 2: Locked m2, waiting on condition variable.\n";

    while (!ready) {
        condition_var.wait(m2);
    }

    std::cout << "Worker 2: Proceeding after notification.\n";

    m2.unlock();
}

void notifier(uintptr_t arg)
{
    thread::yield();
    m1.lock();
    m2.lock();

    std::cout << "Notifier: Notifying all threads.\n";
    ready = true;
    condition_var.broadcast();

    m2.unlock();
    m1.unlock();
}

void boot_function(uintptr_t arg)
{
    thread worker_thread1(worker1, 0);
    thread worker_thread2(worker2, 0);
    thread notifier_thread(notifier, 0);

    worker_thread1.join();
    worker_thread2.join();
    notifier_thread.join();
}

int main()
{
    cpu::boot(1, boot_function, 0, false, false, 0);
    return 0;
}
