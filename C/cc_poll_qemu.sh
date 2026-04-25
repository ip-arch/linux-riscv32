#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
make  logger.qemu ADDFLAGS=$1
make  health.qemu ADDFLAGS=$1
make  payment.qemu ADDFLAGS=$1
make  epoll_server_ans.qemu ADDFLAGS=$1
