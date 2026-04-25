#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
rm -f logger health payment epoll_server
cc  logger.c -o logger
cc  health.c -o health
cc  payment.c -o payment
cc  epoll_server_ans.c -o epoll_server
