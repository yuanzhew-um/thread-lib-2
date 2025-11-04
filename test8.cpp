#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>
#include <queue>

void boot_function(uintptr_t arg)
{
    while (true) {
        thread t1(
            [](uintptr_t) {
                std::cout << "thread1" << std::endl;
                while (true) { }
            },
            0);
    }
}

int main()
{
    cpu::boot(1, boot_function, 0, true, true, 0);
    return 0;
