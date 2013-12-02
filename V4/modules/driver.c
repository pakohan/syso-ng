#define MODULE
#define LINUX
#define __KERNEL__
/*
### Vom Modul zum Treiber

#### Ziele

  * Sie erstellen ein Modul-Template.
  * Sie erlangen Verständnis für die Linux-Code-Besonderheiten.

#### Vorbereitung

  * Quelldatei erstellen: Kopieren Sie das in der vorhergehenden Übung erstellte Template in die Datei treiber.c
  * Makefile anpassen: Modifizieren Sie das Makefile. Ändern Sie den Namen für die zu compilierende Datei in treiber

#### Durchführung

**HINWEIS!** Die notwendigen Prototypen sind in der Headerdatei ```<linux/fs.h>`` deklariert.

  1. Teiber anmelden (Klassisches Verfahren)
    1. Erweitern Sie den Code in treiber.c um die Funktion, die das Modul beim Kernel als Treiber anmeldet (register_chrdev()).
    2. Erweitern Sie den Code in treiber.c um die Funktion, die das Modul beim Kernel als Treiber wieder abmeldet (unregister_chrdev()).
    3. Ergänzen Sie den Code um geeignete „printk()“-Statements, die darüber Aufschluss geben, ob das Anmelden erfolgreich abgelaufen ist oder nicht.
    4. Generieren und testen Sie das Modul.
    5. Überprüfen Sie, ob sich Ihr Modul erfolgreich als Treiber angemeldet hat, indem Sie sich die geladenen Treiber durch Aufruf des
        Kommandos ```cat /proc/devices``` ansehen. Beobachten Sie außerdem die Ausgaben im Syslog.
  2. Melden Sie den Treiber nach der 'Modern Art' im System an.
    1. Lassen Sie sich zusätzlich die Major-Nummer ausgeben.
    2. Generieren und testen Sie das Modul.
    3. Konfigurieren Sie Ihr Target System so, dass Gerätedateien automatisch angelegt werden.
      * Registrieren Sie z.B. mdev als Hotplug-Manager (beim Booten) im Kernel bevor das Modul geladen wird:

     **echo "/sbin/mdev" > /proc/sys/kernel/hotplug**
    4. Welche Zugriffsmodi hat die zugehörige Gerätedatei?
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
    if (alloc_chrdev_region(&first, 0, 1, "driver.c") < 0) goto error_exit;

    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL) goto error_class_create;

    if (device_create(cl, NULL, first, NULL, "mod_3") == NULL) goto error_device_create;

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

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    /*.read = driver_read,
    .write = driver_write,
    .poll = driver_poll,*/
};

module_init( mod_setup );
module_exit( mod_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser drittes Modul");
