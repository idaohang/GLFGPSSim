/* scandir.c -- framework for directory scanning programs
                works within current directory OR within parameter 1 path */

/* this module performs all directory scanning by examining ALL files in the
   tree as specified, and marks the files found for inclusion or exclusion
   in an externally driven search -- wildcard matching is performed by this 
   mmodule, not by the OS -- if a file name does not match the scan wildcards
   or specs, it's filtering data block is marked with a '!' character so it 
   may be ignored -- unless directory matching is specified, dir names are 
   marked with '!' so that only filenames are passed back for file processing 
   NOTE:  the commentary in scandir.c has been limited to keep file size below 
   64K for Turbo C. 
*/

/* OS-neutral directory scan module by Gary Flispart (last modifued 11/30/2009) */

/* Version 3 -- Major changes to API and exposed utility functions 11/30/2009 GLF 
                Also fixed bug in extension matching algorithm which sometimes
                indicated match when it shouldn't have */

/* Much of the code is OS-neutral, but OS-specific code is isolated to 
   OS-specific modules 
*/


#if defined(__TURBOC__)
/* assume Turbo C 2.01 for MS-DOS */
#include "os/sd_tcdos.c"
#else
#ifdef __MINGW32__
/* assume MinGW32/GCC for Windows */
#include "os/sd_mgw.c"
#else
/* assume GCC for Linux */
#include "os/sd_lin.c"
#endif
#endif
