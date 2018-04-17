/*****************************************************************************
 Copyright(c) 2012 NMI Inc. All Rights Reserved

 File name : nmi326.c

 Description : NM326 host interface

 History :
 ----------------------------------------------------------------------
 2012/11/27 	ssw		initial
*******************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
//#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <linux/irq.h>
//#include <asm/irq.h>
#include <asm/mach/irq.h>

#include <linux/wait.h>
#include <linux/stat.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <linux/slab.h>
//#include <plat/gpio.h>
//#include <plat/mux.h>

#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/vmalloc.h>

#include <linux/io.h>
#include <mach/board.h>
//#include <mach/gpio.h>
#include <linux/gpio.h>

#include <linux/time.h>
#include <linux/regulator/consumer.h>
#include <linux/of_gpio.h>

#include "nmi326.h"
#include "nmi326_spi_drv.h"

static struct class *isdbt_class;

static wait_queue_head_t isdbt_irq_waitqueue;
static char g_bCatchIrq = 0;
static int g_irq_status = DTV_IRQ_DEINIT;

//#define NMI_POWER_PIN		79 DP 92 PDP2
//#define NMI_RESET_PIN		91 DP
//#define NMI_IRQN_PIN		101 DP 90 PDP2
//#define NMI_IRQN_IRQ		MSM_GPIO_TO_INT(NMI_IRQN_PIN)

static int nmi_iron_irq =0; 
static int chip_open =0;

//static int NMI_POWER_PIN=0;
//static int NMI_RESET_PIN=0;
//static int NMI_IRQN_PIN=0;
//static int HW_DP_DET_PIN= 35;

#define NM32X_IRQ_NAME		"NM32X_IRQ"


#define SPI_RW_BUF		(188*50*2)

#define GPIO_DTV_SELECT 49

static int dtv_status =0;

static struct nmi_spi_dtv_data *nmi;

/*[Arima5908][39233][bozhi_lin] fix after launch DTV apk can't turn off vreg_l26 & vreg_l28 20140606 begin*/
static int chip_is_power_on =0;
/*[Arima5908][39233][bozhi_lin] 20140606 end  */
//----------------------------------------------------------------------
// [[ GPIO


