#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <qlibc/qlibc.h>
#include <qlibc/utilities/qfile.h>

#define FILE_NAME_MAX    256
#define BUILD_PATH_MAX    64
#define CMD_MAX         1024

static char script_path[FILE_NAME_MAX + BUILD_PATH_MAX];
static char    exe_path[FILE_NAME_MAX + BUILD_PATH_MAX];
static char    comp_cmd[CMD_MAX];
static char     run_cmd[CMD_MAX];


int
main(int argc, char *argv[])
{
	if (argc == 1) {
		printf("usage: %s script.c\n", argv[0]);
		return EXIT_SUCCESS;
	}
	size_t bytes;
	char *script = qfile_load(argv[1], &bytes);
	// Skip shebang line if exists
	char *script_start = script;
	if (script[0] == '#' && script[1] == '!') {
		script_start = strchr(script, '\n');
		script_start++;
	}
	// Script source and exe file names
	char *script_name = qfile_get_name(argv[1]);
	char *build_dir = "/tmp";
	sprintf(script_path, "%s/%s", build_dir, script_name);
	sprintf(exe_path, "%s/%s", build_dir, script_name);
	exe_path[strlen(exe_path) - 2] = '\0';
	// Save script to file after processing, compile, and run
	qfile_save(script_path, script_start, bytes - (script_start - script), false);
	sprintf(comp_cmd, "gcc %s -o %s", script_path, exe_path);
	system(comp_cmd);
	sprintf(run_cmd, "%s", exe_path);
	system(run_cmd);
	return EXIT_SUCCESS;
}
