#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x122c3a7e, "_printk" },
	{ 0x15ba50a6, "jiffies" },
	{ 0xe2964344, "__wake_up" },
	{ 0xe2c17b5d, "__SCT__might_resched" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x1000e51, "schedule" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0x92540fbf, "finish_wait" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x3185476b, "cdev_init" },
	{ 0xaabf546, "cdev_add" },
	{ 0xc74fa6a1, "class_create" },
	{ 0xf4ee463a, "device_create" },
	{ 0x69278d43, "gpiod_get" },
	{ 0x1b1db38c, "gpiod_to_irq" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0x3a1b27aa, "_dev_err" },
	{ 0x81ef79db, "platform_driver_unregister" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x2d3385d3, "system_wq" },
	{ 0xc5b6f236, "queue_work_on" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xfc802b99, "__platform_driver_register" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x3c06c15f, "device_destroy" },
	{ 0x1fc34ff, "class_destroy" },
	{ 0xf41c271a, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xc1514a3b, "free_irq" },
	{ 0x47baf7be, "gpiod_put" },
	{ 0x3b4cf56c, "_dev_info" },
	{ 0xc6227e48, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cgpiovendor,gpio_btn");
MODULE_ALIAS("of:N*T*Cgpiovendor,gpio_btnC*");

MODULE_INFO(srcversion, "32BEA1BA452B36C23B5DDD5");
