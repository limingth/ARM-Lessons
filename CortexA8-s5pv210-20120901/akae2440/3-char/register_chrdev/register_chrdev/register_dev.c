#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>   /* copy_to_user, copy_from_user */
#include <linux/ioctl.h>

#define AKAE_MAJOR 240

MODULE_AUTHOR("Asmcos");
MODULE_DESCRIPTION("module example");
MODULE_LICENSE("GPL");

/* only user for ioctl */
//_IO(type,nr)  //ioctl没有参数要传递
//_IOR(type,nr,size) //ioctl仅仅读出设备参数(copy_to_user)
//_IOW(type,nr,size) //ioctl仅仅写入设备参数(copy_from_user)
//_IOWR(type,nr,size) //ioctl读出也写入设备参数
//type跟设备类型匹配的幻数， nr命令序号（序数）， size 数据大小（结构体类型名字)
//ioctl命令的组成：  direction(方向), size, type, nr 四个部分组成
//具体参见内核源码目录下的Documentation目录中的ioctl-numbers.txt文件。

//用简单的1或2作为ioctl命令数是不对的，内核会认为它是一个无效的命令，因为type、size等都为空。
//#define SET_A 1
//#define GET_A 2
#define SET_A _IOWR(300,1,struct kinfo)
#define GET_A _IOWR(300,2,struct kinfo)

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
	struct inode *inode = filp->f_dentry->d_inode;
	//struct inode *inode = filp->f_path.dentry->d_inode;
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
	int rc;

	printk("Test char dev\n");

	rc = register_chrdev(AKAE_MAJOR, "akae", &akae_fops);
	if(rc < 0)
	{
		printk("register %s char dev error\n", "akae");
		return -1;
	}

	//printk("%x,%x\n", SET_A, GET_A);
	printk("ok!\n");

	return 0;
}

void __exit
akae_exit(void)
{
	unregister_chrdev(AKAE_MAJOR, "akae");
	printk("module exit\n");
	return;
}

module_init(akae_init);
module_exit(akae_exit);

