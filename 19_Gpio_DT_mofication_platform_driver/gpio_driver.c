#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/of.h>
#include<linux/of_gpio.h>
#include<linux/gpio/consumer.h>
#include<linux/platform_device.h>

static struct gpio_desc *led_gpiod;

static int gpio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	led_gpiod = gpiod_get(dev,NULL,GPIOD_OUT_LOW);

	if(IS_ERR(led_gpiod))
	{
		dev_err(dev,"Failed to read GPIO\n");
		return PTR_ERR(led_gpiod);
	}

	dev_info(dev,"GPIO LED driver bound successfully\n");

	gpiod_set_value(led_gpiod,1);
	dev_info(dev,"LED turned ON\n");

	return 0;
}

static int gpio_remove(struct platform_device *pdev)
{
	gpiod_set_value(led_gpiod,0);
	gpiod_put(led_gpiod);
	dev_info(&pdev->dev, "GPIO LED driver removed\n");
	return 0;
}

static struct of_device_id gpio_of_ids[]={
	{.compatible = "gpiovendor,gpio_led"},
	{}
};

MODULE_DEVICE_TABLE(of,gpio_of_ids);

static struct platform_driver gpio_driver = {
	.driver ={
		.name = "gpio_driver",
		.of_match_table = gpio_of_ids,
	},
	.probe = gpio_probe,
	.remove = gpio_remove,
};

module_platform_driver(gpio_driver);


MODULE_LICENSE("GPL");
