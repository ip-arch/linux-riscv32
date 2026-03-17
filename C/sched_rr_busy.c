// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
// purebusy.c
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <sys/syscall.h>
#include <sys/types.h>

int main(void) {
    pid_t pid = getpid();
    unsigned long counter = 0;

    while (1) {
        counter++;
        if (counter % 100000000 == 0)
            printf("PID %d running\n", pid);
    }
    return 0;
}

