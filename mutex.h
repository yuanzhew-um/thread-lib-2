/*
 * mutex.h -- interface to the mutex class
 *
 * You may add new variables and functions to this class.
 *
 * Do not modify any of the given function declarations.
 */

#pragma once
#include "cpu.h"
#include <memory>
#include <queue>

extern std::queue<std::shared_ptr<context>> ready_queue;

class mutex {
    friend class cv;
public:
    mutex();
    ~mutex();

    void lock();
    void unlock();

    /*
     * Disable the copy constructor and copy assignment operator.
     */
    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

    /*
     * Move constructor and move assignment operator.  Implementing these is
     * optional in Project 2.
     */
    mutex(mutex&&);
    mutex& operator=(mutex&&);

private:
    bool locked;
    std::queue<std::shared_ptr<context>> mu_wait_queue;

    /**
     * @brief The context ID of the thread that holds the mutex.
     * 
     * This variable holds the context ID of the thread that currently holds
     * the mutex. It is used to ensure that only the thread that holds the mutex
     * can unlock it.
     */
    int context_id;


    /**
     * @brief Locks the mutex without checking interrupts.
     * 
     * This function locks the mutex without checking interrupts. It is used
     * internally by the condition variable class to avoid interrupt checks.
     */
    void internal_lock();

    /**
     * @brief Unlocks the mutex without checking interrupts.
     * 
     * This function unlocks the mutex without checking interrupts. It is used
     * internally by the condition variable class to avoid interrupt checks.
     * 
     * @throws std::runtime_error if the current thread does not hold the mutex
     */
    void internal_unlock();
};
