// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/*
 *   RISC-V GPIO Interrupt Example with Debugfs trigger
 *   For QEMU testing without real hardware
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/machine.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

static int ledvalue;
static ktime_t irq_time;   /* トップハーフで記録した時刻 */

module_param(ledvalue, int, S_IRUGO);
MODULE_PARM_DESC(ledvalue, "gpio-sim led level");

static struct gpiod_lookup_table lookup = {
    .dev_id = "gpio-sim-demo",
    .table = {
        GPIO_LOOKUP("SW0", U16_MAX, "sw", GPIO_ACTIVE_HIGH),
        GPIO_LOOKUP("LED0", U16_MAX, "led", GPIO_ACTIVE_HIGH),
        { }
    },
};

static struct gpio_desc *gpio_sw, *gpio_led;
static int count, irq;
static struct platform_device *pdev_sim;

/* トップハーフ：ハードIRQコンテキスト、最小限の処理のみ */
static irqreturn_t irq_sw_hard(int irq, void *dev_id)
{
    irq_time = ktime_get();
    return IRQ_WAKE_THREAD;   /* ボトムハーフ(スレッド)を起床 */
}


static irqreturn_t irq_sw_thread(int irq, void *dev_id)
{
	s64 delay_us = ktime_us_delta(ktime_get(), irq_time);
        count++;
	ledvalue = 1 - ledvalue;
    	gpiod_set_value_cansleep(gpio_led, ledvalue);
	pr_info("GPIO interrupt #%d: SW=%d (top->bottom delay: %lld us)\n",
            count, gpiod_get_value_cansleep(gpio_sw), delay_us);
        return IRQ_HANDLED;
}

static int my_probe(struct platform_device *pdev)
{
    int ret;
    gpiod_add_lookup_table(&lookup);

    pr_info("Probing exboard GPIO device\n");

    gpio_sw = devm_gpiod_get(&pdev->dev, "sw", GPIOD_IN);
    if (IS_ERR(gpio_sw)) {
        pr_warn("SW GPIO not available: %ld\n", PTR_ERR(gpio_sw));
        return 0;
    }
    gpio_led = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
    if (IS_ERR(gpio_led)) {
        pr_warn("LED GPIO not available: %ld\n", PTR_ERR(gpio_led));
        return 0;
    }

    irq = gpiod_to_irq(gpio_sw);
    if (irq < 0) {
        pr_err("Failed to get IRQ from GPIO: %d\n", irq);
        return irq;
    }
    gpiod_set_value_cansleep(gpio_led, ledvalue);
    pr_info("GPIO value: LED=%d, SW=%d\n", gpiod_get_value_cansleep(gpio_led), gpiod_get_value_cansleep(gpio_sw));

    ret = devm_request_threaded_irq(&pdev->dev, irq,
                                 irq_sw_hard,        /* ハードIRQ時刻記録 */
                                 irq_sw_thread,      /* プロセスコンテキストで実行 */
                                 IRQF_TRIGGER_RISING | IRQF_ONESHOT,
                                 "GPIO_SW_INT", &pdev->dev);
    if (ret < 0) {
        pr_err("request_irq failed: %d\n", ret);
        return ret;
    }

    pr_info("GPIO initialized: IRQ=%d, SW=%d\n", irq, gpiod_get_value_cansleep(gpio_sw));
    return 0;
}

static void my_remove(struct platform_device *pdev)
{
    pr_info("GPIO value: LED=%d, SW=%d\n", gpiod_get_value_cansleep(gpio_led), gpiod_get_value_cansleep(gpio_sw));
    pr_info("Removing GPIO driver: interrupt count=%d\n", count);
    gpiod_remove_lookup_table(&lookup);
}

static struct platform_driver gpio_driver = {
    .probe = my_probe,
    .remove = my_remove,
    .driver = { .name = "gpio-sim-demo" },
};

static int __init gpio_sim_demo_init(void)
{
    int ret;

    pdev_sim = platform_device_register_simple("gpio-sim-demo", -1, NULL, 0);
    if (IS_ERR(pdev_sim))
        return PTR_ERR(pdev_sim);

    ret = platform_driver_register(&gpio_driver);
    if (ret)
        platform_device_unregister(pdev_sim);

    return ret;
}

static void __exit gpio_sim_demo_exit(void)
{
    platform_driver_unregister(&gpio_driver);
    platform_device_unregister(pdev_sim);
}

module_init(gpio_sim_demo_init);
module_exit(gpio_sim_demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Naohiko Shimizu");
