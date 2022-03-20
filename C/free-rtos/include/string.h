/* $Revision: 1.2 $ */

#ifndef _STRING_H_
#define _STRING_H_

int             islower(char ch);
int             toupper(char ch);
int             isnewline(char ch);
void            bcopy(char *b1, char *b2, int length);
int             strncmp(char *s1, char *s2, int n);
char           *index(char *p, char ch);
char           *rindex(char *p, char ch);
int             isspace(char ch);
int             hexchar_to_int(char ch);
int             atoi(char *cp);
int             getopt(int nargc, char *const * nargv, char *ostr);
int             strlen(const char *str);

#endif
