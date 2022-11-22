#!/usr/bin/env runcc
#libs qlibc pthread
#include <stdio.h>
#include <qlibc/qlibc.h>
#include <qlibc/utilities/qfile.h>


main(int argc, char *argv[])
{
	if (argc == 1) { printf("usage: %s <file>\n", argv[0]); return 0; }
	size_t fsize;
	char *ifile = qfile_load(argv[1], &fsize);
	qlist_t *words = qstrtokenizer(ifile, " \t\r\n");
	qhashtbl_t *wordmap = qhashtbl(0, 0);
	char *w; int wc = 0;
	while ((w = qlist_popfirst(words, NULL))) {
		if (strlen(w)) {
			printf("%s\n", w);
			wc++;
		}
	}
	printf("word count: %d\n", wc);
}
