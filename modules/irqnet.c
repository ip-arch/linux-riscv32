#include <linux/module.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Naohiko Shimizu");

static int irq_number = 14;  // virtio1 network from /proc/interrupts
static void *dev_id;
static int int_count = 0;

static irqreturn_t my_shared_irq_handler(int irq, void *dev_id)
{
    pr_info("Network IRQ %d detected! (top half)\n", irq);
    
    int_count++;
    // Return IRQ_NONE to let the real virtio driver handle it
    // Or return IRQ_HANDLED if you want to claim it
    return IRQ_WAKE_THREAD;
    // return IRQ_NONE;
}

static irqreturn_t my_thread_handler(int irq, void *dev_id)
{
    pr_info("Bottom half called:int_count=%d\n",int_count);
    // return IRQ_HANDLED;
    return IRQ_NONE;
}

static int __init my_init(void)
{
    int ret;
    
    // Use module address as unique dev_id
    dev_id = &irq_number;
    
    ret = request_threaded_irq(
        irq_number,
        my_shared_irq_handler,
        my_thread_handler,
        IRQF_SHARED,     // MUST be shared for existing IRQ
        "irq_monitor",
        dev_id           // Must be unique non-NULL pointer
    );
    
    if (ret) {
        pr_err("Failed to share IRQ %d: error %d\n", irq_number, ret);
        return ret;
    }
    
    pr_info("Monitoring IRQ %d. Send network traffic from host to trigger.\n", 
            irq_number);
    pr_info("From host: ping 10.0.2.15\n");
    return 0;
}

static void __exit my_exit(void)
{
    free_irq(irq_number, dev_id);
    pr_info("Stopped monitoring IRQ %d\n", irq_number);
}

module_init(my_init);
module_exit(my_exit);

