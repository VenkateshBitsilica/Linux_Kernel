#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timekeeping.h>

int sample_init(void)
{
	uint64_t time = ktime_get_real_seconds();
	pr_info("%ld --> Author : Venaktesh\n",time);
	return 0;
}

void sample_exit(void)
{
	pr_info("Goodbye Unloading from kernel");
}

module_init(sample_init);
module_exit(sample_exit);
MODULE_LICENSE("GPL");

