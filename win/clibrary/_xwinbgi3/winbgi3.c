/* winbgi3.c   -- (Mingw32) libwinbgi3.a assumed for Windows
                  (GCC) libxwinbgi3.a and libxwinlite.a assumed for Ubuntu Linux
                  (Turbo C 2.01/DOS) "graphic3.h" is a copy of Borland <graphics.h>
                  TC programs link to a copy of graphics.lib named graphic3.lib
                  
                  NOTE:  C version of library file only defined for Linux
*/                  


#include "graphic3.h"

#if defined(__MINGW32__) || defined(__TURBOC__)

#if defined(__TURBOC__)
/* assume Borland Turbo C 2.01 for DOS */
#else
/* assume Windows -- must compile as C++, compatible with C program callers */
#endif  /* __TURBOC__ or not */

#else
/* Assume Linux */

/* Linux version uses X library and only C code */

#include "xwinbgi3.c"

#endif
