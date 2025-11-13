#
# OS with RISC-V 32bit and Linux exercise environment
#   2025-10-14
#   By Naohiko Shimizu
#
#

LINUX ?= 6.12

SYS=$(shell uname -o)
ifeq ($(SYS),Cygwin)
	OS=Cygwin
else
OS=$(shell lsb_release -si)
endif

PKGWD=$(shell pwd)
MANDB=$(PKGWD)/share/man_db.conf
MANPATH=$(PKGWD)/share/man
ARCH=riscv
ARCHx=riscv32
ARCHSUB=$(ARCHx)-unknown-linux-gnu
CROSS_COMPILE=$(ARCHSUB)-
TOOLDIR=$(PKGWD)/x-tools/$(ARCHSUB)/bin
SYSROOT=$(TOOLDIR)/../$(ARCHSUB)/$(ARCHSUB)/sysroot
QEMUDIR=$(shell dirname '$(shell which qemu-system-$(ARCHx))')
PATH:=$(QEMUDIR):$(TOOLDIR):/usr/bin:/bin:/sbin


