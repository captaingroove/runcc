#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <qlibc/qlibc.h>
#include <qlibc/utilities/qfile.h>

/// We can make safe assumptions on the following string lengths,
/// as this is either not no user input or these are OS limits
#define FILE_NAME_MAX (256)
#define BUILD_PATH_MAX (64)
#define OUT_PATH_MAX (FILE_NAME_MAX + BUILD_PATH_MAX)
#define CMD_MAX (1024)

static char build_dir[OUT_PATH_MAX];
static char ccode_path[OUT_PATH_MAX];
static char exe_path[OUT_PATH_MAX];
size_t script_size = 0;
char *script_ptr = NULL, *ccode_start = NULL;
/// FIXME need to find a different build_dir if we want to cash the executables
char *build_base_dir = "/tmp/runcc";
char *comp_warnings = "-Wno-implicit-int";


char *
find_ccode_start(char *script_ptr, size_t script_size)
{
	char *ccode_start = script_ptr;
	/// Skip shebang line if exists
	if (script_ptr[0] == '#' && script_ptr[1] == '!') {
		ccode_start = strchr(script_ptr, '\n');
		ccode_start++;
	}
	return ccode_start;
}


bool
create_build_dir(const char *build_base_dir, char *build_dir)
{
	/// FIXME use a unique temporary file name instead of user name if user name is not available
	char *user_name = getenv("USER");
	sprintf(build_dir, "%s/%s", build_base_dir, user_name);
	if (!qfile_exist(build_dir) && !qfile_mkdir(build_dir, S_IRWXU, true)) {
		return false;
	}
	return true;
}


bool
get_paths(const char *script_path, const char *build_dir, char *ccode_path, char *exe_path)
{
	char *script_name = qfile_get_name(script_path);
	sprintf(ccode_path, "%s/%s", build_dir, script_name);
	sprintf(exe_path, "%s/%s", build_dir, script_name);
	char *dotpos = strrchr(script_name, '.');
	if (!dotpos) {
		fprintf(stderr, "script file name has no extension like '.c'\n");
		return false;
	}
	exe_path[strlen(exe_path) - strlen(dotpos)] = '\0';
	return true;
}


void
write_ccode_compile_and_run(
	char *ccode_start,
	size_t ccode_size,
	char *ccode_path,
	char *exe_path,
	char *comp_warnings)
{
	char comp_cmd[CMD_MAX];
	qfile_save(ccode_path, ccode_start, ccode_size, false);
	sprintf(comp_cmd, "gcc %s %s -o %s", comp_warnings, ccode_path, exe_path);
	system(comp_cmd);
	system(exe_path);
}


int
main(int argc, char *argv[])
{
	if (argc == 1) {
		printf("usage: %s <script.c>\n", argv[0]);
		return EXIT_SUCCESS;
	}
	if (!create_build_dir(build_base_dir, build_dir)) {
		fprintf(stderr, "failed to create temporary build directory %s\n", build_dir);
		return EXIT_FAILURE;
	}
	script_ptr = qfile_load(argv[1], &script_size);
	ccode_start = find_ccode_start(script_ptr, script_size);
	get_paths(argv[1], build_dir, ccode_path, exe_path);
	write_ccode_compile_and_run(
		ccode_start,
		script_size - (ccode_start - script_ptr),
		ccode_path,
		exe_path,
		comp_warnings);
	return EXIT_SUCCESS;
}
