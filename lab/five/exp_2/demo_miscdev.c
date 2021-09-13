#include<linux/miscdevice.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/init.h>
#include<linux/cdev.h>

#define DEV_NAME "demo_miscdev"

#define DEMO_NAME "demo_misc"



static dev_t dev;
static struct cdev *demo_cdev;
static signed count = 1;

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
demodrv_read(struct file *file, char __user *buf, size_t count,loff_t *ppos) {
    printk("%s enter \n", __func__);
    return 0;
}

static ssize_t 
demodrv_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos) {
    printk("%s enter \n", __func__);
    return 0;
}

static const struct file_operations demodrv_fops = {
    .owner = THIS_MODULE,
    .open = demodrv_open,
    .release = demodrv_release,
    .read = demodrv_read,
    .write = demodrv_write
};

static struct device *demo_misc_device;
static struct miscdevice demo_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEMO_NAME,
    .fops = &demodrv_fops,
};

static int __init simple_char_init(void) {
    int ret;
    ret = misc_register(&demo_miscdev);
    if (ret) {
        printk("failed register misc device\n");
        return ret;
    }
    demo_misc_device = demo_miscdev.this_device;
    printk("succeeded register char device: %s \n", DEMO_NAME);
    return 0;
}

static void __exit simple_char_exit(void) {
    printk("remove device\n");
    misc_deregister(&demo_miscdev);
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_AUTHOR("Peng Jinghui");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("demo_chardev");