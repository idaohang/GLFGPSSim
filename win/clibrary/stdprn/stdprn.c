/* stdprn.c -- add equivalent to TURBOC stdprn unit to GCC-based code */

#include "gflib.h"
#include "obsolete.h"

/* the following ONLY gets defined here, so user programs can HIDE the REAL stdprn */
#define __STDPRN_COMPILING__
#include "stdprn.h"

/* use fstdprn internally as substitute for stdprn */
/* (stdprn is file pointer predefined for Turbo C, but not MinGW or Linux) */

/* definitions in stdprn.h ensure user programs see the handle "stdprn" in all OSes */

FILE *fstdprn = NULL;  

/* define these for all -- all use fstdprn; only TC sees stdprn at this point */

/* function allows for redirection of printer to filename */
int open_stdprn(char portname[])
  {
   /* if no name passed, try to open a standard printer port */
   if (portname[0] == 0)
     {
      #if defined (__MINGW32__)
         /* Assume Windows / Mingw32 / GCC */
         if (NULL == (fstdprn = fopen("LPT1","wb")))
           {
            return FALSE;
           }
      #endif
      #if defined(__TURBOC__)
         /* Assume Turbo C has predefined and opened stdprn */
         fstdprn = stdprn;   /* duplicate the file pointer */
      #endif
      #if defined(__linux__)
         /* Assume Linux / GCC */
         if (NULL == (fstdprn = fopen("/dev/lp0","wb")))
           {
            return FALSE;
           }
      #endif
      return TRUE;
     }
     
   /* ... portname is NOT null... */
   /* all OS's should allow print diveresion to a text file */
   if (NULL == (fstdprn = fopen(portname,"wb")))
     {
      return FALSE;
     }
   return TRUE;
  }

void close_stdprn(void)
  {
/*
#ifdef __TURBOC__
   if (fstdprn != stdprn)
     {
      fclose(fstdprn);
     } 
#else
   fclose(fstdprn);
#endif
*/
   fclose(fstdprn);
  }

  
 
