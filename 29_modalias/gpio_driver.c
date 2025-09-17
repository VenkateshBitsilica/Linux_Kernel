#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>

struct my_pdata 
{
	int id;
	char label[200];
};
static int gpio_probe(struct platform_device *pdev)
{
        struct my_pdata *pdata;

        pdata = dev_get_platdata(&pdev->dev);
        pr_info("Data: id = %d, label = %s",pdata->id, pdata->label);
    pr_info("Probe called for device: %s\n", pdev->name);
    return 0;
}

static int gpio_remove(struct platform_device *pdev)
{
    pr_info("Remove called for device: %s\n", pdev->name);
    return 0;
}


static struct platform_driver gpio_driver = {
    .driver = {
        .name = "gpio_driver",  
    },
    .probe = gpio_probe,
    .remove = gpio_remove,
};

MODULE_ALIAS("platform:gpio_driver");

module_platform_driver(gpio_driver);

MODULE_LICENSE("GPL");


