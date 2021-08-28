#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "parse.h"

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

	char *buf = (char *)malloc(m - n);
	if(buf == NULL) return EOF;

	buf = strncpy(buf, linebuf + n, m);
	int ret = atoi(buf);
	free(buf);
	return ret;
}

/*
Parses the name of a class from the class data line (at a given offset n).

Example:
	parse_class_name("1100_1245_MW_CLASS", 13) -> "class/CLASS\0"

Arguments:
	char *linebuf = String of line from time document
	int n = Offset to start from

Returns:
	String of location to the given class's subdirectory or EOF on error

Assumptions:
	n is a nonnegative integer and within range of array linebuf
*/
char *parse_class_name(char *linebuf, int n) {
	int nl = (strchr(linebuf, '\n') == NULL) ? 1 : 0, clen = strchr(linebuf + n, '\0') - (linebuf + n);
	char *cname = (char *)malloc(clen + 7);

	cname = strcpy(cname, "class/");
	cname = strncpy(cname + 6, linebuf + n, clen - (nl ? 2 : 1)) - 6;
	cname = strcat(cname, "/\0");

	return cname;	
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
			
	if(stat(cdir, sp) < 0) {
		if(!mkdir(cdir, S_IRWXU)) {
			status = 1;
		}
		else {
			fprintf(stderr, "Error creating directory '%s'\n", cdir);
			status = EOF;
		}
	}
	else if(S_ISDIR(sp->st_mode)) {
		status = 0;
	}

	free(sp);
	return status;
}
