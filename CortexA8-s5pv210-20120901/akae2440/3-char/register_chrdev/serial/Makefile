######################################################
#*Function:driver makefile
#*Author:asmcos@gmail.com
#*Date:2005-08-24
#*$Id:Makefile,v 1.3 2006/05/17 12:37:19 asmcos Exp $
######################################################

ifneq ($(KERNELRELEASE),)
#kbuild part of makefile
obj-m:=serial.o
else


KERNELSRC :=/home/bryan/akaedu/工作相关/18期驱动实验相关/秦老师资料/kernel-2.6.27-android_ok

modules:
	make -C $(KERNELSRC) SUBDIRS=$(PWD) $@
	arm-linux-gcc app.c -o app -g -Wall

clean:
	rm -f app *.o *.ko *.mod.c *~

endif
