// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

void timer_thread(union sigval sv) {
	printf("Timer expired! (id=%p)\n", sv.sival_ptr);
}

int main(void) {
	struct sigevent sev;
	struct itimerspec its;
	timer_t timerid;
	
	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = timer_thread;
	sev.sigev_notify_attributes = NULL;
	sev.sigev_value.sival_ptr = &timerid;
	
	timer_create(CLOCK_MONOTONIC, &sev, &timerid);
	
	its.it_value.tv_sec = 1;          /* 初回は1秒後 */
	its.it_value.tv_nsec = 0;
	its.it_interval.tv_sec = 0;       /* 周期0.5秒 */
	its.it_interval.tv_nsec = 500000000;
	timer_settime(timerid, 0, &its, NULL);
	
	while (1) pause();
	return 0;
}
