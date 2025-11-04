#include "cv.h"

cv::cv() { }

cv::~cv() { }

void cv::wait(mutex& mu)
{
    interrupt_guard guard;
    mu.internal_unlock();
    cv_wait_queue.push(cpu::self()->current_context);
    context::switchcontext(cpu::self()->current_context);
    mu.internal_lock();
}

void cv::signal()
{
    interrupt_guard guard;
    // Wake up one thread
    if (!cv_wait_queue.empty()) {
        ready_queue.push(cv_wait_queue.front());
        cv_wait_queue.pop();
    }
}

void cv::broadcast()
{
    interrupt_guard guard;
    // Wake up all threads
    while (!cv_wait_queue.empty()) {
        ready_queue.push(cv_wait_queue.front());
        cv_wait_queue.pop();
    }
}