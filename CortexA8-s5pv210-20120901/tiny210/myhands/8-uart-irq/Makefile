
obj-m := uart_drv.o 

KDIR := /home/akaedu/teacher_li/CortexA8-s5pv210-20120901/tiny210/build-env/src/linux-2.6.35.7

all:
	make modules -C $(KDIR) SUBDIRS=$(PWD)
	arm-linux-gcc test.c -o test

clean:
	-rm *.o
	-rm *.ko
	-rm char_drv.mod.*
	-rm modules.*
	-rm Module.*

