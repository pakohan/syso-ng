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

static struct file_operations fops;
static dev_t first;
static struct class *cl1;
static struct cdev c_dev;

#define NUMBER_PINS 17

static char params[NUMBER_PINS];
module_param_array(params, char, , 0);

static char activated_pins[NUMBER_PINS];

static int getindex(int pin_index);

static int __init mod_setup(void)
{
    int i, index;
    if (alloc_chrdev_region(&first, 0, NUMBER_DEVICES, "buf.c") < 0)
        goto error_exit;

    if ((cl1 = class_create(THIS_MODULE, "chardrv_buf")) == NULL)
        goto error_class_create;

    for (i = 0; i < NUMBER_PINS; i++)
    {
        index = getindex(params[i]);
        if (index > -1) {
            activated_pins[index] = 1;
            if (device_create(cl1, NULL, MKDEV(MAJOR(first), MINOR(first) + i), NULL, "mygpio", params[i]) == NULL)
                goto error_device_create;
        } else {
            activated_pins[index] = 0;
        }
    }

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
    cdev_del(&c_dev);
    for (i = 0; i < NUMBER_PINS; i++)
    {
        index = getindex(params[i]);
        if (index > -1) {
            device_destroy(cl1, MKDEV(MAJOR(first), MINOR(first) + i));
        }
    }
    class_destroy(cl1);
    unregister_chrdev_region(first, NUMBER_DEVICES);
    printk(KERN_INFO "module removed\n");
}

static int driver_open( struct inode *device, struct file *instance )
{
    return 0;
}

static int driver_close( struct inode *device, struct file *instance )
{
    return 0;
}

static size_t get_max_size(size_t to_read, size_t own_ptr, size_t opposite_ptr, size_t buf_size, int flag)
{
    return 0;
}

static ssize_t driver_read(struct file *instance, char __user *buff, size_t count, loff_t *offp)
{
	return 0;
}


static ssize_t driver_write(struct file *instance, const char __user *buff, size_t count, loff_t *offp)
{
	return 0;
}

static int getindex(int pin_index)
{
    int returnVal = 0;
    
    switch (pin_index) {
        case 2:
            return 0;
        case 3:
            return 1;
        case 4:
            return 2;
        case 7:
            return 3;
        case 8:
            return 4;
        case 9:
            return 5;
        case 10:
            return 6;
        case 11:
            return 7;
        case 14:
            return 8;
        case 15:
            return 9;
        case 17:
            return 10;
        case 18:
            return 11;
        case 22:
            return 12;
        case 23:
            return 13;
        case 24:
            return 14;
        case 25:
            return 15;
        case 27:
            return 16;
        default:
            return -1;
    }
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    .write = driver_write

};

module_init( mod_setup );
module_exit( mod_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser GPIO Treiber");
