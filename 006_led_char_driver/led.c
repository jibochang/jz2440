#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/kdev_t.h>


#include <asm/uaccess.h>
#include <asm/hardware.h>
#include <asm/io.h>

/* GPIO引脚地址 */
#define S3C2440_GPFCON	0x56000050


/* 设备名 */
#define DEVICE_NAME		"s3c2440_leds"

static unsigned int major;	/* 主设备号 */
static struct class *s3c2440_leds_class;		
static struct class_device *s3c2440_leds_dev;	


volatile unsigned long * gpfcon;
volatile unsigned long * gpfdat;
volatile unsigned long * gpfup;


static int s3c2440_leds_open(struct inode *inode, struct file *file)
{
	/* 配置GPIO4\5\6为输出引脚 */
	*gpfcon &= ~((0x3 << 8) | (0x3 << 10) | (0x3 << 12)); /* 清零 */
	*gpfcon |=  ((0x1 << 8) | (0x1 << 10) | (0x1 << 12)); /* 设置为输出 */
	return 0;
}



static int s3c2440_leds_close(struct inode *inode, struct file *file)
{
	/* 什么都不干 */
	return 0;
}

static int s3c2440_leds_write(struct file * file, 
		const char __user *buf, size_t count, loff_t *ppos)
{
	int val;
	unsigned long ret;
	ret = copy_from_user(&val, buf, count);
	if (val == 1) 
		*gpfdat &= ~((1 << 4) | (1 << 5) | (1 << 6));	/* 点灯 */
	else
		*gpfdat |=  ((1 << 4) | (1 << 5) | (1 << 6));	/* 灭灯 */
	return 0;
}

static struct file_operations s3c2440_leds_fops = {
	.owner   =      THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
	.open    =      s3c2440_leds_open,
	.release =      s3c2440_leds_close,
	.write   =      s3c2440_leds_write,
};



static int __init s3c2440_leds_init(void)
{
	/* 注册字符设备
	 *          *          *      * 参数为主设备号、设备名字、file_operations结构；
	 *                   *                   *           * 这样，主设备号就和具体的file_operations结构联系起来了，
	 *                            *                            *                * 操作主设备为LED_MAJOR的设备文件时，就会调用s3c24xx_leds_fops中的相关成员函数
	 *                                     *                                     *                     * LED_MAJOR可以设为0，表示由内核自动分配主设备号, 此时反回值就是主设备号
	 *                                              *                                              *                          */
	major = register_chrdev(0, DEVICE_NAME, &s3c2440_leds_fops);    /* 注册驱动程序 */
	if (major < 0) {
		printk(DEVICE_NAME " can't register major number\n");
		return major;
	}

	/* 创建逻辑类 */
	s3c2440_leds_class = class_create(THIS_MODULE, "s3c2440_leds");
	if (IS_ERR(s3c2440_leds_class))
		return PTR_ERR(s3c2440_leds_class);

	/* 在class目录下创建一个设备，使得mdev可以根据设备信息
	 *          *          *       * 生成设备节点——/dev/s3c2440_leds 
	 *                   *                   *               */
	s3c2440_leds_dev = class_device_create(s3c2440_leds_class, NULL,
			MKDEV(major, 0), NULL, "s3c2440_leds");
	if (unlikely(IS_ERR(s3c2440_leds_dev)))
		return PTR_ERR(s3c2440_leds_dev);

	/* 驱动程序可操作的虚拟地址，映射到GIPO的物理地址映射，
	 *          *          *       * 从而实现通过指针操作GPIO引脚 
	 *                   *                   *               */
	gpfcon = (volatile unsigned long*)ioremap(S3C2440_GPFCON, 16);
	gpfdat = gpfcon + 1;
	gpfup  = gpfdat + 1;

	printk(DEVICE_NAME " initialized\n");
	return 0;
}

static void __exit s3c2440_leds_exit(void)
{
	/* 卸载驱动程序 */
	class_device_destroy(s3c2440_leds_class, MKDEV(major, 0));
	class_destroy(s3c2440_leds_class);
	unregister_chrdev(major, DEVICE_NAME);
	iounmap(gpfcon);
	printk(DEVICE_NAME " unregister\n");
}



module_init(s3c2440_leds_init);
module_exit(s3c2440_leds_exit);

MODULE_LICENSE("GPL");
