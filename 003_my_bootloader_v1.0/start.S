
#define S3C2440_MPLL_200MHZ ((0x5c<<12)|(0x01<<4)|(0x02))
#define S3C2440_MPLL_400MHZ ((0x5c<<12)|(0x01<<4)|(0x01))
#define MEM_CTL_BASE 0x48000000


.text
.global _start
_start:

/*1.close watch dog*/
	ldr r0, =0x53000000
	mov r1, #0
	str r1, [r0]

/*2.set clock */
	ldr r0, =0x4c000014
	mov r1, #0x05
	str r1, [r0]

	mrc p15, 0, r1, c1, c0, 0
	orr r1, r1, #0xc0000000
	mcr p15, 0, r1, c1, c0, 0

	ldr r0, =0x4c000004
	ldr r1, =S3C2440_MPLL_400MHZ
	str r1, [r0]

	/*using ICACHE*/
	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, #(1<<12)
	mcr p15, 0, r0, c1, c0, 0


/*3.init SDRAM*/
	ldr r0, =MEM_CTL_BASE
	adr r1, sdram_config /*now addr of sdram_config*/

	add r3, r0, #13*4
1:
	ldr r2, [r1], #4
	str r2, [r0], #4
	cmp r0, r3
	bne 1b

/*4.redirction: copy bootloader code from flash to its link addr*/
	ldr sp, =0x34000000

	bl nand_init

	mov r0, #0
	ldr r1, =_start
	ldr r2, =__bss_start
	sub r2, r2, r1
	
	

	bl copy_code_to_sdram
	bl clear_bss
	



/*5.run main_func*/
	ldr lr, =halt
	ldr pc, =main

halt:
	b halt

sdram_config:
	.long 0x22011110
	.long 0x00000700
	.long 0x00000700
	.long 0x00000700
	.long 0x00000700
	.long 0x00000700
	.long 0x00000700
	.long 0x00018005
	.long 0x00018005
	.long 0x008C04F4
	.long 0x000000B1
	.long 0x00000030
	.long 0x00000030

