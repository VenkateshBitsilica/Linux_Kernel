#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

struct my_pdata {
    int id;
    char label[20];
};

static struct my_pdata my_data = {
    .id = 5,
    .label = "SensorNode"
};

static void mydev_release(struct device *dev)
{
    pr_info("platform_dev: device released\n");
}

static struct platform_device my_pdev = {
    .name = "gpio_driver",
    .id = -1,
    .dev = {
        .platform_data = &my_data,
        .release = mydev_release,  
    },
};

static int __init pdev_init(void)
{
    pr_info("Registering platform device...\n");
    return platform_device_register(&my_pdev);
}

static void __exit pdev_exit(void)
{
    pr_info("Unregistering platform device...\n");
    platform_device_unregister(&my_pdev);
}

module_init(pdev_init);
module_exit(pdev_exit);

MODULE_LICENSE("GPL");

