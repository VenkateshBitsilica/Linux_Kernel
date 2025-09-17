#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>

#define MAX_SIZE		1024
#define CHAR_DEVICE_NAME	"simple_char_driver"
static int major;
char Buffer[MAX_SIZE];
static int count = 0;

static int dev_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO"File opened successfully\n");
	return 0;
}

static ssize_t dev_read(struct file *file,char *buf,size_t len,loff_t *offset)
{
	pr_info("%d bytes had wrote last time",count);
	return simple_read_from_buffer(buf, len,offset,Buffer,strlen(Buffer));
}

static ssize_t dev_write(struct file *file, const char *buf,size_t len, loff_t *offset)
{
	if(len > MAX_SIZE)
		len = MAX_SIZE-1;

	if(copy_from_user(Buffer, buf, len))
		return -EFAULT;

	Buffer[len] = '\0';
	count = len;

	pr_info("Message Received from User Space: %s\n",Buffer);
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

int char_init(void)
{
	pr_info("Registering Character Device\n");
	major = register_chrdev(0,CHAR_DEVICE_NAME,&fops);
	if(major < 0)
	{
		pr_info("Failed to register Char device\n");
	}
	pr_info("Char driver registered with major number: %d\n",major);
	return 0;
}

void char_exit(void)
{
	unregister_chrdev(major,CHAR_DEVICE_NAME);
	pr_info("Char driver unregistered\n");
}


module_init(char_init);
module_exit(char_exit);
MODULE_LICENSE("GPL");

