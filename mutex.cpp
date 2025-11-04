#include "mutex.h"

mutex::mutex() { this->locked = false; }

mutex::~mutex() { }

void mutex::lock()
{
    assert_interrupts_enabled();
    interrupt_guard guard;
    this->internal_lock();
}

void mutex::unlock()
{
    assert_interrupts_enabled();
    interrupt_guard guard;
    try{
        this->internal_unlock();
    } catch(...) {
        throw;
    }
}

void mutex::internal_lock()
{
    assert_interrupts_disabled();
    if (this->locked) {
        // If the mutex is already locked, add the current thread to the wait
        // queue and switch to the next context
        this->mu_wait_queue.push(cpu::self()->current_context);
        context::switchcontext(cpu::self()->current_context);
    } else {
        // lock the mutex
        this->locked = true;
        this->context_id = cpu::self()->current_context->context_id;
    }
    assert_interrupts_disabled();
}

void mutex::internal_unlock()
{
    assert_interrupts_disabled();
    if (this->context_id != cpu::self()->current_context->context_id) {
        throw std::runtime_error("The current thread does not hold the mutex "
                                 "while unlocking");
    }
    if (!this->mu_wait_queue.empty()) {
        // Hand over the mutex to the next thread in the wait queue
        std::shared_ptr<context> next = this->mu_wait_queue.front();
        this->mu_wait_queue.pop();
        ready_queue.push(next);
        this->context_id = next->context_id;
    } else {
        // If the wait queue is empty, unlock the mutex
        this->locked = false;
        this->context_id = -1;
    }
}