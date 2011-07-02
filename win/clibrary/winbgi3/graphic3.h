/*
 graphic3.h  -- Wrapper and/or replacement for Borland <graphics.h>
                Use this to make BGI portable to other compilers
                
                GLF 1/19/2010 -- now supports:

                    Turbo C DOS
                    Mingw32 Windows
                    GCC Ubuntu Linux (using X and GLF xwinlite library)
*/

#ifndef __GRAPHIC3_H__
#define __GRAPHIC3_H__

#include <string.h>
#include <math.h>

#if defined(__TURBOC__)
   /* BORLAND GRAPHICS DIRECTLY SUPPORTED */
   #include <graphics.h>

   #include <dos.h>
   #include <stdlib.h>

   static int drivers_registered = 0;

   /* must fix number of parameters to initgraph for Borland version */
   /* also prevent execution if OS is newer than Win 9X ME (else hangs up) */

   /* redefine calls to winbgi2 initgraph() to point to
      the original Borland initgraph(), but only after verifying safety */

   /* parameters below match Borland graphics.h except
      for dummies at end, which satisfy libwinbgi2.a library */

   void far safe_initgraph(int far *graphdriver,
                 int far *graphmode, char far *pathtodriver)
     {
      if (_version != 5)  /* check OS version number for NT/2K/XP/Vista */
        {
         /*
            OK, this gets complicated...   The DOS INT 21H Func 30H returns
            the value also found in TC 2.0's _version variable.
         
            This is supposed to be high byte = major version, low byte = minor.
            When reading the variable as a word, however, due to Intel
            little-endian rules, the byte order would be reversed, maning major 5,
            minor 0 becomes 0x0005 when retrieved from _version.
         
            However, under Windows this value can be reversed, and older OS may
            show higher numbers than earlier ones due to a disconnect
            between DOS-supoported Win9X/ME and NT-based systems after that.
            Windows 98SE shows a major version of 7, but under windows NT and
            above it appears from testing to always show _version = 0x0005.
         
            This should still suffice for a safety check for TC on Windows. If MS-DOS
            5.01 or higher exists, _version should NOT equal 5 (0x0105 perhaps?)
            and all other non-NT based OS's should show OTHER THAN 5.  The problem
            under consideration is ONLY whether NT-class OS is running, since that
            will crash the TC 2.01 BGI library (GRAPHICS.LIB) and hang the (DOS
            or console) command window.
         */
         
         /* assume safe to continue MS-DOS 5.01 or above, Win9X/ME, FreeDOS */
         
         /* because winbgi2 programs don't need to register fonts and video drivers, 
            bury the DOS invocations here, but only once per program run */

         if (!drivers_registered)    /* static, globally initialized to 0 */
           {
            registerfarbgidriver(CGA_driver_far);
            registerfarbgidriver(EGAVGA_driver_far);
            registerfarbgidriver(Herc_driver_far);
            registerfarbgidriver(ATT_driver_far);
            registerfarbgidriver(PC3270_driver_far);
            registerfarbgidriver(IBM8514_driver_far);
            registerfarbgifont(triplex_font_far);
            registerfarbgifont(small_font_far);
            registerfarbgifont(sansserif_font_far);
            registerfarbgifont(gothic_font_far);

            drivers_registered = 1;  
           }

         /* now initialize the graphics using the original Borland function */
         initgraph(graphdriver,graphmode,pathtodriver);
        }
      else
        {
         /* prevent execution if OS is Win 2K, XP or newer (else hangs up) */
         exit(EXIT_FAILURE); /* Abort with error code  */
        }
     }

   /* use the following macro to add to the expected parameters to match
      winbgi2 expectations (the last 2 params are window width and height) */

   #undef  initgraph
   #define initgraph(a,b,c,d,e) safe_initgraph(a,b,c)

   /* finally redefine prototype to restore type safety lost by macro above */

   void far initgraph(int far *graphdriver,
                 int far *graphmode, char far *pathtodriver,
                 int dummy1, int dummy2);

				 
   typedef struct PTS 
     {
      int x, y;
     } 
	  PTS;	/* Structure to hold vertex points	*/
				 
   /* this is defined in xwinlite library -- redefine this as an ESC key for TC */
   #define KB_WINDOW_QUIT 0x1B

#else

/* ----- NOT Turbo C (DOS) -- either Mingw32 (Windows) or Linux (X) -------- */
/*      
         Much of this header code is identical for Windows or X -- 
         differences are conditionally defined 
*/


/* -------------- Windows Only --------------------------------------------- */
#ifdef __MINGW32__

/* most references to structs and functions are identical under Windows or Linux
   unless specifically defined below */
