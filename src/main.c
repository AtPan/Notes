#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "action.h"
#include "main.h"
#include "misc.h"

int main(int, char*[]);

/* Set up buffers and flags for file names, editing, and class name */
char *name, *class, edit = 1;
u_int8_t vbose = 0, skip = 0;
extern char *editorpath, *touchpath, *classdir, *timefile;

int main(int argc, char *argv[]) {
	/* On startup:
	   save $HOME/.note dir to installdir (1)
	   parse $HOME/.note/data/path file (2)
	   save $HOME/.note/class dir to classdir (3)
	   save $HOME/.note/data/time file to timefile (4)
	*/

	/* (1) */
	char *installdir = argv[1];
	int dirlen = strlen(installdir);

	/* (2) */
	char *pathfile;
	pathfile = securebuf(pathfile, dirlen + 11);
	pathfile = strcpy(pathfile, installdir);
	pathfile = strcat(pathfile, "/data/path\0");
	parse_path(pathfile);
	free(pathfile);

	/* (3) */
	classdir = securebuf(classdir, dirlen + 7);
	classdir = strcpy(classdir, installdir);
	classdir = strcat(classdir, "/class\0");

	/* (4) */
	timefile = securebuf(timefile, dirlen + 11);
	timefile = strcpy(timefile, installdir);
	timefile = strcat(timefile, "/data/time\0");

	void (*action)() = &find_name_and_class;

	/* Loop through all arguments  */
	for(int i = 2; i < argc; i++) {

		/* All args are 1 char (i.e. -h, -c, -e, etc.) */
		/* Check letter AFTER dash to see the arg */
		switch(*(argv[i] + 1)) {
			case 'h': 
				fprintf(stdout, "Note-organizing program that automatically sorts notes.\n" 
				"Optional Flags:\n"
				"\t-h: 			Shows this message\n"
				"\t-c class:	Override automatic class schedule with a given class\n"
				"\t-n name: 	Override automatice naming scheme with a custom file name\n"
				"\t-e:			Only create the note file, do not open to edit\n"
				"\t-v(v):		Have the program send more and more updates to stdout\n"
				"\t-d:			Create and store custom information about your machine\n");
				return 0;
			case 'c':
				/* Skips class name to the next potential arg */
				i++;
				
				/* Save given class name */
				class = (char *)malloc(8 + strlen(argv[i]));
				if(class != NULL) {
					class = strcpy(class, "class/");
					class = strcat(class, argv[i]);
					class = strcat(class, "/");
				}
				
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

void parse_path(const char *dat_path) {
	FILE *dat;
	char *linebuf;
	if((dat = fopen(dat_path, "r")) != NULL && (linebuf = (char *)malloc(MAX_LINE)) != NULL) {
		while((linebuf = fgets(linebuf, MAX_LINE, dat)) != NULL) {
			int len = strchr(linebuf, ':') - linebuf;
			if(len < 0 || *linebuf == '#') continue;

			if(editorpath == NULL && !strncmp(linebuf, "editor", len)) {
				editorpath = (char *)malloc(strlen(linebuf + len));

				if(editorpath != NULL) {
					editorpath = strcpy(editorpath, (linebuf + len + 1));
				}
			}
			else if(touchpath == NULL && !strncmp(linebuf, "touch", len)) {
				touchpath = (char *)malloc(strlen(linebuf + len));

				if(touchpath == NULL) {
					fprintf(stderr, "touchpath cannot allocate memory\n");
					exit(1);
				}

				touchpath = strcpy(touchpath, (linebuf + len + 1));
			}
		}
		
		fclose(dat);
		free(linebuf);
	}
}
