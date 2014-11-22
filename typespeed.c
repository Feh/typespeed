#include <linux/module.h>
#include <linux/init.h>

static int __init typespeed_init(void)
{
    printk ("Typespeed loaded!\n");
    return 0;
}

static void __exit typespeed_exit(void)
{
    printk ("Typespeed says good-bye.\n");
    return;
}

module_init(typespeed_init);
module_exit(typespeed_exit);

MODULE_LICENSE("GPL");
