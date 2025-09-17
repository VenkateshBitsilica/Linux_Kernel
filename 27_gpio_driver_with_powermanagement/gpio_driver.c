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


#define DEVICE_NAME             "gpio_btn"
#define CLASS_NAME              "gpio_btn_class"
#define MAX_SIZE                1024

static dev_t dev_num;
static struct cdev gpio_cdev;
static struct class *gpio_devclass;
static struct device *gpio_device;

static struct gpio_desc *led_gpiod;
static struct gpio_desc *btn_gpiod;
static int irq_num;
static int led_state = 0;
char msg[MAX_SIZE];

static irqreturn_t btn_irq_handler(int irq,void *dev_id)
{
	//gpiod_set_value(led_gpiod,1);
	led_state = 1;
	pr_info("Button interrupt received & LED turned ON\n");
	return IRQ_HANDLED;
}

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
        return simple_read_from_buffer(buf,len,off,msg,sizeof(msg));
}

static ssize_t gpio_write(struct file *file,const char __user *buf, size_t len, loff_t *off)
{
        if(len > MAX_SIZE)
                len = MAX_SIZE-1;

        if(copy_from_user(msg,buf,len))
                return -EFAULT;

        msg[len] = '\0';

        return len;
}

static int gpio_suspend(struct device *dev)
{
	pr_info("Suspending...\n");
	//led_state = 0;
    	disable_irq(irq_num);

    	gpiod_set_value(led_gpiod, led_state);

    	return 0;
}

static int gpio_resume(struct device *dev)
{
	pr_info("[gpiobtn] Resuming...\n");

    	if (led_state)
        	gpiod_set_value(led_gpiod, led_state);

    	enable_irq(irq_num);

    	return 0;
}

static struct file_operations fops = {
        .owner = THIS_MODULE,
        .open = gpio_open,
        .release = gpio_release,
        .read = gpio_read,
        .write = gpio_write,
};


static int gpio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

        alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME);
        cdev_init(&gpio_cdev,&fops);
        cdev_add(&gpio_cdev,dev_num,1);

        gpio_devclass = class_create(CLASS_NAME);
        gpio_device = device_create(gpio_devclass,NULL,dev_num,NULL,DEVICE_NAME);


	btn_gpiod = gpiod_get(dev,"Button",GPIOD_IN);
	led_gpiod = gpiod_get(dev,"LED",GPIOD_OUT_LOW);

	if(IS_ERR(btn_gpiod))
	{
		dev_err(dev,"Failed to read GPIO\n");
		return PTR_ERR(btn_gpiod);
	}
	if(IS_ERR(led_gpiod))
	{
		dev_err(dev,"Failed to read LED pin\n");\
		return PTR_ERR(led_gpiod);
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
	gpiod_set_value(led_gpiod,0);
	gpiod_put(led_gpiod);
	dev_info(&pdev->dev, "GPIO Button interrupt driver removed\n");
	return 0;
}

static const struct dev_pm_ops gpio_pm_ops = {
        .suspend = gpio_suspend,
        .resume = gpio_resume,
};

static struct of_device_id gpio_of_ids[]={
	{.compatible = "gpiovendor,gpio_btn"},
	{}
};

MODULE_DEVICE_TABLE(of,gpio_of_ids);

static struct platform_driver gpio_driver = {
	.driver ={
		.name = "gpio_btnirq_driver",
		.of_match_table = gpio_of_ids,
		.pm = &gpio_pm_ops,
	},
	.probe = gpio_probe,
	.remove = gpio_remove,
};

module_platform_driver(gpio_driver);


MODULE_LICENSE("GPL");
