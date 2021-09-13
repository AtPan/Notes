#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "action.h"
#include "parse.h"

char *concat_int(char *, int);

/* Connect to the name, class, and edit flags setup in main.c */
extern char *name, *class, edit;
extern int vbose, skip;

/*
Finds the name and class subdir for the note file.
For more detailed information, see:
	* find_name
	* find_class
	* open_file
*/
void find_name_and_class() {
	find_name();
	find_class();
	open_file();
}

/*
Find name of the file using default dd-mm.txt format.
Saves file name to the global variable name.
*/
void find_name() {
	if(vbose) { fprintf(stdout, "Finding name for file...\n"); }

	time_t td = time(NULL);
	struct tm *local = localtime(&td);
	int date = local->tm_mday; /* Date of the month */
	int month = local->tm_mon + 1; /* Month of the year */

	char *filename = (char *)malloc(10); /* dd-mm.txt = 9 chars + ending 0 */
	filename = init_str(filename, 10);
	
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

		fn = strcat(fn, ".txt\0");

		name = filename; /* Save the file name */

		if(vbose) { fprintf(stdout, "Filename '%s'\n", filename); }
	}
	else {
		fprintf(stderr, "ERROR: Cannot create name for file\n");
		name = NULL;
	}

	if(skip) { open_file(); }
}

/*
Attempts to find the valid subdir path for the current class.
Saves path string to global variable class.
*/
void find_class() {
	if(vbose) { fprintf(stdout, "Searching for class name...\n"); }

	enum Day { SUN = 1, MON = 77, TUE = 84, WED = 87, THR = 82, FRI = 70, SAT = 2 };
	const int EDAY[] = { SUN, MON, TUE, WED, THR, FRI, SAT };

	/* Create our auto variables */
	FILE *ctime;
	char *linebuf, *cname;
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

	int cname_len = CLASS_DIR_LEN + MAX_NAME_SIZE;
	if((cname = (char *)malloc(cname_len)) == NULL) {
		fprintf(stderr, "Error allocating '%d' bytes for class name buffer\n", cname_len);
		exit(1);
	}
	
	/* Find out what times the classes are and figure out if we fall in any of said time intervals */
	while((linebuf = fgets(linebuf, MAX_LINE - 1, ctime)) != 0 && (long)linebuf != EOF) {
		if(*linebuf == '#') continue; //Allow for in-line comments
		
		int start_time = parse_class_time(linebuf, 0, 4), end_time = parse_class_time(linebuf, 5, 9);

		if(strchr(linebuf, '\n') != NULL) {
			int newline_offset = strchr(linebuf, '\n') - linebuf;
			*(linebuf + newline_offset) = '\0';
		}

		if(vbose) { fprintf(stdout, "\nClass Line:'%s'Time: '%d-%d'\n\n", linebuf, start_time, end_time); }

		/* Check current time against time of class */
		if(DEBUG_SKIP || (hour >= (start_time / 100) && hour <= (end_time / 100) && min >= (start_time % 100) && min <= (end_time % 100))) {
			char c;
			for(int i = 10; (c = *(linebuf + i)) != '_'; i++) {
				if(DEBUG_SKIP || (EDAY[day] == c)) { /* If today is one of the class days */
					for(; *(linebuf+ i) != '_'; i++);
					
					/* linebuf + i = "_CLASS" */
					i++; //Increment past the leftover _
					int clen = strchr(linebuf + i, '\0') - (linebuf + i);

					cname = strcpy(cname, CLASS_DIR);
					cname = strncpy(cname + CLASS_DIR_LEN, linebuf + i, clen) - CLASS_DIR_LEN;
					cname = strcat(cname, "/\0");

					if(parse_class_dir(cname) != EOF) {
						class = cname;
					}
					
					goto cfound;
				}
			}
		}
	}

	fclose(ctime);
	free(linebuf);
	free(cname);

	class = NULL;
	return;

cfound:
	if(vbose) { fprintf(stdout, "Class name found: '%s'\n", class); }

	if(skip) { open_file(); }
}

void open_file() {
	if(class != NULL && name != NULL) {
		char *cmd = (char *)malloc(strlen(PROGRAM_PATH) + strlen(class) + strlen(name) + 1);
		sprintf(cmd, "%s%s%s", PROGRAM_PATH, class, name);

		if(cmd == NULL) {
			fprintf(stderr, "Error allocating memory for full file path '%s%s%s'\n", PROGRAM_PATH, class, name);
			exit(1);
		}

		if(edit == 1) {
			char *args[] = {VIM_PATH, cmd, NULL};
			execvp("vim", args);
		}
		else {
			char *args[] = {TOUCH_PATH, cmd, NULL};
			execvp("touch", args);
		}
	}
}

/*
Concats the printable characters of n onto string s

Example:
	concat_int("Hello", 20) -> "Hello20"

Arguments:
	char *s = String to concat to
	int n = Printable digits to concat onto s

Returns:
	A string concatenated with the digits from n

Assumptions:
	String s is a valid pointer and is large enough to hold the digits of n
*/
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
A function that initiates a string to all 0's to avoid potential errors.

Example:
	init_str(&str, 10) -> First 10 chars of str initialized to 0

Arguments:
	char *s = String pointer
	int n = Length of char *s

Returns:
	String s or NULL on error

Assumptions:
	char *s is a valid pointer and n is a correct length of s
*/
char *init_str(char *s, int n) {
	if(s == NULL || n <= 0) return NULL;
	
	for(int i = 0; i < n; i++) *(s + i) = 0;
	return s;
}
