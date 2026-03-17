// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>

void handler(int sig) {
	static int count = 0;
	printf("Timer expired: %d\n", ++count);
}

int main(void) {
	struct itimerval itv = {{1,0},{1,0}};
	signal(SIGALRM, handler);
	setitimer(ITIMER_REAL, &itv, NULL);
	while (1);
}

