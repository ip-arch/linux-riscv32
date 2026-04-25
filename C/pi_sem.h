// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
// pi_sem.h  ── Priority Inheritance セマフォのラッパー
#ifndef PI_SEM_H
#define PI_SEM_H

#ifdef USE_PI_SEM
// Priority Inheritance版
#define pi_sem_open_or_create(name, ...)  _pi_sem_open_or_create(name)

#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

typedef struct {
    pthread_mutex_t mutex;
} pi_sem_t;

static inline int pi_sem_init(pi_sem_t *s) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    int ret = pthread_mutex_init(&s->mutex, &attr);
    pthread_mutexattr_destroy(&attr);
    return ret;
}

static inline pi_sem_t *_pi_sem_open_or_create(const char *name) {
    int created = 0;
    int fd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (fd >= 0) {
        created = 1;
        ftruncate(fd, sizeof(pi_sem_t));
    } else {
        fd = shm_open(name, O_RDWR, 0666);
	if (fd < 0) return NULL;
    }
    pi_sem_t *s = mmap(NULL, sizeof(pi_sem_t),
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED, fd, 0);
    close(fd);
    if (s == MAP_FAILED) return NULL;
    if (created) pi_sem_init(s);
    return s;
}

#define pi_sem_wait(s)    pthread_mutex_lock(&(s)->mutex)
#define pi_sem_post(s)    pthread_mutex_unlock(&(s)->mutex)
#define pi_sem_close(s)   munmap(s, sizeof(pi_sem_t))
#define pi_sem_unlink(name) shm_unlink(name)

#else
// 通常のPOSIXセマフォ版（sem_openと同じ引数で使える）
#include <semaphore.h>
typedef sem_t pi_sem_t;

/* GCC拡張(statement expression)を使用 */
#define pi_sem_open_or_create(name, ...) \
    ({ pi_sem_t *_s = (pi_sem_t*)sem_open(name, ##__VA_ARGS__); \
       (_s == (pi_sem_t*)SEM_FAILED) ? NULL : _s; })
#define pi_sem_wait(s)    sem_wait(s)
#define pi_sem_post(s)    sem_post(s)
#define pi_sem_close(s)   sem_close(s)
#define pi_sem_unlink(name) sem_unlink(name)
#endif

#endif
