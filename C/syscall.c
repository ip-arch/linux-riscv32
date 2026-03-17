// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <asm/unistd.h>

int main(void)
{
	char *buf = "Hello World\n";
	int len = strlen(buf);
	register long a0 asm("a0");
	register long a1 asm("a1");
	register long a2 asm("a2");
	register long a7 asm("a7");
	a0 = STDOUT_FILENO;
       	a1 = (long)buf;
       	a2 = len;
       	a7 = __NR_write;             // SYS_write = 64
	
	__asm__ volatile (
	"ecall"
	: "=r"(a0)
	: "r"(a7), "r"(a0), "r"(a1), "r"(a2)
	: "memory"
	);
	
	printf("res = %ld\n", a0);
	return 0;
}
