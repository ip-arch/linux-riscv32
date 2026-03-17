// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Naohiko Shimizu");

static int irq_number = -1;
static struct device *trigger_dev;

static irqreturn_t my_hardirq_handler(int irq, void *dev_id)
{
    pr_info("TOP half called\n");
    return IRQ_WAKE_THREAD;
}

static irqreturn_t my_thread_handler(int irq, void *dev_id)
{
    pr_info("Bottom half called\n");
    return IRQ_HANDLED;
}

// Sysfs attribute to trigger IRQ manually
static ssize_t trigger_store(struct device *dev,
                             struct device_attribute *attr,
                             const char *buf, size_t count)
{
    // Manually call the IRQ handler for testing
    pr_info("Manual IRQ trigger requested\n");
    my_hardirq_handler(irq_number, &irq_number);
    return count;
}

static DEVICE_ATTR_WO(trigger);

static int __init my_init(void)
{
    int ret;
    
    irq_number = 16;  // Use unused IRQ
    
    ret = request_threaded_irq(
        irq_number,
        my_hardirq_handler,
        my_thread_handler,
        0,
        "mydevice",
        &irq_number
    );
    
    if (ret) {
        pr_err("Failed to request IRQ: %d\n", ret);
        return ret;
    }
    
    // Create sysfs entry for manual trigger
    trigger_dev = root_device_register("irq_trigger");
    if (IS_ERR(trigger_dev)) {
        free_irq(irq_number, &irq_number);
        return PTR_ERR(trigger_dev);
    }
    
    ret = device_create_file(trigger_dev, &dev_attr_trigger);
    if (ret) {
        root_device_unregister(trigger_dev);
        free_irq(irq_number, &irq_number);
        return ret;
    }
    
    pr_info("IRQ %d registered. Trigger via: echo 1 > /sys/devices/irq_trigger/trigger\n",
            irq_number);
    return 0;
}

static void __exit my_exit(void)
{
    device_remove_file(trigger_dev, &dev_attr_trigger);
    root_device_unregister(trigger_dev);
    free_irq(irq_number, &irq_number);
    pr_info("Module unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

