#!/usr/bin/env runcc
#incdirs $HOME/cc/sys/include
// #libdirs $HOME/cc/sys/lib
// #libs qlibc
// #include <qlibc/qlibc.h>
#include <stdio.h>

main(int argc, char *argv[])
{
	if (argc == 1) { printf("usage: %s <file>\n", argv[0]); return 0; }
}
