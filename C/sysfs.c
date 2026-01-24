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
        printf("Passed from EPOC:%dSec--> %dYears\n", value, value/((60*60/100)*24*36525));
    else
        printf("Failed to get value\n");

    fclose(fp);
    return 0;
}

