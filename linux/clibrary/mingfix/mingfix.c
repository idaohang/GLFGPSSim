/* mingfix.c -- dummy patches to solve certain problems with outside programs */

#include "mingfix.h"

#if defined(__TURBOC__) || (__MINGW32__)

/* stubs to allow compilation to proceed */

char *ttyname (int fd)
  {
   return NULL;
  }
  
  
int /* ssize_t */ getline(char **lineptr, /* size_t */ int *n, int *stream)
{
  return 0;
}

  
int fchmod (int fd, /* mode_t */ int mode)
  {
   /*
   if (fd < 0)
      {
       __set_errno (EINVAL);
       return -1;
      }

    __set_errno (ENOSYS);
   return -1;
   */ 
   return -1;
  }

#else
/* Assume Linux defines these properly */
#endif
  
