#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static int typespeed_proc_show(struct seq_file *m, void *v)
{
    char string[] = "Can you read me?";

    seq_printf(m, "%s\n", string);
    return 0;
}

static int typespeed_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, typespeed_proc_show, NULL);
}

static const struct file_operations typespeed_proc_fops = {
    .open       = typespeed_proc_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};

static int __init typespeed_init(void)
{
    printk ("Typespeed loaded!\n");
    proc_create("typespeed", 0, NULL, &typespeed_proc_fops);
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
