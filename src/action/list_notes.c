#include "notes.h"

void list_notes() {
	char buf[256];
	sprintf(buf, "tree %s", classdir);
	
	FILE *pipe = popen(buf, "r");
	if(pipe == NULL) exit(1);

	while(fgets(buf, 256, pipe) != NULL) {
		printf("%s", buf);
	}
	fflush(stdout);

	pclose(pipe);
}
