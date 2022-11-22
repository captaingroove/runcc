#!/usr/bin/env runcc
#libs qlibc pthread
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <qlibc/qlibc.h>
#include <qlibc/utilities/qfile.h>


main(int argc, char *argv[])
{
	if (argc == 1) { printf("usage: %s <file>\n", argv[0]); return 0; }
	size_t fsize;
	char *file = qfile_load(argv[1], &fsize);
	qlist_t *words = qstrtokenizer(file, " \t\r\n");
	qhashtbl_t *wordmap = qhashtbl(0, 0);
	char *w; int wc = 0;
	while ((w = qlist_popfirst(words, NULL))) {
		if (strlen(w)) {
			qhashtbl_putint(wordmap, w, qhashtbl_getint(wordmap, w) + 1);
			wc++;
		}
	}
	printf("Total word count: %d\nDifferent words: %ld\nWordmap:\n", wc, qhashtbl_size(wordmap));
	qhashtbl_obj_t o = {0};
	while (qhashtbl_getnext(wordmap, &o, false)) {
		printf("  '%s': %s\n", o.name, (char*)o.data);
	}
}
