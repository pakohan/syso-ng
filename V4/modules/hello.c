#define MODULE
#define LINUX
#define __KERNEL__

/*
### Lesefunktion

#### Ziele

  * Lesefunktion im Treiber realisieren
  * Daten zwischen Kernel- und User-Space austauschen

#### Vorbereitung

Quelldatei erstellen:

  * Erstellen Sie auf Basis des Templates eine Datei namens ```hello.c```.

Makefile anpassen:

  * Modifizieren Sie das Makefile. Ändern Sie den Namen für die zu compilierende Datei in ```hello```.

#### Durchführung

  1. Erweitern Sie das Codefragment um eine Lese-Funktion, die beim lesenden Zugriff einer Applikation den String 'Hello World' zurückgibt.
  2. Starten Sie den Generierungsprozess durch Eingabe von **make**.
  3. Testen Sie den Treiber mit dem Systemprogramm **cat**. Ihr Treiber liefert ständig »hello world« zurück. Warum?
  4. Testen Sie den Treiber mit Hilfe einer selbst erstellten Applikation.
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
static struct class *cl;
static struct cdev c_dev;
static atomic_t sem = ATOMIC_INIT(0);

static int __init mod_setup(void)
{
    if (alloc_chrdev_region(&first, 0, 1, "hello.c") < 0) goto error_exit;

    if ((cl = class_create(THIS_MODULE, "chardrv3")) == NULL) goto error_class_create;

    if (device_create(cl, NULL, first, NULL, "mod_5") == NULL) goto error_device_create;

    cdev_init(&c_dev, &fops);

    if (cdev_add(&c_dev, first, 1) == -1) goto error_cdev_add;

    printk(KERN_INFO "module loaded\n");
    return 0;

error_cdev_add:
    device_destroy(cl, first);
error_device_create:
    class_destroy(cl);
error_class_create:
    unregister_chrdev_region(first, 1);
error_exit:
    printk(KERN_ERR "module loading failed\n");
    return -1;
}

static void __exit mod_cleanup(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "module removed\n");
}

static int driver_open( struct inode *device, struct file *instance )
{
    printk(KERN_INFO "Open file called\n");
    if (atomic_inc_return(&sem) > 1)
    {
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
    /*printk(KERN_INFO "read called\n");*/
    int not_copied, to_copy;
    char* hello_world = "hello world";

    to_copy = strlen(hello_world)+1;
    to_copy = min( to_copy, count );
    not_copied = copy_to_user( user, hello_world, to_copy);
    return to_copy-not_copied;
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
MODULE_DESCRIPTION("Unser fuenftes Modul");
