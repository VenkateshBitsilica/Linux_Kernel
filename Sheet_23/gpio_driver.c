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
#include<linux/proc_fs.h>
#include<linux/seq_file.h>

static struct proc_dir_entry *proc_entry;

static struct gpio_desc *btn_gpiod;
static int irq_num;
static int count;

static irqreturn_t btn_irq_handler(int irq,void *dev_id)
{
	count++;
	pr_info("Button interrupt received\n");
	return IRQ_HANDLED;
}

static int gpiobtn_proc_show(struct seq_file *m, void *v)
{
    seq_printf(m, "Driver: GPIO Button + LED\n");
    seq_printf(m, "IRQ Number: %d\n", irq_num);
    seq_printf(m, "Press Count: %d\n", count);
    return 0;
}

static int gpiobtn_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, gpiobtn_proc_show, NULL);
}

static const struct proc_ops gpiobtn_proc_ops = {
    .proc_open    = gpiobtn_proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static int gpio_probe(struct platform_device *pdev)
{


	struct device *dev = &pdev->dev;

	proc_entry = proc_create("gpiobtn_info", 444, NULL, &gpiobtn_proc_ops);
        if (!proc_entry)
		dev_warn(dev, "Failed to create /proc/gpiobtn_info\n");


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
	proc_remove(proc_entry);
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
/*
static const struct proc_ops gpiobtn_proc_ops = {
    .proc_open    = gpiobtn_proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};
*/
module_platform_driver(gpio_driver);


MODULE_LICENSE("GPL");
