/*
 * thread.h -- interface to the thread library
 *
 * This file should be included by the thread library and by application
 * programs that use the thread library.
 *
 * You may add new variables and functions to this class.
 *
 * Do not modify any of the given function declarations.
 */

#pragma once

#include "cpu.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <queue>
#include <ucontext.h>
#include <unordered_set>
class context;


#if !defined(__cplusplus) || __cplusplus < 201700L
#error Please configure your compiler to use C++17 or C++20
#endif

#if !defined(__clang__) && defined(__GNUC__) && __GNUC__ < 11
#error Please use g++ version 11 or higher
#endif

static constexpr unsigned int STACK_SIZE
    = 262144; // size of each thread's stack in bytes

using thread_startfunc_t = void (*)(uintptr_t);
class thread {
    friend class cpu;

public:
    thread(thread_startfunc_t func, uintptr_t arg); // create a new thread
    ~thread();

    void join(); // wait for this thread to finish

    static void yield(); // yield the CPU

    /*
     * Disable the copy constructor and copy assignment operator.
     */
    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

    /*
     * Move constructor and move assignment operator.  Implementing these is
     * optional in Project 2.
     */
    thread(thread&&);
    thread& operator=(thread&&);

    /**
     * @brief A pointer to the context object associated with the thread.
     *
     * This pointer holds a shared pointer to the context object that is
     * associated with the thread.
     */
    std::shared_ptr<context> context_ptr;

    /**
     * @brief A queue to manage waiting contexts for the thread.
     *
     * This queue holds shared pointers to context objects, representing
     * the contexts that are currently waiting to join the thread.
     */
    std::queue<std::shared_ptr<context>> join_list;

private:
    /**
     * @brief Constructs a new thread object, should only be used by the thread
     * library.
     *
     * This constructor initializes a new thread object and sets up the thread
     * to start executing the provided function with the given argument.
     * Optionally, it can enable or disable guard pages for stack overflow
     * protection.
     *
     * @param func The function to be executed by the thread.
     * @param arg The argument to be passed to the function.
     * @param guard_enabled A boolean flag indicating whether guard pages should
     * be enabled.
     */
    thread(thread_startfunc_t func, uintptr_t arg, bool guard_enabled);

    /**
     * @brief A boolean flag indicating whether the thread has finished.
     *
     * This flag is set to true when the thread has finished executing.
     */
    bool finished;

    /**
     * @brief Wrapper function to execute a thread start function with its
     * argument.
     *
     * This static function serves as a wrapper to call the provided thread
     * start function with the given argument. It ensures proper interrupt
     * handling and manages the thread's lifecycle, including marking it as
     * finished and handling join operations. It also sets the next context to
     * be executed on the CPU.
     *
     * @param t_in An integer value representing the thread identifier or other
     * relevant data.
     * @param func A function pointer to the thread start function to be
     * executed.
     * @param func_arg An integer value representing the argument to be passed
     * to the thread start function.
     */
    static void function_wrapper(
        uintptr_t t_in, thread_startfunc_t func, uintptr_t func_arg);
};

/**
 * @class context
 * @brief Manages the execution context for a thread, including its stack and
 * context ID.
 *
 * The context class encapsulates the stack and execution context for a thread.
 * It provides mechanisms to allocate and manage the stack and context, and to
 * switch between contexts.
 *
 * @note Copy and move operations are deleted to prevent unintended behavior.
 */
class context {
private:
    std::shared_ptr<char> stack_ptr;

public:
    std::shared_ptr<ucontext_t> context_ptr;
    int context_id;

    /**
     * @brief Constructs a new context object.
     *
     * This constructor initializes a new context object by allocating memory
     * for the stack and the context structure. It sets up the stack pointer,
     * stack size, and other context attributes. If memory allocation fails, it
     * throws a std::bad_alloc exception.
     *
     * @throws std::bad_alloc if memory allocation for the stack or context
     * structure fails.
     */
    context();

    /**
     * @brief Switches the current execution context to the next ready context.
     *
     * This static function is responsible for switching the execution context
     * from the current context to the next available ready context. It also
     * sets the current context of the CPU to the next context to be executed
     *
     * @param current_context A shared pointer to the context that will be
     * switched from.
     */
    static void switchcontext(std::shared_ptr<context> current_context);

    context(const context&) = delete;
    context& operator=(const context&) = delete;
    context(context&&) = delete;
    context& operator=(context&&) = delete;
};

/**
 * @class interrupt_guard
 * @brief A guard class to manage interrupt states.
 *
 * The interrupt_guard class is designed to manage the state of interrupts
 * within a scope. Upon creation, it disables interrupts, and upon destruction,
 * it restores the previous interrupt state.
 */
class interrupt_guard {
public:
    interrupt_guard();
    ~interrupt_guard();
};