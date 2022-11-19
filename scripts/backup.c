//#!/home/jorg/cc/runcc/build/runcc
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
	char subdir[128] = "";
	if (argc == 2)
		sprintf(subdir, "%s/", argv[1]);

	char cmd[512];
	sprintf(cmd, "sudo rsync -av --delete "
		"/home/jorg/%s /media/jorg/Elements/asymptotic/%s", subdir, subdir);
	system(cmd);
}
