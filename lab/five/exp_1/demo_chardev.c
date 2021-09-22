#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/init.h>
#include<linux/cdev.h>

#define DEV_NAME "demo_chardev"

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

static int __init simple_char_init(void) {
    int ret;
    //1. 分配设备号
    ret = alloc_chrdev_region(&dev, 0, count, DEV_NAME);
    if (ret) {
        printk("failed to allocate char device region");
        return ret;
    }
    //2. 分配cdev结构体
    demo_cdev = cdev_alloc();
    if (!demo_cdev) {
        printk("cdev_alloc failed \n");
        goto unregister_chrdev;
    }
    //3. 初始化cdev结构体,将操作函数加入到设备中
    cdev_init(demo_cdev, &demodrv_fops);
    
    //4. 添加结构体到设备中
    ret = cdev_add(demo_cdev, dev, count);

    if (ret) {
        printk("cdev_add failed \n");
        goto cdev_fail;
    }

    printk("succeeded register char device: %s \n", DEV_NAME);
    printk("major = %d, minor = %d\n", MAJOR(dev), MINOR(dev));
    return 0;

cdev_fail:
    cdev_del(demo_cdev);

unregister_chrdev:
    unregister_chrdev_region(dev, count);
    return ret;
}

static void __exit simple_char_exit(void) {
    printk("remove device\n");
    if (demo_cdev) cdev_del(demo_cdev);

    unregister_chrdev_region(dev, count);
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_AUTHOR("Peng Jinghui");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("demo_chardev");