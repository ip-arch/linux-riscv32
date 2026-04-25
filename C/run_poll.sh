#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
taskset -c 0 chrt -b 0 ./logger &
sleep 1
taskset -c 0 chrt -f 50 ./payment &
chrt -d --sched-runtime 5000000 \
	--sched-deadline 100000000 \
	--sched-period 100000000 0 ./health  &
sleep 1; nice -n 15 ./epoll_server &

