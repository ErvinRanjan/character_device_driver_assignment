#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/version.h>

#define KERNEL_BUFFER_SIZE 512
#define MAX_PARAM_SIZE 100

dev_t dev = 0;
static struct class *dev_class;
static struct cdev chr_cdev;
uint8_t *kernel_buffer;
static int buffer_pointer;
static int kernel_version[MAX_PARAM_SIZE];
static int param_count = 0;
module_param_array(kernel_version,int,&param_count,0660);
MODULE_PARM_DESC(kernel_version,"major, minor, and patch");
static int open(struct inode *inode,struct file *file);
static int release(struct inode *inode,struct file *file);
static ssize_t read(struct file *file,char __user *buf,size_t len,loff_t *file_offset);
static ssize_t write(struct file *file,const char __user *buf,size_t len,loff_t *file_offset);
static int __init chr_driver_init(void);
static void __exit chr_driver_exit(void);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = read,
	.write = write,
	.open = open,
	.release = release
};


static int open(struct inode *inode,struct file *file){
	printk(KERN_INFO "Open function called\n");
	printk(KERN_INFO "Device File Opened\n");
	return 0;
}

static int release(struct inode *inode, struct file *file){
	printk(KERN_INFO "Close function called\n");
	printk(KERN_INFO "Device File Closed\n");
	return 0;
}

static ssize_t read(struct file *file,char __user *buf,size_t len,loff_t *file_offset){
	printk(KERN_INFO "Read function called\n");
	if(len > buffer_pointer){
		len = buffer_pointer;
	}

	if(*file_offset >= buffer_pointer){
		return 0;
	}
	
	int to_copy = len;
       	int not_copied = copy_to_user(buf,kernel_buffer,len);
	int data_read = to_copy - not_copied;	
	if(not_copied){
		printk(KERN_ERR "Failed to copy data to user space\n");
		return -EFAULT;
	}

	
	*file_offset += data_read;	
	printk(KERN_INFO "Data Read Success\n");
	return data_read;
}

static ssize_t write(struct file *file, const char __user *buf,size_t len,loff_t *file_offset){
	printk("Write function called\n");
	
	if(len > KERNEL_BUFFER_SIZE){
		len = KERNEL_BUFFER_SIZE;
	}

	int to_write = len;
	int not_written = copy_from_user(kernel_buffer,buf,len);
	int data_written = to_write - not_written;
	if(not_written){
		printk(KERN_ERR "Failed to copy data from user space\n");
		return -EFAULT;
	}

	buffer_pointer = data_written;
	
	printk(KERN_INFO "Data Write Success\n");
	return data_written;
}

static int chr_driver_init(void){
	if(LINUX_VERSION_CODE != KERNEL_VERSION(
		kernel_version[0],kernel_version[1],kernel_version[2]
		)
			){
		printk(KERN_ERR "kernel version does not match\n");
		printk(KERN_INFO "linux version code: %d\n",LINUX_VERSION_CODE);
		printk(KERN_INFO "kernel version code: %d\n",
				KERNEL_VERSION(
		kernel_version[0],kernel_version[1],kernel_version[2])
				);
		return -1;
	}
	
	if((kernel_buffer = kmalloc(KERNEL_BUFFER_SIZE,GFP_KERNEL)) == 0){
			printk(KERN_INFO "Cannot allocate memory\n");
			return -1;
	}


	if(alloc_chrdev_region(&dev,0,1,"character_device") < 0){
		printk(KERN_INFO "Cannot allocate major number\n");
	       return -1;	
	}
	
	printk(KERN_INFO "Major: %d, Minor: %d",MAJOR(dev),MINOR(dev));
	cdev_init(&chr_cdev,&fops);

	if(cdev_add(&chr_cdev,dev,1) < 0){
		printk(KERN_INFO "Cannot add the device to the system\n");
		goto destroy_class;
	}

	if((dev_class = class_create("character_device_class")) == 0){
		printk(KERN_INFO "Cannot create class\n");
		goto destroy_class;
	}

	if(device_create(dev_class,NULL,dev,NULL,"character_device_1") == NULL){
		printk(KERN_INFO "Cannot create device\n");
		goto destroy_device;
	}

	printk(KERN_INFO "Device Driver Insert Success\n");

	return 0;

	destroy_device:
		class_destroy(dev_class);
	
	destroy_class:
		unregister_chrdev_region(dev,1);
		cdev_del(&chr_cdev);
	
	return -1;
}

static void __exit chr_driver_exit(void){
	kfree(kernel_buffer);
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(&chr_cdev);
	unregister_chrdev_region(dev,1);
	printk(KERN_INFO "Device Driver Remove Success\n");
} 


module_init(chr_driver_init);
module_exit(chr_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ervin Ranjan");
MODULE_DESCRIPTION("Linux Character Device Driver");
