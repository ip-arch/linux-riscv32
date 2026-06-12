// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <sys/mman.h>

int main(int argc, char *argv[]) {

int *p;

p=mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0);
if((long)p&0xFFF) {
	perror("mmap ERROR");
	return -1;
}
*p =1;
printf("*p=%d\n", *p);
return 0;
}
