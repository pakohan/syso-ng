#define MODULE
#define LINUX
#define __KERNEL__

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

static struct file_operations fops;
static dev_t first;
static struct class *cl1;
static struct cdev c_dev;
static atomic_t sem = ATOMIC_INIT(0);
static int number_devices = 2;


static int __init mod_setup(void)
{
    int i = 0;
    if (alloc_chrdev_region(&first, 0, number_devices, "zero.c") < 0)
        goto error_exit;

    if ((cl1 = class_create(THIS_MODULE, "chardrv4")) == NULL)
        goto error_class_create;

    for(i = 0; i < number_devices; i++)
    {
        if (device_create(cl1, NULL, MKDEV(MAJOR(first), MINOR(first)+i),
            NULL, "mod_zero%d", i) == NULL)
            goto error_device_create;
    }

    cdev_init(&c_dev, &fops);

    if (cdev_add(&c_dev, first, number_devices) == -1)
        goto error_cdev_add;

    printk(KERN_INFO "module loaded\n");
    return 0;

error_cdev_add:
    device_destroy(cl1, first);
error_device_create:
    class_destroy(cl1);
error_class_create:
    unregister_chrdev_region(first, number_devices);
error_exit:
    printk(KERN_ERR "module loading failed\n");
    return -1;
}

static void __exit mod_cleanup(void)
{
    int i = 0;
    cdev_del(&c_dev);
    for(i = 0; i < number_devices; i++)
    {
        device_destroy(cl1, MKDEV(MAJOR(first), MINOR(first)+i));
    }
    class_destroy(cl1);
    unregister_chrdev_region(first, number_devices);
    printk(KERN_INFO "module removed\n");
}

static int driver_open( struct inode *device, struct file *instance )
{
    printk(KERN_INFO "Open file called\n");
    if (atomic_inc_return(&sem) > 1) {
            atomic_dec(&sem);
            printk(KERN_ERR "File is already opened by another process\n");
            return -1;
    }
    printk(KERN_INFO "File has been opened\n");
    return 0;
}

static int driver_close( struct inode *device, struct file *instance )
{
    printk(KERN_INFO "Close file called\n");
    atomic_dec(&sem);
    return 0;
}

static ssize_t driver_read( struct file *instance, char *user, size_t count, loff_t *offset )
{
    int minor = iminor( instance->f_dentry->d_inode );

    char *hello_world = "hello world";

    if ( minor == 0 ) 
        hello_world = "0";

    copy_to_user( user, hello_world, strlen( hello_world )+1 );
 
    return strlen( hello_world )+1;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    /*.write = driver_write,
    .poll = driver_poll,*/
};

module_init( mod_setup );
module_exit( mod_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser erstes Modul");
