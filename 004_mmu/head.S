.equ	MEM_CTL_BASE,	0x48000000
.equ	SDRAM_BASE,	0x30000000

.text
.gloabl _start
_start:
	bl disable_watch_dog
	bl memsetup
	bl copy_steppingstone_to_sdram
	ldr pc, =on_sdram

on_sdram:
	ldr sp, =0x34000000
	bl main

halt_loop:
	b halt_loop

disable_watch_dog:
	mov r1, #0x53000000
	mov r2, #0x0
	str r2, [r1]
	mov pc, lr

copy_steppingstone_to_sdram:
	mov r1, #0
	ldr r2, =SDRAM_BASE
	mov r3, #4*1024

1:
	ldr r4, [r1], #4
	str r4, [r2], #4
	cmp r1, r3
	bme 1b
	mov pc, lr

memsetup:
