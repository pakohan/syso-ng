#define MODULE
#define LINUX
#define __KERNEL__

/*
### open() und close() Funktion

#### Ziele

  * Bedeutung und Aufbau der Open- und Close-Funktionen kennen lernen
  * Open- und Close-Funktionen kodieren können
  * Zugriff auf Treiber mit Hilfe selbst geschriebener Applikationen.

#### Vorbereitung

Quelldatei erstellen:

  * Erstellen Sie auf Basis des Templates eine Datei namens ```openclose.c```.

Makefile anpassen:

  * Modifizieren Sie das Makefile. Ändern Sie den Namen für die zu compilierende Datei in ```openclose```.

#### Durchführung

  1. Erweitern Sie das Codefragment um eine Open-Funktion, die nur eine ```printk```-Ausgabe macht.
  2. Erweitern Sie den Code um eine Release-Funktion. Auch diese soll nur eine ```printk``` Ausgabe machen.
  3. Starten Sie den Generierungsprozess durch Eingabe von **make**.
  4. Greifen Sie auf die 'Gerätedatei' (und damit auf den Treiber) mit dem Systemprogramm **cat** zu.
  5. Überwachen Sie die Ausgaben des Treibers mit Hilfe von tail -f /var/log/kern.log
  6. Erweitern Sie Ihren Treiber um einen Zugriffsschutz: Zu einem Zeitpunkt darf immer nur ein Rechenprozess zugreifen. Testen Sie 2 Mechanismen
    1. 'normale' Variable open_count: Für die Realisierung des Zugriffsschutzes verwenden Sie eine globale Variable (entweder als Flag oder als Zähler)
    2. 'atomic' Variable: Der Zugriff auf die globale Variable stellt einen kritischen Abschnitt dar! Mehrere Rechenprozesse können gleichzeitig die
        Variable überprüfen und/oder verändern. Linux bietet zum Schutz der Variable den Datentyp „atomic_t“ an. Die hier notwendigen Funktionen lauten:
      * int atomic_inc_and_test( atomic_t *v );
      * void atomic_dec( atomic_t*v);
    3. Instrumentieren Sie den Code so, dass Sie geeignete DEBUG Ausgaben erhalten.
  7. Laden Sie Ihren Treiber und testen Sie diesen mit Hilfe des Systemkommandos **cat**.
  8. Erstellen Sie eine eigene Applikation (mit Hilfe der Systemcalls **open**, **close**, **read** und **write**).
  9. Testen Sie den Treiber mit Hilfe der selbst erstellten Applikation (access.c).
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
        if (alloc_chrdev_region(&first, 0, 1, "openclose.c") < 0) goto error_exit;

        if ((cl = class_create(THIS_MODULE, "chardrv2")) == NULL) goto error_class_create;

        if (device_create(cl, NULL, first, NULL, "mod_4") == NULL) goto error_device_create;

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
        printk(KERN_INFO "read called\n");
        return count;
}

static ssize_t driver_write( struct file *instance, const char *user, size_t count, loff_t *offs )
{
        printk(KERN_INFO "write called\n");
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
MODULE_DESCRIPTION("Unser viertes Modul");
