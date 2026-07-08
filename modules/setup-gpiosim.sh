#!/bin/bash
set -e
# configfs/debugfs
mountpoint -q /sys/kernel/config || \
    mount -t configfs none /sys/kernel/config
mountpoint -q /sys/kernel/debug || \
    mount -t debugfs none /sys/kernel/debug
# 古い設定削除
if [ -d /sys/kernel/config/gpio-sim/my_sw ]; then
    echo 0 > /sys/kernel/config/gpio-sim/my_sw/live 2>/dev/null || true
    rmdir /sys/kernel/config/gpio-sim/my_sw/bank0/line0 2>/dev/null || true
    rmdir /sys/kernel/config/gpio-sim/my_sw/bank0 2>/dev/null || true
    rmdir /sys/kernel/config/gpio-sim/my_sw 2>/dev/null || true
fi
if [ -d /sys/kernel/config/gpio-sim/my_led ]; then
    echo 0 > /sys/kernel/config/gpio-sim/my_led/live 2>/dev/null || true
    rmdir /sys/kernel/config/gpio-sim/my_led/bank0/line0 2>/dev/null || true
    rmdir /sys/kernel/config/gpio-sim/my_led/bank0 2>/dev/null || true
    rmdir /sys/kernel/config/gpio-sim/my_led 2>/dev/null || true
fi
mkdir -p /sys/kernel/config/gpio-sim/my_sw/bank0
echo 1 > /sys/kernel/config/gpio-sim/my_sw/bank0/num_lines
mkdir -p /sys/kernel/config/gpio-sim/my_sw/bank0/line0
echo SW0 > /sys/kernel/config/gpio-sim/my_sw/bank0/line0/name
echo 1 > /sys/kernel/config/gpio-sim/my_sw/live

mkdir -p /sys/kernel/config/gpio-sim/my_led/bank0
echo 1 > /sys/kernel/config/gpio-sim/my_led/bank0/num_lines
mkdir -p /sys/kernel/config/gpio-sim/my_led/bank0/line0
echo LED0 > /sys/kernel/config/gpio-sim/my_led/bank0/line0/name
echo 1 > /sys/kernel/config/gpio-sim/my_led/live
sleep 0.2

GPIO=$(grep -oE 'gpio-[0-9]+' /sys/kernel/debug/gpio | head -1 | cut -d- -f2)
echo "GPIO=$GPIO"
echo "Ready."
