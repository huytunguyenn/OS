#include <linux/module.h> /*thu vien dinh nghia cac marco nhu module_init va module_exit */

#define DRIVER_AUTHOR "HUY TU"
#define DRIVER_DESC "test bai tap HDH"

static int __init init_hello(void){
	printk("xin chao VN\n");
	return 0;
}
static void __exit exit_hello(void){
	printk("tam biet the gioi\n");
}

module_init(init_hello);
module_exit(exit_hello);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("testdevice");
