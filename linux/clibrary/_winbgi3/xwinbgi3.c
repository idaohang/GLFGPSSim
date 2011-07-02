
/*
 xwinbgi3.c -- Borland Graphics Interface (BGI) cross-platform substitute for original Borland 
               GRAPHICS.LIB component of Turbo C and Turbo Pascal for DOS.  This library is valuable for
               porting code from old Turbo C programs, and versions have been built for Turbo C (2.01-DOS),
               GCC 3 (Mingw32-Windows), and most recently GCC 4 (Ubuntu Linux and X Windows).
               the file "xwinbgi3.c" is intended to be #included by a wrapper and conditionals are only used 
               to briefly verify the target OS, since X code is only meaningful in a Unix-like context.

               Code adapted (11 Dec 2009) by Gary L Flispart (GLF) from WinBGIM and Xbgi packages
               developed pre-2003 by Konstantin Knizhnik.  Xbgi is included as part of the PtoC 
               Pascal-C converter package from Konstantin Knizhnik's website and separately from 
               Taiji Yamada website.  The WinBGI and WinBGIM packages are found at many open source sites.

               From Konstantin Knizhnik'S PtoC README.TXT, homepage http://www.garret.ru/:

                  " PtoC now provides emulation libraries of Borland Graphics Interface (BGI) 
                    for X-Windows and Windows-95/NT are included in this distribution (BGI emulators 
                    can be also used without converter for C programs using BGI). I found source code 
                    of BGI emulator for X-Windows in Internet, so I only have to do some changes and 
                    fix few bugs. Unfortunately this emulation library is not fully completed and 
                    tested, also not all BGI functionality is supported. And BGI emulator for MS-Windows 
                    I created myself (in Internet I found only commercial products). I called this 
                    library WinBGI. " 

               While developing the GLF Programming Envioronment, a custom (private) distribution
               of MinGW32, Dev-C++, and various add-on libraries, GLF adapted WinBGIM as a portability
               library to port old GLF code to GCC.  The X variant was found in 2009 and adapted into
               the portability suite.

               GLF (lightly) traced the code's origin, and a rough history follows:

               1991-1998 Konstantin Knizhnik developed X Windows and Windows 95 variants of
                         BGI emulation -- X Windows variant callled Xbgi, Windows variant WinBGI

               1994-2009 Taiji Yamada <taiji@aihara.co.jp> improvements and patches to Xbgi -- from webpage:

                         " In this version, an improvement at event loop of xbgi, the bug-fixed convenience 
                           function to save graphics to XPM image format, the bug-fixed PostScript driver 
                           of device coordinates, and some improvements in the superiour library to BGI,  
                           are achieved. Therefore, this version is recomented for not only new users but 
                           also the others already using an old one. "
  
               1998-2005 Various authors adapted WinBGI, leading to WinBGIM, a variant optimized for
                         compilation under MinGW32 compiler under Windows -- last known maintainer
                         was Michael Main at the University of Colorado (possibly obsolete website):
                         http://www.cs.colorado.edu/~main/cs1300/doc/bgi/bgi.html

               2004-2009 GLF adapted WinBGIM into winbgi2 library (some optimizations and code fixes)
                         Used successfully as a portability library (mostly the header) between
                         Turbo C 2.01 and MinGW (compiled from source using GCC 3.4.5 and Dev-C++)

                         -- renamed composite source file winbgi2.cpp and header renamed graphic2.h
                         -- conditionals in header support either Turbo C (DOS) and MinGW32 directly
                         -- by copying Borland BGI library GRAPHICS.LIB to winbgi2.lib and using the new 
                            graphic2.h, projects can be rendered source-portable. Wrapper around
                            initgraph() function adds 2 parameters for window width and height to make
                            Borland's initgraph() compatible with WinBGI equivalent function, which needs
                            to set a window size (DOS assumed full screen at specified driver resolution).

                         This package supports all functions found in the BGIDEMO.C program delivered with 
                         TC 2.01, slightly modified to handle the revised API, when run on Windows 9x/2K/XP.
                        
               2009      GLF found Xbgi code (old) on web in PtoC converter package and adapted code to work 
                         under Ubuntu Linux -- immediately obvious the original developer of WinBGI did 
                         the X version earlier or at the same time, and the code was 99% identical at 
                         higher levels.  The original Xbgi code was mainly C, not C++ as was WinBGIM
                         (GLF prefers C).  Initial testing under Ubuntu was frustrating due to a few bugs 
                         and puzzling side effects (may have worked fine under a diffewrent compiler, 
                         but the latest GCC 4 version complained).  When all the C code compiled, it still 
                         didn't work at first.  GLF had no prior X experience and started a week-long 
                         learning curve which included finding tutorial sample code which compiled but 
                         wouldn't display properly until modified.  The upshot:  it seems under GUIs, 
                         simple X Windows programs need some kind of event loop to show their windows --
                         maybe under full screen older variants the event loop was optional, but not 
                         anymore.  GLF hybridized the tutorial code with the Xbgi code to allow the 
                         screens to display, but the BGI code is mostly from the original source.  

                         Later, GLF found updated Xbgi by Taiji Yamada, and merged code into the adapted 
                         old Xbgi code, and combined all source modules into a single xwinbgi3.c file.  
                         GLF renamed the header graphic3.h, which now supports conditionals for all 3 OS's 
                         (TC-DOS, MinGW32-Windows, GCC-Ubuntu Linux).  New functions to support X event 
                         loops and melded some WinBGIM code into Xbgi to implement functions not supported 
                         in older version.
                
               GLF considers the WinBGI and Xbgi products to be important, excellent work and essential
               libraries for porting old Borland code.  GLF thanks Konstantin Knizhnik, Taiji Yamada and
               subsequent maintainers and popularizers of this code for their valuable contributions to 
               Open Source.

*/

/*

               From the WinBGI Readme.txt file:

This package contains emulation library of Borland Graphics Interface (BGI) 
for Windows-95/NT. This library strictly emulates most of BGI functions
(except using of non-standard drivers). Also may be mapping of fonts
is not correct. But as far as sources are also available, you can 
easily customize them for your application. Unfortunately direct work
with palette colors (setpalette, setbkcolor, write and putimage modes other 
than COPYPUT) is supported only for 256-colors Windows mode.
Also I have used this library for only few programs (bgidemo is
certainly the most complex one) so I can't guaranty that all
functions always work properly. I am also sorry for the lack of 
parameter checking in WinBGI functions. So summarizing all above:

WinBGI advantages: 
1) Allows you to run your old Turbo-C DOS applications in 32-bit mode
   in normal windows. So you can easily overcome all 64Kb limitations
   and getting 32-bit application by simple recompilation !

2) Graphics is much faster with WinBGI (because native Win32 API
   is used with minimal emulation overhead) in comparison with
   original application running in DOS session under Windows 
   (especially at my PPro-200 with NT). 
   Also it seems to me that some things (like switching of graphical 
   pages) are not working properly in DOS mode under Windows-NT.

3) You can use WinBGI for creating non-event driven graphical applications.
   For example if you want to write a program which only draws
   graphic of functions, it is not so easy to do with windows.
   You have to handle REDRAW messages, create timers to output next
   graphics iteration... It seems to me that BGI is much more 
   comfortable for this purposes: you just draw lines or points and do
   not worry about window system at all...

WinBGI shortcomings:
1) Handling of Windows events is done in BGI functions kbhit(). getch() 
   and delay(). So to make your application work properly You should
   periodically call one of this functions. For example,  
   the following program will not work with WinBGI:

   	initgraph(&hd, &hm, NULL);
   	while (1) putpixel(random(640), random(480), random(16));
   	closegraph();
  
   Correct version of this program is:

   	initgraph(&hd, &hm, NULL);
   	while (!kbhit()) putpixel(random(640), random(480), random(16));
   	closegraph();

2) To handle REDRAW message WinBGI has to perform drawing twice:  
   at the screen and in the pixmap which can be used while redrawing.
   I find that speed of drawing is still very fast but if you want to 
   make it even faster you can assign 0 to global variable 
   "bgiemu_handle_redraw". In this case drawing is performed only at
   the screen but correct redrawing is not possible. If your application
   makes some kind of animation (constantly updates pictures at the screen) 
   then may be storing image in the pixmap is not necessary, because your
   application will draw new picture instead of old one. 

3) Work with palettes is possible only in 256-colors Windows mode. 
   I don't know how to solve this problem with Win32 
   (I am not going to use DirectX).  

4) It is still not so good tested and not all BGI functionality 
   is precisely simulated. I am hope that current version of WinBGI
   can satisfy requirements of most simple Turbo-C graphics applications. 

By default WinBGI emulates VGA device with VGAHI (640x480) mode.
Default mode parameter can be changed using "bgiemu_default_mode"
variable. Special new mode VGAMAX is supported by WinBGI, causing
creation of maximized window. To use this mode you should either
change value of "bgiemu_default_mode" variable to VGAMAX and specify
DETECT device type, or specify VGA device type and VGAMAX mode.

I am using Microsoft Visual C++ 5.0 to compile this application.
To build library and BGIDEMO example you should only issue command "nmake". 
As a result you will have library "winbgi.lib", header file "graphics.h"
which you can put in any place you want and bgidemo.exe - example of using 
WinBGI with original Borland BGIDEMO sample from BCC 4.5, 


WinBGI is shareware. You can do everything you want with this library. 
Certainly I can give you absolutely no warranty but I will be glad
to answer all your questions and fix bugs which you may be find in WinBGI...

My e-mail address:
knizhnik@cecmow.enet.dec.com

*/


/* verify this is Linux, else include nothing useful */

#if !defined(__TURBOC__) && !defined(__MINGW32__)

/* assume Linux and X */

/* ----------- all includes for monolithic source file ------------------ */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "xwinlite.h"
#include "graphic3.h"



/* -------------- Global defs and data ------------------------------------------ */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifdef DEBUG_XWINLITE
extern FILE *outdebug;
#endif



/* The following global variables manage the main window's emulation of BGI */

Xwin *mainXwin = NULL;    /* user should not ordinarily reference this -- made external only for debugging */

XWindowAttributes mainwinattr;

palettetype mainBGIpalette;

