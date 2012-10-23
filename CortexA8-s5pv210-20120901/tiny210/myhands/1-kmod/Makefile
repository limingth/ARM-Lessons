
# this Makefile is for kernel module

obj-m := hello2.o

KDIR := /home/akaedu/teacher_li/CortexA8-s5pv210-20120901/tiny210/build-env/src/linux-2.6.35.7

all:
	make -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	-rm *.ko
	-rm *.o
	-rm *.order
	-rm *.symvers
	-rm hello.mod.*

