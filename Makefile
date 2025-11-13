include Makefile.h


linux/linux-v$(LINUX): 
	(cd linux; \
	git clone --depth 1 --branch v$(LINUX) https://github.com/torvalds/linux.git linux-v$(LINUX); \
	cp linux-v$(LINUX).config linux-v$(LINUX)/.config; \
	cd linux-v$(LINUX); \
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) oldconfig all \
	)

oldconfig: linux/linux-v$(LINUX)
	(cd linux; \
	cd linux-v$(LINUX); \
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) distclean mrproper \
	cp ../linux-v$(LINUX).config .config; \
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) oldconfig prepare modules_prepare scripts \
	)

linux/Image: linux/linux-v$(LINUX)
	(cd linux; \
	cd linux-v$(LINUX); \
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) \
	)

tags: linux/linux-v$(LINUX)
	(cd linux/linux-v$(LINUX); \
	patch -p1 < ../tags_sh.patch; \
	make ARCH=$(ARCH) tags \
	)

qemu-run:
	(cd linux; \
	echo $(QEMUDIR) ; \
	qemu-system-riscv32 -machine virt -m 256M   -kernel Image   -nographic   -append "console=ttyS0 rdinit=/init root=/dev/ram0"   -initrd rootfs.img \
	)

veryclean:
	rm -fr linux/linux-v$(LINUX)
	make clean
clean:
	(cd modules; make clean)
	(cd apps/C; make clean)

################################
# suffix rules
################################
.SUFFIXES:  .man

%.man:
	man -C $(MANDB) $*
	# MANPATH=$(PKGWD)/share/man man -C $(MANDB) $*
