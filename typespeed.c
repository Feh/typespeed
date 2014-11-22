#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/input.h>

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

struct key_code {
    unsigned char code;
    char *desc;
};
static struct key_code c2d[] = {
    { KEY_GRAVE, "KEY_GRAVE", },
    { KEY_ESC, "KEY_ESC", },
    { KEY_1, "KEY_1", },
    { KEY_2, "KEY_2", },
    { KEY_3, "KEY_3", },
    { KEY_4, "KEY_4", },
    { KEY_5, "KEY_5", },
    { KEY_6, "KEY_6", },
    { KEY_7, "KEY_7", },
    { KEY_8, "KEY_8", },
    { KEY_9, "KEY_9", },
    { KEY_0, "KEY_0", },
    { KEY_MINUS, "KEY_MINUS", },
    { KEY_EQUAL, "KEY_EQUAL", },
    { KEY_BACKSPACE, "KEY_BACKSPACE", },
    { KEY_TAB, "KEY_TAB", },
    { KEY_Q, "KEY_Q", },
    { KEY_W, "KEY_W", },
    { KEY_E, "KEY_E", },
    { KEY_R, "KEY_R", },
    { KEY_T, "KEY_T", },
    { KEY_Y, "KEY_Y", },
    { KEY_U, "KEY_U", },
    { KEY_I, "KEY_I", },
    { KEY_O, "KEY_O", },
    { KEY_P, "KEY_P", },
    { KEY_LEFTBRACE, "KEY_LEFTBRACE", },
    { KEY_RIGHTBRACE, "KEY_RIGHTBRACE", },
    { KEY_ENTER, "KEY_ENTER", },
    { KEY_LEFTCTRL, "KEY_LEFTCTRL", },
    { KEY_A, "KEY_A", },
    { KEY_S, "KEY_S", },
    { KEY_D, "KEY_D", },
    { KEY_F, "KEY_F", },
    { KEY_G, "KEY_G", },
    { KEY_H, "KEY_H", },
    { KEY_J, "KEY_J", },
    { KEY_K, "KEY_K", },
    { KEY_L, "KEY_L", },
    { KEY_SEMICOLON, "KEY_SEMICOLON", },
    { KEY_APOSTROPHE, "KEY_APOSTROPHE", },
    { KEY_BACKSLASH, "KEY_BACKSLASH", },
    { KEY_LEFTSHIFT, "KEY_LEFTSHIFT", },
    { KEY_GRAVE, "KEY_GRAVE", },
    { KEY_Z, "KEY_Z", },
    { KEY_X, "KEY_X", },
    { KEY_C, "KEY_C", },
    { KEY_V, "KEY_V", },
    { KEY_B, "KEY_B", },
    { KEY_N, "KEY_N", },
    { KEY_M, "KEY_M", },
    { KEY_COMMA, "KEY_COMMA", },
    { KEY_DOT, "KEY_DOT", },
    { KEY_SLASH, "KEY_SLASH", },
    { KEY_RIGHTSHIFT, "KEY_RIGHTSHIFT", },
    { KEY_KPASTERISK, "KEY_KPASTERISK", },
    { KEY_LEFTALT, "KEY_LEFTALT", },
    { KEY_SPACE, "KEY_SPACE", },
    { KEY_CAPSLOCK, "KEY_CAPSLOCK", },
    { KEY_F1, "KEY_F1", },
    { KEY_F2, "KEY_F2", },
    { KEY_F3, "KEY_F3", },
    { KEY_F4, "KEY_F4", },
    { KEY_F5, "KEY_F5", },
    { KEY_F6, "KEY_F6", },
    { KEY_F7, "KEY_F7", },
    { KEY_F8, "KEY_F8", },
    { KEY_F9, "KEY_F9", },
    { KEY_F10, "KEY_F10", },
    { KEY_ESC, "KEY_ESC", },
    { KEY_DELETE, "KEY_DELETE", },
    { KEY_KP7, "KEY_KP7", },
    { KEY_KP8, "KEY_KP8", },
    { KEY_KP9, "KEY_KP9", },
    { KEY_KPMINUS, "KEY_KPMINUS", },
    { KEY_KP4, "KEY_KP4", },
    { KEY_KP5, "KEY_KP5", },
    { KEY_KP6, "KEY_KP6", },
    { KEY_KPPLUS, "KEY_KPPLUS", },
    { KEY_KP1, "KEY_KP1", },
    { KEY_KP2, "KEY_KP2", },
    { KEY_KP3, "KEY_KP3", },
    { KEY_KP0, "KEY_KP0", },
    { KEY_KPDOT, "KEY_KPDOT", },
    { KEY_KPLEFTPAREN, "KEY_KPLEFTPAREN", },
    { KEY_KPRIGHTPAREN, "KEY_KPRIGHTPAREN", },
    { KEY_KPASTERISK, "KEY_KPASTERISK", },
    { KEY_KPASTERISK, "KEY_KPASTERISK", },
    { KEY_KPPLUS, "KEY_KPPLUS", },
    { KEY_HELP, "KEY_HELP", },
    { KEY_BACKSLASH, "KEY_BACKSLASH", },
    { KEY_KPASTERISK, "KEY_KPASTERISK", },
    { KEY_KPSLASH, "KEY_KPSLASH", },
    { KEY_KPLEFTPAREN, "KEY_KPLEFTPAREN", },
    { KEY_KPRIGHTPAREN, "KEY_KPRIGHTPAREN", },
    { KEY_KPSLASH, "KEY_KPSLASH", },
    { KEY_KPASTERISK, "KEY_KPASTERISK", },
    { KEY_UP, "KEY_UP", },
    { KEY_KPASTERISK, "KEY_KPASTERISK", },
    { KEY_LEFT, "KEY_LEFT", },
    { KEY_RIGHT, "KEY_RIGHT", },
    { KEY_KPASTERISK, "KEY_KPASTERISK", },
    { KEY_DOWN, "KEY_DOWN", },
    { KEY_KPASTERISK, "KEY_KPASTERISK", },
    { KEY_KPASTERISK, "KEY_KPASTERISK", },
    { KEY_KPASTERISK, "KEY_KPASTERISK", },
    { KEY_KPASTERISK, "KEY_KPASTERISK", },
    { KEY_KPASTERISK, "KEY_KPASTERISK", },
};