int isdbt_gpio_init(struct platform_device *pdev)
{
	int ret=0;
	struct device_node *dnode = pdev->dev.of_node;

	nmi->reset_gpio = of_get_named_gpio_flags(dnode,
			"nmi,reset-gpio", 0, &nmi->reset_flags);
			
	nmi->irq_gpio = of_get_named_gpio_flags(dnode,
			"nmi,irq-gpio", 0, &nmi->irq_flags);

	nmi->rf_switch_gpio = of_get_named_gpio_flags(dnode, 
			"nmi,rf_switch-gpio", 0, &nmi->rf_switch_flags);
			
	nmi->dtv_select_gpio = of_get_named_gpio_flags(dnode, 
			"nmi,dtv_select-gpio", 0, &nmi->dtv_select_flags);
				
	pr_info("[B]%s(%d): nmi->reset_gpio=%d, nmi->irq_gpio=%d\n", __func__, __LINE__, nmi->reset_gpio, nmi->irq_gpio);
	pr_info("[B]%s(%d): nmi->rf_switch_gpio=%d, nmi->dtv_select_gpio=%d\n", __func__, __LINE__, nmi->rf_switch_gpio, nmi->dtv_select_gpio);

	ret = gpio_request_one(nmi->dtv_select_gpio, GPIOF_IN, "nmi_dtv_select");	
	if (ret)
		goto fail;

/*[Arima5908][34917][bozhi_lin] fix DTV cause suspend current issue 20140317 begin*/
	if(gpio_get_value(nmi->dtv_select_gpio))
	{
		pr_info("[B]%s(%d): GPIO_%d is HIGH(%d), DTV enable\n", __func__, __LINE__, nmi->dtv_select_gpio, gpio_get_value(nmi->dtv_select_gpio));
	}
	else
	{
		goto fail_gpio_dtv_select;
	} 
/*[Arima5908][34917][bozhi_lin] 20140317 end  */

	// interrupt
	ret = gpio_request_one(nmi->irq_gpio, GPIOF_IN, "nmi_int");
	if (ret)
		goto fail_gpio_dtv_select;

	//PWR Enable
	ret = gpio_request_one(nmi->rf_switch_gpio, GPIOF_OUT_INIT_LOW, "nmi_rf_switch");
	if (ret)
		goto fail_gpio_irq;

	//n_Reset
	ret = gpio_request_one(nmi->reset_gpio, GPIOF_OUT_INIT_HIGH,"nmi_rst");
	if (ret)
		goto fail_gpio_rf_switch;

	nmi_iron_irq = gpio_to_irq(nmi->irq_gpio);
	
	//gpio_set_value(NMI_POWER_PIN,1);
	
	//msleep(100);	
	/*
	gpio_set_value(NMI_RESET_PIN,0);
	msleep(100);	
	gpio_set_value(NMI_RESET_PIN,1);
	msleep(100);
	*/
	nmi->vdd_io = regulator_get(&pdev->dev, "vdd_io");

	if (IS_ERR(nmi->vdd_io)) {
		ret = PTR_ERR(nmi->vdd_io);
		goto fail_gpio;
	}

	nmi->vdd = regulator_get(&pdev->dev, "vdd");

	if (IS_ERR(nmi->vdd)) {
		regulator_put(nmi->vdd_io);
		ret = PTR_ERR(nmi->vdd);
		goto fail_gpio;
	}

/*[Arima5908][34917][bozhi_lin] fix DTV cause suspend current issue 20140317 begin*/
#if 0
	ret = regulator_enable(nmi->vdd_io);
	if (ret) {
		pr_info("[B]%s(%d): Regulator vdd enable failed rc=%d\n", __func__, __LINE__, ret);
	}	
	
	ret = regulator_enable(nmi->vdd);
	if (ret) {
		pr_info("[B]%s(%d): Regulator vdd enable failed rc=%d\n", __func__, __LINE__, ret);
	}
#endif	
/*[Arima5908][34917][bozhi_lin] 20140317 end  */
	
	pr_info("[B]%s(%d): \n", __func__, __LINE__);
	return 0;
		
fail_gpio:
	if (gpio_is_valid(nmi->reset_gpio))
		gpio_free(nmi->reset_gpio);

fail_gpio_rf_switch:
	if (gpio_is_valid(nmi->rf_switch_gpio))
		gpio_free(nmi->rf_switch_gpio);

fail_gpio_irq:		
	if (gpio_is_valid(nmi->irq_gpio))
		gpio_free(nmi->irq_gpio);
		
fail_gpio_dtv_select:
	if (gpio_is_valid(nmi->dtv_select_gpio))
		gpio_free(nmi->dtv_select_gpio);
		
fail:
	ret = -EINVAL;
	return ret;	
}


void isdbt_gpio_power_on(void)
{
	int retval;

	/*[Arima5908][39233][bozhi_lin] fix after launch DTV apk can't turn off vreg_l26 & vreg_l28 20140606 begin*/
	if (chip_is_power_on) {
		NM_KMSG("chip is already power on\n");
		return;
	}
	/*[Arima5908][39233][bozhi_lin] 20140606 end  */

	retval = regulator_enable(nmi->vdd_io);
	if (retval) {
		pr_info("[B]%s(%d): Regulator vdd enable failed rc=%d\n", __func__, __LINE__, retval);
	}

	retval = regulator_enable(nmi->vdd);
	if (retval) {
		pr_info("[B]%s(%d): Regulator vdd enable failed rc=%d\n", __func__, __LINE__, retval);
	}
	
	/* Wait for atleast 10ms after turning on regulator */
	//usleep_range(10000, 11000);	

	gpio_set_value(nmi->rf_switch_gpio, 1);
	
	/*[Arima5908][39233][bozhi_lin] fix after launch DTV apk can't turn off vreg_l26 & vreg_l28 20140606 begin*/
	chip_is_power_on = 1;
	/*[Arima5908][39233][bozhi_lin] 20140606 end  */
}

