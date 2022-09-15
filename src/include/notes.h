#ifndef __NOTES_H
#define __NOTES_H

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "main.h"
#include "action.h"
#include "parse.h"
#include "misc.h"

/* Connect to the name, class, and edit flags setup in main.c */
extern char *name, *class, edit;
extern int vbose, skip;
extern char *timefile, *classdir, *editorpath, *touchpath;

#endif
