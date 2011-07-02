/*
   xwinlite.h -- X windows library for handling basic windows, fonts, colors, 
                 and keystrokes

                 hides most of the more arcane details and minimizes what a 
                 user program, needs to do or know

                 GLF 22 Dec 2009   Gary L. Flispart

                 Based on a lot of work by a few other people who left their 
                 code on the internet for others to find.  Huzzah!
*/


#ifndef __XWINLITE_H_
#define __XWINLITE_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>


#include <stdio.h>
#include <stdlib.h>		/* getenv(), etc. */
#include <sys/time.h>           /* for timers */

/* not strictly needed -- see if compatible */
#include <unistd.h>
#include <string.h>


/* #define DEBUG_XWINLITE */
/* #define DEBUG_XWINLITE */

#ifdef DEBUG_XWINLITE
extern FILE *outdebug;
#endif


/* 
   X library programming is arcane in the extreme, composed of invocations of secret
   forms of black magic and calling upon many competing X spirits who seemingly squabble 
   with one another.  The X wizards and necromancers who wield this magic tend to hoard 
   their knowlege, expecting upstart apprentice wizards to go through the same pain 
   they once endured if they should dare to seek summoning the spirits of X.
   Though the online Xlib manual http://www.tronche.com/gui/x/xlib/ is essential, 
   it is often indecipherable, full of functions which seem to need just one more item 
   than is currently available, or requiring simple processes to jump through three  
   non-obvious hoops.  Since X has been around seemingly forever, the whole topic is
   apparently considered boring or "done that already" by many serious developers who
   have moved on to bigger and better things. Beginners are hard pressed to figure X out.
 
   The first invaluable site I found with example code formed the core of my experimental code: 
   http://users.actcom.co.il/~choo/lupg/tutorials/xlib-programming/xlib-programming.html#preface
   This site had code which *SORT OF* worked, though I had to experiment a lot and make changes until
   I got parts of the code to work one step at a time.  X seems to have changed a lot of locations
   and specifications over the years and much old code seems to need editing.

   Credit for much of the following struct originally named XWindow, renamed to Xwin,
   and associated code ideas belongs to the author of the website "Introduction 
   to X Window programming", http://nobug.ifrance.com/nobug2/article1/babyloon/tut_xwin.htm

   These sites were beacons in the desert night.  It seems everybody on Linux uses X windows, 
   but few either know how it works or can demonstrate working code others can share.
   Thanks, whoever you are!  I did, however make substantial changes to the schemes in
   the tutorials and the XWindow (Xwin) structure to implement my own simplified vision 
   of how to make windows "just work" for fairly low-level programs, without the weight 
   of hundreds of widgets in a more developed library, and using old-fashioned C code 
   instead of C++.  

   My ultimate goal was a simple conversion of old Borland MS-DOS BGI code to work under Ubuntu 
   Linux (based on a variant of the winBGI library -- XBGI exists in old form (incomplete) 
   or in an updated more complicated form, C++ Japanese, hard for my old USA C brain to wrap around).  
   The more I got into X windows programming, the more optimistic I became that I might actually
   be able to come up with a lightweight translation layer to get the Windows calls in winBGI 
   to use underlying X library calls. As of this writing, I'm not there yet, but a lot closer 
   than a month ago.  The learning process was reminiscent of going through the classic Petzold 
   tutorial, "Programming Windows 95" some years ago.  Let others try to rule the internet with 
   Java and PHP, I'm thrilled just to get my "Hello World" to redraw itself from a virtual Pixmap!

                                                   -- GLF 22 Dec 2009
*/


/* ------------------------------------------------------------------------------------------------ */


