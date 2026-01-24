/*
 *   RISC-V GPIO Interrupt Example with Debugfs trigger
 *   For QEMU testing without real hardware
 */
#include <linux/module.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/debugfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Naohiko Shimizu");

static int ledlevel = 1;
static int irq;
static long count = 0;
static struct gpio_desc *gpio_led, *gpio_sw;
static struct dentry *debugfs_dir;

static irqreturn_t irq_sw(int irq, void *dev_id)
{
	ledlevel = 1 - ledlevel;
	if (gpio_led)
		gpiod_set_value(gpio_led, ledlevel);
	count++;
	
	pr_info("GPIO interrupt #%ld: LED=%d\n", count, ledlevel);
	
	return IRQ_HANDLED;
}

/* Debugfs file to manually trigger interrupt for testing */
static ssize_t trigger_write(struct file *file, const char __user *ubuf,
			     size_t len, loff_t *offp)
{
	char buf[8];
	
	if (len > sizeof(buf) - 1)
		return -EINVAL;
	
	if (copy_from_user(buf, ubuf, len))
		return -EFAULT;
	
	buf[len] = '\0';
	
	if (buf[0] == '1') {
		pr_info("Manual interrupt trigger from debugfs\n");
		/* Manually call interrupt handler for testing */
		irq_sw(irq, NULL);
	}
	
	return len;
}

static const struct file_operations trigger_fops = {
	.write = trigger_write,
	.open = simple_open,
	.llseek = default_llseek,
};

static ssize_t count_read(struct file *file, char __user *ubuf,
			  size_t len, loff_t *offp)
{
	char buf[32];
	int ret;
	
	ret = snprintf(buf, sizeof(buf), "%ld\n", count);
	return simple_read_from_buffer(ubuf, len, offp, buf, ret);
}

static const struct file_operations count_fops = {
	.read = count_read,
	.open = simple_open,
	.llseek = default_llseek,
};

static int my_probe(struct platform_device *pdev)
{
	int ret;
	struct device *dev = &pdev->dev;
	
	pr_info("Probing exboard GPIO device\n");
	
	/* Get LED GPIO (optional for QEMU) */
	gpio_led = devm_gpiod_get_index(dev, "ex_led", 0, GPIOD_OUT_LOW);
	if (IS_ERR(gpio_led)) {
		pr_warn("LED GPIO not available (OK for testing): %ld\n",
			PTR_ERR(gpio_led));
		gpio_led = NULL;
	}
	
	/* Get Switch GPIO (optional for QEMU) */
	gpio_sw = devm_gpiod_get_index(dev, "ex_sw", 0, GPIOD_IN);
	if (IS_ERR(gpio_sw)) {
		pr_warn("SW GPIO not available (OK for testing): %ld\n",
			PTR_ERR(gpio_sw));
		gpio_sw = NULL;
	}
	
	/* Get IRQ number from GPIO or use dummy */
	if (gpio_sw) {
		irq = gpiod_to_irq(gpio_sw);
		if (irq < 0) {
			pr_err("Failed to get IRQ from GPIO: %d\n", irq);
			return irq;
		}
		
		/* Request IRQ */
		ret = request_irq(irq,
				  (irq_handler_t)irq_sw,
				  IRQF_SHARED | IRQF_TRIGGER_FALLING,
				  "GPIO_SW_INT",
				  THIS_MODULE->name);
		if (ret < 0) {
			pr_err("request_irq failed: %d\n", ret);
			return ret;
		}
		
		pr_info("GPIO initialized: IRQ=%d, SW=%d\n",
			irq, gpiod_get_value(gpio_sw));
	} else {
		pr_info("Running in test mode (no hardware GPIO)\n");
		irq = 0;
	}
	
	/* Create debugfs interface for testing */
	debugfs_dir = debugfs_create_dir("exboard_gpio", NULL);
	if (debugfs_dir) {
		debugfs_create_file("trigger", 0200, debugfs_dir,
				    NULL, &trigger_fops);
		debugfs_create_file("count", 0400, debugfs_dir,
				    NULL, &count_fops);
		pr_info("Debugfs interface created at /sys/kernel/debug/exboard_gpio/\n");
		pr_info("  Test: echo 1 > /sys/kernel/debug/exboard_gpio/trigger\n");
		pr_info("  Read: cat /sys/kernel/debug/exboard_gpio/count\n");
	}
	
	return 0;
}

static int my_remove(struct platform_device *pdev)
{
	pr_info("Removing GPIO driver: interrupt count=%ld\n", count);
	
	debugfs_remove_recursive(debugfs_dir);
	
	if (irq > 0)
		free_irq(irq, THIS_MODULE->name);
	
	return 0;
}

static const struct of_device_id gpio_dt_ids[] = {
	{ .compatible = "shmz,exboard" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, gpio_dt_ids);

static struct platform_driver gpio_driver = {
	.probe = my_probe,
	.remove = my_remove,
	.driver = {
		.name = "exboard-shmz",
		.of_match_table = gpio_dt_ids,
		.owner = THIS_MODULE,
	},
};

module_platform_driver(gpio_driver);

