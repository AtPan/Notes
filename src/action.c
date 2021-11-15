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

	char *filename = securebuf(10); /* dd-mm.txt = 9 chars + ending 0 */
	char *fn = filename;

	if(month < 10) {
		*fn = '0';
	}
	fn = concat_int(fn, month) + 2;

	*fn++ = '-';

	if(date < 10) {
		*fn = '0';
	}
	fn = concat_int(fn, date) + 2;
	
	fn = strcat(fn, ".txt\0");

	name = filename; /* Save the file name */

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
		fprintf(stderr, "Error opening time-schedule file '%s'\n", timefile);
		fprintf(stderr, "ERROR: 0%o\n", 0301);
		exit(1);
	}

	linebuf = securebuf(MAX_LINE);
	cname = securebuf(MAX_LINE);

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

					cname = strcpy(cname, linebuf + i);

					if(parse_class_dir(cname) != EOF) {
						class = cname;
						class = strcat(class, "/");
					}
					else {
						class = NULL;
					}
					
					found = 1;
					break;
				}
			}
		}
	}

	fclose(ctime);
	free(linebuf);

	if(vbose) { fprintf(stdout, "Class name found: '%s'\n", class); }

	if(!found) {
		fprintf(stderr, "No class at this time and day\n");
		exit(3);
	}
	if(class == NULL) {
		fprintf(stderr, "Could not parse class name\n");
		exit(2);
	}

	if(skip) { open_file(); }
}

void open_file() {
	if(class != NULL && name != NULL) {
		char *cmd = securebuf(strlen(classdir) + strlen(class) + strlen(name) + 1);
		sprintf(cmd, "%s%s%s", classdir, class, name);

		if(edit == 1) {
			char *args[] = {editorpath, cmd, NULL};
			execvp("vim", args);
		}
		else {
			char *args[] = {touchpath, cmd, NULL};
			execvp("touch", args);
		}
	}
}

void add_class() {
	char *read_in(char *, int, const char *);

	FILE *timefilestream = fopen(timefile, "a");
	char *buffer = securebuf(MAX_LINE);

	/* Name of Class */
	char *classname = read_in(buffer, MAX_LINE, "Enter Name of Class: ");

	/* Start Time */
	buffer = securebuf(MAX_LINE);
	buffer = read_in(buffer, MAX_LINE, "Enter Class Start Time (in 24-hour notation, no colon): ");
	int starttime = atoi(buffer);

	/* End Time */
	buffer = read_in(buffer, MAX_LINE, "Enter Class End Time (in 24-hour notation, no colon): ");
	int endtime = atoi(buffer);

	/* Days */
	char *days = read_in(buffer, MAX_LINE, "Enter Class Days (no spaces, one letter): ");

	buffer = securebuf(MAX_LINE);
	sprintf(buffer, "%d_%d_%s_%s\n", starttime, endtime, days, classname);

	fputs(buffer, timefilestream);
	fflush(timefilestream);

	fclose(timefilestream);
	free(buffer);
	free(classname);
	free(days);
}

char *read_in(char *buf, int n, const char *msg) {
	fprintf(stdout, msg);
	fflush(stdout);
	buf = fgets(buf, n, stdin);
	*(buf + strlen(buf) - 1) = '\0';
	return buf;
}
