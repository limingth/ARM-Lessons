
obj-m := dm9000.o


KDIR = /home/limingth/tiny210/src/linux-2.6.35.7

modules:
	make -C $(KDIR)	SUBDIRS=$(PWD)  $@
	ls -l dm9000.ko

clean:
	-rm *.o *.ko

i install:
	ls -l dm9000.ko
	../ftp.sh . dm9000.ko

