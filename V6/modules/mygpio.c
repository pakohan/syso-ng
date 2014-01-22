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
#include <asm/io.h>

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
