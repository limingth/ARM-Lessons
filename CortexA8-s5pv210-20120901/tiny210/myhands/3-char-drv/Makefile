
obj-m := char_drv.o 

KDIR := /home/akaedu/teacher_li/CortexA8-s5pv210-20120901/tiny210/build-env/src/linux-2.6.35.7

all:
	make modules -C $(KDIR) SUBDIRS=$(PWD)

clean:
	-rm *.o
	-rm *.ko
	-rm led.mod.*
	-rm modules.*
	-rm Module.*

