// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
int f(int x) {
	int y = x + 1;
	return y;
}

int main(void) {
	int a = 10;
	int b = f(a);
	return b;
}

