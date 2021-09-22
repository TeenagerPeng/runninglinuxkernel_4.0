#include<linux/miscdevice.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include<linux/kfifo.h>
#include <linux/wait.h>
#include <linux/sched.h>


#define DEV_NAME "demo_miscdev"

#define DEMO_NAME "demo_block"
#define MAX_DEVICE_BUFFER_SIZE 64

DEFINE_KFIFO(kfifo_buffer, char, 64);

struct demo_block_device {
    char *name;
    struct device *dev;
    struct miscdevice *misc_dev;
    wait_queue_head_t read_queue;
    wait_queue_head_t write_queue;
};

struct demo_private_data {
    struct demo_block_device *device;
};

static struct demo_block_device *demo_device;





static int 
demodrv_open(struct inode *inode, struct file *file) {
    int major = MAJOR(inode->i_rdev);
    int minor = MINOR(inode->i_rdev);
    printk("%s: major=%d, minor = %d \n", __func__, major, minor);
    struct demo_private_data *data = kmalloc(sizeof(struct demo_private_data), GFP_KERNEL);
    if (!data) return -ENOMEM;
    data->device = demo_device;
    file->private_data = data;
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
    struct demo_private_data *data = file->private_data;
    struct demo_block_device *device = data->device;
    if (kfifo_is_empty(&kfifo_buffer)) {
        if (file->f_flags & O_NONBLOCK) {
            return -EAGAIN; 
        }
        printk("%s: pid=%d, going to sleep\n", __func__, current->pid);
        ret = wait_event_interruptible(device->read_queue, !kfifo_is_empty(&kfifo_buffer));
        if (ret) return ret;
    }
    ret = kfifo_to_user(&kfifo_buffer, buf, count, &actual_readed);
    if (ret) return -EIO;
    if (!kfifo_is_full(&kfifo_buffer)) {
        wake_up_interruptible(&device->write_queue);
    }
    printk("%s, actual_readed = %d, pos = %lld\n", __func__, actual_readed, *ppos);
    return actual_readed;
}

static ssize_t 
demodrv_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    unsigned int actual_write;
    int ret;
    struct demo_private_data *data = file->private_data;
    struct demo_block_device *device = data->device;
    if (kfifo_is_full(&kfifo_buffer)) {
        if (file->f_flags & O_NONBLOCK) {
            return -EAGAIN;
        }
        printk("%s: pid=%d, going to sleep\n", __func__, current->pid);
        ret = wait_event_interruptible(device->write_queue, !kfifo_is_empty(&kfifo_buffer));
        if (ret) return ret;
    }
    ret = kfifo_from_user(&kfifo_buffer, buf, count, &actual_write);
    if (!kfifo_is_empty(&kfifo_buffer)) {
        wake_up_interruptible(&device->read_queue);
    }
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
    struct demo_block_device *device = kmalloc(sizeof(struct demo_block_device), GFP_KERNEL);
    int ret;
    ret = misc_register(&demo_miscdev);
    if (ret) {
		printk("failed register misc device\n");
		goto free_device;
	}
    device->misc_dev = &demo_miscdev;
    device->dev = demo_miscdev.this_device;
    init_waitqueue_head(&device->read_queue);
    init_waitqueue_head(&device->write_queue);
    demo_device = device;
    printk("succeeded register char device: %s \n", DEMO_NAME);
    return 0;
free_device:
    kfree(device);
    return ret;
}

static void __exit simple_char_exit(void) {
    printk("remove device\n");
    misc_deregister(&demo_miscdev);
    struct demo_block_device *device = demo_device;
    kfree(device);
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_AUTHOR("Peng Jinghui");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("demo_chardev");