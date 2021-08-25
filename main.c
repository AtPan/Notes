#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

int main(int, char*[]);
void open_file();
void find_name_and_class();
void find_name();
void find_class();
char *concat_int(char *, int);

/* Set up buffers and flags for file names, editing, and class name */
char *name, *class, edit = 1;

int main(int argc, char *argv[]) {
	void (*action)() = &find_name_and_class;

	/* Loop through all arguments  */
	for(int i = 1; i < argc; i++) {

		/* All args are 1 char (i.e. -h, -c, -e, etc.) */
		/* Check letter AFTER dash to see the arg */
		switch(*(argv[i] + 1)) {
			case 'h': 
				/* A little sloppy but it works */
				printf("Note-organizing program that automatically sorts notes.\n");
				printf("Optional Flags:\n");
				printf("\t-h: 		 Shows this screen\n");
				printf("\t-c class:  Override automatic class schedule with a given class\n");
				printf("\t-n name: 	 Override automatice naming scheme with a custom file name\n");
				printf("\t-e:		 Only create the note file, do not open to edit\n");
				return 0;
			case 'c':
				/* Skips class name to the next potential arg */
				i++;
				
				/* Save given class name */
				class = argv[i];
				
				/* Correctly sets the action fp */
				action = (action == &find_name_and_class ? &find_name : &open_file);
				break;
			case 'n': /* Same deal as with c arg but with name */
				i++;
				name = argv[i];
				action = (action == &find_name_and_class ? &find_class : &open_file);
				break;
			case 'e': /* Turns off editing flag */
				edit = 0;
				break;
		}
	}

	(*action)();
}

void find_name_and_class() {
	find_name();
	find_class();
	open_file();
}

void find_name() {
	time_t td = time(NULL);
	struct tm *local = localtime(&td);
	int date = local->tm_mday; /* Date of the month */
	int month = local->tm_mon + 1; /* Month of the year */

	char *filename = (char *)malloc(10); /* dd-mm.txt = 9 chars */
	
	if(filename >= 0) {
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

		name = filename;
	}
	else printf("ERROR: Cannot create name for file\n");
}

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

void find_class() {

}

void open_file() {

}
