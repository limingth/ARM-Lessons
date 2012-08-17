#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/interrupt.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/etherdevice.h>
#include <linux/irq.h>
#include "cs8900.h"

MODULE_AUTHOR("Asmcos");
MODULE_DESCRIPTION("cs8900a driver");
MODULE_LICENSE("GPL");

struct net_device *cs8900dev;
spinlock_t mylock;

static inline u16 cs8900_read(struct net_device *dev, u16 regno)
{
	outw(regno, dev->base_addr + PP_Address);
	return (inw(dev->base_addr + PP_Data)); 
}

static inline void cs8900_write(struct net_device *dev, u16 regno, u16 value)
{
	outw(regno, dev->base_addr + PP_Address);
	outw(value, dev->base_addr + PP_Data);
}

static inline void cs8900_set(struct net_device *dev, u16 regno, u16 value)
{
	cs8900_write(dev, regno, cs8900_read(dev, regno) | value);
}

static inline void cs8900_frame_read(struct net_device *dev, struct sk_buff *skb, u16 length)
{
	insw(dev->base_addr, skb_put(skb, length), (length+1)/2);
}

static inline void cs8900_frame_write(struct net_device *dev, struct sk_buff *skb)
{
	outsw(dev->base_addr, skb->data, (skb->len+1)/2);
}

void cs8900_recv(struct net_device *dev)
{
	volatile u16 status, length;
	struct sk_buff *skb;

	status = cs8900_read(dev, PP_RxStatus);
	length = cs8900_read(dev, PP_RxLength);

	if(!(status & RxOK))
	{
		return;
	}

	skb = dev_alloc_skb(length + 2);
	if(!skb)
	{
		//printk("alloc skb error!\n");
		return;
	}	
	skb_reserve(skb, 2);
	skb->dev = dev;

	cs8900_frame_read(dev, skb, length);
	skb->protocol = eth_type_trans(skb, dev);
	netif_rx(skb);

	return;
}

irqreturn_t cs8900_interrupt(int irq, void *dev_id)
{
	volatile u16 status;
	struct net_device *dev = (struct net_device *)dev_id;
	
	while((status = cs8900_read(dev, PP_ISQ)) != 0)
	{
		switch(RegNum(status))
		{
		case RxEvent:
			cs8900_recv(dev); //接收数据
			break;
		case TxEvent:
			netif_wake_queue(dev);
			break;
		case BufEvent: //既有接收中断事件，又有发送中断事件
			if(((RegContent(status)) & TxUnderrun))
			{
				netif_wake_queue(dev);
			}
			break;
		}
	}

	return IRQ_HANDLED;
}


int cs8900_xmit(struct sk_buff *skb, struct net_device *dev)
{
	volatile u16 status;

	spin_lock_irq(&mylock);
	netif_stop_queue(dev);
	
	cs8900_write(dev, PP_TxCMD, TxStart(After5));
	cs8900_write(dev, PP_TxLength, skb->len);

	status = cs8900_read(dev, PP_BusST);
	if((status & TxBidErr))
	{
		spin_unlock_irq(&mylock);
		printk("invalid data length!\n");
		return 0;
	}
	if(!(status & Rdy4TxNOW))
	{
		spin_unlock_irq(&mylock);
		printk("tx buffer is not free!\n");
		return 0;
	}

	cs8900_frame_write(dev, skb);
	spin_unlock_irq(&mylock);

	dev_kfree_skb(skb);
	
	return 0;
}

int cs8900_open(struct net_device *dev)
{
	//init registers
	cs8900_set(dev, PP_RxCTL, RxOKA | IndividualA | BroadcastA);
	cs8900_set(dev, PP_RxCFG, RxOKiE | BufferCRC | CRCerroriE | RuntiE | ExtradataiE);
	cs8900_set(dev, PP_TxCFG, TxOKiE | Out_of_windowiE | JabberiE);
	cs8900_set(dev, PP_BufCFG, Rdy4TxiE | RxMissiE | TxUnderruniE | TxColOvfiE | MissOvfloiE);
	cs8900_set(dev, PP_LineCTL, SerRxON | SerTxON);
	cs8900_set(dev, PP_BusCTL, EnableRQ);

	//上升沿触发中断
	set_irq_type(dev->irq, IRQF_TRIGGER_RISING);
	
	//register an interrupt handler
	if(request_irq(dev->irq, cs8900_interrupt, 0, "cs8900a", (void *)dev))
	{
		printk("error register cs8900a irq at %d\n", dev->irq);
		return -1;
	}
	
	//open Tx queue in tcpip stack
	netif_start_queue(dev);
	
	return 0;
}

int cs8900_stop(struct net_device *dev)
{
	//clear registers
	cs8900_write(dev, PP_RxCTL, 0);
	cs8900_write(dev, PP_RxCFG, 0);
	cs8900_write(dev, PP_TxCFG, 0);
	cs8900_write(dev, PP_BufCFG, 0);
	cs8900_write(dev, PP_LineCTL, 0);
	cs8900_write(dev, PP_BusCTL, 0);

	//free_irq
	free_irq(dev->irq, (void *)dev);

	//stop Tx queue
	netif_stop_queue(dev);

	return 0;
}

int cs8900_init(struct net_device *dev)
{
	int i;

	dev->open = cs8900_open;
	dev->stop = cs8900_stop;
	dev->hard_start_xmit = cs8900_xmit;

	ether_setup(dev);

	//协议栈用的MAC地址
	dev->dev_addr[0] = 0x08;	
	dev->dev_addr[1] = 0x00;
	dev->dev_addr[2] = 0x3e;
	dev->dev_addr[3] = 0x26;
	dev->dev_addr[4] = 0x0a;
	dev->dev_addr[5] = 0x5b;

	dev->if_port = IF_PORT_10BASET; //RJ45
	dev->irq = IRQ_EINT9;
	dev->base_addr = (unsigned long)ioremap(0x19000000, 16);
	
	//检测是否是cs8900a网卡芯片
	if(cs8900_read(dev, PP_ProductID) != 0x630E)
	{
		printk("this is not a CS8900A chip!\n");
		return -1;
	}
	
	//cs8900网卡芯片端使用的是INTRQ0管脚连接了s3c2440芯片的EINT9中断管脚
	cs8900_write(dev, PP_IntNum, 0);

	for(i = 0; i < 6; i += 2)
	{
		cs8900_write(dev, PP_IA + i, 
					(dev->dev_addr[i+1]<<8) | dev->dev_addr[i]);
	}

	return 0;
}

int __init akae_init(void)
{
	struct net_device *dev = alloc_etherdev(0);
	if(!dev)
	{
		printk("alloc net_device error!\n");
		return -1;
	}
	cs8900dev = dev;
	dev->init = cs8900_init;
	strcpy(dev->name, "eth%d");
	spin_lock_init(&mylock);

	return (register_netdev(dev));
}

void __exit akae_exit(void)
{
	unregister_netdev(cs8900dev);
	free_netdev(cs8900dev);
	return;
}

module_init(akae_init);
module_exit(akae_exit);
