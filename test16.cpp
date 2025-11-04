#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"
#include <iostream>
#include <queue>

/* test calling unlock when mutex is free or don't have the mutex*/

static mutex mtx;

void thread1(uintptr_t arg)
{
    mtx.lock();
    thread::yield();
    mtx.unlock();
}

void thread2(uintptr_t arg)
{
    mtx.unlock();
    mtx.lock();
    mtx.unlock();
}

void thread3(uintptr_t arg)
{
    mtx.unlock();
}

void boot_function(uintptr_t arg)
{
    thread t1(thread1, 0);
    t1.join();

    thread t2(thread2, 0);
    t2.join();

    thread t3(thread3, 0);
    t3.join();
}

int main()
{
    cpu::boot(1, boot_function, 0, false, false, 0);
    return 0;
}
