#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "action.h"
#include "misc.h"

char *securebuf(char *buf, int n) {
	char *initbuf(char *, int);
	static int bufc;

	if(n < 0) {
		n = MAX_LINE;
	}

	buf = malloc(n);
	if(buf == NULL) {
		fprintf(stderr, "Could not secure buffer\n");
		fprintf(stderr, "ERROR: 0%o\n", (MEM_ERR_CDE + bufc));
		exit(2);
	}
	else {
		bufc++;
		return initbuf(buf, n);
	}
}

/*
A function that initiates a string to NULL bytes.
Useful to avoid any potential issues when using a string buffer.

Arguments:
	char *s = Memory pointer
	int n	= Length of char *s

Returns:
	Initiated buffer s or NULL on error

Assumptions:
	char *s is a valid pointer and n is the not greater than what s points to
*/
char *initbuf(char *s, int n) {
	if(s == NULL || n <= 0) return NULL;

	return memsetset(s, 0, n);
}

/*
Concats the printable characters of n onto string s

Example:
	concat_int("Hello", 20) -> "Hello20"

Arguments:
	char *s = String to concat to
	int n   = Printable digits to concat onto s

Returns:
	Formatted string concatenated with n

Assumptions:
	String s is a valid pointer and is large enough to hold the digits of n
*/
char *concat_int(char *s, int n) {
	char *str = s;
	int len = 0;
	for(int i = n; i > 0; i /= 10) len++;

	for(int i = 0; i < len; i++) {
		*(str + (len - i - 1)) = (n % 10) + '0';
		n /= 10;
	}

	return s;
}
