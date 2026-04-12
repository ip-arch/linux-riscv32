// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int fd[2];
    pipe(fd);          /* fd[0]=読み取り端, fd[1]=書き込み端 */

    /* --- Child A: ps --- */
    if (fork() == 0) {
        dup2(fd[1], 1);        /* stdout をパイプ入口に差し替え */
        close(fd[0]);          /* 使わない端を閉じる */
        close(fd[1]);          /* dup2済みなので元のFDも閉じる */
        execl("/bin/ps", "ps", NULL);
    }

    /* --- Child B: grep --- */
    if (fork() == 0) {
        dup2(fd[0], 0);        /* stdin をパイプ出口に差し替え */
        close(fd[0]);
        close(fd[1]);
        execl("/bin/grep", "grep", "bash", NULL);
    }

    /* --- Parent: 両端を閉じてから待つ --- */
    close(fd[0]);   /* ← これがないと grep が EOF を受け取れない */
    close(fd[1]);
    wait(NULL);
    wait(NULL);
    return 0;
}
