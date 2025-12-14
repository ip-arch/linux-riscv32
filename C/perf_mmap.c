#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <linux/perf_event.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define PAGE_SIZE 4096

// perf_event_open システムコールを呼び出すラッパー
static long perf_event_open(struct perf_event_attr *attr, pid_t pid, int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, attr, pid, cpu, group_fd, flags);
}

// =========================================================================

int main() {
    struct perf_event_attr attr;
    int fd_cycle;
    struct perf_event_mmap_page *pmu_page;
    uint64_t start_cycles, end_cycles;
    
    // --- 1. サイクルカウンタイベントの準備 ---
    memset(&attr, 0, sizeof(attr));
    attr.type = PERF_TYPE_HARDWARE;
    // PERF_COUNT_HW_CPU_CYCLES (CPUが実際に実行したサイクル数) を要求
    attr.config = PERF_COUNT_HW_CPU_CYCLES; 
    attr.size = sizeof(attr);
    attr.exclude_kernel = 1; // カーネルのサイクルはカウントしない

    // --- 2. perf_event_open の実行 ---
    // 現在のプロセス/CPUでイベントをオープン
    fd_cycle = perf_event_open(&attr, 0, -1, -1, 0); 
    if (fd_cycle < 0) {
        // PERF_COUNT_HW_CPU_CYCLES がハードウェアでサポートされていない場合の代替策が必要
        fprintf(stderr, "Error opening perf_event: %s. Hardware cycles not available or permissions denied.\n", strerror(errno));
        return 1;
    }
    
    // --- 3. mmap によるカウンタ領域のマッピング ---
    // 最初の1ページ(PAGE_SIZE)を mmap する
    pmu_page = mmap(NULL, PAGE_SIZE, PROT_READ, MAP_SHARED, fd_cycle, 0);
    if (pmu_page == MAP_FAILED) {
        fprintf(stderr, "Error mmapping perf page: %s\n", strerror(errno));
        close(fd_cycle);
        return 1;
    }

    // --- 4. 計測対象メモリの準備 (ページフォルト演習用) ---
    void *fault_addr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    void *nofault_addr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    *(volatile int *)nofault_addr = 0; // 物理ページを割り当てておく

    printf("--- RISC-V High-Resolution Cycle Counter (mmap) ---\n");
    
    // --- 5. 計測の実行: フォルトなし ---
    // mmapページから直接 time_cycles を読み出すことで、トラップを回避
    start_cycles = pmu_page->time_cycles;
    *(volatile int *)nofault_addr = 1;
    end_cycles = pmu_page->time_cycles;
    uint64_t duration_nofault = end_cycles - start_cycles;

    // --- 6. 計測の実行: フォルトあり ---
    start_cycles = pmu_page->time_cycles;
    *(volatile int *)fault_addr = 2; // ページフォルト発生
    end_cycles = pmu_page->time_cycles;
    uint64_t duration_fault = end_cycles - start_cycles;

    // --- 7. 結果の表示 ---
    printf("1. access without page faults: %llu cycles\n", duration_nofault);
    printf("2. access with a page fault: %llu cycles\n", duration_fault);
    
    if (duration_fault > duration_nofault) {
        printf("Difference (2 - 1): %llu cycles\n", duration_fault - duration_nofault);
    } else {
        printf("Difference is too small or negative. (Measurement noise/Optimization)\n");
    }

    // --- 8. 後処理 ---
    munmap(pmu_page, PAGE_SIZE);
    close(fd_cycle);
    munmap(fault_addr, PAGE_SIZE);
    munmap(nofault_addr, PAGE_SIZE);

    return 0;
}
