// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
// logger.c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include "shared_lock.h"

#define LOG_FIFO "/tmp/logpipe"

int main(int argc, char *argv[]) {
    char buf[256];

    unlink(LOG_FIFO);
    mkfifo(LOG_FIFO, 0666);

    // セマフォ初期化（loggerが作成）
    pi_sem_unlink(SEM_NAME);
    pi_sem_t *sem = pi_sem_open_or_create(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == NULL) {
	    perror("sem_open");
	    fprintf(stderr,":%s\n",argv[0]);
	    exit(1);
    }

    int fd = open(LOG_FIFO, O_RDONLY);
    int logfd = open(SHARED_LOG, O_WRONLY|O_CREAT|O_APPEND, 0644);
    if(logfd < 0) {
	    perror("logfd");
	    exit(1);
    }

    while (1) {
	    static int cnt  = 0;
	    // FIFOを確認
	    struct pollfd pfd = {fd, POLLIN, 0};
	    if (poll(&pfd, 1, 0) > 0) {  // ノンブロッキング
		    // セマフォ取得（ここで長時間保持 → インバージョンの原因）
		    pi_sem_wait(sem);
		    int n = read(fd, buf, sizeof(buf)-1);
		    if (n > 0) {
			    char tbuf[32];
			    time_t now = time(NULL);
			    struct tm *tm = localtime(&now);
			    strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", tm);
			    buf[n] = '\0';
			    if (buf[n-1] == '\n') buf[n-1] = '\0';
			    dprintf(logfd, "%s [LOG sem=%d] %s\n", tbuf, ++cnt, buf);
                            printf("[LOG] %s", buf);
                    }
		    pi_sem_post(sem);
	    }
    }
}
