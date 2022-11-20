#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <qlibc/qlibc.h>
#include <qlibc/utilities/qfile.h>

#define FILE_NAME_MAX    256
#define BUILD_PATH_MAX    64
#define CMD_MAX         1024

static char  ccode_path[FILE_NAME_MAX + BUILD_PATH_MAX];
static char    exe_path[FILE_NAME_MAX + BUILD_PATH_MAX];
static char    comp_cmd[CMD_MAX];
static char     run_cmd[CMD_MAX];
size_t script_size = 0;
char *script_ptr = NULL, *ccode_start = NULL;


char *
find_ccode_start(char *script_ptr, size_t script_size)
{
	char *ccode_start = script_ptr;
	// Skip shebang line if exists
	if (script_ptr[0] == '#' && script_ptr[1] == '!') {
		ccode_start = strchr(script_ptr, '\n');
		ccode_start++;
	}
	return ccode_start;
}


void
get_paths(char *script_path, char *ccode_path, char *exe_path)
{
	char *script_name = qfile_get_name(script_path);
	char *build_dir = "/tmp";
	sprintf(ccode_path, "%s/%s", build_dir, script_name);
	sprintf(exe_path, "%s/%s", build_dir, script_name);
	exe_path[strlen(exe_path) - 2] = '\0';
}


void
write_ccode_compile_and_run(
	char *ccode_start,
	size_t ccode_size,
	char *ccode_path,
	char *exe_path)
{
	qfile_save(ccode_path, ccode_start, ccode_size, false);
	sprintf(comp_cmd, "gcc %s -o %s", ccode_path, exe_path);
	system(comp_cmd);
	sprintf(run_cmd, "%s", exe_path);
	system(run_cmd);
}


int
main(int argc, char *argv[])
{
	if (argc == 1) {
		printf("usage: %s <script.c>\n", argv[0]);
		return EXIT_SUCCESS;
	}
	script_ptr = qfile_load(argv[1], &script_size);
	ccode_start = find_ccode_start(script_ptr, script_size);
	get_paths(argv[1], ccode_path, exe_path);
	write_ccode_compile_and_run(
		ccode_start,
		script_size - (ccode_start - script_ptr),
		ccode_path,
		exe_path);
	return EXIT_SUCCESS;
}
