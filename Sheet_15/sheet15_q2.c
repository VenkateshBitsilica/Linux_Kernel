#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/poll.h>

#define DEV_NAME		"waitq_dev"
#define CLASS_NAME		"waitq_class"
#define MAX_SIZE		1024

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;
static struct device *my_dev;
char Buffer[MAX_SIZE];

static int read_count = 0;
static int can_write = 1;
static int data_available = 0;
static DECLARE_WAIT_QUEUE_HEAD(wq);
static DEFINE_MUTEX(mutex);


static int dev_open(struct inode *inode, struct file *file)
{
	pr_info("%s-->File Opened successfully\n",DEV_NAME);
	return 0;
}

static ssize_t dev_read(struct file *file,char *buf,size_t len,loff_t *offset)
{
	int ret;

	if(file->f_flags & O_NONBLOCK)
	{
		if(!data_available)
			return -EAGAIN;
	}
	else
	{
		pr_info("Waiting for write to fill Buffer\n%d times Readers havebeen blocked\n",read_count);
	}

	mutex_lock(&mutex);
	ret = simple_read_from_buffer(buf, len,offset,Buffer,strlen(Buffer));
	data_available =0;
	can_write = 1;
	mutex_unlock(&mutex);

	return ret;
}

static ssize_t dev_write(struct file *file, const char *buf,size_t len, loff_t *offset)
{

	if(len > MAX_SIZE)
		len = MAX_SIZE-1;

	mutex_lock(&mutex);
	if(copy_from_user(Buffer, buf, len))
		return -EFAULT;

	Buffer[len] = '\0';

	data_available = 1;
	can_write = 0;
	mutex_unlock(&mutex);

	wake_up_interruptible(&wq);
	pr_info("Message Received from User Space: %s\n",Buffer);

	return len;
}

static __poll_t dev_poll(struct file *file, poll_table *wait)
{
	__poll_t mask = 0;

	poll_wait(file, &wq, wait);

//	if(data_available)
//	{
//		mask |= POLLIN|POLLRDNORM;
//	}
	if(can_write)
	{
		mask |= POLLOUT;
	}

	return mask;
}


static int dev_close(struct inode *inode, struct file *file)
{
	pr_info("%s-->File closed Successfully\n",DEV_NAME);
	return 0;
}


static struct file_operations fops =
{
	.owner   = THIS_MODULE,
	.open    = dev_open,
	.read    = dev_read,
	.write   = dev_write,
	.release = dev_close,
	.poll 	 = dev_poll,
};

int char_init(void)
{
	int ret_val;
	pr_info("Registering Character Device dyamically\n");

	ret_val = alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	if(ret_val < 0)
	{
		pr_err("alloc_chrdev_region failed\n");
		return ret_val;
	}

	cdev_init(&my_cdev,&fops);
	
	ret_val = cdev_add(&my_cdev,dev_num,1);
	if(ret_val < 0)
	{
		pr_err("Registering character device failed (cdev_add)\n");
		unregister_chrdev_region(dev_num,1);
		return ret_val;
	}

	my_class = class_create(CLASS_NAME);
	if(IS_ERR(my_class))
	{
		cdev_del(&my_cdev);
        	unregister_chrdev_region(dev_num, 1);
        	return PTR_ERR(my_class);
	}

	my_dev = device_create(my_class,NULL,dev_num,NULL,DEV_NAME);
	if (IS_ERR(my_dev)) 
	{
        	class_destroy(my_class);
        	cdev_del(&my_cdev);
        	unregister_chrdev_region(dev_num, 1);
        	return PTR_ERR(my_dev);
	}

	pr_info("%s-->Device created:/dev/%s\n",DEV_NAME,DEV_NAME);
	return 0;
}

void char_exit(void)
{
	device_destroy(my_class, dev_num);
   	class_destroy(my_class);
    	cdev_del(&my_cdev);
    	unregister_chrdev_region(dev_num, 1);
    	pr_info("%s-->Device removed\n", DEV_NAME);
	pr_info("Char driver unregistered\n");
}

module_init(char_init);
module_exit(char_exit);
MODULE_LICENSE("GPL");

