/* updated.h -- GLF support for functions still needed but deemed 
                 obsolete or deprecated by language gurus 
                 
                 best to include this header LAST in case functions are still
                 available from original support libraries -- otherwise
                 platform-specific substitutes are provided here -- library
                 might be empty but could contain bridge code
                 
                 12/07/2008 Gary Flispart
*/


/* in general when converting old code, use "obsolete.h" before editing old 
                calls, use "updated.h" after editing to use new call names */
                
/* obsolete.h   resolves old names and replaces with newer calls
                in NEWER environments -- leaves old calls as-is */ 

/* updated.h   resolves new names and replaces with older calls
                in OLDER environments -- leaves new calls as-is */ 

#ifndef UPDATED_H__
#define UPDATED_H__

#include <string.h>   /* NOTE: <mem.h> is actually a wrapper for <string.h> */

#if defined(__TURBOC__) || defined (__MINGW32__)
   #ifdef __TURBOC__
      /* assume TC 2.0 */
      #undef strcasecmp
      #define strcasecmp  stricmp 
      #undef strncasecmp
      #define strncasecmp strnicmp 

   #else
      /* assume mingw32 with gcc 3.4.5 */
      #undef strcasecmp
      #define strcasecmp  stricmp 
      #undef strncasecmp
      #define strncasecmp strnicmp 
   #endif
#else
   /* assume Linux -- assume gcc 4.x and libc6 */
#endif
                                                             
#endif