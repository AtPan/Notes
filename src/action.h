#ifndef _ACTION_H
#define _ACTION_H

void open_file();
void find_name_and_class();
void find_name();
void find_class();
char *init_str(char *s, int n);

#define TIME_LOC "data/time.txt"
#define CLASS_LOC "data/class.txt"
#define MAX_LINE 50
#define VIM_PATH "/usr/bin/vim"
#define TOUCH_PATH "/usr/bin/touch"
#define PROGRAM_PATH "/home/floofykn/Programming/Projects/Notes/"
#define CLASS_DIR "class/"
#define CLASS_DIR_LEN (strlen(CLASS_DIR))
#define MAX_NAME_SIZE 4

#ifdef DEBUG
#define DEBUG_SKIP 1
#define debug_print(fmt, ...) \
	do { fprintf(stderr, "%s:%d:%s\n", fmt, __FILE__, __LINE__, __func__, VA_ARGS);  } while(0)
#else
#define DEBUG_SKIP 0
#endif

#endif
