/* portio.h -- wrapper for inpout32.dll interface emulating Borland
               inportb() and outportb() functions */

#ifndef PORTIO_H__
#define PORTIO_H__

#include <string.h>
#include <math.h>

#if defined(__TURBOC__)
#include <dos.h>
#endif

#if defined(__MINGW32__)
/* these are the public interfaces emulating Borland functions */
/* assume inpout32.dll is copied to \windows\system directory */
void outportb(int port, unsigned char kar);
void outport(int port, int value);
unsigned char inportb(int port);
int inport(int port);
#endif


#endif
