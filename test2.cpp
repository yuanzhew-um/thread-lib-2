#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"
#include <iostream>

static mutex mu;
static cv cv_var;
static bool ready = false;

void test_thread_creation(uintptr_t arg)
{
    std::cout << "Running test_thread_creation...\n";
    std::cout << "Thread " << arg << " is running." << std::endl;
}

void test_thread_yield(uintptr_t arg)
{
    std::cout << "Running test_thread_yield...\n";
    std::cout << "Thread " << arg << " yielding...\n";
    int usesless_counter = 0;
    thread::yield();
    std::cout << "Thread " << arg << " resumed after yield." << std::endl;
    for (int i = 0; i < 2000; i++) {
        usesless_counter++;
        if (usesless_counter % 1000 == 0) {
            std::cout << "usesless counter 2: " << usesless_counter
                      << std::endl;
            std::cout << "thread2 yielding..." << std::endl;
            thread::yield();
        }
    }
}

void test_mutex_lock_unlock(uintptr_t arg)
{
    std::cout << "Running test_mutex_lock_unlock...\n";
    int usesless_counter = 0;
    std::cout << "Thread " << arg << " attempting to lock mutex.\n";
    mu.lock();
    std::cout << "Mutex acquired by thread " << arg << "." << std::endl;
    for (int i = 0; i < 2000; i++) {
        usesless_counter++;
        if (usesless_counter % 1000 == 0) {
            std::cout << "usesless counter 3: " << usesless_counter
                      << std::endl;
            std::cout << "thread3 yielding..." << std::endl;
            thread::yield();
        }
    }
    std::cout << "thread3 yielding after loop..." << std::endl;
    thread::yield(); // Yield to let other threads attempt locking
    mu.unlock();
    std::cout << "Mutex released by thread " << arg << "." << std::endl;
}

void test_cv_wait(uintptr_t arg)
{
    std::cout << "Running test_cv_wait...\n";

    std::cout << "Thread " << arg
              << " attempting to lock mutex for wait test.\n";
    mu.lock();
    std::cout << "Mutex acquired by thread " << arg << "." << std::endl;
    if (!ready) {
        std::cout << "Thread " << arg << " waiting on condition variable...\n";
        cv_var.wait(mu); // Wait on the condition variable (must hold lock)
        std::cout << "Thread " << arg
                  << " resumed after wait, got signaled up.\n";
    } else {
        std::cout << "Thread " << arg << " found ready == true, continuing.\n";
    }
    mu.unlock();
    std::cout << "Thread " << arg << " released mutex after wait.\n";
}

void test_cv_signal(uintptr_t arg)
{
    std::cout << "Running test_cv_signal...\n";
    std::cout << "Thread " << arg << " attempting to lock mutex for signal.\n";
    mu.lock();
    std::cout << "Mutex acquired by thread " << arg << "." << std::endl;
    ready = true;
    std::cout << "Thread " << arg << " signaling condition variable.\n";
    cv_var.signal(); // Signal the condition variable while holding lock
    mu.unlock();
    std::cout << "Thread " << arg << " released mutex after signal.\n";
}

void boot_function(uintptr_t arg)
{
    std::cout << "Boot function running on CPU...\n";
    thread t1(test_thread_creation, 1);
    thread t2(test_thread_yield, 2);
    thread t3(test_mutex_lock_unlock, 3);

    // CV test with signal
    thread t4(test_cv_wait, 4);
    thread t5(test_cv_signal, 5);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    std::cout << "All test threads joined.\n";
}

int main()
{
    // Boot the CPU and run all tests in a single thread
    cpu::boot(1, boot_function, 0, false, false, 0);
    return 0;
}
