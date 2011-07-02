/* osdep.h    -- GLF support for functions or headers which allow more os-independent
                 source code by hiding OS dependencies from user source modules
                 
                 12/03/2009 Gary Flispart
*/


#include "osdep.h"

#if defined(__TURBOC__) || defined (__MINGW32__)
   #ifdef __TURBOC__
      /* assume TC 2.0 */
   #else
      /* assume mingw32 with gcc 3.4.5 */
   #endif
#else
   /* assume Linux -- assume gcc 4.x and libc6 */
#endif
                                                             