unsigned char testuserpatternbrush1[8] = {
 0x80, 0x80, 0x80, 0xFF, 0x08, 0x08, 0x08, 0xFF
};

int   userbrushno = 0;
int   userfillcolor = 0;

int   bgiemu_handle_redraw;

int   aspectratio_x;
int   aspectratio_y;


/* support for BGI viewports */
XPoint CP;			/* the current drawing position */
XPoint VPorigin;		/* the origin of the current viewport */
XRectangle xrectangle;

viewporttype view_settings;
linesettingstype line_settings;

int   BGIbackgcolor = 0;
int   BGIforegcolor = 15;

textsettingstype txt_settings;
fillsettingstype fill_settings;

bgi_info *vga_palette;

arccoordstype last_arc;

	/* user defined fill patterns */

/* Pixmap fill_pattern;  */
	/* used for clipping */

/* Colormap cmap; */

/*
unsigned long plane_mask[4];
unsigned long plane_masks;
*/



#ifdef __cplusplus
extern "C" {
#endif




/* -------------------- X Window support ---------------------------------------------- */

/* GLF 1/7/2010 -- convert winBGI to use calls to xwinlite.h functions */

/* -------------------- Publicly visible event handler interface ---------------------- */

/* Original programs written to use the BGI API would have a limited interface to control
   event loops, especially kbhit(), getch(), and delay() which are implemented within the WinBGI 
   library to emulate and extend the original <conio.h> and <dos.h> Borland functions.  Since 
   these must be called many times per second to watch for keystrokes, they represent the ideal 
   insertion points for code to dispatch and process background event handlers.  The original 
   Xbgi code lacked "exposure" event handling, and needed considerable revision to work correctly 
   in windowed mode under Ubuntu Linux (9.04 tested). -- (GLF 12 Dec 2009) 

   The background event handlers are limited to the BGI-invisible functions of redrawing the
   screen and other menial housekeeping tasks to make X Windows mimic the DOS BGI graphics.
   BGI user event loops should process kbhit() and getch() as usual for DOS programs.
*/

/* Somewhere in the BGI main program...
 
    do
      {
       while (!kbhit())     -- wait indefinitely for a keystroke (run background event loop) ...
         {
          -- if desired, can add additional high-speed user event processing here (don't dawdle) --
         }
       kar = getch();       -- then acquire a single keypress
      }
       while (kar != KEY_ESCAPE);  -- loops continuously until ESC hit --


  -- or possibly --

       {    -- (user event loop) --
  
        if (kbhit())         -- one pass of background event loop -- 
          {
           kar = getch();       -- then acquire a single keypress
          }  

        -- more user event loop stuff --
       }

*/


/* test if key is in buffer without waiting for a keypress */
int kbhit(void)
{
 return kbhit_Xwin(mainXwin, 1);	/* contains one pass through default eventloop */
}




int getch(void)  /* get next key in buffer -- if empty, waits until key (or KB_QUIT) is in buffer */
{
 int kar;

/* if kbhit() says there is a key, getch() is queued up, otherwise wait until there IS a key */
 while (!kbhit())  
   {
    /* NOTE: event loop processing (essential) occurs here */
   }
 return getch_Xwin(mainXwin);
}



void delay(int msec)
{
 delay_Xwin(mainXwin, msec);
}



/* -------------------- General support functions ------------------------------------- */


void _graphfreemem(void *ptr, unsigned int size)
{
 /* 
  * This routine not currently implemented as a user hook -- just calls free()
  */

 free(ptr);
}


/*
 * $Id: _graphgetmem.c,v 0.1 1993/12/09 23:59:27 king Exp king $
 * User hook into graphics memory allocation.
 *
 * $Log: _graphgetmem.c,v $
 * Revision 0.1  1993/12/09  23:59:27  king
 * Initial version.  __graphgetmem not currently implemented.
 *
 */


void *_graphgetmem(unsigned int size)
{
 /* 
  * This routine not currently implemented as a user hook -- just calls malloc()
  */
 return malloc(size);
}


char *itoa(int value, char *string, int radix)
  {
   /* quick hack -- only works for certain radix values (10,16) */
   switch (radix)
     {
      case 16:
        { 
         sprintf(string,"%X",value);
         break;
        }
      default:
        {
         sprintf(string,"%d",value);
        }
     }
   return string;
  } 
 



#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


/* ---------------------------------------------------------------------- */


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


/* ---------------------------------------------------------------------- */

/* A structure holding current magnification and bounding box padding */

/* ---------------------------------------------------------------------- */


static char *my_strdup();
static char *my_strtok();


static XImage *MakeXImage();

/* ------------------------------------------------------------------------------------ */

/* translate a direct reference to color number into an indirect reference to a palette color */
int BGIcolor(int color)
{
 /* get a color number from BGI palette table */
 if ((color >= 0) && (color < mainBGIpalette.size))
   {
    return mainBGIpalette.colors[color];
   }
}


void BGIpalette_init(void)
{
 int   i;

 /* applies only to VGAHI */
 mainBGIpalette.size = 16;
 for (i = 0; i < mainBGIpalette.size; i++)
   {
    mainBGIpalette.colors[i] = i;
   }
}


int convert_pixel_to_BGIcolor(unsigned long pixel)
  {
   int i;
   int j;

   /* find the color number in the xwinlite color list which FIRST matches the pixel value */
   /* works only for VGAHI */
   for (i=0; i<16; i++)
     { 
      if (pixel == mainXwin->palette[i].pixel)
        {
         /* i now == the actual color index, but may or may not be the BGI palette index */
         for (j=0; j<mainBGIpalette.size; j++)
           {
            if (mainBGIpalette.colors[j] == i)
              {
               return j;
              } 
           }      

         /* if the above doesn't find the color, for now return the actual color index */  
         return i;
        }
     }

   /* if the above doesn't find the color, return something within legal range */
   return 0;
  }


/* -------------------- Specific BGI API support -- implemented ----------------------- */

/*
 * $Id: initgraph.c,v 0.12 1994/04/07 00:48:00 king Exp king $
 * Initializes the graphics system.
 *
 * $Log: initgraph.c,v $
 * Revision 0.12  1994/04/07  00:48:00  king
 * Changed a few things to support the use of the backing store for X11R5
 * and higher.
 *
 * Revision 0.11  1993/12/13  23:46:03  king
 * Made changes for graphicsdriver and graphicsmode, which now also help
 * determine the window size.
 *
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *

 * Rewritten 1/7/2010 GLF to use xwinlite calls  
 */

void initgraph(int *graphdriver, int *graphmode, char const *pathtodriver, int wid, int hgt)
{
 int   gd;
 int   gm;
 int   width;
 int   height;

 /* create a window based on the parameters passed to initgraph() */
 /* NOTE: initially all parameters will be ignored and a VGAHI 640x480 screen is created --
    however, retain code to derive correct emulations for later expansion */
 gd = *graphdriver;
 gm = *graphmode;
 switch (gd)
   {
    case CGA:
      {
       height = 200;
       switch (gm)
	 {
	  case CGAC0:
	  case CGAC1:
	  case CGAC2:
	  case CGAC3:
	    {
	     width = 320;
	     break;
	    }
	  case CGAHI:
	    {
	     width = 640;
	     break;
	    }
	  default:
	   width = 320;
	   break;
	 }
       break;
      }
    case MCGA:
      {
       height = 200;
       switch (gm)
	 {
	  case MCGAC0:
	  case MCGAC1:
	  case MCGAC2:
	  case MCGAC3:
	    {
	     width = 320;
	     break;
	    }
	  case MCGAMED:
	    {
	     width = 640;
	     break;
	    }
	  case MCGAHI:
	    {
	     width = 640;
	     height = 480;
	     break;
	    }
	  default:
	    {
	     width = 320;
	     break;
	    }
	 }
       break;
      }
    case EGA:
      {
       width = 640;
       switch (gm)
	 {
	  case EGALO:
	    {
	     height = 200;
	     break;
	    }
	  case EGAHI:
	    {
	     height = 350;
	     break;
	    }
	  default:
	    {
	     height = 350;
	     break;
	    }
	 }
       break;
      }
    case EGA64:
      {
       width = 640;
       switch (gm)
	 {
	  case EGA64LO:
	    {
	     height = 200;
	     break;
	    }
	  case EGA64HI:
	    {
	     height = 350;
	     break;
	    }
	  default:
	    {
	     height = 350;
	     break;
	    }
	 }
       break;
      }
    case EGAMONO:
      {
       width = 640;
       height = 350;
       break;
      }
    case HERCMONO:
      {
       width = 720;
       height = 348;
       break;
      }
    case ATT400:
      {
       height = 200;
       switch (gm)
	 {
	  case ATT400C0:
	  case ATT400C1:
	  case ATT400C2:
	  case ATT400C3:
	    {
	     width = 320;
	     break;
	    }
	  case ATT400MED:
	    {
	     width = 640;
	     break;
	    }
	  case ATT400HI:
	    {
	     width = 640;
	     height = 400;
	     break;
	    }
	  default:
	    {
	     width = 320;
	     break;
	    }
	 }
       break;
      }
    case VGA:
      {
       width = 640;
       switch (gm)
	 {
	  case VGALO:
	    {
	     height = 200;
	     break;
	    }
	  case VGAMED:
	    {
	     height = 350;
	     break;
	    }
	  case VGAHI:
	    {
	     height = 480;
	     break;
	    }
	  default:
	    {
	     height = 480;
	     break;
	    }
	 }
       break;
      }
    case PC3270:
      {
       width = 720;
       height = 350;
       break;
      }
    case IBM8514:
      {
       switch (gm)
	 {
	  case IBM8514LO:
	    {
	     width = 640;
	     height = 480;
	     break;
	    }
	  case IBM8514HI:
	    {
	     width = 1024;
	     height = 768;
	     break;
	    }
	  default:
	    {
	     width = 1024;
	     height = 768;
	     break;
	    }
	 }
       break;
      }
    case DETECT:
      {
       gm = X11_VGA;
      }
    case X11:
      {
       switch (gm)
	 {
	  case X11_CGALO:
	    {
	     width = 320;
	     height = 200;
	     break;
	    }
	  case X11_CGAHI:
	    {
	     width = 640;
	     height = 200;
	     break;
	    }
	  case X11_EGA:
	    {
	     width = 640;
	     height = 350;
	     break;
	    }
	  case X11_VGA:
	    {
	     width = 640;
	     height = 480;
	     break;
	    }
	  case X11_HERC:
	    {
	     width = 720;
	     height = 348;
	     break;
	    }
	  case X11_PC3270:
	    {
	     width = 720;
	     height = 350;
	     break;
	    }
	  case X11_SVGALO:
	    {
	     width = 800;
	     height = 600;
	     break;
	    }
	  case X11_SVGAMED1:
	    {
	     width = 1024;
	     height = 768;
	     break;
	    }
	  case X11_SVGAMED2:
	    {
	     width = 1152;
	     height = 900;
	     break;
	    }
	  case X11_SVGAHI:
	    {
	     width = 1280;
	     height = 1024;
	     break;
	    }
	  case X11_USER:
	  default:
	    {
	     break;
	    }
	 }
      }
    default:
      {
       break;
      }
   }

 /* valid for VGAHI only */
 BGIpalette_init();
 BGIbackgcolor = 0;
 BGIforegcolor = 15;
 width = 640;
 height = 480;

 mainXwin = create_main_Xwin("BGI Graphics Window", width, height, 0, 16, 0, None);

 load_fontlist_Xwin(mainXwin, XFonts);	/* load default xwinlite (BGI) fonts */
 userbrushno = add_brush_Xwin(mainXwin, testuserpatternbrush1);

 /* set the default text settings */
 style_set_Xwin(mainXwin, XWIN_DEFAULT, 0, 1, WHITE, BLACK, XWIN_DEFAULT);

 aspectratio_x = 10000;
 aspectratio_y = 10000;

 /* set the default viewport */

/* visible_all_Xwin(mainXwin);  */
 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/

 /* set default viewport to full window */
/*  visible_all_Xwin(mainXwin); */

 xrectangle.x = 0; 
 xrectangle.y = 0; 
 xrectangle.width = width;
 xrectangle.height = height; 
  
 view_settings.left = 0;
 view_settings.top = 0;
 view_settings.right = width - 1;
 view_settings.bottom = height - 1;
 view_settings.clip = 0;
 
 VPorigin.x = 0;
 VPorigin.y = 0;

 CP.x = 0;
 CP.y = 0;
 
}


/* ------------------------------------------------------------------------------------ */

/*
 * $Id: closegraph.c,v 0.1 1993/12/10 00:10:55 king Exp king $
 * Shuts down the graphics system.
 *
 * $Log: closegraph.c,v $
 * Revision 0.1  1993/12/10  00:10:55  king
 * Initial version.
 *
 */

void closegraph(void)
{
/* visible_all_Xwin(mainXwin); */
 finish_Xwin(mainXwin);
}


/*
 * $Id: detectgraph.c,v 0.1 1993/12/10 00:11:18 king Exp king $
 * Determines the graphics driver and graphics mode to use by checking the
 * hardware.
 *
 * $Log: detectgraph.c,v $
 * Revision 0.1  1993/12/10  00:11:18  king
 * Initial version.
 *
 */

void detectgraph(int *graphdriver, int *graphmode)
{
 *graphdriver = VGA;
 *graphmode = VGAHI;
}


/* --------------------------------------------------------------------------------- */

/*
 * $Id: arc.c,v 0.11 1993/12/16 00:20:57 king Exp king $
 * Draws an arc.
 *
 * $Log: arc.c,v $
 * Revision 0.11  1993/12/16  00:20:57  king
 * Fixed bug with endstangle not being relative to stangle.  Thanks to Tero and
 * Juki for this one.
 *
 * Revision 0.1  1993/12/10  00:03:16  king
 * Initial version.
 *
 */

void arc(int x, int y, int stangle, int endangle, int radius)
{
 int   X;
 int   Y;
 int   angle1;
 int   angle2;
 float radian1;
 float radian2;
 int   diameter;

 X = x - radius + VPorigin.x;
 Y = y - radius + VPorigin.y;
 angle1 = stangle * 64;		/* angles specified in 64-ths */
 angle2 = endangle * 64;	/* of a degree in X11 */
 angle2 -= angle1;
 radian1 = (float) stangle / 180.0 * M_PI;
 radian2 = (float) endangle / 180.0 * M_PI;
 diameter = 2 * radius;

 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

 XDrawArc(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc, X, Y,
	  diameter, diameter, angle1, angle2);

 last_arc.x = x;
 last_arc.y = y;
 last_arc.xstart = x + radius * cos(radian1);
 last_arc.ystart = y - radius * sin(radian1);
 last_arc.xend = x + radius * cos(radian2);
 last_arc.yend = y - radius * sin(radian2);

 CP.x = last_arc.xend;
 CP.y = last_arc.yend;

 XDrawArc(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc, X, Y, diameter, diameter, angle1, angle2);


 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}


/*
 * $Id: bar.c,v 0.1 1993/12/10 00:03:42 king Exp king $
 * Draws a two-dimensional bar.
 *
 * $Log: bar.c,v $
 * Revision 0.1  1993/12/10  00:03:42  king
 * Initial version.
 *
 * GLF 12 Dec 2009 -- algorithm verified equivalent to WinBGIM 
 */

void bar(int left, int top, int right, int bottom)
{
 int   X;
 int   Y;
 int   temp;
 int   wid;
 int   hgt;

 if (left > right)		/* Turbo C corrects for badly ordered corners */
   {
    temp = left;
    left = right;
    right = temp;
   }
 if (bottom < top)		/* Turbo C corrects for badly ordered corners */
   {
    temp = bottom;
    bottom = top;
    top = temp;
   }

 /* fill color is selected by choice of mainXwin->virtualgc */
 X = left + VPorigin.x + 1;
 Y = top + VPorigin.y + 1;
 wid = (right - left) /* + 1 */;
 hgt = (bottom - top) /* + 1 */;

 style_set_Xwin(mainXwin, -1, -1, -1, fill_settings.color, -1, fill_settings.pattern);

 fillrect_Xwin(mainXwin,X,Y,wid,hgt);

 
 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}


/*
 * $Id: bar3d.c,v 0.1 1993/12/10 00:04:05 king Exp king $
 * Draws a three-dimensional bar.
 *
 * $Log: bar3d.c,v $
 * Revision 0.1  1993/12/10  00:04:05  king
 * Initial version.
 *
 * GLF 12 Dec 2009 -- algorithm appears equivalent to WinBGIM 
 */


void bar3d(int left, int top, int right, int bottom, int depth, int topflag)
{
 int   X;
 int   Y;
 int   temp;
 int   wid;
 int   hgt;
 XPoint *points;
 XPoint *xptr;
 int   i;

 if (left > right)		/* Turbo C corrects for badly ordered corners */
   {
    temp = left;
    left = right;
    right = temp;
   }
 if (bottom < top)		/* Turbo C corrects for badly ordered corners */
   {
    temp = bottom;
    bottom = top;
    top = temp;
   }


 X = left + VPorigin.x;
 Y = top + VPorigin.y;
 wid = right - left + 1;
 hgt = bottom - top + 1;

 style_set_Xwin(mainXwin, -1, -1, -1, fill_settings.color, -1, fill_settings.pattern);

 XFillRectangle(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page],
		mainXwin->virtualgc, X, Y, wid + 1, hgt + 1);

 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

 XDrawRectangle(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc, X, Y, wid, hgt);

 points = (XPoint *) malloc(4 * sizeof(XPoint));	/* 4 per lozenge */
 for (i = 0, xptr = points; i < 4; i++, xptr++)
   {
    switch (i)
      {
       case 0:
	 {
	  xptr->x = right + VPorigin.x;
	  xptr->y = bottom + VPorigin.y;
	  break;
	 }
       case 1:
	 {
	  xptr->x = depth;
	  xptr->y = -depth;
	  break;
	 }
       case 2:
	 {
	  xptr->x = 0;
	  xptr->y = -hgt;
	  break;
	 }
       case 3:
	 {
	  xptr->x = -depth;
	  xptr->y = depth;
	  break;
	 }
       default:
	 {
	  break;
	 }
      }
   }

 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

 XDrawLines(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc,
	    points, 4, CoordModePrevious);

 free(points);

 if (topflag)
   {
    points = (XPoint *) malloc(3 * sizeof(XPoint));
    for (i = 0, xptr = points; i < 3; i++, xptr++)
      {
       switch (i)
	 {
	  case 0:
	    {
	     xptr->x = left + VPorigin.x;
	     xptr->y = top + VPorigin.y;
	     break;
	    }
	  case 1:
	    {
	     xptr->x = depth;
	     xptr->y = -depth;
	     break;
	    }
	  case 2:
	    {
	     xptr->x = wid;
	     xptr->y = 0;
	     break;
	    }
	  default:
	    {
	     break;
	    }
	 }
      }

    style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

    XDrawLines(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc,
	       points, 3, CoordModePrevious);
    free(points);
   }

 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}


