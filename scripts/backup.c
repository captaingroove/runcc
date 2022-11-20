#!/usr/bin/env runcc
#include <stdio.h>
#include <stdlib.h>

void
main(int argc, char *argv[])
{
	if (argc == 1) {
		printf("usage: %s targetdir [subdir]\n", argv[0]); return;
	}
	char *targetdir = argv[1];
	char *subdir = "";
	if (argc == 3) {
		subdir = argv[2];
	}
	char *homedir = getenv("HOME");

	char cmd[512];
	sprintf(cmd,
		"sudo rsync -av --delete %s/%s %s/%s",
		homedir, subdir, targetdir, subdir);
	printf("%s\n", cmd);
	// system(cmd);
}
