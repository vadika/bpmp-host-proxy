/**
 * 
 * NVIDIA BPMP Host Proxy Kernel Module
 * (c) 2023 Unikie, Oy
 * (c) 2023 Vadim Likholetov vadim.likholetov@unikie.com
 * 
*/
#include <linux/module.h>	  // Core header for modules.
#include <linux/device.h>	  // Supports driver model.
#include <linux/kernel.h>	  // Kernel header for convenient functions.
#include <linux/fs.h>		  // File-system support.
#include <linux/uaccess.h>	  // User access copy function support.
#include <linux/slab.h>
#include <soc/tegra/bpmp.h>

#define DEVICE_NAME "bpmp-host" // Device name.
#define CLASS_NAME "char"	  /// < The device class -- this is a character device driver

MODULE_LICENSE("GPL");						 ///< The license type -- this affects available functionality
MODULE_AUTHOR("Vadim Likholetov");					 ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("NVidia BPMP Host Proxy Kernel Module"); ///< The description -- see modinfo
MODULE_VERSION("0.1");						 ///< A version number to inform users

/**
 * Important variables that store data and keep track of relevant information.
 */
static int major_number;

static struct class *bpmp_host_proxy_class = NULL;	///< The device-driver class struct pointer
static struct device *bpmp_host_proxy_device = NULL; ///< The device-driver device struct pointer

/**
 * Prototype functions for file operations.
 */
static int open(struct inode *, struct file *);
static int close(struct inode *, struct file *);
static ssize_t read(struct file *, char *, size_t, loff_t *);
static ssize_t write(struct file *, const char *, size_t, loff_t *);

/**
 * File operations structure and the functions it points to.
 */
static struct file_operations fops =
	{
		.owner = THIS_MODULE,
		.open = open,
		.release = close,
		.read = read,
		.write = write,
};




/**
 * Initializes module at installation
 */
int init_module(void)
{

	
	printk(KERN_INFO "bpmp-host-proxy: installing module.\n");

	// Allocate a major number for the device.
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	if (major_number < 0)
	{
		printk(KERN_ALERT "bpmp-host-proxy: could not register number.\n");
		return major_number;
	}
	printk(KERN_INFO "bpmp-host-proxy: registered correctly with major number %d\n", major_number);

	// Register the device class
	bpmp_host_proxy_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(bpmp_host_proxy_class))
	{ // Check for error and clean up if there is
		unregister_chrdev(major_number, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(bpmp_host_proxy_class); // Correct way to return an error on a pointer
	}
	printk(KERN_INFO "bpmp-host-proxy:: device class registered correctly\n");

	// Register the device driver
	bpmp_host_proxy_device = device_create(bpmp_host_proxy_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
	if (IS_ERR(bpmp_host_proxy_device))
	{								 // Clean up if there is an error
		class_destroy(bpmp_host_proxy_class); 
		unregister_chrdev(major_number, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(bpmp_host_proxy_device);
	}
	printk(KERN_INFO "bpmp-host-proxy: device class created correctly\n"); // Made it! device was initialized


	return 0;
}



/*
 * Removes module, sends appropriate message to kernel
 */
void cleanup_module(void)
{
	printk(KERN_INFO "bpmp-host-proxy: removing module.\n");
	device_destroy(bpmp_host_proxy_class, MKDEV(major_number, 0)); // remove the device
	class_unregister(bpmp_host_proxy_class);						  // unregister the device class
	class_destroy(bpmp_host_proxy_class);						  // remove the device class
	unregister_chrdev(major_number, DEVICE_NAME);		  // unregister the major number
	printk(KERN_INFO "bpmp-host-proxy: Goodbye from the LKM!\n");
	unregister_chrdev(major_number, DEVICE_NAME);
	return;
}

/*
 * Opens device module, sends appropriate message to kernel
 */
static int open(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "bpmp-host-proxy: device opened.\n");
	return 0;
}

/*
 * Closes device module, sends appropriate message to kernel
 */
static int close(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "bpmp-host-proxy: device closed.\n");
	return 0;
}

/*
 * Reads from device, displays in userspace, and deletes the read data
 */
static ssize_t read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	printk(KERN_INFO "bpmp-host-proxy: read stub");
	return 0;
}

/*
 * Writes to the device
 */


extern int tegra_bpmp_transfer(struct tegra_bpmp *, struct tegra_bpmp_message *);
extern struct tegra_bpmp *tegra_bpmp_host_device;

#define BUF_SIZE 1024 

static ssize_t write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{

	int ret = len;
	struct tegra_bpmp_message *kbuf = NULL;
	void *txbuf = NULL;
	void *rxbuf = NULL;
	void *usertxbuf = NULL;
	void *userrxbuf = NULL;

	if (len > 65535) {	/* paranoia */
		printk("count %zu exceeds max # of bytes allowed, "
			"aborting write\n", len);
		goto out_nomem;
	}

	printk(" wants to write %zu bytes\n", len);

	if (len!=sizeof(struct tegra_bpmp_message ))
	{
		printk("bpmp-host: message size %zu != %zu", len, sizeof(struct tegra_bpmp_message));
		goto out_notok;
	}

	ret = -ENOMEM;
	kbuf = kmalloc(len, GFP_KERNEL);
	txbuf = kmalloc(BUF_SIZE, GFP_KERNEL);
	rxbuf = kmalloc(BUF_SIZE, GFP_KERNEL);

	if (!kbuf || !txbuf || !rxbuf)
		goto out_nomem;

	memset(kbuf, 0, len);
	memset(txbuf, 0, len);
	memset(rxbuf, 0, len);

	ret = -EFAULT;
	
	if (copy_from_user(kbuf, buffer, len)) {
		printk("copy_from_user(1) failed\n");
		goto out_cfu;
	}

	if (copy_from_user(txbuf, kbuf->tx.data, kbuf->tx.size)) {
		printk("copy_from_user(2) failed\n");
		goto out_cfu;
	}

	if (copy_from_user(rxbuf, kbuf->rx.data, kbuf->rx.size)) {
		printk("copy_from_user(3) failed\n");
		goto out_cfu;
	}	

	usertxbuf=kbuf->tx.data; //save userspace buffers addresses
	userrxbuf=kbuf->rx.data;

	kbuf->tx.data=txbuf; //reassing to kernel space buffers
	kbuf->rx.data=rxbuf;


	if(!tegra_bpmp_host_device){
		printk("bpmp-host: host device not initialised, can't do transfer!");
		return -EFAULT;
	}

	ret = tegra_bpmp_transfer(tegra_bpmp_host_device, (struct tegra_bpmp_message *)kbuf);



	if (copy_to_user((void *)usertxbuf, kbuf->tx.data, kbuf->tx.size)) {
		printk("copy_to_user(2) failed\n");
		goto out_notok;
	}

	if (copy_to_user((void *)userrxbuf, kbuf->rx.data, kbuf->rx.size)) {
		printk("copy_to_user(3) failed\n");
		goto out_notok;
	}

	kbuf->tx.data=usertxbuf;
	kbuf->rx.data=userrxbuf;
	
	if (copy_to_user((void *)buffer, kbuf, len)) {
		printk("copy_to_user(1) failed\n");
		goto out_notok;
	}



	kfree(kbuf);
	return len;
out_notok:
out_nomem:
	printk ("memory allocation failed");
out_cfu:
	kfree(kbuf);
	kfree(txbuf);
	kfree(rxbuf);
    return -EINVAL;

}

