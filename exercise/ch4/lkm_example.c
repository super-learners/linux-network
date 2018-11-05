#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/notifier.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ezekiel Lee");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

#define DEVICE_NAME "lkm_example"

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int major_num;
static int device_open_count = 0;

static BLOCKING_NOTIFIER_HEAD(lkm_example_chain);

static struct file_operations file_ops = {
   .read = device_read,
   .write = device_write,
   .open = device_open,
   .release = device_release
};

static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
    blocking_notifier_call_chain(&lkm_example_chain, 1, NULL);

    return 0;
}

static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) {
    blocking_notifier_call_chain(&lkm_example_chain, 2, NULL);

    return -EINVAL;
}

static int device_open(struct inode *inode, struct file *file) {
    if (device_open_count) {
        return -EBUSY;
    }
    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static int lkm_example_event(struct notifier_block *nb,
                            unsigned long event,
                            void *ptr)
{
    printk(KERN_INFO "Event %lu is fired!\n", event);

    return NOTIFY_STOP;
}

static struct notifier_block lkm_example_notifier = {
    .notifier_call = lkm_example_event,
};

static int __init lkm_example_init(void) {
    major_num = register_chrdev(0, "lkm_example", &file_ops);
    if (major_num < 0) {
        printk(KERN_ALERT "Could not register device: %d\n", major_num);
        return major_num;
    } else {
        printk(KERN_INFO "lkm_example module loaded with device major number %d\n", major_num);
        blocking_notifier_chain_register(&lkm_example_chain, &lkm_example_notifier);
        return 0;
    }
}

static void __exit lkm_example_exit(void) {
    unregister_chrdev(major_num, DEVICE_NAME);
    blocking_notifier_chain_unregister(&lkm_example_chain, &lkm_example_notifier);
    printk(KERN_INFO "Goodbye, World!\n");
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);
