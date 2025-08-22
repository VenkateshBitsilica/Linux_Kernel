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

static struct gpio_desc *btn_gpiod;
static int irq_num;
static int count;

volatile int etx_value = 0;
struct kobject *kobj_ref;

static ssize_t  sysfs_show(struct kobject *kobj,
                        struct kobj_attribute *attr, char *buf);
static ssize_t  sysfs_store(struct kobject *kobj,
                        struct kobj_attribute *attr,const char *buf, size_t count);
struct kobj_attribute etx_attr = __ATTR(etx_value, 0660, sysfs_show, sysfs_store);


static irqreturn_t btn_irq_handler(int irq,void *dev_id)
{
	count++;
	pr_info("Button interrupt received\n");
	return IRQ_HANDLED;
}


static ssize_t  sysfs_show(struct kobject *kobj,struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf,"etx_Value:%d\nPress_Count:%d",etx_value,count);
}

static ssize_t  sysfs_store(struct kobject *kobj,struct kobj_attribute *attr,const char *buf, size_t count)
{
	 sscanf(buf,"%d",&etx_value);
	 return count;
}

static int gpio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

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


	kobj_ref = kobject_create_and_add("etx_sysfs",kernel_kobj);

        /*Creating sysfs file for etx_value*/
        if(sysfs_create_file(kobj_ref,&etx_attr.attr)){
                pr_err("Cannot create sysfs file......\n");
		free_irq(irq_num, NULL);
       	 	gpiod_put(btn_gpiod);
                return -EBUSY;
    }

	return 0;
}

static int gpio_remove(struct platform_device *pdev)
{
	free_irq(irq_num,NULL);
	gpiod_put(btn_gpiod);
	sysfs_remove_file(kernel_kobj, &etx_attr.attr);
	kobject_put(kobj_ref);
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
