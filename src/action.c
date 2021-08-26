#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "action.h"

/* Connect to the name, class, and edit flags setup in main.c */
extern char *name, *class;

/* Makes calls to both find_name and find_class functions */
void find_name_and_class() {
	find_name();
	find_class();
	open_file();
}

/* Find name of the file using default dd-mm.txt format */
void find_name() {
	time_t td = time(NULL);
	struct tm *local = localtime(&td);
	int date = local->tm_mday; /* Date of the month */
	int month = local->tm_mon + 1; /* Month of the year */

	char *filename = (char *)malloc(10); /* dd-mm.txt = 9 chars + ending 0 */
	
	if(filename >= 0) { /* If pointer exists, fill it with the naming convention */
		char *fn = filename;

		if(date < 10) {
			*fn++ = '0';
		}
		fn = concat_int(fn, date);
		
		*fn++ = '-';
		
		if(month < 10) {
			*fn++ = '0';
		}
		fn = concat_int(fn, month);
		
		strcat(fn, ".txt");
		
		*(fn + 9) = '\0';

		name = filename; /* Save the file name */
	}
	else {
		printf("ERROR: Cannot create name for file\n");
	}

	#ifdef SKIP
	open_file();
	#endif
}

void find_class() {
	
	#ifdef SKIP
	open_file();
	#endif
}

void open_file() {

}

/* Concats a given string s with ASCII characters of int n
   Example: concat_int("Hello", 15) -> "Hello15" */
char *concat_int(char *s, int n) {
	char *str = s;
	int len = 0;
	for(int i = n; i > 0; i /= 10) len++;

	for(int i = 0; i < len; i++) {
		*(str+(len - i - 1)) = (n % 10) + '0';
		n /= 10;
	}

	return s + len;
}

