/* gftermio.c -- terminal i/o package -- adapted to C by Gary Flispart

          Designed for Mingw32 GCC 3.31 --OR-- or Turbo C 2.0

          Turbo C 2.0 uses native DOS interface and direct hardware I/O
          MinGW port DEPENDS ON WINDOWS CALLS!

          original routines were written in Turbo Pascal and derive from
          files downloaded from Compuserve Information Service on 24 Feb 87.
          The following header comments are included as acknowledgement:

          TITLE : Interrupt handler
          PRODUCT : Turbo Pascal
          VERSION : All
          OS : Ms DOS 1.00 or greater
          DATE : 2/1/85

          Written by,
          Jim McCarthy
          Technical Support
          Borland International

        and
          Andy Batony
          Teleware Incorporated

------------------------------------------------------------------------- */

#include "gflib.h"
#include "gftermio.h"     /* use common header for all platforms */

/* bring in platform specific code */
#ifdef __TURBOC__
#include "os/gftiotc.c"
#else
#ifdef __MINGW32__
#include "os/gftiomgw.c"
#else                   /* assume Linux */
#include "os/gftiolin.c"
#endif
#endif