/* assume Windows  and MinGW32 */


#ifndef NOT_USE_PROTOTYPES
#define PROTO(ARGS) ARGS
#else
#define PROTO(ARGS) ()
#endif


/* this is defined in xwinlite library -- redefine this as an ESC key in Windows */
#define KB_WINDOW_QUIT 0x1B

#else
/* -------------- Linux Only ----------------------------------------------- */

/* Linux adaptation of WinBGI and XBGI code -- adapted to use GLF xwinlite
   interface to Xlib ("X Windows") -- end programs must link with both
   libxwinlite.a and libxwinbgi3.a */

/*
 * $Id: graphics.h,v 0.1 1993/12/12 22:18:54 king Exp king $
 *	Reconstructed version of Borland's graphics.h file.
 *
 * $Log: graphics.h,v $
 * Revision 0.1  1993/12/12  22:18:54  king
 * Initial version.
 *
 */

#include <stdio.h>
#include <stdlib.h>


#ifdef __EMX__
#include <float.h>
#define usleep(t) _sleep2( ((t)+500) / 1000 )
#endif


#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>

#define ALLOCATE_COLOR_CELLS 1
/* #define CREATE_NEW_COLORMAP */

#define MAXLINELENGTH					256


#endif

/* -------------------------------------------------------------------------- */

#define far
#define huge

#undef random      /* random() defined differently in glibc -- give it up 
                      and use the Borland macro definition instead */
#define random(range) (rand() % (range))

#ifndef bool
#define bool int
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif


enum colors { 
    BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHTGRAY, DARKGRAY, 
    LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE
};

enum write_modes { 
    COPY_PUT, XOR_PUT, OR_PUT, AND_PUT, NOT_PUT 
};

enum line_styles { 
    SOLID_LINE, DOTTED_LINE, CENTER_LINE, DASHED_LINE, USERBIT_LINE
};
enum fill_styles { 
    EMPTY_FILL, SOLID_FILL, LINE_FILL, LTSLASH_FILL, SLASH_FILL, BKSLASH_FILL,
    LTBKSLASH_FILL, HATCH_FILL, XHATCH_FILL, INTERLEAVE_FILL, WIDE_DOT_FILL, 
    CLOSE_DOT_FILL, USER_FILL
};
enum text_directions { 
    HORIZ_DIR, VERT_DIR
};
enum font_types { 
    DEFAULT_FONT, TRIPLEX_FONT, SMALL_FONT, SANSSERIF_FONT, GOTHIC_FONT 
};

enum graphics_errors {
	grOk = 0,
	grNoInitGraph = -1,
	grNotDetected = -2,
	grFileNotFound = -3,
	grInvalidDriver	= -4,
	grNoLoadMem = -5,
	grNoScanMem = -6,
	grNoFloodMem = -7,
	grFontNotFound = -8,
	grNoFontMem = -9,
	grInvalidMode =	-10,
	grError = -11,
	grIOerror = -12,
	grInvalidFont = -13,
	grInvalidFontNum = -14,
	grInvalidDeviceNum = -15,
	grInvalidVersion = -18
};


#define USER_CHAR_SIZE			0   /* from winbgi2 */

#ifdef __MINGW32__
   #define MAXCOLORS					63 
#else   /* assume Linux */
   #define MAXCOLORS					63 
#endif


/* ---- from winbgi2 -------- */
   #define CLIP_ON					1
   #define CLIP_OFF					0

   #define TOP_ON						1
   #define TOP_OFF	            0

   #define LEFT_BUTTON				0
   #define RIGHT_BUTTON			   1
/* -------------------------- */

#define LEFT_TEXT					0
#define CENTER_TEXT					1
#define RIGHT_TEXT					2
#define BOTTOM_TEXT					0
#define TOP_TEXT					2

#define NORM_WIDTH					1
#define THICK_WIDTH					3

#define DOTTEDLINE_LENGTH				2
#define CENTRELINE_LENGTH				4
#define DASHEDLINE_LENGTH				2

#ifdef __MINGW32__
   #define MAX_PAGES					16
#else
   #define MAX_PAGES					4
#endif

/* -------------------------------------------------------------------------- */
/* Graphics drivers constants, includes X11 which is particular to XBGI. */
#define DETECT						0
#define CGA						1
#define MCGA						2
#define EGA						3
#define EGA64						4
#define EGAMONO						5
#define IBM8514						6
#define HERCMONO					7
#define ATT400						8
#define VGA						9
#define PC3270						10
#define X11						11


/* Graphics modes constants. */
#define CGAC0						0
#define CGAC1						1
#define CGAC2						2
#define CGAC3						3
#define CGAHI						4

