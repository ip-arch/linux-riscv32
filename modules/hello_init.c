// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>

static int debug_level = 0;
static char *device_name = "mydevice";

// パラメータの定義
module_param(debug_level, int, S_IRUGO);
MODULE_PARM_DESC(debug_level, "Debug level (0-5)");

module_param(device_name, charp, S_IRUGO);
MODULE_PARM_DESC(device_name, "Device name");

static int __init param_init(void)
{
    pr_info( "Debug Level: %d\n", debug_level);
    pr_info( "Device Name: %s\n", device_name);
    return 0;
}

static void __exit param_exit(void)
{
    pr_info( "Module unloaded\n");
}

module_init(param_init);
module_exit(param_exit);
MODULE_LICENSE("GPL");

