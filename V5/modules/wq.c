#define LINUX
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/jiffies.h>

static struct workqueue_struct *wq;
static void work_queue_func( struct work_struct *work );
static DECLARE_DELAYED_WORK( work_obj, work_queue_func );

static void work_queue_func( struct work_struct *work )
{
    printk(KERN_INFO "work queue func called\n" );
    if( queue_delayed_work( wq, &work_obj, 2*HZ ) )
        printk( KERN_INFO "queue work successful\n" );
    else
        printk( KERN_ERR "queue work successful\n" );
}

static int __init mod_setup(void)
{
    printk(KERN_INFO "init module called\n");
    wq = create_singlethread_workqueue( "DrvrSmpl" );
    if( queue_delayed_work( wq, &work_obj, 2*HZ ) )
        printk( KERN_INFO "queue work successful\n" );
    else
        printk( KERN_ERR "queue work successful\n" );
    return 0;
}

static void __exit mod_cleanup(void)
{
    printk(KERN_INFO "exit module called\n");
    cancel_delayed_work( &work_obj );
    destroy_workqueue( wq );
}

module_init( mod_setup );
module_exit( mod_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser fuenftes Modul");
