#define MODULE
#define LINUX
#define __KERNEL__

#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

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

static int major;
static struct file_operations fops;

static int __init mod_setup(void)
{
    dev_t* template_dev_number;
    cdev* driver_object;

    if( alloc_chrdev_region(&template_dev_number, 0,1,TEMPLATE) <  0 )
    {
        printk(KERN_ERR "alloc failed");
        return -EIO;
    }
    
    driver_object = cdev_alloc();
    if( driver_object == NULL )
        goto free_device_number;

    driver_object->owner = THIS_MODULE;
    driver_object->ops = &fops

    if( cdev_add(driver_object, template_dev_number,1) )
        goto free_cdev;

    



    return 0;
/*
    if((major=register_chrdev(0,"driver.c",&fops))==0) {
        printk(KERN_ERR "loading failed");
        return -EIO;
    }
    printk(KERN_INFO "init_module called, got major number: %d\n", major);
    return 0;
*/
}

static void __exit mod_cleanup(void)
{
    unregister_chrdev( major, "driver.c" );
    printk(KERN_INFO "cleanup_module called\n");
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