/* MS-WINDOWS composite typedefs emulated here for programming convenience */
#ifndef RECT 
typedef struct tagRECT {
	int left;
	int top;
	int right;
	int bottom;
} RECT,*PRECT,*LPRECT;
typedef const RECT *LPCRECT;
#endif
#ifndef POINT 
typedef struct tagPOINT {
	int x;
	int y;
} POINT,POINTL,*PPOINT,*LPPOINT,*PPOINTL,*LPPOINTL;
#endif
#ifndef SIZE 
typedef struct tagSIZE {
	int cx;
	int cy;
} SIZE,SIZEL,*PSIZE,*LPSIZE,*PSIZEL,*LPSIZEL;
#endif

#ifndef FALSE 
#define FALSE 0
#endif

#ifndef TRUE 
#define TRUE 1
#endif

/* a pixel value GUARANTEED to not == ANY pixel legitimately returned from a 24-bit Pixmap 
   -- used to describe "pixels" offscreen of actual window */
#define NOTAPIXEL 0x01000000


/* colors 0-15 define EGAVGA palette, 16-63 fill in alternative emulated palettes (winBGIm) */
#define MAXCOLORS 63

/* 12 builtin brushes and 18 user brushes -- enough to do 16-shade dither 
   halftoning with patterns to spare -- any pattern may be reassigned once loaded */
#define MAXBRUSHES 30

/* use the following value in call to setstyle_Xwin() to set any variable to default */
#define XWIN_DEFAULT  1000000

/* use the following value in call to setstyle_Xwin() to retain any variable unchanged */
#define XWIN_RETAIN  -1

/* this may need adjustment to make double-clicks reliable yet responsive (about 0.3 second) */
#define DBLCLICK_TIMER_FULLVALUE 60000

/* first 17 fonts reserved for winBGIm-compatible font sizing and scaling scheme */
#define MAXFONTS 30

#define KB_EMPTY -1
#define KB_WINDOW_QUIT -2
#define KB_BUFSIZE 200


/* --------------------------------------------------------------------------------- */
/* default font list -- last element must be NULL to signal end of list */
extern char *XFonts[];

/* --------------------------------------------------------------------------------- */

/* support multiple visual pages */
#define MAX_VPAGES 16


typedef struct tagXwin
  {
   Display *display;
   Window window;
   Pixmap vpixmap[MAX_VPAGES+1];
   int draw_page;
   int video_page;
   int numvpages;
 
   void *parentxw;      /* must be declared void (instead of Xwin) to avoid recursive struct definition 
                           pointer size is known at compile time regardless of type of pointer target */

   int screennum;
   Visual *visual;
   Cursor cursor;
   Colormap colormap;
   XColor palette[MAXCOLORS+1];

   GC gc;
   GC virtualgc;

   int force_redraw; 
   int hasresized;
   int allowresize;
   int width;
   int height;
   int depth;

   int wposx;
   int wposy;

   int mouse_wposx;       /* mouse position within this window only */
   int mouse_wposy;
   int mouse_button1;     /* TRUE = button currently pressed */
   int mouse_button2;
   int mouse_click1;      /* TRUE = button was clicked -- position below */
   int mouse_click2;
   int mouse_dblclick1;   /* TRUE = button was clicked twice in 0.3 sec -- position below */
   int mouse_click1x;
   int mouse_click1y;
   int mouse_click2x;
   int mouse_click2y;
   int mouse_dblclick1x;
   int mouse_dblclick1y;
   int mouse_dbltimect;

   int pixelsize;
   int screensize;
   int firstevent;
   int stopevents;

   /* XImages and data to cache pixels from virtual screen to allow getpixel operations -- X Drawables
      have no getpixel function, but XImages support XGetPixel() -- caches alow more efficient 
      access when many pixels must be looked up, but care must be taken to avoid changes to screen
      in between pixel lookups */ 

   XImage *pixelcache;                /* for 8 x 8 pixel cache */
   XImage *fullscreen_ximage;         /* for fullscreen pixel cache */
   int use_fullcache;                 /* flag to turn on/off hidden switch to full cache vs. no cache */ 
   int force_killcache;               /* set this to TRUE if you want to clear both pixel caches */ 
   int pixelcache_x;
   int pixelcache_y;
   int putchange_x;
   int putchange_y;

   /* small-interval timer simulation support (PC clock not guaranteed to support high resolution) */
   unsigned long eventloopcount;
   unsigned long loops_per_ms;         /* timer calibration */
   unsigned long calibrate_megaloops;  /* fifths of seconds timer seen 1 million loops ago 
                                          for ongoing calibration of loops_per_ms */

   /* variable to "watch the clock" for timers, screen repainting, and minimal animation support */
   unsigned long fifth_secs;     

   int kbbuffer[KB_BUFSIZE+1];
   int kbtail;
   int kbhead;

   int ignore_modifiers;   
 
   int nbrushesloaded;
   Pixmap brush[MAXBRUSHES+1];

   unsigned long currentbgcolorpixel; 
   unsigned long currentcolorpixel; 
   int currentbrush; 
   int currentx;
   int currenty;
   
   unsigned long backgroundpixel; 
   unsigned long foregroundpixel; 

   XFontStruct* fontinfo[MAXFONTS+1];      
   int fontloaded[MAXFONTS+1];
   int nfontsloaded;
   int currentfontindex;

   int font_direction;
   int font_size; 
   double fontscale_x;
   double fontscale_y;
 
   void *userdata;        /* a pointer for any expansion data a specific window
                             might need that others won't -- large structs or buffers, etc. */  
  } 
   Xwin;

