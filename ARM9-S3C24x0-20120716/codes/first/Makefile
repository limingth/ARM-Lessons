all:	
	armcc test.c -o test.axf
	armlink test.o -o test.axf
	fromelf -bin test.axf -o test.bin
	fromelf -c test.axf -o test.txt
	dir *.bin
	
clean:
	del *.axf
	del *.o 
	del *.bin