/*
 * $Id: circle.c,v 0.1 1993/12/10 00:04:25 king Exp king $
 * Draws a circle of the given radius with its centre at (x, y).
 *
 * $Log: circle.c,v $
 * Revision 0.1  1993/12/10  00:04:25  king
 * Initial version.
 *
 * GLF 12 Dec 2009 -- algorithm appears equivalent to WinBGIM 
 */

void circle(int x, int y, int radius)
{
 int   X;
 int   Y;
 int   angle;
 int   diameter;
 unsigned rx = radius;
 unsigned ry = rx * aspectratio_x / aspectratio_y;

 X = x - rx + VPorigin.x;
 Y = y - ry + VPorigin.y;
 angle = 360 * 64;

 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

 XDrawArc(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc, X, Y,
	  2 * rx, 2 * ry, 0, angle);
 CP.x = x + rx;
 CP.y = y;


 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}


/*
 * $Id: cleardevice.c,v 0.1 1993/12/10 00:10:02 king Exp king $
 * Clears the graphics screen.
 *
 * $Log: cleardevice.c,v $
 * Revision 0.1  1993/12/10  00:10:02  king
 * Initial version.
 *
 * GLF 12 Dec 2009 -- algorithm appears equivalent to WinBGIM 
 */

void cleardevice(void)
{
 /* reset the viewport to main screen */
/*
 xrectangle.x = 0; 
 xrectangle.y = 0; 
 xrectangle.width = mainXwin->width;
 xrectangle.height = mainXwin->height; 
  
 view_settings.left = 0;
 view_settings.top = 0;
 view_settings.right = mainXwin->width - 1;
 view_settings.bottom = mainXwin->height - 1;
 view_settings.clip = 0;
 
 VPorigin.x = 0;
 VPorigin.y = 0;

 CP.x = 0;
 CP.y = 0;


 clearrect_Xwin(mainXwin,0,0,mainXwin->width,mainXwin->height);

 flushbuffers_Xwin(mainXwin); 
*/

/*
 XSync(mainXwin->display,False);
*/
/*
 repaint_Xwin(mainXwin); 
*/
        XWindowAttributes window_attributes_return;
        int width;
        int height;
        GC temp_gc;
        XGCValues current_values;
        XGCValues temp_values;

        XGetWindowAttributes(mainXwin->display, mainXwin->window, &window_attributes_return);
        width = window_attributes_return.width;
        height = window_attributes_return.height;

        XGetGCValues(mainXwin->display, mainXwin->virtualgc, GCBackground, &current_values);

        temp_values.fill_style = FillSolid;
        temp_values.foreground = current_values.background;
        temp_gc = XCreateGC(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page],
                                                    GCForeground, &temp_values);

        XFillRectangle(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], 
                                                   temp_gc, 0, 0, width, height);

