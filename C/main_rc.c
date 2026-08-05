// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "read_counter.h"
int foo(int arg);

int main(int argc, char **argv) {
	long long st, et;
	st=read_freerun_counter();;
	if(argc > 1)
	  usleep(atoi(argv[1]));
  printf("Hello World\n");
	et=read_freerun_counter();;
	printf("runtime = %lld ticks = %.6fs\n", et-st,(double)(et-st)/read_freerun_freq());
  return 0;
}
