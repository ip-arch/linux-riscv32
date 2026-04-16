// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
// server_unix.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/wait.h>

void handle_client(int clientfd) {
    char buf[1024];
    int pid;

    int n=read(clientfd, buf, sizeof(buf));
    buf[n] = '\0';
    printf("PID %d got: %s\n", getpid(), buf);

    char body[256];
    snprintf(body, sizeof(body),
             "Hello! UNIX socket, Served by PID: %d\n", getpid());

    char response[512];
    snprintf(response, sizeof(response),
             "%s", body);

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
    struct sockaddr_un addr;

    signal(SIGCHLD, reap);

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "/tmp/my_socket");

    unlink("/tmp/my_socket");

    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 5);

    printf("Listening on /tmp/my_socket...\n");

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
