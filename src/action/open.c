#include "notes.h"

void open_file() {
	if(class != NULL && name != NULL) {
		char *cmd = securebuf(strlen(classdir) + strlen(class) + strlen(name) + 1);
		sprintf(cmd, "%s%s%s", classdir, class, name);

		if(edit == 1) {
			char *args[] = {editorpath, cmd, NULL};
			execvp(editorpath, args);
		}
		else {
			char *args[] = {touchpath, cmd, NULL};
			execvp(touchpath, args);
		}
	}
}

char *read_in(char *buf, int n, const char *msg) {
	fprintf(stdout, "%s", msg);
	fflush(stdout);
	buf = fgets(buf, n, stdin);
	*(buf + strlen(buf) - 1) = '\0';
	return buf;
}
