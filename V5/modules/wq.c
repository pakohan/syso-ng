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

static struct workqueue_struct *wq;

static void work_queue_func( void *data )
{
    printk(KERN_INFO "work queue func called\n" );
    wq = NULL;
}

static DECLARE_WORK( work_obj, work_queue_func );

static int __init mod_setup(void)
{
    printk(KERN_INFO "init module called\n");
    wq = create_singlethread_workqueue( "DrvrSmpl" );
    if( queue_work( wq, &work_obj ) )
        printk( KERN_INFO "queue work successful\n" );
    else
        printk( KERN_ERR "queue work successful\n" );
    return 0;
}

static void __exit mod_cleanup(void)
{
    printk(KERN_INFO "exit module called\n");
    if ( wq )
        destroy_workqueue( wq );
}

module_init( mod_setup );
module_exit( mod_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser fuenftes Modul");
