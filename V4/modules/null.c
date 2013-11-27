#define MODULE
#define LINUX
#define __KERNEL__

#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

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

module_init( hello_setup );
module_exit( hello_cleanup );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser erstes Modul");
