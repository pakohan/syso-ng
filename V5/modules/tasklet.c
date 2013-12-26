#define LINUX
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

/*static struct tasklet_struct *tldescr;*/

static void tasklet_func( unsigned long data )
{
    printk(KERN_INFO "tasklet called\n");
}

DECLARE_TASKLET( tldescr, tasklet_func, 0L );

static int __init mod_setup(void)
{
    printk(KERN_INFO "init module called\n");
    tasklet_schedule( &tldescr );
    return 0;
}

static void __exit mod_cleanup(void)
{
    printk(KERN_INFO "exit module called\n");
    tasklet_kill( &tldescr );
}

module_init( mod_setup );
module_exit( mod_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser fuenftes Modul");
