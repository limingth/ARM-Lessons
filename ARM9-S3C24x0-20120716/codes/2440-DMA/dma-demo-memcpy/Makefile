all:
	armasm start.s
	armcc -c main.c
	armlink start.o main.o \
-first start.o -ro-base 0x30000000 -o demo.axf
	fromelf -bin demo.axf -o demo.bin
	fromelf -c -d -s demo.axf -o demo.txt
	dir demo.bin