// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
// cpu_hog.c
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    const char *name = argc > 1 ? argv[1] : "worker";
    long count = 0;
    struct timespec t1, t2;
    
    clock_gettime(CLOCK_MONOTONIC, &t1);
    
    while (1) {
        count++;
        clock_gettime(CLOCK_MONOTONIC, &t2);
        long elapsed = (t2.tv_sec - t1.tv_sec) * 1000
                     + (t2.tv_nsec - t1.tv_nsec) / 1000000;
        if (elapsed >= 5000) {  // 5秒ごとに報告
            printf("[%s] count=%ld elapsed=%ldms rate=%.1f/ms\n",
                   name, count, elapsed,
                   (double)count / elapsed);
            count = 0;
            clock_gettime(CLOCK_MONOTONIC, &t1);
        }
    }
}
