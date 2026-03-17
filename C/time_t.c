// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <time.h>

int main(void) {
	time_t t = time(NULL);
	printf("time_t = %ld\n", (long)t);
	printf("ctime  = %s", ctime(&t));
	return 0;
}
