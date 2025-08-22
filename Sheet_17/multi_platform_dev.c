#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>


static void dev_release(struct device *dev)
{
    pr_info("Releasing platform device\n");
}

static struct my_pdata{
	int id;
	char label[20];
};

struct my_pdata data0 = {0,"SensorNode-0"};
struct my_pdata data1 = {1,"SensorNode-1"};

static struct platform_device pdev0 = {
    .name = "dev0",
    .id = -1,
    .dev = {
    	.platform_data = &data0,
	.release = dev_release,
    },
};

static struct platform_device pdev1 = {
	.name = "dev1",
	.id = -1,
	.dev = {
		.platform_data = &data1,
		.release = dev_release,
	},
};

static int __init pdev_init(void)
{
    pr_info( "Registering platform devices dev0 & dev1\n");
    platform_device_register(&pdev0);
    platform_device_register(&pdev1);
    return 0;
}

static void __exit pdev_exit(void)
{
    pr_info("Unregistering platform device...\n");
    platform_device_unregister(&pdev0);
    platform_device_unregister(&pdev1);
}

module_init(pdev_init);
module_exit(pdev_exit);

MODULE_LICENSE("GPL");

