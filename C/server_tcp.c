// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
// server_tcp.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

void handle_client(int clientfd) {
    char buf[1024];
    int pid;

    int n=read(clientfd, buf, sizeof(buf));
    buf[n] = '\0';
    printf("PID %d handling request:\n%s\n", getpid(), buf);

    // レスポンス生成（PIDを埋め込む）
    char body[256];
    snprintf(body, sizeof(body),
             "Hello! Served by PID: %d\n", getpid());

    char response[512];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain\r\n"
             "\r\n"
             "%s",
             body);

    write(clientfd, response, strlen(response));
    if ((pid = fork()) == 0) {
	    dup2(clientfd, STDOUT_FILENO);
	    close(clientfd);
	    execlp("date", "date", NULL);
	    exit(1);
	}
    waitpid(pid, NULL, 0);
    close(clientfd);
}

void reap(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    int sockfd, clientfd;
    struct sockaddr_in addr;

    signal(SIGCHLD, reap);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(18080);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 5);

    printf("Listening on port 18080...\n");

    while (1) {
        clientfd = accept(sockfd, NULL, NULL);

        if (fork() == 0) {
            close(sockfd);
            handle_client(clientfd);
            exit(0);
        } else {
            close(clientfd);
        }
    }
}
