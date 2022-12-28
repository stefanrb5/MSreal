#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/device.h>
#define BUFF_SIZE 100
MODULE_LICENSE("Dual BSD/GPL");

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;

char lifo[100];
int pos = 0;
int endRead = 0;

int stred_open(struct inode *pinode, struct file *pfile);
int stred_close(struct inode *pinode, struct file *pfile);
ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);

struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = stred_open,
	.read = stred_read,
	.write = stred_write,
	.release = stred_close,
};


int stred_open(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully opened stred\n");
		return 0;
}

int stred_close(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully closed stred\n");
		return 0;
}

ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) 
{
	int ret;
	char buff[BUFF_SIZE];
	long int len = 0;
	if (endRead){
		endRead = 0;
		return 0;
	}

	if(pos > 0)
	{
		pos --;
		len = scnprintf(buff, BUFF_SIZE, "%c", lifo[pos]);
		ret = copy_to_user(buffer, buff, len);
		if(ret)
			return -EFAULT;
		printk(KERN_INFO "Succesfully read\n");
		endRead = 1;
	}
	else
	{
			printk(KERN_WARNING "Lifo is empty\n"); 
	}

	return len;
}

ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) 
{
/*	char buff[BUFF_SIZE];
	char value[10]={"aaaaaa"};
	int ret;
	int len;
	char command[15];
	

	ret = copy_from_user(buff, buffer, length);
	if(ret)
		return -EFAULT;
	buff[length-1] = '\0';

		ret = sscanf(buff,"%s[=/]%s",command,value);
		len = strlcpy(buff,value,BUFF_SIZE);




	ret=sscanf(buff, "%s", value);

	return length;*/
	char command [15];
	int len;
	int maxdatalen = 30, ncopied;
	char databuf[maxdatalen];
	char value[maxdatalen];
	char separator[2];

	if (length<maxdatalen){
	maxdatalen = length;
	}

	ncopied = copy_from_user(databuf, buffer, maxdatalen);

	
	databuf[maxdatalen-1] ='\0';
	printk("databuf: %s",databuf );

	ncopied = sscanf(databuf,"%[^=]= %[^\0]",command,value);

	len = sprintf(databuf,value,maxdatalen);

	printk("value: %s", value);
	printk("command: %s", command);

	if(len){
		printk(KERN_INFO "Succesfully wrote in databuf\n");
		printk("buffer: %s", databuf);
	}
	else
		printk(KERN_INFO "failed to write in databuf\n");



	return length;
	
}

static int __init stred_init(void)
{
   int ret = 0;
	int i=0;

	//Initialize array
	for (i=0; i<10; i++)
		lifo[i] = 0;

   ret = alloc_chrdev_region(&my_dev_id, 0, 1, "stred");
   if (ret){
      printk(KERN_ERR "failed to register char device\n");
      return ret;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "stred_class");
   if (my_class == NULL){
      printk(KERN_ERR "failed to create class\n");
      goto fail_0;
   }
   printk(KERN_INFO "class created\n");
   
   my_device = device_create(my_class, NULL, my_dev_id, NULL, "stred");
   if (my_device == NULL){
      printk(KERN_ERR "failed to create device\n");
      goto fail_1;
   }
   printk(KERN_INFO "device created\n");

	my_cdev = cdev_alloc();	
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, 1);
	if (ret)
	{
      printk(KERN_ERR "failed to add cdev\n");
		goto fail_2;
	}
   printk(KERN_INFO "cdev added\n");
   printk(KERN_INFO "Hello world\n");

   return 0;

   fail_2:
      device_destroy(my_class, my_dev_id);
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}

static void __exit stred_exit(void)
{
   cdev_del(my_cdev);
   device_destroy(my_class, my_dev_id);
   class_destroy(my_class);
   unregister_chrdev_region(my_dev_id,1);
   printk(KERN_INFO "Goodbye, cruel world\n");
}


module_init(stred_init);
module_exit(stred_exit);