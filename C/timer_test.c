// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

void* test_timer(void* arg) {
    time_t now=time(NULL);
    clockid_t clk_id = (clockid_t)(long)arg;
    struct timespec ts;
    char *name = (clk_id == CLOCK_REALTIME) ? "REALTIME " : "MONOTONIC";

    // 現在時刻を取得して 10秒後を計算
    clock_gettime(clk_id, &ts);
    ts.tv_sec += 10;

    printf("[%s] Start: Target at +10s@%s\n", name,ctime(&now));
    
    // 絶対時刻 (TIMER_ABSTIME) でスリープ
    // これにより時刻変更の影響がダイレクトに現れる
    if (clock_nanosleep(clk_id, TIMER_ABSTIME, &ts, NULL) != 0) {
        perror("clock_nanosleep");
    }

    now=time(NULL);
    printf("[%s] Woke up!%s\n", name, ctime(&now));
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, test_timer, (void*)CLOCK_REALTIME);
    pthread_create(&t2, NULL, test_timer, (void*)CLOCK_MONOTONIC);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