/*
	if (visual_page == active_page)
		XFillRectangle(mainXwin->display, mainXwin->window, temp_gc, 0, 0, width, height);
*/
        XFreeGC(mainXwin->display, temp_gc);

        CP.x = 0;                 /* set the current drawing position to HOME */
        CP.y = 0;

	XFlush(mainXwin->display);
 repaint_Xwin(mainXwin); 
}


/*
 * $Id: clearviewport.c,v 0.1 1993/12/10 00:10:23 king Exp king $
 * Clears the current viewport.
 *
 * $Log: clearviewport.c,v $
 * Revision 0.1  1993/12/10  00:10:23  king
 * Initial version.
 *
 * GLF 12 Dec 2009 -- algorithm appears equivalent to WinBGIM 
 */

void clearviewport(void)
{



/*
 CP.x = 0;
 CP.y = 0;

 clearrect_Xwin(mainXwin,VPorigin.x, VPorigin.y, xrectangle.width, xrectangle.height);

 flushbuffers_Xwin(mainXwin); 
*/
/*
 repaint_Xwin(mainXwin); 
*/
/*
 XSync(mainXwin->display,False);
*/

        XWindowAttributes window_attributes_return;
        int width;
        int height;
        GC temp_gc;
        XGCValues current_values;
        XGCValues temp_values;

/*
        XGetWindowAttributes(mainXwin->display, mainXwin->window, &window_attributes_return);
        width = window_attributes_return.width;
        height = window_attributes_return.height;
*/

        XGetGCValues(mainXwin->display, mainXwin->virtualgc, GCBackground, &current_values);

        temp_values.fill_style = FillSolid;
        temp_values.foreground = current_values.background;
        temp_gc = XCreateGC(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page],
                                                    GCForeground, &temp_values);

        XFillRectangle(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], 
                        temp_gc, view_settings.left, view_settings.top, 
                        (view_settings.right-view_settings.left)+1, 
                        (view_settings.bottom-view_settings.top)+1);

        XFreeGC(mainXwin->display, temp_gc);

        CP.x = 0;                 /* set the current drawing position to HOME */
        CP.y = 0;

	XFlush(mainXwin->display);

 repaint_Xwin(mainXwin); 


}


/*
 * $Id: drawpoly.c,v 0.1 1993/12/10 00:11:48 king Exp king $
 * Draws the outline of a polygon.
 *
 * $Log: drawpoly.c,v $
 * Revision 0.1  1993/12/10  00:11:48  king
 * Initial version.
 *
 * GLF 12 Dec 2009 -- algorithm appears equivalent to WinBGIM 
 */

void drawpoly(int numpoints, int *polypoints)
{
 XPoint *points;
 XPoint *xptr;
 int   i;

 points = (XPoint *) malloc(numpoints * sizeof(XPoint));

 for (i = 0, xptr = points; i < numpoints; i++, xptr++)
   {
    xptr->x = *(polypoints++) + VPorigin.x;
    xptr->y = *(polypoints++) + VPorigin.y;
   }

 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

 XDrawLines(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc,
	    points, numpoints, CoordModeOrigin);
 free(points);

 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}

/*
 * $Id: ellipse.c,v 0.1 1993/12/10 00:12:23 king Exp king $
 * Draws an elliptical arc.
 *
 * $Log: ellipse.c,v $
 * Revision 0.1  1993/12/10  00:12:23  king
 * Initial version.
 *
 */


void ellipse(int x, int y, int stangle, int endangle, int xradius, int yradius)
{
 int   X;
 int   Y;
 int   angle1;
 int   angle2;
 float radian1;
 float radian2;
 int   minor;
 int   major;

 X = x - xradius + VPorigin.x;
 Y = y - yradius + VPorigin.y;
 angle1 = stangle * 64;		/* angles specified in 64-ths */
 angle2 = endangle * 64;	/* of a degree in X11 */
 angle2 -= angle1;
 radian1 = (float) stangle / 180.0 * M_PI;
 radian2 = (float) endangle / 180.0 * M_PI;
 minor = 2 * yradius;
 major = 2 * xradius;

 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

 XDrawArc(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc, X, Y,
	  major, minor, angle1, angle2);

 last_arc.x = x;
 last_arc.y = y;
 last_arc.xstart = x + xradius * cos(radian1);
 last_arc.ystart = y - yradius * sin(radian1);
 last_arc.xend = x + xradius * cos(radian2);
 last_arc.ystart = y - yradius * sin(radian2);

 CP.x = last_arc.xend;
 CP.y = last_arc.yend;


 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}

/*
 * $Id: fillellipse.c,v 0.1 1993/12/10 00:12:36 king Exp king $
 * Draws and fills an ellipse.
 *
 * $Log: fillellipse.c,v $
 * Revision 0.1  1993/12/10  00:12:36  king
 * Initial version.
 *
 */

void fillellipse(int x, int y, int xradius, int yradius)
{
 int   X;
 int   Y;
 int   angle;
 int   xdiameter;
 int   ydiameter;

 X = x - xradius + VPorigin.x;
 Y = y - yradius + VPorigin.y;
 angle = 360 * 64;
 xdiameter = 2 * xradius;
 ydiameter = 2 * yradius;

 style_set_Xwin(mainXwin, -1, -1, -1, fill_settings.color, -1, fill_settings.pattern);


 XFillArc(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page],
	  mainXwin->virtualgc, X, Y, xdiameter, ydiameter, 0, angle);

 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

 XDrawArc(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc, X, Y,
	  xdiameter, ydiameter, 0, angle);


 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}


/*
 * $Id: fillpoly.c,v 0.1 1993/12/10 00:13:00 king Exp king $
 * Draws and fills a polygon.
 *
 * $Log: fillpoly.c,v $
 * Revision 0.1  1993/12/10  00:13:00  king
 * Initial version.
 *
 */

void fillpoly(int numpoints, int *polypoints)
{
 XPoint *points;
 XPoint *xptr;
 int  *iptr;
 int   i;

 points = (XPoint *) malloc(numpoints * sizeof(XPoint));

 for (i = 0, xptr = points, iptr = polypoints; i < numpoints; i++, xptr++)
   {
    xptr->x = *(iptr++) + VPorigin.x;
    xptr->y = *(iptr++) + VPorigin.y;
   }

 style_set_Xwin(mainXwin, -1, -1, -1, fill_settings.color, -1, fill_settings.pattern);

 XFillPolygon(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page],
	      mainXwin->virtualgc, points, numpoints, Complex, CoordModeOrigin);

 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);


 free(points);

/* draw the outline of the polygon */
 points = (XPoint *) malloc((numpoints + 1) * sizeof(XPoint));

 for (i = 0, xptr = points, iptr = polypoints; i < numpoints; i++, xptr++)
   {
    xptr->x = *(iptr++) + VPorigin.x;
    xptr->y = *(iptr++) + VPorigin.y;
   }
 iptr = polypoints;
 xptr->x = *(iptr++) + VPorigin.x;
 xptr->y = *(iptr) + VPorigin.y;

 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

 XDrawLines(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc,
	    points, numpoints + 1, CoordModeOrigin);

 free(points);


 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}


/* 
 * $Id: getarccoords.c,v 0.1 1993/12/10 00:13:38 king Exp king $
 * Gets coordinates of the last call to arc().
 *
 * $Log: getarccoords.c,v $
 * Revision 0.1  1993/12/10  00:13:38  king
 * Initial version.
 *
 */

void getarccoords(arccoordstype * arccoords)
{
 memcpy(arccoords, &last_arc, sizeof(arccoordstype));
}

/*
 * $Id: getaspectratio.c,v 0.1 1993/12/10 00:13:38 king Exp king $
 * Retrieves the current graphics mode's aspect ratio.
 *
 * $Log: getaspectratio.c,v $
 * Revision 0.1  1993/12/10  00:13:38  king
 * Initial version.
 *
 */

void getaspectratio(int *xasp, int *yasp)
{
 *xasp = aspectratio_x;
 *yasp = aspectratio_y;
}


/*
 * $Id: getbkcolor.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Returns the current background colour.
 *
 * $Log: getbkcolor.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

int getbkcolor(void)
{

/* The following tries to look up the color num by scanning pixels in a list
 return mainXwin->currentbgcolor
        XGCValues values;
        bgi_info *BGIptr;
        int i;

        XGetGCValues(mainXwin->display, mainXwin->virtualgc, GCBackground, &values);
        for (i = 0, BGIptr = vga_palette; i <= MAXCOLORS; i++, BGIptr++) {
                if (values.background == BGIptr->pixel_value)
                        return BGIptr->colour_index;
        }
*/
 return BGIbackgcolor;
}

/*
 * $Id: getcolor.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Returns the current drawing colour.
 *
 * $Log: getcolor.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

int getcolor(void)
{
 /* 
    XGCValues values; bgi_info *BGIptr; int i;

    XGetGCValues(mainXwin->display, mainXwin->virtualgc, GCForeground, &values); for (i = 0, BGIptr = vga_palette; i 
    <= MAXCOLORS; i++, BGIptr++) { if (values.foreground == BGIptr->pixel_value) return
    BGIptr->colour_index; } return 0; */

 return BGIforegcolor;
}

