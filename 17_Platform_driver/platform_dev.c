#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Simulated platform device");

static struct my_pdata{
	int id;
	char label[20];
}my_data = {
	.id = 5,
	.label = "SensorNode"
};

static struct platform_device my_pdev = {
    .name = "myplatformdev",
    .id = -1,
    .dev = {
    	.platform_data = &my_data
    },
};

static int __init pdev_init(void)
{
    printk(KERN_INFO "Registering platform device...\n");
    return platform_device_register(&my_pdev);
}

static void __exit pdev_exit(void)
{
    printk(KERN_INFO "Unregistering platform device...\n");
    platform_device_unregister(&my_pdev);
}

module_init(pdev_init);
module_exit(pdev_exit);
