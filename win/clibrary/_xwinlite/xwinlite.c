/*
   xwinlite.c -- X windows library for handling basic windows, fonts, colors, 
                 and keystrokes

                 hides most of the more arcane details and minimizes what a 
                 user program, needs to do or know

                 GLF 22 Dec 2009   Gary L. Flispart

                 Based on a lot of work by a few other people who left their 
                 code on the internet for others to find.  Huzzah!
*/

#include "xwinlite.h"

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

#ifdef DEBUG_XWINLITE
FILE *outdebug = NULL;
#endif


/* ------------------------------------------------------------------------------------------------ */


/* default font list -- last element must be NULL to signal end of list */

/* The following fonts are known to exist under Ubuntu 9.04 Linux -- the font sizes were 
   chosen to allow winBGIm Size 1-4 to use actual installed fonts*/
char *XFonts[] = 
  {                                                  
 /*       "-misc-fixed-medium-r-normal-*-13-*-*-*-*-80-iso8859-8", */  /* Default font    Size 1 */
        "-misc-fixed-bold-r-normal-*-13-*-*-*-*-80-*-*",             /* Default font    Size 1 */
        "-adobe-times-medium-r-normal-*-18-*",                       /* Triplex font    Size 1 */
        "-adobe-helvetica-medium-r-normal-*-10-*",                   /* Small font      Size 1 */
        "-adobe-helvetica-medium-r-normal-*-18-*",                   /* Sansserif font  Size 1 */
        "-adobe-helvetica-bold-r-normal-*-18-*",                     /* Gothic font     Size 1 */
        "-adobe-times-medium-r-normal-*-20-*",                       /* Triplex font    Size 2 */
        "-adobe-helvetica-medium-r-normal-*-11-*",                   /* Small font      Size 2 */
        "-adobe-helvetica-medium-r-normal-*-20-*",                   /* Sansserif font  Size 2 */
        "-adobe-helvetica-bold-r-normal-*-20-*",                     /* Gothic font     Size 2 */
        "-adobe-times-medium-r-normal-*-24-*",                       /* Triplex font    Size 3 */
        "-adobe-helvetica-medium-r-normal-*-12-*",                   /* Small font      Size 3 */
        "-adobe-helvetica-medium-r-normal-*-24-*",                   /* Sansserif font  Size 3 */
        "-adobe-helvetica-bold-r-normal-*-24-*",                     /* Gothic font     Size 3 */
        "-adobe-times-medium-r-normal-*-34-*",                       /* Triplex font    Size 4 */
        "-adobe-helvetica-medium-r-normal-*-17-*",                   /* Small font      Size 4 */
        "-adobe-helvetica-medium-r-normal-*-34-*",                   /* Sansserif font  Size 4 */
        "-adobe-helvetica-bold-r-normal-*-34-*",                     /* Gothic font     Size 4 */
        NULL
  };


/* if 64 colors are specified (allow_colors != 0) they match winBGI color list */
/* colors names are matched to winBGIm RGB color list -- * indicates "best guess" */
char *XColors[] = 
  {
   /* 00  0x00 0x00 0x00 */ "black",
   /* 01  0x00 0x00 0xFF */ "blue",
   /* 02  0x00 0xFF 0x00 */ "green",
   /* 03  0x00 0xFF 0xFF */ "cyan",
   /* 04  0xFF 0x00 0x00 */ "red",
   /* 05  0xFF 0x00 0xFF */ "magenta",
   /* 06  0xA5 0x2A 0x2A */ "brown",
   /* 07  0xD3 0xD3 0xD3 */ "light gray",
   /* 08  0x2F 0x4F 0x4F */ "dark slate gray",
   /* 09  0xAD 0xD8 0xE6 */ "light blue",
   /* 10  0x20 0xB2 0xAA */ "light sea green",
   /* 11  0xE0 0xFF 0xFF */ "light cyan",
   /* 12  0xF0 0x80 0x80 */ "light coral",
   /* 13  0xDB 0x70 0x93 */ "pale violet red",
   /* 14  0xFF 0xFF 0x00 */ "yellow",
   /* 15  0xFF 0xFF 0xFF */ "white",
 
   /* 16  0xF0 0xF8 0xFF */ "alice blue",
   /* 17  0xFA 0xEB 0xD7 */ "antique white",
   /* 18* 0x1E 0x90 0xFF */ "dodger blue",
   /* 19  0x7F 0xFF 0xD4 */ "aquamarine",
   /* 20  0xF0 0xFF 0xFF */ "azure",
   /* 21  0xF5 0xF5 0xDC */ "beige",
   /* 22  0xFF 0xE4 0xC4 */ "bisque",
   /* 23* 0xFF 0x69 0xB4 */ "hot pink",
   /* 24  0x00 0x00 0xFF */ "blue",
   /* 25  0x8A 0x2B 0xE2 */ "blue violet",
   /* 26* 0x8B 0x24 0x24 */ "firebrick4",
   /* 27  0xDE 0xB8 0x87 */ "burlywood",
   /* 28  0x5F 0x9E 0xA0 */ "cadet blue",
   /* 29  0x7F 0xFF 0x00 */ "chartreuse",
   /* 30* 0xD3 0x69 0x1E */ "chocolate",
   /* 31  0xFF 0x7F 0x50 */ "coral",
   /* 32  0x64 0x95 0xED */ "cornflower blue",
   /* 33  0xFF 0xF8 0xDC */ "cornsilk",
   /* 34* 0xCD 0x10 0x76 */ "DeepPink3",
   /* 35* 0x6C 0xA6 0xCD */ "SkyBlue3",
   /* 36  0x00 0x00 0x8B */ "dark blue",
   /* 37  0x00 0x8B 0x8B */ "dark cyan",
   /* 38  0xB8 0x86 0x0B */ "dark goldenrod",
   /* 39  0xA9 0xA9 0xA9 */ "dark gray",
   /* 40  0x00 0x64 0x00 */ "dark green",
   /* 41  0xBD 0xB7 0x6B */ "dark khaki",
   /* 42  0x8B 0x00 0x8B */ "dark magenta",
   /* 43  0x55 0x6B 0x2F */ "dark olive green",
   /* 44  0xFF 0x8C 0x00 */ "dark orange",
   /* 45* 0xB4 0x52 0xCD */ "MediumOrchid3",
   /* 46  0x8B 0x00 0x00 */ "dark red",
   /* 47  0xE9 0x96 0x7A */ "dark salmon",
   /* 48  0x8F 0xBC 0x8F */ "dark sea green",
   /* 49  0x48 0x3D 0x8B */ "dark slate blue",
   /* 50  0x2F 0x4F 0x4F */ "dark slate grey",
   /* 51  0x00 0xCE 0xD1 */ "dark turquoise",
   /* 52  0x94 0x00 0xD3 */ "dark violet",
   /* 53* 0xFF 0x1E 0x93 */ "deep pink",
   /* 54  0x00 0xBF 0xFF */ "deep sky blue",
   /* 55  0x69 0x69 0x69 */ "dim gray",
   /* 56  0x1E 0x90 0xFF */ "dodger blue",
   /* 57  0xB2 0x22 0x22 */ "firebrick",
   /* 58  0xFF 0xFA 0xF0 */ "floral white",
   /* 59  0x22 0x8B 0x22 */ "forest green",
   /* 60  0xFF 0x00 0xFF */ "magenta",
   /* 61  0xDC 0xDC 0xDC */ "gainsboro",
   /* 62* 0xFA 0xFA 0xD3 */ "light goldenrod yellow",
   /* 63  0xFF 0xD7 0x00 */ "gold",
   NULL
  };

/* -------------------------------------------------------------------------------- */
/*
static char* font_name[] = 
{
    "Console",          
    "Times New Roman",  
    "Small Fonts",      
    "MS Sans Serif",    

    "Arial",            
    "Script",           
    "Times New Roman",  
    "Script",           
    "Courier New",      
    "Times New Roman",  
    "Courier New Bold", 
    NULL
};
*/


