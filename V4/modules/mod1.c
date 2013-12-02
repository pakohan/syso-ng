#define MODULE
#define LINUX
#define __KERNEL__

#include <linux/init.h>
#include <linux/module.h>

/*
###  Kernel Modul

#### Ziele

  * Sie lernen den Umgang mit Modulen.
  * Sie lernen den Aufbau einfacher Module kennen.

#### Vorbereitung

##### Debug Ausgaben

Mit dem Befehl ```dmesg``` können Sie sich die letzten Systemmeldungen anzeigen lassen

#### Durchführung

  * Starten Sie einen Editor und geben Sie das vorgestellte Treiber Codegerüst ein. Speichern Sie die editierte Datei unter dem Namen mod1.c
  * Geben Sie im Editor das vorgestellte Makefile ein. Speichern Sie die Eingaben unter dem Dateinamen Makefile ab.

**ACHTUNG!** Bitte auf Groß- und Kleinschreibung achten.

  * Starten Sie den Generierungsprozess durch Eingabe von **make**
  * Testen Sie Ihr Modul, indem Sie es:
    1. laden (**insmod ...**),
    2. anzeigen lassen (**lsmod**),
    3. und wieder entfernen (**rmmod ...**)
  * Beachten Sie die Ausgaben in den Systemmeldungen

**HINWEIS!** Beim Laden des Moduls wird der komplette Modulname angegeben mod1.ko. Beim Entladen dagegen findet nur der Basisname Verwendung (mod1).
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser erstes Modul");

static int __init ModInit(void)
{
    printk(KERN_INFO "module mod1 loaded\n");
    return 0;
}

static void __exit ModExit(void)
{
    printk(KERN_INFO "Goodbye, cruel world (Pink Floyd / Roger Waters hommage???)\n");
}

module_init(ModInit);
module_exit(ModExit);
