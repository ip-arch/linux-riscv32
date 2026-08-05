// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdint.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/* read_freerun_counter() : フリーランカウンタの現在値を返す           */
/* ------------------------------------------------------------------ */

#if defined(__x86_64__) || defined(__i386__)

static inline uint64_t read_freerun_counter(void)
{
    uint32_t lo, hi;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

#elif defined(__riscv)

static inline uint64_t read_freerun_counter(void)
{
#if __riscv_xlen == 64
    uint64_t val;
    __asm__ __volatile__("rdtime %0" : "=r"(val));
    return val;
#else
    uint32_t lo, hi, hi2;
    do {
        __asm__ __volatile__("rdtimeh %0" : "=r"(hi));
        __asm__ __volatile__("rdtime  %0" : "=r"(lo));
        __asm__ __volatile__("rdtimeh %0" : "=r"(hi2));
    } while (hi != hi2);
    return ((uint64_t)hi << 32) | lo;
#endif
}

#elif defined(__aarch64__)

static inline uint64_t read_freerun_counter(void)
{
    uint64_t val;
    __asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(val));
    return val;
}

#elif defined(__arm__)

static inline uint64_t read_freerun_counter(void)
{
    uint32_t lo, hi;
    __asm__ __volatile__("mrrc p15, 1, %0, %1, c14" : "=r"(lo), "=r"(hi));
    return ((uint64_t)hi << 32) | lo;
}

#else
#error "unsupported architecture"
#endif


/* ------------------------------------------------------------------ */
/* read_freerun_freq()   : 上記カウンタの刻む周波数(Hz)を返す          */
/* ------------------------------------------------------------------ */

#if defined(__x86_64__) || defined(__i386__)
#include <cpuid.h>

/* CPUID leaf 0x15が使える世代ならそれを使う。
 * 使えない/ECX=0を返すCPU(古い世代、あるいはQEMU TCGなど)では
 * clock_gettime(CLOCK_MONOTONIC)を基準にした実測キャリブレーションに
 * フォールバックする。結果はプロセス内で1回だけ計算しキャッシュする。 */
static inline uint64_t read_freerun_freq(void)
{
    static uint64_t cached_freq = 0;

    if (cached_freq != 0)
        return cached_freq;

    if (__get_cpuid_max(0, NULL) >= 0x15) {
        uint32_t eax, ebx, ecx, edx;
        __cpuid(0x15, eax, ebx, ecx, edx);
        if (eax != 0 && ebx != 0 && ecx != 0) {
            cached_freq = (uint64_t)ecx * (uint64_t)ebx / (uint64_t)eax;
            return cached_freq;
        }
    }

    /* フォールバック: 10ms間隔でTSCとCLOCK_MONOTONICを比較して概算 */
    {
        struct timespec t0, t1;
        uint64_t tsc0, tsc1;

        clock_gettime(CLOCK_MONOTONIC, &t0);
        tsc0 = read_freerun_counter();

        do {
            clock_gettime(CLOCK_MONOTONIC, &t1);
        } while ((t1.tv_sec - t0.tv_sec) * 1000000000LL +
                 (t1.tv_nsec - t0.tv_nsec) < 10 * 1000 * 1000 /* 10ms */);

        tsc1 = read_freerun_counter();

        int64_t elapsed_ns = (t1.tv_sec - t0.tv_sec) * 1000000000LL +
                              (t1.tv_nsec - t0.tv_nsec);
        cached_freq = (uint64_t)((tsc1 - tsc0) * 1000000000ULL / (uint64_t)elapsed_ns);
    }
    return cached_freq;
}

#elif defined(__riscv)
#include <stdio.h>
#include <sys/auxv.h>

static inline uint64_t read_freerun_freq(void)
{
    static uint64_t cached_freq = 0;

    if (cached_freq != 0)
        return cached_freq;

    /* device treeの timebase-frequency を直接読む (32bit big-endian) */
    FILE *fp = fopen("/proc/device-tree/cpus/timebase-frequency", "rb");
    if (fp) {
        unsigned char buf[4];
        if (fread(buf, 1, 4, fp) == 4) {
            cached_freq = ((uint64_t)buf[0] << 24) | ((uint64_t)buf[1] << 16) |
                          ((uint64_t)buf[2] << 8)  |  (uint64_t)buf[3];
        }
        fclose(fp);
    }

    if (cached_freq == 0) {
        /* device-treeノードが無い環境向けフォールバック:
         * CLOCK_MONOTONICを基準にrdtimeを実測キャリブレーションする */
        struct timespec t0, t1;
        uint64_t c0, c1;

        clock_gettime(CLOCK_MONOTONIC, &t0);
        c0 = read_freerun_counter();

        do {
            clock_gettime(CLOCK_MONOTONIC, &t1);
        } while ((t1.tv_sec - t0.tv_sec) * 1000000000LL +
                 (t1.tv_nsec - t0.tv_nsec) < 10 * 1000 * 1000 /* 10ms */);

        c1 = read_freerun_counter();

        int64_t elapsed_ns = (t1.tv_sec - t0.tv_sec) * 1000000000LL +
                              (t1.tv_nsec - t0.tv_nsec);
        cached_freq = (uint64_t)((c1 - c0) * 1000000000ULL / (uint64_t)elapsed_ns);
    }
    return cached_freq;
}

#elif defined(__aarch64__)

static inline uint64_t read_freerun_freq(void)
{
    uint64_t freq;
    __asm__ __volatile__("mrs %0, cntfrq_el0" : "=r"(freq));
    return freq;
}

#elif defined(__arm__)

static inline uint64_t read_freerun_freq(void)
{
    uint32_t freq;
    __asm__ __volatile__("mrc p15, 0, %0, c14, c0, 0" : "=r"(freq));
    return (uint64_t)freq;
}

#endif