void isdbt_gpio_power_off(void)
{
	int retval;
	
	/*[Arima5908][39233][bozhi_lin] fix after launch DTV apk can't turn off vreg_l26 & vreg_l28 20140606 begin*/
	if (!chip_is_power_on) {
		NM_KMSG("chip is already power off\n");
		return;
	}
	/*[Arima5908][39233][bozhi_lin] 20140606 end  */
	
	gpio_set_value(nmi->rf_switch_gpio, 0);
	
	retval = regulator_disable(nmi->vdd);
	if (retval) {
		pr_info("[B]%s(%d): Regulator vdd disable failed rc=%d\n", __func__, __LINE__, retval);
	}
	
	retval = regulator_disable(nmi->vdd_io);
	if (retval) {
		pr_info("[B]%s(%d): Regulator vdd disable failed rc=%d\n", __func__, __LINE__, retval);
	}
	
	/*[Arima5908][39233][bozhi_lin] fix after launch DTV apk can't turn off vreg_l26 & vreg_l28 20140606 begin*/
	chip_is_power_on = 0;
	/*[Arima5908][39233][bozhi_lin] 20140606 end  */
}

void isdbt_gpio_reset_up(void)
{
	gpio_set_value(nmi->reset_gpio, 1);
}

void isdbt_gpio_reset_down(void)
{
	gpio_set_value(nmi->reset_gpio, 0);
}

static irqreturn_t isdbt_irq_handler(int irq, void *dev_id)
{
	ISDBT_OPEN_INFO_T* isdbt_dev = (ISDBT_OPEN_INFO_T*)(dev_id);
	unsigned long flags;

	spin_lock_irqsave(&isdbt_dev->isr_lock, flags);
	disable_irq_nosync(nmi_iron_irq);
	g_irq_status = DTV_IRQ_INIT;
	g_bCatchIrq = 1;
	wake_up(&isdbt_irq_waitqueue);
	spin_unlock_irqrestore(&isdbt_dev->isr_lock, flags);

    	NM_KMSG("<isdbt> %s\n", __FUNCTION__);
	return IRQ_HANDLED;
}

void isdbt_gpio_interrupt_register(ISDBT_OPEN_INFO_T* pdev)
{
	int ret=1;
	// configuration for detect
	NM_KMSG("<isdbt> isdbt_gpio_interrupt_register, IN [+]\n, chip_open=%d",chip_open);

	// irq register

		ret = request_irq(nmi_iron_irq, isdbt_irq_handler,
			 IRQF_TRIGGER_LOW, NM32X_IRQ_NAME, pdev);
		if (ret) {
			NM_KMSG("IOCTL_ISDBT_INTERRUPT_REGISTER request_irq failed\n");
		}

		// disable irq
	//disable_irq_nosync(nmi_iron_irq);  should we disable it??
		NM_KMSG("<isdbt> isdbt_gpio_interrupt_register, OUT [-]\n");

		
	
}

void isdbt_gpio_interrupt_unregister(ISDBT_OPEN_INFO_T* pdev)
{

		NM_KMSG("<isdbt> isdbt_gpio_interrupt_unregister, IN [+], chip_open=%d \n",chip_open);

		free_irq(nmi_iron_irq, pdev);

		NM_KMSG("<isdbt> isdbt_gpio_interrupt_unregister, OUT [-]\n");

}

void isdbt_gpio_interrupt_enable(void)
{
    NM_KMSG("<isdbt> isdbt_gpio_interrupt_enable, IN [-], chip_open=%d \n",chip_open);
	enable_irq(nmi_iron_irq);
    NM_KMSG("<isdbt> isdbt_gpio_interrupt_enable, OUT [-]\n");
}

void isdbt_gpio_interrupt_disable(void)
{
    NM_KMSG("<isdbt> isdbt_gpio_interrupt_disable, IN [-], chip_open=%d \n",chip_open);
	 disable_irq_nosync(nmi_iron_irq);
	 NM_KMSG("<isdbt> isdbt_gpio_interrupt_disable, OUT [-]\n");
}

