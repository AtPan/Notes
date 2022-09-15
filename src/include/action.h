#ifndef _ACTION_H
#define _ACTION_H

void open_file(void);
void find_name_and_class(void);
void find_name(void);
void find_class(void);
void add_class(void);
void delete_class(void);
void list_notes(void);

char *read_in(char *, int, const char *);

#define MAX_LINE 75

#ifdef DEBUG
#define DEBUG_SKIP 1
#else
#define DEBUG_SKIP 0
#endif

#endif
