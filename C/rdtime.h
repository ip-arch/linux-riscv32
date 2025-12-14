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