// ]]] GPIO
//----------------------------------------------------------------------
static void isdbt_read_gpio(void)
{
    int val = -1;
    val = gpio_get_value(nmi->irq_gpio);
    NM_KMSG("~~~~~ <isdbt> val=%d\n", val);
}

static unsigned int	isdbt_poll(struct file *filp, poll_table *wait)
{
	unsigned int mask = 0;

	poll_wait(filp, &isdbt_irq_waitqueue, wait);

	if( g_irq_status == DTV_IRQ_DEINIT)
	{
		NM_KMSG("<isdbt> DTV_IRQ_DEINIT - do nothing\n");

      	isdbt_read_gpio();
		return mask;
	}

	if ( g_bCatchIrq == 1)
	{
		mask |= (POLLIN | POLLRDNORM);
		NM_KMSG("<isdbt> poll: release (%d)\n", g_bCatchIrq);
		g_bCatchIrq = 0;
	} else {
		NM_KMSG("<isdbt> poll: release (%d)\n", g_bCatchIrq);
	}

	return mask;
}

static int isdbt_open(struct inode *inode, struct file *filp)
{
	ISDBT_OPEN_INFO_T *pdev = NULL;

	NM_KMSG("<isdbt> isdbt open\n");	

	pdev = (ISDBT_OPEN_INFO_T *)kmalloc(sizeof(ISDBT_OPEN_INFO_T), GFP_KERNEL);
	if(pdev == NULL)
	{
		NM_KMSG("<isdbt> pdev kmalloc FAIL\n");
		return -1;
	}

	NM_KMSG("<isdbt> pdev kmalloc SUCCESS\n");

	memset(pdev, 0x00, sizeof(ISDBT_OPEN_INFO_T));
	//g_irq_status = DTV_IRQ_DEINIT;
	g_bCatchIrq = 0;

	filp->private_data = pdev;

	pdev->rwBuf = kmalloc(SPI_RW_BUF, GFP_KERNEL);

	if(pdev->rwBuf == NULL)
	{
		NM_KMSG("<isdbt> pdev->rwBuf kmalloc FAIL\n");
		return -1;
	}

	spin_lock_init(&pdev->isr_lock);
	init_waitqueue_head(&isdbt_irq_waitqueue);

	/*[Arima5908][39233][bozhi_lin] fix after launch DTV apk can't turn off vreg_l26 & vreg_l28 20140606 begin*/
	isdbt_gpio_power_on();
	isdbt_gpio_reset_up();

	if( g_irq_status == DTV_IRQ_DEINIT && dtv_status==0)
	{
		g_bCatchIrq = 0;
		isdbt_gpio_interrupt_register(pdev);
		g_irq_status = DTV_IRQ_INIT;
		dtv_status =1;
	}
	/*[Arima5908][39233][bozhi_lin] 20140606 end  */

	NM_KMSG("<isdbt> isdbt open, success\n");

   chip_open = 1;

	return 0;
}

static int isdbt_release(struct inode *inode, struct file *filp)
{
	ISDBT_OPEN_INFO_T *pdev = (ISDBT_OPEN_INFO_T*)(filp->private_data);

   chip_open = 0;

	NM_KMSG("<isdbt> isdbt release \n");

	/*[Arima5908][39233][bozhi_lin] fix after launch DTV apk can't turn off vreg_l26 & vreg_l28 20140606 begin*/
	isdbt_gpio_reset_down();
	isdbt_gpio_power_off();

	if( g_irq_status != DTV_IRQ_DEINIT )
	{
		g_irq_status = DTV_IRQ_DEINIT;
		isdbt_gpio_interrupt_unregister(pdev);
		dtv_status = 0;
	}
	/*[Arima5908][39233][bozhi_lin] 20140606 end  */
	
	g_bCatchIrq = 0;

	kfree(pdev->rwBuf);
	kfree((void*)pdev);   

	return 0;
}

