// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include <stdio.h>
#include <stdlib.h>

int array_bss[1000];
int array_data[1000]={100};

int main(int argc, char *argv[]) {
	int var_auto, var_auto_ini=1;
	static int var_static, var_static_ini=1;
	int *p1, *p2;
	p1=(int*)malloc(100);
	p2=(int*)malloc(10000000);
	printf("main@%010p\narray_bss@%010p\narray_data@%010p\n"
			"argc@%010p\nargv@%010p\n"
			"var_auto@%010p\nvar_auto_ini@%010p\n"
			"var_static@%010p\nvar_static_ini@%010p\n"
			"p1=%010p\np2=%010p\n",
			main, array_bss, array_data, &argc, &argv,
			&var_auto, &var_auto_ini,
			&var_static, &var_static_ini,
			p1,p2);
	return 0;
}
