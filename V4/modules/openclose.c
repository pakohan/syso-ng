#define MODULE
#define LINUX
#define __KERNEL__

#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

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
    2. 'atomic' Variable: Der Zugriff auf die globale Variable stellt einen kritischen Abschnitt dar! Mehrere Rechenprozesse können gleichzeitig die Variable überprüfen und/oder verändern. Linux bietet zum Schutz der Variable den Datentyp „atomic_t“ an. Die hier notwendigen Funktionen lauten:
      * int atomic_inc_and_test( atomic_t *v );
      * void atomic_dec( atomic_t*v);
    3. Instrumentieren Sie den Code so, dass Sie geeignete DEBUG Ausgaben erhalten.
  7. Laden Sie Ihren Treiber und testen Sie diesen mit Hilfe des Systemkommandos **cat**.
  8. Erstellen Sie eine eigene Applikation (mit Hilfe der Systemcalls **open**, **close**, **read** und **write**).
  9. Testen Sie den Treiber mit Hilfe der selbst erstellten Applikation (access.c).
*/

static int major;
static struct file_operations fops;

static int __init hello_setup(void)
{
	if((major=register_chrdev(0,"TestDriver",&fops))==0) {
		return -EIO;
	}
	printk("<1>init_module called: %d\n", major);
	return 0;
}

static void __exit hello_cleanup(void)
{
	unregister_chrdev( major, "TestDriver" );
	printk("<1>cleanup_module called\n");
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