static void typespeed_event(struct input_handle *handle,
             unsigned int type, unsigned int code, int value)
{
    int i;

    if(type != EV_KEY)
        return;

    /* value: 0 = key up, 1 = key press, 2 = key hold? */
    if(value != 1)
        return;

    for(i = 0; i < sizeof(c2d)/sizeof(struct key_code); i++) {
        if(c2d[i].code == code)
            printk("Event: type=%d, code=%d, value=%d sym=%s\n",
                type, code, value, c2d[i].desc);
    }
}

static struct input_handler typespeed_input_handler = {
    .connect    = typespeed_connect,
    .disconnect = typespeed_disconnect,
    .event      = typespeed_event,
    .name       = "typespeed",
    .id_table   = typespeed_ids,
};

/* /roc file handling */

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

/* Initialization & Exiting */

static int __init typespeed_init(void)
{
    int error;
    printk("Typespeed loaded!\n");
    proc_create("typespeed", 0, NULL, &typespeed_proc_fops);
    if((error = input_register_handler(&typespeed_input_handler)))
        printk("Failed to register input handler, error: %d", error);
    else
        printk("Successfully registered input handler.");
    return 0;
}

static void __exit typespeed_exit(void)
{
    remove_proc_entry("typespeed", NULL);
    input_unregister_handler(&typespeed_input_handler);
    printk ("Typespeed says good-bye.\n");
    return;
}

module_init(typespeed_init);
module_exit(typespeed_exit);

MODULE_LICENSE("GPL");
