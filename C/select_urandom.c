#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#include <string.h>

int main(void) {
    int urandom_fd;
    fd_set readfds;
    int max_fd;
    char stdin_buf[256];
    unsigned char random_buf[8];
    ssize_t n;

    // /dev/urandom を開く
    urandom_fd = open("/dev/urandom", O_RDONLY);
    if (urandom_fd < 0) {
        perror("open /dev/urandom");
        exit(EXIT_FAILURE);
    }

    max_fd = (STDIN_FILENO > urandom_fd) ? STDIN_FILENO : urandom_fd;

    printf("標準入力または /dev/urandom を監視中...\n");
    printf("標準入力に文字を入力するか、Ctrl+D で終了\n\n");

    while (1) {
        // fd_set を初期化
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(urandom_fd, &readfds);

        // select で両方のファイルディスクリプタを監視
        int ret = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        
        if (ret < 0) {
            if (errno == EINTR) {
                continue;  // シグナルで中断された場合は再試行
            }
            perror("select");
            break;
        }

        // 標準入力にデータが到着
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            n = read(STDIN_FILENO, stdin_buf, sizeof(stdin_buf) - 1);
            if (n < 0) {
                perror("read stdin");
                break;
            } else if (n == 0) {
                // EOF (Ctrl+D)
                printf("\n標準入力が閉じられました。終了します。\n");
                break;
            } else {
                stdin_buf[n] = '\0';
                printf("[標準入力] %s", stdin_buf);
            }
        }

        // /dev/urandom にデータが到着（常に読み取り可能）
        if (FD_ISSET(urandom_fd, &readfds)) {
            n = read(urandom_fd, random_buf, sizeof(random_buf));
            if (n < 0) {
                perror("read urandom");
                break;
            } else if (n > 0) {
                printf("[乱数] ");
                for (ssize_t i = 0; i < n; i++) {
                    printf("%02x ", random_buf[i]);
                }
                printf("\r");
            }
        }
    }

    close(urandom_fd);
    return 0;
}
