#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>


struct my_pdata{
	int id;
	char label[20];
};

static int my_probe(struct platform_device *pdev)
{
	struct my_pdata *pdata;

	pdata = dev_get_platdata(&pdev->dev);
	pr_info("Data: id = %d, label = %s",pdata->id, pdata->label);
    printk(KERN_INFO "Probe called for device: %s\n", pdev->name);
    return 0;
}

static int my_remove(struct platform_device *pdev)
{
    printk(KERN_INFO "[driver] Remove called for device: %s\n", pdev->name);
    return 0;
}

static struct platform_driver my_platform_driver = {
    .driver = {
        .name = "myplatformdev",
        .owner = THIS_MODULE,
    },
    .probe = my_probe,
    .remove = my_remove,
};

static int pdrv_init(void)
{
    printk(KERN_INFO "Platform driver loaded\n");
    return platform_driver_register(&my_platform_driver);
}

static void pdrv_exit(void)
{
    platform_driver_unregister(&my_platform_driver);
    printk(KERN_INFO "Platform driver unloaded\n");
}

module_init(pdrv_init);
module_exit(pdrv_exit);
MODULE_LICENSE("GPL");

