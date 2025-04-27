#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>

// Get total system memory in bytes
size_t get_total_memory();

// Get current memory usage in bytes
size_t get_current_memory_usage();

// Check if memory usage is within limits
int check_memory_limits();

// Set memory limits for the process
int set_memory_limits(size_t max_memory);

#endif // MEMORY_MANAGER_H 