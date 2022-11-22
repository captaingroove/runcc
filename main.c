#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <qlibc/qlibc.h>
#include <qlibc/utilities/qfile.h>

/// We can safely make assumptions on the following string lengths,
/// as this is either no user input or these are OS limits
#define FILE_NAME_MAX (256)
#define BUILD_PATH_MAX (64)
#define OUT_PATH_MAX (FILE_NAME_MAX + BUILD_PATH_MAX)
#define CMD_MAX (1024)
#define PREPROC_INCDIRS ("#incdirs")
#define PREPROC_LIBS    ("#libs")
#define ENV_INCDIRS     ("RUNCC_INCDIRS")
#define ENV_LIBDIRS     ("RUNCC_LIBDIRS")
#define ENV_BUILDDIR    ("RUNCC_BUILDDIR")
#define ENV_CFLAGS      ("RUNCC_CFLAGS")
#define ENV_CC          ("RUNCC_CC")

static char build_dir[OUT_PATH_MAX]       = {0};
static char ccode_path[OUT_PATH_MAX]      = {0};
static char exe_path[OUT_PATH_MAX]        = {0};
static char include_dirs[OUT_PATH_MAX]    = {0};
static char include_headers[OUT_PATH_MAX] = {0};
static char linker_dirs[OUT_PATH_MAX]     = {0};
static char linker_path[OUT_PATH_MAX]     = {0};
static char linker_libs[OUT_PATH_MAX]     = {0};
static char comp_cflags[CMD_MAX]          = {0};
static char comp_exec[FILE_NAME_MAX]      = {0};
size_t script_size = 0;
char *script_ptr = NULL, *ccode_start = NULL;
char *build_base_dir = "/tmp/runcc";


bool get_params_from_list(char *params, const char *liststr, const char *param_prefix);


char *
find_ccode_start(char *script_ptr, size_t script_size)
{
	char *ccode_start = script_ptr;
	/// Skip shebang line if exists
	if (script_ptr[0] == '#' && script_ptr[1] == '!') {
		ccode_start = strchr(script_ptr, '\n');
		ccode_start++;
	}
	while (*ccode_start == '#') {
		char *space = strchr(ccode_start, ' ');
		if (space && strncmp(ccode_start, PREPROC_INCDIRS, strlen(PREPROC_INCDIRS)) == 0) {
			printf("found %s\n", PREPROC_INCDIRS);
			ccode_start = strchr(ccode_start, '\n');
			ccode_start++;
		}
		if (space && strncmp(ccode_start, PREPROC_LIBS, strlen(PREPROC_LIBS)) == 0) {
			printf("found %s\n", PREPROC_LIBS);
			char *libstr_ptr = ccode_start + strlen(PREPROC_LIBS) + 1;
			ccode_start = strchr(ccode_start, '\n');
			int libstr_len = ccode_start - libstr_ptr;
			char libstr[CMD_MAX] = {0};
			strncpy(libstr, libstr_ptr, libstr_len);
			get_params_from_list(linker_libs, libstr, " -l");
			ccode_start++;
		}
	}
	if (!ccode_start || (ccode_start - script_ptr) == script_size) {
		return NULL;
	}
	return ccode_start;
}


bool
create_build_dir(char *build_dir, const char *build_base_dir)
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
get_paths(char *ccode_path, char *exe_path, const char *script_path, const char *build_dir)
{
	char *script_name = qfile_get_name(script_path);
	sprintf(ccode_path, "%s/%s", build_dir, script_name);
	sprintf(exe_path, "%s/%s", build_dir, script_name);
	char *dotpos = strrchr(script_name, '.');
	if (!dotpos) {
		fprintf(stderr, "script file name has no extension like '.c'\n");
		return false;
	}
	/// Eliminate .c extension for the executable file name
	exe_path[strlen(exe_path) - strlen(dotpos)] = '\0';
	return true;
}


bool
get_params_from_list(char *params, const char *liststr, const char *param_prefix)
{
	int liststr_len = strlen(liststr);
	const char *liststr_end = liststr + liststr_len;
	char *space = NULL;
	/// FIXME handle escaped spaces
	while ((space = strchr(liststr, ' '))) {
		liststr_len = space - liststr + 1;
		snprintf(params, liststr_len + strlen(param_prefix), "%s%s", param_prefix, liststr);
		params += liststr_len + strlen(param_prefix) - 1;
		liststr += liststr_len;
	}
	int param_len = liststr_end - liststr + 1 + strlen(param_prefix);
	snprintf(params, param_len, "%s%s", param_prefix, liststr);
	return true;
}


bool
get_params_from_env(char *params, const char *envvar, const char *param_prefix)
{
	char *envval = getenv(envvar);
	if (!envval) return false;
	return get_params_from_list(params, envval, param_prefix);
}


void
write_ccode_compile_and_run(
	int argc, char *argv[],
	char *ccode_start,
	size_t ccode_size,
	char *ccode_path,
	char *exe_path,
	char *include_dirs)
{
	char comp_cmd[CMD_MAX];
	char run_cmd[CMD_MAX];
	char *cflags = "";
	if (getenv(ENV_CFLAGS)) {
		cflags = getenv(ENV_CFLAGS);
	}
	char *comp_exec = "cc";
	if (getenv(ENV_CC)) {
		comp_exec = getenv(ENV_CC);
	}
	qfile_save(ccode_path, ccode_start, ccode_size, false);
	sprintf(comp_cmd, "%s -o %s %s %s %s %s %s",
		comp_exec,
		exe_path,
		cflags,
		include_dirs,
		linker_dirs,
		ccode_path,
		linker_libs);
	printf("%s\n", comp_cmd);
	system(comp_cmd);
	sprintf(run_cmd, "LD_LIBRARY_PATH=%s %s", linker_path, exe_path);
	int run_cmd_pos = strlen(run_cmd);
	for (int ac = 2; ac < argc; ac++) {
		sprintf(&run_cmd[run_cmd_pos], " %s", argv[ac]);
	}
	printf("%s\n", run_cmd);
	system(run_cmd);
}


int
main(int argc, char *argv[])
{
	if (argc == 1) {
		printf("usage: %s script.c\n", argv[0]);
		return EXIT_SUCCESS;
	}
	if (getenv(ENV_BUILDDIR)) {
		build_base_dir = getenv(ENV_BUILDDIR);
	}
	if (!create_build_dir(build_dir, build_base_dir)) {
		fprintf(stderr, "failed to create temporary build directory %s\n", build_dir);
		return EXIT_FAILURE;
	}
	script_ptr = qfile_load(argv[1], &script_size);
	ccode_start = find_ccode_start(script_ptr, script_size);
	if (!ccode_start) {
		fprintf(stderr, "no C code found in script\n");
		return EXIT_FAILURE;
	}
	get_paths(ccode_path, exe_path, argv[1], build_dir);
	get_params_from_env(include_dirs, ENV_INCDIRS, " -I");
	get_params_from_env(linker_dirs, ENV_LIBDIRS, " -L");
	get_params_from_env(linker_path, ENV_LIBDIRS, ":");
	// get_params_from_env(linker_libs, ENV_LIBS, " -l");
	write_ccode_compile_and_run(
		argc, argv,
		ccode_start,
		script_size - (ccode_start - script_ptr),
		ccode_path,
		exe_path,
		include_dirs);
	return EXIT_SUCCESS;
}
