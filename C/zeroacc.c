// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <sys/mman.h>

int main(int argc, char *argv[]) {
	int *p, i;
	printf("main is @%10p\n", main);
	for(i=0x100000; i>0; i-=0x1000) {
		p = (int*)mmap((void*)i, 0x1000,
				PROT_READ|PROT_WRITE|PROT_EXEC,
				MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);
		printf("check @ %010p\n", p);
		*p=i;
		printf("@0=%x\n", *p);
		fflush(stdout);
	}
	return 0;
}
