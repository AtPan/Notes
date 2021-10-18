#include <stdio.h>
#include <string.h>

extern char *touchpath, *editorpath;

int find_path(char *buf, int n) {
	FILE *pipe = popen("echo $HOME/.note", "r");
	if(pipe == NULL) {
		exit(2);
	}

	char i, c;
	for(i = 0; i < (n - 10) && (c = fgetc(pipe)) != EOF && c != '\n'; i++) {
		*(buf + i) = c;
	}

	pclose(pipe);

	buf = strcat(buf, "/data/path");

	return i;
}
