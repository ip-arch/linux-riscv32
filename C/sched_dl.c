// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <linux/types.h>
#include <linux/sched.h>

void *run_deadline(void *arg) {
	struct sched_attr attr = {0};
	attr.size = sizeof(attr);
	attr.sched_policy = SCHED_DEADLINE;
	attr.sched_runtime  = 10 * 1000 * 1000;
	attr.sched_deadline = 30 * 1000 * 1000;
	attr.sched_period   = 30 * 1000 * 1000;
	
	if (sched_setattr(0, &attr, 0) < 0) {
		perror("sched_setattr");
		pthread_exit(NULL);
	}
	printf("deadline thread started [%ld]\n", gettid());
	while (1) { /* 周期タスク */ pthread_testcancel(); }
	return NULL;
}

int main(void) {
	pthread_t thread;
	pthread_create(&thread, NULL, run_deadline, NULL);
	sleep(3);
	pthread_cancel(thread);
	pthread_join(thread, NULL);
	return 0;
}
