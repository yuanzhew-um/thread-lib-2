#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>
#include <queue>

static mutex mu;
static cv cv_var;
static std::queue<int> buffer;
static const int MAX_BUFFER_SIZE = 5;

void producer(uintptr_t arg)
{
    for (int i = 0; i < 10; ++i) {
        mu.lock();
        while (buffer.size() >= MAX_BUFFER_SIZE) {
            std::cout << "Buffer full. Producer waiting.\n";
            cv_var.wait(mu);
        }
        buffer.push(i);
        std::cout << "Produced item: " << i << std::endl;
        cv_var.signal();
        mu.unlock();
    }
}

void consumer(uintptr_t arg)
{
    for (int i = 0; i < 10; ++i) {
        mu.lock();
        while (buffer.empty()) {
            std::cout << "Buffer empty. Consumer waiting.\n";
            cv_var.wait(mu);
        }
        int item = buffer.front();
        buffer.pop();
        std::cout << "Consumed item: " << item << std::endl;
        cv_var.signal();
        mu.unlock();
    }
}

void boot_function(uintptr_t arg)
{
    thread t1(producer, 1);
    thread t2(consumer, 2);
    t1.join();
    t2.join();
}

int main()
{
    cpu::boot(1, boot_function, 0, false, false, 0);
    return 0;
}
