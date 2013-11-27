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
    5. Überprüfen Sie, ob sich Ihr Modul erfolgreich als Treiber angemeldet hat, indem Sie sich die geladenen Treiber durch Aufruf des Kommandos ** # cat /proc/devices** ansehen. Beobachten Sie außerdem die Ausgaben im Syslog.
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

static int __init hello_setup(void)
{
	if((major=register_chrdev(0,"TestDriver",&fops))==0) {
		return -EIO;
	}
	printk(KERN_INFO "init_module called: %d\n", major);
	return 0;
}

static void __exit hello_cleanup(void)
{
	unregister_chrdev( major, "TestDriver" );
	printk(KERN_INFO "cleanup_module called\n");
}

static int driver_open( struct inode *device, struct file *instance )
{
	return 0;
}

static int driver_close( struct inode *device, struct file *instance )
{
	return 0;
}

static ssize_t driver_read( struct file *instance, char *user, size_t count, loff_t *offset )
{
	int not_copied, to_copy;
	char* hello_world = "hello world";

	to_copy = strlen(hello_world)+1;
	to_copy = min( to_copy, count );
	not_copied = copy_to_user( user, hello_world, to_copy);
	return to_copy-not_copied;
}

static ssize_t driver_write( struct file *instanz, const char *user, size_t count, loff_t *offs )
{
	int to_copy;
	int not_copied;
	char buf[10];

	to_copy = min( 10, count);
	not_copied = copy_from_user( buf, user, to_copy );
	printk(">%s<", buf );
	return to_copy-not_copied;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.read = driver_read,
	.write = driver_write,
	/*.poll = driver_poll,*/
};

module_init( hello_setup );
module_exit( hello_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser erstes Modul");
