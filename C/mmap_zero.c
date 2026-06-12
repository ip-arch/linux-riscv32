// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>

// There are two methods to run this program without exception
//   1: set mmap_min_addr to 0 as: sudo sysctl -w vm.mmap_min_addr=0 
//   2: set CAP_SYS_RAWIO capability as: sudo setcap cap_sys_rawio+ep ./a.out

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
