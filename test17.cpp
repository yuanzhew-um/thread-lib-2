#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"
#include <iostream>
#include <queue>

static mutex mtx;
static cv cv1;
static cv cv2;
static bool ready1 = false;
static bool ready2 = false;

void worker1(uintptr_t arg)
{
    mtx.lock();
    while (!ready1)
    {
        cv1.wait(mtx);
    }
    mtx.unlock();
}

void worker2(uintptr_t arg)
{
    mtx.lock();
    while (!ready2)
    {
        cv2.wait(mtx);
    }
    mtx.unlock();
}

void notifier1(uintptr_t arg)
{
    mtx.lock();
    ready1 = true;
    cv1.signal();
    mtx.unlock();
}

void notifier2(uintptr_t arg)
{
    mtx.lock();
    ready2 = true;
    cv2.signal();
    mtx.unlock();
}

void boot_function(uintptr_t arg)
{
    thread t1(worker1, 0);
    thread t2(worker2, 0);
    thread n1(notifier1, 0);
    thread n2(notifier2, 0);

    t1.join();
    t2.join();
    n1.join();
    n2.join();
}

int main()
{
    cpu::boot(1, boot_function, 0, false, false, 0);
    return 0;
}
