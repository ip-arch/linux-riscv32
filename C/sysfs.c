#include <stdio.h>
#include <stdlib.h>

int main(void) {
    FILE *fp = fopen("/sys/class/rtc/rtc0/since_epoch", "r");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    int value;
    if (fscanf(fp, "%d", &value) == 1)
        printf("EPOCからの経過値: %d\n", value);
    else
        printf("値の取得に失敗しました。\n");

    fclose(fp);
    return 0;
}

