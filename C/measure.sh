#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
# measure.sh

duration=10  # 測定時間（秒）

# 全プロセスのCPU使用時間を取得
for pid in $(pgrep cpu_hog); do
    name=$(cat /proc/$pid/cmdline | tr '\0' ' ')
    stat=$(cat /proc/$pid/stat)
    utime=$(echo $stat | awk '{print $14}')
    stime=$(echo $stat | awk '{print $15}')
    echo "PID=$pid name=$name utime=$utime stime=$stime"
done