/*
 * $Id: getdefaultpalette.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Returns the palette definition structure.
 *
 * $Log: getdefaultpalette.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

struct palettetype *getdefaultpalette(void)
{

/*
 * This routine not currently implmented.
 */
 return NULL;
}

/*
 * $Id: getdrivername.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Returns a pointer to a string containing the name of the current graphics
 * driver.
 *
 * $Log: getdrivername.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

char *getdrivername(void)
{
 return "EGAVGA";
}

/*
 * $Id: getfillpattern.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Copies a user-defined fill pattern into memory.
 *
 * $Log: getfillpattern.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

void getfillpattern(const char *pattern)
{
 XGCValues values;

 XGetGCValues(mainXwin->display, mainXwin->virtualgc, GCFillStyle, &values);

 /* the stipple pattern Pixmap returned in values must now be copied to the pattern array */
  
}

/*
 * $Id: getfillsettings.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Gets information about the current fill pattern and colour.
 *
 * $Log: getfillsettings.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

void getfillsettings(fillsettingstype * fillinfo)
{
 XGCValues values;

 XGetGCValues(mainXwin->display, mainXwin->virtualgc, GCFillStyle | GCForeground, &values);
 *fillinfo = fill_settings;
}


/*
 * $Id: getgraphmode.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Returns the current graphics mode.
 *
 * $Log: getgraphmode.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

int getgraphmode(void)
{
 return VGAHI;
}


/*
 * $Id: getimage.c,v 0.11 1993/12/16 00:06:58 king Exp king $
 * Saves a bit image of the specified region into memory.
 *
 * $Log: getimage.c,v $
 * Revision 0.11  1993/12/16  00:06:58  king
 * getimage() provided by Tero and Juki.
 *
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

void getimage(int left, int top, int right, int bottom, void *bitmap)
{
 int   X;
 int   Y;
 int   width;
 int   height;
 bgi_image *bm = (bgi_image *) bitmap;
 Pixmap pixmap;
 XGCValues op_gcv;
 GC    op_gc;

 X = left + VPorigin.x;
 Y = top + VPorigin.y;
 width = right - left + 1;
 height = bottom - top + 1;

 pixmap = XCreatePixmap(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], width, height,
			XDefaultDepth(mainXwin->display, DefaultScreen(mainXwin->display)));
 op_gc = XCreateGC(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], 0, NULL);
 XCopyGC(mainXwin->display, mainXwin->virtualgc, (GCBackground | GCForeground | GCPlaneMask), op_gc);
 op_gcv.function = GXcopy;
 XChangeGC(mainXwin->display, op_gc, GCFunction, &op_gcv);

 XCopyArea(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], pixmap,
	   op_gc, X, Y, width, height, 0, 0);
 bm->width = width;
 bm->height = height;
 bm->pixmap = pixmap;
 XFreeGC(mainXwin->display, op_gc);
}


/*
 * $Id: getlinesettings.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Gets the current line style, pattern and thickness.
 *
 * $Log: getlinesettings.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

void getlinesettings(linesettingstype * lineinfo)
{
 XGCValues values;

 XGetGCValues(mainXwin->display, mainXwin->virtualgc, GCLineStyle | GCLineWidth, &values);

 switch (values.line_style)
   {
    case LineSolid:
      {
       lineinfo->linestyle = SOLID_LINE;
       break;
      }
    case LineOnOffDash:
      {
       lineinfo->linestyle = DASHED_LINE;
       break;
      }
    default:
      {
       break;
      }
   }
 switch (values.line_width)
   {
    case 0:
    case 1:
      {
       lineinfo->thickness = NORM_WIDTH;
       break;
      }
    case 3:
      {
       lineinfo->thickness = THICK_WIDTH;
       break;
      }
    default:
      {
       lineinfo->thickness = values.line_width;
       break;
      }
   }
}

/*
 * $Id: getmaxcolor.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Returns the maximum colour value that can be passed to the setcolor()
 * function.
 *
 * $Log: getmaxcolor.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

int getmaxcolor(void)
{
 /* currently supports the colours used by EGAVGA.BGI */
 return (int) WHITE;
}

/*
 * $Id: getmaxmode.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Returns the maximum mode number for the current driver. 
 *
 * $Log: getmaxmode.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

int getmaxmode(void)
{
 return 0;
}


/*
 * $Id: getmaxx.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Returns the maximum x window coordinate.
 *
 * $Log: getmaxx.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

int getmaxx(void)
{

 if (mainXwin== NULL)
  {
   return 679;
  }

/* returns the maximum x window coordinate */
/*
 XWindowAttributes window_attributes_return;

 XGetWindowAttributes(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], &window_attributes_return);

 return window_attributes_return.width - 1;
*/
 return mainXwin->width - 1;
}


/*
 * $Id: getmaxy.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Returns the maximum y window coordinate.
 *
 * $Log: getmaxy.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

int getmaxy(void)
{
 if (mainXwin== NULL)
  {
   return 479;
  }

/* returns the maximum y window coordinate */
/*
 XWindowAttributes window_attributes_return;

 XGetWindowAttributes(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], &window_attributes_return);

 return window_attributes_return.height - 1;
*/

 return mainXwin->height - 1;
}

/*
 * $Id: getmodename.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Returns a pointer to a string containing the name of a specified graphics 
 * mode.
 *
 * $Log: getmodename.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

char *getmodename(int mode_number)
{
 char *displayname;
 static char modename[256];

 displayname = getenv("DISPLAY");
 sprintf(modename, "%d x %d %s", getmaxx(), getmaxy(),
	 displayname ? displayname : "EGAVGA");
 return modename;
}


/* 
 * $Id: getmoderange.c,v 0.1 1993/12/10 00:15:31 king Exp king $ 
 * Gets the range of modes for a given graphics driver.
 *
 * $Log: getmoderange.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

void getmoderange(int graphdriver, int *lomode, int *himode)
{

}

/*
 * $Id: getpalettesize.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Returns the size of the palette colour lookup table.
 *
 * $Log: getpalettesize.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

int getpalettesize(void)
{
 return 16;
}

/*
 * $Id: getpixel.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Gets the colour of a specified pixel.
 *
 * $Log: getpixel.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

int getpixel(int x, int y)
{
 XImage *ximage;
 unsigned long pixel;

/* This is from XBGI -- gets pixel and then converts directly to a 4-bit value:
 ximage = XGetImage(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], x, y, 1, 1, plane_masks, XYPixmap);
 pixel = XGetPixel(ximage, 0, 0);
 XDestroyImage(ximage);
 return ((pixel & plane_mask[0]) ? 1 : 0) +
       ((pixel & plane_mask[1]) ? 2 : 0) +
       ((pixel & plane_mask[2]) ? 4 : 0) + ((pixel & plane_mask[3]) ? 8 : 0);
*/

 return convert_pixel_to_BGIcolor(get_pixel_Xwin(mainXwin,x,y));
}


/*
 * $Id: gettextsettings.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Gets information about the current graphics text font.
 *
 * $Log: gettextsettings.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

void gettextsettings(textsettingstype * texttypeinfo)
{
 *texttypeinfo = txt_settings;
}


/*
 * $Id: getviewsettings.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Gets information about the current viewport.
 *
 * $Log: getviewsettings.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

void getviewsettings(viewporttype * viewport)
{
 memcpy(viewport, &view_settings, sizeof(viewporttype));
}


/*
 * $Id: getx.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Returns the current graphics position's x coordinate.
 *
 * $Log: getx.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

int getx(void)
{
 return (int) CP.x;
}


/*
 * $Id: gety.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Returns the current graphics position's y coordinate.
 *
 * $Log: gety.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

int gety(void)
{
 return (int) CP.y;
}

/*
 * $Id: graphdefaults.c,v 0.1 1993/12/10 00:37:39 king Exp king $
 * Resets all the graphics settings to their default values.
 *
 * $Log: graphdefaults.c,v $
 * Revision 0.1  1993/12/10  00:37:39  king
 * Initial version.
 *
 */

void graphdefaults(void)
{
 CP.x = 0;
 CP.y = 0;
 VPorigin.x = 0;
 VPorigin.y = 0;

 mainXwin->draw_page = 0;
 mainXwin->video_page = 0;

 txt_settings.font = 0;
 txt_settings.direction = HORIZ_DIR;
 txt_settings.charsize = 1;
 txt_settings.horiz = LEFT_TEXT;
 txt_settings.vert = TOP_TEXT;

 aspectratio_x = 10000;
 aspectratio_y = 10000;

 fill_settings.pattern = SOLID_FILL;
 fill_settings.color = WHITE;

 style_set_Xwin(mainXwin, -1, -1, -1, BLACK, WHITE, SOLID_FILL);

 /* visible_all_Xwin(mainXwin); */

 xrectangle.x = 0; 
 xrectangle.y = 0; 
 xrectangle.width = mainXwin->width;
 xrectangle.height = mainXwin->height; 
  
 view_settings.left = 0;
 view_settings.top = 0;
 view_settings.right = mainXwin->width - 1;
 view_settings.bottom = mainXwin->height - 1;
 view_settings.clip = 0;
 
 VPorigin.x = 0;
 VPorigin.y = 0;

 CP.x = 0;
 CP.y = 0;
}


/*
 * $Id: grapherrormsg.c,v 0.1 1993/12/10 00:37:39 king Exp king $
 * Returns a pointer to an error message string.
 *
 * $Log: grapherrormsg.c,v $
 * Revision 0.1  1993/12/10  00:37:39  king
 * Initial version.
 *
 */

