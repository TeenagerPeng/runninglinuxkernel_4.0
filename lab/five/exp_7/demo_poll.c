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
#include <linux/poll.h>


#define DEV_NAME "demo_miscdev"

#define DEMO_NAME "demo_poll"
#define MAX_DEVICE_BUFFER_SIZE 64

//DEFINE_KFIFO(kfifo_buffer, char, 64);
static dev_t dev;
static struct cdev *demo_cdev;

struct demo_poll_device {
    char name[64];
    struct device *dev;
    struct miscdevice *misc_dev;
    wait_queue_head_t read_queue;
    wait_queue_head_t write_queue;
    struct kfifo kfifo_buffer;
};

struct demo_private_data {
    char name[64];
    struct demo_poll_device *device;
};
#define DEMO_MAX_DEVICES 8

static struct demo_poll_device *demo_device[DEMO_MAX_DEVICES];

static int 
demodrv_open(struct inode *inode, struct file *file) {
    unsigned int minor = iminor(inode);
    //printk("%s: major=%d, minor = %d \n", __func__, major, minor);
    
    struct demo_private_data *data = kmalloc(sizeof(struct demo_private_data), GFP_KERNEL);
    if (!data) return -ENOMEM;
    sprintf(data->name, "private_data_%d", minor);
    data->device = demo_device[minor];
    file->private_data = data;
    printk("%s: major=%d, minor=%d, device=%s\n", __func__, 
			MAJOR(inode->i_rdev), MINOR(inode->i_rdev), demo_device[minor]->name);
    return 0;
}

static int 
demodrv_release(struct inode *inode, struct file *file) {
    struct mydemo_private_data *data = file->private_data;
	
	kfree(data);
    return 0;
}

static ssize_t 
demodrv_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    int actual_readed;
    int ret;
    struct demo_private_data *data = file->private_data;
    struct demo_poll_device *device = data->device;
    if (kfifo_is_empty(&device->kfifo_buffer)) {
        if (file->f_flags & O_NONBLOCK) {
            return -EAGAIN; 
        }
        printk("%s: pid=%d, going to sleep\n", __func__, current->pid);
        ret = wait_event_interruptible(device->read_queue, !kfifo_is_empty(&device->kfifo_buffer));
        if (ret) return ret;
    }
    ret = kfifo_to_user(&device->kfifo_buffer, buf, count, &actual_readed);
    if (ret) return -EIO;
    if (!kfifo_is_full(&device->kfifo_buffer)) {
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
    struct demo_poll_device *device = data->device;
    if (kfifo_is_full(&device->kfifo_buffer)) {
        if (file->f_flags & O_NONBLOCK) {
            return -EAGAIN;
        }
        printk("%s: pid=%d, going to sleep\n", __func__, current->pid);
        ret = wait_event_interruptible(device->write_queue, !kfifo_is_full(&device->kfifo_buffer));
        if (ret) return ret;
    }
    ret = kfifo_from_user(&device->kfifo_buffer, buf, count, &actual_write);
    if (!kfifo_is_empty(&device->kfifo_buffer)) {
        wake_up_interruptible(&device->read_queue);
    }
    printk("%s, actual_write = %d, ppos = %lld\n", __func__, actual_write, *ppos);
    return actual_write;
}
static unsigned int
demodrv_poll(struct file *file, poll_table *wait) {
    int mask = 0;
    struct demo_private_data *data = file->private_data;
    struct demo_poll_device *device = data->device;
    poll_wait(file, &device->read_queue, wait);
    poll_wait(file, &device->write_queue, wait);

    if (!kfifo_is_empty(&device->kfifo_buffer)) mask |= POLLIN | POLLRDNORM;
    if (!kfifo_is_full(&device->kfifo_buffer)) mask |= POLLOUT | POLLWRNORM;
    return mask;
}

static const struct file_operations demodrv_fops = {
    .owner = THIS_MODULE,
    .open = demodrv_open,
    .release = demodrv_release,
    .read = demodrv_read,
    .write = demodrv_write,
    .poll = demodrv_poll
};

static struct miscdevice demo_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEMO_NAME,
    .fops = &demodrv_fops,
};

static int __init simple_char_init(void) {
    struct demo_poll_device *device;
    int ret;
    int i;
    ret = alloc_chrdev_region(&dev, 0, DEMO_MAX_DEVICES, DEMO_NAME);
    if (ret) {
        printk("failed to allocate char device region");
        return ret;
    }
    demo_cdev = cdev_alloc();
    if (!demo_cdev) {
        printk("cdev_alloc failed \n");
        goto unregister_chrdev;
    }
    cdev_init(demo_cdev, &demodrv_fops);
    ret = cdev_add(demo_cdev, dev, DEMO_MAX_DEVICES);
    if (ret) {
        printk("cdev_add failed \n");
        goto cdev_fail;
    }
    for (i = 0; i < DEMO_MAX_DEVICES; i++) {
        device = kmalloc(sizeof(struct demo_poll_device), GFP_KERNEL);
        if (!device) {
            ret = -ENOMEM;
            goto free_device;
        }
        sprintf(device->name, "%s%d", DEMO_NAME, i);
        demo_device[i] = device;
        init_waitqueue_head(&device->read_queue);
        init_waitqueue_head(&device->write_queue);
        ret = kfifo_alloc(&device->kfifo_buffer, MAX_DEVICE_BUFFER_SIZE, GFP_KERNEL);
        if (ret) {
            ret = -ENOMEM;
            goto free_kfifo;
        }
        printk("kfifo_buffer = %p \n", &device->kfifo_buffer);
    }
    printk("succeeded register char device: %s \n", DEV_NAME);
    return 0;
free_kfifo:
    for (i = 0; i < DEMO_MAX_DEVICES; i++) {
        if (&device->kfifo_buffer) kfifo_free(&device->kfifo_buffer);
    }
free_device:
    kfree(device);

cdev_fail:
    cdev_del(demo_cdev);

unregister_chrdev:
    unregister_chrdev_region(dev, DEMO_MAX_DEVICES);
    return ret;
}

static void __exit simple_char_exit(void) {
    int i;
	printk("removing device\n");

	if (demo_cdev)
		cdev_del(demo_cdev);

	unregister_chrdev_region(dev, DEMO_MAX_DEVICES);

	for (i =0; i < DEMO_MAX_DEVICES; i++)
		if (demo_device[i])
			kfree(demo_device[i]);
}

module_init(simple_char_init);
module_exit(simple_char_exit);

MODULE_AUTHOR("Peng Jinghui");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("demo_chardev");