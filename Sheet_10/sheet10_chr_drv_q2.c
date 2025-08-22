#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define MSG_SIZE		100
#define MAX_MSG			10

static char *char_device_name = "chardev1";
static int major;
static int msg_count = 0;
char **Buffer;
static int read_idx = 0, write_idx = 0;

static DEFINE_MUTEX(mutex);


static int dev_open(struct inode *inode, struct file *file)
{
	pr_info("File opened successfully\n");
	return 0;
}

static ssize_t dev_read(struct file *file,char *buf,size_t len,loff_t *offset)
{
	ssize_t ret_val;

	mutex_lock(&mutex);

	if(msg_count==0)
	{
		pr_info("Buffer is Empty\n");
		mutex_unlock(&mutex);
		return 0;
	}

	ret_val = simple_read_from_buffer(buf, len,offset,Buffer[read_idx],strlen(Buffer[read_idx]));

	kfree(Buffer[read_idx]);

	read_idx = (read_idx+1)%MAX_MSG;
	msg_count--;

	mutex_unlock(&mutex);

	return ret_val;
}

static ssize_t dev_write(struct file *file, const char *buf,size_t len, loff_t *offset)
{

        if(len > MSG_SIZE)
                len = MSG_SIZE-1;

	//Enabling mutex lock to protect shared resource
        mutex_lock(&mutex);

        if(msg_count > MAX_MSG)
        {
                pr_info("%s-->Buffer is full\n",char_device_name);
                mutex_unlock(&mutex);
                return -ENOMEM;
        }

	Buffer[write_idx] = kmalloc(MSG_SIZE,GFP_KERNEL);
	if(!Buffer[write_idx])
	{
		pr_err("Memory allocation failed\n");
		return -ENOMEM;
	}

	if(copy_from_user(Buffer[write_idx], buf, len))
	{
		mutex_unlock(&mutex);
		return -EFAULT;
	}

	Buffer[write_idx][len] = '\0';

	pr_info("Message Received from User Space: %s\n",Buffer[write_idx]);
	
	write_idx = (write_idx+1)%MAX_MSG;
	msg_count++;
	mutex_unlock(&mutex);

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
	major = register_chrdev(0,char_device_name,&fops);
	if(major < 0)
	{
		pr_info("Failed to register Char device\n");
	}
	pr_info("Char driver registered with major number: %d\n",major);

	//Allocating buffer size dynamically
	Buffer = kmalloc_array(MAX_MSG, sizeof(char *), GFP_KERNEL);
	if (!Buffer) 
	{
 		 pr_err("Failed to allocate memory for buffer pointers\n");
   		 return -ENOMEM;
	}
	return 0;
}

void char_exit(void)
{
	unregister_chrdev(major,char_device_name);
	pr_info("Char driver unregistered\n");
}


module_init(char_init);
module_exit(char_exit);
MODULE_LICENSE("GPL");