char *grapherrormsg(int errorcode)
{
 enum graphics_errors gr_error;
 static char errormsg[100];	/* original code here [char *errormsg;] left uninitialized pointer */

 gr_error = errorcode;
 switch (gr_error)
   {
    case grOk:
      {
       strcpy(errormsg, "No error");
       break;
      }
    case grNoInitGraph:
      {
       strcpy(errormsg, "XBGI graphics not installed (use initgraph()).");
       break;
      }
    case grNotDetected:
      {
       strcpy(errormsg, "Graphics hardware not detected.");
       break;
      }
    case grFileNotFound:
      {
       strcpy(errormsg, "Device driver file not found.");
       break;
      }
    case grInvalidDriver:
      {
       strcpy(errormsg, "Invalid device driver file.");
       break;
      }
    case grNoLoadMem:
      {
       strcpy(errormsg, "Not enough memory to load driver.");
       break;
      }
    case grNoScanMem:
      {
       strcpy(errormsg, "Out of memory in scan fill.");
       break;
      }
    case grNoFloodMem:
      {
       strcpy(errormsg, "Out of memory in flood fill.");
       break;
      }
    case grFontNotFound:
      {
       strcpy(errormsg, "Font file not found.");
       break;
      }
    case grNoFontMem:
      {
       strcpy(errormsg, "Not enough memory to load font.");
       break;
      }
    case grInvalidMode:
      {
       strcpy(errormsg, "Invalid graphics mode for" "selected driver.");
       break;
      }
    case grError:
      {
       strcpy(errormsg, "Graphics error.");
       break;
      }
    case grIOerror:
      {
       strcpy(errormsg, "Graphics I/O error.");
       break;
      }
    case grInvalidFont:
      {
       strcpy(errormsg, "Invalid font file.");
       break;
      }
    case grInvalidFontNum:
      {
       strcpy(errormsg, "Invalid font number.");
       break;
      }
    case grInvalidDeviceNum:
      {
       strcpy(errormsg, "Invalid device number.");
       break;
      }
    case grInvalidVersion:
      {
       strcpy(errormsg, "Invalid version of file.");
       break;
      }
    default:
      {
       break;
      }
   }

 return (errormsg);
}


/*
 * $Id: graphresult.c,v 0.1 1993/12/10 00:37:39 king Exp king $
 * Returns an error code for the last unsuccessfull graphics operation.
 *
 * $Log: graphresult.c,v $
 * Revision 0.1  1993/12/10  00:37:39  king
 * Initial version.
 *
 */

int graphresult(void)
{
 return 0;
}


/*
 * $Id: imagesize.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Returns the number of bytes required to store a bit image.
 *
 * $Log: imagesize.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

unsigned int imagesize(int left, int top, int right, int bottom)
{
 return sizeof(bgi_image);
}



/* ------------------------------------------------------------------------------------ */


/*
 * $Id$
 * Draws a line between two specified points.
 *
 * $Log$
 */

void line(int x1, int y1, int x2, int y2)
{
 int   X1;
 int   Y1;
 int   X2;
 int   Y2;

 X1 = x1 + VPorigin.x;
 Y1 = y1 + VPorigin.y;
 X2 = x2 + VPorigin.x;
 Y2 = y2 + VPorigin.y;

 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

 XDrawLine(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc, X1, Y1, X2, Y2);


 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}


/*
 * $Id: linerel.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Draws a line a relative distance from the current position, CP.
 *
 * $Log: linerel.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void linerel(int dx, int dy)
{
 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

 XDrawLine(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc,
	   VPorigin.x + CP.x, VPorigin.y + CP.y, VPorigin.x + CP.x + dx, VPorigin.y + CP.y + dy);

 CP.x += dx;
 CP.y += dy;


 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}


/*
 * $Id: lineto.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Draws a line from the current position (CP) to (x, y).
 *
 * $Log: lineto.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void lineto(int x, int y)
{
 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

 XDrawLine(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc,
	   VPorigin.x + CP.x, VPorigin.y + CP.y, VPorigin.x + x, VPorigin.y + y);

 CP.x = x;
 CP.y = y;


 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}


/*
 * $Id: moverel.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Moves the current position (CP) a relative distance.
 *
 * $Log: moverel.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void moverel(int dx, int dy)
{
 CP.x += dx;
 CP.y += dy;
}


/*
 * $Id: moveto.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Moves the current position (CP) to (x, y).
 *
 * $Log: moveto.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void moveto(int x, int y)
{
 CP.x = x;
 CP.y = y;
}


/*
 * $Id: outtext.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Displays a string in the viewport.
 *
 * $Log: outtext.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */


void outtext(char const *textstring)
{
 int   alignment;
 float angle;
 XFontStruct *font_info;
 char *txtfont;
 int justify;
 int dir, usex, usey, usewidth;


 usewidth = scaled_textwidth_Xwin(mainXwin, textstring, strlen(textstring));

 usex = CP.x + VPorigin.x;
 usey = CP.y + VPorigin.y; 

 if (txt_settings.direction == HORIZ_DIR)
   {
    dir = 0;
    switch (txt_settings.horiz)
      {
       case LEFT_TEXT:
         {
          justify = 0;
          break;
         }
       case CENTER_TEXT:
         {
          justify = 1; 
          usex = usex - (usewidth / 2);
          break;
         }
       case RIGHT_TEXT:
         {
          justify = 2; 
          usex = usex - usewidth;
          break;
         }
       default:
         {
          justify = 0;
          break; 
         }
      }
   }
 else
   {
    dir = 1;
    switch (txt_settings.vert)
      {
       case BOTTOM_TEXT:
         {
          justify = 0;
          break;
         }
       case CENTER_TEXT:
         {
          justify = 1;
/*          usey = usey + (usewidth / 2); */
          break;
         }
       case TOP_TEXT:
         {
          justify = 2;
          usey = usey + usewidth;
          break;
         }
       default:
         {
          justify = 0;
          break;
         }
      }
   }


 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

 stringout_Xwin(mainXwin, 0, usex, usey, usewidth,textstring); 

 if ((txt_settings.horiz == LEFT_TEXT) && (txt_settings.direction == HORIZ_DIR))
   {
    CP.x += usewidth;
   }


 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}


/*
 * $Id: outtextxy.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Displays a string at a specified position.
 *
 * $Log: outtextxy.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void outtextxy(int x, int y, char const *textstring)
{
 int   alignment;
 float angle;
 XFontStruct *font_info;
 char *txtfont;
 int   X;
 int   Y;
 int dir, justify, usex, usey, usewidth;


 X = x + VPorigin.x;
 Y = y + VPorigin.y;

 usewidth = scaled_textwidth_Xwin(mainXwin, textstring, strlen(textstring));
 usex = X;
 usey = Y; 

 if (txt_settings.direction == HORIZ_DIR)
   {
    dir = 0;
    switch (txt_settings.horiz)
      {
       case LEFT_TEXT:
         {
          justify = 0;
          break;
         }
       case CENTER_TEXT:
         {
          justify = 1; 
          usex = X - (usewidth / 2);
          break;
         }
       case RIGHT_TEXT:
         {
          justify = 2; 
          usex = X - usewidth;
          break;
         }
       default:
         {
          justify = 0;
          break; 
         }
      }
   }
 else
   {
    dir = 1;
    switch (txt_settings.vert)
      {
       case BOTTOM_TEXT:
         {
          justify = 0;
          break;
         }
       case CENTER_TEXT:
         {
          justify = 1;
 /*         usey = Y + (usewidth / 2); */
          break;
         }
       case TOP_TEXT:
         {
          justify = 2;
          usey = Y + usewidth;
          break;
         }
       default:
         {
          justify = 0;
          break;
         }
      }
   }


 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);
 stringout_Xwin(mainXwin, 0, usex, usey, usewidth,textstring); 


 /* GLF -- not sure if this meets BGI specs??? */
 if ((txt_settings.horiz == LEFT_TEXT) && (txt_settings.direction == HORIZ_DIR))
   {
    CP.x += scaled_textwidth_Xwin(mainXwin, textstring, strlen(textstring));
   }


 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}




/*
 * $Id: sector.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Draws and fills in an elliptical pie slice.
 *
 * $Log: sector.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * 
 *
 */


void sector(int x, int y, int stangle, int endangle, int radiusx, int radiusy)
{
 int   X;
 int   Y;
 int   angle1;
 int   angle2;
 
 double   rangle1;
 double   rangle2;
 double  px1, py1, px2, py2; 

 int   diameterx, diametery;

 X = x - radiusx + VPorigin.x;
 Y = y - radiusy + VPorigin.y;

 rangle1 = (double)stangle;
 rangle2 = (double)endangle;

 while (rangle1 >= 360.0)
   {
    rangle1 -= 360.0;
   }

 while (rangle2 >= 360.0)
   {
    rangle2 -= 360.0;
   }

 while (rangle1 < 0.0)
   {
    rangle1 += 360.0;
   }

 while (rangle2 < 0.0)
   {
    rangle2 += 360.0;
   }

 angle1 = stangle * 64;
 angle2 = endangle * 64;
 angle2 -= angle1;

 diameterx = 2 * radiusx;
 diametery = 2 * radiusy;

 rangle1 = rangle1  / 57.29577951;
 rangle2 = rangle2  / 57.29577951;


 /* NOTE: y polarity is reverse of standard Cartesian y axis, hence -sin */
 px1 = cos(rangle1) * radiusx;
 py1 = -sin(rangle1) * radiusy;

 px2 = cos(rangle2) * radiusx;
 py2 = -sin(rangle2) * radiusy;


 /* determine the start and end points for arc -- must draw the border lines for wedge */
 
 
 style_set_Xwin(mainXwin, -1, -1, -1, fill_settings.color, -1, fill_settings.pattern);

 XFillArc(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page],
	  mainXwin->virtualgc, X, Y, diameterx, diametery, angle1, angle2);

 flushbuffers_Xwin(mainXwin);

 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

 XDrawArc(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc, X, Y,
	  diameterx, diametery, angle1, angle2);


 XDrawLine(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc,
	   X+radiusx, Y+radiusy, X+radiusx+(int)px1, Y+radiusy+(int)py1);


 XDrawLine(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc, 
	   X+radiusx, Y+radiusy, X+radiusx+(int)px2, Y+radiusy+(int)py2);


 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}



/*
 * $Id: pieslice.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Draws and fills in a pie slice.
 *
 * $Log: pieslice.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */


void pieslice(int x, int y, int stangle, int endangle, int radius)
{
 sector(x,y,stangle,endangle,radius,radius);
}



