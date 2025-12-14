#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("High-resolution timer example for Linux v6.12");
MODULE_VERSION("1.0");

// Timer structure
static struct hrtimer hrt;
static ktime_t kt;

// Counter for timer iterations (using atomic for safety)
static atomic_t count = ATOMIC_INIT(10);  // Run 10 times for demo

/**
 * timer_timeout - Timer callback function
 * @timer: Pointer to the hrtimer structure
 *
 * This function is called in interrupt context (softirq),
 * so it must be fast and cannot sleep.
 *
 * Return: HRTIMER_RESTART to restart timer, HRTIMER_NORESTART to stop
 */
static enum hrtimer_restart timer_timeout(struct hrtimer *timer)
{
    int current_count = atomic_dec_return(&count);
    
    pr_info("hrtimer: Timer fired, remaining count: %d\n", current_count);
    
    if (current_count > 0) {
        // Reschedule timer for next interval
        hrtimer_forward_now(timer, kt);
        return HRTIMER_RESTART;
    }
    
    pr_info("hrtimer: Timer completed all iterations\n");
    return HRTIMER_NORESTART;
}

static int __init my_init(void)
{
    pr_info("hrtimer: Module loaded\n");
    
    // Set timer interval to 1 second
    kt = ms_to_ktime(1000);  // 1000ms = 1 second
    
    // Alternative: kt = ktime_set(1, 0);  // 1 second + 0 nanoseconds
    
    // Initialize high-resolution timer
    // CLOCK_MONOTONIC: Monotonic clock (doesn't jump on time adjustments)
    // HRTIMER_MODE_REL: Relative time mode (from now)
    hrtimer_init(&hrt, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    
    // Set callback function
    hrt.function = timer_timeout;
    
    // Start the timer
    hrtimer_start(&hrt, kt, HRTIMER_MODE_REL);
    
    pr_info("hrtimer: Timer started with 1 second interval\n");
    
    return 0;
}

static void __exit my_remove(void)
{
    int ret;
    
    // Cancel the timer and wait for callback to complete
    ret = hrtimer_cancel(&hrt);
    
    if (ret)
        pr_info("hrtimer: Timer was active when cancelled\n");
    else
        pr_info("hrtimer: Timer was not active\n");
    
    pr_info("hrtimer: Module unloaded\n");
}

module_init(my_init);
module_exit(my_remove);

