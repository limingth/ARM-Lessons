	AREA demo, CODE
	
	ENTRY
	
	;mov r0, pc	
	;mov r0, pc
	;ldr pc, =swi_handler
	
	; i++ 		r0 (i)
	; sum += i		r1 (sum)
	; i <= 100		r2 (100)
	mov r0, #1
	mov r1, #0
	mov r2, #1000	

	; 1 + 2 + 3 + .... + r2  
	swi 0x0
	
	;b swi_handler
	
	mov r10, r1
  	b    .
  	
  	
swi_handler 
loop
	; sum = sum + i
	add r1, r1, r0
	
	; i++
	add r0, r0, #1
	
	; i <= 101
	cmp r0, r2

	ble loop	
	
	mov pc, lr
	; end
	;b     . 
	
	END