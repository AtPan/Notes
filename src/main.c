#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "action.h"
#include "main.h"
#include "startup.h"

int main(int, char*[]);

/* Set up buffers and flags for file names, editing, and class name */
char *name, *class, edit = 1;
u_int8_t vbose = 0, skip = 0;
extern char *editorpath, *touchpath;

int main(int argc, char *argv[]) {
	/* On startup:
	   parse $HOME/.note dir
	   parse $HOME/.note/data/path file
	*/

	editorpath = touchpath = NULL;

	char *path = (char *)malloc(MAX_LINE + 10);

	if(path == NULL) {
		exit(1);
	}

	int pathlen = find_path(path, MAX_LINE);

	printf("%s\n", path);
	return 0;

	free(buf);



	void (*action)() = &find_name_and_class;

	/* Loop through all arguments  */
	for(int i = 1; i < argc; i++) {

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
			case 'd': /* Allows for the saving of custom paths around your machine */
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
					editorpath = strcpy(editorpath, linebuf + len);
				}
			}
			else if(touchpath == NULL && !strncmp(linebuf, "touch", len)) {
				touchpath = (char *)malloc(strlen(linebuf + len));

				if(touchpath != NULL) {
					touchpath = strcpy(touchpath, linebuf + len);
				}
			}
		}
		
		fclose(dat);
		free(linebuf);
	}
}
