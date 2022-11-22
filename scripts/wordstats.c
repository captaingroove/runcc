#!/usr/bin/env runcc
#libs qlibc pthread
#include <stdio.h>
#include <qlibc/qlibc.h>

main(int argc, char *argv[])
{
	if (argc == 1) { printf("usage: %s <file>\n", argv[0]); return 0; }
	qhashtbl_t *wordmap = qhashtbl(0, 0);
}
