#define LINUX

#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>

#define NUMBER_DEVICES 1
#define SIZE 100

static struct file_operations fops;
static dev_t first;
static struct class *cl1;
static struct cdev c_dev;
static atomic_t sem_read = ATOMIC_INIT(0);
static atomic_t sem_write = ATOMIC_INIT(0);
static wait_queue_head_t read_wq;
static wait_queue_head_t write_wq;
static int read_ptr = 0;
static int write_ptr = 1;
static char buffer[SIZE];

#define WRITE_BUF_SIZE ((write_ptr >= read_ptr) ? ((SIZE-write_ptr)+read_ptr-1) : (read_ptr-write_ptr-1))
#define READ_BUF_SIZE  ((write_ptr >= read_ptr) ? (write_ptr-read_ptr-1) : ((SIZE-read_ptr)+write_ptr-1))
#define IS_READ_BUF_BROKEN (read_ptr>write_ptr&&write_ptr>0)
#define IS_WRITE_BUF_BROKEN (write_ptr>read_ptr&&read_ptr>0)

static int __init mod_setup(void)
{
    int i = 0;
    if (alloc_chrdev_region(&first, 0, NUMBER_DEVICES, "buf.c") < 0)
        goto error_exit;

    if ((cl1 = class_create(THIS_MODULE, "chardrv_buf")) == NULL)
        goto error_class_create;

    for (i = 0; i < NUMBER_DEVICES; i++) {
        if (device_create(cl1, NULL, MKDEV(MAJOR(first), MINOR(first) + i),
                          NULL, "mod_buf%d", i) == NULL)
            goto error_device_create;
    }

    init_waitqueue_head( &read_wq );
    init_waitqueue_head( &write_wq );

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
    int i = 0;
    cdev_del(&c_dev);
    for (i = 0; i < NUMBER_DEVICES; i++) {
        device_destroy(cl1, MKDEV(MAJOR(first), MINOR(first) + i));
    }
    class_destroy(cl1);
    unregister_chrdev_region(first, NUMBER_DEVICES);
    printk(KERN_INFO "module removed\n");
}

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

static size_t get_max_size(size_t to_read, size_t own_ptr, size_t opposite_ptr, size_t buf_size, int flag)
{
    size_t retVal = to_read;
    int decision;
    if (buf_size < to_read)
        retVal = buf_size;

    decision = flag ? (own_ptr >= opposite_ptr): (own_ptr > opposite_ptr);

    if (decision) {
        retVal = min(retVal, (SIZE - own_ptr));
    } else {
        retVal = min(retVal, (opposite_ptr - own_ptr));
    }

    printk("getmaxsize: %d for flag %d, own_ptr = %d, opposite_ptr = %d, buf_size = %d\n", min(retVal, to_read), flag,own_ptr,opposite_ptr,buf_size);
    return min(retVal, to_read);
}

static ssize_t driver_read(struct file *instance, char __user *buff, size_t count, loff_t *offp)
{
    size_t to_read = count;
    int max = 0;
    int not_red = 0;
    if ( !(instance->f_flags & O_RDONLY) == O_RDONLY ) {
        printk(KERN_ERR "read called from wrong instance!\n");
        return -1;
    }

    if ( count > SIZE ) {
        printk(KERN_ERR "buffer too small! %d/%d\n", SIZE, count);
        return -1;
    }

    if ((count > READ_BUF_SIZE) && (instance->f_flags & O_NONBLOCK) )
        return -EAGAIN;

    while (to_read > 0 ) {
        wait_event_interruptible(read_wq, get_max_size(to_read, read_ptr, write_ptr, READ_BUF_SIZE, 0) > 0 || to_read == 0);
        max = get_max_size(to_read, read_ptr, write_ptr, READ_BUF_SIZE, 0);
        not_red = copy_to_user(buff, &buffer[read_ptr], max);
        read_ptr += (max - not_red);
        to_read = (to_read - max) + not_red;
        read_ptr = read_ptr % SIZE;
    }
    wake_up_interruptible(&write_wq);

    return count;
}


static ssize_t driver_write(struct file *instance, const char __user *buff, size_t count, loff_t *offp)
{
    size_t to_write = count;
    int max = 0;
    int not_written = 0;
    if ( !(instance->f_flags & O_WRONLY) == O_WRONLY ) {
        printk(KERN_ERR "write called from wrong instance!\n");
        return -1;
    }

    if ((count > WRITE_BUF_SIZE) && (instance->f_flags & O_NONBLOCK) )
        return -EAGAIN;

    if ( count > SIZE ) {
        printk(KERN_ERR "buffer too small! %d/%d\n", SIZE, count);
        return -1;
    }

    while (to_write > 0 ) {
        wait_event_interruptible(write_wq, get_max_size(to_write, write_ptr, read_ptr, WRITE_BUF_SIZE, 1) > 0 || to_write == 0);
        max = get_max_size(to_write, write_ptr, read_ptr, WRITE_BUF_SIZE, 1);
        not_written = copy_from_user(&buffer[write_ptr], buff, max);
        write_ptr += (max - not_written);
        to_write = (to_write - max) + not_written;
        write_ptr = write_ptr % SIZE;
    }
	wake_up_interruptible(&read_wq);

    return count;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    .write = driver_write
    /*.poll = driver_poll,*/
};

module_init( mod_setup );
module_exit( mod_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser erstes Modul");
