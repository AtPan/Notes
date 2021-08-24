#include <stdio.h>

int main(int, char*[]);

/* Set up buffers and flags for file names, editing, and class name (20 chars max) */
char name[20], class[20], edit = 1;

int main(int argc, char *argv[]) {

	/* Loop through all arguments  */
	for(int i = 1; i < argc; i++) {
		/* All args are 1 char (i.e. -h, -c, -e, etc.) */
		/* Check letter AFTER dash to see the arg */
		switch(*(argv[i] + 1)) {
			case 'h': /* A little sloppy but it works */
				printf("Note-organizing program that automatically sorts notes.\n");
				printf("Optional Flags:\n");
				printf("\t-h: 		 Shows this screen\n");
				printf("\t-c class:  Override automatic class schedule with a given class\n");
				printf("\t-n name: 	 Override automatice naming scheme with a custom file name\n");
				printf("\t-e:		 Only create the note file, do not open to edit\n");
				return 0;
			case 'c':
				i++; /* Skips class name to the next potential arg */
				class = argv[i]; /* Save given class name */
				break;
			case 'n': /* Same deal as with c arg but with name */
				i++;
				name = argv[i];
				break;
			case 'e': /* Turns off editing flag */
				edit = 0;
				break;
		}
	}
}
