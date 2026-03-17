// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
int sum_array(int *a, int n) {
	int sum = 0;
	for (int i = 0; i < n; i++)
		sum += a[i];
	return sum;
	}
