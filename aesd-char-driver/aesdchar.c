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
#include <linux/fs.h> 	// file_operations
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include "aesdchar.h"
#include "aesd_ioctl.h"
int aesd_major =   0; 	// use dynamic major
int aesd_minor =   0;
int n_flag = 1;			// new line flag

MODULE_AUTHOR("Mohit Rane");
MODULE_LICENSE("Dual BSD/GPL");

struct aesd_dev aesd_device;

/* OPEN METHOD */
int aesd_open(struct inode *inode, struct file *filp)
{
    struct aesd_dev *dev; /* device information */
    
	PDEBUG("OPEN\n");
    
    dev = container_of(inode->i_cdev, struct aesd_dev, cdev);
    filp->private_data = dev; /* for other methods */

    return 0;
}

/* RELEASE METHOD */
int aesd_release(struct inode *inode, struct file *filp)
{
	PDEBUG("CLOSE\n");
    return 0;
}

/* IOCTL METHOD */
int aesd_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	// TODO: remove if not required
	// int err = 0, tmp;
	int retval = 0;

	if (_IOC_TYPE(cmd) != AESD_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > AESDCHAR_IOC_MAXNR) return -ENOTTY;

	// if (_IOC_DIR(cmd) & _IOC_READ)
	// 	err = !access_ok_wrapper(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	// else if (_IOC_DIR(cmd) & _IOC_WRITE)
	// 	err = !access_ok_wrapper(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	// if (err) return -EFAULT;

	switch(cmd) {
	  case AESDCHAR_IOCSEEKTO:
		// TODO
		break;

	  default:
		return -ENOTTY;
	}

	return retval;
}

/* READ METHOD */
ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
	ssize_t retval = 0;
    struct aesd_dev *dev = filp->private_data;

	if(mutex_lock_interruptible(&dev->lock)) { return -ERESTARTSYS; }
	PDEBUG("MUTEX LOCKED\n");

#if 0
	int l_tail = dev->CB.tail;	// store CB tail in variable for iteration
	int i = 0;

    PDEBUG("READ\n");
	PDEBUG("dev->size = %ld and f_pos = %lld\n", dev->size, *f_pos);

	// return when there is no more content left to read
	if (*f_pos >= dev->size)
		goto out;

	// send device file contents
	for(i=0; i<CB_SIZE; i++)
	{
		PDEBUG("l_tail = %d\n", l_tail);
		
		// enters this condition after reading all filled strings (only when buffer is not full)
		if(dev->CB.data[l_tail] == NULL) { break; }

		// send data to user	
		if (copy_to_user(buf + *f_pos, dev->CB.data[l_tail], dev->CB.size[l_tail])) { return -EFAULT; }
		PDEBUG("CB[%d] = %s; size = %d\n", l_tail, dev->CB.data[l_tail], dev->CB.size[l_tail]);
		
		// update file offset and retval
		*f_pos += dev->CB.size[l_tail];
		retval += dev->CB.size[l_tail];
		PDEBUG("f_pos = %lld\n", *f_pos);

		// increment l_tail
		l_tail = (l_tail + 1) % CB_SIZE;
	}
#else
	// store seeked position in local variable
	loff_t l_pos = *f_pos;
	loff_t s_pos = l_pos;
	loff_t b_pos = 0;		// for buffer space position, actual content returned
	int i = 0;
	int j = 0;
	int k = 0;
	int size = 0;
	int prev_size = 0;
	char* temp_buff;

    PDEBUG("READ\n");
	PDEBUG("dev->size = %ld and f_pos = %lld\n", dev->size, *f_pos);

	// return when there is no more content left to read
	if (*f_pos >= dev->size)
	{
		*f_pos = 0;
		goto out;
	}

	// when this loop breaks, i corresponds to the buffer with current f_pos
	for(i=0; i<CB_SIZE; i++)
	{
		size = size + dev->CB.size[i];
		if(l_pos <= (size-1)) { break; }
		prev_size = size;
	}

	// allocate temporary buffer to store seeked buffer
	temp_buff = (char*)kmalloc(((size - l_pos) * sizeof(char)), GFP_KERNEL);
	for(j=0; j<(size - l_pos); j++)
	{
		temp_buff[j] = dev->CB.data[i][s_pos - prev_size];
		s_pos++;
	}
	PDEBUG("tempbuff = %s\n", temp_buff);
	PDEBUG("CB.data  = %s\n", dev->CB.data[i]);
	// send temporary buffer to user space
	if (copy_to_user(buf, temp_buff, (size - l_pos))) { return -EFAULT; }
	b_pos += (size - l_pos);
	// retval = b_pos;
	kfree(temp_buff);

	// send other device file contents
	for(k=(i+1); k<CB_SIZE; k++)
	{		
		// 
		if(dev->CB.data[k] == NULL) { break; }

		// send data to user	
		if (copy_to_user(buf + b_pos, dev->CB.data[k], dev->CB.size[k])) { return -EFAULT; }
		
		// update file offset and retval
		b_pos += dev->CB.size[k];
		// retval = b_pos;
	}