#define MCGAC0						0
#define MCGAC1						1
#define MCGAC2						2
#define MCGAC3						3
#define MCGAMED						4
#define MCGAHI						5

#define EGALO						0
#define EGAHI						1

#define EGA64LO						0
#define EGA64HI						1

#define EGAMONOHI					3

#define HERCMONOHI					0

#define ATT400C0					0
#define ATT400C1					1
#define ATT400C2					2
#define ATT400C3					3
#define ATT400MED					4
#define ATT400HI					5

#define VGALO						0
#define VGAMED						1
#define VGAHI						2
#define VGAMAX                3   /* from winbgi2 */

#define PC3270HI					0

#define IBM8514LO					0
#define IBM8514HI					1

#define X11_CGALO					0
#define X11_CGAHI					1
#define X11_EGA						2
#define X11_VGA						3
#define X11_HERC					4
#define X11_PC3270					5
#define X11_SVGALO					6
#define X11_SVGAMED1					7
#define X11_SVGAMED2					8
#define X11_SVGAHI					9
#define X11_USER					10


/* BORLAND MIMICS */

#define BLACK                                           0
#define BLUE                                            1
#define GREEN                                           2
#define CYAN                                            3
#define RED                                             4
#define MAGENTA                                         5
#define BROWN                                           6
#define LIGHTGRAY                                       7
#define DARKGRAY                                        8
#define LIGHTBLUE                                       9
#define LIGHTGREEN                                      10
#define LIGHTCYAN                                       11
#define LIGHTRED                                        12
#define LIGHTMAGENTA                                    13
#define YELLOW                                          14
#define WHITE                                           15

#define CGA_LIGHTGREEN					1
#define CGA_LIGHTRED					2
#define CGA_YELLOW					3

#define CGA_LIGHTCYAN					1
#define CGA_LIGHTMAGENTA				2
#define CGA_WHITE					3

#define CGA_GREEN					1
#define CGA_RED						2
#define CGA_BROWN					3

#define CGA_CYAN					1
#define CGA_MAGENTA					2
#define CGA_LIGHTGRAY					3

#define EGA_BLACK					0
#define EGA_BLUE					1
#define EGA_GREEN					2
#define EGA_CYAN					3
#define EGA_RED						4
#define EGA_MAGENTA					5
#define EGA_LIGHTGRAY					7
#define EGA_BROWN					20
#define EGA_DARKGRAY					56
#define EGA_LIGHTBLUE					57
#define EGA_LIGHTGREEN					58
#define EGA_LIGHTCYAN					59
#define EGA_LIGHTRED					60
#define EGA_LIGHTMAGENTA				61
#define EGA_YELLOW					62
#define EGA_WHITE					63


/* --------------- XBGI Only ------------------------------------------- */
#ifndef __MINGW32__
struct date {
	int da_year;
	int da_day;
	int da_mon;
};

typedef struct {
	int colour_index;
	char *colour_name;
	unsigned long pixel_value;
} bgi_info;


typedef struct {
    int    width;
    int    height;
    Pixmap pixmap;
} bgi_image; 

extern Xwin *mainXwin;

#endif

/* ----------------------------------------------------------------------- */

typedef struct PTS {
  int x, y;
} PTS;	/* Structure to hold vertex points	*/

typedef char fillpatterntype[8];

typedef struct arccoordstype {
    int x;
    int y;
    int xstart;
    int ystart;
    int xend;
    int yend;
} arccoordstype;

typedef struct fillsettingstype {
    int pattern;
    int color;
} fillsettingstype;

typedef struct linesettingstype {
    int linestyle;
    unsigned int upattern;
    int thickness;
} linesettingstype;

typedef struct palettetype {
    unsigned char size;
    signed char colors[64];
} palettetype;

typedef struct textsettingstype {
    int font;	
    int direction;  
    int charsize;
    int horiz;
    int vert;
} textsettingstype;

typedef struct viewporttype {
    int left;
    int top;
    int right;
    int bottom;
    int clip;
} viewporttype;


