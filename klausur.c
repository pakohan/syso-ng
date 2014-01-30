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

 /*
 * Atomic Operationen
 */
int atomic_read(atomic_t *v);
int atomic_set(atomic_t *v, int i);
int atomic_add(int i, atomic_t *v);
int atomic_sub(int i, atomic_t *v);
int atomic_inc(atomic_t *v);
int atomic_dec(atomic_t *v);
int atomic_sub_and_test(int i, atomic_t *v);
int atomic_inc_and_test(atomic_t *v);
int atomic_dec_and_test(atomic_t *v);
int atomic_add_negative(int i, atomic_t *v);

void set_bit(int nr, volatile unsigned long * addr);
void clear_bit(int nr, volatile unsigned long * addr);
void change_bit(int nr, volatile unsigned long * addr);
int test_and_set_bit(int nr, volatile unsigned long * addr);
int test_and_clear_bit(int nr, volatile unsigned long * addr);
int test_and_change_bit(int nr, volatile unsigned long* addr);
int test_bit(int nr, const volatile void * addr);

/*
 * Atomic
 */

static atomic_t sem_read = ATOMIC_INIT(0);
static atomic_t sem_write = ATOMIC_INIT(0);


static int driver_open( struct inode *device, struct file *instance )
{
    printk(KERN_INFO "Open file called with flags %u\n", instance->f_flags);
    if ( instance->f_flags & O_RDWR ) {
        printk( KERN_ERR "Open device driver only allowed with flag O_RDONLY or O_WRONLY");
        return -1;
    } else if ( instance->f_flags & O_RDONLY ) {
        if (atomic_inc_return(&sem_read) > 1) {
            atomic_dec(&sem_read);
            printk(KERN_ERR "File is already opened by another process for reading\n");
            return -1;
        }
    } else if ( instance->f_flags & O_WRONLY ) {
        if (atomic_inc_return(&sem_write) > 1) {
            atomic_dec(&sem_write);
            printk(KERN_ERR "File is already opened by another process for writing\n");
            return -1;
        }
    }
    printk(KERN_INFO "File has been opened\n");
    return 0;
}

