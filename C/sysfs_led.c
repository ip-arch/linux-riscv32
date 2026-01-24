#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define LED_PATH "/sys/class/leds/led0/brightness"
#define BLINK_INTERVAL 1  // 秒

// プログラム終了フラグ
volatile sig_atomic_t running = 1;

// シグナルハンドラ（Ctrl+C で終了）
void signal_handler(int signum) {
    running = 0;
}

// LEDの明るさを設定する関数
int set_led_brightness(const char *path, int value) {
    int fd;
    char buf[16];
    ssize_t len;
    
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("open LED brightness");
        return -1;
    }
    
    // 値を文字列に変換（0 = OFF, 1 = ON, または0-255の範囲）
    len = snprintf(buf, sizeof(buf), "%d\n", value);
    
    if (write(fd, buf, len) < 0) {
        perror("write LED brightness");
        close(fd);
        return -1;
    }
    
    close(fd);
    return 0;
}

int main(void) {
    int state = 0;  // 0 = OFF, 1 = ON
    
    // シグナルハンドラを設定（Ctrl+C で終了）
    signal(SIGINT, signal_handler);
    
    printf("LED点滅プログラムを開始します\n");
    printf("LED: %s\n", LED_PATH);
    printf("間隔: %d秒\n", BLINK_INTERVAL);
    printf("Ctrl+C で終了します\n\n");
    
    // LEDデバイスが存在するか確認
    if (access(LED_PATH, W_OK) != 0) {
        fprintf(stderr, "エラー: %s にアクセスできません\n", LED_PATH);
        fprintf(stderr, "原因: ");
        if (errno == ENOENT) {
            fprintf(stderr, "ファイルが存在しません\n");
        } else if (errno == EACCES) {
            fprintf(stderr, "書き込み権限がありません（sudo が必要かもしれません）\n");
        } else {
            perror("");
        }
        fprintf(stderr, "\n注: このプログラムは Raspberry Pi などの実機でのみ動作します\n");
        return EXIT_FAILURE;
    }
    
    // 最初にLEDを消灯
    if (set_led_brightness(LED_PATH, 0) < 0) {
        return EXIT_FAILURE;
    }
    
    // 点滅ループ
    while (running) {
        // 状態を反転
        state = !state;
        
        printf("LED: %s\n", state ? "ON" : "OFF");
        
        // LEDの明るさを設定
        if (set_led_brightness(LED_PATH, state) < 0) {
            break;
        }
        
        // 指定秒数待機
        sleep(BLINK_INTERVAL);
    }
    
    // 終了時にLEDを消灯
    printf("\nプログラムを終了します。LEDを消灯します。\n");
    set_led_brightness(LED_PATH, 0);
    
    return EXIT_SUCCESS;
}

