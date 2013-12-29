#define LINUX
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>

static struct task_struct *thread_id;
static wait_queue_head_t wq;
static DECLARE_COMPLETION( on_exit );

static int thread_code( void *data )
{
    unsigned long timeout;
    int i;

    allow_signal( SIGTERM );
    /*for (i=0; i<5 && (kthread_should_stop()==0); i++ ) {*/
    for ( i=0;;i++ ) {
        timeout=HZ;
        timeout=wait_event_interruptible_timeout( wq, (timeout==0), timeout );
        printk("thread_function: woke up ... %ld\n", timeout);
        if( timeout==-ERESTARTSYS ) {
            printk("got signal, break\n");
            break;
        }
    }
    complete_and_exit(&on_exit, 0 );
}

static int __init mod_setup(void)
{
    printk(KERN_INFO "init module called\n");
    init_waitqueue_head(&wq);
    thread_id=kthread_create(thread_code,NULL,"mykthread");
    if ( thread_id==0 )
        return -EIO;
    wake_up_process( thread_id );
    return 0;
}

static void __exit mod_cleanup(void)
{
    printk(KERN_INFO "exit module called\n");
    kill_pid( task_pid(thread_id), SIGTERM, 1 );
    wait_for_completion( &on_exit );
}

module_init( mod_setup );
module_exit( mod_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser fuenftes Modul");
