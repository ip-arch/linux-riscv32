// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <linux/module.h>
#include <linux/interrupt.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Naohiko Shimizu");
// ハードIRQ部分（高速実行が必要な部分のみ）
static int irq_number = 12;

static irqreturn_t my_hardirq_handler(int irq, void *dev_id)
{
    // 最小限の処理
    pr_info("TOP half called");
    return IRQ_WAKE_THREAD; // スレッド部分を起動
}
// スレッド部分（時間のかかる処理）
static irqreturn_t my_thread_handler(int irq, void *dev_id)
{
    void *dev = dev_id;
    // ここではスリープ可能
    // 複雑なデータ処理など
    // データ読み取り
    pr_info("Bottom half called");
    return IRQ_HANDLED;
}
// 登録
static int __init my_init(void)
{
    int ret;
    ret = request_threaded_irq(
        irq_number,
        my_hardirq_handler,  // ハードIRQ部
        my_thread_handler,   // スレッド部
        IRQF_SHARED,
        "mydevice",
        &THIS_MODULE->name
    );
    if (ret) {
        pr_err( "Failed to request threaded IRQ\n");
        return ret;
    }
    return 0;
}