static int driver_close( struct inode *device, struct file *instance )
{
    printk(KERN_INFO "Close file called\n");
    if ( instance->f_flags & O_RDONLY ) {
        atomic_dec(&sem_read);
    } else if ( instance->f_flags & O_WRONLY ) {
        atomic_dec(&sem_write);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/*
 * Wait Queues
 */


static wait_queue_head_t read_wq;
static wait_queue_head_t write_wq;

//In init_module
init_waitqueue_head( &read_wq );
init_waitqueue_head( &write_wq );

// Read -- Write
while (to_read > 0 ) {
        wait_event_interruptible(read_wq, 'Bedingung' )
        //do something
}
wake_up_interruptible(&write_wq);


////////////////////////////////////////////////////////////////////////////////
static dev_t first;
static struct class *cl1;
static struct cdev c_dev;

// Geraet anmelden:
static int __init mod_setup(void){
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

static dev_t first;
static struct class *cl1;
static struct cdev c_dev;

// Geraet abmelden:
static void __exit mod_cleanup(void)
{
    int i = 0;
    cdev_del(&c_dev);
    for (i = 0; i < NUMBER_DEVICES; i++) {
        device_destroy(cl1, MKDEV(MAJOR(first), MINOR(first) + i));
    }
    class_destroy(cl1);
    unregister_chrdev_region(first, NUMBER_DEVICES);
    printk(KERN_INFO "module removed\n");
}

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
////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////
/*
 * Event-Workqueue
 */
 
 ////////////////////////////////////////////////////////////////////////////////
 /*
 * Semaphore
 */
 
// normal
DEFINE_SEMAPHORE()
void sema_init(struct semaphore *sem, int val);
void down(struct semaphore *sem);
void up(struct semaphore *sem);
int down_trylock(struct semaphore *sem);

// read / write
down_read()
down_read_trylock()
down_write()
down_write_trylock()
downgrade_write()
up_read()
up_write()

 DEFINE_SEMAPHORE( sem );
 
 // In init_module
sema_init( &sem, 1 );
 
static int driver_open( struct inode *device, struct file *instance )
{
    printk(KERN_INFO "Open file called\n");
    while ( down_trylock(&sem) ) {
        printk(KERN_INFO "sleeping...\n");
        msleep( 200 );
    }

    printk(KERN_INFO "got it...\n");
    msleep( 3000 );
    up( &sem );

    return 0;
}
 ////////////////////////////////////////////////////////////////////////////////
 /*
 * Spinlock
 */
 
static struct arch_spinlock_t lock;
static spinlock_t my_lock;
 // In init_module
spin_lock_init( &my_lock );

static int driver_open( struct inode *device, struct file *instance )
{
    printk(KERN_INFO "Open file called\n");
    while (!spin_trylock( &lock ))
    {
        printk(KERN_INFO "sleeping...\n");
        msleep( 200 );
    }

    printk(KERN_INFO "got it...\n");
    msleep( 3000 );
    arch_spin_unlock( &lock );

    return 0;
}
////////////////////////////////////////////////////////////////////////////////

 /*
 * mygpio
 */

#define NUMBER_DEVICES 1

#define DREI_BIT 0x7
#define OUTPUT 0x1
#define INPUT 0x0
#define LEISTE 0xFFFFFFFF
#define START_REGISTER 0xF2200000

#define SET_REGISTER 0xF220001c
#define CLEAR_REGISTER 0xF2200028
#define LEV_REGISTER 0xF2200034

#define LED 18
#define SWITCH 25

static struct file_operations fops;
static dev_t first;
static struct class *cl1;
static struct cdev c_dev;

static void set_pin( int pin_nr, int dir );
static void write_pin( int pin_nr, u32 start_reg, int val2 );
static u32 read_pin( int pin_nr, u32 start_reg );

static int __init mod_setup(void)
{

    if (alloc_chrdev_region(&first, 0, NUMBER_DEVICES, "buf.c") < 0)
        goto error_exit;

    if ((cl1 = class_create(THIS_MODULE, "chardrv_buf")) == NULL)
        goto error_class_create;

    if (device_create(cl1, NULL, MKDEV(MAJOR(first), MINOR(first)), NULL, "mygpio") == NULL)
        goto error_device_create;


    cdev_init(&c_dev, &fops);

    if (cdev_add(&c_dev, first, NUMBER_DEVICES) == -1)
        goto error_cdev_add;

    printk(KERN_INFO "module loaded\n");
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
}

static void __exit mod_cleanup(void)
{
        set_pin( LED, INPUT );

    cdev_del(&c_dev);
    device_destroy(cl1, MKDEV(MAJOR(first), MINOR(first)));
    class_destroy(cl1);
    unregister_chrdev_region(first, NUMBER_DEVICES);
    printk(KERN_INFO "module removed\n");
}

static void set_pin( int pin_nr, int dir )
{
    u32 *ptr;
    u32 old_value, old_value2;
    int pos;
    if (pin_nr > 53 || pin_nr < 0)
    {
        printk(KERN_ERR "undefined pin %d\n", pin_nr);
        return;
    }
    
    if( dir != 1 && dir != 0 )
    {
        printk(KERN_ERR "undefined direction %d\n", dir);
        return;
    }
    
    ptr = (u32*) (START_REGISTER + ((pin_nr / 10) * 0x4));
    /*atomarer bereich ... bei verschiedenen pins*/
    old_value = readl( ptr );
    old_value2 = old_value;
    rmb();
    pos = pin_nr % 10;
    old_value = old_value & (LEISTE ^ (DREI_BIT << (pos*3)));
    wmb();
        old_value |= dir << (pos*3);
    writel( old_value, ptr );
    /*... bis hier*/
}

static void write_pin( int pin_nr, u32 start_reg, int val2 )
{
    u32 *ptr;
    int pos;

    if (pin_nr > 53 || pin_nr < 0)
    {
        printk(KERN_ERR "undefined pin %d\n", pin_nr);
        return;
    }
        
        ptr = (u32*) (start_reg + ((pin_nr / 32) * 0x4));
        pos = pin_nr % 32;
        wmb();
        writel( val2 << pos, ptr );
}

static u32 read_pin( int pin_nr, u32 start_reg )
{
    u32 *ptr;
    u32 val;
    int pos;

    if (pin_nr > 53 || pin_nr < 0)
    {
        printk(KERN_ERR "undefined pin %d\n", pin_nr);
        return 0;
    }
        
        ptr = (u32*) (start_reg + ((pin_nr / 32) * 0x4));
        val = readl( ptr );
        rmb();
        pos = pin_nr % 32;
        val = val & 1 << pos;
        
    return val;
}

static int driver_open( struct inode *device, struct file *instance )
{ /* exklusiver zugriff hier */
        set_pin( LED, OUTPUT );
        set_pin( SWITCH, INPUT );
    return 0;
}

static int driver_close( struct inode *device, struct file *instance )
{
    return 0;
}

static ssize_t driver_read(struct file *instance, char __user *buff, size_t count, loff_t *offp)
{
        int not_copied;
        int to_copy = 2;
    u32 value;
    
    value = read_pin( SWITCH, LEV_REGISTER );
                
    if(value == 0)
    {
                not_copied = copy_to_user( buff, "1", to_copy);
        }
        else
        {
                not_copied = copy_to_user( buff, "0", to_copy);
        }
        

        return to_copy - not_copied;
}


static ssize_t driver_write(struct file *instance, const char __user *buff, size_t count, loff_t *offp)
{
        int value;
        int not_copied = 0;
        int to_copy = count;
        
        
        not_copied = kstrtoint_from_user(buff, count, 0, &value);
        
        if(value == 1)
        {
                write_pin( LED, CLEAR_REGISTER, 1 );
        }
        else if(value == 0)
        {
                write_pin( LED, SET_REGISTER, 1 );
        }

        return to_copy - not_copied;
}
////////////////////////////////////////////////////////////////////////////////

 /*
 * led5.c
 */

#define EXPORT "/sys/class/gpio/export"
#define UNEXPORT "/sys/class/gpio/unexport"

#define LED "/sys/class/gpio/gpio18/"
#define BUTTON "/sys/class/gpio/gpio25/"

static volatile int our_signal = 0;

void *blink_worker(void *arg);
void signal_handler(int value);
void turn_on(int fpointer);
void turn_off(int fpointer);

static int button_state = 0;

int  main()
{
    pthread_t blinker;
    struct sigaction new_action;
    int device;

    int button;
    int led;
    
    int export = open(EXPORT, O_WRONLY);
        if (export < 0)
        {
                fprintf(stderr, "could not open '%s'\n", EXPORT);
        }
        write(export, "18", 2);
        write(export, "25", 2);
    if( close(export) == EOF )
    {
                fprintf(stderr, "could not close '%s'\n", EXPORT);
        return errno;
    }

    new_action.sa_handler = signal_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(SIGINT, &new_action, NULL);


        
        int direction25 = open(BUTTON "direction", O_WRONLY);
        if (export < 0)
        {
                fprintf(stderr, "could not open '%s'\n", BUTTON "direction");
        }

        write(direction25, "in", 2);
        close(direction25);
        
        
    if (pthread_create(&blinker, NULL, &blink_worker, NULL) != 0)
    {
        fprintf(stderr, "could not create thread\n");
        return 1;
    }
    


    int val;
    char value[2];

    while(1)
    {
            device = open(BUTTON "value", O_RDONLY);
                if (device < 0)
                {
                        fprintf(stderr, "could not open '%s'\n", BUTTON "value");
                }
                read(device, value, 2);
                close(device);
        
        val = atoi(value);
                
        if(val == 0)
        {
                        if (button_state == 0)
                        {
                                button_state = 1;
                                printf("button down\n");
                        }
        }
        else
        {
                        if (button_state == 1)
                        {
                                button_state = 0;
                                our_signal = !our_signal;
                                printf("button up\n");
                        }
                }

        sched_yield();
        usleep(20000);
    }
    
    
    pthread_join(blinker, NULL);
    
    

      
    return 0;
}

void *blink_worker(void *arg)
{
        struct timespec times;
        times.tv_sec = 0;
        times.tv_nsec = 100000000;
        
        int led = open(LED "direction", O_WRONLY);
        if (led < 0)
        {
                fprintf(stderr, "could not open '%s'\n", LED "direction");
        }
        write(led, "out", 3);
        if( close(led) == EOF )
    {
                fprintf(stderr, "could not close '%s'\n", LED "direction");
        return NULL;
    }

        led = open(LED "value", O_WRONLY);
        if (led < 0)
        {
                fprintf(stderr, "could not open '%s'\n", LED "direction");
        }

    while(1)
    {
            while (!our_signal)
            {
                    clock_nanosleep(CLOCK_REALTIME, 0, &times, NULL);
                    turn_on(led);
                    clock_nanosleep(CLOCK_REALTIME, 0, &times, NULL);
                    turn_off(led);
            }
            turn_off(led);
        }
        
        if( close(led) == EOF )
    {
                fprintf(stderr, "could not close '%s'\n", LED);
        return NULL;
    }

        return NULL;
}

void signal_handler(int value)
{
        our_signal = value;
        printf("Signal caught\n");
        int led = open(LED "value", O_WRONLY);
        if (led < 0)
        {
                fprintf(stderr, "could not open '%s'\n", LED "direction");
        }
        write(led, "1", 1);

        if( close(led) == EOF )
    {
                fprintf(stderr, "could not close '%s'\n", LED);
    }
        
        int unexport = open(UNEXPORT, O_WRONLY);
        if (unexport < 0)
        {
                fprintf(stderr, "could not open '%s'\n", EXPORT);
        }
        
        write(unexport, "18", 2);
        write(unexport, "25", 2);
        
        close(unexport);
        
    _exit(value);
}

void turn_on(int fpointer)
{
        write(fpointer, "0", 1);
}

void turn_off(int fpointer)
{
        write(fpointer, "1", 1);
}
////////////////////////////////////////////////////////////////////////////////