static struct 
  { 
   int width; 
   int height; 
  } font_metrics[][11] = 
  { 
   /* DefaultFont */
   {{0,0},{8,8},{16,16},{24,24},{32,32},{40,40},{48,48},{56,56},{64,64},{72,72},{80,80}}, 
   /* TriplexFont */ 
   {{0,0},{13,18},{14,20},{16,23},{22,31},{29,41},{36,51},{44,62},{55,77},{66,93},{88,124}}, 
   /* SmallFont */
   {{0,0},{3,5},{4,6},{4,6},{6,9},{8,12},{10,15},{12,18},{15,22},{18,27},{24,36}},
   /* SansSerifFont  */
   {{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, 
   /* GothicFont */
   {{0,0},{13,19},{14,21},{16,24},{22,32},{29,42},{36,53},{44,64},{55,80},{66,96},{88,128}}, 
   
   /* I am not sure about metrics of following fonts */
   /* ScriptFont */
   {{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, 
   /* SimplexFont */   
   {{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, 
   /* TriplexScriptFont */
   {{0,0},{13,18},{14,20},{16,23},{22,31},{29,41},{36,51},{44,62},{55,77},{66,93},{88,124}}, 
   /* ComplexFont */
   {{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, 
   /* EuropeanFont */
   {{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}}, 
   /* BoldFont */
   {{0,0},{11,19},{12,21},{14,24},{19,32},{25,42},{31,53},{38,64},{47,80},{57,96},{76,128}} 
  }; 


static int normal_font_size[] = 
  { 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

static enum { ALIGN_NOT_SET, UPDATE_CP, NOT_UPDATE_CP } text_align_mode; 


static unsigned char EmptyBrushBitmap[8] = 
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  
static unsigned char SolidBrushBitmap[8] = 
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  
static unsigned char LineBrushBitmap[8] = 
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
static unsigned char LtslashBrushBitmap[8] = 
  {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
static unsigned char SlashBrushBitmap[8] = 
  {0x81, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0};
static unsigned char BkslashBrushBitmap[8] = 
  {0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x81};
static unsigned char LtbkslashBrushBitmap[8] = 
  {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
static unsigned char HatchBrushBitmap[8] = 
  {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF};
static unsigned char XhatchBrushBitmap[8] = 
  {0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81};
static unsigned char InterleaveBrushBitmap[8] = 
  {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};
static unsigned char WidedotBrushBitmap[8] = 
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
static unsigned char ClosedotBrushBitmap[8] = 
  {0x44, 0x00, 0x11, 0x00, 0x44, 0x00, 0x11, 0x00};

/* -------------------------------------------------------------------------------- */

int add_font_Xwin(Xwin *xw, char *fontname)
  {
   if ((xw != NULL) && ((xw->nfontsloaded) < MAXFONTS))
     {
      /* try to load the given font -- if OK, set current font to it */
      xw->fontinfo[xw->nfontsloaded] = XLoadQueryFont(xw->display, fontname);
      if (xw->fontinfo[xw->nfontsloaded]) 
       {
        xw->fontloaded[xw->nfontsloaded] = TRUE;
        xw->currentfontindex = xw->nfontsloaded;

        /* set font into BOTH real and virtual GCs */ 
        XSetFont(xw->display, xw->gc, (xw->fontinfo[xw->currentfontindex])->fid);
        XSetFont(xw->display, xw->virtualgc, (xw->fontinfo[xw->currentfontindex])->fid);

        (xw->nfontsloaded)++;
        return TRUE;
       }      
     }
  
   return FALSE;
  }


int change_font_height_Xwin(Xwin *xw, int fontindex)
  {
   /* change font if safe to do so and return height in pixels of new font */
   int font_height;


   if (xw == NULL)
     {
      return 0;   /* Xwin invalid -- no font or height */
     }
   
   if (fontindex < 0)   /* special input code for getting height of current valid font without changing */
     {
      /* currentfontindex will be retained */
     } 
   else
     {
      if (fontindex >= (xw->nfontsloaded))  /* if font index invalid, change to 0 */
        {
         xw->currentfontindex = 0;
        }
      else
        { 
         /* change current font index to new one specified */
         xw->currentfontindex = fontindex;
        }
     }

   if (xw->nfontsloaded)  /* must have at least one font loaded or it can't be set */
     { 
      /* set the font into the GC and find the pixel height of the font */
      XSetFont(xw->display, xw->gc, (xw->fontinfo[xw->currentfontindex])->fid);
      XSetFont(xw->display, xw->virtualgc, (xw->fontinfo[xw->currentfontindex])->fid);
      font_height = (xw->fontinfo[xw->currentfontindex])->ascent 
                                    + (xw->fontinfo[xw->currentfontindex])->descent;
   
      return font_height;
     }

   return 0;    /* no valid font installed */ 
  }



void load_fontlist_Xwin(Xwin *xw, char *fontlist[])
  {
   int i;

   for (i=0; fontlist[i]; i++)   /* NULL signifies end of list */
     {
      if (add_font_Xwin(xw,fontlist[i]))  
        {
        }
     }
/*
#ifdef DEBUG_XWINLITE
         if (NULL != outdebug)
           {
            fprintf(outdebug,"%d fonts now loaded\n",xw->nfontsloaded);
            fflush(outdebug); 
           }
#endif
*/
  }



/* -------------------------------------------------------------------------------------- */

void visible_inside_rect_Xwin(Xwin *xw, int x, int y, int wid, int hgt)
  {
   XRectangle rect[2];

   /* NOTE: multiple rectangles may be specified as clipping regions -- all interiors
            of specified rectangles allow drawing -- in order to allow use of the
            text scaling buffer (off bottom of screen) when clipping regions are in effect,
            it is necessary to also include this area everytime this function is called. */
 
   /* define text scaling buffer to be always visible */
   rect[0].x = 0;
   rect[0].y = xw->height;
   rect[0].width = xw->width;
   rect[0].height = 36;

   /* make sure defined rectangles make sense and also do not extend into text buffer 
      at bottom of screen */ 

   if ((wid <= 0) || (hgt <= 0) || (x >= xw->width) 
                  || (y >= xw->height) || ((x + xw->width) <= 0) || ((y + xw->height) <= 0))
     { 
      /* everything is invisible except text scaling buffer */
      XSetClipRectangles(xw->display, xw->virtualgc, 0, 0, rect, 1, Unsorted);
      return;
     }

   if ((y+hgt) > xw->height)
     {
      hgt = xw->height - y;  /* force bottom of rectangle at buffer boundary */
      if (hgt <= 0)
        {
         /* everything is invisible except text scaling buffer */
         XSetClipRectangles(xw->display, xw->virtualgc, 0, 0, rect, 1, Unsorted);
         return;
        }
     }

   /* some rectangle other than buffer must be visible -- add it to list */
   rect[1].x = x;
   rect[1].y = y;
   rect[1].width = wid;
   rect[1].height = hgt;

   XSetClipRectangles(xw->display, xw->virtualgc, 0, 0, rect, 2, Unsorted);
  }


void visible_outside_rect_Xwin(Xwin *xw, int x, int y, int wid, int hgt)
  {
   XRectangle rect[5];
   int x0,y0, x1,y1,    x2,y2, x3,y3,     x4,y4, x5,y5,    x6,y6, x7,y7;
   int ct;

   /* make sure defined rectangles make sense and also do not extend into text buffer 
      at bottom of screen */ 

   if (wid <= 0)  /* nothing to do -- no width */
     {
      return;
     }

   if (hgt <= 0)  /* nothing to do -- no height */
     {
      return;
     }

   if (x >= xw->width)  /* nothing to do -- offscreen */
     {
      return;
     }

   if (y >= xw->height)  /* nothing to do -- offscreen */
     {
      return;
     }

   if ((x + xw->width) <= 0)  /* nothing to do -- offscreen */
     {
      return;
     }

   if ((y + xw->height) <= 0)  /* nothing to do -- offscreen */
     {
      return;
     }

   hgt = xw->height - y;  /* force bottom of rectangle at buffer boundary */


   int have01 = TRUE;
   int have23 = TRUE;
   int have45 = TRUE;
   int have67 = TRUE;
   
   /* at first, assume rectangle described fits entirely within window */
   /* determine four rectangles outside the described rectangle */ 
   x0 = 0;
   y0 = 0;
   x1 = (x-1);
   y1 = xw->height - 1;
   if (x <= 0)
      have01 = FALSE;

   x2 = x;
   y2 = 0;
   x3 = x+(wid-1);
   y3 = (y-1);
   if (y <= 0)
      have23 = FALSE;
   
   x4 = x;
   y4 = (y+hgt);
   x5 = x+(wid-1);
   y5 = xw->height - 1;
   if (y+hgt > xw->height)
      have45 = FALSE;
   
   x6 = (x+wid);
   y6 = 0;
   x7 = xw->width - 1;
   y7 = xw->height - 1;
   if (x+wid > xw->width)
      have67 = FALSE;
   

   /* add rectangles which need to be defined as visible -- will NOT include text buffer */
   ct = 0;

   if (have01)
     {
      rect[ct].x       = x0;
      rect[ct].y       = y0;
      rect[ct].width   = (x1+1)-x0;
      rect[ct].height  = (y1+1)-y0;
     }
   ct++;

   if (have23)
     {
      rect[ct].x       = x2;
      rect[ct].y       = y2;
      rect[ct].width   = (x3+1)-x2;
      rect[ct].height  = (y3+1)-y2;
     }
   ct++;

   if (have45)
     {
      rect[ct].x       = x4;
      rect[ct].y       = y4;
      rect[ct].width   = (x5+1)-x4;
      rect[ct].height  = (y5+1)-y4;
     }
   ct++;

   if (have67)
     {
      rect[ct].x       = x6;
      rect[ct].y       = y6;
      rect[ct].width   = (x7+1)-x6;
      rect[ct].height  = (y7+1)-y6;
     }
   ct++;

   /* add one more rectangle for text scaling buffer */
   rect[ct].x       = 0;
   rect[ct].y       = xw->height;
   rect[ct].width   = xw->width;
   rect[ct].height  = 36;
   ct++;

   if (ct)
     {
      XSetClipRectangles(xw->display, xw->virtualgc, 0, 0, rect, ct, Unsorted);
     }
  }


void visible_all_Xwin(Xwin *xw)
  {
   /* make whole screen visible, including text buffer */ 
   visible_inside_rect_Xwin(xw,0,0,xw->width,xw->height+36);  
  }


/* ---------------------------------------------------------------------------- */

/* guaranteed to be current but inefficient in large numbers */
unsigned long get_pixel_Xwin(Xwin *xw, int x, int y)
  {
   int src_x, src_y, dest_x, dest_y;
   int i,j;
   unsigned long plane_mask = 0x00FFFFFF;
   unsigned long pixeltemp;
   XImage *pixel_ximage = NULL;

   if (xw == NULL)
     {
      return NOTAPIXEL;
     }   

   if ((x < 0) || (y < 0) || (x >= xw->width) || (y >= xw->height))
     {
      /* (x,y) is offscreen */
      return NOTAPIXEL;
     }

   /* this creates an Ximage with a single pixel in it */
   pixel_ximage =  XGetImage(xw->display, xw->vpixmap[xw->draw_page], x, y, 1, 1, plane_mask, ZPixmap);
   if (pixel_ximage != NULL)
     {
      pixeltemp = XGetPixel(pixel_ximage,0,0);

      /* kill off the temporary Ximage structure */   
      XDestroyImage(pixel_ximage);
     }
   return pixeltemp;
  }
  

/* more efficient in large numbers but must not putpixels to same area while reading 
   or may get erroneous pixels -- this implementation assumes screen width and height 
   are 0 mod 8 */

unsigned long get_pixel_cache_Xwin(Xwin *xw, int x, int y)
  {
   unsigned long plane_mask = 0x00FFFFFF;
   int x_block8 = -1;
   int y_block8 = -1;
   int chgx_block8 = -2;
   int chgy_block8 = -2;
   int cachex_block8 = -3;
   int cachey_block8 = -3;
   int cache_change = TRUE;
   
   if (xw == NULL)
     {
      return NOTAPIXEL;
     }   

   if ((x < 0) || (y < 0) || (x >= xw->width) || (y >= xw->height))
     {
      /* (x,y) is offscreen */
      return NOTAPIXEL;
     }

   if ((xw->pixelcache_x >= 0) && (xw->pixelcache_y >= 0))
     {
      cachex_block8 = ((xw->pixelcache_x >> 3) << 3);
      cachey_block8 = ((xw->pixelcache_y >> 3) << 3);
     }

   if ((xw->putchange_x >= 0) && (xw->putchange_y >= 0))
     {
      chgx_block8 = ((xw->putchange_x >> 3) << 3);
      chgy_block8 = ((xw->putchange_y >> 3) << 3);
     }

   x_block8 = ((x >> 3) << 3);
   y_block8 = ((y >> 3) << 3);

   if ((x_block8 == cachex_block8) &&  (y_block8 == cachey_block8))
     {
      /* if from same cache block as already in memory, assume no need to reread... */
      cache_change = FALSE;
     }

   if ((x_block8 == chgx_block8) &&  (y_block8 == chgy_block8))
     {
      /* ...unless a prior call to putvpixel affected the same block */
      cache_change = TRUE;
     }

   if ((cache_change) ||  (xw->force_killcache))
     {
      if (xw->pixelcache != NULL)
        {
         XDestroyImage(xw->pixelcache);
        }
      xw->pixelcache = NULL;

      if (xw->fullscreen_ximage != NULL)
        {
         XDestroyImage(xw->fullscreen_ximage);
        }
      xw->fullscreen_ximage = NULL;
      xw->force_killcache = FALSE;
     }

   if (NULL == xw->pixelcache)
     {
      xw->pixelcache =  XGetImage(xw->display, xw->vpixmap[xw->draw_page], x_block8, y_block8, 8, 8, plane_mask, ZPixmap);
     }

   xw->pixelcache_x = x; 
   xw->pixelcache_y = y; 
   xw->putchange_x = -1; 
   xw->putchange_y = -1; 

   /* at this point pixel cache exists and holds an 8 x 8 block of pixels -- locate pixel within
      that block */
   return XGetPixel(xw->pixelcache,x-x_block8,y-y_block8);
  }


/* more efficient when dealing with large volume fullscreen random lookups within a given
   function not concerned with additional "putpixels" to screen such as image output to file,
   stretch BLTs, or floodfills -- XImage is full screen size (not including text buffer) */

unsigned long get_pixel_fullcache_Xwin(Xwin *xw, int x, int y)
  {
   unsigned long plane_mask = 0x00FFFFFF;

   if (xw == NULL)
     {
      return NOTAPIXEL;
     } 

   if ((x < 0) || (y < 0) || (x >= xw->width) || (y >= xw->height))
     {
      /* (x,y) is offscreen */
      return NOTAPIXEL;
     }

   if (xw->force_killcache)
     {   
      if (xw->pixelcache != NULL)
        {
         XDestroyImage(xw->pixelcache);
        }
      xw->pixelcache =  NULL;

      if (xw->fullscreen_ximage != NULL)
        {
         XDestroyImage(xw->fullscreen_ximage);
        }
      xw->fullscreen_ximage = NULL;
      xw->force_killcache = FALSE;
     }

   if (xw->fullscreen_ximage == NULL)
     {
      xw->fullscreen_ximage =  XGetImage(xw->display, xw->vpixmap[xw->draw_page], 
                                       0, 0, xw->width, xw->height, plane_mask, ZPixmap);
     }

   /* at this point fullscreen pixel cache exists */
   return XGetPixel(xw->fullscreen_ximage,x,y);
  }
  

/* put_pixel_Xwin works with either cached or uncached getpixel directly to virtual screen
   -- caller is responsible to detect dirty pixels or other changes to screen if reloading
   of getpixel cache is required (set xw->force_killcache = TRUE) -- however, that is not 
   necessary if only get_pixel_Xwin is used, since it always gets current virtual pixels */
  
void put_pixel_Xwin(Xwin *xw, int x, int y, unsigned long pixel)
  { 
   XSetForeground(xw->display, xw->virtualgc, pixel);
   XDrawPoint(xw->display,xw->vpixmap[xw->draw_page], xw->virtualgc,x,y); 
   XSetForeground(xw->display, xw->virtualgc, xw->foregroundpixel);
   xw->putchange_x = x;
   xw->putchange_y = y;
  }


/* this version of putpixel works directly on fullscreen pixel cache, and NOT on 
   virtual screen itself -- useful for floodfill function */ 
void put_pixel_fullcache_Xwin(Xwin *xw, int x, int y, unsigned long pixel)
  { 
   unsigned long plane_mask = 0x00FFFFFF;

   if (xw == NULL)
     {
      return;
     } 

   if ((x < 0) || (y < 0) || (x >= xw->width) || (y >= xw->height))
     {
      /* (x,y) is offscreen */
      return;
     }

   if (xw->fullscreen_ximage == NULL)
     {
      xw->fullscreen_ximage =  XGetImage(xw->display, xw->vpixmap[xw->draw_page], 
                                       0, 0, xw->width, xw->height, plane_mask, ZPixmap);
     }

   /* at this point fullscreen pixel cache exists -- not a Drawable, so use XImage functions 
      to access pixels -- don't worry about dirty cache -- let caller handle that if needed
      by setting xw->force_killcache */

   XPutPixel(xw->fullscreen_ximage, x, y, pixel);
  }

/* ---------------------------------------------------------------------------- */

/* the following use the getpixel variants above to get a pixel and test 
   whether it:

        (testnotb)   is NOT the color of background pixel
        (test)       is the color of the specified pixel

   The first test is TRUE against ALL BUT one color, the second test
   is TRUE against ONLY ONE color.

   NOTE: if (x,y) is offscreen, all variants of getpixel() return NOTAPIXEL, which will 
         NEVER match any ACTUAL pixel value.  This is convenient for detecting a "border"
         automatically at all edges of window.  This is critical for floodfills.
*/         

int testnotb_pixel_Xwin(Xwin *xw, int x, int y)
  {
   if (get_pixel_Xwin(xw,x,y) != xw->currentbgcolorpixel)
     {
      return TRUE;
     }
   return FALSE;
  }
  
int testnotb_pixel_cache_Xwin(Xwin *xw, int x, int y)
  {
   if (get_pixel_cache_Xwin(xw,x,y) != xw->currentbgcolorpixel)
     {
      return TRUE;
     }
   return FALSE;
  }

int testnotb_pixel_fullcache_Xwin(Xwin *xw, int x, int y)
  {
   if (get_pixel_fullcache_Xwin(xw,x,y) != xw->currentbgcolorpixel)
     {
      return TRUE;
     }
   return FALSE;
  }
 
int test_pixel_Xwin(Xwin *xw, int x, int y, unsigned long pixel)
  {
   if (get_pixel_Xwin(xw,x,y) == pixel)
     {
      return TRUE;
     }
   return FALSE;
  }
  
int test_pixel_cache_Xwin(Xwin *xw, int x, int y, unsigned long pixel)
  {
   if (get_pixel_cache_Xwin(xw,x,y) == pixel)
     {
      return TRUE;
     }
   return FALSE;
  }

int test_pixel_fullcache_Xwin(Xwin *xw, int x, int y, unsigned long pixel)
  {
   if (get_pixel_fullcache_Xwin(xw,x,y) == pixel)
     {
      return TRUE;
     }
   return FALSE;
  }
  

unsigned long getpixel_Xwin(Xwin *xw, int x, int y)
  {
   if (xw->use_fullcache)
     {
      return get_pixel_fullcache_Xwin(xw,x,y);
     }
   else
     { 
      return get_pixel_Xwin(xw,x,y);
     }
   return TRUE;
  }


int testpixel_Xwin(Xwin *xw, int x, int y, unsigned long pixel)
  {
   if (xw->use_fullcache)
     {
      return test_pixel_fullcache_Xwin(xw,x,y,pixel);
     }
   else
     { 
      return test_pixel_Xwin(xw,x,y,pixel);
     }
   return TRUE;
  }


void putpixel_Xwin(Xwin *xw, int x, int y, unsigned long pixel)
  {
   if (xw->use_fullcache)
     {
      put_pixel_fullcache_Xwin(xw,x,y,pixel);
     }
   else
     { 
      put_pixel_Xwin(xw,x,y,pixel);
     }
   return;
  }


/* ------------------------------------------------------------------------- */

/* The following are glue functions which satisfy calls found in old GLF gphbsubs.c 
   code used to provide a floodfill function */

/* replacement for gphpttestonly() */
int testnotbpixel_Xwin(Xwin *xw, int x, int y)
  {
   if (xw->use_fullcache)
     {
      /* test only! */ 
      /* put_pixel_Xwin(xw,x,y,xw->palette[1].pixel); */   /* draws directly to screen IN BLUE */
      return testnotb_pixel_fullcache_Xwin(xw,x,y);
     }
   else
     { 
      return testnotb_pixel_Xwin(xw,x,y);
     }
   return TRUE;
  }



/* replacement for gphpt() */
int pointpixel_Xwin(Xwin *xw, int x, int y, int code, int wx0, int wy0, int wx1, int wy1, int biton)
  {
   unsigned long onpixel;
   unsigned long offpixel;
   
   biton = FALSE;

   if (xw == NULL)
     {
      return FALSE;
     }

   if ((x < wx0) || (x > wx1)        || (y < wy0)  || (y > wy1)       ||
       (y < 0)   || (y > xw->height) ||  (x < 0)   || (x > xw->width)     )

     {
      return FALSE;
     }

   offpixel = xw->currentbgcolorpixel;
   onpixel = xw->currentcolorpixel;
 
   switch (code)
     {
      case 2:
         putpixel_Xwin(xw,x,y,offpixel);
         biton = FALSE;
         break;

      case 3:
         if (testnotbpixel_Xwin(xw,x,y))
           {  
            biton =  TRUE;
           }
         else
           {
            biton =  FALSE;
           }
         break;

      case 4:
         if (testnotbpixel_Xwin(xw,x,y))
           {  
            putpixel_Xwin(xw,x,y,offpixel);
            biton =  FALSE;
           }
         else
           {
            putpixel_Xwin(xw,x,y,onpixel);
            biton =  TRUE;
           }
         break;

      default:
         putpixel_Xwin(xw,x,y,onpixel);
         biton = TRUE;
     }

   return biton;
  }



void fillrect_Xwin(Xwin *xw, int x, int y, int width, int height)
  { 
   XFillRectangle(xw->display, xw->vpixmap[xw->draw_page], xw->virtualgc, x, y, width, height);
  }


void drawrect_Xwin(Xwin *xw, int x, int y, int width, int height)
  { 
   XDrawRectangle(xw->display, xw->vpixmap[xw->draw_page], xw->virtualgc,  x, y, width, height);
  }


void clearrect_Xwin(Xwin *xw, int x, int y, int width, int height)
  { 
#ifdef DEBUG_XWINLITE
       if (NULL != outdebug)
         {
          fprintf(outdebug,"clearrect_Xwin(xw,x=%d,y=%d,width=%d,height=%d)\n",
                           x, y, width, height);
          fflush(outdebug);
         }
#endif
   XSetForeground(xw->display, xw->virtualgc, xw->currentbgcolorpixel);
   XSetBackground(xw->display, xw->virtualgc, xw->currentcolorpixel);

   XFillRectangle(xw->display, xw->vpixmap[xw->draw_page], xw->virtualgc, x, y, width, height);

   XSetForeground(xw->display, xw->virtualgc, xw->currentcolorpixel);
   XSetBackground(xw->display, xw->virtualgc, xw->currentbgcolorpixel);
  }


void drawline_Xwin(Xwin *xw, int x0, int y0, int x1, int y1)
  { 
   unsigned long holdpixel;

   XDrawLine(xw->display, xw->vpixmap[xw->draw_page], xw->virtualgc, x0, y0, x1, y1);
  }


void moveto_Xwin(Xwin *xw, int x, int y)
  { 
   unsigned long holdpixel;

   xw->currentx = x;
   xw->currenty = y;
  }


void drawto_Xwin(Xwin *xw, int x, int y)
  { 
   unsigned long holdpixel;

   XDrawLine(xw->display, xw->vpixmap[xw->draw_page], xw->virtualgc, xw->currentx, xw->currenty, x, y);
   xw->currentx = x;
   xw->currenty = y;
  }

/*
XDrawArc(display, d, gc, x, y, width, height, angle1, angle2)

XFillArc(display, d, gc,  x, y, width, height, angle1, angle2)

XFillPolygon(display, d, gc, points, npoints, shape, mode)

XSetDashes(display, gc, dash_offset, dash_list, n)

XSetLineAttributes(display, gc, line_width, line_style, cap_style, join_style)
      Display *display;
      GC gc;
      unsigned int line_width;
      int line_style;
      int cap_style;
      int join_style;


Arguments
display 	Specifies the connection to the X server.
gc 	Specifies the GC.
line_width 	Specifies the line-width you want to set for the specified GC.
line_style 	Specifies the line-style you want to set for the specified GC. You can pass LineSolid, LineOnOffDash, or LineDoubleDash.
cap_style 	Specifies the line-style and cap-style you want to set for the specified GC. You can pass CapNotLast, CapButt, CapRound, or CapProjecting.
join_style 	Specifies the line join-style you want to set for the specified GC. You can pass JoinMiter, JoinRound, or JoinBevel.
*/


/* ----------------------------------------------------------------------------------------------- */


/* add one brush to the brush list and set up stipple pattern from data (8 bits X 8) */
/* if maximum patterns already allocated, overwrite the last pattern */
/* return value is brush number of last pattern loaded OR -1 if not valid */

int add_brush_Xwin(Xwin *xw, unsigned char pattern[8])
  {
   GC temp_gc;
   unsigned long valuemask = 0;		
   XGCValues values;	
   int i,j;

   unsigned char mask[8] =
     {
      0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 
     };     


   if (xw == NULL)
      return -1;
    
   if ((xw->nbrushesloaded < 0) || (xw->nbrushesloaded > MAXBRUSHES))
     {
      return -1; /* something is WRONG -- quit now */
     }

   if (xw->nbrushesloaded < MAXBRUSHES)
     {
      /* allocate a new pixmap to hold brush pattern */
      xw->brush[xw->nbrushesloaded] = XCreatePixmap(xw->display, xw->vpixmap[xw->draw_page], 8, 8, 1);
     }

   if (xw->nbrushesloaded == MAXBRUSHES)
     {
      xw->nbrushesloaded--; /* if already at maximum, reuse the last brush already allocated */
     }

   /* create a temporary GC to draw pattern data into brush pixmap */
   temp_gc = XCreateGC(xw->display, xw->brush[xw->nbrushesloaded], valuemask, &values);
   if (temp_gc >= 0) 
     {
      /* set up minimal ability to draw into brush Pixmap */
      XSetBackground(xw->display, temp_gc, BlackPixel(xw->display,xw->screennum));
      XSetLineAttributes(xw->display, temp_gc, 0, LineSolid, CapButt, JoinBevel);
      XSetFillStyle(xw->display, temp_gc, FillSolid);

      /* clear this brush pattern to 0s */
      XSetForeground(xw->display, temp_gc, BlackPixel(xw->display,xw->screennum));
      XFillRectangle(xw->display, xw->brush[xw->nbrushesloaded], temp_gc, 0, 0, 8, 8);

      XSetForeground(xw->display, temp_gc, WhitePixel(xw->display,xw->screennum));
      /* draw bits into brush from pattern data */
      for (i=0; i<8; i++)
        {
         for (j=0; j<8; j++)
           {
            if (pattern[i] & mask[j])
              {
               XDrawPoint(xw->display,xw->brush[xw->nbrushesloaded],temp_gc,j,i);
              }                 
           }
        }  

      /* now that brush pattern exists, kill off the temporary GC */
      XFreeGC(xw->display, temp_gc);

      xw->nbrushesloaded++;
      return (xw->nbrushesloaded - 1); 
     }
   else
     {
     } 
   return -1; 
  }


/* replaces pattern only if brush number already exists in list -- return TRUE if replaced */
int replace_brush_Xwin(Xwin *xw, int brushno, unsigned char const pattern[8])
  {
   GC temp_gc;
   unsigned long valuemask = 0;		
   XGCValues values;	
   int i,j;

   unsigned char mask[8] =
     {
      0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 
     };     


   if (xw == NULL)
      return FALSE;
    
   if ((xw->nbrushesloaded < 0) || (xw->nbrushesloaded > MAXBRUSHES))
     {
      return FALSE; /* something is WRONG -- quit now */
     }

   /* nbrushes is a valid value, but may be 0 */
   if ((brushno < 0) || (brushno >= xw->nbrushesloaded))
     {
      return FALSE;
     }

   /* brush number exists in brush list -- so pattern exists for replacement */

   /* create a temporary GC to draw pattern data into brush pixmap */
   temp_gc = XCreateGC(xw->display, xw->brush[brushno], valuemask, &values);
   if (temp_gc >= 0) 
     {
      /* set up minimal ability to draw into brush Pixmap */
      XSetBackground(xw->display, temp_gc, BlackPixel(xw->display,xw->screennum));
      XSetLineAttributes(xw->display, temp_gc, 0, LineSolid, CapButt, JoinBevel);
      XSetFillStyle(xw->display, temp_gc, FillSolid);

      /* clear this brush pattern to 0s */
      XSetForeground(xw->display, temp_gc, BlackPixel(xw->display,xw->screennum));
      XFillRectangle(xw->display, xw->brush[brushno], temp_gc, 0, 0, 8, 8);

      XSetForeground(xw->display, temp_gc, WhitePixel(xw->display,xw->screennum));
      /* draw bits into brush from pattern data */
      for (i=0; i<8; i++)
        {
         for (j=0; j<8; j++)
           {
            if (pattern[i] & mask[j])
              {
               XDrawPoint(xw->display,xw->brush[brushno],temp_gc,j,i);
              }                 
           }
        }  

      /* now that brush pattern exists, kill off the temporary GC */
      XFreeGC(xw->display, temp_gc);
      return TRUE; 
     }
   else
     {
     } 
   return FALSE; 
  }


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


static int a_stackpos;

static int a_offset;
static int a_quit = 0;
static int a_wx0, a_wy0, a_wx1, a_wy1;
static int a_code = 1;
static int a_patnbit[8];
static int a_x;

static int a_lowx, a_lowy;
static int a_origx, a_origy;
static int a_up = TRUE;



/* keep technique for measuring polygon segment crossings 
   hidden from outside (see gphareameasure()) */
static short int ditherlink[MAXDITHERLINKS+1];
static short int ditherymax = -1;
static short int ditherymin = -1;

paintlist ditherlist[MAXPAINTLIST];
int ditherlistnum = 0;

int put_dithlist(int i, int ixl, int ixr, int iy)

  {
   paintlist temp;

   if (i > (MAXPAINTLIST-1))
     {
      return FALSE;
     }
   temp.xl = ixl;
   temp.xr = ixr;
   temp.y = iy;

   /* default to clearing ylink */
   temp.ylink = -1;
   
   ditherlist[i] = temp;

   return TRUE;
  }


int get_dithlist(int i, int *pxl, int *pxr, int *py)

  {
   paintlist temp;

   if ((i > (MAXPAINTLIST-1)) || (i >= ditherlistnum))
     {
      *pxl = 0;
      *pxr = 0;
      *py = 0;
      return FALSE;
     }
   temp = ditherlist[i];
   *pxl = temp.xl;
   *pxr = temp.xr;
   *py =  temp.y;
   /* this version of the function ignores ylink */

   return TRUE;
  }

int add_dithlist(int ixl, int ixr, int iy)

  {
   paintlist temp;

   if (ditherlistnum > (MAXPAINTLIST-2))
     {
      return FALSE;
     }
   temp.xl = ixl;
   temp.xr = ixr;
   temp.y = iy;
   temp.ylink = -1;     /* clear link for optional y list lookup */
   ditherlist[ditherlistnum] = temp;
   ditherlistnum++;

   return TRUE;
  }


void clear_dithlist(void)

  {
   int i;
   
   ditherlistnum = 0;
   
   /* because ylinks are now trashed, clear any ditherlinks */
   for (i=0; i<MAXDITHERLINKS; i++)
     {
      ditherlink[i] = -1;
     }
   ditherymin = -1;
   ditherymax = -1;  
  }

#pragma pack()

int bug_wx0 = -1;
int bug_wy0 = -1;
int bug_wx1 = -1;
int bug_wy1 = -1;

int is_bug_window(int x, int y)
  {
   if ((x < bug_wx0) || (x > bug_wx1) ||
       (y < bug_wy0) || (y > bug_wy1))
     {
      return FALSE;
     }

   return TRUE;
  }



void walkborder_Xwin(Xwin *xw, int *x, int *y)   
                                  /* walks clockwise around inside border
                                     one point at a time -- assumes already
                                     on border */
  {
   int ix, iy;

   ix = *x;
   iy = *y;

   /* x and y change position depending only upon the state of pixels
      surrounding the starting (x,y) pixel and ongoing direction of movement
      (up or down) -- effect is to cling to borders and move around obstacles
      one step at a time.  Once adjacent to a border, (x,y) will follow the
      border completely, guaranteed to revisit a starting adjacent pixel */

   if (a_up)
     {
      if (!testnotbpixel_Xwin(xw,ix-1,iy))  /* left, down */
        {
         ix--;
         if (!testnotbpixel_Xwin(xw,ix,iy-1))
           {
            iy--;
            a_up = FALSE;
           }
         else
           {
            a_up = TRUE;
           }
         *x = ix;
         *y = iy;
         if (iy <= a_lowy)
           {
            if (iy == a_lowy)
              {
               if (ix < a_lowx)
                 {
                  a_lowx = ix;
                 }
              }
            else
              {
               a_lowy = iy;
               a_lowx = ix;
              }
           }
         return;
        }

      if (!testnotbpixel_Xwin(xw,ix,iy+1))   /* up, left */
        {
         iy++;
         a_up = TRUE;
         if (!testnotbpixel_Xwin(xw,ix-1,iy))
           {
            ix--;
           }
         *x = ix;
         *y = iy;
         if (iy <= a_lowy)
           {
            if (iy == a_lowy)
              {
               if (ix < a_lowx)
                 {
                  a_lowx = ix;
                 }
              }
            else
              {
               a_lowy = iy;
               a_lowx = ix;
              }
           }
         return;
        }

      if (!testnotbpixel_Xwin(xw,ix+1,iy))  /* right, up */
        {
         ix++;
         if (!testnotbpixel_Xwin(xw,ix,iy+1))
           {
            iy++;
            a_up = TRUE;
           }
         else
           {
            a_up = FALSE;
           }
         *x = ix;
         *y = iy;
         if (iy <= a_lowy)
           {
            if (iy == a_lowy)
              {
               if (ix < a_lowx)
                 {
                  a_lowx = ix;
                 }
              }
            else
              {
               a_lowy = iy;
               a_lowx = ix;
              }
           }
         return;
        }

      if (!testnotbpixel_Xwin(xw,ix,iy-1)) /* down, right */
        {
         iy--;
         a_up = FALSE;
         if (!testnotbpixel_Xwin(xw,ix+1,iy))
           {
            ix++;
           }
         *x = ix;
         *y = iy;
         if (iy <= a_lowy)
           {
            if (iy == a_lowy)
              {
               if (ix < a_lowx)
                 {
                  a_lowx = ix;
                 }
              }
            else
              {
               a_lowy = iy;
               a_lowx = ix;
              }
           }
         return;
        }
     }
   else
     {
      if (!testnotbpixel_Xwin(xw,ix+1,iy))  /* right, up */
        {
         ix++;
         if (!testnotbpixel_Xwin(xw,ix,iy+1))
           {
            iy++;
            a_up = TRUE;
           }
         else
           {
            a_up = FALSE;
           }
         *x = ix;
         *y = iy;
         if (iy <= a_lowy)
           {
            if (iy == a_lowy)
              {
               if (ix < a_lowx)
                 {
                  a_lowx = ix;
                 }
              }
            else
              {
               a_lowy = iy;
               a_lowx = ix;
              }
           }
         return;
        }

      if (!testnotbpixel_Xwin(xw,ix,iy-1)) /* down, right */
        {
         iy--;
         a_up = FALSE;
         if (!testnotbpixel_Xwin(xw,ix+1,iy))
           {
            ix++;
           }
         *x = ix;
         *y = iy;
         if (iy <= a_lowy)
           {
            if (iy == a_lowy)
              {
               if (ix < a_lowx)
                 {
                  a_lowx = ix;
                 }
              }
            else
              {
               a_lowy = iy;
               a_lowx = ix;
              }
           }
         return;
        }
      if (!testnotbpixel_Xwin(xw,ix-1,iy))  /* left, down */
        {
         ix--;
         if (!testnotbpixel_Xwin(xw,ix,iy-1))
           {
            iy--;
            a_up = FALSE;
           }
         else
           {
            a_up = TRUE;
           }
         *x = ix;
         *y = iy;
         if (iy <= a_lowy)
           {
            if (iy == a_lowy)
              {
               if (ix < a_lowx)
                 {
                  a_lowx = ix;
                 }
              }
            else
              {
               a_lowy = iy;
               a_lowx = ix;
              }
           }
         return;
        }

      if (!testnotbpixel_Xwin(xw,ix,iy+1))   /* up, left */
        {
         iy++;
         a_up = TRUE;
         if (!testnotbpixel_Xwin(xw,ix-1,iy))
           {
            ix--;
           }
         *x = ix;
         *y = iy;
         if (iy <= a_lowy)
           {
            if (iy == a_lowy)
              {
               if (ix < a_lowx)
                 {
                  a_lowx = ix;
                 }
              }
            else
              {
               a_lowy = iy;
               a_lowx = ix;
              }
           }
         return;
        }
     }
  }


void findborder_Xwin(Xwin *xw, int x, int y)

  {
   /* scan left until blocked */
   x--;

   while (!testnotbpixel_Xwin(xw,x,y))
     {
      x--;
     }
   x++;

   /* try walking around blockage -- it could either be the actual border
      or it could be a local obstruction entirely inside the border */
   a_origx = x;
   a_origy = y;
   a_lowx = x;
   a_lowy = y;
   a_up = TRUE;


   for (;;)    /* loop breaks when a border point is verified
                                 by tracing entire border back to that point */
     {
      walkborder_Xwin(xw,&x,&y);

      /* the following change was made (July 2004 GLF) while debugging
         a problem with conversion to GCC.  The original check for a_up
         status which ostensibly was to prevent lockup in an anticipated situation,
         appeared to actually cause a lockup in a specific case during testing.
         removal of the test of a_up seems to have allowed the program
         to function normally.  It is uncertain at this time whether the current
         state of this code is correct, but for now it seems to work */
         
      if ((a_origx == x) && (a_origy == y) /* DEBUG! */ /* & (a_up) */)
        {
         break;   /* has traced border & revisited point -- note that checking
                     a_up is intended to ensure proper tracing of narrow 
                     (1 pixel wide) routes */
        }

      if (y == a_origy)  /* if original row is revisited... */
        {
         if (x < a_origx)  /* this was a "polyp" or other local blockage */
           {
            /* resume looking for border -- scan left until blocked */
            x--;

            while (!testnotbpixel_Xwin(xw,x,y))
              {
               x--;
              }
            x++;
            a_origx = x;
            a_origy = y;
            a_lowx = x;
            a_lowy = y;
            a_up = TRUE;
           }
         /* ...otherwise, this is possibly on far right of border */
        }
     }
  }


int a_scanct = 0;


int borderfill_Xwin(Xwin *xw, int x, int y, unsigned long fillpixel)
                      /* RECURSIVE! */

  {
   int belowxl, belowxr;
   int nextx, nextxr;
   int xl, xr;
   int count;
   int retval;
   unsigned long holdbackgroundpixel;
   unsigned long temppixel;


   /* FIX 07/10/2004 GLF -- fundamental assumption is that starting 
      point (x,y) is INSIDE the polygon to fill -- if ON THE LINE,
      unpredictable behavior or endless loops might occur.
      The next lines ensure that if starting point is already set, function 
      returns immediately.
   */

   /* GLF: for now, treat this as black/white -- any color other than xw->background blocks fill */

   if (xw == NULL)
     {
      return 0;
     }
   holdbackgroundpixel = xw->currentbgcolorpixel;
      
   if (a_stackpos == 0)  /* if initial call to recursive function */
     {
      /* set up full screen pixel cache to save time and keep initial floodfill offscreen */
      xw->use_fullcache = TRUE;
      xw->force_killcache = TRUE;

      /* get pixel at initial seed point -- this will be considered "background" color of area to 
         fill -- any other color becomes a "wall" -- MUST initially be DIFFERENT than fill color */
      temppixel = getpixel_Xwin(xw,x,y);
      if ((temppixel == fillpixel) || (temppixel == NOTAPIXEL))  /* also won't start if (x,y) is offscreen) */
        { 
         xw->use_fullcache = FALSE;
         xw->force_killcache = TRUE;
         xw->currentbgcolorpixel = holdbackgroundpixel;
         return x;
        } 
      xw->currentbgcolorpixel = temppixel;
     }

   if (testnotbpixel_Xwin(xw,x,y))
     {
      if (a_stackpos == 0) /* only happens when ready to end */
        {
         xw->use_fullcache = FALSE;
         xw->force_killcache = TRUE;
         xw->currentbgcolorpixel = holdbackgroundpixel;
        } 
      return x;     
     } 
   /* end 07/10/2004 fix */   

   a_stackpos++;   /* this keeps track of number of recursive calls */

   /* determine return value: rightmost open (will be filled) x on seed line */
   retval = x + 1;
   while (!testnotbpixel_Xwin(xw,retval,y))
     {
      retval++;
     }
   retval--;

   /* locate and trace out border, keeping track of lowest-left inside point */

   findborder_Xwin(xw,x,y);

   x = a_lowx;
   y = a_lowy;

   /* (x,y) is now low-left point inside border */
   xl = x;

   /* scan right to border */
   xr = x + 1;
   while (!testnotbpixel_Xwin(xw,xr,y))
     {
      xr++;
     }
   xr--;

   /* by definition of low-left point inside border, it can be assumed that
      a border line exists underneath each point in its horizontal line */

   belowxl = BEYONDEXTREMELEFT; /* these values are far offscreen to prevent */
   belowxr = BEYONDEXTREMERIGHT;  /* scanning below lowest line actually used */

   for (;;)  /* loop breaks when polygon is filled, see below */
     {
      /* fill the current line */
      for (x=xl; x <= xr; x++)
        {
         putpixel_Xwin(xw,x,y,fillpixel);
        }
      /* store the range of this segment for later dithering */
      add_dithlist(xl,xr,y);

      /* if current line extends past previous line, there may exist
         unfilled partial polygons below it */

      for (x=xl; x < belowxl; x++) /* scan leftward of previous line */
        {
         if (!testnotbpixel_Xwin(xw,x,y-1))
           {
            x = borderfill_Xwin(xw,x,y-1,fillpixel); /* RECURSIVE: fill polygon below */
           }
        }

      for (x=belowxr+1; x <=xr; x++) /* scan rightward of previous line */
        {
         if (!testnotbpixel_Xwin(xw,x,y-1))
           {
            x = borderfill_Xwin(xw,x,y-1,fillpixel); /* RECURSIVE: fill polygon below */
           }
        }

      /* look upward for any split-off areas to fill */
      nextx = BEYONDEXTREMERIGHT+2; /* this high value prevents unnecessary scanning at end */
      count = 0;

      for (x=xl; x<=xr; x++)
        {
         if (!testnotbpixel_Xwin(xw,x,y+1))   /* look upward... */
           {
            if (count == 0)
              {
               nextx = x;       /* find leftmost open area above */
              }
            count++;            /* count areas above -- 2 or more mean split */

            /* look ahead at next line to see if any breaks exist to right */
            nextxr = x + 1;
            while (!testnotbpixel_Xwin(xw,nextxr,y+1))
              {
               nextxr++;
              }
            nextxr--;
            x = nextxr + 1;

            if (count > 1) /* splits exist if count at least 2... */
              {
               break;
              }
           }
        }

      if (count > 1)   /* splits exist... */
        {
         for (x=nextx; x <=xr; x++)
           {
            if (!testnotbpixel_Xwin(xw,x,y+1))  /* look upward... */
              {
               x = borderfill_Xwin(xw,x,y+1,fillpixel); /* RECURSIVE: fill polygon above */
              }
           }
        }

      if (count != 1)
        {
         /* If count > 1, all higher lines have been filled.  If count
            == 0, no higher lines exist to fill.  In either case, finish. */
         break;
        }

      /* NOTE: count == 1 */

      /* non-recursive continuation to area above */
      belowxl = xl;
      belowxr = xr;
      x = nextx;
      y++;

      /* scan next line left */
      xl = x - 1;
      while (!testnotbpixel_Xwin(xw,xl,y))
        {
         xl--;
        }
      xl++;
      xr = nextxr;  /* since count == 1, nextxr was already done above */
     }

   a_stackpos--;

   if (a_stackpos == 0) /* only happens when ready to end */
     {
      xw->currentbgcolorpixel = holdbackgroundpixel;  
      xw->use_fullcache = FALSE;
      xw->force_killcache = TRUE;
     } 

   return retval;
  }


void areaflood_Xwin(Xwin *xw, int x0, int y0, unsigned long fillpixel)
  {
   int x, xl, xr, y, i, toss;

   if ((x0 < 0) || (x0 >= xw->width) || (y0 < 0) || (y0 >= xw->height))
     {
      return;
     }

   a_stackpos = 0;   /* MUST do this before first call to (recursive) borderfill_Xwin() */
   a_quit = 0;

   a_wx0 = 0;
   a_wy0 = 0;
   a_wx1 = xw->width-1;
   a_wy1 = xw->height-1;

   clear_dithlist();
   
   /* RECURSIVE fill procedure -- uses a fullscreen pixel cache (already set up) and  
          only fills pixels within the cache XImage -- creaes a list of line segments 
          which need to be drawn on virtual (or actua) window -- this provides an 
          opening to use brush patterns, tiles, colors, etc. set up by the caller 
          within the GC for the virtual screen. Fill pixel color MUST be different than 
          pixel found at initial seed point or function immediately returns with no 
          fill ditherlist (color at seed point establishes a temporary background
          to detect fillable area -- any other color is "wall").         
   */

   toss = borderfill_Xwin(xw,x0,y0,fillpixel); 




#ifdef DEBUG_XWINLITE
   if (NULL != outdebug)
     {
      fprintf(outdebug,"Ditherlist:\n"); 
      fflush(outdebug);
     }
#endif


   /* draw each line segment accumulated in ditherlist to virtual screen 
      using any fill style (tile, stipple, solid) set up previously by caller  */
   XSetForeground(xw->display, xw->virtualgc, fillpixel);
   for (i=0; get_dithlist(i,&xl,&xr,&y); i++)
     {

      #ifdef DEBUG_XWINLITE
         if (NULL != outdebug)
           {
            fprintf(outdebug,"   xl=%d    xr=%d    y=%d\n",xl,xr,y); 
            fflush(outdebug);
           }
      #endif

      /* draw a line from (xl,y) to (xr,y) in current style, fill, tile, or brush using 
         fillpixel color */
      XDrawLine(xw->display, xw->vpixmap[xw->draw_page], xw->virtualgc, xl,y, xr,y);
     }
   XSetForeground(xw->display, xw->virtualgc, xw->foregroundpixel);

   /* at this point ditherlist is established, but no links for y lookup have
      been created */

   XFlush(xw->display);
   xw->force_redraw = TRUE;
  }

/* ---------------------------------------------------------------------------- */

/* Font sizing support assumes that the specific 17 fonts listed above in XFonts[] are in 
   positions 0-16 in font list in the order shown.  Other fonts may be loaded after that,
   but sizing support will ignore them. */

int convert_fontnumber_Xwin(Xwin *xw, int fontnumber, int size)
  {
   int implied_size = 1;
   int fonttype = 0;
 
   /* fonts numbered 0-16 are reserved to those in XFonts list above -- 4 groups of 5 fonts */
   
   /* user selects fonts 0-4 (matches BGI fonts) -- the source font size for possible scaling 
      operations is computed and a (possibly) new font number is returned which reflects 
      size 1-4 -- if user scaling is needed (size 0) then size 4 is used as a basis for copy 
      except in case of default font, which uses size 1 for scaling */

   switch (fontnumber)
     {
      case 0:        
      case 1:        
      case 2:        
      case 3:        
      case 4:        
        {
         fonttype = fontnumber;
         implied_size = 1;
         break;  
        }
      case 5:        
      case 6:        
      case 7:        
      case 8:        
        {
         fonttype = fontnumber - 4;
         implied_size = 2;
         break;  
        }
      case 9:        
      case 10:        
      case 11:        
      case 12:        
        {
         fonttype = fontnumber - 8;
         implied_size = 3;
         break;  
        }
      case 13:        
      case 14:        
      case 15:        
      case 16:        
        {
         fonttype = fontnumber - 12;
         implied_size = 4;
         break;  
        }
      default: 
        {
         /* font number other than built-in font system -- pass it back unchanged */  
         return fontnumber;   
        }
     }

   /* if size is 0-10, that specification overrides the implied size from fontnumber */
   if (fonttype == 0)    /* default font only has one point size in table, at size 1 */
     {
      return 0;   
     }
   switch (size)
     {
      case 0:          /* user scaled fonts -- use size 4 fonts as data source */ 
        {
         implied_size = 4;
         break;  
        }
      case 1:          /* font size 1-4 use directly loaded fonts */ 
      case 2:
      case 3:
      case 4:
        {
         implied_size = size;
         break;  
        }
      case 5:          /* legal integer font sizes 5-10 scale from size 4 fonts */ 
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
        {
         if (fonttype == 0)
           {
            implied_size = 1;
           }
         else
           {  
            implied_size = 4;
           }
         break;  
        }
      default: /* size not specified -- determined by font number -- therefore pass back 
                  font number unchanged */ 
        {
         return fontnumber;   
        }
     }

   /* implied_size is now 1-4, and fonttype is now 0-4 */
   return ((implied_size - 1) * 4) + fonttype;
  }


void linefeedxy_Xwin(Xwin *xw, int dir, int size, int *x, int *y)
  {
   /* calculate scaled font height and advance (x, y) one line given the current font, 
      size, and direction */

   int xwidth, yheight;

   scalexy_currentfont_Xwin(xw, size, 100, &xwidth, &yheight);
   switch (dir)
     {
      case 1:     /* vertical bottom-to-top */
        { 
         *x = *x + yheight; 
         break;
        }
      case 2:     /* horizontal right-to-left upside down */
        {
         *y = *y - yheight; 
         break;
        }
      case 3:     /* vertical top_to_bottom */
        { 
         *x = *x - yheight; 
         break;
        } 
      default:    /* assume left_to_right */
        {
         *y = *y + yheight; 
        }
     }
  }


void scalexy_currentfont_Xwin(Xwin *xw, int size, int fontwidth, int *xwidth, int *yheight)
  {
   /* return height in pixels of new font AS SCALED */
   int font_height;
   int actual_font_used;
   double scale_x, scale_y; 
   double xwid, yhgt;

   *xwidth = 0; 
   *yheight = 0;

   if (xw == NULL)
     {
      return;   /* Xwin invalid -- no font or height */
     }
   
   if (xw->nfontsloaded)  /* must have at least one font loaded or it can't be set */
     { 
      /* find the pixel height of the font */
      actual_font_used = convert_fontnumber_Xwin(xw,xw->currentfontindex,xw->font_size);



      font_height = (xw->fontinfo[actual_font_used])->ascent 
                                    + (xw->fontinfo[actual_font_used])->descent;

      scale_x = 1.0;
      scale_y = 1.0;

      switch (size)
        {
         case 0:          /* user scaled fonts -- use size 4 fonts as data source
                             except for default font, which is size 1 */ 
           {
            scale_x = xw->fontscale_x;
            scale_y = xw->fontscale_y;

            if (scale_x < 0.10)
              {
               scale_x = 0.10;
              }
            if (scale_x > 50.0)
              {
               scale_x = 50.0;
              }
            if (scale_y < 0.10)
              {
               scale_y = 0.10;
              }
            if (scale_y > 50.0)
              {
               scale_y = 50.0;
              }

            if (actual_font_used != 0)
              {  
               scale_x = scale_x / 4.0;  /* scale data in window struct is normalized 
                                             as if size 1 is being scaled (but size 4 is) */
               scale_y = scale_y / 4.0;
              } 
            break;  
           }
         case 1:          /* font size 1-4 use directly loaded fonts */ 
           {
            scale_x = 1.0;
            scale_y = 1.0;
            break;
           }
         case 2:
         case 3:
         case 4:
           {
            if (actual_font_used != 0)
              {  
               scale_x = 1.0;
               scale_y = 1.0;
              }
            else
              {
               scale_x = (double) size * 0.70;
               scale_y = (double) size * 0.70;
              } 
            break;  
           }
         case 5:   /* legal integer font sizes 5-10 scale from size 4 fonts... */ 
         case 6:
         case 7:
         case 8:
         case 9:
         case 10:
           {
            if (actual_font_used != 0)
              {  
               scale_x = (double)size / 4.0;
               scale_y = (double)size / 4.0;
              }
            else    /* ... except default font scales from size 1 -- factors equalize with size 4 */
              {
               scale_x = (double) size * 0.70;
               scale_y = (double) size * 0.70;
              } 
            break;  
           }
         default: /* size not specified -- determined by font number -- assume drawn at 1:1 in buffer */ 
           {
            scale_x = 1.0;
            scale_y = 1.0;
           }
        }

      xwid = 0.5 + (scale_x * fontwidth);
      yhgt = 0.5 + (scale_y * font_height);

      *xwidth   = xwid;
      *yheight  = yhgt;
     }

   return;    /* no valid font installed */ 
  }




/* a replacement for XTextWidth() which takes scaling into account for CURRENT font */
int scaled_textwidth_Xwin(Xwin *xw, const char *string, int length)
  {
   int origwidth;
   int actual_font_used;
   int scaled_width, scaled_height;
    

   if (xw == NULL)
     {
      return 0;   /* Xwin invalid -- no font or width */
     }
   
   actual_font_used = convert_fontnumber_Xwin(xw,xw->currentfontindex,xw->font_size);
   origwidth = XTextWidth(xw->fontinfo[actual_font_used], string, length);
   scalexy_currentfont_Xwin(xw,xw->font_size,origwidth,&scaled_width,&scaled_height);

   return scaled_width;
  }


/* the following fuunction calculates the X normalized location of a rectangle which fully encompasses
   text which will be copied to screen -- font, direction, & scaling are taken into account -- this is 
   necessary for background clearing operation */

void textadjust_rect_Xwin(Xwin *xw, int x, int y, int width, 
                                      int *dest_x, int *dest_y, int *dest_width, int *dest_height)
  {
   int xwidth;
   int yheight;

 
   if (xw == NULL)
     {
      return;
     }   

   /* given current font and size specs, find size the FULL BUFFER WIDTH x font_height
      would be if drawn to the destination (maximum source rectangle)
      -- that result would be:  xwidth  x  yheight 
   */

   scalexy_currentfont_Xwin(xw,xw->font_size,xw->width,&xwidth,&yheight);

   /* xwidth should now be maximum width on screen of text which may be drawn from buffer 
      and represents the upper limit of the length of output for directions 0,1,2, or 4 
      -- if direction == -1, no scaling will occur */
   
   if (width < 0)           /* if no width specified ... */
     {
      width = xwidth;       /* this is intended to be open-ended -- go for the max */
     }
   else
     {
      /* width is externally defined */
     }

   switch (xw->font_direction)
     {
      case 1:     /* vertical bottom-to-top */
        { 
         /* normalize rectangle position such that specified x,y is at left top beginning
            of text when turned to normal reading orientation */
         *dest_y = y - width; 
         *dest_x = x;
         *dest_height = width; 
         *dest_width = yheight;
         break;
        }
      case 2:     /* horizontal right-to-left upside down */
        {
         /* normalize rectangle position such that specified x,y is at left top beginning
            of text when turned to normal reading orientation */
         *dest_y = y - yheight; 
         *dest_x = x - width;
         *dest_height = yheight; 
         *dest_width = width;
         break;
        }
      case 3:     /* vertical top_to_bottom */
        { 
         /* normalize rectangle position such that specified x,y is at left top beginning
            of text when turned to normal reading orientation */
         *dest_y = y; 
         *dest_x = x - yheight;
         *dest_height = width;
         *dest_width = yheight;
         break;
        } 
      default:    /* (0) assume left_to_right */
        {
         *dest_y = y; 
         *dest_x = x;
         *dest_height = yheight; 
         *dest_width = width;
        }
     }
  }

 
/* for all styles settable by this function, a value < 0 retains prior state for that style
   -- for all variables, a value higher than the legal maximum will set that variable to default 
   -- definitions XWIN_DEFAULT and XWIN_RETAIN are provided to always work for any of these styles */

void style_set_Xwin(Xwin *xw, int dir, int fontnumber, int size, 
                                      int colornum, int bgcolornum, int brushnum) 
  { 
   int justify = 0;
   int i;
   
   if (xw == NULL)
     {
      return;
     }

   if (fontnumber >= xw->nfontsloaded)
     {
      /* high value sets size to default font */
      xw->currentfontindex = 0;
     }
   else
     {  
      if (fontnumber >= 0)   /* if valid font number, change */       
        {
         xw->currentfontindex = fontnumber;
        }
     }  

   if (dir > 4)
     {
      /* high value sets size to default */
      xw->font_direction = -1;
     }
   else
     {  
      if (dir >= 0)   /* if valid direction value, change */       
        {
         xw->font_direction = dir;
        }
     }  


   if (size > 10)
     {
      /* high value sets size to default */
      xw->font_size = -1;
     }
   else
     {  
      if (size >= 0)   /* if valid size value, change */       
        {
         xw->font_size = size;
        }
     }  


   if (colornum > MAXCOLORS)
     {
      /* set to default */
      xw->currentcolorpixel = xw->foregroundpixel;
      XSetForeground(xw->display, xw->virtualgc, xw->currentcolorpixel);
     }
   else
     { 
      if (colornum >= 0)  /* a legal color index -- change, else existing settings */
        {
         xw->currentcolorpixel = xw->palette[colornum].pixel;
         XSetForeground(xw->display, xw->virtualgc, xw->currentcolorpixel);
        }
     }


   if (bgcolornum > MAXCOLORS)
     {
      /* set to default */
      xw->currentbgcolorpixel = xw->backgroundpixel;
      XSetBackground(xw->display, xw->virtualgc, xw->currentbgcolorpixel);
     }
   else
     { 
      if (bgcolornum >= 0)  /* a legal color index -- change, else existing settings */
        {
         xw->currentbgcolorpixel = xw->palette[bgcolornum].pixel;
         XSetBackground(xw->display, xw->virtualgc, xw->currentbgcolorpixel);
        }
     }


   if (brushnum >= xw->nbrushesloaded)
     {
      /* high value sets GC brush to solid fill */
      xw->currentbrush = -1;
      XSetFillStyle(xw->display, xw->virtualgc, FillSolid);
     }
   else
     {  
      if (brushnum >= 0)   /* if valid brush number, change, else retain prior state  */
        {
         xw->currentbrush = brushnum;
         XSetFillStyle(xw->display, xw->virtualgc, FillStippled); 
         XSetStipple(xw->display,xw->virtualgc,xw->brush[brushnum]); 
        }
     }  

  }


void fontscale_size0_Xwin(Xwin *xw, double xscale, double yscale)
  {
   xw->fontscale_x = 4.5; 
   xw->fontscale_y = 5.5; 
  }



int text_nextbreak_Xwin(Xwin *xw, int *pos, 
                                         int x, int y, int textwidth, 
                                                         const char string[])
  {
   /* find the position at which it is best to break a left-to-right string of text
      to fit within a specified window when wrapping to a next line -- if a blank or 
      linefeed is found within a few chars of the position which would last fit
      into the line, it is used to cut the line and set up to wrap the remainder 
      of the string to the next line -- string should not have any tabs or other 
      special chars in it other than linefeeds and blanks -- uses current font index */
   
   int i, j;
   int widthblank;
   int target;
   int widthsofar;
   int lastblank;
   int tpos, bpos;

   if (xw == NULL)
     {
      return;
     }   

   widthblank = scaled_textwidth_Xwin(xw, " ", 1);
   widthsofar = 0;
   target = textwidth - widthblank;
   lastblank = -1; 

   if (*pos < 0)   /* negative position is considered finished -- no more to scan */
     {
      return 0;  /* 0 count is an empty string -- could be interpreted as linefeed */
     }

   bpos = *pos;

   /* the characters next expected to be output are from [pos] to [returned_value] */
   for (i=*pos; string[i]; i++)
     {
      if (string[i] == '\n')   /* unconditional line break */ 
        {
         /* skip the linefeed itself */
         *pos = i+1;  
         return (i - bpos);
        }
      if (string[i] == ' ')
        {
         lastblank = i;
        }
      widthsofar += scaled_textwidth_Xwin(xw, string+i, 1); 
      if (widthsofar >= target)
        {
         if (lastblank >= 0)
           {
            /* cut off at last blank -- skip that blank itself and following blanks 
                                              until nonblank found */
            for (tpos = lastblank+1; string[tpos]; tpos++)
              {
               if (string[tpos] != ' ')
                 {
                  for (j=lastblank; j>=*pos; j--)
                    {
                     if (string[j] != ' ')
                       {
                        *pos = tpos;
                        return ((j+1) - bpos);
                       } 
                    } 
                  /* if you're here, everything in line was blank up to last blank cutoff */ 
                  *pos = tpos;
                  return 0;
                 }  
              } 
            
            /* if you get here, you're at end of string */
            for (j=lastblank; j>=*pos; j--)
              {
               if (string[j] != ' ')
                 {
                  *pos = -1; 
                  return ((j+1) - bpos);
                 } 
              } 
            /* if you're here, everything in line was blank up to last blank cutoff */ 
            *pos = -1; 
            return 0;
           }  
         else
           {
            /* already hit target and there are NO blanks in this line so far 
               -- break it here anyway */
            *pos = i;
            return (i - bpos); 
           }
        }
     }

   /* if you get here, you're at end of string and shorter than target */
   *pos = -1; 
   return (i - bpos);
  } 


/* replacement -- scan destination, center loop maps to source -- allows font sizing */
static void copy_font_buffer(Xwin *xw, int dest_x, int dest_y, int dest_width)
  {
   int src_x, src_y, src_width, src_height;
   int i,j;
   int scaling;
   unsigned long plane_mask = 0x00FFFFFF;
   int actual_font_used;
   int source_font_height;
   double inc_x, inc_y;
   double pos_x, pos_y;
   int xwidth, yheight;
   int dest_height;

   /* Ximage used to get around lack of direct "get pixel" from Pixmap X function --
      must copy font drawing buffer from pixmap to Ximage then use XGetPixel() */
   XImage *font_ximage;

 
   if (xw == NULL)
     {
      return;
     }   

   XSetForeground(xw->display, xw->virtualgc, xw->currentcolorpixel);

   /* source in ximage coordinates (640 x 36) -- dest in virtual window coordinates (640 x 480) */ 

   /* font has already been selected and was used to draw the text in the buffer -- need to determine
      the size of the source rectangle which encompasses all the text drawn given the actual 
      font size used */

   actual_font_used = convert_fontnumber_Xwin(xw,xw->currentfontindex,xw->font_size);
   source_font_height = (xw->fontinfo[actual_font_used])->ascent 
                                    + (xw->fontinfo[actual_font_used])->descent; 
   src_height = source_font_height;

   switch (xw->font_size)
     {
      case 0:          /* user scaled fonts -- use size 4 fonts as data source */ 
        {
         scaling = TRUE; 
         break;  
        }
      case 1:          /* font size 1-4 use directly loaded fonts except default font */
        {
         scaling = FALSE; 
         break;
        }
      case 2:
      case 3:
      case 4:
        {
         scaling = FALSE; 
         if (actual_font_used == 0)
           {
            scaling = TRUE; 
           }
         break;  
        }
      case 5:          /* legal integer font sizes 5-10 scale from size 4 fonts */ 
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
        {
         scaling = TRUE; 
         break;  
        }
      default: /* size not specified -- determined by font number -- assume drawn at 1:1 in buffer */ 
        {
         scaling = FALSE; 
        }
     }

   /* given current font and size specs, find size the FULL BUFFER WIDTH x font_height
      would be if drawn to the destination (maximum source rectangle)
      -- that result would be:  xwidth  x  yheight 
   */

   scalexy_currentfont_Xwin(xw,xw->font_size,xw->width,&xwidth,&yheight);
   dest_height = yheight;


   /* get increments to advance getpixel lookup if scaling is in effect */
   inc_y = 0.0;
   if (yheight != 0)
     {
      inc_y = (double) source_font_height / (double) yheight;
     }

   inc_x = 0.0;
   if (xwidth != 0)
     {
      inc_x = (double) xw->width / (double) xwidth;
     } 
   
   
   /* determine scale factors for copying pixels -- destination will be scanned pixel by pixel 
      for potential outputs -- source rectangle may be smaller or larger than destination */ 

   /* need to allocate a temporary Ximage and copy pixels from Drawable buffer to Ximage
      because X doesn't support reading individual pixels from a Pixmap */

   font_ximage =  XGetImage(xw->display, xw->vpixmap[xw->draw_page], 0, xw->height, xw->width, 36, plane_mask, ZPixmap);
   if (font_ximage)
     {
      /* NOTE: when using scaling, offsetting to center of first pixel ensures 
               pixels are picked up in a stable manner for normal scale range */ 

      switch (xw->font_direction)
        {
         case 1:     /* vertical bottom-to-top */
           { 
            /* normalize rectangle position such that specified x,y is at left top beginning
               of text when turned to normal reading orientation */
            dest_y -= dest_width; 
            src_y = 0; 
            pos_y = 0.5;
            for (j=0; j<dest_height; j++)
              {
               src_x = 0; 
               pos_x = 0.5;   
               for (i=(dest_width-1); i>=0; i--)
                 {
                  if ((src_x < xw->width) && (src_y < 36))  /* ensure getpixel stays in buffer */
                    {
                     if (xw->currentbgcolorpixel != XGetPixel(font_ximage,src_x,src_y))  
                       {
                        XDrawPoint(xw->display,xw->vpixmap[xw->draw_page],
                                        xw->virtualgc,(dest_x)+j,(dest_y)+i); 
                       } 
                    }
                  if (scaling)
                    {
                     pos_x += inc_x; 
                     src_x = pos_x;
                    }
                  else
                    {
                     src_x += 1;  
                    }
                 }

               if (scaling)
                 {
                  pos_y += inc_y; 
                  src_y = pos_y;
                 }
               else
                 {
                  src_y += 1;  
                 }
              }           
            break;
           }
         case 2:     /* horizontal right-to-left upside down */
           {
            /* normalize rectangle position such that specified x,y is at left top beginning
               of text when turned to normal reading orientation */
            dest_x -= dest_width; 
            dest_y -= dest_height; 
            src_y = 0; 
            pos_y = 0.5;
            for (j=(dest_height-1); j>=0; j--)
              {
               src_x = 0; 
               pos_x = 0.5;   
               for (i=(dest_width-1); i>=0; i--)
                 {
                  if ((src_x < xw->width) && (src_y < 36))  /* ensure getpixel stays in buffer */
                    {
                     if (xw->currentbgcolorpixel != XGetPixel(font_ximage,src_x,src_y))  
                       {
                        XDrawPoint(xw->display,xw->vpixmap[xw->draw_page],
                                        xw->virtualgc,(dest_x)+i,(dest_y)+j); 
                       } 
                    }
                  if (scaling)
                    {
                     pos_x += inc_x; 
                     src_x = pos_x;
                    }
                  else
                    {
                     src_x += 1;  
                    }
                 }
               if (scaling)
                 {
                  pos_y += inc_y; 
                  src_y = pos_y;
                 }
               else
                 {
                  src_y += 1;  
                 }
              }           
            break;
           }
         case 3:     /* vertical top_to_bottom */
           { 
            /* normalize rectangle position such that specified x,y is at left top beginning
               of text when turned to normal reading orientation */
            dest_x -= dest_height; 
            src_y = 0; 
            pos_y = 0.5;
            for (j=(dest_height-1); j>=0; j--)
              {
               src_x = 0; 
               pos_x = 0.5;   
               for (i=0; i<dest_width; i++)
                 {
                  if ((src_x < xw->width) && (src_y < 36))  /* ensure getpixel stays in buffer */
                    {
                     if (xw->currentbgcolorpixel != XGetPixel(font_ximage,src_x,src_y))  
                       {
                        XDrawPoint(xw->display,xw->vpixmap[xw->draw_page],
                                        xw->virtualgc,(dest_x)+j,(dest_y)+i); 
                       } 
                    }
                  if (scaling)
                    {
                     pos_x += inc_x; 
                     src_x = pos_x;
                    }
                  else
                    {
                     src_x += 1;  
                    }
                 }
               if (scaling)
                 {
                  pos_y += inc_y; 
                  src_y = pos_y;
                 }
               else
                 {
                  src_y += 1;  
                 }
              }           
            break;
           } 
         default:    /* (0) assume left_to_right */
           {
            src_y = 0; 
            pos_y = 0.5;
            for (j=0; j<dest_height; j++)
              {
               src_x = 0; 
               pos_x = 0.5;   
               for (i=0; i<dest_width; i++)
                 {
                  if ((src_x < xw->width) && (src_y < 36))  /* ensure getpixel stays in buffer */
                    {
                     if (xw->currentbgcolorpixel != XGetPixel(font_ximage,src_x,src_y))  
                       {
                        XDrawPoint(xw->display,xw->vpixmap[xw->draw_page],
                                        xw->virtualgc,(dest_x)+i,(dest_y)+j); 
                       } 
                    }
                  if (scaling)
                    {
                     pos_x += inc_x; 
                     src_x = pos_x;
                    }
                  else
                    {
                     src_x += 1;  
                    }
                 }
               if (scaling)
                 {
                  pos_y += inc_y; 
                  src_y = pos_y;
                 }
               else
                 {
                  src_y += 1;  
                 }
              }           
           }
        }

      /* kill off the temporary Ximage structure */   
      XDestroyImage(font_ximage);
     }

   XSetForeground(xw->display, xw->virtualgc, xw->foregroundpixel);  
   XFlush(xw->display);   /* flush to make sure ximage is done */
  }



/* NOTE: x, y, and textywidth are specified here in terms of window coordinates and thus
         represent limits on actual size.  THe width of characters in a specific font
         varies with scaling of that font, and scaling must be taken into account when 
         determining text breakpoints, etc.
*/

char holdem[500];

void text_charsout_Xwin(Xwin *xw, int justify, int colornumber, 
                                         int x, int y, int textwidth,  
                                         const char string[], int length)
  {
   int font_height;
   int font_yoffset;
   int actual_font_used; 
   int string_width;
   int bpos;
   int buf_x, buf_y, buf_textwidth, buf_textheight;
   int textheight; 
   int scaled_width, scaled_height;

   strncpy(holdem,string,length);
   holdem[length] = 0;

   if (xw == NULL)
     {
      return;
     }   

   if (!(xw->nfontsloaded))
     {
      return;
     }

   actual_font_used = convert_fontnumber_Xwin(xw,xw->currentfontindex,xw->font_size);
   font_height = (xw->fontinfo[actual_font_used])->ascent 
                                 + (xw->fontinfo[actual_font_used])->descent;

   font_yoffset = (xw->fontinfo[actual_font_used])->ascent;

   scalexy_currentfont_Xwin(xw,xw->font_size,textwidth,&scaled_width,&scaled_height);

   /* height at destination is scaled -- use SCALED font height */
   textheight = scaled_height;


   buf_x = x;
   buf_y = y;
   buf_textwidth = textwidth; 
   buf_textheight = font_height;  /* in buffer height is UNSCALED font height */

   /* the actual font number used is a function of the current selected font and 
      the font_size specification -- if font_size is < 0, the current selected font
      is retained as-is -- if size is 0-10, the actual font used may be different
      according to the expectations of the scaling alogorithm -- retain the window's
      current font specification data and change font (if needed) temporarily here */ 

   XSetFont(xw->display, xw->virtualgc, 
              (xw->fontinfo[actual_font_used])->fid);

   if (xw->font_direction >= 0)  /* nonnegative directions require indirect drawing to buffer */
     {
      /* DRAWABLE buffer is located off bottom of visible screen */
      buf_x = 0;
      buf_y = xw->height; 

      /* clear text buffer prior to use */
      clearrect_Xwin(xw, buf_x, buf_y, xw->width, 36);
     } 

   if (colornumber > MAXCOLORS)
     {
      xw->currentcolorpixel = xw->foregroundpixel;  
      XSetForeground(xw->display, xw->virtualgc, xw->currentcolorpixel);
     }
   else
     { 
      if (colornumber >= 0)
        {
         xw->currentcolorpixel = xw->palette[colornumber].pixel;  
         XSetForeground(xw->display, xw->virtualgc, xw->currentcolorpixel);
        }
     }
 
   if (justify == 0)  /* left justify */
     {
      XDrawString(xw->display, xw->vpixmap[xw->draw_page], xw->virtualgc, buf_x, buf_y+font_yoffset, 
                                                   string, length);
     }
   if (justify == 1)  /* center justify */
     {
      /* XTextWidth doesn't know about font scaling, so its output must be adjusted for scaling */
      string_width = scaled_textwidth_Xwin(xw, string, length);
      bpos = textwidth - string_width;
      bpos = bpos / 2;  
      XDrawString(xw->display, xw->vpixmap[xw->draw_page], xw->virtualgc, buf_x+bpos, buf_y+font_yoffset, 
                                                   string, length);
     }
   if (justify == 2)  /* right justify */
     {
      string_width = scaled_textwidth_Xwin(xw, string, length);
      bpos = textwidth - string_width;
      XDrawString(xw->display, xw->vpixmap[xw->draw_page], xw->virtualgc, buf_x+bpos, buf_y+font_yoffset, 
                                                   string, length);
     }

   if (xw->font_direction >= 0)  /* directions other than 0 require copying pixels from buffer
                                    to virtual screen destinations */
     {
      /* copy pixels in appropriate size and direction 
         -- copy from rectangle (textwidth x height)
         -- copy to same size rectangle, but scale within it as needed  */

      copy_font_buffer(xw,x,y,textwidth);      
     }
   else /* if default font drawing -- this is already done */
     {
     } 

   /* set the font back to original specification just in case */
   XSetFont(xw->display, xw->virtualgc, 
                                  (xw->fontinfo[xw->currentfontindex])->fid);
  }



void text_lineout_Xwin(Xwin *xw, int justify, int colornumber, 
                                         int x, int y, int textwidth, 
                                                         const char string[])
  {
   text_charsout_Xwin(xw,justify,colornumber,x,y,textwidth, string,strlen(string));
  }



/* outputs a text box with automatic word wrapping */
void text_boxout_Xwin(Xwin *xw, int justify, int colornumber, 
                                         int x, int y, int textwidth, int textheight, int applyclipping, 
                                                         const char string[])
  {
   int scaled_width, scaled_height;
   int bpos;
   int len;
   int curx, cury;
   int pos;
   int linelimit;
   int numlines;
   int clipx, clipy, clipwidth, clipheight;


   if (xw == NULL)
     {
      return;
     }   

   /* if clipping is applied, the edges of text may be drawn up to the edge of the defined box
      -- if clipping is NOT specified, the text will still be bound by the box, but will not attempt 
      to draw text where it is not known to fit */

   /* determine how many lines (rows) will completely fit inside box as defined */
   scalexy_currentfont_Xwin(xw,xw->font_size,textwidth,&scaled_width,&scaled_height);
   linelimit = textheight / scaled_height;

   /* assume direction 0 until otherwise established */
   clipx = x;
   clipy = y;
   clipwidth = textwidth;
   clipheight = textheight;

   if (xw->nfontsloaded)
     {
      if (applyclipping)
        {
         /* must normalize rectangle based on font direction */
         if (xw->font_size >= 0)  /* if font_size is not selected, ignore direction (same as 0) */
           {
            switch (xw->font_direction)
              {
               case 1:
                  clipx = x;
                  clipy = y-textwidth+1;
                  clipwidth = textheight;
                  clipheight = textwidth;
                  break;
               case 2:
                  clipx = x-textwidth+1;
                  clipy = y-textheight+1;
                  clipwidth = textwidth;
                  clipheight = textheight;
                  break;
               case 3:
                  clipx = x-textheight+1;
                  clipy = y;
                  clipwidth = textheight;
                  clipheight = textwidth;
                  break;
               default:   /* assume 0 */
                  clipx = x;
                  clipy = y;
                  clipwidth = textwidth;
                  clipheight = textheight;
              }
           }

         visible_inside_rect_Xwin(xw,clipx,clipy,clipwidth,clipheight);  /* clip to box */
        } 

      pos = 0;
      cury = y;
      curx = x;
      numlines = 0;
      while (pos >=0)
        {
         bpos = pos;
         /* NOTE: textwidth passed to nextbreak or charsout is same as original textwidth 
                  (not scaled width) since it defines placement on screen */    
         len = text_nextbreak_Xwin(xw,&pos,x,cury,textwidth,string);
         /* pos will be set to -1 when string is done */

         text_charsout_Xwin(xw,justify,colornumber,curx,cury,textwidth,string+bpos,len);

         numlines++;
         /* advance either x or y according to scaled height and font direction */
         linefeedxy_Xwin(xw, xw->font_direction, xw->font_size, &curx, &cury);

         if (applyclipping)
           {
            
            if (numlines > linelimit)  /* draw one more line than fits (chop off at clipped edge) */
               break;
           }
         else
           {
            if (numlines >= linelimit)  /* don't draw last line if it won't fit */
               break;
           }
        }
      if (applyclipping)
        {
         visible_all_Xwin(xw); /* turn off clipping */
        } 
     }
  }


void text_out_Xwin(Xwin *xw, int dir, int fontnumber, int size, int colornum, int drawbackground,  
                                     int x, int y, int limitwidth, const char *string, int length)
  { 
   int justify = 0;
   int i;
   int scaled_width, scaled_height;
   int calc_width;
   int dest_x, dest_y, dest_width, dest_height;
   
   if (xw == NULL)
     {
      return;
     }

   /* style_set_Xwin(xw,dir,fontnumber,size,colornum,-1,-1);  */
   style_set_Xwin(xw,dir,fontnumber,size,colornum,-1, 1);   /* force solid fill brush for text */  

   scalexy_currentfont_Xwin(xw,xw->font_size,100,&scaled_width,&scaled_height);

   /* the following assumes the scaling of fonts as preset or as implied by size */
   if (limitwidth < 0)
     {
      /* send out all chars regardless of the width it takes */
      calc_width = scaled_textwidth_Xwin(xw,string,length);  
      if (calc_width < limitwidth)
        {
         calc_width = limitwidth;
        }

      textadjust_rect_Xwin(xw, x, y, calc_width, &dest_x, &dest_y, &dest_width, &dest_height);

      if (drawbackground)
        {
         /* rectangle to be cleared must be calculated same way text output is determined
            -- direction influences where destination rectangle is placed on screen and size
            and scaling determines size of rectangle */
         clearrect_Xwin(xw,dest_x,dest_y,dest_width,dest_height); 
        }   
      text_charsout_Xwin(xw,0,colornum,x,y,calc_width,string,length); 
     }
   else
     {
      textadjust_rect_Xwin(xw, x, y, limitwidth, &dest_x, &dest_y, &dest_width, &dest_height);



      if (drawbackground)
        {
         clearrect_Xwin(xw,dest_x,dest_y,dest_width,dest_height); 
        }   
      text_charsout_Xwin(xw,0,colornum,x,y,limitwidth,string,length); 
     }

   XSetForeground(xw->display, xw->virtualgc, xw->foregroundpixel);
  }


void string_out_Xwin(Xwin *xw, int dir, int fontnumber, int size, int colornum, int drawbackground,  
                                     int x, int y, int limitwidth, const char *string)
  { 
   text_out_Xwin(xw,dir,fontnumber,size,colornum,drawbackground,x,y,limitwidth,string,strlen(string)); 
  }
 

void textout_Xwin(Xwin *xw, int drawbackground, int x, int y, int limitwidth, const char *string, int length)
  { 
   text_out_Xwin(xw,-1,-1,-1,-1,drawbackground,x,y,limitwidth,string,length);
  }


void stringout_Xwin(Xwin *xw, int drawbackground, int x, int y, int limitwidth, const char *string)
  { 
   textout_Xwin(xw,drawbackground,x,y,limitwidth,string,strlen(string)); 
  }


/* ----------------------------------------------------------------------------------------------- */


/* empty the keyboard buffer */
static void kb_clear_Xwin(Xwin *xw)
  {
   xw->kbtail = 0;
   xw->kbhead = 0;
  }


/* append a char to keyboard buffer -- may be called by any function to "stuff" key buffer
   special case allows any function to tell window to quit by prepending KB_WINDOW_QUIT 
   contains safety measures to prevent losing buffer when full -- lose most recent chars 
   instead.
*/


/* In a typical human queue, customers enter the tail of the queue and are served at the head.
   Likewise, chars go into the tail and out from the head -- special case is KB_WINDOW_QUIT,
   which goes directly in FRONT of the head to force attention on the next inspection of the
   keyboard buffer -- therefore the keyboard buffer is actually a dequeue (double-ended queue) */

static void kb_append_Xwin(Xwin *xw, int kar)
  {
   int karsave;
 
   if (kar != KB_WINDOW_QUIT)
     {
      karsave = xw->kbbuffer[xw->kbtail];
      xw->kbbuffer[xw->kbtail] = kar;  /* actually store the new char at the tail */

      if (xw->kbtail < KB_BUFSIZE)
        {
         xw->kbtail++;
        }  
      else
        {
         xw->kbtail = 0;
        }
      if (xw->kbtail == xw->kbhead)   
        {
         /* OOPS! just filled (emptied?) buffer and must lose this most recent char to 
                  to avoid losing everything else in the buffer */
         if (xw->kbtail > 0)  /* back off and lose the prior saved char */
           {
            xw->kbtail--;
           }  
         else
           {
            xw->kbtail = KB_BUFSIZE;
           }
         xw->kbbuffer[xw->kbtail] = karsave;
        }
     }
   else
     {
      /* if KB_WINDOW_QUIT, force signal to head to ensure 
         it will be handled immediately by event loop without killing other buffered chars */ 

      if (xw->kbhead > 0)
        {
         xw->kbhead--;
        }  
      else
        {
         xw->kbhead = KB_BUFSIZE;
        }

      xw->kbbuffer[xw->kbhead] = kar;  /* now actually store the QUIT signal at the head */

      if (xw->kbtail == xw->kbhead)
        {
         /* OOPS! just filled (emptied?) buffer and must lose most recently appended char 
                  (NOT the prepended QUIT signal) to avoid losing everything else in the buffer */
         if (xw->kbtail > 0)
           {
            xw->kbtail--;
           }  
         else
           {
            xw->kbtail = KB_BUFSIZE;
           }
        }
     }
  }

 

 
static int kb_read_Xwin(Xwin *xw, int remove)   
                                 /* if returns >= 0, is a normal key press --  if empty or 
                                    specified KB_WINDOW_QUIT, always returns < 0 
                                    remove != 0 causes buffer to shrink, otherwise look 
                                    w/o removing char from buffer */
  {
   int kar;

   if (xw->kbtail == xw->kbhead)
     {
      return KB_EMPTY; 
     }
   

   kar = xw->kbbuffer[xw->kbhead];

   if (remove)
     {
      if (xw->kbhead < KB_BUFSIZE)
        {
         xw->kbhead++;  
        }
      else
        {
         xw->kbhead = 0;
        }
     }

   return kar;
  }

 

/* -------------------------------------------------------------------------------- */




/* utility function to get position of mouse anywhere on main screen either in absolute
   coordinates (upper-left of screen = (0,0)) and current position of this window on
   the main screen, also in absolute coordinates -- coordinates of mouse must be positive 
   within (0,0) to (width-1,height-1) of display -- origin of screen may be negative 
   if window is pushed off left side of screen 
*/

void get_screen_mousepos_Xwin(Xwin *xw, int *mousex, int *mousey, int *origx, int *origy)
  {
   Window       rep_root, rep_child;
   int          absx, absy;
   int          relx, rely;
   unsigned int rep_mask;

   if (xw != NULL)
     {
      XQueryPointer (xw->display, xw->window, &rep_root, &rep_child,
 			&absx, &absy, &relx, &rely, &rep_mask);
      *mousex = absx;
      *mousey = absy;
      *origx  = absx - relx;
      *origy  = absy - rely;
     }
   else
     {
      *mousex = 0;
      *mousey = 0;
      *origx  = 0;
      *origy  = 0;
     }
  }
 


/* --------------------------------------------------------------------------------- */

void finish_Xwin(Xwin *xw)
  {
   int i;
   Display *dpy;

   if (xw != NULL)
     {
      if (xw->pixelcache != NULL)
        {
         XDestroyImage(xw->pixelcache);
        }

      if (xw->fullscreen_ximage != NULL)
        {
         XDestroyImage(xw->fullscreen_ximage);
        }
      xw->pixelcache = NULL;
      xw->fullscreen_ximage = NULL;


      for (i=(xw->nbrushesloaded)-1; i >= 0; i--)
        {
         XFreePixmap(xw->display, xw->brush[i]);
        } 

      
      for (i=(xw->numvpages)-1; i >= 0; i--)
        {
         XFreePixmap(xw->display, xw->vpixmap[i]);
        } 


      /* unload and free fonts in reverse order */
      for (i=(xw->nfontsloaded)-1; i >= 0; i--)
        {
         XFreeFont(xw->display,xw->fontinfo[i]);
        }

      /* free any color cells */ 

      /* disconnect the assocoation between colormap ID and the server */
      XFreeColormap(xw->display, xw->colormap);

      /* free the GCs */ 
      XFreeGC(xw->display, xw->virtualgc);
      XFreeGC(xw->display, xw->gc);
 
      XFlush(xw->display);

      XCloseDisplay(xw->display);
  
      /* finally, free the Xwin structure itself */ 
      free(xw);
     }
  }


Xwin *create_sub_Xwin(Xwin *parent, const char *windowname, int width, int height, int resizable, 
                                          int virtualbuffers, int whitebackground, Cursor specialcursor)
  {
   XSizeHints Hints;
   XWindowAttributes winattr;

   unsigned long valuemask = 0;		
   XGCValues values;	
   Xwin *xw;

   unsigned long p;
   unsigned long blkpixel[MAXCOLORS+1];

   int i;
   int j;
   int k;

   unsigned long q;
   int limitvpages;

   Display *dpy;
   Window parentwindow;

   struct timeval tv;

   Status rc;			/* return status of various Xlib functions.  */


#ifdef DEBUG_XWINLITE
   if (NULL == outdebug)   /* only open file if not already open */
     { 
      outdebug = fopen("out_debug_xwinlite.txt","w");  /* either NULL or file is open */
     }
#endif


   xw = (Xwin *)malloc(sizeof(Xwin));

   if (xw == NULL)
      return NULL;

   xw->parentxw = parent;    /* this is only needed for tree traversal */
   if (parent == NULL)
     {
      /* this subwindow is actually the main window */
      dpy =  XOpenDisplay(getenv("DISPLAY"));  
      parentwindow = RootWindow(dpy, DefaultScreen(dpy));
     }
   else
     {
      parentwindow = parent->window;
     }

   /* NOTE:  At this point xw->parentxw is either set to NULL if this is a main window or set
             to the parent Xwin if this is a subwindow -- xw->parentxw allows tree traversal
             upwards to the main window root -- parentwindow is set to the (Window type)
             window designation of the parent Xwin or the Root window if parent Xwin 
             was NULL.  This hides the root window itself from the tree traversal but still 
             uses it to create the main window.
   */

   /* determine number of virtual buffers to allocate */
   limitvpages = MAX_VPAGES;
   if (virtualbuffers < limitvpages)
     {
      limitvpages = virtualbuffers;
     } 
   if (limitvpages <= 0)
     {
      limitvpages = 1;     /* must have at least one virtual buffer */
     }

   xw->draw_page = 0;
   xw->video_page = 0;
   xw->numvpages = 0;
   xw->nbrushesloaded = 0;
   xw->currentbrush = -1;
   xw->force_redraw = FALSE;
   xw->use_fullcache = FALSE;
   xw->pixelcache = NULL;
   xw->fullscreen_ximage = NULL;
   xw->force_killcache = TRUE;   
   xw->pixelcache_x = -1;
   xw->pixelcache_y = -1;
   xw->putchange_x = -1;
   xw->putchange_y = -1;

   xw->currentx = 0;
   xw->currenty = 0;


   xw->font_direction       = -1; /* < 0 = direct, 0= Horizontal L-R, 1= V B-T, 2= H R-L 3= V T-B */

   xw->font_size            = -1;  /* < 0 = IGNORE (use font directly), 
                                        0 = use scale x 7 y factors, 
                                        1-4 = direct font size groups, 
                                        5-10 = scale upwards assuming size 4 is scaled 4.0 */    
   xw->fontscale_x      = 1.0;
   xw->fontscale_y      = 1.0;
   
   xw->width  = width;
   xw->height = height;
   xw->wposx  = 0;
   xw->wposy  = 0;

   xw->mouse_wposx = -1;    /* mouse is either on-window (positive x and y) or off (negative) */
   xw->mouse_wposy = -1;    /* since the offscreen position is not meaningful, define it as -1 */
   xw->mouse_button1 = 0;
   xw->mouse_button2 = 0;
   xw->mouse_click1 = 0;
   xw->mouse_click2 = 0;
   xw->mouse_dblclick1 = 0;
   xw->mouse_click1x = -1;
   xw->mouse_click1y = -1;
   xw->mouse_click2x = -1;
   xw->mouse_click2y = -1;
   xw->mouse_dblclick1x = -1;
   xw->mouse_dblclick1y = -1;
   xw->mouse_dbltimect = 0;     /* countdown to check for double-click on button 1 */

   xw->userdata = NULL;

   xw->kbtail = 0;
   xw->kbhead = 0;

   xw->ignore_modifiers = TRUE;

   for (i=0; i<MAXFONTS; i++)
     { 
      xw->fontinfo[i]   = NULL;
      xw->fontloaded[i] = FALSE;
     }
   xw->nfontsloaded = 0;
   xw->currentfontindex = 0;

   /* the following flags set up event loop to run when eventually called */ 
   xw->firstevent = TRUE;
   xw->stopevents = FALSE;
   xw->eventloopcount = 0;
   xw->loops_per_ms = 400;     /* initial KLUGE calibration of small-interval "timer" */

   gettimeofday(&tv,NULL);
   xw->fifth_secs = ((unsigned long)tv.tv_sec * 5) + (tv.tv_usec / 200000);     
   xw->calibrate_megaloops = xw->fifth_secs; 
 
   xw->display = XOpenDisplay(getenv("DISPLAY"));
   if (xw->display == NULL) 
     {
      free(xw);
      return NULL;
     }

   xw->screennum = DefaultScreen(xw->display);
   xw->visual = DefaultVisual(xw->display,xw->screennum);

#ifdef DEBUG_XWINLITE
       if (NULL != outdebug)
         {
          switch ((xw->visual)->class)
            {
             case PseudoColor:
                fprintf(outdebug,"PseudoColor\n");
                break;
             case DirectColor:
                fprintf(outdebug,"DirectColor\n");
                break;
             case TrueColor:
                fprintf(outdebug,"TrueColor\n");
                break;
             case StaticColor:
                fprintf(outdebug,"StaticColor\n");
                break;
            } 
          fflush(outdebug);
         }
#endif
       
   /* NOTE: GLF determined that this is TrueColor under Ubuntu */

   if (whitebackground)
     {
      xw->backgroundpixel =  WhitePixel(xw->display, xw->screennum);
      xw->foregroundpixel =  BlackPixel(xw->display, xw->screennum);
     }
   else
     {
      xw->backgroundpixel =  BlackPixel(xw->display, xw->screennum);
      xw->foregroundpixel =  WhitePixel(xw->display, xw->screennum);
     } 
   xw->window = XCreateSimpleWindow(xw->display, parentwindow,
                         0, 0, xw->width, xw->height, 1,
                         xw->foregroundpixel,
                         xw->backgroundpixel);

   xw->currentbgcolorpixel = xw->backgroundpixel;
   xw->currentcolorpixel = xw->foregroundpixel;

   xw->cursor = specialcursor;

   if (xw->cursor != None)
     {
      /* set up a special cursor (Font Cursor:  e.g. XC_crosshair or None) */
      XDefineCursor(xw->display, xw->window, XCreateFontCursor(xw->display, xw->cursor)); 
     }

   XGetWindowAttributes(xw->display, xw->window, &winattr);
   xw->depth = winattr.depth; 

   /* if depth required is higher than depth available, kill this window */
   if (8 > xw->depth)  /* the Xwin color system assumes AT LEAST 8-bit (256 color) display 
                           -- prefer 24 bit TRUECOLOR for photo quality images */
     {
      free(xw);
      return NULL; 
     } 

   /* From http://nobug.ifrance.com/nobug2/article1/babyloon/tut_xwin.htm:
    * a very dirty way to know the pixelsize. It's better to use XVisualInfo.
    * I'll search informations for this critical and important part.
    */
    
   switch(xw->depth)
     {
      case 8:
         xw->pixelsize=1;
         break;
      case 16:
         xw->pixelsize=2; 
	 break;
      case 24:
         xw->pixelsize=4;
         break;
      default:
         xw->pixelsize=1;
     }

   xw->allowresize = TRUE;
   if (!resizable)
     {
      /* This prevents a user from resizing the window, so bitmaps will be stable */
      xw->allowresize = FALSE;  
      Hints.flags = PSize | PMinSize | PMaxSize;
      Hints.min_width  = Hints.max_width  = Hints.base_width  = xw->width;
      Hints.min_height = Hints.max_height = Hints.base_height = xw->height;
      XSetWMNormalHints(xw->display,xw->window,&Hints);
     } 

   /* Give the window a name */
   XStoreName(xw->display,xw->window,windowname);

   /* make the window actually appear on the screen. */
   XMapWindow(xw->display, xw->window);

   /* flush all pending requests to the X server. */
   XFlush(xw->display);

   /* create GCs for actual and virtual windows */
   xw->gc    = XCreateGC(xw->display, xw->window, valuemask, &values);
   if (xw->gc >= 0) 
     {
      XSetForeground(xw->display, xw->gc, xw->foregroundpixel);
      XSetBackground(xw->display, xw->gc, xw->backgroundpixel);
      XSetLineAttributes(xw->display, xw->gc, 0, LineSolid, CapButt, JoinBevel);
      XSetFillStyle(xw->display, xw->gc, FillSolid);
     }
   else
     {
      /* gc did not succeed */ 
     } 

  /* flush all pending requests to the X server. */
   XFlush(xw->display);


   /* get access to the screen's color map. */
   xw->colormap = DefaultColormap(xw->display, xw->screennum);
 
   for (i=0; i<=MAXCOLORS; i++)
     {
      rc = XAllocNamedColor(xw->display, xw->colormap, XColors[i], &(xw->palette[i]), &(xw->palette[i]));
      if (rc == 0) 
        {
         /* failed */
        }
     }

   xw->currentcolorpixel = xw->palette[MAXCOLORS].pixel;
   XFlush(xw->display);

   for (i=0; i<limitvpages; i++)
     {
      /* allocate virtual visual pages as specified in "virtualbuffers" */

      /* create a virtual screen Pixmap to which all window drawing operations will be directed 
         -- the virtual screen will be recopied (quickly) to the actual window as needed */

      /* NOTE:  reserve an extra 36 pixel strip at BOTTOM of virtual screen as a text drawing
                buffer for directional font operations -- ordinarily this area will NOT be 
                displayed to actual window, but will be used as a source for redrawing text to 
                the visible working area of the virtual screen.  By using a piece of the virtual 
                screen itself, no additional GCs or colormaps need be created */

      xw->vpixmap[i] = XCreatePixmap(xw->display, xw->window, xw->width, xw->height+36, xw->depth);
      xw->numvpages++;
     }

   xw->virtualgc    = XCreateGC(xw->display, xw->vpixmap[xw->draw_page], valuemask, &values);
   if (xw->virtualgc >= 0) 
     {
      XSetForeground(xw->display, xw->virtualgc, xw->foregroundpixel);
      XSetBackground(xw->display, xw->virtualgc, xw->backgroundpixel);
      XSetLineAttributes(xw->display, xw->virtualgc, 0, LineSolid, CapButt, JoinBevel);
      XSetFillStyle(xw->display, xw->virtualgc, FillSolid);
     }

   /* set up pattern brushes which match winBGIm built-in patterns 0-11 */
   add_brush_Xwin(xw,EmptyBrushBitmap);
   add_brush_Xwin(xw,SolidBrushBitmap);
   add_brush_Xwin(xw,LineBrushBitmap);
   add_brush_Xwin(xw,LtslashBrushBitmap);
   add_brush_Xwin(xw,SlashBrushBitmap);
   add_brush_Xwin(xw,BkslashBrushBitmap);
   add_brush_Xwin(xw,LtbkslashBrushBitmap);
   add_brush_Xwin(xw,HatchBrushBitmap);
   add_brush_Xwin(xw,XhatchBrushBitmap);
   add_brush_Xwin(xw,InterleaveBrushBitmap);
   add_brush_Xwin(xw,WidedotBrushBitmap);
   add_brush_Xwin(xw,ClosedotBrushBitmap);

   XFlush(xw->display);

   return xw;
  }        



Xwin *create_main_Xwin(const char *windowname, int width, int height, int resizable, 
                                          int virtualbuffers, int whitebackground, Cursor specialcursor)
  {
   return create_sub_Xwin(NULL, windowname, width, height, resizable,  
                                            virtualbuffers, whitebackground, specialcursor);
  }



unsigned int modifier_mask_Xwin(Xwin *xw)  /* get a bit mask to identify SHIFT CTRL ALT state 
                                         0x0200 = SHIFT, 0x0400 = CTRL, 0x800 = ALT     */
  {
   unsigned int modmask = 0;
   char keys_return[32];
   int i;

   /* NOTE:  This is a KLUGE!  It is highly implementation-dependent because it uses keycodes 
             and keymaps directly */

   XQueryKeymap(xw->display, keys_return);

   if ((keys_return[6] & 0x04) || (keys_return[7] & 0x40))
     {
      modmask |= 0x0200;  /* SHIFT ON */
     }

   if ((keys_return[4] & 0x20) || (keys_return[13] & 0x02))
     {
      modmask |= 0x0400;  /* CTRL ON */
     }

   if ((keys_return[8] & 0x01) || (keys_return[13] & 0x10))
     {
      modmask |= 0x0800;  /* ALT ON */
     }

   return modmask;
  }
 
/* ------------------------------------------------------------------------- */

/* any_window_event_Xwin()
   ------------------- 
   This function allows a window to check the window-specific event queue
   without blocking -- returns False if no pertinent events are ready, otherwise
   returns TRUE with the event found.  ALL Client messages (global) are
   returned -- it is up to the caller to interpret Client messages
   and determine if they belong to the window -- if not they may be lost to
   the rightful client.  Unfortunately, This is the ONLY mechanism available 
   to detect a user's clicking the "X to Close" button on a window.
*/

Bool any_window_event_Xwin(Xwin *xw, long event_mask, XEvent *event)
  {
   /* before limiting to window events, check ALL events for any
      client messages */

   if (XCheckTypedEvent(xw->display, ClientMessage, event))
     {
      return True;
     }

   /* no client messages */
   return XCheckWindowEvent(xw->display, xw->window, event_mask, event);
  }



int default_singlepass_eventloop_Xwin(Xwin *xw)
  {
   XEvent an_event;
   int cont = TRUE;

   unsigned int modmask;
   int doit;

   long difftime;
   struct timeval tv;
   unsigned long newfifths;

   int i, n;
   KeySym key;		        /* a dealie-bob to handle KeyPress Events */	
   char text[256];		/* a char buffer for KeyPress Events */

   static holdoff = 0;
   static Atom wmDeleteMessage;    /* needed to process "click X to close window" events */
   static long event_mask = ExposureMask | KeyPressMask 
                                         | ButtonPressMask 
                                         | ButtonReleaseMask 
                                         | Button1MotionMask 
                                         | Button2MotionMask 
                                         | PointerMotionMask 
                                         | LeaveWindowMask
                                         | StructureNotifyMask;

   /* encapsulate the mask setup for event loop within the first pass of processing the loop
      itself, but exit immediately -- it won't have time to execute that fast anyway */

   if (xw == NULL)
     { 
      return FALSE;
     }

   /* increment loop counter to assist timeslicing events (alternate to RTC) */
   (xw->eventloopcount)++;

   if ((xw->eventloopcount % 9000) == 0) /* check clock about 50 times per second */
     {
      gettimeofday(&tv,NULL);
      newfifths = ((unsigned long)tv.tv_sec * 5) + (tv.tv_usec / 200000);     
      if (newfifths != xw->fifth_secs)  /* if 1/5 second count changed... */
        {
         /* ...set up a redraw every 1/5 sec -- automatically supports animation */
         xw->force_redraw = TRUE;
        } 
      xw->fifth_secs = newfifths;     
     }  

   if ((xw->eventloopcount % 2000000) == 0)  
     {
      /* this happens about once per 10 seconds (modern cpus) */

      /* recalibrate small-interval "timer" constant -- assume fifth-seconds count is maintained elsewhere */
      difftime =  xw->fifth_secs - xw->calibrate_megaloops;
      xw->calibrate_megaloops = xw->fifth_secs;
      if ((difftime <= 2) || (difftime > 1000))
        {
         /* avoid division by 0 and don't let value be unreasonable --keep original estimate */
        }

      /*  estimated loops per ms = ((2 million loops) / (200 msec in 1/5 sec)) / (number of 1/5 secs elapsed) */
      xw->loops_per_ms =  10000 / (difftime);
 
#ifdef DEBUG_XWINLITE
      if (NULL != outdebug)
        { 
         fprintf(outdebug,"Event loop count: %ld\n",xw->eventloopcount);
         fprintf(outdebug,"---------------\n");
         fflush(outdebug);
        }  
#endif
     }
 

   if (xw->firstevent)
     {
      /* register interest in the delete window message */
      wmDeleteMessage = XInternAtom(xw->display, "WM_DELETE_WINDOW", False);
      XSetWMProtocols(xw->display, xw->window, &wmDeleteMessage, 1);


      /* register interest in all other event types pertinent to this window */
      XSelectInput(xw->display, xw->window, event_mask);

      xw->firstevent = FALSE;
      xw->stopevents = FALSE;

      /* Need to "redraw" the window on startup -- assume screen was already built in virtual screen
         Pixmap (xw->vpixmap[xw->draw_page]) before event loop was called */

      /* copy the selected video refresh virtual pixmap to the actual window (redraw) */
      XCopyArea(xw->display, xw->vpixmap[xw->video_page], xw->window, xw->virtualgc, 
                                                          0, 0, xw->width, xw->height,  0, 0);

      XFlush(xw->display);
      return TRUE;
     }

   if (xw->stopevents)
     {
      return FALSE;
     }

   /* maintain mouse double-click timer count -- countdown to 0 and stop -- 
      (0 signals timeout beyond which a second click is considered independent of first one) */
   if (xw->mouse_dbltimect > 0)   
     {
      (xw->mouse_dbltimect)--;
     }

   /* if client specifies "redraw screen" message, handle it here */
   if (xw->force_redraw)
     {
      /* assume window is already built in a virtual pixmap */
      /* copy the virtual pixmap to the actual window (redraw) */
      XCopyArea(xw->display, xw->vpixmap[xw->video_page], xw->window, xw->virtualgc, 0, 0, 
                                                     xw->width, xw->height,  0, 0);
      XFlush(xw->display);
      xw->force_redraw = FALSE;
     }

   if (any_window_event_Xwin(xw,event_mask,&an_event))
     { 
      switch (an_event.type) 
        { 
         case ClientMessage:
            if (an_event.xclient.data.l[0] == wmDeleteMessage)
              {
               /* allows program to handle "click X" "close window" event gracefully */
#ifdef DEBUG_XWINLITE
               if (NULL != outdebug)
                 {
                  fprintf(outdebug,"User clicked X close...\n");
                  fflush(outdebug);
                 }
#endif
               cont = 0;

               /* notify event loop caller by inserting a special "key" to key buffer */
               kb_append_Xwin(xw,KB_WINDOW_QUIT);  

               /* NOTE: GLF verified that wmDeleteMessage remains valid across multiple 
                        instances of ClientMessage even if user quit request is ignored for several
                        tries.  It can apparently be safely assumed to be constant and valid 
                        for the life of the program run (until X server shuts down, in which case
                        so does program).                                                         */
              }
            break;  
         case Expose:
            /* if this is the first in a set of expose events - ignore this event. */
            if (an_event.xexpose.count == 0)
              {
               /* assume window is already built in a virtual pixmap */
               /* copy the virtual pixmap to the actual window (redraw) */
               XCopyArea(xw->display, xw->vpixmap[xw->video_page], xw->window, xw->virtualgc, 0, 0, 
                                                             xw->width, xw->height,  0, 0);
               XFlush(xw->display);
              }
            break;
  
         case ConfigureNotify:
            xw->wposx = an_event.xconfigure.x + an_event.xconfigure.border_width;
            xw->wposy = an_event.xconfigure.y + an_event.xconfigure.border_width;

            if ((xw->width != an_event.xconfigure.width) || (xw->height != an_event.xconfigure.height))
              {   
               xw->width = an_event.xconfigure.width;
               xw->height = an_event.xconfigure.height;

/*
#ifdef DEBUG_XWINLITE
               if (NULL != outdebug)
                 {
                  fprintf(outdebug,"New window size: width = %d, height = %d, pos = (%d,%d)\n",
                                          xw->width,xw->height,xw->wposx,xw->wposy);
                  fflush(outdebug);
                 }
#endif
*/
               xw->hasresized = TRUE;
              }
            break;
  
         case ButtonPress:
            switch (an_event.xbutton.button) 
              { 
               case Button1: 
                  xw->mouse_button1 = TRUE;
                  
                  /* click flags should be removed (= FALSE) by client (the event loop won't) 
                     -- new clicks override old ones -- if double-clicks are meaningful, don't 
                     interpret single clicks until timer count expires ( == 0) for 
                     *possible* double-click */
 
                  if (xw->mouse_click1)   /* if already clicked once... */
                    {
                     if (xw->mouse_dbltimect > 0)
                       {
                        xw->mouse_dblclick1 = TRUE;
                        xw->mouse_dblclick1x = xw->mouse_wposx;
                        xw->mouse_dblclick1y = xw->mouse_wposy;
                       } 
                     else
                       {
                        xw->mouse_dblclick1 = FALSE;
                       }
                     xw->mouse_dbltimect = 0;
                    }
                  else
                    {
                     xw->mouse_dblclick1 = FALSE;
                     xw->mouse_dbltimect = DBLCLICK_TIMER_FULLVALUE;
                    } 
                  xw->mouse_click1 = TRUE;
                  xw->mouse_click1x = xw->mouse_wposx;
                  xw->mouse_click1y = xw->mouse_wposy;

                  break;
               case Button3:                   /* NOTE: assumes 3 button mouse */
                  xw->mouse_button2 = TRUE;
                  xw->mouse_click2 = TRUE;
                  xw->mouse_click2x = xw->mouse_wposx;
                  xw->mouse_click2y = xw->mouse_wposy;
                  break;
              }
            break;
  
         case ButtonRelease:
            switch (an_event.xbutton.button) 
              { 
               case Button1: 
                  xw->mouse_button1 = FALSE;
                  break;
               case Button3: 
                  xw->mouse_button2 = FALSE;
                  break;
              }
            break;
  
         case LeaveNotify:
            xw->mouse_wposx = -1; 
            xw->mouse_wposy = -1;
            break;
  
         case MotionNotify:
            if (an_event.xmotion.window == xw->window)  /* if event is in this window, update position */
              {
               xw->mouse_wposx = an_event.xmotion.x; 
               xw->mouse_wposy = an_event.xmotion.y;
              }
            else
              {
               xw->mouse_wposx = -1; 
               xw->mouse_wposy = -1;
              }
            break;
  
         case KeyPress:
            /* handle key presses -- put keys into circular buffer -- X makes this weirder than usual */
 
            /* go ahead and get all keys -- might be a macro key string */
            n = XLookupString(&an_event.xkey,text,255,&key,0);
            for (i=0; i<n; i++)   
              {
               if (text[i])  
                 {
                  kb_append_Xwin(xw,text[i]);
                 } 
               else
                 {
                 } 
              }
            if (n == 0)   /* key event occurred but no key translation was produced */
              {
               modmask = modifier_mask_Xwin(xw);
               doit = TRUE;
               if (xw->ignore_modifiers)
                 {
                  switch (an_event.xkey.keycode)
                    {
                     case 0x25:   /* L CTRL */
                     case 0x32:   /* L SHIFT */
                     case 0x42:   /* CAPS LOCK */
                     case 0x69:   /* R CTRL */
                     case 0x3E:   /* R SHIFT */
                     case 0x4D:   /* NUM LOCK */
                     case 0x40:   /* L ALT */
                     case 0x6C:   /* R ALT */
                       { 
                        doit = FALSE;
                        break;
                       }
                     default:
                       {
                       } 
                    }
                 }
               if (doit)
                 {
                  /* special key? -- append to buffer a null char followed by keycode */
                  kb_append_Xwin(xw,0);
                  kb_append_Xwin(xw,(an_event.xkey.keycode | modmask));
                 }  
              } 

            /* ESC char quits -- ?? */
            /*  
            if ((n > 0) && (text[0] == 0x1B))  
              {
               xw->stopevents = TRUE;  
               cont = 0;
              } 
            */   

            break;

         default: 
            break;
        }
     }

   /* this might be a good place to insert calls to similar single
      passes of subwindow event loops -- that way they all get called
      within the outer "while loop" without any extra attention --
      in those loop passes, check events for those displays */ 

   return cont;  
  }


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



int kbhit_Xwin(Xwin *xw, int runevents)
  {
   int toss;
   int i;

   toss = default_singlepass_eventloop_Xwin(xw);
     
   if (kb_read_Xwin(xw,0) != KB_EMPTY) /* use only buffered keys here... */ 
     {
      return TRUE; /* indicate a key is waiting -- includes the KB_WINDOW_QUIT 
                      signal if present */
     } 
   else 
     { 
      /* normally this would tell caller "no key is in buffer" -- however, if events are stopped, 
         no more keys can appear -- to avoid infinite loop, detect this case and pretend a user quit
         message is in buffer -- getch_Xwin() below cooperates in this behavior */
      if (xw->stopevents)
        {
         return TRUE;
        }
      return FALSE;
     } 
  }


int getch_Xwin(Xwin *xw)   /*  wait until key is ready then get next key in buffer */
  {
   int kar;
   int toss;
   int i;

   /* if events have been stopped, getch() must react as if a KB_WINDOW_QUIT request has occurred
      (even though there is not actually a key in the buffer) -- if events are already stopped, 
      no more keys can appear -- to avoid infinite loop, detect this case and pretend a user quit
      message is in buffer -- kbhit_Xwin() above cooperates in this behavior -- this will be seen
      by caller as an endlessly repeating sequence of KB_WINDOW_QUIT "keys" until caller reacts 
      by shutting down or re-enabling event processing */

   if (xw->stopevents)
     {
      return KB_WINDOW_QUIT;
     }

   while (!kbhit_Xwin(xw,1))
     {
      /* process window event loop while waiting for keys -- as soon as key is available,
         getch_Xwin() will be ready to report it */
   /*
      for (i=0; i<29; i++)
        {
         toss = default_singlepass_eventloop_Xwin(xw);
        }
*/
      toss = default_singlepass_eventloop_Xwin(xw);

     }  

   kar = kb_read_Xwin(xw,1);
   if (kar > 0)     /* all normal chars (ASCII?) are <= 0xFF */
     {
      return kar;
     }
   if (kar == 0)    /* special keys returned as 2-byte sequences:  0x00, keycode */
     {
      kar = kb_read_Xwin(xw,1);
      if (kar > 0)
        {
         /* might as well make it compatible with winBGI ??? (would need translation table)  */
         return (0x0100 | kar);  /* return a keycode instead of ASCII char -- 
                                    keycode always <= 0xFF, set bit 8 to indicate keycode, 
                                    bit 9 set if SHIFT, bit 10 if CTRL, bit 11 if ALT  */
        }
     }
   /* at this point either a bad special key (null without a followup) or a negative value
      (special signal -- empty buffer or request to quit) was encountered  */
   if (kar == KB_WINDOW_QUIT)
     {
      /* should post a QUIT message to event loop -- return this as a special key to caller */
      return kar;
     }
   /* otherwise treat as a single NULL char */
   return 0;
  }

/* TODO:  make a line-oriented input function and/or screen-oriented edit functions */


void delay_Xwin(Xwin *xw, int msec)
  {
   int i;
   int toss; 
   
   /* don't sleep too long without checking event loop */
   for (i=0; i<msec; i++)
     {
      do
        {
         toss = default_singlepass_eventloop_Xwin(xw);
        }   
      while (xw->eventloopcount % xw->loops_per_ms); 
     }
  }

/* -------------------------------------------------------------------------------- */

int open_xpm_Xwin(Xwin *xw, Xwin_xpmimage *xpmstore, const char filename[])
  {
   XpmReadFileToImage(xw->display, filename, &(xpmstore->ximage), &(xpmstore->shapeximage), NULL);
   if (xpmstore->ximage != NULL)
      return TRUE;

   return FALSE;
  }


void close_xpm_Xwin(Xwin *xw, Xwin_xpmimage *xpmstore)
  {
   if (xpmstore->ximage != NULL)
     {
      XDestroyImage(xpmstore->ximage);
     }
   if (xpmstore->shapeximage != NULL)
     {
      XDestroyImage(xpmstore->shapeximage);
     }
   xpmstore->ximage = NULL; 
   xpmstore->shapeximage = NULL; 
  } 


void copy_xpm_to_Xwin(Xwin *xw, Xwin_xpmimage *xpmstore, 
                         int src_x, int src_y, int src_width, int src_height, int dest_x, int dest_y)
  {
   if (xpmstore->ximage == NULL)
      return;

   XPutImage(xw->display, xw->vpixmap[xw->draw_page], xw->virtualgc, xpmstore->ximage, src_x, 0, 0, 0,  src_width, src_height); 
   XFlush(xw->display);
  }




void repaint_Xwin(Xwin *xw)
  {
   if (xw == NULL)
      return;
   
   xw->force_redraw = TRUE;    
  }


void setvisual_Xwin(Xwin *xw, int pageno)
  {
   if (xw == NULL)
      return;

   if ((pageno < 0) || (pageno >= xw->numvpages))
      return;

   xw->video_page = pageno;
  }


void setactive_Xwin(Xwin *xw, int pageno)
  {
   if (xw == NULL)
      return;

   if ((pageno < 0) || (pageno >= xw->numvpages))
      return;

   xw->draw_page = pageno;
  }


void flushbuffers_Xwin(Xwin *xw)
  {
   XFlush(xw->display);
  }




/* -------------------------------------------------------------------------------------- */




