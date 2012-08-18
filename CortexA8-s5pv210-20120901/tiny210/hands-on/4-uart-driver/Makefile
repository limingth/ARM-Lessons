
obj-m := uart_drv.o


KDIR = /home/limingth/tiny210/src/linux-2.6.35.7

modules:
	make -C $(KDIR)	SUBDIRS=$(PWD)  $@

clean:
	-rm *.o *.ko

a app:
	make uart_app

i install:
	./ftp.sh . uart_drv.ko

i2:
	arm-linux-gcc uart_app.c -o uart_app
	./ftp.sh . uart_app
