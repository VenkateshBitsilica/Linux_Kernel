#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>

struct my_pdata{
	int id;
	char label[20];
};

static const struct platform_device_id my_ids[] = {
    { "dev0", 0 },
    { "dev1", 0 },
    { }
};


static int my_probe(struct platform_device *pdev)
{
	struct my_pdata *pdata;

	pdata = dev_get_platdata(&pdev->dev);
	pr_info("Data: id = %d, label = %s",pdata->id, pdata->label);
    pr_info("Probe called for device: %s\n", pdev->name);
    return 0;
}

static int my_remove(struct platform_device *pdev)
{
    pr_info(" Remove called for device: %s\n", pdev->name);
    return 0;
}

static struct platform_driver my_platform_driver = {
    .probe  = my_probe,
    .remove = my_remove,
    .driver = {
        .name = "dev_match",
        .owner = THIS_MODULE,
    },
    .id_table = my_ids,
};

MODULE_DEVICE_TABLE(platform, my_ids);

static int pdrv_init(void)
{
    pr_info("Platform driver loaded\n");
    return platform_driver_register(&my_platform_driver);
}

static void pdrv_exit(void)
{
    platform_driver_unregister(&my_platform_driver);
    pr_info("Platform driver unloaded\n");
}

module_init(pdrv_init);
module_exit(pdrv_exit);
MODULE_LICENSE("GPL");

