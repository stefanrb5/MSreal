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

char databuf[100];
int pos = 0;
int endRead = 0;

int stred_open(struct inode *pinode, struct file *pfile);
int stred_close(struct inode *pinode, struct file *pfile);
ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);

static void delete(char string[], char substr[]);


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
	int ret,i;
	char buff[BUFF_SIZE];
	long int len ;
	char newline[3] = "\n";
	
	if (endRead){
		endRead = 0;
		pos = 0;
		printk(KERN_INFO "Succesfully read from file\n");
		return 0;
	}
			
		len = scnprintf(buff, BUFF_SIZE, "%c", databuf[pos]);
		 
		ret = copy_to_user(buffer, buff, len);
		
		if(ret)
			return -EFAULT;
		pos++;
		if(pos==100){
			endRead =1;
			copy_to_user(buffer,newline,len);
		}
		

	
	return len;
}

ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) 
{

	char command [15];
	int len;
	char tmpbuffer[100];	
	int maxdatalen = 100, ncopied;
	int i,j,k=5;
	char value[100];
	
	

	if (length<maxdatalen){
	maxdatalen = length;
	}

	ncopied = copy_from_user(tmpbuffer, buffer, maxdatalen);

	
	tmpbuffer[maxdatalen-1] ='\0';
	databuf[maxdatalen-1] = '\0';

	printk("tmpbuf: %s",tmpbuffer );


	ncopied = sscanf(tmpbuffer,"%10[^=]= %100[^\n]",command,value);



	printk("value: %s", value);
	printk("command: %s", command);
	
		
	if(strcmp(command, "string")==0)
	{
		len = sprintf(databuf,value,maxdatalen);	
	}
	else if(strcmp(command, "append")==0)
	{

		len= strcat(databuf,value);
			
		
	}	
	else if(strcmp(command, "truncate")==0)
	{
		len = strlen(databuf);
	
		while(k!=0)
		{
			databuf[len-k] = NULL;
			k--;
		}
	}
	
	else if(strcmp(command, "remove")==0)
	{
	
		delete(databuf,value);
	

	}

	else if(strcmp(command, "clear")==0)
	{
		len = strlen(databuf);

		for(i = 0; i<len;i++)
		{
			databuf[i] = '0';
		}
	}





	if(len){
		printk(KERN_INFO "Succesfully wrote in databuf\n");
		printk("databuf: %s", databuf);
		
	}
	else
		printk(KERN_INFO "failed to write in databuf\n");



	return length;
	
}

void delete(char string[], char substr[])
{
	int i = 0;
	int string_length = strlen(string);
	int substr_length = strlen(substr);
	int j;

	while(i < string_length)
	{
		if(strstr(&string[i], substr) == &string[i])
		{
			string_length -= substr_length;

			for(j=i; j< string_length;j++)
				string[j] = string[j+substr_length];
		}
		else i++;
	}
	string[i] = '\0';
}









static int __init stred_init(void)
{
   int ret = 0;
	int i=0;

	//Initialize array
	for (i=0; i<100; i++)
		databuf[i] = 0;

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
