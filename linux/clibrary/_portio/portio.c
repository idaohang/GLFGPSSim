/* portio.c -- wrapper for inpout32.dll interface emulating Borland
               inportb() and outportb() functions */

#include "portio.h"


#if defined(__MINGW32__) || defined(__TURBOC__)
#if defined(__TURBOC__)
/* these functions are available in Turbo C under <dos.h> */
#include <dos.h>

#else
/* Assume Windows + MinGW32 */

#include <windows.h>

/**************************************************/
/*** portio.c was developed November 23, 2005   ***/
/*** by Gary Flispart from the following free   ***/
/*** published code demonstrating inpout32.dll  ***/
/*** interface allowing WIN32 access to         ***/
/*** low-level PC-compatible hardware ports:    ***/
/***                                            ***/ 
/*** TEST.c  -- test interface to inpout32.dll  ***/
/***  ( http://www.logix4u.net/inpout32.htm )   ***/
/***                                            ***/
/*** Copyright (C) 2003, Douglas Beattie Jr.    ***/
/***                                            ***/
/***    <beattidp@ieee.org>                     ***/
/***    http://www.hytherion.com/beattidp/      ***/
/***                                            ***/
/**************************************************/

/* prototype (function typedef) for DLL functions */
typedef short _stdcall (*inpfuncPtr)(short portaddr);
typedef void _stdcall (*oupfuncPtr)(short portaddr, short datum);

/* hidden support for emulated Borland inportb() and outportb() */
/* assume inpout32.dll has been copied to \windows\system directory */
static HINSTANCE hLib;
static inpfuncPtr inp32;
static oupfuncPtr oup32;
static int access_has_opened = 0;

static void close_port_access(void)
  {
   if (access_has_opened)
     {
      FreeLibrary(hLib);
      access_has_opened = 0;
     }
  }

static int init_port_access(void)
  {
   if (access_has_opened)
     {
      return 1;                         
     }

   /* Load the library */
   hLib = LoadLibrary("inpout32.dll");
   if (hLib == NULL)
     {
      /* LoadLibrary Failed */
      return 0;
     }

   /* get the address of the function */
   inp32 = (inpfuncPtr) GetProcAddress(hLib, "Inp32");
   if (inp32 == NULL)
     {
      /* GetProcAddress for Inp32 Failed*/
      return 0;
     }
   oup32 = (oupfuncPtr) GetProcAddress(hLib, "Out32");
   if (oup32 == NULL)
     {
      /* GetProcAddress for Oup32 Failed */
      return 0;
     }

   /* now that library has been connected properly, set up closure */ 
   if (atexit(close_port_access) == 0)
     {
      access_has_opened = 1;
      } 
   else
     {
      FreeLibrary(hLib);
     }   
   return access_has_opened;  
  }


/* these are the public interfaces emulating Borland functions */

void outportb(int port, unsigned char kar)
  {
   if (init_port_access())
     {
      (oup32)(port,kar); 
     }
  }


unsigned char inportb(int port)
  {
   if (init_port_access())
     {
      return (unsigned char)((inp32)(port));
     }
   return 0;  
  }

void outport(int port, int value)
  {
   unsigned int kar1,kar2;
     
   if (init_port_access())
     {
      kar1 = 0x00FF & (unsigned)value;                    
      kar2 = 0xFF00 & (unsigned)value;                    
      kar2 = kar2 >> 8;
      (oup32)(port,kar1); 
      (oup32)(port,kar2); 
     }
  }


int inport(int port)
  {
   unsigned char kar1,kar2;
   unsigned int combo;

   if (init_port_access())
     {
      kar1 = (inp32)(port);
      kar2 = (inp32)(port);
      combo = kar2 << 8;
      combo = combo || kar1;
      return (int)combo;
     }
   return 0;  
  }
#endif   /* __TURBOC__ or not */

#else
/* Assume Linux */
/* as of 12/14/2008 GLF these are not yet defined for Linux */

#endif