/*
 * $Id: putimage.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Outputs a bit image to the screen.
 *
 * $Log: putimage.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void putimage(int left, int top, void *bitmap, int op)
{
 int   X;
 int   Y;
 int   width;
 int   height;
 bgi_image *bm = (bgi_image *) bitmap;
 XGCValues op_gcv;
 GC    op_gc;
 Pixmap pixmap;

 width = bm->width;
 height = bm->height;
 pixmap = bm->pixmap;

 X = left + VPorigin.x;
 Y = top + VPorigin.y;

 op_gc = XCreateGC(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], 0, NULL);
 XCopyGC(mainXwin->display, mainXwin->virtualgc, (GCBackground | GCForeground | GCPlaneMask), op_gc);

 switch (op)
   {
    case COPY_PUT:
      {
       op_gcv.function = GXcopy;
       break;
      }
    case XOR_PUT:
      {
       op_gcv.function = GXxor;
       break;
      }
    case OR_PUT:
      {
       op_gcv.function = GXor;
       break;
      }
    case AND_PUT:
      {
       op_gcv.function = GXand;
       break;
      }
    case NOT_PUT:
      {
       op_gcv.function = GXcopyInverted;
       break;
      }
    default:
      {
       break;
      }
   }
 op_gcv.tile = pixmap;
 op_gcv.fill_style = FillTiled;
 op_gcv.ts_x_origin = X;
 op_gcv.ts_y_origin = Y;

 XChangeGC(mainXwin->display, op_gc,
	   (GCFunction | GCTile | GCFillStyle | GCTileStipXOrigin | GCTileStipYOrigin), &op_gcv);

 style_set_Xwin(mainXwin, -1, -1, -1, fill_settings.color, -1, fill_settings.pattern);

 XFillRectangle(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page],
		op_gc, X, Y, width, height);

 style_set_Xwin(mainXwin, -1, -1, -1, fill_settings.color, -1, SOLID_FILL);


 XFreeGC(mainXwin->display, op_gc);


 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}


/*
 * $Id: putpixel.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Plots a pixel at a specified point.
 *
 * $Log: putpixel.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void putpixel(int x, int y, int color)
{
 /*
 XGCValues values;
 XGetGCValues(mainXwin->display, mainXwin->virtualgc, GCForeground, &values);
 XSetForeground(mainXwin->display, mainXwin->virtualgc, vga_palette[color].pixel_value);

 XDrawPoint(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc,
	    VPorigin.x + x, VPorigin.y + y);
 XSetForeground(mainXwin->display, mainXwin->virtualgc, values.foreground);
 */

 put_pixel_Xwin(mainXwin,VPorigin.x + x,VPorigin.y + y,mainXwin->palette[color].pixel);
 
 CP.x = x;
 CP.y = y;


 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/* 
XSync(mainXwin->display,False);
*/
}


/*
 * $Id: rectangle.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Draws a rectangle.
 *
 * $Log: rectangle.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void rectangle(int left, int top, int right, int bottom)
{
 unsigned int width;
 unsigned int height;

 width = right - left + 1;
 height = bottom - top + 1;

 style_set_Xwin(mainXwin, -1, -1, -1, BGIforegcolor, -1, SOLID_FILL);

 XDrawRectangle(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->virtualgc,
		VPorigin.x + left, VPorigin.y + top, width, height);


 repaint_Xwin(mainXwin); 

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
}



/*
 * $Id: setactivepage.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Sets the active page for graphics output.
 *
 * $Log: setactivepage.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void setactivepage(int page)
{
 setactive_Xwin(mainXwin, page);
}


/*
 * $Id: setallpalette.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Changes all palette colours as specified.
 *
 * $Log: setallpalette.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */


void setallpalette(palettetype * palette)
{
 int   i;

 if (palette->size <= MAXCOLORS)
   {
    mainBGIpalette.size = palette->size;
    for (i = 0; i < palette->size; i++)
      {
       mainBGIpalette.colors[i] = palette->colors[i];
      }
   }
}


/*
 * $Id: setaspectratio.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Changes the default aspect ratio correction factor.
 *
 * $Log: setaspectratio.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void setaspectratio(int xasp, int yasp)
{
 aspectratio_x = xasp;
 aspectratio_y = yasp;
}


/*
 * $Id: setbkcolor.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Sets the current background colour using the palette.
 *
 * $Log: setbkcolor.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void setbkcolor(int color)
{
 style_set_Xwin(mainXwin, -1, -1, -1, -1, color, -1);
 BGIbackgcolor = color;
}


/*
 * $Id: setcolor.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Sets the current drawing colour using the palette.
 *
 * $Log: setcolor.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void setcolor(int color)
{
 style_set_Xwin(mainXwin, -1, -1, -1, color, -1, -1);
 BGIforegcolor = color;
}


/*
 * $Id: setfillpattern.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Selects a user-defined fill pattern.
 *
 * $Log: setfillpattern.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void setfillpattern(unsigned char const *upattern, int color)
{
 /* 8x8 bitmap patterns supported */
 /* assumes standard 12 patterns to 0-11, and userbrushno = (12) = user pattern position */

 replace_brush_Xwin(mainXwin, userbrushno, upattern);
 userfillcolor = color;
 setfillstyle(userbrushno,color);
}


/*
 * $Id: setfillstyle.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Sets the fill pattern and colour.
 *
 * $Log: setfillstyle.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void setfillstyle(int pattern, int color)
{

/*
   XColor foreground;
   XColor unused;
  
   char *fill_color;
   int screen_num;
     
   screen_num = DefaultScreen(mainXwin->display);
   fill_color = Colors[color];
   XSetForeground(mainXwin->display, mainXwin->virtualgc, vga_palette[color].pixel_value);
*/

 fill_settings.pattern = pattern;
 fill_settings.color = color;
 style_set_Xwin(mainXwin, -1, -1, -1, color, -1, pattern);
}



/*
 * $Id: setlinestyle.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Sets the current line width and style.
 *
 * $Log: setlinestyle.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 * GLF 1/8/2010:  add user bit pattern for dashes
 */

int   dash_list_length[] = 
{
 DOTTEDLINE_LENGTH, CENTRELINE_LENGTH, DASHEDLINE_LENGTH
};

#define USERBITLINE_LENGTH 16

unsigned char dottedline[DOTTEDLINE_LENGTH] = { 3, 3 };

unsigned char centreline[CENTRELINE_LENGTH] = { 7, 3, 3, 3 };

unsigned char dashedline[DASHEDLINE_LENGTH] = { 11, 3 };

/* the folowing is a placeholder of maximum logical size to hold user data 
   -- the X dash list holds (pixelson,pixelsoff,pixelson,...) but the BGI 
   dash pattern is a 16-bit mask for pixels on/off -- the longest X dash 
   list consistent with this is a dotted line (i.e. 16 1's) */
unsigned char userbitline[USERBITLINE_LENGTH] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
int userbitlinelength = 16;

unsigned char *dash_list[] = 
{
 dottedline, centreline, dashedline, userbitline
};



void setlinestyle(int linestyle, unsigned upattern, int thickness)
{
 XGCValues values;
 int   dash_offset;
 int   i, runlength, listlength;
 int   bit, priorbit;
 int   totruns, oddruns;

 line_settings.linestyle = linestyle;
 line_settings.upattern = upattern;
 line_settings.thickness = thickness;


 dash_offset = 0;
 switch (linestyle)
   {
    case SOLID_LINE:
      {
       values.line_style = LineSolid;
       break;
      }
    case DOTTED_LINE:
      {
       values.line_style = LineOnOffDash;
       XSetDashes(mainXwin->display, mainXwin->virtualgc, dash_offset, dash_list[0], dash_list_length[0]);
       break;
      }
    case CENTER_LINE:
      {
       values.line_style = LineOnOffDash;
       XSetDashes(mainXwin->display, mainXwin->virtualgc, dash_offset, dash_list[1], dash_list_length[1]);
       break;
      }
    case DASHED_LINE:
      {
       values.line_style = LineOnOffDash;
       XSetDashes(mainXwin->display, mainXwin->virtualgc, dash_offset, dash_list[2], dash_list_length[2]);
       break;
      }
    case USERBIT_LINE:
      {
       /* read in bits of user dash pattern and convert to dash list */
       /* these CANNOT be completely compatible with BGI due to X lib dash line subsystem design,
          but BGI technique is simulated to extent possible -- cannot have odd number of pixel runs in
          pattern (allowed but converted to a concatenated reversal of that pattern) -- the following
          forces an even number of pixel runs and forces the first pixel to be ON -- that way 16-pixel
          repetetive user patterns are (mostly) preserved */
  
       values.line_style = LineOnOffDash;

       totruns  = 0; 
       oddruns = 0;
       runlength = 0;
       priorbit = -1; /* special case -- will NOT compare first bit found */
       listlength = 0;  

       /* always start with an "ON" bit to meet demands of X lib -- all dashed lines will have at least 
          one pixel per 16 in pattern showing -- may have all on, but never all off */
       upattern = upattern | 0x0001; 

       for (i=0; i<16; i++)
         {
          if (upattern & 0x0001)  /* test low bit of 16-bit pattern */
            {
             bit = 1;
            }
          else
            {
             bit = 0; 
            }
          if ((priorbit >= 0) && (priorbit != bit))
            {
             if (runlength >= 16)
               {
                /* special case -- can't use dash-on-off directly -- X lib won't allow 0 dash length */
                values.line_style = LineSolid;
               }
             if (totruns >= 16)
               { 
                runlength--;
                totruns--;
               }
             totruns += runlength;
             dash_list[3][listlength] = runlength; 
             oddruns != oddruns;
             listlength++;  
             runlength = 0;   
            }
          priorbit = bit; 
          runlength++;
          upattern = upattern >> 1;   /* shift 1 bit right to fetch next lowest bit */  
         }
        

       if (runlength >= 16)
         {
          /* special case -- can't use dash-on-off directly */
          values.line_style = LineSolid;
         }
       else
         { 
          if (totruns >= 16)
            { 
             runlength--;
             totruns--;
            }
          if (!oddruns)
            {
             /* ensure even number of runs */   
             dash_list[3][listlength] = runlength; 
             totruns += runlength;
             listlength++;  
            }
          runlength = 0; 
         } 

     
       userbitlinelength = listlength;
       dash_list_length[3] = listlength;
       if (totruns < 16)
         {
          dash_list[3][listlength-1] += (16 - totruns); 
         } 

       if (values.line_style != LineSolid)
         {
          XSetDashes(mainXwin->display, mainXwin->virtualgc, dash_offset, dash_list[3], dash_list_length[3]);
         }
       break;
      }
    default:
      {
       return;
      }
   }
 values.line_width = thickness;
 XChangeGC(mainXwin->display, mainXwin->virtualgc, GCLineWidth | GCLineStyle, &values);

}


