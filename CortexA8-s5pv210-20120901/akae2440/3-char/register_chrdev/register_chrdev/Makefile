ifneq ($(KERNELRELEASE),)
obj-m:=register_dev.o register_dev_new.o
else


KERNELSRC :=/usr/src/linux-headers-2.6.32-40-generic


modules:
	gcc app.c -o app
	make -C $(KERNELSRC) SUBDIRS=$(PWD) $@

clean:
	rm -f *.o *.ko *.mod.c *~ app

endif
