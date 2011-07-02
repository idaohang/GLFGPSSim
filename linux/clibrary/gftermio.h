/* gftermio.h -- terminal i/o package -- adapted to C by Gary Flispart

          Designed for Mingw32 GCC 3.31 --OR-- or Turbo C 2.0

          -- adapted to C by Gary Flispart 1988-2008
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
#ifndef GFTERMIO_H__
#define GFTERMIO_H__

#include <string.h>

void close_com(int portspec);
unsigned int status_com(int portspec);
void set_com_dtrrts(int portspec, int dtr, int rts);  /* set DTR and RTS state */
int open_com_dtrrts(int portspec, int ratespec, int bits, int parity,
                    int stopbits, int dtr, int rts);
int open_com(int portspec, int ratespec, int bits, int parity, int stopbits);
  

char read_com(int portspec);
void write_com(int portspec, char ch);
int avail_com(int portspec);


#endif