#endif

	*f_pos = *f_pos + b_pos;
	retval = b_pos;
	PDEBUG("retval = %ld\n", retval);

	out:
		mutex_unlock(&dev->lock);
		PDEBUG("MUTEX UNLOCKED\n");
		return retval;
}

/* WRITE METHOD */
ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
	struct aesd_dev *dev = filp->private_data;
    ssize_t retval = -ENOMEM;
	int i = 0;
    retval = count;

	if(mutex_lock_interruptible(&dev->lock)) { return -ERESTARTSYS; }
	PDEBUG("MUTEX LOCKED\n");

    PDEBUG("WRITE\n");
    PDEBUG("write %zu bytes with offset %lld\n",count,*f_pos);

	// if overwrite then free previous data
	if (dev->CB.data[dev->CB.head] != NULL && n_flag == 1)
	{
		dev->CB.tail = dev->CB.head + 1;
		kfree(dev->CB.data[dev->CB.head]);
	}
	PDEBUG("tail  	: %d\n", dev->CB.tail);

	// insert received data if new line flag is set else append
	// update size of character array (CB.data) in both case
	if(n_flag == 1)
	{
		dev->CB.data[dev->CB.head] = (char*)kmalloc(count * sizeof(char), GFP_KERNEL);
		if (copy_from_user(dev->CB.data[dev->CB.head], buf, count)) { retval = -EFAULT; }
		dev->CB.size[dev->CB.head] = count;
	}
	else 
	{
		krealloc(dev->CB.data[dev->CB.head], count + dev->CB.size[dev->CB.head], GFP_KERNEL);
		if (copy_from_user(dev->CB.data[dev->CB.head] + dev->CB.size[dev->CB.head], buf, count)) { retval = -EFAULT; }
		dev->CB.size[dev->CB.head] = dev->CB.size[dev->CB.head] + count;
	}
	PDEBUG("CB[%d]	: %s\n", dev->CB.head, dev->CB.data[dev->CB.head]);
	PDEBUG("size  	: %d\n", dev->CB.size[dev->CB.head]);

	// set new line flag if last character of received string is \n
	n_flag = (*(dev->CB.data[dev->CB.head] + dev->CB.size[dev->CB.head] - 1) == '\n') ? 1 : 0;
	PDEBUG("n_flag	: %d\n", n_flag);

	// increment head pointer if new line flag is set
	if(n_flag == 1) { dev->CB.head = (dev->CB.head + 1) % CB_SIZE; }
	PDEBUG("head  	: %d\n", dev->CB.head);

	// calculate total device size
	dev->size = 0;
	for(i=0; i<CB_SIZE; i++)
		dev->size = dev->size + dev->CB.size[i];
	PDEBUG("dev size: %ld\n", dev->size);

	mutex_unlock(&dev->lock);
	PDEBUG("MUTEX UNLOCKED\n");

    return retval;
}

/* LLSEEK METHOD */
// method taken from scull llseek
loff_t aesd_llseek(struct file *filp, loff_t off, int whence)
{
	struct aesd_dev *dev = filp->private_data;
	loff_t newpos;

	switch(whence)
	{
		case 0: /* SEEK_SET */
			newpos = off;
			break;

		case 1: /* SEEK_CUR */
			newpos = filp->f_pos + off;
			break;

		case 2: /* SEEK_END */
			newpos = dev->size + off;
			break;

		default: /* can't happen */
			return -EINVAL;
	}
	if (newpos < 0) return -EINVAL;

	filp->f_pos = newpos;
	
	return newpos;
}


/* file operations structure for character driver */
struct file_operations aesd_fops = {
	.owner =    THIS_MODULE,
	.llseek =   aesd_llseek,
	.read =     aesd_read,
	.write =    aesd_write,
	// .ioctl =	aesd_ioctl,
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

	mutex_init(&aesd_device.lock);

	memset(&aesd_device,0,sizeof(struct aesd_dev));

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
