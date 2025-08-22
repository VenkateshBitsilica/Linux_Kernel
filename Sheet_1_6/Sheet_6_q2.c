#include <linux/kernel.h>
#include <linux/module.h>

char *author = "Linuxkernel";
int count = 0;
int arr[100];

module_param(author,charp,S_IRUGO);
module_param(count,int,S_IRUGO);
module_param_array(arr, int, &count, S_IRUGO);

int cmd_init(void)
{
	pr_info("Module Inserted\nArray Elements:");
	for(int i=0; i<count; i++){
		//pr_info("%d\t", arr[i]);
		pr_info("Author Name: %s\n",author);
	}
	return 0;
}

void cmd_exit(void)
{
	pr_info("Exit Module\n");
}

MODULE_LICENSE("GPL");

module_init(cmd_init);
module_exit(cmd_exit);

