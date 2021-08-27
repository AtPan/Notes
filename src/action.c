#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "action.h"

char *concat_int(char *, int);
int parse_class_time(char *, int, int);
char *parse_class_name(char *, int);
int parse_class_dir(char *);

/* Connect to the name, class, and edit flags setup in main.c */
extern char *name, *class;

/* Makes calls to both find_name and find_class functions */
void find_name_and_class() {
	find_name();
	find_class();
	open_file();
}

/* Find name of the file using default dd-mm.txt format */
void find_name() {
	time_t td = time(NULL);
	struct tm *local = localtime(&td);
	int date = local->tm_mday; /* Date of the month */
	int month = local->tm_mon + 1; /* Month of the year */

	char *filename = (char *)malloc(10); /* dd-mm.txt = 9 chars + ending 0 */
	
	if(filename >= 0) { /* If pointer exists, fill it with the naming convention */
		char *fn = filename;

		if(date < 10) {
			*fn++ = '0';
		}
		fn = concat_int(fn, date);
		
		*fn++ = '-';
		
		if(month < 10) {
			*fn++ = '0';
		}
		fn = concat_int(fn, month);
		
		strcat(fn, ".txt");
		
		*(fn + 9) = '\0';

		name = filename; /* Save the file name */
	}
	else {
		printf("ERROR: Cannot create name for file\n");
	}

	#ifdef SKIP
	open_file();
	#endif
}

// TODO: Make loop work without a million variables, try to reuse some, add comments explaining reuse of variables
//		 Add better error-handling. Maybe make a default dir in case of class-finding errors?
void find_class() {
	enum Day { SUN = 1, MON = 77, TUE = 84, WED = 87, THR = 82, FRI = 70, SAT = 2 };
	const int EDAY[] = { SUN, MON, TUE, WED, THR, FRI, SAT };

	/* Create our auto variables */
	FILE *ctime;
	char *linebuf, *c_buf;
	time_t td = time(NULL);
	struct tm *local = localtime(&td);
	int hour = local->tm_hour, min = local->tm_min, day = local->tm_wday;

	/* Error handle for our buffers */
	if((ctime = fopen(TIME_LOC, "r")) == NULL) {
		fprintf(stderr, "Error opening time-schedule file '%s'\n", TIME_LOC);
		exit(1);
	}

	if((linebuf = (char *)malloc(MAX_LINE)) == NULL) {
		fprintf(stderr, "Error allocating '%d' bytes for line buffer\n", MAX_LINE);
		exit(1);
	}

	if((c_buf = (char *)malloc(4)) == NULL) { //All class times will be in xxxx (i.e. 0145) format so always 4 characters
		fprintf(stderr, "Error allocating '4' bytes for time schedule buffer\n");
	}
	
	/* Find out what times the classes are and figure out if we fall in any of said time intervals */
	while((linebuf = fgets(linebuf, MAX_LINE - 1, ctime)) != 0 && linebuf != EOF) {
		if(*linebuf == '#') continue; //Allow for in-line comments

		int start_time = parse_class_time(linebuf, 0, 4), end_time = parse_class_time(linebuf, 5, 9);

//		if(hour > (start_time / 100) && hour < (end_time / 100) && min > (start_time % 100) && min < (end_time % 100)) {
		char c;
		for(int i = 0; (c = *(linebuf + 10 + i)) != '_'; i++) {
			if(EDAY[day] == c) {
				for(; *(linebuf + 10 + i) != '_'; i++);
				char *cname = parse_class_name(linebuf, 11 + i);
				parse_class_dir(cname);
				goto cfound;
			}
		}
//	}

cfound:
;
	#ifdef SKIP
	open_file();
	#endif
}
}

void open_file() {

}

/* Concats a given string s with ASCII characters of int n
   Example: concat_int("Hello", 15) -> "Hello15" */
char *concat_int(char *s, int n) {
	char *str = s;
	int len = 0;
	for(int i = n; i > 0; i /= 10) len++;

	for(int i = 0; i < len; i++) {
		*(str+(len - i - 1)) = (n % 10) + '0';
		n /= 10;
	}

	return s + len;
}

/* 
Parses a line from our class schedule document and returns the time caught between n and m.

Example: parse_class_time("1100_1245_MW_CLASS", 0, 4) -> 1100

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

char *parse_class_name(char *linebuf, int n) {
	int j;
	for(j = 0; *(linebuf + n + j) != '\0'; j++);
	char *cname = (char *)malloc(j + 7);

	cname = strcpy(cname, "class/");
	cname = strncpy(cname + 6, linebuf + n, j - 1) - 6;
	cname = strcat(cname, "/\0");

	return cname;	
}

int parse_class_dir(char *cdir) {
	int ret = 0;

	struct stat *sp = (struct stat *)malloc(sizeof(struct stat));
	if(sp == NULL) {
		fprintf(stderr, "Error in attempting to secure class directory");
		exit(1);
	}
			
	if(stat(cdir, sp) < 0) {
		if(!(ret = mkdir(cdir, S_IRWXU))) {
			class = cdir;
		}
		else {
			fprintf(stderr, "Error creating directory '%s'\n", cdir);
			exit(1);
		}
	}
	else if(S_ISDIR(sp->st_mode)) {
		class = cdir;
	}
	else if(stat("class/etc/", sp) < 0) {
		mkdir("class/etc/", S_IRWXU);
		class = "class/etc/";
	}
	else {
		class = "class/etc/";
	}

	free(sp);
	return ret;
}
