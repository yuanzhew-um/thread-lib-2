# Thread Library

## Description
This project implements a custom **threading library** in **C++**, designed to simulate how an operating system schedules and manages user-level threads.  
It recreates core synchronization primitives and scheduling mechanisms without relying on built-in OS threading, offering a hands-on view of low-level concurrency.

## Architecture
The system is structured around several core modules:
- **CPU Module:** Manages context switching between threads and controls execution.  
- **Thread Module:** Handles thread creation, scheduling, and state management.  
- **Mutex & Condition Variable Modules:** Implement synchronization mechanisms for thread-safe communication and mutual exclusion.  

## Core Functionality
- Cooperative user-level threading using context switching.  
- Mutex and condition variable support for synchronized access.  
- Thread sleep, wakeup, and signal operations.  
- Lightweight, portable design running entirely in user space.  
- Comprehensive test suite validating various thread and synchronization scenarios.
