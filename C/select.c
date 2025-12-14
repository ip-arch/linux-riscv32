#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#define FIFO_PATH "/tmp/testpipe"

int main(void) {
    int fifo_rd, fifo_wr;
    fd_set readfds;
    int max_fd;
    char stdin_buf[256];
    char fifo_buf[256];
    ssize_t n;

    // FIFO が存在しない場合は作成
    if (access(FIFO_PATH, F_OK) != 0) {
        if (mkfifo(FIFO_PATH, 0666) < 0) {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
        printf("FIFO %s を作成しました\n", FIFO_PATH);
    }

    // 重要: 読み取り用を先に開く（NONBLOCKで）
    fifo_rd = open(FIFO_PATH, O_RDONLY | O_NONBLOCK);
    if (fifo_rd < 0) {
        perror("open fifo for read");
        exit(EXIT_FAILURE);
    }
    
    // 次に書き込み用を開く（これでEOFを防ぐ）
    fifo_wr = open(FIFO_PATH, O_WRONLY | O_NONBLOCK);
    if (fifo_wr < 0) {
        perror("open fifo for write");
        close(fifo_rd);
        exit(EXIT_FAILURE);
    }

    max_fd = (STDIN_FILENO > fifo_rd) ? STDIN_FILENO : fifo_rd;

    printf("標準入力と FIFO (%s) を監視中...\n", FIFO_PATH);
    printf("- 標準入力: 文字を入力してください\n");
    printf("- FIFO: 別ターミナルで echo \"メッセージ\" > %s\n", FIFO_PATH);
    printf("- 終了: Ctrl+D\n\n");

    while (1) {
        // fd_set を初期化
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(fifo_rd, &readfds);

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

        // FIFO にデータが到着
        if (FD_ISSET(fifo_rd, &readfds)) {
            n = read(fifo_rd, fifo_buf, sizeof(fifo_buf) - 1);
            if (n < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // データなし（通常起こらない）
                    continue;
                }
                perror("read fifo");
                break;
            } else if (n == 0) {
                // これは起こらないはず（自分でwrite側を開いているため）
                continue;
            } else {
                // データ受信成功
                fifo_buf[n] = '\0';
                printf("[FIFO] %s", fifo_buf);
            }
        }
    }

    close(fifo_rd);
    close(fifo_wr);
    unlink(FIFO_PATH);  // 終了時に FIFO を削除
    return 0;
}
