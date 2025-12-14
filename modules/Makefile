include ../Makefile.h
PKGWD=$(shell pwd)/..
BUILD=$(PKGWD)/linux/linux-v$(LINUX)
M=`pwd`
CC=$(TOOLDIR)/$(CROSS_COMPILE)gcc
EXTRA_CFLAGS="-UCONFIG_STACKPROTECTOR -fno-stack-protector"

clean:
	rm -rf ./.[a-z]* *.ko *.mod.c *.o *.mod modules.order Module.symvers *~ *.exe

qemu:
	(cd ..; make qemu-run)

################################
# suffix rules
################################
.SUFFIXES:  .c .ko .exe .qemu

.c.ko:
	ARCH=$(ARCH) CROSS_COMPILE=$(TOOLDIR)/$(CROSS_COMPILE) make -C $(BUILD) M=$(M) V=1 NAME=$* EXTRA_CFLAGS=$(EXTRA_CFLAGS) modules

.c.exe:
	$(CC) -g -O2 $< -o $*.exe $(ADDFLAGS)

.ko.qemu:
	cp $< $(QEMUSHARE)
