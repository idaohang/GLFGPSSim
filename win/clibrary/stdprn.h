/* stdprn.h -- add equivalent to TURBOC stdprn unit to GCC-based code */

#ifndef STDPRN_H__
#define STDPRN_H__

/* stdprn is predefined for Turbo C, but not MinGW or Linux 
   -- the following makes stdprn defined for ALL OSes */

/* DO NOT use fstdprn directly;  instead refer to stdprn,
   which below will be redefined for user programs 
*/
extern FILE *fstdprn;

/* NEVER set __STDPRN_COMPILING__ (only stdprn.c does that) */
#ifndef __STDPRN_COMPILING__
/* first make sure TC programs can't use stdprn accidentally... */
   #undef stdprn
/* ...then give them a stdprn they can really use */
   #define stdprn fstdprn
#endif   


/* define these for all */

int open_stdprn(char portname[]);
void close_stdprn(void);

#endif

