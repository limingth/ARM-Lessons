#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>   /* copy_to_user, copy_from_user */
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/device.h>

#define AKAE_MAJOR 240

MODULE_AUTHOR("Asmcos");
MODULE_DESCRIPTION("module example");
//MODULE_LICENSE("GPL");
MODULE_LICENSE("Dual BSD/GPL");

/* only user for ioctl */
#define SET_A 1
#define GET_A 2

static int my_major = AKAE_MAJOR, my_minor = 0;
module_param(my_major, int, 0);
module_param(my_minor, int, S_IRUGO);

struct cdev *my_cdev;

//用动态分配方法得到主设备号
dev_t dev;

struct class *ex1_class;

struct kinfo{
	char a;
	int b;
};

int akae_open(struct inode *inode, struct file *filp)
{
	int major, minor;

	minor = MINOR(inode->i_rdev);
	major = MAJOR(inode->i_rdev);

	printk("akae_open ok major=%d, minor=%d\n", major, minor);

	return 0;
}

ssize_t akae_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct inode *inode = filp->f_path.dentry->d_inode;
//	struct inode *inode = filp->f_dentry->d_inode; //2.6.17.14
	int major, minor;
	char *rbuf = "akae read";

	minor = MINOR(inode->i_rdev);
	major = MAJOR(inode->i_rdev);

	printk("akae_read ok major=%d, minor=%d\n", major, minor);

	/* rbuf <= 9 */
	if(count>9)
		count = 9;

	copy_to_user(buf, rbuf,count);

	return count;
}

ssize_t akae_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	//struct inode *inode = filp->f_dentry->d_inode;
	struct inode *inode = filp->f_path.dentry->d_inode;
	int major, minor;
	char wbuf[10];

	minor = MINOR(inode->i_rdev);
	major = MAJOR(inode->i_rdev);

	printk("akae_write ok major=%d, minor=%d\n", major, minor);

	/* wbuf= 10 */
	if(count > 10)
		count = 10;

	copy_from_user(wbuf, buf, count);
	/* BUG? */
	wbuf[9] = 0;

	printk("user write:%s\n", wbuf);

	return count;
}

int akae_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct kinfo geta, seta;

	geta.a = 'k';
	geta.b = 33;

	switch(cmd)
	{
	case SET_A:
		copy_from_user(&seta, (void*)arg, sizeof(struct kinfo));
		printk("seta.a=%c,seta.b=%d\n", seta.a, seta.b);
		break;

	case GET_A:
		copy_to_user((void*)arg, &geta, sizeof(struct kinfo));
		break;

	default:
		break;
	}

	return 0;
}

int akae_release(struct inode *inode, struct file *filp)
{
	return 0;
}

struct file_operations akae_fops = {
	.owner = THIS_MODULE,
	.open  = akae_open,
	.read  = akae_read,
	.write = akae_write,
	.release = akae_release,
	.ioctl = akae_ioctl,
};

int __init
akae_init(void)
{
	int rc = 0;

	if(my_major) /*静态指定由宏定义的设备号*/
	{
		dev = MKDEV(my_major, my_minor);
		rc = register_chrdev_region(dev, 1, "akae");
	}
	else /*动态分配主设备号*/
	{
		rc = alloc_chrdev_region(&dev, my_minor, 1, "akae");
		my_major = MAJOR(dev);
	}

	if(rc < 0)
	{
		printk(KERN_WARNING "akae: can't get major %d\n", my_major);
                return rc;
	} 
	
	//字符设备的注册
	my_cdev = cdev_alloc();
	cdev_init(my_cdev, &akae_fops);
	my_cdev->owner = THIS_MODULE;
	rc = cdev_add(my_cdev, dev, 1);
	if(rc)
	{
		printk(KERN_NOTICE "Error %d adding akae.\n", rc);
		return -1;
	}

	printk("ok!\n");

	return 0;
}

void __exit
akae_exit(void)
{
	
	/*移除字符设备*/
	cdev_del(my_cdev);

	/*释放设备号*/
	unregister_chrdev_region(my_cdev->dev, 1);

	printk("module exit\n");
	return;
}

module_init(akae_init);
module_exit(akae_exit);