ssize_t isdbt_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	int rv1 = 0;
	int rv2 = 0;
	int blk_cnt = count / 1024;
	int remain = count % 1024;
	ISDBT_OPEN_INFO_T *pdev = (ISDBT_OPEN_INFO_T *)(filp->private_data);

	if(blk_cnt) {
		rv1 = nmi326_spi_read(pdev->rwBuf, blk_cnt*1024);
		if (rv1 < 0) {
			NM_KMSG("isdbt_read() : nmi326_spi_read failed(rv1:%d)\n", rv1);
			return rv1;
		}
	}

	if(remain) {
		rv2 = nmi326_spi_read(&pdev->rwBuf[rv1], remain);
		if(rv2 < 0) {
			NM_KMSG("isdbt_read() : nmi326_spi_read failed(rv2:%d)\n", rv2);
			return rv1 + rv2;
		}
	}

	if(copy_to_user(buf, pdev->rwBuf, count) ) {
		return -1;
	}

	return rv1 + rv2;
}

static ssize_t isdbt_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	int rv = 0;
	ISDBT_OPEN_INFO_T* pdev = (ISDBT_OPEN_INFO_T*)(filp->private_data);

	/* move data from user area to kernel  area */
	if(copy_from_user(pdev->rwBuf, buf, count)) {
		return -1;
	}

	/* write data to SPI Controller */
	rv = nmi326_spi_write(pdev->rwBuf, count);
	if (rv < 0) {
		NM_KMSG("isdbt_write() : nmi326_spi_write failed(rv:%d)\n", rv);
		return rv;
	}

	return rv;
}

