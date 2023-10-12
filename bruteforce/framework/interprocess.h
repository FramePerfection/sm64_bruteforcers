#ifndef BF_INTERPROCESS_H
#define BF_INTERPROCESS_H

/// @brief A callback function to initialize memory reserved via 'bf_reserve_shared_memory' when it is allocated in 'bf_start_multiprocessing'.
/// @param dataPtr A persistent pointer that may be used to address the allocated memory for the entire runtime of the program.
typedef void (*MemoryAllocatedCallback)(char *dataPtr);

/// @brief Retrieves whether this process is a parent or a child process.
/// @return 'TRUE' if this process is a parent process, 'FALSE' otherwise. 
///         Unlike 'bf_get_process_index', the result is defined even before calling 'bf_start_multiprocessing'.
unsigned char bf_is_parent_process();

/// @brief Retrieves an index unique to the running process, only after 'bf_start_multiprocessing' has been called.
/// @return A unique index in the range [0; bfStaticState.max_processes).
///         If 'bf_start_multiprocessing' has not been called, the result is undefined.
unsigned int bf_get_process_index();

/// @brief Reserves 'size' bytes in the shared memory page used for multiprocessing and registers a callback when the memory has been allocated.
/// @param size The number of bytes to reserve.
/// @param MemoryAllocatedCallback A callback accepting the virtual memory address the reserved memory has been allocated at.
/// @remarks All instances of the same process are expected to call this function in the same order with the same arguments before calling 'bf_start_multiprocessing'.
void bf_reserve_shared_memory(unsigned int size, MemoryAllocatedCallback callback);

/// @brief If called from the parent process, spawns 'bfStaticState.max_processes' - 1 child processes, 
///        allocates and initializes shared memory as defined by previous calls to 'bf_reserve_shared_memory',
///        and begins listening for control state inputs on stdin.
///        If called from a child process, this will only initialize the shared memory.
void bf_start_multiprocessing();

/// @brief Print to the standard output with an interprocess lock to ensure coherency in the printed result.
/// @param fmt The format string, as would be used by 'printf'.
/// @param ... Arguments to be formatted, as would be used by 'printf'.
void bf_safe_printf(const char *fmt, ...);

#endif // BF_INTERPROCESS_H