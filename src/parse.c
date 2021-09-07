#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "parse.h"

extern int vbose;

/* 
Parses a line from our class schedule document and returns the time caught between n and m.

Example:
	parse_class_time("1100_1245_MW_CLASS", 0, 4) -> 1100

Arguments:
	char *linebuf = String of line from document
	int n = Starting offset to begin from
	int m = Ending offset to end

Returns:
	Int of time of class between buffers or EOF on error

Assumptions:
	Ints n and m are nonnegative, within range of linebuf, and m is greater than n
*/
int parse_class_time(char *linebuf, int n, int m) {
	if(linebuf == EOF || linebuf == 0 || n < 0 || m < n) return EOF;

	if(vbose > 1) { fprintf(stdout, "Parsing line '%s' for time between indices '%d' and '%d'\n", linebuf, n, m); }

	char *buf = (char *)malloc(m - n);
	if(buf == NULL) return EOF;

	buf = strncpy(buf, linebuf + n, m);
	int value = atoi(buf);
	
	if(vbose > 1) { fprintf(stdout, "Class time '%d' found\n", value); }
	
	free(buf);
	return value;
}

/*
Parses existence of a given subdirectory.

Example:
	parse_class_dir("class/CLASS/") -> 0 (if subdir exists), 1 (if subdir was created), EOF on error

Arguments:
	char *cdir = A subdirectory of a class

Returns:
	Status of the class's subdirectory (see example above for details)

Assumptions:
	cdir is a valid string that points to a valid subdir subdir

*/
int parse_class_dir(char *cdir) {
	int status = 0;

	struct stat *sp = (struct stat *)malloc(sizeof(struct stat));
	if(sp == NULL) {
		fprintf(stderr, "Error in attempt to allocate memory for subdir data\n");
		exit(1);
	}

	if(vbose > 1) { fprintf(stdout, "Parsing subdir '%s' for class name\n", cdir); }
	
	if(stat(cdir, sp) < 0) { /* If subdir does not exist */
		if(!mkdir(cdir, S_IRWXU)) { /* If subdir has been created */
			if(vbose > 1) { fprintf(stdout, "Subdir '%s' has been created\n", cdir); }

			status = 1;
		}
		else {
			fprintf(stderr, "Error creating directory '%s'\n", cdir);
			status = EOF;
		}
	}
	else if(S_ISDIR(sp->st_mode)) { /* If subdir exists as a dir */
		if(vbose > 1) { fprintf(stdout, "Subdir '%s' has been found\n", cdir); }

		status = 0;
	}

	free(sp);
	return status;
}
