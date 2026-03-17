include Makefile.h

FILE_TO_CHECK := ip-arch.patch

linux/linux-v$(LINUX): 
	(cd linux; \
	git clone --depth 1 --branch v$(LINUX) https://github.com/torvalds/linux.git linux-v$(LINUX); \
	cp linux-v$(LINUX).config linux-v$(LINUX)/.config; \
	cd linux-v$(LINUX); \
	if [ ! -f $(FILE_TO_CHECK) ]; then \
		echo "$(FILE_TO_CHECK) が見つからないため、パッチを適用します。"; \
		touch $(FILE_TO_CHECK); \
		patch -p1 < ../tags_sh.patch; \
	else \
		echo "$(FILE_TO_CHECK) は既に存在します。スキップします。"; \
	fi; \
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -j $(NPROC) oldconfig all \
	)

oldconfig: linux/linux-v$(LINUX)
	(cd linux; \
	cd linux-v$(LINUX); \
	cp ../linux-v$(LINUX).config .config; \
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -j $(NPROC) oldconfig prepare modules_prepare scripts \
	)

menuconfig: linux/linux-v$(LINUX)
	(cd linux; \
	cd linux-v$(LINUX); \
	cp ../linux-v$(LINUX).config .config; \
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -j $(NPROC) oldconfig menuconfig \
	)

linux/Image: linux/linux-v$(LINUX)
	(cd linux; \
	cd linux-v$(LINUX); \
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -j $(NPROC); ) 
	cp linux/linux-v$(LINUX)/arch/$(ARCH)/boot/Image linux 

linux-clean: linux/linux-v$(LINUX)
	(cd linux; \
	cd linux-v$(LINUX); \
	cp ../linux-v$(LINUX).config .config; \
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -j $(NPROC) distclean mrproper \
	)


tags: linux/linux-v$(LINUX)
	(cd linux/linux-v$(LINUX); \
	make ARCH=$(ARCH) -j $(NPROC) tags \
	)

qemu-run:
	(cd linux; \
	echo $(QEMUDIR) ; \
	mkdir -p "$(QEMUSHARE)" ; \
	LD_LIBRARY_PATH=$(EXTRA_LIB_PATH)$${LD_LIBRARY_PATH:+:$$LD_LIBRARY_PATH}; \
	qemu-system-riscv32 -machine virt -m 256M   -kernel Image \
      	-nographic   -append "console=ttyS0 rdinit=/init root=/dev/ram0 " \
       	-cpu rv32,sscofpmf=true -initrd rootfs.img \
	-drive file=fat:rw:"$(QEMUSHARE)",format=raw,id=shared \
	-device virtio-net-device,netdev=net0 \
	-netdev user,id=net0,hostfwd=tcp::$(SSH_PORT)-:22,hostfwd=tcp::$(HTTP_PORT)-:80 \
	-device virtio-serial-device \
	-chardev socket,host=0.0.0.0,port=5555,server=on,nowait,id=uart1 \
	-device virtconsole,chardev=uart1 \
	-serial mon:stdio \
#	 -virtfs local,path=. ,security_model=none,mount_tag=winhome \
#	 -monitor unix:/tmp/qemu-monitor.sock,server,nowait \
	)

exboard.dtbo: dts/exboard.dtso
	cpp -nostdinc -undef -D__DTS__ -x assembler-with-cpp \
	-I./linux/linux-v$(LINUX)/scripts/dtc/include-prefixes  dts/exboard.dtso | \
	linux/linux-v$(LINUX)/scripts/dtc/dtc -O dtb -o exboard.dtbo


veryclean:
	rm -fr linux/linux-v$(LINUX)
	make -j $(NPROC) clean
	rm -f exboard.dtbo
	rm -f linux/Image
clean:
	(cd modules; make  clean)
	(cd C; make  clean)

################################
# suffix rules
################################
.SUFFIXES:  .man

%.man:
	man -C $(MANDB) $*
	# MANPATH=$(PKGWD)/share/man man -C $(MANDB) $*
