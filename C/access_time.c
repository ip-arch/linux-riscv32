// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#define PAGE_SIZE (4096)
#define REPEAT    1000

// Read 64-bit time counter on RV32
static inline uint64_t read_time_rv32(void) {
    uint32_t lo, hi, tmp;
    __asm__ __volatile__ (
        "1: \n"
        "  rdtimeh %0 \n"      // Read high 32 bits
        "  rdtime  %1 \n"      // Read low 32 bits  
        "  rdtimeh %2 \n"      // Read high again
        "  bne %0, %2, 1b \n"  // If high changed, retry
        : "=r" (hi), "=r" (lo), "=r" (tmp)
    );
    return ((uint64_t)hi << 32) | lo;
}

void measure_access_time(void *addr, uint64_t *duration) {
    uint64_t start, end;
    volatile int value;
    
    start = read_time_rv32();
    __asm__ __volatile__ (
        "lw %0, 0(%1) \n"
        : "=r" (value)
        : "r" (addr)
        : "memory"
    );
    end = read_time_rv32();
    
    *duration = (end > start) ? (end - start) : 0;
}

int main() {
    uint64_t duration;
    uint64_t total_nofault = 0;
    int valid_count = 0;
    
    printf("--- RISC-V32 Memory Access Time Measurement ---\n");
    
    // Test timer reads
    uint64_t t1 = read_time_rv32();
    uint64_t t2 = read_time_rv32();
    printf("Timer test: t1=%llu, t2=%llu, diff=%llu\n", t1, t2, t2-t1);
    
    // 1. No-fault measurement
    void *nofault_addr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (nofault_addr == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }
    
    // Pre-fault
    *(volatile int *)nofault_addr = 42;
    
    // Warmup
    for (int i = 0; i < 100; i++) {
        volatile int x = *(volatile int *)nofault_addr;
        (void)x;
    }
    
    // Measure
    for (int i = 0; i < REPEAT; i++) {
        measure_access_time(nofault_addr, &duration);
        
        if (duration > 0 && duration < 10000) {
            total_nofault += duration;
            valid_count++;
        }
    }
    
    uint64_t avg_nofault = valid_count > 0 ? (total_nofault / valid_count) : 0;
    printf("1. No page fault (avg of %d): %llu ticks\n", valid_count, avg_nofault);
    
    // 2. Page fault measurement
    void *fault_addr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (fault_addr == MAP_FAILED) {
        perror("mmap fault_addr failed");
        return 1;
    }
    
    madvise(fault_addr, PAGE_SIZE, MADV_DONTNEED);
    
    measure_access_time(fault_addr, &duration);
    
    printf("2. With page fault: %llu ticks\n", duration);
    
    if (duration > avg_nofault && duration < 1000000000ULL) {
        printf("Difference: %llu ticks\n", duration - avg_nofault);
    } else {
        printf("WARNING: Unexpected timing (possible measurement error)\n");
    }
    
    munmap(nofault_addr, PAGE_SIZE);
    munmap(fault_addr, PAGE_SIZE);
    
    return 0;
}
