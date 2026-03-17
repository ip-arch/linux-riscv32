// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <unistd.h>

int main() {
	void *end1, *end2;
	int *p;
	end1 = sbrk(0);
	sbrk(0x2000);
	end2 = sbrk(0);
	p = (int *)((long)end1 + 0x1000);
	*p = 1;
	printf("end1=%p, end2=%p, p=%p, %d\n", end1, end2, p, *p);
	return 0;
}
