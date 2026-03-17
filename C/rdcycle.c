// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>

int main() {
    unsigned long cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    printf("Current cycle count: %lu\n", cycles);
    return 0;
}
