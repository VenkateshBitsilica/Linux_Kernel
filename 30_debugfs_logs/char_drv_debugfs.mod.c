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
	{ 0x3185476b, "cdev_init" },
	{ 0xaabf546, "cdev_add" },
	{ 0xc74fa6a1, "class_create" },
	{ 0xf4ee463a, "device_create" },
	{ 0x83c2031b, "debugfs_create_dir" },
	{ 0x8454a43b, "debugfs_create_u32" },
	{ 0xf41c271a, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x1fc34ff, "class_destroy" },
	{ 0xa916b694, "strnlen" },
	{ 0x619cb7dd, "simple_read_from_buffer" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0x575e2ddc, "debugfs_remove" },
	{ 0x3c06c15f, "device_destroy" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x122c3a7e, "_printk" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xc6227e48, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "0EAFC8E3372E36FC707DCE9");
