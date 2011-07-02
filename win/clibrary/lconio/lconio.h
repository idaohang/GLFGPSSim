/* lconio.h -- adapted from linux-conio.c (see below) by Gary Flispart
               12/5/2008 
                
               Substitute for Borland or Mingw32 conio.h using curses calls 
               -- some functions updated to match PDCurses 3.3 and X Windows 

*/
               
/***************************************************************************/
/** File: conio.h     Date: 03/09/1997       Version: 1.02                **/
/** --------------------------------------------------------------------- **/
/** CONIO.H an implementation of the conio.h for Linux based on ncurses.  **/
/** This is copyright (c) 1996,97 by Fractor / Mental EXPlosion.          **/
/** If you want to copy it you must do this following the terms of the    **/
/** GNU Library Public License                                            **/
/** Please read the file "README" before using this library.              **/
/** --------------------------------------------------------------------- **/
/** To use this thing you must have got the curses library.               **/
/** This thing was tested on Linux Kernel 2.0.29, GCC 2.7.2 and           **/
/** ncurses 1.9.9e which is (c) 1992-1995 by Zeyd M. Ben-Halim and Eric S.**/
/** Raymond.                                                              **/
/** Please also read the copyright notices for ncurses before using this !**/
/***************************************************************************/             

#ifndef LCONIO_H__
#define LCONIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <curses.h>

#define linux_conio_version 1.02

/* added 12/5/2008 by GLF to adapt expanded parameters of wattr_get() */
typedef struct 
  {
   attr_t attrs;
   short color_pair;
   void *opts;
  }
   attr_store_type;

extern int directvideo; /* ignored by linux conio.h */
extern char color_warning; /* Tell the users when terminal can't display colors ? */
extern WINDOW *conio_scr;
 
struct text_info {
    unsigned char winleft;
    unsigned char wintop;
    unsigned char winright;
    unsigned char winbottom;
    unsigned char attribute;
    unsigned char normattr;
    unsigned char currmode;
    unsigned char screenheight;
    unsigned char screenwidth;
    unsigned char curx;
    unsigned char cury;
};

enum text_modes { LASTMODE=-1, BW40=0, C40, BW80, C80, MONO=7, C4350=64 };

enum COLORS {
   BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHTGRAY,
   DARKGRAY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE
};

#define BLINK 128    

extern void init_lconio(void); /* Please run this function before any other */ 
extern void done_lconio(void); /* Please run this function before exiting your program */

extern int wherex(void);
extern int wherey(void);
extern int putch(int c);
extern int getche(void); 

/* curses.h defines a getch() macro, so override it first... */
#ifdef getch
#undef getch
#endif

extern int getch(void);  /* replacement compatible both curses.h and conio.h */

extern int kbhit(void);
extern int cprintf(char *format, ...);
extern int cscanf(const char *format, ...); 

extern void clreol(void);
extern void clrscr(void);
extern void gotoxy(int x, int y);
extern void delline(void);
extern void gettextinfo(struct text_info *r);
extern void highvideo(void);
extern void insline(void);
extern void lowvideo(void);
extern void normvideo(void);
extern void textattr(int attribute);
extern void textbackground(int color);
extern void textcolor(int color);
extern void textmode(int unused_mode);
extern void window(int left, int top, int right, int bottom);
extern void cputs(char *str);

extern char *cgets(char *str); 

/* added by GLF to support conversions of programs using gflib.h and getln() */
/* equivalents of getl() and getln() which use only lconio + curses calls */
char *cgetl(char strg[], int n);
char *cgetln(char strg[], int n);


#ifdef  __cplusplus
}
#endif

#endif
