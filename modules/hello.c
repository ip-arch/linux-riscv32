#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init hello_init(void)
{
    pr_info( "Hello, Kernel Module!\n");
    return 0; // 成功時は0を返す
}

static void __exit hello_exit(void)
{
    pr_info( "Goodbye, Kernel Module!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple Hello World module");
MODULE_VERSION("1.0");

