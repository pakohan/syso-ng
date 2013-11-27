#define MODULE
#define LINUX
#define __KERNEL__

#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

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
