/**
 * @file aesdchar.c
 * @brief Functions and data related to the AESD char driver implementation
 *
 * Based on the implementation of the "scull" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author Dan Walkes
 * @date 2019-10-22
 * @copyright Copyright (c) 2019
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h> // file_operations
#include <linux/slab.h>
#include <linux/uaccess.h>
#include "aesdchar.h"
int aesd_major =   0; // use dynamic major
int aesd_minor =   0;
int flag = 0;

MODULE_AUTHOR("Mohit Rane"); /** DONE - TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");

struct aesd_dev aesd_device;

//char* ptr;

/* OPEN METHOD */
int aesd_open(struct inode *inode, struct file *filp)
{
    struct aesd_dev *dev; /* device information */
    
    PDEBUG("In %s function\n", __FUNCTION__);

    PDEBUG("Device Major : %d \n", imajor(inode));
    PDEBUG("Device Minor : %d \n", iminor(inode));
    
    dev = container_of(inode->i_cdev, struct aesd_dev, cdev);
    filp->private_data = dev; /* for other methods */

    return 0;
}

/* RELEASE METHOD */
int aesd_release(struct inode *inode, struct file *filp)
{
    PDEBUG("In %s function\n", __FUNCTION__);
	
    return 0;
}

/* READ METHOD */
ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
	ssize_t retval = 0;
    struct aesd_dev *dev = filp->private_data;

    PDEBUG("In %s function\n", __FUNCTION__);
    PDEBUG("read %zu bytes with offset %lld\n",count,*f_pos);

    count = sizeof(dev->CB.data) + 1;

	PDEBUG("sizeof(dev->CB.data) = %ld\n", sizeof(dev->CB.data));

    if (copy_to_user(buf, dev->CB.data, count)) {
        retval = -EFAULT;
    }

	PDEBUG("dev->CB.data = %s\n", dev->CB.data);
	PDEBUG("buf = %s\n", buf);

	retval = count;

	if(flag == 1)
	{
		retval = 0;
	}

	flag = 1;

    return retval;
}

/* WRITE METHOD */
ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
	struct aesd_dev *dev = filp->private_data;

    ssize_t retval = -ENOMEM;
    
    retval = count;

    PDEBUG("In %s function\n", __FUNCTION__);
    PDEBUG("write %zu bytes with offset %lld\n",count,*f_pos);
    PDEBUG("Received string: %s\n", buf);
    PDEBUG("String count:    %ld\n", count);

    dev->CB.data = (char*)kmalloc(count * sizeof(char), GFP_KERNEL);
    
    if (copy_from_user(dev->CB.data, buf, count)) {
        retval = -EFAULT;
    }
    
    PDEBUG("Malloced and wrote: %s\n", dev->CB.data);

    return retval;
}

/* file operations structure for character driver */
struct file_operations aesd_fops = {
	.owner =    THIS_MODULE,
	.read =     aesd_read,
	.write =    aesd_write,
	.open =     aesd_open,
	.release =  aesd_release,
};

/* character device setup called during module initialization */
static int aesd_setup_cdev(struct aesd_dev *dev)
{
	int err, devno = MKDEV(aesd_major, aesd_minor);

	cdev_init(&dev->cdev, &aesd_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &aesd_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	if (err) {
		printk(KERN_ERR "Error %d adding aesd cdev", err);
	}
	return err;
}


/* This function is called when module is inserted in kernel */
int aesd_init_module(void)
{
	dev_t dev = 0;
	int result;
	result = alloc_chrdev_region(&dev, aesd_minor, 1,
			"aesdchar");
	aesd_major = MAJOR(dev);
	if (result < 0) {
		printk(KERN_WARNING "Can't get major %d\n", aesd_major);
		return result;
	}
	memset(&aesd_device,0,sizeof(struct aesd_dev));

	/**
	 * TODO: initialize the AESD specific portion of the device
	 */

	result = aesd_setup_cdev(&aesd_device);

	if( result ) {
		unregister_chrdev_region(dev, 1);
	}
	return result;

}

/* This function is called when module is inserted in kernel */
void aesd_cleanup_module(void)
{
	dev_t devno = MKDEV(aesd_major, aesd_minor);

	cdev_del(&aesd_device.cdev);

	/**
	 * TODO: cleanup AESD specific poritions here as necessary
	 */

	unregister_chrdev_region(devno, 1);
}

module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
