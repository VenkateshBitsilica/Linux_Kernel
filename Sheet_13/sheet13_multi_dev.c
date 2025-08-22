#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEV_NAME		"multi_char_dev"
#define CLASS_NAME		"multi_class"
#define MAX_SIZE		1024
#define NUM_DEVICES		3

static dev_t dev_num;
static struct class *my_class;

struct mydev_t 
{
	int index;
	char Buffer[MAX_SIZE];
	struct cdev cdev;
};

struct mydev_t my_devices[NUM_DEVICES];


static int dev_open(struct inode *inode, struct file *file)
{
	struct mydev_t *dev = container_of(inode->i_cdev,struct mydev_t, cdev);
	file->private_data = dev;
	pr_info("%s%d opened successfully\n",DEV_NAME,dev->index);
	return 0;
}

static ssize_t dev_read(struct file *file,char *buf,size_t len,loff_t *offset)
{
	struct mydev_t *dev = file->private_data;
	return simple_read_from_buffer(buf, len,offset,dev->Buffer,strlen(dev->Buffer));
}

static ssize_t dev_write(struct file *file, const char *buf,size_t len, loff_t *offset)
{
	struct mydev_t *dev = file->private_data;
	if(len > MAX_SIZE)
		len = MAX_SIZE-1;

	if(copy_from_user(dev->Buffer, buf, len))
		return -EFAULT;

	dev->Buffer[len] = '\0';

	pr_info("Message Received from User Space: %s\n",dev->Buffer);
	return len;
}


static int dev_close(struct inode *inode, struct file *file)
{
	struct mydev_t *dev = file->private_data;
	pr_info("%s%d closed successfully\n",DEV_NAME,dev->index);
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
		
int char_init(void)
{
	int ret_val;
	int i;
	dev_t devno;

	pr_info("Registering Character Devices dynamically\n");

	ret_val = alloc_chrdev_region(&dev_num, 0, NUM_DEVICES, DEV_NAME);
	if (ret_val < 0) {
		pr_err("alloc_chrdev_region failed\n");
		return ret_val;
	}

	my_class = class_create(CLASS_NAME);
	if (IS_ERR(my_class)) {
		unregister_chrdev_region(dev_num, NUM_DEVICES);
		pr_err("class_create failed\n");
		return PTR_ERR(my_class);
	}

	for (i = 0; i < NUM_DEVICES; i++) {
		devno = MKDEV(MAJOR(dev_num), MINOR(dev_num) + i);

		my_devices[i].index = i;
		memset(my_devices[i].Buffer, 0, MAX_SIZE);

		cdev_init(&my_devices[i].cdev, &fops);
		my_devices[i].cdev.owner = THIS_MODULE;

		ret_val = cdev_add(&my_devices[i].cdev, devno, 1);
		if (ret_val < 0) {
			pr_err("cdev_add failed for device %d\n", i);
			
			for (--i; i >= 0; i--) {
				cdev_del(&my_devices[i].cdev);
			}
			class_destroy(my_class);
			unregister_chrdev_region(dev_num, NUM_DEVICES);
			return ret_val;
		}

		if (device_create(my_class, NULL, devno, NULL, "%s%d", DEV_NAME, i) == NULL) {
			pr_err("device_create failed for device %d\n", i);
			cdev_del(&my_devices[i].cdev);
			for (--i; i >= 0; i--) {
				device_destroy(my_class, MKDEV(MAJOR(dev_num), MINOR(dev_num) + i));
				cdev_del(&my_devices[i].cdev);
			}
			class_destroy(my_class);
			unregister_chrdev_region(dev_num, NUM_DEVICES);
			return -1;
		}
	}

	pr_info("%s: Devices created successfully\n", DEV_NAME);
	return 0;
}


void char_exit(void)
{
	int i;
	dev_t devno;

	for (i = 0; i < NUM_DEVICES; i++) {
		devno = MKDEV(MAJOR(dev_num), MINOR(dev_num) + i);
		device_destroy(my_class, devno);
		cdev_del(&my_devices[i].cdev);
	}

	class_destroy(my_class);
	unregister_chrdev_region(dev_num, NUM_DEVICES);
	pr_info("%s: Devices removed and driver unloaded\n", DEV_NAME);
}

module_init(char_init);
module_exit(char_exit);
MODULE_LICENSE("GPL");