#if defined(__cplusplus) 
extern "C" {
#endif


/* ------------------- Windows only -------------------------------------- */
#ifdef __MINGW32__
/*
   Setting this variable to 0 increase speed of drawing but
   correct redraw is not possible. By default this variable is initialized by 1
*/

extern int bgiemu_handle_redraw;

/*
   Default mode choosed by WinBGI if DETECT value is specified for 
   device parameter of initgraoh(). Default value is VGAMAX which
   cause creation of maximized window (resolution depends on display mode)
*/

extern int bgiemu_default_mode;
#endif
/* -------------------------------------------------------------------- */

void _graphfreemem (void *ptr, unsigned int size);
void* _graphgetmem (unsigned int size);
void arc (int, int, int, int, int);
void bar (int, int, int, int);
void bar3d (int, int, int, int, int, int);
void circle (int, int, int);
void cleardevice (void);
void clearviewport (void);
void closegraph (void);
void detectgraph (int *, int *);
void drawpoly (int, int *);
void ellipse (int, int, int, int, int, int);
void fillellipse (int, int, int, int);
void fillpoly (int, int *);
void floodfill (int, int, int);
void getarccoords (arccoordstype *);
void getaspectratio (int *, int *);
int getbkcolor (void);
int getcolor (void);
palettetype* getdefaultpalette (void);
char* getdrivername (void);
void getfillpattern (char const *);
void getfillsettings (fillsettingstype *);
int getgraphmode (void);
void getimage (int, int, int, int, void *);
void getlinesettings (linesettingstype *);
int getmaxcolor (void);
int getmaxmode (void);
int getmaxx (void);
int getmaxy (void);
char* getmodename (int);
void getmoderange (int, int *, int *);
void getpalette (palettetype *);
int getpalettesize (void);
int getpixel (int, int);
void gettextsettings (textsettingstype *);
void getviewsettings (viewporttype *);
int getx (void);
int gety (void);
void graphdefaults (void);
char* grapherrormsg (int);
int graphresult (void);
unsigned int imagesize (int, int, int, int);
void initgraph (int *, int *, char const *,int , int);
int installuserdriver (char const *, int *);
int installuserfont (char const *);
void line (int, int, int, int);
void linerel (int, int);
void lineto (int, int);
void moverel (int, int);
void moveto (int, int);
void outtext (char const *);
void outtextxy (int, int, char const *);
void pieslice (int, int, int, int, int);
void putimage (int, int, void *, int);
void putpixel (int, int, int);
void rectangle (int, int, int, int);
int registerbgidriver (void *);
int registerbgifont (void *);
void restorecrtmode (void);
void sector (int, int, int, int, int, int);
void setactivepage (int);
void setallpalette (palettetype *);
void setaspectratio (int, int);
void setbkcolor (int);
void setcolor (int);
void setfillstyle (int, int);
unsigned int setgraphbufsize (unsigned int);
void setgraphmode (int);
void setlinestyle (int, unsigned int, int);
void setpalette (int, int);
void setrgbpalette (int, int, int, int);
void settextjustify (int, int);
void settextstyle (int, int, int);
void setusercharsize (int, int, int, int);
void setviewport (int, int, int, int, int);
void setvisualpage (int);
void setwritemode (int);
int textheight (char const *);
int textwidth (char const *);

int getch (void);
int kbhit (void);		

#if defined(__MINGW32__) || defined(__TURBOC__)
void setfillpattern (char const *, int);
#else
void setfillpattern (unsigned char const *, int);
char *itoa(int value, char *string, int radix);
#endif

/* ---------------------- Windows only -------------------------------- */
#ifdef __MINGW32__
void delay (unsigned msec);
void restorecrtmode (void);

bool mouseup();
bool mousedown();
void clearmouse();
int mouseclickx();
int mouseclicky();
int mousecurrentx();
int mousecurrenty();
int whichmousebutton();
#endif
/* ---------------------------------------------------------------------- */

/* --------------- XBGI Only ------------------------------------------- */
#ifndef __MINGW32__
/* void get_date (date *); */

extern unsigned long background_pixel; 
extern Display *dpy;
extern char *display_name;
extern GC gc;
extern GC fill_gc;
extern Window window;
extern Drawable drawable;
extern Pixmap *pages;
extern int active_page;
extern int visual_page;
extern Pixmap fill_pattern;
extern Region region;
extern XRectangle xrectangle;
extern Colormap cmap;
extern int aspectratio_x, aspectratio_y; 
extern XPoint CP;
extern XPoint VPorigin;

extern char *Colors[];

extern char *Fonts[];

extern int dash_list_length[];
extern unsigned char dottedline[DOTTEDLINE_LENGTH];
extern unsigned char centreline[CENTRELINE_LENGTH];
extern unsigned char dashedline[DASHEDLINE_LENGTH];
extern unsigned char *dash_list[];

extern unsigned long plane_mask[4];
extern unsigned long plane_masks;

extern bgi_info *vga_palette;
extern textsettingstype txt_settings;
extern fillsettingstype fill_settings;
extern viewporttype view_settings;
extern arccoordstype last_arc;
#endif
/* ----------------------------------------------------------------------- */


#if defined(__cplusplus) 
};
#endif

#endif   /* ...NOT Turbo C (DOS) */

#endif  /* __GRAPHIC3_H__ */
