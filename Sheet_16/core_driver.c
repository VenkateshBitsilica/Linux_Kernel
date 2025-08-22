#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

void log_msg(const char *str)
{
	pr_info("Log: %s\n",str);
}

char* core_version(void)
{
	return "CoreDriver-v12.9";
}
EXPORT_SYMBOL(log_msg);
EXPORT_SYMBOL(core_version);

int char_init(void)
{
	pr_info("Core driver loded into kernel\n");
	return 0;
}

void char_exit(void)
{
	pr_info("Core driver removed\n");
}

module_init(char_init);
module_exit(char_exit);
MODULE_LICENSE("GPL");

