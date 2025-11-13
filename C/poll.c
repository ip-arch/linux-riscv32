#include <stdio.h>
#include <poll.h>
#include <unistd.h>

int main(void) {
    struct pollfd fds[1];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    printf("3秒以内に入力してください: ");
    fflush(stdout);

    int ret = poll(fds, 1, 3000); // 3秒待つ
    if (ret == 0) {
        printf("\nタイムアウトしました。\n");
    } else if (ret > 0) {
        char buf[128];
        int  len;
        len=read(STDIN_FILENO, buf, sizeof(buf));
        buf[len]=0;
        printf("\n入力を受け取りました。:%s:\n",buf);
    } else {
        perror("poll");
    }
    return 0;
}

