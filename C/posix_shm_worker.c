// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
// posix_shm_worker.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include "posix_shm.h"

int main() {
    sem_t *sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    struct mybuf *mb = mmap(NULL, sizeof(*mb),
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (mb == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // ===== クリティカルセクション =====
    sem_wait(sem);

    printf("[%d] before: %s\n", getpid(), mb->buf);

    snprintf(mb->buf, BUF_SIZE,
        "Hello from %d (count=%zu)", getpid(), mb->count);

    mb->count++;

    printf("[%d] wrote: %s\n", getpid(), mb->buf);

    sleep(1);

    sem_post(sem);
    // ===============================

    return 0;
}
