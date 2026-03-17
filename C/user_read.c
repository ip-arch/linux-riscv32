// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
int main () {
   int *p = (int*)0x10000000;
   printf("%d\n", *p);
   return 0;
}
