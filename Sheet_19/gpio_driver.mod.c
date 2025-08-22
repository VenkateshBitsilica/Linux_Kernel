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
	{ 0xf0dba330, "gpiod_set_value" },
	{ 0x47baf7be, "gpiod_put" },
	{ 0x3b4cf56c, "_dev_info" },
	{ 0x69278d43, "gpiod_get" },
	{ 0x3a1b27aa, "_dev_err" },
	{ 0x81ef79db, "platform_driver_unregister" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xfc802b99, "__platform_driver_register" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xc6227e48, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cgpiovendor,gpio_led");
MODULE_ALIAS("of:N*T*Cgpiovendor,gpio_ledC*");

MODULE_INFO(srcversion, "2E27E0D579DA63EB3925FBF");
