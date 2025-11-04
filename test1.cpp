#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>
void cpu_test1(uintptr_t)
{
    thread t1(
        [](uintptr_t) {
            std::cout << cpu::self()->current_context->context_id << std::endl;
            thread t11(
                [](uintptr_t) {
                    std::cout << cpu::self()->current_context->context_id
                              << std::endl;
                },
                0);
            thread t12(
                [](uintptr_t) {
                    std::cout << cpu::self()->current_context->context_id
                              << std::endl;
                },
                0);
        },
        0);
    thread t2(
        [](uintptr_t) {
            for (int i = 0; i < 100; i++) {
                std::cout << cpu::self()->current_context->context_id
                          << " count: " << i << std::endl;
                if (i % 10 == 0) {
                    thread::yield();
                    cv cv1;
                    mutex mu;
                    cv1.wait(mu); // undefined behavior of not holdlokc when
                                  // call wait
                }
            }
        },
        0);
    thread t3(
        [](uintptr_t) {
            std::cout << cpu::self()->current_context->context_id << std::endl;
        },
        0);
    std::cout << "cpu_test1 passed" << std::endl;
}
int main()
{
    cpu::boot(1, cpu_test1, 0, 1, 1, 10);
    return 0;
}