/* mingfix.h -- dummy patches to solve certain problems with outside programs */

#ifndef MINGFIX_H__
#define MINGFIX_H__

#if defined(__TURBOC__) || (__MINGW32__)

#include <stdlib.h>
#include <stdio.h>

/* stubs to allow compilation to proceed for non-Linux programs */
extern char *ttyname (int fd);
extern /* ssize_t */ int getline(char **lineptr, /* size_t */ int *n, int *stream);
extern int fchmod (int fd, /* mode_t */ int mode);
#else
/* Assume Linux defines these */

#endif

#endif
