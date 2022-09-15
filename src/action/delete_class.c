#include "notes.h"

/* WORK IN PROGRESS: Not thoroughly tested yet. */
void delete_class() {
	FILE *fp = fopen(timefile, "r+");
	if(fp == NULL) exit(1);

	FILE *tmp_file = popen("echo $HOME/.note/data/time_tmp", "r");
	if(tmp_file == NULL) exit(1);
	char tmp_file_name[MAX_LINE];
	fgets(tmp_file_name, MAX_LINE, tmp_file);
	pclose(tmp_file);

	tmp_file = fopen(tmp_file_name, "w");
	if(tmp_file == NULL) exit(1);

	char buf[256];
	int i;

	for(i = 1; fgets(buf, 256, fp) != NULL; i++) {
		printf("%d. %s", i, buf);
	}

	fflush(stdout);
	fseek(fp, 0, SEEK_SET);

	int line = 0;
	do {
		read_in(buf, 256, "Choose a line to delete: ");
		line = atoi(buf);
	} while(line > i || line < 1);

	for(i = 0; i < line - 1; i++) {
		fgets(buf, 256, fp);
	}
	
	int pos_before_line = ftell(fp);

	// Seek to start of line to delete
	fseek(fp, 0, SEEK_SET);
	while(1) {
		if(ftell(fp) == pos_before_line) {
			fgets(buf, 256, fp);
			continue;
		}
		fgets(buf, 256, fp);

		if(feof(fp)) {
			break;
		}

		fputs(buf, tmp_file);
	}

	fclose(fp);
	fclose(tmp_file);

	rename(tmp_file_name, timefile);
}