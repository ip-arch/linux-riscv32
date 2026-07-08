// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/*
 *   RISC-V GPIO Interrupt Example with Debugfs trigger
 *   For QEMU testing without real hardware
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/machine.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

static int ledvalue;

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
static struct platform_device *pdev_sim;

static int my_probe(struct platform_device *pdev)
{
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
    gpiod_set_value_cansleep(gpio_led, ledvalue);
    pr_info("GPIO value: LED=%d, SW=%d\n", gpiod_get_value_cansleep(gpio_led), gpiod_get_value_cansleep(gpio_sw));
    return 0;
}

static void my_remove(struct platform_device *pdev)
{
    pr_info("GPIO value: LED=%d, SW=%d\n", gpiod_get_value_cansleep(gpio_led), gpiod_get_value_cansleep(gpio_sw));
    pr_info("Removing GPIO driver\n");
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
