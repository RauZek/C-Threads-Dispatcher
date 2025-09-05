# C-Threads-Dispatcher

Language Standard: C99
Build System: CMake
Platform: Linux (tested with GCC, -Wall -Wextra -Werror -Wpedantic)

# 1. Project Overview

This project implements a modular threaded message dispatching system in C.
It consists of:

Static library providing a base threading abstraction.

Two process libraries (Simple & Advanced) implementing message handling logic.

Dispatcher library that manages message routing between processes.

Test application that validates CLI input messages and interacts with the dispatcher.

The solution is split into multiple libraries and folders, ensuring separation of concerns and scalability.


# 2. Workspace Layout
c_threads/
- test_shell.sh
- CMakeLists.txt
- c_base_thread/
- c_simple_process/
- c_advanced_process/
- dispatcher/
- test_app/


# 3. Base Thread Library (c_base_thread)
Library type: Static (libc_base_thread.a)
Responsibilities: Provide generic threading and message queuing support.
Implementation:

- init: Creates a new base_process_t, spawns a thread running loop.
- send_msg: Thread-safe enqueue operation.
- loop: Waits for messages, calls process_fn. Terminates on message 0.
- destroy: Signals termination, joins thread, frees resources.


# 4. Simple Process Library (c_simple_process)
Library type: Shared (libc_simple_process.so)

- Message Range: 10–20 and 50–60 (inclusive).
- Implements a Process function that validates and logs incoming messages.
- Messages outside supported range are ignored as invalid.


# 5. Advanced Process Library (c_advanced_process)
Library type: Shared (libc_advanced_process.so)

- Message Range: 30–40 and 50–60.
- Acts the same as Simple process, but message ranges differ.


# 6. Dispatcher Library (dispatcher)
Library type: Shared (libc_dispatcher.so)

Responsibilities:
Threaded component (built on base_thread just like processes).
- Dynamically loads both simple_process and advanced_process.
- Queries both for their supported messages.
- Builds one merged array of 33 unique supported messages:
- 22 from Simple, 22 from Advanced.
- Overlapping 11 (50–60) removed → 33 remain.
- Exposes a uniform interface to the application.
- Dispatcher itself runs a thread via base_thread.
- It implements send_msg → which routes messages to the correct process(es).
- When a message is between 50–60, it dispatches to both Simple & Advanced.


# 7. Test Application (test_app)

Responsibilities:
- Dynamically links with dispatcher only.
- Calls dispatcher’s get_supported_msg to obtain merged 33-message set.
- Parses CLI arguments as uint32_t messages.
- Validates each message against the supported set.
- If unsupported → print error and exit with non-zero code.
- For valid messages, sends them to dispatcher via interface.
- After the last CLI argument, it sends end condition (0) internally, triggering clean shutdown of all the created threads.


# 8. End Condition
- Message 0 = reserved for termination.
- Not allowed as CLI input.
- Sent internally by app after last valid message processed.
- Ensures proper cleanup of all threads.


# 9. Testing
A shell script that must:
- Run the app 1000 times, each time with 100,000 random CLI arguments (within and outside valid ranges).
- Verify correct handling (valid messages processed, invalid exit).

# 10.CMake Rules
Root CMakeLists.txt builds:
- c_base_thread (static lib).
- c_simple_process, c_advanced_process, c_dispatcher (shared libs).
- c_threads_cmake (test app).
- Each library gets its own subfolder with its own CMakeLists.txt.
