#define MODULE
#define LINUX
#define __KERNEL__

#include <linux/init.h>
#include <linux/module.h>

/*
###  Modul Template

#### Ziele

  * Erstellen eines Modul-Templates
  * Verständnis für die Linux-Code-Besonderheiten

#### Durchführung

  1. Geben Sie das vorgestellte Code-Template ein und legen Sie dieses unter dem Dateinamen template.c ab.
  2. Modifzieren Sie das Makefile. Ändern Sie den Namen für die zu compilierende Datei von mod1 in template.
  3. Starten Sie den Generierungsprozess durch Eingabe von **make**.
  4. Testen Sie Ihr Modul, indem Sie es laden (**insmod**), anzeigen lassen (**lsmod**) und wieder entfernen (**rmmod**). Beobachten Sie die Ausgaben im Syslog.
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Kohan Tobias Birkle");
MODULE_DESCRIPTION("Unser zweites Modul");

static int __init ModInit(void)
{
        printk(KERN_INFO "module template loaded\n");
        return 0;
}

static void __exit ModExit(void)
{
        printk(KERN_INFO "Goodbye, cruel world (Pink Floyd / Roger Waters hommage???)\n");
}

module_init(ModInit);
module_exit(ModExit);
