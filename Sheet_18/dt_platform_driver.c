#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>

struct dt_data
{
	int id;
	const char *label;
};

static int dt_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *dt_node = dev->of_node;

	struct dt_data data;

	if(of_property_read_u32(dt_node, "id", &data.id))
	{
		data.id = -1;
	}

	if(of_property_read_string(dt_node, "label",&data.label))
	{
		data.label = "unknown";
	}

	dev_info(dev,"Probed via DT: id=%d, label=%s\n",data.id,data.label);
	return 0;
}

static int dt_remove(struct platform_device *pdev)
{
	dev_info(&pdev->dev,"Device removed\r\n");
	return 0;
}

static struct of_device_id dt_ids[] = {
	{.compatible = "dt_vendor,dt_device"},
	{}
};

MODULE_DEVICE_TABLE(of,dt_ids);

static struct platform_driver dt_driver={
	.probe = dt_probe,
	.remove = dt_remove,
	.driver = {
		.name = "dt_device",
		.of_match_table = dt_ids,
	},
};

module_platform_driver(dt_driver);

MODULE_LICENSE("GPL");