static long isdbt_ioctl(/*struct inode *inode, */struct file *filp, unsigned int cmd, unsigned long arg)
{
	long res = 1;
	ISDBT_OPEN_INFO_T* pdev = (ISDBT_OPEN_INFO_T*)(filp->private_data);

	int	err=0, size=0;

	if(_IOC_TYPE(cmd) != IOCTL_MAGIC) return -EINVAL;
	if(_IOC_NR(cmd) >= IOCTL_MAXNR) return -EINVAL;

	size = _IOC_SIZE(cmd);

	if(size) {
		if(_IOC_DIR(cmd) & _IOC_READ)
			err = access_ok(VERIFY_WRITE, (void *) arg, size);
		else if(_IOC_DIR(cmd) & _IOC_WRITE)
			err = access_ok(VERIFY_READ, (void *) arg, size);
		if(!err) {
			NM_KMSG("%s : Wrong argument! cmd(0x%08x) _IOC_NR(%d) _IOC_TYPE(0x%x) _IOC_SIZE(%d) _IOC_DIR(0x%x)\n",
			__FUNCTION__, cmd, _IOC_NR(cmd), _IOC_TYPE(cmd), _IOC_SIZE(cmd), _IOC_DIR(cmd));
			return err;
		}
	}

	switch(cmd)
	{
		case IOCTL_ISDBT_POWER_ON:
			NM_KMSG("ISDBT_POWER_ON enter..\n");
			/*[Arima5908][39233][bozhi_lin] fix after launch DTV apk can't turn off vreg_l26 & vreg_l28 20140606 begin*/
			#if 0
			isdbt_gpio_power_on();
			#endif
			/*[Arima5908][39233][bozhi_lin] 20140606 end  */
			break;

		case IOCTL_ISDBT_POWER_OFF:
			NM_KMSG("ISDBT_POWER_OFF enter..\n");
			/*[Arima5908][39233][bozhi_lin] fix after launch DTV apk can't turn off vreg_l26 & vreg_l28 20140606 begin*/
			#if 0
			isdbt_gpio_power_off();
			#endif
			/*[Arima5908][39233][bozhi_lin] 20140606 end  */
			break;

		case IOCTL_ISDBT_RST_DN:
			NM_KMSG("IOCTL_ISDBT_RST_DN enter..\n");
			/*[Arima5908][39233][bozhi_lin] fix after launch DTV apk can't turn off vreg_l26 & vreg_l28 20140606 begin*/
			#if 0
			isdbt_gpio_reset_down();
			#endif
			/*[Arima5908][39233][bozhi_lin] 20140606 end  */
			break;

		case IOCTL_ISDBT_RST_UP:
			NM_KMSG("IOCTL_ISDBT_RST_UP enter..\n");
			/*[Arima5908][39233][bozhi_lin] fix after launch DTV apk can't turn off vreg_l26 & vreg_l28 20140606 begin*/
			#if 0
			isdbt_gpio_reset_up();
			#endif
			/*[Arima5908][39233][bozhi_lin] 20140606 end  */
			break;

		case IOCTL_ISDBT_INTERRUPT_REGISTER:
		{
			unsigned long flags;
			spin_lock_irqsave(&pdev->isr_lock, flags);

			NM_KMSG("<isdbt> ioctl: interrupt register, (stat : %d)\n", g_irq_status);

			/*[Arima5908][39233][bozhi_lin] fix after launch DTV apk can't turn off vreg_l26 & vreg_l28 20140606 begin*/
			#if 0
			if( g_irq_status == DTV_IRQ_DEINIT && dtv_status==0)
			{
				g_bCatchIrq = 0;
				isdbt_gpio_interrupt_register(pdev);
				g_irq_status = DTV_IRQ_INIT;
				dtv_status =1;
			}
			#endif
			/*[Arima5908][39233][bozhi_lin] 20140606 end  */

			spin_unlock_irqrestore(&pdev->isr_lock, flags);

			break;
		}

		case IOCTL_ISDBT_INTERRUPT_UNREGISTER:
		{
			unsigned long flags;
			spin_lock_irqsave(&pdev->isr_lock, flags);

			NM_KMSG("<isdbt> ioctl: interrupt unregister, (stat : %d)\n", g_irq_status);

			/*[Arima5908][39233][bozhi_lin] fix after launch DTV apk can't turn off vreg_l26 & vreg_l28 20140606 begin*/
			#if 0
			if(dtv_status==1)
			{
				isdbt_gpio_interrupt_unregister(pdev);
				dtv_status =0;
			}
			#endif
			/*[Arima5908][39233][bozhi_lin] 20140606 end  */
			
			#if 0
			if( g_irq_status == DTV_IRQ_INIT )
			{
				g_bCatchIrq = 0;
				isdbt_gpio_interrupt_unregister();
				//g_irq_status = DTV_IRQ_DEINIT;
			}
			#endif	
			
			spin_unlock_irqrestore(&pdev->isr_lock, flags);

			break;
		}

		case IOCTL_ISDBT_INTERRUPT_ENABLE:
		{
			unsigned long flags;
			
			printk(KERN_EMERG "IOCTL_ISDBT_INTERRUPT_ENABLE ,  g_irq_status=%d \n",g_irq_status);   
			
			spin_lock_irqsave(&pdev->isr_lock, flags);

			if(g_irq_status == DTV_IRQ_INIT)
			{
				isdbt_gpio_interrupt_enable();
				g_irq_status = DTV_IRQ_SET;
			}
			spin_unlock_irqrestore(&pdev->isr_lock, flags);
			break;
		}

		case IOCTL_ISDBT_INTERRUPT_DISABLE:
		{
			unsigned long flags;
			
			printk(KERN_EMERG "IOCTL_ISDBT_INTERRUPT_DISABLE , g_irq_status=%d\n",g_irq_status); 
			spin_lock_irqsave(&pdev->isr_lock, flags);

			if(g_irq_status == DTV_IRQ_SET)
			{
				g_bCatchIrq = 0;
				isdbt_gpio_interrupt_disable();
				//pdev->irq_status = DTV_IRQ_INIT;
				g_irq_status = DTV_IRQ_INIT;
			}
			spin_unlock_irqrestore(&pdev->isr_lock, flags);
			break;
		}

		case IOCTL_ISDBT_INTERRUPT_DONE:
		{
			unsigned long flags;

			spin_lock_irqsave(&pdev->isr_lock,flags);

			if(g_irq_status == DTV_IRQ_INIT)
			{
				isdbt_gpio_interrupt_enable();
				g_irq_status = DTV_IRQ_SET;
			}
			spin_unlock_irqrestore(&pdev->isr_lock,flags);
			break;
		}

		default:
			res = 1;
			break;

	}

	return res;
}

