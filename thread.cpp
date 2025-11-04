#include "thread.h"
#include <optional>

std::queue<std::shared_ptr<context>> ready_queue;
std::unordered_set<thread*> thread_set;

void thread::yield()
{
    assert_interrupts_enabled();
    interrupt_guard guard; // Disable interrupts
    ready_queue.push(
        cpu::self()->current_context); // Mark the current context as ready
    context::switchcontext(
        cpu::self()
            ->current_context); // Switch to the next context in the ready queue
    assert_interrupts_disabled();
}

void thread::join()
{
    assert_interrupts_enabled();
    interrupt_guard guard; // Disable interrupts
    if (this->finished) // If the thread has already finished, return
        return;
    while (!this->finished) {
        this->join_list.push(
            cpu::self()->current_context); // Mark the current context as
                                           // waiting for join
        context::switchcontext(cpu::self()->current_context);
        assert_interrupts_disabled();
    }
}

void thread::function_wrapper(
    uintptr_t t_in, thread_startfunc_t func, uintptr_t func_arg)
{
    thread* t = reinterpret_cast<thread*>(t_in);
    cpu::interrupt_enable(); // Enable interrupts before running user code
    func(func_arg); // Run the user code
    assert_interrupts_enabled(); // Ensure interrupts are enabled after user
                                 // code
    cpu::interrupt_disable(); // Disable interrupts for thread library code

    // Mark the thread as finished and move all threads waiting to join it to
    // the ready queue
    if (thread_set.find(t) != thread_set.end()) {
        t->finished = true;
        while (!t->join_list.empty()) {
            ready_queue.push(t->join_list.front());
            t->join_list.pop();
        }
    }

    // If the ready queue is empty, suspend the CPU
    if (ready_queue.empty()) {
        cpu::interrupt_enable_suspend();
    }

    // Switch to the next context in the ready queue
    cpu::self()->last_context = cpu::self()->current_context;
    cpu::self()->current_context = ready_queue.front();
    ready_queue.pop();
    setcontext(cpu::self()->current_context->context_ptr.get());
}

thread::thread(thread_startfunc_t func, uintptr_t arg, bool guard_enabled)
{
    std::optional<interrupt_guard> guard;
    if (guard_enabled)
        guard.emplace(); // Disable interrupts if called by user code

    // Create a new context for this thread
    this->context_ptr = std::make_shared<context>();

    // Initialize the context with the function wrapper and arguments
    makecontext(this->context_ptr.get()->context_ptr.get(),
        reinterpret_cast<void (*)()>(thread::function_wrapper), 3,
        reinterpret_cast<uintptr_t>(this), func, arg);

    this->finished = false; // Mark the thread as not finished

    // Add this thread to the global thread set
    thread_set.insert(this);

    // Add this thread's context to the ready queue
    ready_queue.push(this->context_ptr);
}

thread::thread(thread_startfunc_t func, uintptr_t arg)
    : thread(func, arg, true)
{
}

thread::~thread() { thread_set.erase(this); }

void context::switchcontext(std::shared_ptr<context> last_context)
{
    assert_interrupts_disabled();
    // If the ready queue is empty, suspend the CPU
    if (ready_queue.empty()) {
        cpu::interrupt_enable_suspend();
    }

    // Get the next context from the ready queue
    std::shared_ptr<context> next = ready_queue.front();
    ready_queue.pop();

    cpu::self()->current_context = next;
    swapcontext(last_context->context_ptr.get(), next->context_ptr.get());
    assert_interrupts_disabled();
}

static int context_count = 0;
context::context()
{
    try {
        stack_ptr = std::shared_ptr<char>(new char[STACK_SIZE]);
        context_ptr = std::make_shared<ucontext_t>();
    } catch (const std::bad_alloc&) {
        throw;
    }
    context_ptr->uc_stack.ss_sp = stack_ptr.get();
    context_ptr->uc_stack.ss_size = STACK_SIZE;
    context_ptr->uc_stack.ss_flags = 0;
    context_ptr->uc_link = nullptr;
    context_id = context_count++;
}

interrupt_guard::interrupt_guard() { cpu::interrupt_disable(); }
interrupt_guard::~interrupt_guard() { cpu::interrupt_enable(); }