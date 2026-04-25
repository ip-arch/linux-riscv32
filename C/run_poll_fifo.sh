#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
rm -f /dev/shm/health_payment_log_sem
rm -f /dev/shm/sem.health_payment_log_sem
chrt -b 0 ./logger &
chrt -f 50 ./payment &
chrt -f 90 ./health  &
sleep 1; nice -n 15 ./epoll_server &

