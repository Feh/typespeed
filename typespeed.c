#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/timer.h>
#include <linux/mutex.h>

/* Our internal stats, protected by a single mutex */

struct mutex lock;
static int t;
static size_t events;
#define LENGTH 60
static size_t interval[LENGTH];

/* Per-second callback for rotation */

static struct timer_list timer;

static void timer_callback(unsigned long data)
{
    mutex_lock(&lock);
    interval[t] = events;
    t = (t + 1) % LENGTH;
    mutex_unlock(&lock);

    events = 0;

    /* We’ll be back… in about a second! */
    mod_timer(&timer, jiffies + msecs_to_jiffies(1000));
}

/* Input handling */

static const struct input_device_id typespeed_ids[] = {
    /* This is just ... keyboards? Let's hope. */
    {
        .flags = INPUT_DEVICE_ID_MATCH_EVBIT,
        .evbit = { BIT_MASK(EV_KEY) },
    },
    { },
};

static int typespeed_connect(struct input_handler *handler,
             struct input_dev *dev,
             const struct input_device_id *id)
{
    struct input_handle *handle;

    handle = kzalloc(sizeof(*handle), GFP_KERNEL);
    if(!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "typespeed";

    if(input_register_handle(handle)) {
        printk("Failed to register input handle");
        goto err;
    }

    if(input_open_device(handle)) {
        printk("Failed to open input device");
        goto unregister;
    }

    return 0;

unregister:
    input_unregister_handle(handle);
err:
    return -1;
}

static void typespeed_disconnect(struct input_handle *handle)
{
    input_close_device(handle);
    input_unregister_handle(handle);
}

static void typespeed_event(struct input_handle *handle,
             unsigned int type, unsigned int code, int value)
{
    if(type != EV_KEY || code == 0 || code >= 128)
        return;

    /* value: 0 = key up, 1 = key press, 2 = key hold? */
    if(value != 1)
        return;

    /* Ignore modifier keys */
    if(code == KEY_RIGHTSHIFT || code == KEY_LEFTSHIFT ||
        code == KEY_RIGHTCTRL || code == KEY_LEFTCTRL ||
        code == KEY_RIGHTALT || code == KEY_LEFTALT ||
        code == KEY_CAPSLOCK)
        return;

    printk("Event: type=%d, code=%d, value=%d\n", type, code, value);
    events++;
}

static struct input_handler typespeed_input_handler = {
    .connect    = typespeed_connect,
    .disconnect = typespeed_disconnect,
    .event      = typespeed_event,
    .name       = "typespeed",
    .id_table   = typespeed_ids,
};

/* /proc file handling */

static int typespeed_proc_show(struct seq_file *m, void *v)
{
    int i;
    size_t sum10 = 0;
    size_t sum30;
    size_t sum;

    mutex_lock(&lock);

    for(i = 0; i < 10; i++)
        sum10 += interval[(LENGTH + t - i)%LENGTH];

    sum30 = sum10;
    for(; i < 30; i++)
        sum30 += interval[(LENGTH + t - i)%LENGTH];

    sum = sum30;
    for(; i < LENGTH; i++)
        sum += interval[(LENGTH + t - i)%LENGTH];

    mutex_unlock(&lock);

    seq_printf(m, "%zd %zd %zd\n",
        sum10 * LENGTH / 10, sum30 * LENGTH / 30, sum);
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

/* Initialization & Exiting */

static int __init typespeed_init(void)
{
    int error;

    mutex_init(&lock);

    setup_timer(&timer, timer_callback, 0);
    mod_timer(&timer, jiffies + msecs_to_jiffies(1000));

    proc_create("typespeed", 0, NULL, &typespeed_proc_fops);

    if((error = input_register_handler(&typespeed_input_handler)))
        printk("Failed to register input handler, error: %d", error);

    printk("Typespeed successfully initialized! Type on!\n");
    return 0;
}

static void __exit typespeed_exit(void)
{
    del_timer(&timer);
    remove_proc_entry("typespeed", NULL);
    input_unregister_handler(&typespeed_input_handler);
    printk ("Typespeed says good-bye.\n");
    return;
}

module_init(typespeed_init);
module_exit(typespeed_exit);

MODULE_LICENSE("GPL");
