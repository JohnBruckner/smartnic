#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x24a92850, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x19c500f4, __VMLINUX_SYMBOL_STR(wait_for_completion_io_timeout) },
	{ 0xe95f5faa, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0xfa5333d7, __VMLINUX_SYMBOL_STR(kernel_sendmsg) },
	{ 0x1060adf, __VMLINUX_SYMBOL_STR(my_nid) },
	{ 0xaa666413, __VMLINUX_SYMBOL_STR(try_wait_for_completion) },
	{ 0xb22236a0, __VMLINUX_SYMBOL_STR(broadcast_my_node_info) },
	{ 0x7d59f35b, __VMLINUX_SYMBOL_STR(pcn_kmsg_process) },
	{ 0x2da157a8, __VMLINUX_SYMBOL_STR(sock_release) },
	{ 0xe0afe59e, __VMLINUX_SYMBOL_STR(down_interruptible) },
	{ 0xc00fcf65, __VMLINUX_SYMBOL_STR(set_popcorn_node_online) },
	{ 0x5e62b474, __VMLINUX_SYMBOL_STR(seq_printf) },
	{ 0xab7554aa, __VMLINUX_SYMBOL_STR(filp_close) },
	{ 0x37901f7a, __VMLINUX_SYMBOL_STR(param_set_int) },
	{ 0xb8bd60b, __VMLINUX_SYMBOL_STR(kernel_listen) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x33746cb, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0x485c10d8, __VMLINUX_SYMBOL_STR(param_ops_string) },
	{ 0xdc9e3c9e, __VMLINUX_SYMBOL_STR(kernel_read) },
	{ 0xfb578fc5, __VMLINUX_SYMBOL_STR(memset) },
	{ 0x2f2e30e8, __VMLINUX_SYMBOL_STR(kernel_connect) },
	{ 0x1916e38c, __VMLINUX_SYMBOL_STR(_raw_spin_unlock_irqrestore) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xa115aa4a, __VMLINUX_SYMBOL_STR(kthread_stop) },
	{ 0xa1c76e0a, __VMLINUX_SYMBOL_STR(_cond_resched) },
	{ 0x5792f848, __VMLINUX_SYMBOL_STR(strlcpy) },
	{ 0x93a6e0b2, __VMLINUX_SYMBOL_STR(io_schedule) },
	{ 0xc0c5509d, __VMLINUX_SYMBOL_STR(kernel_getpeername) },
	{ 0x373db350, __VMLINUX_SYMBOL_STR(kstrtoint) },
	{ 0x9dda1dd6, __VMLINUX_SYMBOL_STR(init_net) },
	{ 0x93fca811, __VMLINUX_SYMBOL_STR(__get_free_pages) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xaccabc6a, __VMLINUX_SYMBOL_STR(in4_pton) },
	{ 0xa8935f6e, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xfc93aa0d, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xe259ae9e, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0x680ec266, __VMLINUX_SYMBOL_STR(_raw_spin_lock_irqsave) },
	{ 0xdc5fe785, __VMLINUX_SYMBOL_STR(kernel_recvmsg) },
	{ 0x1e01660e, __VMLINUX_SYMBOL_STR(vsnprintf) },
	{ 0xf909cab2, __VMLINUX_SYMBOL_STR(kernel_accept) },
	{ 0x4302d0eb, __VMLINUX_SYMBOL_STR(free_pages) },
	{ 0xb3f7646e, __VMLINUX_SYMBOL_STR(kthread_should_stop) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x5a03bb8, __VMLINUX_SYMBOL_STR(pcn_kmsg_set_transport) },
	{ 0xbc9b76cc, __VMLINUX_SYMBOL_STR(sock_create) },
	{ 0x78e739aa, __VMLINUX_SYMBOL_STR(up) },
	{ 0xa92efe06, __VMLINUX_SYMBOL_STR(kernel_bind) },
	{ 0x4610aee6, __VMLINUX_SYMBOL_STR(complete) },
	{ 0x6c47d785, __VMLINUX_SYMBOL_STR(filp_open) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

