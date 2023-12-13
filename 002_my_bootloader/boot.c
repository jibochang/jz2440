#include "setup.h"

extern void uart0_init(void);
extern void nand_read(unsigned int addr, unsigned char *buf,unsigned int len);
extern void puts(char *str);
extern void puthex(unsigned int val);

static struct tag *params;

void setup_start_tag(void)
{
	params = (struct tag *)0x30000100;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}


void setup_memory_tags(void)
{
	params->hdr.tag = ATAG_MEM;
	params->hdr.size = tag_size (tag_mem32);

	params->u.mem.start = 0x30000000;
	params->u.mem.size = 0x4000000;

	params = tag_next (params);
}

int strlen(char *p)
{
	int i = 0;
	while(p[i]){
		i++;
	}
	return i;
}

void strcpy(char *dest, char *src)
{
	while((*dest++ = *src++) != '\0');
}

void setup_commandline_tag(char *cmdline)
{
	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
		(sizeof (struct tag_header) + strlen (cmdline) + 1 + 3) >> 2;

	strcpy (params->u.cmdline.cmdline, cmdline);

	params = tag_next (params);
}


void setup_end_tag(void)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}


int main(void)
{
	void (*theKernel)(int zero,int arch,unsigned int params);

	/*0. init serial*/
	uart0_init();


	puts("copy kernel from nand!\n");
	
	/*1.write kernel to sdram from nand flash*/
	nand_read(0x60000+64,(unsigned char *)0x30008000,0x200000);
	/*2.set param*/
	puts("set boot params\n");
	setup_start_tag();
	setup_memory_tags();
	setup_commandline_tag("noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0");
	setup_end_tag();
	/*3.jump to execute*/

	puts("booting kernel\n");
	
	theKernel = (void (*)(int,int,unsigned int))0x30008000;

	theKernel(0,362,0x30000100);

	puts("error!\n\r");

	return -1;
	
}
