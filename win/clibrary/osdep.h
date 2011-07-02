/* osdep.h    -- GLF support for functions or headers which allow more os-independent
                 source code by hiding OS dependencies from user source modules
                 
                 Including this header is optional for most programs, but may solve or
                 prevent certain weird errors under certain OS cases (especially
                 TC-DOS stack overflows). 
                 
                 12/03/2009 Gary Flispart
*/


#ifndef OSDEP_H__
#define OSDEP_H__

#if defined(__TURBOC__) || defined (__MINGW32__)
   #ifdef __TURBOC__
      /* assume TC 2.0 */
      #include <dos.h>          /* needed for _stklen below */
      #include <process.h>      /* needed for system() if it is used in TC-DOS */

      /* increased stack length is essential to making TC-DOS versions of memory-intensive
         programs work or if system() is called 
         -- for some reason putting _stklen it in the individual libraries doesn't work
         (presumably because it affects main()), so put it in this header so it is 
         transparent to user code */

      extern unsigned _stklen = 60000;   /* go for the max (almost) */
   #else
      /* assume mingw32 with gcc 3.4.5 */
   #endif
#else
   /* assume Linux -- assume gcc 4.x and libc6 */
#endif
                                                             
#endif
