#include "notes.h"

int main(int, char*[]);

/* Set up buffers and flags for file names, editing, and class name */
char *name, *class, edit = 1;
int vbose = 0, skip = 0;
char *editorpath, *touchpath, *classdir, *timefile;

int main(int argc, char *argv[]) {
	/* On startup:
	   (1) save $HOME/.note dir to installdir
	   (2) parse $HOME/.note/data/path file
	   (3) save $HOME/.note/class dir to classdir
	   (4) save $HOME/.note/data/time file to timefile
	*/

	/* (1) */
	char *installdir = securebuf(MAX_LINE);
	FILE *pipe = popen("echo $HOME/.note", "r");
	installdir = fgets(installdir, MAX_LINE, pipe);
	pclose(pipe);
	int dirlen = strlen(installdir) - 1;
	*(installdir + dirlen) = '\0';

	/* (2) */
	char *pathfile = securebuf(dirlen + 11);
	pathfile = strcpy(pathfile, installdir);
	pathfile = strcat(pathfile, "/data/path");
	parse_path(pathfile);
	free(pathfile);

	/* (3) */
	classdir = securebuf(dirlen + 8);
	classdir = strcpy(classdir, installdir);
	classdir = strcat(classdir, "/class/");

	/* (4) */
	timefile = securebuf(dirlen + 11);
	timefile = strcpy(timefile, installdir);
	timefile = strcat(timefile, "/data/time");

	free(installdir);

	void (*action)() = &find_name_and_class;

	/* Loop through all arguments  */
	for(int i = 1; i < argc; i++) {

		/* All args are 1 char (i.e. -h, -c, -e, etc.) */
		/* Check letter AFTER dash to see the arg */
		switch(*(argv[i] + 1)) {
			case 'h': 
				fprintf(stdout, "Note-organizing program that automatically sorts notes.\n" 
				"Optional Flags:\n"
				"\t-h:          Shows this message\n"
				"\t-c class:    Override automatic class schedule with a given class\n"
				"\t-n name:     Override automatice naming scheme with a custom file name\n"
				"\t-e:          Only create the note file, do not open to edit\n"
				"\t-v(v):       Have the program send more and more updates to stdout\n"
				"\t-l:          List all saved notes under all stored class directories\n"
//				"\t-d:          Delete a class from your schedule\n"
				"\t-a:          Add a class to your schedule\n");
				return 0;
			case 'c':
				/* Skips class name to the next potential arg */
				i++;
				
				/* Save given class name */
				class = securebuf(strlen(argv[i]));
				if(parse_class_dir(argv[i]) != EOF) {
					class = strcpy(class, argv[i]);
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
			case 'a': /* Add a class to the schedule */
				action = &add_class;
				break;
			case 'e': /* Turns off editing flag */
				edit = 0;
				break;
			case 'l': /* List all notes */
				action = &list_notes;
				break;
			case 'v': /* Sets verbose flag */
				for(; *(argv[i] + vbose + 1) == 'v'; vbose++);
				break;
			case 'd':
				action = &delete_class;
				break;
			default:
				printf("Invalid switch passed\n");
				exit(1);
				break;
		}
	}

	/* Pass execution to proper function */
	(*action)();

	return 0;
}

/*
void delete_class() {
	FILE *fp = fopen(timefile, "r+");
	if(fp == NULL) exit(1);

	char buf[256];
	int i;

	for(i = 1; fgets(buf, 256, fp) != NULL; i++) {
		printf("%d. %s", i, buf);
	}
	fflush(stdout);
	fseek(fp, 0, SEEK_SET);


	fclose(fp);
}
*/
