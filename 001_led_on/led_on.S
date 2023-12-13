/*
 *���� LED:
*/

.text
.global _start

_start:

	ldr r1, =0x56000050
	ldr r0, =0x100
	str r0, [r1]

/*
 *配置GPF4为输出引脚：地址0x56000050写入0x100
*/

	ldr r1, =0x56000054
	ldr r0, =0
	str r0, [r1]
//death loop
halt:
	b halt