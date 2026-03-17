// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define DEVICE_NAME "mychardev"
#define CLASS_NAME "mychar"
#define BUFFER_SIZE PAGE_SIZE

static dev_t dev_number;
static struct cdev my_cdev;
static struct class *my_class;
static struct device *my_device;
static char *kernel_buffer;
static size_t data_len = 0;
static DEFINE_MUTEX(dev_mutex);

static int dev_open(struct inode *inode, struct file *file)
{
    pr_info("mychardev: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    pr_info("mychardev: Device closed\n");
    return 0;
}

static loff_t dev_llseek(struct file *file, loff_t offset, int whence)
{
    loff_t new_pos;
    
    mutex_lock(&dev_mutex);
    new_pos = fixed_size_llseek(file, offset, whence, data_len);
    mutex_unlock(&dev_mutex);
    
    if (new_pos >= 0)
        pr_info("mychardev: llseek to %lld\n", new_pos);
    
    return new_pos;
}

static ssize_t dev_read(struct file *file, char __user *buf,
                        size_t count, loff_t *pos)
{
    int available_data;
    int bytes_to_read;
    int bytes_read;
    
    mutex_lock(&dev_mutex);
    
    if (*pos >= data_len) {
        mutex_unlock(&dev_mutex);
        return 0;
    }
    
    available_data = data_len - *pos;
    bytes_to_read = min_t(size_t, count, available_data);
    
    bytes_read = bytes_to_read - copy_to_user(buf,
                    kernel_buffer + *pos, bytes_to_read);
    *pos += bytes_read;
    
    mutex_unlock(&dev_mutex);
    
    pr_info("mychardev: Read %d bytes (pos=%lld)\n", bytes_read, *pos);
    return bytes_read;
}

static ssize_t dev_write(struct file *file, const char __user *buf,
                         size_t count, loff_t *pos)
{
    int bytes_to_write;
    int bytes_written;
    
    mutex_lock(&dev_mutex);
    
    if (*pos >= BUFFER_SIZE) {
        mutex_unlock(&dev_mutex);
        return -ENOSPC;
    }
    
    bytes_to_write = min_t(size_t, count, BUFFER_SIZE - *pos);
    
    if (bytes_to_write == 0) {
        mutex_unlock(&dev_mutex);
        return -ENOSPC;
    }
    
    bytes_written = bytes_to_write - copy_from_user(
                    kernel_buffer + *pos, buf, bytes_to_write);
    *pos += bytes_written;
    
    if (*pos > data_len)
        data_len = min_t(size_t, *pos, BUFFER_SIZE);
    
    mutex_unlock(&dev_mutex);
    
    pr_info("mychardev: Written %d bytes (pos=%lld)\n", 
            bytes_written, *pos);
    return bytes_written;
}

static int remap_mm(struct file *filp, struct vm_area_struct *vma) 
{
    unsigned long size = vma->vm_end - vma->vm_start;
    pgprot_t prot;
    
    if (size > PAGE_SIZE) {
        pr_err("mychardev: mmap size exceeds buffer\n");
        return -EINVAL;
    }
    
    if (vma->vm_pgoff != 0) {
        pr_err("mychardev: mmap offset must be 0\n");
        return -EINVAL;
    }
    
    prot = pgprot_writecombine(vma->vm_page_prot);
    vm_flags_set(vma, VM_IO | VM_DONTEXPAND | VM_DONTDUMP);
    
    if (remap_pfn_range(vma, vma->vm_start, 
                        virt_to_pfn(kernel_buffer),
                        size, prot)) {
        pr_err("mychardev: remap_pfn_range failed\n");
        return -EAGAIN;
    }
    
    pr_info("mychardev: mmap successful\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
    .llseek = dev_llseek,
    .mmap = remap_mm,
};

static int __init mychar_init(void)
{
    int ret;
    
    kernel_buffer = (void*)__get_free_page(GFP_KERNEL);
    if (!kernel_buffer) {
        pr_err("mychardev: Failed to allocate buffer\n");
        return -ENOMEM;
    }
    memset(kernel_buffer, 0, PAGE_SIZE);
    
    ret = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        free_page((unsigned long)kernel_buffer);
        return ret;
    }
    
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;
    
    ret = cdev_add(&my_cdev, dev_number, 1);
    if (ret < 0) {
        goto fail_cdev;
    }
    
    my_class = class_create(CLASS_NAME);
    if (IS_ERR(my_class)) {
        ret = PTR_ERR(my_class);
        goto fail_class;
    }
    
    my_device = device_create(my_class, NULL, dev_number,
                              NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        ret = PTR_ERR(my_device);
        goto fail_device;
    }
    
    pr_info("mychardev: Loaded (Major=%d, Minor=%d)\n",
           MAJOR(dev_number), MINOR(dev_number));
    return 0;

fail_device:
    class_destroy(my_class);
fail_class:
    cdev_del(&my_cdev);
fail_cdev:
    unregister_chrdev_region(dev_number, 1);
    free_page((unsigned long)kernel_buffer);
    return ret;
}

static void __exit mychar_exit(void)
{
    device_destroy(my_class, dev_number);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_number, 1);
    free_page((unsigned long)kernel_buffer);
    pr_info("mychardev: Unloaded\n");
}

module_init(mychar_init);
module_exit(mychar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Character device with mmap for Linux v6.12");
MODULE_VERSION("1.1");

