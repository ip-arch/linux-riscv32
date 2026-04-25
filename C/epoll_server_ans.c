// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

#define MAX_EVENTS 10
#define PAYMENT_SOCK "/tmp/payment.sock"
#define HEALTH_SOCK "/tmp/health.sock"
#define LOG_FIFO "/tmp/logpipe"

/* logger.c/health.c への接続を保持するグローバル変数 */
int logger_fd = -1;
int health_fd = -1;

/* payment.c への接続を保持するグローバル変数 */
int payment_fd = -1;

// payment プロセスへの接続初期化
void init_payment_connection() {
    payment_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, PAYMENT_SOCK);

    if (connect(payment_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("payment.c への接続に失敗しました (先に payment を起動してください)");
        exit(1);
    }
}

// logger プロセスへの接続初期化
void init_log_fifo(void) {
	/* FIFOはopen()だけで接続できる。
       ソケットの場合は何が必要か？ payment を参考にせよ */
	logger_fd = open(LOG_FIFO, O_WRONLY);
	if (logger_fd < 0) {
		perror("open logpipe");
                exit(1);
        }
}

// health プロセスへの接続初期化
void init_health_connection() {
    health_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, HEALTH_SOCK);
    if (connect(health_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("health への接続失敗");
        exit(1);
    }
}

int main() {
    int listenfd, epfd;
    struct sockaddr_in serv_addr;
    struct epoll_event ev, events[MAX_EVENTS];

    // 1. 各サブプロセスとの接続（このサンプルでは起動済みを前提とする）
    init_log_fifo();
    init_health_connection();
    init_payment_connection();

    // 2. 待ち受けソケットの作成
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(18080);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    /* エラー処理は省略（実装時は必ず確認すること） */
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 5);

    // 3. epoll の初期化と登録
    epfd = epoll_create1(0);
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

    printf("Server started on port 18080...\n");

    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == listenfd) {
                // 新規クライアントの接続受付
                int connfd = accept(listenfd, NULL, NULL);
                ev.events = EPOLLIN;
                ev.data.fd = connfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
                dprintf(logger_fd, "New client(%d) connected\n", connfd);
            } else {
                // 既存クライアントからのデータ受信
                int fd = events[i].data.fd;
                char buf[1024] = {0};
                int n = read(fd, buf, sizeof(buf));

                if (n <= 0) {
                    // 切断処理
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    dprintf(logger_fd, "Client(%d) disconnected\n", fd);
                } else {
			char cmd = buf[0];
			switch (cmd) {
			case 'P':
                            // loggerを先に起床させてセマフォを取得させる（インバージョン誘発のため）
			    dprintf(logger_fd, "[%d]PAYING %.*s", fd, n-1, buf+1); // Payment 過程をloggerに登録
			    // 受信データを payment プロセスへ横流し
			    write(payment_fd, buf+1, n-1);
			    // クライアントへ応答
			    write(fd, "ACK payment\n", 13);
			    break;
			 case 'L':
			    // loggerへ
			    dprintf(logger_fd, "[%d]LOGGING %.*s", fd, n-1, buf+1); // Logging 過程をloggerに登録
			    write(fd, "ACK: logger\n", 12);
			    break;

			case 'H':
			    // healthへ
			    write(health_fd, buf+1, n-1);
			    dprintf(logger_fd, "[%d]HEALTH %.*s", fd, n-1, buf+1); // loggerに登録
			    write(fd, "ACK: health\n", 12);
			    break;
			default:
			    dprintf(fd, "[%d]ERR: unknown\n", fd);
			    break;
		    }
                }
            }
        }
    }
    return 0;
}
