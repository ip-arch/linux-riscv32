// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
// shared_lock.h  ── 3プロセス共通
#ifndef SHARED_LOCK_H
#define SHARED_LOCK_H
#include <stdlib.h>
#include <semaphore.h>
#define SEM_NAME "/health_payment_log_sem"
#define SHARED_LOG "/tmp/shared_log"
// shared_lock.h に追加
#include "pi_sem.h"

// ビジーループでCPU占有
static void busy_wait_ms(long ms) {
    struct timespec t1, t2;
    clock_gettime(CLOCK_MONOTONIC, &t1);
    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &t2);
        long elapsed = (t2.tv_sec  - t1.tv_sec)  * 1000
                     + (t2.tv_nsec - t1.tv_nsec) / 1000000;
        if (elapsed >= ms) break;
    }
}
#endif