/*
 * $Id: settextjustify.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Sets the text justification for the graphics functions.
 *
 * $Log: settextjustify.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void settextjustify(int horiz, int vert)
{
 txt_settings.horiz = horiz;
 txt_settings.vert = vert;
}

/*
 * $Id: settextstyle.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Sets the current text characteristics for graphics output.
 *
 * $Log: settextstyle.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void settextstyle(int font, int direction, int charsize)
{
 int dir;

 if (direction == HORIZ_DIR)
   {
    dir = 0;
   }
 else
   {
    dir = 1;
   }

 style_set_Xwin(mainXwin, dir, font, charsize, -1, -1, -1);
}


/*
 * $Id: setusercharsize.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Varies character width and height for stroked fonts.
 *
 * $Log: setusercharsize.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void setusercharsize(int multx, int divx, int multy, int divy)
{
 double xmagnification;
 double ymagnification;

 xmagnification = (double) multx / (double) divx;
 ymagnification = (double) multy / (double) divy;

 fontscale_size0_Xwin(mainXwin,xmagnification,ymagnification);
}


/*
 * $Id: setviewport.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Sets the current viewport for graphics output.
 *
 * $Log: setviewport.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 * GLF 1/7/2010:  kept XBGI code intact -- may be OK as-is
 */

void setviewport(int left, int top, int right, int bottom, int clip)
{
 /* the following APPEARS to be a guaranteed redraw to visual window before changing things */
/*
 XCopyArea(mainXwin->display, mainXwin->vpixmap[mainXwin->draw_page], mainXwin->window,
	   mainXwin->virtualgc, 0, 0, mainXwin->width, mainXwin->height, 0, 0);
*/
 /* verify inputs -- if not OK set up -11 code for graphresult() */
/*
 if ((left < 0)   || (left >= mainXwin->width)    || (right < 0) || (right >= mainXwin->width) ||
     (bottom < 0) || (bottom >= mainXwin->height) || (top < 0)   || (top >= mainXwin->height)     )
   {
    
    return;
   }
*/


 /* first, remove any existing clipping window */
 visible_all_Xwin(mainXwin); 
  
 xrectangle.x = (short) left;
 xrectangle.y = (short) top;
 xrectangle.width = (unsigned short) (right - left) + 1;
 xrectangle.height = (unsigned short) (bottom - top) + 1;

 if (clip)
   {
    /* visible_inside_rect_Xwin(mainXwin,xrectangle.x,xrectangle.y,xrectangle.width,xrectangle.height);  */
   }
/*
 else
   {
    xrectangle.x = 0;
    xrectangle.y = 0;
    xrectangle.width = getmaxx();
    xrectangle.height = getmaxy();
   }
*/

 view_settings.left = left;
 view_settings.top = top;
 view_settings.right = right;
 view_settings.bottom = bottom;
 view_settings.clip = clip;
 
 VPorigin.x = left;
 VPorigin.y = top;
 CP.x = 0;
 CP.y = 0;

 flushbuffers_Xwin(mainXwin);
/*
 XSync(mainXwin->display,False);
*/
/*
 repaint_Xwin(mainXwin); 
*/
}


/*
 * $Id: setvisualpage.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Sets the visual graphics page number.
 *
 * $Log: setvisualpage.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void setvisualpage(int page)
{
 setvisual_Xwin(mainXwin, page);
}


/*
 * $Id: setwritemode.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Sets the writing mode for line drawing in graphics mode.
 *
 * $Log: setwritemode.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 * GLF 1/7/2010:  kept XBGI code intact -- may be OK as-is
 */

void setwritemode(int mode)
{
 XGCValues values;

 switch (mode)
   {
    case COPY_PUT:
      {
       values.function = GXcopy;
       break;
      }
    case XOR_PUT:
      {
       values.function = GXxor;
       break;
      }
    case OR_PUT:
      {
       values.function = GXor;
       break;
      }
    case AND_PUT:
      {
       values.function = GXand;
       break;
      }
    case NOT_PUT:
      {
       values.function = GXcopyInverted;
       break;
      }
    default:
      {
       break;
      }
   }
 XChangeGC(mainXwin->display, mainXwin->virtualgc, GCFunction, &values);
}


/*
 * $Id: textheight.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Returns the height of a string in pixels.
 *
 * $Log: textheight.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

int textheight(char const *textstring)
{
 int   xwidth, yheight;

 scalexy_currentfont_Xwin(mainXwin, mainXwin->font_size, 100, &xwidth, &yheight);
 return yheight;
}


/*
 * $Id: textwidth.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Returns the width of a string in pixels.
 *
 * $Log: textwidth.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */


int textwidth(char const *textstring)
{
 /* using current font info and scaling, calculate effective output width in pixels */
 return scaled_textwidth_Xwin(mainXwin, textstring, strlen(textstring));
}






/* -------------------- Specific BGI API support -- NOT YET implemented --------------- */

/*
 * $Id: floodfill.c,v 0.1 1993/12/10 00:13:14 king Exp king $
 * Flood-fills a bounded region.
 *
 * $Log: floodfill.c,v $
 * Revision 0.1  1993/12/10  00:13:14  king
 * Initial version.
 *
 */


void floodfill(int x, int y, int border)
{
 /* use seed point (x,y) to fill a bounded area in current fill pattern and color */
 /* style_set_Xwin(xw,-1,-1,-1,1,XWIN_DEFAULT,8); -- demo that floodfill can use fill pattern */


 /* for now, ignore border color (any non-background color is considered border, as well as
    anything offscreen) */

 int   X;
 int   Y;

 X = x + VPorigin.x;
 Y = y + VPorigin.y;

 areaflood_Xwin(mainXwin, X, Y, userfillcolor);
 repaint_Xwin(mainXwin); 
}


/*
 * $Id: getpalette.c,v 0.1 1993/12/10 00:15:31 king Exp king $
 * Gets information about the current palette.
 *
 * $Log: getpalette.c,v $
 * Revision 0.1  1993/12/10  00:15:31  king
 * Initial version.
 *
 */

void getpalette(palettetype * palette)
{
 int   i;
 /* 
  * This routine not currently fully implemented.
  */

 /* the following applies only to initial VGAHI mode */
 palette->size = 16;
 for (i = 0; i < 16; i++)
   {
    palette->colors[i] = i;
   }
 /* if other modes -- the palette entries would consist of subsets of 64 indices which point to the 
    actual pixels colors used */
}


/*
 * $Id: _graphfreemem.c,v 0.1 1993/12/09 23:59:27 king Exp king $
 * User hook into graphics memory deallocation.
 *
 * $Log: _graphfreemem.c,v $
 * Revision 0.1  1993/12/09  23:59:27  king
 * Initial version.  __graphfreemem not currently implemented.
 *
 */

/*
 * $Id: installuserdriver.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Installs a vendor-added device driver to the BGI device driver table.
 *
 * $Log: installuserdriver.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

int installuserdriver(char const *name, int *detect)
{
 /* 
  * This routine not currently implemented.
  */
 return 0;
}


/*
 * $Id: installuserfont.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Loads a font file that is not built into the BGI system.
 *
 * $Log: installuserfont.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */



int installuserfont(char const *name)
{
 /* 
  * This routine not currently implemented.
  */
 return 0;
}


/*
 * $Id: registerbgidriver.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Registers a user-loaded or linked-in graphics driver code with the graphics
 * system.
 *
 * $Log: registerbgidriver.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

int registerbgidriver(void *driver)
{
 /* 
  * This routine not currently implemented.
  */
 return 0;
}


/*
 * $Id: registerbgifont.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Registers lined-in stroked font code.
 *
 * $Log: registerbgifont.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

int registerbgifont(void *font)
{
 /* 
  * This routine not currently implemented.
  */
 return 0;
}

/*
 * $Id: restorecrtmode.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Restores the screen mode to its pre-initgraph() setting.
 *
 * $Log: restorecrtmode.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void restorecrtmode(void)
{
 /* 
  * This routine not currently implemented.
  */

 /* if it were, it should bring up a terminal windowrunning a command shell, lconio, curses, or
    similar interface -- the main windows event loop may be suspended while the command shell is
    active */
}


/*
 * $Id: setgraphbufsize.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Changes the size of the internal graphics buffer.
 *
 * $Log: setgraphbufsize.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

unsigned int setgraphbufsize(unsigned int bufsize)
{
 /* 
  * This routine not currently implemented.
  */
 return 0;
}


/*
 * $Id: setgraphmode.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Sets the system to graphics mode and clears the screen.
 *
 * $Log: setgraphmode.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void setgraphmode(int mode)
{
 /* 
  * This routine not currently implemented.
  */

 /* if it were, it should shift from command window back to mainwindow and maybe put the main
    window on top */
 cleardevice();
}


/*
 * $Id: setpalette.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Changes one palette colour.
 *
 * $Log: setpalette.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void setpalette(int colornum, int color)
{
 /* 
  * This routine not currently implemented.
  */
 /* need an extra layer of indirection -- need a 16-color palette table for use by BGI functions
    which can be used to look up the color numbers (0-63) ACTUALLY selected */

 if ((colornum >= 0) && (colornum <= MAXCOLORS)
     && (colornum < mainBGIpalette.size) && (color <= MAXCOLORS))
   {
    mainBGIpalette.colors[colornum] = color;
   }
}


/*
 * $Id: setrgbpalette.c,v 0.1 1993/12/10 00:39:08 king Exp king $
 * Allows the user to define colours for the IBM 8514.
 *
 * $Log: setrgbpalette.c,v $
 * Revision 0.1  1993/12/10  00:39:08  king
 * Initial version.
 *
 */

void setrgbpalette(int colornum, int red, int green, int blue)
{
 /* 
  * This routine not currently implemented.
  */
}



/* ------------------------------------------------------------------------------------ */

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

 
#endif /* ...end assume Linux */
