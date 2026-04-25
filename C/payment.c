// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
// payment.c
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <time.h>
#include "shared_lock.h"

#define SOCK_PATH "/tmp/payment.sock"


int main(int argc, char *argv[]) {
    int listen_sock, client_sock;
    struct sockaddr_un addr;
    char buf[1024];

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

    listen_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    unlink(SOCK_PATH);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCK_PATH);

    bind(listen_sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(listen_sock, 1);

    printf("Waiting for connection from epoll_server...\n");
    
    // 一度だけ接続を受け入れ、その接続を使い続ける（Keep-Alive）
    client_sock = accept(listen_sock, NULL, NULL);
    printf("epoll_server connected. Ready to process payments.\n");

    while (1) {
        int n = read(client_sock, buf, sizeof(buf) - 1);
        if (n <= 0) break; // エラーまたは切断で終了

        buf[n] = '\0';
        printf("[PAYMENT] Processing: %s", buf);
	// paymentがloggerとhealthの間でCPUを占有 → 古典的3プロセスインバージョン
	// （loggerはsem保持中だがpaymentに横取りされ解放できない）
	busy_wait_ms(2000);  // 2000ms CPUを独占→loggerが動けない
	printf("[PAYMENT] 処理完了\n");
    }

    close(client_sock);
    close(listen_sock);
    return 0;
}
