include ../Makefile.h
PKGWD=$(shell pwd)/..
BUILD=$(PKGWD)/linux/linux-v$(LINUX)
M=`pwd`
CC=$(TOOLDIR)/$(CROSS_COMPILE)gcc

clean:
	rm -rf ./.[a-z]* *.ko *.mod.c *.o *.mod modules.order Module.symvers *~ *.exe

qemu:
	(cd ../; make qemu-run)

################################
# suffix rules
################################
.SUFFIXES:  .c .exe .qemu

.c.exe:
	$(CC) -g -O2 $< -o $*.exe $(ADDFLAGS)

.exe.qemu:
	mkdir -p "$(QEMUSHARE)"
	cp $< $(QEMUSHARE)
