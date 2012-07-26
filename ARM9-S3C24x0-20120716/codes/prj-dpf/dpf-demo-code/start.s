	
	AREA demo, CODE, READONLY
	
	import mymain
	ENTRY
	
	;mov sp, #0x1000
	
	; irq
	; change to irq mode, set sp
	mov r0, #0xD2	
	msr CPSR_cxsf, r0
	mov sp, #0x33000000
	
	; change to svc mode
	mov r0, #0xD3	
	msr CPSR_cxsf, r0
	mov sp, #0x34000000
	
	b mymain
	
	END