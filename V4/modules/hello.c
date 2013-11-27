#define MODULE
#define LINUX
#define __KERNEL__

#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

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
	char* hello_world = "hello world\n";

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

module_init( hello_setup );
module_exit( hello_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser fuenftes Modul");