// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <unistd.h>

void *rr_task(void *arg) {
	int id = (int)(long)arg;
	struct sched_param param = { .sched_priority = 60 };
	if (sched_setscheduler(0, SCHED_RR, &param) == -1) {
		perror("sched_setscheduler");
		return NULL;
	}
	for (int i=0; i<10; i++) {
		printf("[RR %d] running\n", id);
		usleep(100000);
	}
	return NULL;
}

int main(void) {
	pthread_t t1, t2;
	pthread_create(&t1, NULL, rr_task, (void*)1);
	pthread_create(&t2, NULL, rr_task, (void*)2);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	return 0;
}
