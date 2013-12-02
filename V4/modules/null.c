#define MODULE
#define LINUX
#define __KERNEL__

/*
### Schreibfunktion (```/dev/null```)

#### Ziele

  * Den schreibende Zugriff auf den Treiber realisieren.
  * Aufbau und Funktion des virtuellen Geräts »/dev/null« kennen lernen

#### Vorbereitung

Quelldatei erstellen:

  * Erstellen Sie auf Basis des Templates eine Datei namens ```null.c```.

Makefile anpassen:

  * Modifizieren Sie das Makefile. Ändern Sie den Namen für die zu compilierende Datei in ```null```.

#### Durchführung

  1. Erweitern Sie das Codefragment um eine Schreib-Funktion, die alle übergebenen (geschriebenen) Daten »verschluckt«.
  2. Instrumentieren Sie den Code so, dass Sie geeignete DEBUG Ausgaben erhalten.
  3. Testen Sie den Treiber mit dem Systemprogramm **echo**.
  4. Testen Sie den Treiber durch Umlenken der Ausgabe von **cat**:

     $ cat /etc/motd > /dev/mydevice
     $ tail -f /var/log/kern.log
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

static int __init mod_setup(void)
{
        if (alloc_chrdev_region(&first, 0, 1, "null.c") < 0) goto error_exit;

        if ((cl = class_create(THIS_MODULE, "chardrv5")) == NULL) goto error_class_create;

        if (device_create(cl, NULL, first, NULL, "mod_7") == NULL) goto error_device_create;

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
        return 0;
}

static int driver_close( struct inode *device, struct file *instance )
{
        printk(KERN_INFO "Close file called\n");
        return 0;
}

static ssize_t driver_read( struct file *instance, char *user, size_t count, loff_t *offset )
{
        printk(KERN_INFO "give this guy a zero!\n");
        char *hello_world = "\0";
        copy_to_user( user, hello_world, 1);
        return 1;
}

static ssize_t driver_write( struct file *instance, char *user, size_t count, loff_t *offset )
{
        printk(KERN_INFO "ate %d bytes\n", count);
        return count;
}

static struct file_operations fops = {
        .owner = THIS_MODULE,
        .open = driver_open,
        .release = driver_close,
        .read = driver_read,
        .write = driver_write,
        /*.poll = driver_poll,*/
};

module_init( mod_setup );
module_exit( mod_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser erstes Modul");
