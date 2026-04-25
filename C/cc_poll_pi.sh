#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
cc -DUSE_PI_SEM  logger.c -o logger
cc -DUSE_PI_SEM  health.c -o health
cc -DUSE_PI_SEM  payment.c -o payment
cc -DUSE_PI_SEM  epoll_server_ans.c -o epoll_server
