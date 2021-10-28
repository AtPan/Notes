#ifndef _ACTION_H
#define _ACTION_H

void open_file();
void find_name_and_class();
void find_name();
void find_class();
void add_class();

#define MAX_LINE 75

#ifdef DEBUG
#define DEBUG_SKIP 1
#define debug_print(fmt, ...) \
	do { fprintf(stderr, "%s:%d:%s\n", fmt, __FILE__, __LINE__, __func__, VA_ARGS);  } while(0)
#else
#define DEBUG_SKIP 0
#endif

#endif
