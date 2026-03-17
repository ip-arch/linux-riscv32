// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <unistd.h>
#include <asm/unistd.h>

int main(void)
{
	char buf[10];
	long ret, len = 0;
	
	// read(0, buf, 10)
	register long a0 asm("a0") = 0;          // stdin
	register long a1 asm("a1") = (long)buf;  // buffer address
	register long a2 asm("a2") = 10;         // count
	register long a7 asm("a7") = __NR_read;  // syscall number for read
	
	__asm__ volatile("ecall" : "=r"(a0) : "r"(a7), "r"(a0), "r"(a1), "r"(a2));
	ret = a0; // read bytes
	
	if (ret > 0) {
		len = ret;
		// write(1, buf, len)
		a0 = 1;                 // stdout
		a2 = len;               // number of bytes to write
		a7 = __NR_write;        // syscall number for write
		__asm__ volatile("ecall" : "=r"(a0) : "r"(a7), "r"(a0), "r"(a1), "r"(a2));
	}
	
	return 0;
}

