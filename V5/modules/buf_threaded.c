#define LINUX
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

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

typedef struct liste {
    char *buf;
    unsigned long size;
    struct liste *next;
} liste_t;

static struct file_operations fops;
static dev_t first;
static struct class *cl1;
static struct cdev c_dev;
static struct semaphore sem;

static DECLARE_WAIT_QUEUE_HEAD(read_wq);
static unsigned long curr_index = 0;
static liste_t *head = NULL;
static liste_t *read = NULL;

static liste_t* free_elem(liste_t *ptr);
static liste_t* get_elem(liste_t *n);
static size_t get_read_size(size_t count);

static int __init mod_setup(void)
{
    if (alloc_chrdev_region(&first, 0, 1, "buf.c") < 0)
        goto error_exit;

    if ((cl1 = class_create(THIS_MODULE, "chardrv_buf")) == NULL)
        goto error_class_create;

    if (device_create(cl1, NULL, MKDEV(MAJOR(first), MINOR(first)), NULL, "mod_buf_threaded") == NULL)
        goto error_device_create;
    cdev_init(&c_dev, &fops);

    if (cdev_add(&c_dev, first, 1) == -1)
        goto error_cdev_add;

    sema_init(&sem, 1);

    head = get_elem(NULL);
    read = head;
    if (head == NULL)
    {
        goto error_cdev_add;
    }

    printk(KERN_INFO "module loaded\n");
    return 0;

error_cdev_add:
    device_destroy(cl1, first);
error_device_create:
    class_destroy(cl1);
error_class_create:
    unregister_chrdev_region(first, 1);
error_exit:
    printk(KERN_ERR "module loading failed\n");
    return -1;
}

static void __exit mod_cleanup(void)
{
    liste_t *ptr;
    cdev_del(&c_dev);
    device_destroy(cl1, MKDEV(MAJOR(first), MINOR(first)));
    class_destroy(cl1);
    unregister_chrdev_region(first, 1);

    while (head->next != NULL)
    {
        ptr = head;
        head = head->next;
        kfree(ptr->buf);
        kfree(ptr);
    }

    printk(KERN_INFO "module removed\n");
}

static int driver_open( struct inode *device, struct file *instance )
{
    printk(KERN_INFO "File has been opened\n");
    return 0;
}

static int driver_close( struct inode *device, struct file *instance )
{
    printk(KERN_INFO "Close file called\n");
    while (read != NULL)
    {
        read = free_elem(read);
    }
    return 0;
}

static ssize_t driver_read(struct file *instance, char __user *buff, size_t count, loff_t *offp)
{
    size_t to_read = count;
    size_t not_red = 0;
    size_t red = 0;
    int i = 0;

/*
    thread_id = kthread_create(read_thread, NULL, "read_thread");
    if ( thread_id==0 )
        return -EIO;
    wake_up_process( thread_id );
*/

    if ((count > get_read_size(count)) && (instance->f_flags & O_NONBLOCK) )
        return -EAGAIN;

    printk(KERN_INFO "before wait\n");

    if (wait_event_interruptible(read_wq, get_read_size(count) >= count))
        return -ERESTARTSYS;

    printk(KERN_INFO "before sem\n");

    if (down_interruptible(&sem)) {
        printk(KERN_ERR "Could not aquire semaphore\n");
        return -1;
    }

    printk(KERN_INFO "after sem\n");
    while (red < count)
    {
        printk(KERN_INFO "in sem\n");
        to_read = (read->size) - curr_index;
        to_read = min(to_read, count);
        not_red = copy_to_user(buff, &(read->buf[curr_index]), to_read);
        curr_index += to_read - not_red;
        if ( not_red ) {
            up(&sem);
            return red + (to_read - not_red);
        }
        red += to_read;
        if (curr_index == (read->size))
        {
            read = free_elem(read);
            curr_index = 0;
        }

        i++;
        if (i > 20)
            return 0;
    }
    up(&sem);

    return red;
}


static ssize_t driver_write(struct file *instance, const char __user *buff, size_t count, loff_t *offp)
{
    size_t not_written = 0;
    if (down_interruptible(&sem)) {
        printk(KERN_ERR "Could not aquire semaphore\n");
        return -1;
    }

    head->buf = kmalloc(count,GFP_KERNEL);
    if (head->buf == NULL)
        return -EIO;

    not_written = copy_from_user(head->buf,buff,count);
    if (not_written)
        return -EIO;

    head->size = count;
    head = get_elem(head);

    up(&sem);
    wake_up_interruptible(&read_wq);

    return count - not_written;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    .write = driver_write
    /*.poll = driver_poll,*/
};

static liste_t* free_elem(liste_t *ptr)
{
    liste_t *ptr2;
    ptr2 = ptr;
    kfree(ptr->buf);
    kfree(ptr);
    return ptr2;
}

static liste_t* get_elem(liste_t *n)
{
    liste_t *head = kmalloc(sizeof(liste_t),GFP_KERNEL);
    if (head != NULL)
    {
        head->buf = NULL;
        head->size = 0;
        head->next = n;
    }

    return head;
}

static size_t get_read_size(size_t count)
{
    ssize_t size = (read->size)-curr_index;
    while (read->next != NULL)
    {
        read = read->next;
        size += read->size;
    }

    printk(KERN_INFO "get_read_size: %d/%d\n", size, count);

    return size;
}

module_init( mod_setup );
module_exit( mod_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser erstes Modul");