/* -------------------------------------------------------------------------------------- */

int add_font_Xwin(Xwin *xw, char *fontname);
int change_font_height_Xwin(Xwin *xw, int fontindex);
void load_fontlist_Xwin(Xwin *xw, char *fontlist[]);

/* -------------------------------------------------------------------------------------- */

void visible_inside_rect_Xwin(Xwin *xw, int x, int y, int wid, int hgt);
void visible_outside_rect_Xwin(Xwin *xw, int x, int y, int wid, int hgt);
void visible_all_Xwin(Xwin *xw);

/* ---------------------------------------------------------------------------- */
/* guaranteed to be current but inefficient in large numbers */

unsigned long get_pixel_Xwin(Xwin *xw, int x, int y);

/* more efficient in large numbers but must not putpixels to same area while reading 
   or may get erroneous pixels -- this implementation assumes screen width and height 
   are 0 mod 8 */

unsigned long get_pixel_cache_Xwin(Xwin *xw, int x, int y);

/* more efficient when dealing with large volume fullscreen random lookups within a given
   function not concerned with additional "putpixels" to screen such as image output to file,
   stretch BLTs, or floodfills -- XImage is full screen size (not including text buffer) */

unsigned long get_pixel_fullcache_Xwin(Xwin *xw, int x, int y);

/* put_pixel_Xwin works with either cached or uncached getpixel directly to virtual screen
   -- caller is responsible to detect dirty pixels or other changes to screen if reloading
   of getpixel cache is required (set xw->force_killcache = TRUE) -- however, that is not 
   necessary if only get_pixel_Xwin is used, since it always gets current virtual pixels */
  
void put_pixel_Xwin(Xwin *xw, int x, int y, unsigned long pixel);

/* this version of putpixel works directly on fullscreen pixel cache, and NOT on 
   virtual screen itself -- useful for floodfill function */ 

void put_pixel_fullcache_Xwin(Xwin *xw, int x, int y, unsigned long pixel);

/* ---------------------------------------------------------------------------- */

/* the following use the getpixel variants above to get a pixel and test 
   whether it:

        (testnotb)   is NOT the color of background pixel
        (test)       is the color of the specified pixel

   The first test is TRUE against ALL BUT one color, the second test
   is TRUE against ONLY ONE color.
*/         

int testnotb_pixel_Xwin(Xwin *xw, int x, int y);
  
int testnotb_pixel_cache_Xwin(Xwin *xw, int x, int y);

int test_notb_pixel_fullcache_Xwin(Xwin *xw, int x, int y);
 
