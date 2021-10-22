#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "main.h"
#include "action.h"
#include "parse.h"
#include "misc.h"

/* Connect to the name, class, and edit flags setup in main.c */
extern char *name, *class, edit;
extern int vbose, skip;
extern char *timefile, *classdir, *editorpath, *touchpath;

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

	char *filename; /* dd-mm.txt = 9 chars + ending 0 */
	filename = securebuf(filename, 10);
	filename = init_str(filename, 10);
	
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
	free(filename);

	if(vbose) { fprintf(stdout, "Filename '%s'\n", filename); }
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
	int time = (local->tm_hour * 100) + local->tm_min;
	int day = local->tm_wday;
	char found = 0;

	if((ctime = fopen(timefile, "r")) == NULL) {
		fprintf(stderr, "Error opening time-schedule file '%s'\n", TIME_LOC);
		fprintf(stderr, "ERROR: 0%o\n", 0301);
		exit(1);
	}

	linebuf = securebuf(linebuf, MAX_LINE);
	cname = securebuf(cname, MAX_LINE);

	/* Find out what times the classes are and figure out if we fall in any of said time intervals */
	while((linebuf = fgets(linebuf, MAX_LINE - 1, ctime)) != 0 && (long)linebuf != EOF && !found) {
		if(*linebuf == '#') continue; //Allow for in-line comments
		
		int start_time = parse_class_time(linebuf, 0, 4), end_time = parse_class_time(linebuf, 5, 9);

		if(strchr(linebuf, '\n') != NULL) {
			int newline_offset = strchr(linebuf, '\n') - linebuf;
			*(linebuf + newline_offset) = '\0';
		}

		if(vbose) { fprintf(stdout, "\nClass Line:'%s'Time: '%d-%d'\n\n", linebuf, start_time, end_time); }

		/* Check current time against time of class */
		if(DEBUG_SKIP || (time >= start_time && time <= end_time)) {
			char c;
			for(int i = 10; (c = *(linebuf + i)) != '_'; i++) {
				if(DEBUG_SKIP || (EDAY[day] == c)) { /* If today is one of the class days */
					for(; *(linebuf+ i) != '_'; i++);
					
					/* linebuf + i = "_CLASS" */
					i++; //Increment past the leftover _
					int clen = strchr(linebuf + i, '\0') - (linebuf + i);

					cname = strcpy(cname, "");
					cname = strncpy(cname + CLASS_DIR_LEN, linebuf + i, clen) - CLASS_DIR_LEN;
					cname = strcat(cname, "/\0");

					if(parse_class_dir(cname) != EOF) {
						class = cname;
					}
					
					found = 1;
				}
			}
		}
	}

	fclose(ctime);
	free(linebuf);

	if(vbose) { fprintf(stdout, "Class name found: '%s'\n", class); }

	if(!found) {
		class = NULL;
		free(cname);
	}

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

