#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>

bool ready = false;
static mutex mtx;
static cv condition_var;
void worker(uintptr_t arg)
{
    // Lock the mutex
    mtx.lock();

    // Questionable style: no while loop
    condition_var.wait(mtx);

    if (ready) {
        std::cout << "Worker: Proceeding after notification.\n";
    } else {
        std::cout << "Worker: Proceeding, but 'ready' is false.\n";
    }
    // Unlock the mutex
    mtx.unlock();
}

void notifier(uintptr_t arg)
{
    thread::yield(); // Simulate delay
    ready = true;
    condition_var.signal(); // Notify the worker thread
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
