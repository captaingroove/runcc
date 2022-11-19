#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
	if (argc == 1) {
		printf("usage: %s script.c\n", argv[0]);
		return EXIT_SUCCESS;
	}
	// char comp_cmd[512], run_cmd[512], script[512], exec[512];
	char comp_cmd[512], run_cmd[512];
	char *exec = strrchr(argv[1], '/');
	if (!exec) {
		exec = argv[1];
	}
	sprintf(comp_cmd, "gcc %s -o /tmp/%s", argv[1], exec);
	system(comp_cmd);
	sprintf(run_cmd, "/tmp/%s", exec);
	system(run_cmd);
	return EXIT_SUCCESS;
}