int test_pixel_Xwin(Xwin *xw, int x, int y, unsigned long pixel);
  
int test_pixel_cache_Xwin(Xwin *xw, int x, int y, unsigned long pixel);

int test_pixel_fullcache_Xwin(Xwin *xw, int x, int y, unsigned long pixel);
  
unsigned long getpixel_Xwin(Xwin *xw, int x, int y);

int testpixel_Xwin(Xwin *xw, int x, int y, unsigned long pixel);

void putpixel_Xwin(Xwin *xw, int x, int y, unsigned long pixel);

/* ------------------------------------------------------------------------- */

/* The following are glue functions which satisfy calls found in old GLF gphbsubs.c 
   code used to provide a floodfill function */

/* replacement for gphpttestonly() */
int testnotbpixel_Xwin(Xwin *xw, int x, int y);

/* replacement for gphpt() */
int pointpixel_Xwin(Xwin *xw, int x, int y, int code, int wx0, int wy0, int wx1, int wy1, int biton);

/* ----------------------------------------------------------------------------------------------- */

void fillrect_Xwin(Xwin *xw, int x, int y, int width, int height);

void drawrect_Xwin(Xwin *xw, int x, int y, int width, int height);

void clearrect_Xwin(Xwin *xw, int x, int y, int width, int height);

void drawline_Xwin(Xwin *xw, int x0, int y0, int x1, int y1);

void moveto_Xwin(Xwin *xw, int x, int y);

void drawto_Xwin(Xwin *xw, int x, int y);

/* ----------------------------------------------------------------------------------------------- */

/* add one brush to the brush list and set up stipple pattern from data (8 bits X 8) */
/* if maximum patterns already allocated, overwrite the last pattern */
/* return value is brush number of last pattern loaded OR -1 if not valid */

int add_brush_Xwin(Xwin *xw, unsigned char pattern[8]);

/* replaces pattern only if brush number already exists in list -- return TRUE if replaced */

int replace_brush_Xwin(Xwin *xw, int brushno, unsigned char const pattern[8]);

/* ---------------------------------------------------------------------------- */

/* flood fill support adapted from (much) earlier GLF work on PLOTTER program */

/* Support for "flood fill" technique UNIQUE TO THIS PROGRAM (@ Gary Flispart)
   
   The flood fill support in this program avoids large amounts of indefinite
   data storage during recursive area scanning by first evaluating the shape of
   the fill area by tracing around its inside border (treating the bitmnap
   border as "filled" for this purpose.  The walkborder() function performs
   the low-level border trace by "rolling" a virtual "wheel" clockwise around
   the inside of the next available fill area.  The position of the "wheel"
   is monitored and the lower leftmost, bottommost point is latched during
   the traversal.  Once the algorithm detects that a traversal has completed 
   AT LEAST one revolution, therefore visiting all border points, it returns.
   The border (flood) fill function, which then knows the lower leftmost point
   within the area of interest, begins to accumulate a list of lines to fill at
   a later time.  As each line's endpoints are stored, the line is filled in 
   (all black) to prevent the remainder of the area scan from revisiting that line
   segment.  The algorithm recursively detects overflow to new potential areas
   to fill in a manner reminiscent of filling an irregular vessel from the lowest
   point at the bottom (the liquid "spills over" from each lowest subvolume until
   the entire vessel fills up).  When all line specifications have been 
   accumulated, the algorithm quickly applies the final coloration, replacing the
   temporary black filler.  This technique is transparent to preexisting content
   within the area because the line segments to be filled stop at the edges of 
   such content as the edges are detected.  The technique has two primary
   advantages:  by filling from the lowest point of the area, it tends to require 
   minimal data storage resources during the recursive scan so it works reliably 
   in limited memory (originally MS-DOS);  and it may be expanded (future project)
   to allow superposition of specialty content (such as a grayscale image) within 
   the detected boundary.
*/

