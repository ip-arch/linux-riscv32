// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

#define MADR (void*)0x10000000
#define OFS 0xfffff000
#define LEN 0xfffff001
#define LEN2 0x01000000

int main() {
  char *prevmap, *newmap;
  int res;
  
  printf("=== RISC-V 32-bit MMAP Overflow Demo ===\n\n");
  
  prevmap = (char *)mmap(MADR, LEN, PROT_WRITE|PROT_READ,
                      MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  
  if (prevmap == MAP_FAILED) {
    printf("mmap failed for length = 0x%08lx\n", LEN);
    perror("case1 mmap failed");
  }
  newmap = (char *)syscall(SYS_mmap2, MADR, LEN2, PROT_WRITE|PROT_READ,
                      MAP_PRIVATE|MAP_ANONYMOUS, -1, OFS);
  
  if (newmap == MAP_FAILED) {
    printf("mmap2 failed for pgofs, len = 0x%08lx, 0x%08lx\n", OFS, LEN2);
    perror("case2 mmap failed");
  }
  
  printf("Mapped at %p %p \n", prevmap, newmap);
  
  return 0;
}

