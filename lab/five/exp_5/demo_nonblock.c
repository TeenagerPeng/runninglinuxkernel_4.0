#include<linux/miscdevice.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include<linux/kfifo.h>


#define DEV_NAME "demo_miscdev"

#define DEMO_NAME "demo_nonblock"
#define MAX_DEVICE_BUFFER_SIZE 64

DEFINE_KFIFO(kfifo_buffer, char, 64);



static dev_t dev;
static struct cdev *demo_cdev;
static signed count = 1;

static char *device_buffer;



static struct device *demo_device;



static int 
demodrv_open(struct inode *inode, struct file *file) {
    int major = MAJOR(inode->i_rdev);
    int minor = MINOR(inode->i_rdev);
    printk("%s: major=%d, minor = %d \n", __func__, major, minor);
    return 0;
}

static int 
demodrv_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t 
demodrv_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    int actual_readed;
    int ret;
    if (kfifo_is_empty(&kfifo_buffer)) {
        if (file->f_flags & O_NONBLOCK) {
            return -EAGAIN;
        }
    }
    ret = kfifo_to_user(&kfifo_buffer, buf, count, &actual_readed);
    if (ret) return -EIO;
    printk("%s, actual_readed = %d, pos = %lld\n", __func__, actual_readed, *ppos);
    return actual_readed;
}

static ssize_t 
demodrv_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    unsigned int actual_write;
    int ret;
    if (kfifo_is_full(&kfifo_buffer)) {
        if (file->f_flags & O_NONBLOCK) {
            return -EAGAIN;
        }
    }
    ret = kfifo_from_user(&kfifo_buffer, buf, count, &actual_write);
    printk("%s, actual_write = %d, ppos = %lld\n", __func__, actual_write, *ppos);
    return actual_write;
}

static const struct file_operations demodrv_fops = {
    .owner = THIS_MODULE,
    .open = demodrv_open,
    .release = demodrv_release,
    .read = demodrv_read,
    .write = demodrv_write
};

static struct miscdevice demo_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEMO_NAME,
    .fops = &demodrv_fops,
};

static int __init simple_char_init(void) {
    int ret;
    device_buffer = kmalloc(MAX_DEVICE_BUFFER_SIZE, GFP_KERNEL);
    if (!device_buffer) {
        return -ENOMEM;
    }
    ret = misc_register(&demo_miscdev);
    if (ret) {
        printk("failed register misc device\n");
        kfree(device_buffer);
        return ret;
    }
    demo_device = demo_miscdev.this_device;
    printk("succeeded register char device: %s \n", DEMO_NAME);
    return 0;
}

static void __exit simple_char_exit(void) {
    printk("remove device\n");
    kfree(device_buffer);
    misc_deregister(&demo_miscdev);
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_AUTHOR("Peng Jinghui");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("demo_chardev");