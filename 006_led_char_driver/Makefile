KDIR := /home/kernel/linux-2.6.22.6/

PWD := $(shell pwd)

obj-m := led.o

build : kernel_modules

kernel_modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	rm -fr .*.cmd *.o *.mod.c *.ko .tmp_versions modules.* Module.*

