	AREA demo, CODE, READONLY

	import c_entry
	ENTRY

	mov sp, #0x1000

	b c_entry

	END
