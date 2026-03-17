// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

#define TASK_SIZE 0x9C800000

int main() {
  char *prevmap;
  
  printf("=== RISC-V 32-bit MMAP Over TASK_SIZE Demo ===\n\n");
  
  prevmap = (char *)mmap(NULL, TASK_SIZE+getpagesize(), PROT_WRITE|PROT_READ,
                      MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  
  if (prevmap == MAP_FAILED) {
    printf("failed for length = %010lx\n", (unsigned long)(TASK_SIZE+getpagesize()));
    perror("mmap failed");
  }
  
  printf("Mapped at %p \n", prevmap);
  
  return 0;
}

