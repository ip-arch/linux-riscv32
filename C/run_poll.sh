#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
taskset -c 0 chrt -b 0 ./logger 2>&1 > logger.log &
sleep 1
taskset -c 0 chrt -f 50 ./payment 2>&1 > payment.log &
#taskset -c 0  ./payment &
chrt -d --sched-runtime 5000000 \
	--sched-deadline 100000000 \
	--sched-period 100000000 0 ./health  2>&1 > health.log &
sleep 1; nice -n 15 ./epoll_server 2>&1 epoll_server.log &

