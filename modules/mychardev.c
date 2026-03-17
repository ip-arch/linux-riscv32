// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mychardev"
#define CLASS_NAME "mychar"
#define BUFFER_SIZE 1024

static dev_t dev_number;
static struct cdev my_cdev;
static struct class *my_class;
static struct device *my_device;
static char kernel_buffer[BUFFER_SIZE];

// read関数
static ssize_t dev_read(struct file *file, char __user *buf,
                        size_t count, loff_t *pos)
{
    int bytes_to_read;
    int bytes_read;
    
    if (*pos > 0)  	return 0;
    bytes_to_read = BUFFER_SIZE;
    if (bytes_to_read > count)
        bytes_to_read = count;
    bytes_read = bytes_to_read - copy_to_user(buf,
                    kernel_buffer, bytes_to_read);
    *pos += bytes_read;
    return bytes_read;
}
// write関数
static ssize_t dev_write(struct file *file, const char __user *buf,
                         size_t count, loff_t *pos)
{
    int bytes_to_write;
    int bytes_written;
    
    bytes_to_write = BUFFER_SIZE;
    if (bytes_to_write > count)
        bytes_to_write = count;
    bytes_written = bytes_to_write - copy_from_user(
                    kernel_buffer, buf, bytes_to_write);
    *pos = 0;
    return bytes_written;
}
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
    .write = dev_write,
};
// 初期化関数
static int __init mychar_init(void)
{
    int ret; 
    // デバイス番号の動的割り当て
    ret = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    pr_info( "mychardev: Major=%d, Minor=%d\n",
           MAJOR(dev_number), MINOR(dev_number));
    // cdev構造体の初期化
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;
    // cdevの追加
    ret = cdev_add(&my_cdev, dev_number, 1);  
    // デバイスクラスの作成
    my_class = class_create(CLASS_NAME);
    // デバイスの作成 (/dev/mychardev が自動作成される)
    my_device = device_create(my_class, NULL, dev_number,
                              NULL, DEVICE_NAME);  
    return 0;
}
// 終了関数
static void __exit mychar_exit(void)
{
    device_destroy(my_class, dev_number);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_number, 1);
    pr_info( "mychardev: Device driver unloaded\n");
}
module_init(mychar_init);
module_exit(mychar_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple character device driver for Linux v6.12");
MODULE_VERSION("1.0");

