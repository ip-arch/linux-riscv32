// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	int tmp;

	char cmd[64];
	sprintf(cmd, "cat /proc/%d/maps | grep '\\[stack\\]'", getpid());
	printf("start\n");
	system(cmd);
	asm volatile ("li t0, -1000000\n"
			"add sp,sp,t0\n"
			"sw t0, 10(sp)\n" : : : "t0");
	printf("stack moved\n");
	// system(cmd);
	asm volatile ("li t0, 1000000\n"
			"lw %0, 10(sp)\n"
			"add sp,sp,t0\n" 
			: "=r"(tmp): : "t0");

	printf("worte %d\n", tmp);
	system(cmd);
	return 0;
}

