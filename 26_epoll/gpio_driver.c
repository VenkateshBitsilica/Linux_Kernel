#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/of.h>
#include<linux/of_gpio.h>
#include<linux/gpio/consumer.h>
#include<linux/platform_device.h>
#include<linux/irq.h>
#include<linux/irqreturn.h>
#include<linux/interrupt.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/ioctl.h>
#include <linux/poll.h>
#include <linux/jiffies.h>

#define DEVICE_NAME		"gpio_btn"
#define CLASS_NAME		"gpio_btn_class"
#define GPIOBTN_RESET		_IO('V',0)
#define MAX_SIZE		1024


static unsigned long last_press_jiffies;
static unsigned int debounce_ms = 50;

static void btn_work_handler(struct work_struct *work);


static DECLARE_WORK(btn_work, btn_work_handler);
static DECLARE_WAIT_QUEUE_HEAD(wq);
static int data_ready = 0;

static dev_t dev_num;
static struct cdev gpio_cdev;
static struct class *gpio_devclass;
static struct device *gpio_device;

char msg[MAX_SIZE];

static struct gpio_desc *btn_gpiod;
static int irq_num;

int count = 0;

static int gpio_open(struct inode *inode, struct file *file)
{
	pr_info("Device opened\n");
    	return 0;
}

static int gpio_release(struct inode *inode,struct file *file)
{
	pr_info("Device Removed\n");
    	return 0;
}

static ssize_t gpio_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
	wait_event_interruptible(wq, data_ready);  // Block until button is pressed

	char message[64];
    	int msg_len = snprintf(message, sizeof(message), "Press Count: %d\n", count);
    	data_ready = 0;

    	if (*off >= msg_len) return 0;
    	if (copy_to_user(buf, message, msg_len)) return -EFAULT;

    	*off += msg_len;
    	return msg_len;
	//return simple_read_from_buffer(buf,len,off,msg,sizeof(msg));
}

static ssize_t gpio_write(struct file *file,const char __user *buf, size_t len, loff_t *off)
{
	if(len > MAX_SIZE)
		len = MAX_SIZE-1;

	if(copy_from_user(msg,buf,len))
		return -EFAULT;

	msg[len] = '\0';

	if(msg[0] == '1')
	{
		pr_info("Received 1 to make LED pin high\n");
	}

	return len;
}

static long gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case GPIOBTN_RESET:
			count = 0;
			pr_info("Press count reset using ioctl commands\n");
			break;
		default:
			return -EINVAL;
	}

	return 0;
}

static __poll_t gpio_poll(struct file *file, poll_table *wait)
{
    poll_wait(file, &wq, wait); 

    if (data_ready)
        return POLLIN | POLLRDNORM;

    return 0;
}

static void btn_work_handler(struct work_struct *work)
{
	unsigned long now = jiffies;

    	if (time_before(now, last_press_jiffies + msecs_to_jiffies(debounce_ms))) {
        	pr_info("[gpiobtn] Ignored bounce\n");
        	return;
    	}
    	last_press_jiffies = now;
    	count++;
    	data_ready = 1;
    	wake_up_interruptible(&wq);  // Wake waiting readers
    	pr_info("Button pressed, count = %d\n", count);
}

static irqreturn_t btn_irq_handler(int irq,void *dev_id)
{
//	count++;
//	pr_info("%d Button interrupts received\n",count);
	schedule_work(&btn_work);
	return IRQ_HANDLED;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = gpio_open,
	.release = gpio_release,
	.read = gpio_read,
	.write = gpio_write,
	.unlocked_ioctl = gpio_ioctl,
	.poll = gpio_poll,
};

static int gpio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME);
	cdev_init(&gpio_cdev,&fops);
	cdev_add(&gpio_cdev,dev_num,1);

	gpio_devclass = class_create(CLASS_NAME);
	gpio_device = device_create(gpio_devclass,NULL,dev_num,NULL,DEVICE_NAME);


	btn_gpiod = gpiod_get(dev,NULL,GPIOD_IN);

	if(IS_ERR(btn_gpiod))
	{
		dev_err(dev,"Failed to read GPIO\n");
		return PTR_ERR(btn_gpiod);
	}
	
	irq_num = gpiod_to_irq(btn_gpiod);
	if(irq_num < 0)
	{
		dev_err(dev,"Failed to get IRQ\n");
		return irq_num;
	}
	if(request_irq(irq_num, btn_irq_handler, IRQF_TRIGGER_FALLING,"gpio_btn_irq", NULL))
	{
		dev_err(dev, "Failed to request IRQ\n");
        	return -EBUSY;
	}
	dev_info(dev,"GPIO Button driver Initialized with IRQ %d\n",irq_num);

	return 0;
}

static int gpio_remove(struct platform_device *pdev)
{
	device_destroy(gpio_devclass,dev_num);
	class_destroy(gpio_devclass);
	cdev_del(&gpio_cdev);
	unregister_chrdev_region(dev_num,1);
	free_irq(irq_num,NULL);
	gpiod_put(btn_gpiod);
	dev_info(&pdev->dev, "GPIO Button interrupt driver removed\n");
	return 0;
}

static struct of_device_id gpio_of_ids[]={
	{.compatible = "gpiovendor,gpio_btn"},
	{}
};

MODULE_DEVICE_TABLE(of,gpio_of_ids);

static struct platform_driver gpio_driver = {
	.driver ={
		.name = "gpio_btnirq_driver",
		.of_match_table = gpio_of_ids,
	},
	.probe = gpio_probe,
	.remove = gpio_remove,
};

module_platform_driver(gpio_driver);


MODULE_LICENSE("GPL");
