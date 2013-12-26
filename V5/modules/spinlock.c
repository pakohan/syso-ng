#define LINUX
#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/spinlock.h>

static struct file_operations fops;
static struct class *cl1;
static dev_t first;
static struct cdev c_dev;
static spinlock_t lock = SPIN_LOCK_UNLOCKED;
static unsigned long flags;

static int __init mod_setup(void)
{
    if (alloc_chrdev_region(&first, 0, 1, "spinlock.c") < 0)
        goto error_exit;

    if ((cl1 = class_create(THIS_MODULE, "chardrv_spinlock")) == NULL)
        goto error_class_create;

    if (device_create(cl1, NULL, MKDEV(MAJOR(first), MINOR(first)), NULL, "mod_spinlock") == NULL)
        goto error_device_create;

    cdev_init(&c_dev, &fops);

    if (cdev_add(&c_dev, first, 1) == -1)
        goto error_cdev_add;

    printk(KERN_INFO "module loaded\nTODO: SPINLOCKS?!");
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
    cdev_del(&c_dev);
    device_destroy(cl1, MKDEV(MAJOR(first), MINOR(first)));
    class_destroy(cl1);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "module removed\n");
}

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


static int driver_close( struct inode *device, struct file *instance )
{
    printk(KERN_INFO "Close file called\n");
    return 0;
}

static ssize_t driver_read( struct file *instance, char *user, size_t count, loff_t *offset )
{
    printk(KERN_INFO "read called\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
};

module_init( mod_setup );
module_exit( mod_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser fuenftes Modul");