static const struct file_operations isdbt_fops = {
	.owner			= THIS_MODULE,
	.open			= isdbt_open,
	.release		= isdbt_release,
	.read			= isdbt_read,
	.write			= isdbt_write,
	.unlocked_ioctl	= isdbt_ioctl,
	.poll			= isdbt_poll,
};

//[Arima Edison] creat for update lcm moulde id++
static ssize_t dtv_chip_id_read(struct device *dev,
				  struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;	
	unsigned long chip_id = 0;

	if(gpio_get_value(nmi->rf_switch_gpio)==1)
	{
		chip_id = nmi326_spi_read_chip_id();
	}
	else
	{
		isdbt_gpio_power_on();
		/*[Arima5908][43878][bozhi_lin] fix can't read dtv chip id 20140912 begin*/
		isdbt_gpio_reset_up();
		/*[Arima5908][43878][bozhi_lin] 20140912 end*/
		msleep(30);	
		chip_id = nmi326_spi_read_chip_id();
		/*[Arima5908][43878][bozhi_lin] fix can't read dtv chip id 20140912 begin*/
		isdbt_gpio_reset_down();
		/*[Arima5908][43878][bozhi_lin] 20140912 end*/
		isdbt_gpio_power_off();
	}	
	
	ret = snprintf(buf, 30, "%lu\n",chip_id);	

	return ret;
}

//[Arima Edison] creat for update lcm moulde id--
static DEVICE_ATTR(chip_id, 0644, dtv_chip_id_read, NULL);

static struct attribute *dtv_attrs[] = {
	&dev_attr_chip_id.attr,
	NULL,
};
static struct attribute_group dtv_attr_group = {
	.attrs = dtv_attrs,
};

static int __devinit isdbt_probe(struct platform_device *pdev)
{
	int ret,rc=0;
	struct device *isdbt_dev;

   	printk(KERN_EMERG "%s \n",__func__);

	NM_KMSG("<isdbt> isdbt_probe, MAJOR = %d\n", ISDBT_DEV_MAJOR);

	// 1. register character device
	ret = register_chrdev(ISDBT_DEV_MAJOR, ISDBT_DEV_NAME, &isdbt_fops);
	if(ret < 0)
		NM_KMSG("<isdbt> register_chrdev(ISDBT_DEV) failed\n");

	// 2. class create
	isdbt_class = class_create(THIS_MODULE, ISDBT_DEV_NAME);
	if(IS_ERR(isdbt_class)) {
		unregister_chrdev(ISDBT_DEV_MAJOR, ISDBT_DEV_NAME);
		class_destroy(isdbt_class);
		NM_KMSG("<isdbt> class create failed\n");

		return -EFAULT;
	}

	// 3. device create
	isdbt_dev = device_create(isdbt_class, NULL, MKDEV(ISDBT_DEV_MAJOR, ISDBT_DEV_MINOR), NULL, ISDBT_DEV_NAME);

	rc = sysfs_create_group(&isdbt_dev->kobj, &dtv_attr_group);

	if(IS_ERR(isdbt_dev)) {
		unregister_chrdev(ISDBT_DEV_MAJOR, ISDBT_DEV_NAME);
		class_destroy(isdbt_class);
		NM_KMSG("<isdbt> device create failed\n");

		return -EFAULT;
	}


	if (pdev->dev.of_node) {	
		nmi = devm_kzalloc(&pdev->dev,
			sizeof(*nmi),
			GFP_KERNEL);
		if (!nmi) {
			dev_err(&pdev->dev, "Failed to allocate memory\n");
			return -ENOMEM;
		}
		
		ret = isdbt_gpio_init(pdev);
		if (ret)
		{	
			NM_KMSG("<isdbt> isdbt_gpio_init, failed \n");
			return -ENODEV;
		}
	}
	
	//nmi326_spi_init();

	NM_KMSG("<isdbt> nmi326_spi_init, is called\n");

	return 0;
}

