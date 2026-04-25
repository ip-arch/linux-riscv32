// posix_shm_init.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include "posix_shm.h"

int main() {
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(fd, sizeof(struct mybuf)) == -1) {
        perror("ftruncate");
        exit(1);
    }

    struct mybuf *mb = mmap(NULL, sizeof(*mb),
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (mb == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // ★ 初期化（ここが重要）
    mb->count = 0;
    mb->buf[0] = '\0';

    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    printf("Initialized shared memory & semaphore\n");
    return 0;
}
