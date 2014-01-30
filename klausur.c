// init module functions
static int __init mod_setup(void);
static void __exit mod_cleanup(void);

// open and close functions
static int driver_open( struct inode *device, struct file *instance );
static int driver_close( struct inode *device, struct file *instance );

// read and write functions
static ssize_t driver_read(struct file *instance, char __user *buff, size_t count, loff_t *offp);
static ssize_t driver_write(struct file *instance, const char __user *buff, size_t count, loff_t *offp);

// makros, um die init functions an- und abzumelden
module_init( mod_setup );
module_exit( mod_cleanup );

// pass to kernel at module_init function
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    .write = driver_write
};

MODULE_LICENSE("GPL");

////////////////////////////////////////////////////////////////////////////////

// Geraet anmelden:
    if (alloc_chrdev_region(&first, 0, NUMBER_DEVICES, "buf.c") < 0)
        goto error_exit;

    if ((cl1 = class_create(THIS_MODULE, "chardrv_buf")) == NULL)
        goto error_class_create;

    for (i = 0; i < NUMBER_DEVICES; i++) {
        if (device_create(cl1, NULL, MKDEV(MAJOR(first), MINOR(first) + i),
                          NULL, "mod_buf%d", i) == NULL)
            goto error_device_create;
    }

    cdev_init(&c_dev, &fops);

    if (cdev_add(&c_dev, first, NUMBER_DEVICES) == -1)
        goto error_cdev_add;

    return 0;

error_cdev_add:
    device_destroy(cl1, first);
error_device_create:
    class_destroy(cl1);
error_class_create:
    unregister_chrdev_region(first, NUMBER_DEVICES);
error_exit:
    printk(KERN_ERR "module loading failed\n");
    return -1;

////////////////////////////////////////////////////////////////////////////////
/*
 * Tasklet
 */
static void tasklet_func( unsigned long data )
{
    printk(KERN_INFO "tasklet called\n");
}
//               Name,    Funktion,    Übergabewert
DECLARE_TASKLET( tldescr, tasklet_func, 0L );

static int __init mod_setup(void)
{
    tasklet_schedule( &tldescr );
    return 0;
}

static void __exit mod_cleanup(void)
{
    tasklet_kill( &tldescr );
}
/*
 * Timer
 */
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
/*
 * Kernel Thread
 */
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
        timeout=2*HZ;
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
/*
 * Workqueue
 */
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
/*
 * Event-Workqueue
 */