/* these values are far offscreen to prevent */
/* scanning below lowest line actually used */

#define BEYONDEXTREMELEFT   -1000000 
#define BEYONDEXTREMERIGHT   1000000

/* definitions for screen model */

#define MAXDITHERLINKS 1500
#define NUMVDOTS 480
#define NUMHDOTS 640 
#define XDOTSPERINCH 100    /* ? */
#define YDOTSPERINCH 100    /* ? */
#define HSIZECONSTANT 500   /* ? */  
#define VSIZECONSTANT 500   /* ? */
#define MAXPAINTLIST 32760  /* ? */

/* the following might eventually be expanded for high DPI resolutions, but not yet */
#define DITHERSHADES 32

/* calculated from definitions above */
#define MAXVDOT NUMVDOTS - 1
#define MAXHDOT NUMHDOTS - 1
#define MAXSCREENHDOT NUMSCREENHDOTS - 1
#define MAXSCREENVDOT NUMSCREENVDOTS - 1


#pragma pack(1)
typedef struct
  {
   int xl;
   int xr;
   int y;
   short int ylink;
  }
   paintlist;


extern paintlist ditherlist[MAXPAINTLIST];
extern int ditherlistnum;

int put_dithlist(int i, int ixl, int ixr, int iy);

int get_dithlist(int i, int *pxl, int *pxr, int *py);

int add_dithlist(int ixl, int ixr, int iy);

void clear_dithlist(void);

#pragma pack()

extern int bug_wx0;
extern int bug_wy0;
extern int bug_wx1;
extern int bug_wy1;

int is_bug_window(int x, int y);


void walkborder_Xwin(Xwin *xw, int *x, int *y);
                                  /* walks clockwise around inside border
                                     one point at a time -- assumes already
                                     on border */

void findborder_Xwin(Xwin *xw, int x, int y);

extern int a_scanct;


int borderfill_Xwin(Xwin *xw, int x, int y, unsigned long fillpixel);
                      /* RECURSIVE! */


void areaflood_Xwin(Xwin *xw, int x0, int y0, unsigned long fillpixel);

/* ---------------------------------------------------------------------------- */

/* Font sizing support assumes that the specific 17 fonts listed above in XFonts[] are in 
   positions 0-16 in font list in the order shown.  Other fonts may be loaded after that,
   but sizing support will ignore them. */

int convert_fontnumber_Xwin(Xwin *xw, int fontnumber, int size);

void linefeedxy_Xwin(Xwin *xw, int dir, int size, int *x, int *y);

void scalexy_currentfont_Xwin(Xwin *xw, int size, int fontwidth, int *xwidth, int *yheight);

/* a replacement for XTextWidth() which takes scaling into account for CURRENT font */
int scaled_textwidth_Xwin(Xwin *xw, const char *string, int length);

/* for all styles settable by this function, a value < 0 retains prior state for that style
   -- for some variables, a value higher than the legal maximum will set that variable to default */

void style_set_Xwin(Xwin *xw, int dir, int fontnumber, int size, 
                                      int colornum, int bgcolornum, int brushnum);

void fontscale_size0_Xwin(Xwin *xw, double xscale, double yscale);

int text_nextbreak_Xwin(Xwin *xw, int *pos, 
                                         int x, int y, int textwidth, 
                                                         const char string[]);


/* NOTE: x, y, and textywidth are specified here in terms of window coordinates and thus
         represent limits on actual size.  THe width of characters in a specific font
         varies with scaling of that font, and scaling must be taken into account when 
         determining text breakpoints, etc.
*/


void text_charsout_Xwin(Xwin *xw, int justify, int colornumber, 
                                         int x, int y, int textwidth,  
                                         const char string[], int length);

void text_lineout_Xwin(Xwin *xw, int justify, int colornumber, 
                                         int x, int y, int textwidth, 
                                                         const char string[]);

