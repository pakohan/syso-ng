#define LINUX
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/init.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <asm/timex.h>

static struct timer_list mytimer;
static unsigned int start = 0;
static unsigned long last_call = 0;

static void timer_func( unsigned long data )
{
    unsigned int end = 0;
    end = get_cycles();

    printk(KERN_INFO "timer called at %ld (%ld); cycles since last call: %u\n", mytimer.expires, mytimer.expires-last_call, end-start);
    last_call = mytimer.expires;
    start = get_cycles();
    mod_timer(&mytimer, jiffies + (2*HZ));
}

static int __init mod_setup(void)
{
    printk(KERN_INFO "init module called\n");
    init_timer( &mytimer );
    mytimer.function = timer_func;
    mytimer.data = 0;
    mytimer.expires = jiffies + (2*HZ);
    start = get_cycles();
    add_timer( &mytimer );
    return 0;
}

static void __exit mod_cleanup(void)
{
    printk(KERN_INFO "exit module called\n");
    if (del_timer_sync( &mytimer) )
        printk("Timer was active\n");
}

module_init( mod_setup );
module_exit( mod_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser fuenftes Modul");
