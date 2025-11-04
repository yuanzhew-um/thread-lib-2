#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>
#include <queue>

static mutex mtx;
static cv condition_var;
static bool ready = false;
static bool done1 = false;
static bool done2 = false;

void worker1(uintptr_t arg)
{
    mtx.lock();
    while (!ready) {
        condition_var.wait(mtx);
    }
    done1 = true;
    std::cout << "Worker1: Proceeding after notification.\n" << std::endl;
    mtx.unlock();
}

void worker2(uintptr_t arg)
{
    mtx.lock();
    while (!ready) {
        condition_var.wait(mtx);
    }
    done2 = true;
    std::cout << "Worker2: Proceeding after notification.\n" << std::endl;
    mtx.unlock();
}

void notifier(uintptr_t arg)
{
    mtx.lock();
    ready = true;
    condition_var.signal();
    mtx.unlock();
    thread::yield();
    mtx.lock();
    if (!done1 || !done2) {
        condition_var.broadcast();
    }
    std::cout << "Notifer: Broadcasted\n" << std::endl;
    mtx.unlock();
}

void boot_function(uintptr_t arg)
{
    thread t1(worker1, 0);
    thread t2(worker2, 0);
    thread n(notifier, 0);

    t1.join();
    t2.join();
    n.join();
}

int main()
{
    cpu::boot(1, boot_function, 0, false, false, 0);
    return 0;
}
