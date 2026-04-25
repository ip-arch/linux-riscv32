// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
// health.c
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>
#include <sys/epoll.h>
#include <time.h>
#include <sys/timerfd.h>
#include <fcntl.h>
#include "shared_lock.h"

#define HEALTH_SOCK "/tmp/health.sock"

int running = 0;

int main(int argc, char *argv[]) {
    int sock, client;
    struct sockaddr_un addr;
    char buf[128];

    pi_sem_t *sem = NULL;
    for (int i = 0; i< 20; i++) {
	    sem = pi_sem_open_or_create(SEM_NAME, 0);  // loggerが作成済みを開く
	    if (sem != NULL) break;
	    usleep(1000);
    }
    if (sem == NULL) {
	    perror("sem_open");
	    fprintf(stderr,":%s\n",argv[0]);
	    exit(1);
    }

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    unlink(HEALTH_SOCK);
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, HEALTH_SOCK);
    bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(sock, 5);
    printf("health waiting...\n");

    // timerfd で周期を管理し、epoll でソケットと統合
    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    struct itimerspec its = {
        .it_interval = {0, 100000000},  // 100ms周期
        .it_value    = {0, 100000000}
    };
    timerfd_settime(tfd, 0, &its, NULL);
    client = accept(sock, NULL, NULL);

    int epfd = epoll_create1(0);

    // クライアントとタイマー両方を登録
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = tfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, tfd, &ev);
    ev.data.fd = client;
    epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev);

    int logfd = open(SHARED_LOG, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if(logfd < 0) {
	    perror("logfd");
	    exit(1);
    }
    struct epoll_event evs[2];
    while (1) {
    int n = epoll_wait(epfd, evs, 2, -1); // 無期限待機
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char tbuf[32];
    strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", tm);
    for (int i = 0; i < n; i++) {
        if (evs[i].data.fd == tfd) {
        // タイマー満了 → 周期処理
            uint64_t exp;
            read(tfd, &exp, sizeof(exp));
            if (running) {
		    struct timespec t1, t2;
		    clock_gettime(CLOCK_MONOTONIC, &t1);
		    pi_sem_wait(sem);  // ← loggerが保持中ならここでブロック
		    // healthチェックコードがここに入る
		    dprintf(logfd, "%s [HEALTH] Health check %ld.%03lds\n", tbuf, t1.tv_sec%1000,t1.tv_nsec/1000000);
		    clock_gettime(CLOCK_MONOTONIC, &t2);
		    long delay_ms = (t2.tv_sec - t1.tv_sec)*1000
			    + (t2.tv_nsec - t1.tv_nsec)/1000000;
		    if(delay_ms > 10) {
			    printf("[HEALTH] health check（待ち時間: %ld ms）\n", delay_ms);
			    dprintf(logfd, "%s [HEALTH] health check（待ち時間: %ld ms）\n", tbuf, delay_ms);
		    }
		    pi_sem_post(sem);
	    }
        } else if (evs[i].data.fd == client) {
        // ソケット受信 → STOP/START処理
            int rn = read(client, buf, sizeof(buf)-1);
            if (rn > 0) {
            buf[rn] = '\0';
	    pi_sem_wait(sem);
            if (strstr(buf, "STOP")) {
                running = 0;
                printf("health stopped\n");
                dprintf(logfd, "%s [HEALTH] health stopped\n", tbuf);
            } else if (strstr(buf, "START")) {
                running = 1;
                printf("health started\n");
                dprintf(logfd, "%s [HEALTH] health started\n", tbuf);
                }
	    pi_sem_post(sem);
            }
        }
     }
   }
   close(client);
}
