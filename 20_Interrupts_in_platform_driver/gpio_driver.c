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

static irqreturn_t btn_irq_handler(int irq,void *dev_id)
{
	pr_info("Button interrupt received\n");
	return IRQ_HANDLED;
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

	return 0;
}

static int gpio_remove(struct platform_device *pdev)
{
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
