#include "cpu.h"

/**
 * @brief Handles the timer interrupt by yielding the current thread.
 *
 * This function is called when a timer interrupt occurs. It invokes the 
 * thread::yield() function to yield the processor, allowing the scheduler 
 * to switch to another thread.
 */
void timer_interrupt_handler() { thread::yield(); }

cpu::cpu(thread_startfunc_t func, uintptr_t arg)
{
    // Set the timer interrupt handler
    interrupt_vector_table[TIMER] = timer_interrupt_handler;

    // If a thread start function is provided, create a new thread
    if (func != nullptr) {
        thread(func, arg, false);
    }

    // If the ready queue is empty, enable interrupts and suspend the CPU
    if (ready_queue.empty()) {
        cpu::interrupt_enable_suspend();
        cpu::interrupt_disable();
    }

    // Switch to the next context in the ready queue
    this->current_context = ready_queue.front();
    ready_queue.pop();
    setcontext(this->current_context->context_ptr.get());
}
