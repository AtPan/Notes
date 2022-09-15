#include "notes.h"

void add_class() {
	FILE *timefilestream = fopen(timefile, "a");
	char *buffer = securebuf(MAX_LINE);

	/* Name of Class */
	char *classname = read_in(buffer, MAX_LINE, "1. Enter Name of Class: ");

	/* Start Time */
	buffer = securebuf(MAX_LINE);
	buffer = read_in(buffer, MAX_LINE, "2. Enter Class Start Time (in 24-hour notation, no colon): ");
	int starttime = atoi(buffer);

	/* End Time */
	buffer = read_in(buffer, MAX_LINE, "3. Enter Class End Time (in 24-hour notation, no colon): ");
	int endtime = atoi(buffer);

	/* Days */
	char *days = read_in(buffer, MAX_LINE, "4. Enter Class Days ([S]unday, [M]onday, [T]uesday, [W]ednesday, thu[R]sday, [F]riday, s[A]turday): ");

	buffer = securebuf(MAX_LINE);
	sprintf(buffer, "%d_%d_%s_%s\n", starttime, endtime, days, classname);

	fputs(buffer, timefilestream);
	fflush(timefilestream);

	fclose(timefilestream);
	free(buffer);
	free(classname);
	free(days);
}


