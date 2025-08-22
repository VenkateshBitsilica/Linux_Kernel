#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define PROC_WRITE_FILE	 "control"
#define PROC_READ_FILE	"status"
#define MAX_LEN 100
int log_enable;

static char message[MAX_LEN];
ssize_t control_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    return simple_read_from_buffer(buf, count, ppos, message, strlen(message));
}

ssize_t control_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    if (count > MAX_LEN - 1)
        count = MAX_LEN - 1;

    if (copy_from_user(message, buf, count))
        return -EFAULT;

    message[count] = '\0';

    pr_info("Message Received from user: %s\n", message);
    
    return count;
}

ssize_t status_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	const char *msg1 = "Status : Running\n";
	const char *msg2 = "Status : Stopped\n";
	const char *msg3 = "Invalid Command\n";
	if(strncasecmp(message,"start",5)==0)
    		return simple_read_from_buffer(buf, count, ppos, msg1, strlen(msg1));
	else if(strncasecmp(message, "stop",4)==0)
		return simple_read_from_buffer(buf, count, ppos, msg2, strlen(msg2));
	else
		return simple_read_from_buffer(buf, count, ppos, msg3, strlen(msg3));
}

static const struct proc_ops writer_fops = {
    .proc_read = control_read,
    .proc_write = control_write,
};
static const struct proc_ops read_fops = {
    .proc_read = status_read,
   // .proc_write = status_write,
};

static int __init proc_init(void)
{
    proc_create(PROC_WRITE_FILE, 0666, NULL, &writer_fops);
    proc_create(PROC_READ_FILE, 0666, NULL, &read_fops);
    pr_info("/proc/%s and /proc/%s are created.\n", PROC_WRITE_FILE, PROC_READ_FILE);
    return 0;
}

static void __exit proc_exit(void)
{
    remove_proc_entry(PROC_WRITE_FILE, NULL);
    remove_proc_entry(PROC_READ_FILE, NULL);
    pr_info("/proc/%s and /proc/%s are removed.\n", PROC_WRITE_FILE, PROC_READ_FILE);
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");

