#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
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

void thread3(uintptr_t arg)
{
    try

    {
        mtx.unlock();
        std::cout << "wrong";

    } catch (...)

    {
        std::cout << "correct" << '\n';
    }
}

void boot_function(uintptr_t arg)
{
    thread t1(thread1, 0);
    t1.join();

    thread t3(thread3, 0);
    t3.join();
}

int main()
{
    cpu::boot(1, boot_function, 0, false, false, 0);
    return 0;
}
