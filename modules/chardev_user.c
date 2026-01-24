#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#define DEVICE_PATH "/dev/mychardev"
#define TEST_SIZE 1024 // 1ページ分
#define ITERATIONS 1000 // 計測回数

// RISC-V: rdtime命令を読み出すアセンブリ関数
// rdtimeはコアのサイクル数を読み出すCSRです
static inline uint64_t get_rdtime(void) {
    uint32_t lo, hi, tmp;
    __asm__ __volatile__ (
        "1: \n"
        "  rdtimeh %0 \n"      // Read high 32 bits
        "  rdtime  %1 \n"      // Read low 32 bits  
        "  rdtimeh %2 \n"      // Read high again
        "  bne %0, %2, 1b \n"  // If high changed, retry
        : "=r" (hi), "=r" (lo), "=r" (tmp)
    );
    return ((uint64_t)hi << 32) | lo;
}


// --------------------------------------------------
// 1. データフローテスト関数
// --------------------------------------------------
void run_data_flow_test(int fd, char *mmap_ptr) {
    const char *test_data = "Hello, mmap and read/write test!";
    char read_buffer[TEST_SIZE] = {0};
    size_t len = strlen(test_data);

    printf("--- Mutual access confirmation ---\n");
    
    // (1) write() で書き込み、mmap で読み込み
    // ------------------------------------
    lseek(fd, 0, SEEK_SET);
    write(fd, test_data, len);
    
    // mmap領域から読み込み
    memcpy(read_buffer, mmap_ptr, len);
    if (strncmp(read_buffer, test_data, len) == 0) {
        printf("[Success] write() -> mmap: OK (%s)\n", read_buffer);
    } else {
        printf("[Fail] write() -> mmap: Unmatch against expect\n");
    }

    // (2) mmap で書き込み、read() で読み込み
    // ------------------------------------
    const char *new_data = "This data is written via mmap!";
    len = strlen(new_data);
    
    // mmap領域に書き込み
    memcpy(mmap_ptr, new_data, len);

    // read()で読み込み
    lseek(fd, 0, SEEK_SET);
    read(fd, read_buffer, len);

    if (strncmp(read_buffer, new_data, len) == 0) {
        printf("[Success] mmap -> read(): OK (%s)\n", read_buffer);
    } else {
        printf("[Failed] mmap -> read(): Unmatch against expect\n");
    }
    printf("--------------------------\n\n");
}


// --------------------------------------------------
// 2. アクセス時間計測関数
// --------------------------------------------------
void run_timing_test(int fd, char *mmap_ptr) {
    uint64_t      start_time, end_time;
    unsigned long write_total = 0;
    unsigned long mmap_write_total = 0;
    unsigned long read_total = 0;
    unsigned long mmap_read_total = 0;
    int i;
    char temp_data[TEST_SIZE];

    printf("--- Access Time Measure (rdtime) ---\n");
    printf("Num of measure: %d, Data size: %d byte\n", ITERATIONS, TEST_SIZE);
    
    // Warm-up: 最初の数回は計測しない
    for (i = 0; i < 10; i++) {
        lseek(fd, 0, SEEK_SET);
        write(fd, temp_data, TEST_SIZE);
        memcpy(mmap_ptr, temp_data, TEST_SIZE);
    }
    // ============ write() の時間計測 ============
    for (i = 0; i < ITERATIONS; i++) {
        lseek(fd, 0, SEEK_SET);
        start_time = get_rdtime();
        write(fd, temp_data, TEST_SIZE);
        end_time = get_rdtime();
        write_total += (end_time - start_time);
    }
    printf("1. write() Average access time: %lu cycle\n", write_total / ITERATIONS);

    // ============ mmap 書き込みの時間計測 ============
    for (i = 0; i < ITERATIONS; i++) {
        start_time = get_rdtime();
        memcpy(mmap_ptr, temp_data, TEST_SIZE);
        end_time = get_rdtime();
        mmap_write_total += (end_time - start_time);
    }
    printf("2. mmap (memcpy) Write Average access time %lu cycle\n", mmap_write_total / ITERATIONS);
    
    // ============ read() の時間計測 ============
    for (i = 0; i < ITERATIONS; i++) {
        lseek(fd, 0, SEEK_SET);
        start_time = get_rdtime();
        read(fd, temp_data, TEST_SIZE);
        end_time = get_rdtime();
        read_total += (end_time - start_time);
    }
    printf("3. read() Average Access time: %lu cycle\n", read_total / ITERATIONS);

    // ============ mmap 読み込みの時間計測 ============
    for (i = 0; i < ITERATIONS; i++) {
        start_time = get_rdtime();
        memcpy(temp_data, mmap_ptr, TEST_SIZE);
        end_time = get_rdtime();
        mmap_read_total += (end_time - start_time);
    }
    printf("4. mmap (memcpy) Read Average Access Time: %lu Cycle\n", mmap_read_total / ITERATIONS);
    printf("-----------------------------------------\n");

    // 結論
    if (mmap_write_total < write_total && mmap_read_total < read_total) {
        printf("\n=> Result: mmap time is much faster than read()/write() \n");
    } else {
        printf("\n=> Result: mmap time read()/write() are not so different or inverted\n");
    }
}


int main() {
    int fd;
    char *mmap_ptr;

    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return EXIT_FAILURE;
    }

    // mmap マッピング
    mmap_ptr = (char *)mmap(NULL, 
                            TEST_SIZE, 
                            PROT_READ | PROT_WRITE, 
                            MAP_SHARED, 
                            fd, 
                            0);
    if (mmap_ptr == MAP_FAILED) {
        perror("Failed to mmap the device");
        close(fd);
        return EXIT_FAILURE;
    }
    
    // 1. データフローテストの実行
    run_data_flow_test(fd, mmap_ptr);

    // 2. 時間計測テストの実行
    run_timing_test(fd, mmap_ptr);

    // 解放
    munmap(mmap_ptr, TEST_SIZE);
    close(fd);

    return EXIT_SUCCESS;
}