/* outputs a text box with automatic word wrapping */
void text_boxout_Xwin(Xwin *xw, int justify, int colornumber, 
                                         int x, int y, int textwidth, int textheight, int applyclipping, 
                                                         const char string[]);

void text_out_Xwin(Xwin *xw, int dir, int fontnumber, int size, int colornum, int drawbackground,  
                                     int x, int y, int limitwidth, const char *string, int length);

void string_out_Xwin(Xwin *xw, int dir, int fontnumber, int size, int colornum, int drawbackground,  
                                     int x, int y, int limitwidth, const char *string);

void textout_Xwin(Xwin *xw, int drawbackground, int x, int y, int limitwidth, const char *string, int length);

void stringout_Xwin(Xwin *xw, int drawbackground, int x, int y, int limitwidth, const char *string);

/* -------------------------------------------------------------------------------- */

/* utility function to get position of mouse anywhere on main screen either in absolute
   coordinates (upper-left of screen = (0,0)) and current position of this window on
   the main screen, also in absolute coordinates -- coordinates of mouse must be positive 
   within (0,0) to (width-1,height-1) of display -- origin of screen may be negative 
   if window is pushed off left side of screen 
*/

void get_screen_mousepos_Xwin(Xwin *xw, int *mousex, int *mousey, int *origx, int *origy);

/* --------------------------------------------------------------------------------- */

void finish_Xwin(Xwin *xw);

Xwin *create_sub_Xwin(Xwin *parent, const char *windowname, int width, int height, int resizable, 
                                  int virtualbuffers, int whitebackground, Cursor specialcursor);


Xwin *create_main_Xwin(const char *windowname, int width, int height, int resizable, 
                                  int virtualbuffers, int whitebackground, Cursor specialcursor);

/* get a bit mask to identify SHIFT CTRL ALT state:  0x0200 = SHIFT, 0x0400 = CTRL, 0x800 = ALT */
unsigned int modifier_mask_Xwin(Xwin *xw);
 
/* ------------------------------------------------------------------------- */

/* any_window_event()
   ------------------- 
   This function allows a window to check the window-specific event queue
   without blocking -- returns False if no pertinent events are ready, otherwise
   returns TRUE with the event found.  ALL Client messages (global) are
   returned -- it is up to the caller to interpret Client messages
   and determine if they belong to the window -- if not they may be lost to
   the rightful client.  Unfortunately, This is the ONLY mechanism available 
   to detect a user's clicking the "X to Close" button on a window.
*/

Bool any_window_event_Xwin(Xwin *xw, long event_mask, XEvent *event);

int default_singlepass_eventloop_Xwin(Xwin *xw);

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

int kbhit_Xwin(Xwin *xw, int runevents);

/* get next key in buffer -- if empty, returns < 0 */
int getch_Xwin(Xwin *xw);

/* TODO:  make a line-oriented input function and/or screen-oriented edit functions */

void delay_Xwin(Xwin *xw, int msec);

/* -------------------------------------------------------------------------------- */

/* hide picky details of storage for XPM file image data */
typedef struct tagXwin_xpmimage 
  {
   XImage *ximage;                 /* storage for a specific .xpm image file */
   XImage *shapeximage;            /* storage for a specific .xpm image file -- image mask */  
  }
   Xwin_xpmimage;


int open_xpm_Xwin(Xwin *xw, Xwin_xpmimage *xpmstore, const char filename[]);

void close_xpm_Xwin(Xwin *xw, Xwin_xpmimage *xpmstore);

void copy_xpm_to_Xwin(Xwin *xw, Xwin_xpmimage *xpmstore, 
                         int src_x, int src_y, int src_width, int src_height, int dest_x, int dest_y);

/* -------------------------------------------------------------------------------------- */


void repaint_Xwin(Xwin *xw);

void setvisual_Xwin(Xwin *xw, int pageno);

void setactive_Xwin(Xwin *xw, int pageno);

void flushbuffers_Xwin(Xwin *xw);


/* -------------------------------------------------------------------------------------- */


#endif

