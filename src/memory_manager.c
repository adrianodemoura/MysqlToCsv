#include "memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>

size_t get_total_memory() {
    struct sysinfo si;
    if (sysinfo(&si) != 0) {
        fprintf(stderr, "sysinfo() failed\n");
        return 0;
    }
    return si.totalram * si.mem_unit;
}

size_t get_current_memory_usage() {
    FILE* status = fopen("/proc/self/status", "r");
    if (!status) {
        fprintf(stderr, "Failed to open /proc/self/status\n");
        return 0;
    }

    char line[256];
    size_t vm_rss = 0;

    while (fgets(line, sizeof(line), status)) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            sscanf(line, "VmRSS: %zu", &vm_rss);
            break;
        }
    }

    fclose(status);
    return vm_rss * 1024; // Convert KB to bytes
}

int check_memory_limits() {
    size_t total_mem = get_total_memory();
    size_t current_usage = get_current_memory_usage();
    
    if (total_mem == 0) return 0;
    
    double usage_ratio = (double)current_usage / total_mem;
    return usage_ratio <= 0.7; // 70% limit
}

int set_memory_limits(size_t max_memory) {
    struct rlimit rlim;
    
    // Set soft limit
    rlim.rlim_cur = max_memory;
    rlim.rlim_max = max_memory;
    
    if (setrlimit(RLIMIT_AS, &rlim) != 0) {
        fprintf(stderr, "setrlimit() failed\n");
        return 0;
    }
    
    return 1;
} 