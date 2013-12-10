#define LINUX

/*
### Virtuelles Gerät (```/dev/zero```)

#### Ziele

  * Implementierung des original ```/dev/zero``` Gerätes,
  * Auswertung der Minor Number.

#### Vorbereitung

Quelldatei erstellen:

* Erstellen Sie auf Basis des Templates eine Datei namens ```zero.c```.

Makefile anpassen:

* Modifizieren Sie das Makefile. Ändern Sie den Namen für die zu compilierende Datei in ```zero```.


* Was macht das Device ```/dev/zero```?

#### Durchführung

  1. Ergänzen Sie Ihren Treiber um eine driver_read()-Funktion, die das virtuelle Gerät "/dev/zero" implementiert. Dieses Gerät soll bei Leseanfragen jeweils eine 0 zurückgeben.
  2. Testen Sie den Treiber mit Hilfe des cat- und des hexdump-Kommandos:
    ```cat geraetedatei | hexdump``
  3. Die Lesefunktion soll '0' liefern, wenn die Minornummer »0« beträgt. Wird der Treiber mit einer Minornummer »1« aufgerufen, gibt er "hello world" zurück.
  4. Testen Sie den Treiber mit Hilfe des cat-Kommandos.
  5. Begründen Sie, warum cat den String wiederholt ausgibt.
  6. Erweitern Sie Ihre Applikation, um den String "hello world" vom Treiber zu lesen.
*/

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

static struct file_operations fops;
static dev_t first;
static struct class *cl1;
static struct cdev c_dev;
//static atomic_t sem = ATOMIC_INIT(0);
static wait_queue_head_t wq; 
static int condition = 0;
//static char* buffer = NULL;
static char buffer[100];

static int __init mod_setup(void)
{
	int i = 0;
	if (alloc_chrdev_region(&first, 0, NUMBER_DEVICES, "buf.c") < 0)
		goto error_exit;

	if ((cl1 = class_create(THIS_MODULE, "chardrv_buf")) == NULL)
		goto error_class_create;

	for(i = 0; i < NUMBER_DEVICES; i++)
	{
		if (device_create(cl1, NULL, MKDEV(MAJOR(first), MINOR(first)+i),
	    	NULL, "mod_buf%d", i) == NULL)
	    	goto error_device_create;
	}
	
	init_waitqueue_head( &wq );

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
    	for(i = 0; i < NUMBER_DEVICES; i++)
    	{
		device_destroy(cl1, MKDEV(MAJOR(first), MINOR(first)+i));
    	}
	class_destroy(cl1);
	unregister_chrdev_region(first, NUMBER_DEVICES);
	/*if(buffer != NULL)
	{
		kfree( buffer );
	}*/
	printk(KERN_INFO "module removed\n");
}

static int driver_open( struct inode *device, struct file *instance )
{
	printk(KERN_INFO "Open file called\n");
	//if (atomic_inc_return(&sem) > 1) {
	//	atomic_dec(&sem);
	//	printk(KERN_ERR "File is already opened by another process\n");
	//	return -1;
	//}
	printk(KERN_INFO "File has been opened\n");
	return 0;
}

static int driver_close( struct inode *device, struct file *instance )
{
	printk(KERN_INFO "Close file called\n");
	//atomic_dec(&sem);
	return 0;
}

static ssize_t driver_read(struct file *filp, char __user *buff,
    size_t count, loff_t *offp)
{
	int bufsize = 0;
	if((strcmp(buffer,"")) == 0)
		wait_event_interruptible(wq, condition);

	condition = 0;
	bufsize = strlen( buffer )+1;
	copy_to_user( buff, buffer, bufsize );
		
	//kfree( buffer );
	strcpy(buffer,"");
	return bufsize;
}

ssize_t driver_write(struct file *filp, const char __user *buff,
    size_t count, loff_t *offp)
{
	/*if( buffer == "" )
	{
		buffer = kmalloc( count,GFP_KERNEL );
		if( buffer == NULL )
			goto end;
	}*/

	if (copy_from_user(buffer, buff, count))
	{
		//kfree(buffer);
		goto out;
	}

	condition = 1;
	wake_up( &wq );
	

	return 0;

/*end:
	printk(KERN_ERR "No memory allocated\n");
	return -1;
*/
out:
	kfree( buffer );
	printk(KERN_ERR "Can't copy from user\n");
	return -EFAULT;
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
