all:
	armasm start.s
	armcc -c test.c -o test.o
	armlink -first start.o start.o test.o -o test.axf
	fromelf -bin test.axf -o test.bin
	fromelf -c test.axf -o test.txt
	dir *.bin
	
clean:
	del *.axf
	del *.o 
	del *.bin