#!/bin/sh
set -eu
DIR=$HOME
sudo apt update
sudo apt upgrade -y
sudo apt install wget sudo git build-essential jq xz-utils bison flex bc universal-ctags vim file qemu-system-riscv netcat-openbsd -y
cd $DIR
[ -d linux-riscv32 ] || git clone https://github.com/ip-arch/linux-riscv32.git

# RISC-Vクロスコンパイラの構築
mkdir -p ~/x-tools
wget -nc https://ip-arch.jp/riscv-linux/x-tools-riscv32-debian.tar.bz2
tar xf x-tools-riscv32-debian.tar.bz2 -C ~
grep -q 'x-tools/riscv32-unknown-linux-gnu/bin' "$HOME/.bashrc" || echo "PATH=\$PATH:\$HOME/x-tools/riscv32-unknown-linux-gnu/bin" >> ~/.bashrc
. ~/.bashrc

cd $DIR/linux-riscv32/linux; \
wget -nc https://github.com/ip-arch/linux-riscv32/releases/download/1st_minor_1_change_rcS/rootfs.img 
cd $DIR/linux-riscv32; make linux/Image

# 確認時には、下記コマンドでQEMUを動かす
# make qemu-run
