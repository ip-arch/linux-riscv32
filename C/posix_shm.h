// posix_shm.h
#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

#define SEM_NAME   "/my_sem"
#define SHM_NAME   "/my_shm"

#define BUF_SIZE 4096

struct mybuf {
    size_t count;
    char buf[BUF_SIZE];
};

#endif
