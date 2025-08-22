#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>
#include <linux/types.h>

#define DEV_NAME		"Char_dev"
#define CLASS_NAME		"char_class"
#define MAX_SIZE		1024

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;
static struct device *my_dev;
char Buffer[MAX_SIZE];

static struct dentry *debug_dir;
static u32 status = 0;

static int dev_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO"File opened successfully\n");
	return 0;
}

static ssize_t dev_read(struct file *file,char *buf,size_t len,loff_t *offset)
{
	return simple_read_from_buffer(buf, len,offset,Buffer,strlen(Buffer));
}

static ssize_t dev_write(struct file *file, const char *buf,size_t len, loff_t *offset)
{
	if(len > MAX_SIZE)
		len = MAX_SIZE-1;

	if(copy_from_user(Buffer, buf, len))
		return -EFAULT;

	Buffer[len] = '\0';

	pr_info("Message Received from User Space: %s\n",Buffer);
	
	if(status)
		pr_info("Debug enabled using Debugfs\n");
	else
		pr_info("Debug disabled using Debugfs\n");
	return len;
}


static int dev_close(struct inode *inode, struct file *file)
{
	printk(KERN_INFO"File closed Successfully\n");
	return 0;
}


static struct file_operations fops =
{
	.owner   = THIS_MODULE,
	.open    = dev_open,
	.read    = dev_read,
	.write   = dev_write,
	.release = dev_close,
};

static char *char_devnode(const struct device *dev, umode_t *mode)
{
    if (mode)
        *mode = 0666; 
    return NULL;
}

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

	my_class->devnode = char_devnode;

	my_dev = device_create(my_class,NULL,dev_num,NULL,DEV_NAME);
	if (IS_ERR(my_dev)) 
	{
        	class_destroy(my_class);
        	cdev_del(&my_cdev);
        	unregister_chrdev_region(dev_num, 1);
        	return PTR_ERR(my_dev);
	}

	debug_dir = debugfs_create_dir("char_debug_0", NULL);
	if (!debug_dir) {
    		pr_err("Failed to create debugfs directory\n");
    		return -ENOMEM;
	}

	debugfs_create_u32("status", 0666, debug_dir, (u32 *)&status);


	pr_info("%s-->Device created:/dev/%s\n",DEV_NAME,DEV_NAME);
	return 0;
}

void char_exit(void)
{
	if(debug_dir)
	{
		debugfs_remove_recursive(debug_dir);
		debug_dir = NULL;
	}
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

