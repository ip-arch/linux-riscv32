#
# OS with RISC-V 32bit and Linux exercise environment
#   2025-10-14
#   By Naohiko Shimizu
#
#

LINUX ?= 6.12
# ポートが衝突する場合は実行時に変更可能にする
# 例: make qemu-run HTTP_PORT=18080
HTTP_PORT ?= 9080
SSH_PORT  ?= 2222
# ユーザーが指定したパス、または自動検知したパス
EXTRA_LIB_PATH ?= $(shell [ -d /usr/local/lib64 ] && echo "/usr/local/lib64")

SYS=$(shell uname -o 2>/dev/null || uname -s)
ifeq ($(SYS),Cygwin)
    OS=Cygwin
else ifeq ($(SYS),Darwin)
    OS=macOS
else
    OS=$(shell lsb_release -si 2>/dev/null || echo Linux)
endif

# 並列数
ifeq ($(OS),macOS)
    NPROC=$(shell sysctl -n hw.logicalcpu)
else
    NPROC=$(shell nproc)
endif

PKGWD=$(shell pwd)
MANDB=$(PKGWD)/share/man_db.conf
MANPATH=$(PKGWD)/share/man
ARCH=riscv
ARCHx=riscv32
ARCHSUB=$(ARCHx)-unknown-linux-gnu
CROSS_COMPILE=$(ARCHSUB)-
TOOLDIR=$(HOME)/x-tools/$(ARCHSUB)/bin
SYSROOT=$(TOOLDIR)/../$(ARCHSUB)/$(ARCHSUB)/sysroot
QEMUDIR=$(shell dirname '$(shell which qemu-system-$(ARCHx))')
PATH:=$(QEMUDIR):$(TOOLDIR):/usr/bin:/bin:/sbin

# OS別の共有ディレクトリ
ifeq ($(OS),Cygwin)
    QEMUSHARE=$(HOMEDRIVE)$(HOMEPATH)/Desktop/QemuDir
else
    QEMUSHARE=$(HOME)/QemuDir
endif

