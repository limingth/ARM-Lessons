	AREA demo, CODE, READONLY

	import c_entry
	ENTRY

	mov sp, #0x1000

	b c_entry

	import irq_handler
	export asm_irq_handler
asm_irq_handler
	mov sp, #0x800	
	
	; save lr to stack
	stmfd sp!, {r0-r12, lr}
	
	bl irq_handler

	; restore lr from stack
	ldmfd sp!, {r0-r12, lr}

	subs pc, lr, #4

	END
