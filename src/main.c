#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "action.h"

int main(int, char*[]);

/* Set up buffers and flags for file names, editing, and class name */
char *name, *class, edit = 1;
int vbose = 0, skip = 0;
char *timedat, *classdat, *editorpath;

int main(int argc, char *argv[]) {
	timedat = classdat = editorpath = NULL;
	const char *DAT_PATH = "data/data.dat";

	if(access(DAT_PATH, R_OK) == 0) {
		char *linebuf = (char *)malloc(MAX_LINE);
		if(linebuf != NULL) {
			FILE *fp = fopen(DAT_PATH, "r");
			while((linebuf = fgets(linebuf, MAX_LINE, fp)) != NULL) {
				int len = strchr(linebuf, ':') - linebuf;
				if(len < 0 || *linebuf == '#') continue;

				if(!strncmp(linebuf, "time", len) && timedat == NULL) {
					timedat = (char *)malloc(strlen(linebuf + len));

					if(timedat != NULL) {
						timedat = strcpy(timedat, linebuf + len + 1);
						*(timedat + strlen(linebuf + len) - 2) = '\0';
					}
				}
				else if(!strncmp(linebuf, "class", len) && classdat == NULL) {
					classdat = (char *)malloc(strlen(linebuf + len));

					if(classdat != NULL) {
						classdat = strcpy(classdat, linebuf + len);
					}
				}
				else if(!strncmp(linebuf, "editor", len) && editorpath == NULL) {
					editorpath = (char *)malloc(strlen(linebuf + len));

					if(editorpath != NULL) {
						editorpath = strcpy(editorpath, linebuf + len);
					}
				}
				continue;
			}
			
			fclose(fp);
			free(linebuf);
		}
	}
	else {
		FILE *fp = fopen(DAT_PATH, "w");
		if(fp != NULL) {
			
		}
	}

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
				printf("\t-v[v]:	 Have the program send more and more updates to stdout\n");
				return 0;
			case 'c':
				/* Skips class name to the next potential arg */
				i++;
				
				/* Save given class name */
				class = argv[i];
				
				/* Correctly sets the action function pointer */
				action = (action == &find_name_and_class ? &find_name : &open_file);
				
				skip = 1;

				break;
			case 'n': /* Same deal as with c arg but with name */
				i++;
				name = argv[i];
				action = (action == &find_name_and_class ? &find_class : &open_file);
				
				skip = 1;

				break;
			case 'e': /* Turns off editing flag */
				edit = 0;
				break;
			case 'v': /* Sets verbose flag */
				for(; *(argv[i] + vbose + 1) == 'v'; vbose++);

				break;
		}
	}

	/* Pass execution to proper function */
	(*action)();
}