static int __devexit isdbt_remove(struct platform_device *pdev)
{

	//sysfs_remove_group(&mfd->fbi->dev->kobj, &msm_fb_attr_group);

	regulator_put(nmi->vdd);
	regulator_put(nmi->vdd_io);
	regulator_put(nmi->rf_switch);
	
	gpio_free(nmi->reset_gpio);
	gpio_free(nmi->rf_switch_gpio);
	gpio_free(nmi->irq_gpio);
	gpio_free(nmi->dtv_select_gpio);
	return 0;
}

/*
static int isdbt_suspend(struct platform_device *pdev, pm_message_t mesg)
{
	return 0;
}

static int isdbt_resume(struct platform_device *pdev)
{
	return 0;
}
*/

/*********************************************************/
static struct of_device_id nmi325_isdbt_of_match[] = {
	{ .compatible = "nmi,nmi325_isdbt", },
	{ },
};
MODULE_DEVICE_TABLE(of, nmi325_isdbt_of_match);

static struct platform_driver isdbt_driver = {
	.probe		= isdbt_probe,
	.remove		= __devexit_p(isdbt_remove),
	.driver		= {
		.name	= "isdbt",
		.owner	= THIS_MODULE,
		//.pm	= &gpio_keys_pm_ops,
		.of_match_table = nmi325_isdbt_of_match,
	}
};


/********************************************************/


/*
static struct platform_driver isdbt_driver = {
	.probe	= isdbt_probe,
	.remove	= isdbt_probe,
	.suspend	= isdbt_suspend,
	.resume	= isdbt_resume,
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "isdbt"
	},
};
*/

static int __init isdbt_init(void)
{
	int result;
	/*[Arima5908][37779][bozhi_lin] turn on-off one time to avoid vreg_l28 default turn on when boot-up by S1boot 20140512 begin*/
	static struct regulator *ldo28;
	/*[Arima5908][37779][bozhi_lin] 20140512 end  */

	printk(KERN_EMERG "%s \n",__func__); 

	NM_KMSG("<isdbt> isdbt_init \n");
	
	/*[Arima5908][37779][bozhi_lin] turn on-off one time to avoid vreg_l28 default turn on when boot-up by S1boot 20140512 begin*/
	ldo28 = regulator_get(NULL, "8226_l28");
	if (IS_ERR(ldo28)) {
		pr_err("[B]%s(%d): failed to get 8226_l28\n", __func__, __LINE__);
	}
	
	result = regulator_enable(ldo28);
	if (result) {
		pr_err("[B]%s(%d): Unable to enable the regulator 8226_l28\n", __func__, __LINE__);
	}
	
	result = regulator_disable(ldo28);
	if (result) {
		pr_err("[B]%s(%d): Unable to disable the regulator 8226_l28\n", __func__, __LINE__);
	}
	/*[Arima5908][37779][bozhi_lin] 20140512 end  */

	result = gpio_request_one(GPIO_DTV_SELECT, GPIOF_IN, "nmi_dtv_select");	
	if (result)
		goto fail;

	if(gpio_get_value(GPIO_DTV_SELECT))
	{
		pr_info("[B]%s(%d): GPIO_%d is HIGH(%d), DTV enable\n", __func__, __LINE__, GPIO_DTV_SELECT, gpio_get_value(GPIO_DTV_SELECT));
		gpio_free(GPIO_DTV_SELECT);
	}
	else
	{
		goto fail;
	} 

	result = platform_driver_register(&isdbt_driver);
	if(result)
		return result;

	NM_KMSG("<isdbt> isdbt_init, success \n");

	return 0;

fail:
	NM_KMSG("<isdbt> isdbt_init, failed \n");
	return -ENXIO;
}

static void __exit isdbt_exit(void)
{
	NM_KMSG("<isdbt> isdbt_exit \n");

	unregister_chrdev(ISDBT_DEV_MAJOR, "isdbt");
	device_destroy(isdbt_class, MKDEV(ISDBT_DEV_MAJOR, ISDBT_DEV_MINOR));
	class_destroy(isdbt_class);

	platform_driver_unregister(&isdbt_driver);

	//nmi326_spi_exit();
}

module_init(isdbt_init);
module_exit(isdbt_exit);

MODULE_LICENSE("Dual BSD/GPL");

