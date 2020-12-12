#include <linux/init.h>	// for LKM
#include <linux/module.h> 	//
#include <linux/version.h>	//
#include <linux/kernel.h>	//
#include <linux/types.h>	// for dev_t
#include <linux/kdev_t.h>	//
#include <linux/fs.h>		// interact with user space
#include <linux/uaccess.h>	//
#include <linux/device.h>	//
#include <linux/cdev.h>	// for cdev
#include <linux/random.h>	// random lib

#define DRIVER_AUTHOR "Nguyen Huy Tu <18120254@student.hcmus.edu.vn>"
#define DRIVER_DESC "Linux Kernel Module to generate a random number"

#define DEVICE_NAME "RANDOM_NUMBER" 	// device name found in /dev
#define DRIVER_NAME "rngDriver"
#define CLASS_NAME "rngClass"

#define MAX 100			// max range for random number

static dev_t first; 			// major & minor number
static struct cdev c_dev; 		// character device 
static struct class *cl = NULL; 	// device class



// call when using: 'sudo cat /dev/DEVICE_NAME'
static int device_open(struct inode* i, struct file* f){
	printk(KERN_INFO "%s: driver has been opened\n",DEVICE_NAME);
	return 0;
}

static int device_close(struct inode* i, struct file* f){
	printk(KERN_INFO "%s: driver has been closed\n", DEVICE_NAME);
	return 0;
}

static ssize_t device_read(struct file *f, char* buf, size_t len, loff_t* off){
	// create random num
	int rng;
	get_random_bytes(&rng, sizeof(rng));
	rng %= MAX;
	printk(KERN_INFO "Random number: %d\n", rng);

	// only read 1 time
	if(off > 0){
		return 0; 
	}
	
	if(copy_to_user(buf, &rng, sizeof(rng))){ // send to user
		printk(KERN_INFO "%s: failed to send to user space\n", DEVICE_NAME);
		return -EFAULT;
	}
	
	off++;
	
	return 1;
}


static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_close,
	.read = device_read
};


// constructor; call when using: 'sudo insmod randomModule.ko'
static int __init random_init(void) {

	if(alloc_chrdev_region(&first, 0, 1, DRIVER_NAME) < 0){
		return -1;
	}
	
	if((cl = class_create(THIS_MODULE, CLASS_NAME)) == NULL){
		unregister_chrdev_region(first, 1);
		printk(KERN_ALERT "%s: failed to register major number\n", DEVICE_NAME);
		return -1;
	}
	
	if(device_create(cl, NULL, first, NULL, DEVICE_NAME) == NULL){
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		printk(KERN_ALERT "%s: failed to register device class\n", DEVICE_NAME);
		return -1;
	}
	
	cdev_init(&c_dev, &fops);
	
	if(cdev_add(&c_dev, first, 1) == -1){
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		printk(KERN_ALERT "%s: failed to create character device\n", DEVICE_NAME);
		return -1;
	}

	printk(KERN_INFO "%s: module has been loaded\n", DEVICE_NAME);
	return 0;
}

// destructor; call when using: 'sudo rmmod randomModule'
static void __exit random_exit(void) {
	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);
	printk(KERN_INFO "%s: module has been unloaded\n", DEVICE_NAME);
}



module_init(random_init);
module_exit(random_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
