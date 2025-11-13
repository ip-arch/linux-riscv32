/*
 *   This sample source codes are design for a seminar.
 *
 *   Copyright (c) 2005-2015 Naohiko Shimizu, All Rights Reserved.
 *   Author: Naohiko Shimizu : nshimizu@ip-arch.jp
 *
 *   Everyone is permitted to copy and distribute verbatim copies of
 *   this source code under GPL Version 2.
 */
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Naohiko Shimizu");

int init_module(void)		{ printk("Hello, world\n"); return 0; }
void cleanup_module(void)	{ printk("Goodbye world\n"); }

