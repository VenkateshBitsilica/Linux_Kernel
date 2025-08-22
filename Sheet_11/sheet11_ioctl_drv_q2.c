#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/ioctl.h>

#define MAJIC_ID	'V'
#define RESET_BUFFER	_IO(MAJIC_ID, 0)
#define GET_COUNT	_IOR(MAJIC_ID,1,int)
#define SET_DEBUG	_IOW(MAJIC_ID,2,int)

#define MAX_SIZE		1024
#define CHAR_DEVICE_NAME	"ioctl_dev"

static DEFINE_MUTEX(mutex);
static int major;
char Buffer[MAX_SIZE];
static int debug_mode = 0;
static int count = 0;

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
	mutex_lock(&mutex);

	if(copy_from_user(Buffer, buf, len))
		return -EFAULT;

	Buffer[len] = '\0';
	count++;
	mutex_unlock(&mutex);
	pr_info("Message Received from User Space: %s\n",Buffer);
	return len;
}

static ssize_t dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case RESET_BUFFER:
			mutex_lock(&mutex);
			memset(Buffer,0,MAX_SIZE);
			count = 0;
			mutex_unlock(&mutex);
			pr_info("%s-->Buffer reset done\n",CHAR_DEVICE_NAME);
			break;
		case GET_COUNT:
			if(copy_to_user((int*)arg, &count,sizeof(count)))
				return -EFAULT;
			pr_info("Write count sent :%d\n",count);
			break;
		case SET_DEBUG:
			if(copy_from_user(&debug_mode,(int*)arg,sizeof(int)))
				return -EFAULT;
			if(debug_mode == 1)
				pr_info("Debug mode enabled for testing application\n");
			else if(debug_mode == 0)
				pr_info("Debug mode disaled\n");
			else{
				pr_info("Invalid option for Debug mode : %d\n",debug_mode);
				return -EINVAL;
			}
			break;
		default:
			return -EINVAL;
	}
	return 0;
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
	.unlocked_ioctl = dev_ioctl,
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


