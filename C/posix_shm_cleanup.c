//posix_shm_cleanup.c
#include <stdio.h>
#include <semaphore.h>
#include <sys/mman.h>
#include "posix_shm.h"

int main() {
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);

    printf("Cleaned up resources\n");
    return 0;
}
