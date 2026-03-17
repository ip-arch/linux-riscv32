// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    struct timeval tv;
    
    // 現在時刻を取得
    if (gettimeofday(&tv, NULL) < 0) {
        perror("gettimeofday");
        return 1;
    }

    // 5秒引く
    tv.tv_sec -= 5;

    // システム時刻を設定（要：root権限）
    if (settimeofday(&tv, NULL) < 0) {
        perror("settimeofday (sudoが必要かもしれません)");
        return 1;
    }

    printf("Time stepped back 5 seconds.\n");
    return 0;
}
