	AREA demo, CODE, READONLY
	
	import mymain
	ENTRY
	
	;mov sp, #0x1000
	;sp from bootloader go

	; push all regs to stack
	stmfd sp!, {r0-r12, lr}
	
	bl mymain
	
	ldmfd sp!, {r0-r12, lr}
	mov pc, lr
	
	END