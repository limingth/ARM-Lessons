
obj-m := int.o

KDIR = /home/limingth/tiny210/src/linux-2.6.35.7

modules:
	make -C $(KDIR) SUBDIRS=$(PWD)  $@

clean:
	-rm *.o *.ko

i install:
	../ftp.sh . int.ko

