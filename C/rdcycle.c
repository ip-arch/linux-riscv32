#include <stdio.h>

int main() {
    unsigned long cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    printf("Current cycle count: %lu\n", cycles);
    return 0;
}
