

ifneq ($(KERNELRELEASE),)
obj-m :=driver.o
else

KERNELSRC:=/home/akaedu/linux-2.6.27


modules :
	make -C $(KERNELSRC) SUBDIRS=$(PWD) $@

clean:
	rm -f *.o *.ko *.mod.c *~

endif
