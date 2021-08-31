#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "action.h"

char *concat_int(char *, int);

/* Connect to the name, class, and edit flags setup in main.c */
extern char *name, *class, edit;

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
	time_t td = time(NULL);
	struct tm *local = localtime(&td);
	int date = local->tm_mday; /* Date of the month */
	int month = local->tm_mon + 1; /* Month of the year */

	char *filename = (char *)malloc(10); /* dd-mm.txt = 9 chars + ending 0 */
	filename = init_str(filename);
	
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

		/*
		*fn++ = '.';
		*fn++ = 't';
		*fn++ = 'x';
		*fn++ = 't';
		*fn++ = '\0';
		*/

		name = filename; /* Save the file name */
	}
	else {
		printf("ERROR: Cannot create name for file\n");
	}

#ifdef SKIP
	open_file();
#endif
}

/*
Attempts to find the valid subdir path for the current class.
Saves path string to global variable class.
*/
void find_class() {
	enum Day { SUN = 1, MON = 77, TUE = 84, WED = 87, THR = 82, FRI = 70, SAT = 2 };
	const int EDAY[] = { SUN, MON, TUE, WED, THR, FRI, SAT };

	/* Create our auto variables */
	FILE *ctime;
	char *linebuf;
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
	
	/* Find out what times the classes are and figure out if we fall in any of said time intervals */
	while((linebuf = fgets(linebuf, MAX_LINE - 1, ctime)) != 0 && linebuf != EOF) {
		if(*linebuf == '#') continue; //Allow for in-line comments

		int start_time = parse_class_time(linebuf, 0, 4), end_time = parse_class_time(linebuf, 5, 9);

//		if(hour > (start_time / 100) && hour < (end_time / 100) && min > (start_time % 100) && min < (end_time % 100)) {
			char c;
			for(int i = 0; (c = *(linebuf + 10 + i)) != '_'; i++) {
//				if(EDAY[day] == c) {
					for(; *(linebuf + 10 + i) != '_'; i++);
					char *cname = (char *)malloc(11);
					int n = 11 + i;
					int nl = (strchr(linebuf, '\n') == NULL) ? 1 : 0, clen = strchr(linebuf + n, '\0') - (linebuf + n);

					cname = strcpy(cname, "class/");
					cname = strncpy(cname + 6, linebuf + n, clen - (nl ? 2 : 1)) - 6;
					cname = strcat(cname, "/\0");

					if(parse_class_dir(cname) != EOF) {
						class = cname;
					}

					goto cfound;
				}
//			}
//		}

	}

cfound:
	fclose(ctime);

#ifdef SKIP
	open_file();
#endif
}

void open_file() {
	char *cmd = (char *)malloc(strlen(CLASS_PATH) + strlen(class) + strlen(name) + 1);
	sprintf(cmd, "%s%s%s\0", CLASS_PATH, class, name);

	if(cmd == NULL) {
		fprintf(stderr, "Error allocating memory for full file path '%s%s%s'\n", CLASS_PATH, class, name);
		exit(1);
	}

	if(edit == 1) {
		char *args[] = {VIM_PATH, cmd, NULL};
		execvp("vim", args);
	}
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

char *init_str(char *s, int n) {
	for(int i = 0; i < n; i++) *(s + i) = 0;
	return s;
}
