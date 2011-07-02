/* lconio.c -- adapted from linux-conio.c (see below) by Gary Flispart
               12/5/2008 
                
               Substitute for Borland or Mingw32 conio.h using curses calls 
               -- some functions updated to match PDCurses 3.3 and X Windows 

*/
               

/****************************************************************************
 * This is the implementation file for conio.h - a conio.h for Linux.       *
 * It uses ncurses and some internal functions of Linux to simulate the     *
 * I/O-functions.                                                           *
 * This is copyright (c) 1996,97 by Fractor / Mental eXPlosion (MXP)        *
 * Use and distribution is only allowed if you follow the terms of the      *
 * GNU Library Public License Version 2.                                    *
 * Since this work bases on ncurses please read it's copyright notices as   *
 * well !                                                                   *
 * Look into the readme to this file for further information.               *
 * Thanx to SubZero / MXP for his little tutorial on the curses library !   *
 * Many thanks to Mark Hahn and Rich Cochran for solving the inpw and inpd  *
 * mega-bug !!!                                                             *
 * Watch out for other MXP releases, too !                                  *
 * Send bugreports to: fractor@germanymail.com                              *
 ****************************************************************************/

#include <stdio.h>
#include <string.h>

#include "lconio.h"    /* includes curses.h */

static int txtattr,oldattr;

static attr_store_type store_txtattr;
static attr_store_type store_oldattr;

static int fgc,bgc;
static int initialized=0;
char color_warning=1;
int directvideo;
WINDOW *conio_scr;

/* Some internals... */
int colortab(int a) /* convert LINUX Color code to DOS-standard */
{
   switch(a) {
      case 0 : return COLOR_BLACK;
      case 1 : return COLOR_BLUE;
      case 2 : return COLOR_GREEN;
      case 3 : return COLOR_CYAN;
      case 4 : return COLOR_RED;
      case 5 : return COLOR_MAGENTA;
      case 6 : return COLOR_YELLOW;
      case 7 : return COLOR_WHITE;
   }
} 

void docolor (int color) /* Set DOS-like text mode colors */
{
   wattrset(conio_scr,0); /* My curses-version needs this ... */
   if ((color&128)==128) txtattr=A_BLINK; else txtattr=A_NORMAL;
   if ((color&15)>7) txtattr|=A_STANDOUT; else txtattr|=A_NORMAL;
   txtattr|=COLOR_PAIR((1+(color&7)+(color&112)) >> 1);
   if (((color&127)==15) | ((color&127)==7)) txtattr=COLOR_PAIR(0);
   if (((color&127)==127) | ((color&127)==119)) txtattr=COLOR_PAIR(8);
   wattron(conio_scr,txtattr);
   wattron(conio_scr,COLOR_PAIR(1+(color&7)+((color&112)>>1))); 
}


static char *fakelist[] = 
  {
   "program",
   NULL    
  };

/* Call this before any call to linux conio - except the port functions ! */
void init_lconio (void) /* This is needed, because ncurses needs to be initialized */
{
   int x,y;
   initialized=1;
   initscr();

   /* initialize curses screen -- modified by GLF to detect X Windows */
#ifdef XCURSES
   Xinitscr(1, fakelist); /* fake out argument list */
#else
   initscr();
#endif

   start_color();
   oldattr=wattr_get(stdscr,&store_oldattr.attrs,&store_oldattr.color_pair,&store_oldattr.opts);

   nonl();    /* disable translation of CR to newline on input */
   raw();     /* characters immediately available -- break not processed; like 
                 cbreak() but also passes INTR, QUIT, SUSP, and STOP chars  */
   noecho();  /* no echo of key input */

/*
   Removed from original linux-conio:
           
   if (!has_colors() & (color_warning>-1))
      fprintf(stderr,"Attention: A color terminal may be required to run this application !\n");   
*/


   /* GLF settings added to the original linux-conio initializations */

   curs_set(0);         /* turn off cursor */

   /* cbreak(); */ /* characters immediately available -- break not processed */

	clear();     /* clear screen */
	timeout(0);  /* non-blocking read -- if no input, return ERR */

   conio_scr=newwin(0,0,0,0);
   keypad(conio_scr,TRUE);
   meta(conio_scr,TRUE);
   idlok(conio_scr,TRUE);
   scrollok(conio_scr,TRUE);
   /* Color initialization */
   for (y=0;y<=7;y++)
      for (x=0;x<=7;x++)
         init_pair((8*y)+x+1, colortab(x), colortab(y));              
   txtattr=wattr_get(conio_scr,&store_txtattr.attrs,&store_txtattr.color_pair,&store_txtattr.opts);
   bgc=0;
   textcolor(7);
   textbackground(0);
}

/* Call this on exiting your program */
void done_lconio (void)
{
   /* GLF settings added to the original linux-conio initializations */
	timeout(-1);	   /* turn off timeout() */
	curs_set(1);		/* turn cursor back on */
	
   endwin();
}

/* Here it starts... */
char *cgets (char *str) /* ugly function :-( */
{
   char strng[257];
   unsigned char i=2;
   if (initialized==0) init_lconio();
   echo();
   strncpy(strng,str,1);
   wgetnstr(conio_scr,&strng[2],(int) strng[0]);
   while (strng[i]!=0) i++;
   i=i-2;
   strng[1]=i;
   strcpy(str,strng);
   noecho();
   return(&str[2]);
}

void clreol (void)
{
   if (initialized==0) init_lconio();
   wclrtoeol(conio_scr);
   wrefresh(conio_scr);
}

void clrscr (void)
{
   if (initialized==0) init_lconio();
   wclear(conio_scr);
   wmove(conio_scr,0,0);
   wrefresh(conio_scr);
}

int cprintf (char *format, ... )
{
   int i;
   char buffer[BUFSIZ]; /* Well, BUFSIZ is from ncurses...  */
   va_list argp;
   if (initialized==0) init_lconio();
   va_start(argp,format);
   vsprintf(buffer,format,argp);
   va_end(argp);
   i=waddstr(conio_scr,buffer);
   wrefresh(conio_scr);
   return(i);
}

void cputs (char *str)
{
   if (initialized==0) init_lconio();
   waddstr(conio_scr,str);
   wrefresh(conio_scr);
}

int cscanf (const char *format, ...)
{
   int i;
   char buffer[BUFSIZ]; /* See cprintf */
   va_list argp;
   if (initialized==0) init_lconio();
   echo();
   if (wgetstr(conio_scr,buffer)==ERR) return(-1);                    
   va_start(argp,format);
   i=vsscanf(buffer,format,argp);                         
   va_end(argp);
   noecho();
   return(i);
}

void delline (void)
{
   if (initialized==0) init_lconio();  
   wdeleteln(conio_scr);
   wrefresh(conio_scr);
}

void gettextinfo(struct text_info *inforec)
{
   unsigned char xp,yp;
   unsigned char x1,x2,y1,y2;
   unsigned char cols,lines;
   unsigned char dattr,dnattr,a; /* The "d" stands for DOS */

   attr_store_type store_a_attr;

   if (initialized==0) init_lconio();
   getyx(conio_scr,yp,xp);
   getbegyx(conio_scr,y1,x1);
   getmaxyx(conio_scr,y2,x2);
   dattr=(bgc*16)+fgc;
   a = wattr_get(conio_scr,&store_a_attr.attrs,&store_a_attr.color_pair,&store_a_attr.opts);

   if (a==(a & A_BLINK)) dattr=dattr+128;
   dnattr=oldattr;  /* Well, this cannot be right, 
                       because we don't know the COLORPAIR values from before init_lconio() !*/
   inforec->winleft=x1+1;
   inforec->wintop=y1+1;
   if (x1==0) x2--;
   if (y1==0) y2--;
   inforec->winright=x1+x2+1;
   inforec->winbottom=y1+y2+1;
   inforec->curx=xp+1;
   inforec->cury=yp+1;
   inforec->screenheight=y2+1;
   inforec->screenwidth=x2+1;
   inforec->currmode=3; /* This is C80 */
   inforec->normattr=dnattr; /* Don't use this ! */
   inforec->attribute=dattr;
} 

void gotoxy (int x, int y)
{
   if (initialized==0) init_lconio();
   y--;
   x--;
   wmove(conio_scr,y,x);
   wrefresh(conio_scr);
}

void highvideo (void)
{
   if (initialized==0) init_lconio();
   textcolor(15); /* White */
   textbackground(0); /* Black */
}

void insline (void)
{ 
   if (initialized==0) init_lconio();
   winsertln(conio_scr);
   wrefresh(conio_scr);
}

/* added by GLF to make kbhit() more compatible with Borland conio.h */
static int holdchar = 0;
static int have_kbhit = 0;

int kbhit (void)
{
   int i;
   
   if (initialized==0) init_lconio();
   
   if (have_kbhit)    /* if not yet reset by getch() or getche(), 
                         don't lose last char */
     {                    
      return holdchar;
     }
      
   nodelay(conio_scr,TRUE);
   i=wgetch(conio_scr);
   nodelay(conio_scr,FALSE);

   /* hold onto any char received */
   holdchar = i;           

   /* save status of kbhit() for later use by getch() or getche() */
   have_kbhit = 1;
   if (i == ERR)  /* ERR should be -1 */
     {
      i=0; 
      have_kbhit = 0;       
     }

   holdchar = i;  
   return(i);
}

int getche(void)
{
   int i;
   if (initialized==0) init_lconio();

   /* give first crack at any stored char from kbhit() */
   if (have_kbhit)
     {
      have_kbhit = 0;            
      return holdchar;            
     }

   /* force echo on for this char only */
   echo();
   
   i=wgetch(conio_scr);
   if (i == ERR)  /* ERR should be -1 */
     {
      i=0;
     } 

   have_kbhit = 0;     /* kbhit() must set itself, not getch() or getche() */
   holdchar = i;       /* update character regardless of who saw it first */

   /* force back to default of no echo */
   noecho();

   return(i);
}

/* curses.h defines a getch() macro, so override it first... */
#ifdef getch
#undef getch
#endif

/* following is a getch() compatible with both curses.h and conio.h */
int getch(void)
{
   int i;
   if (initialized==0) init_lconio();

   /* force echo off regardless */
   noecho();
   
   /* give first crack at any stored char from kbhit() */
   if (have_kbhit)
     {
      have_kbhit = 0;            
      return holdchar;            
     }

   i=wgetch(conio_scr);
   if (i == ERR)  /* ERR should be -1 */
     {
      i=0;
      have_kbhit = 0;       
     }

   have_kbhit = 0;     /* kbhit() must set itself, not getch() or getche() */
   holdchar = i;       /* update character regardless of who saw it first */
   return(i);
}

void lowvideo (void)
{
   if (initialized==0) init_lconio();
   textbackground(0); /* Black */
   textcolor(8); /* Should be darkgray */
}

void normvideo (void)
{
   if (initialized==0) init_lconio();
   wattrset(conio_scr,oldattr);
}


/* the putch() below is reworked by GLF to avoid a perverse problem with LFs and
   waddch() -- this version is more compatible with conio.h characteristics */
   
int putch (int c)
{
   int x,y; 
    
   if (initialized==0) init_lconio();
   
   /* intercept LF and replace with a move to one row downwards */
   if (c == 0x0A)
     {
      /* NOTE: getyx() is a MACRO that doesn't use POINTERS to y and x 
               but refers to the variables directly (confusing, non-C-like) 
               -- ordinarily one couldn't use a function to alter int variable 
               contents without pointers to the variables (parameters are 
               copies -- call by value, not by reference) -- GLF
      */   
      getyx(conio_scr,y,x); 
      
      if (y < (LINES-1)) /* increment row (not column) unless at last line */
        {
         y++;  
        } 
      wmove(conio_scr,y,x);   
      wrefresh(conio_scr); 
      return(c);
     }
   if (waddch(conio_scr,c) != ERR) 
     {
      wrefresh(conio_scr); 
      return(c);
     }
   return(0);
}
                                     
void textattr (int attr)
{
   if (initialized==0) init_lconio();
   docolor(attr);
}

void textbackground (int color)
{
   if (initialized==0) init_lconio();
   bgc=color;
   color=(bgc*16)+fgc;
   docolor(color);
}

void textcolor (int color)
{
   if (initialized==0) init_lconio();
   fgc=color;
   color=(bgc*16)+fgc;
   docolor(color);
}
 
void textmode (int mode)
{
   if (initialized==0) init_lconio();
   /* Ignored */
}

int wherex (void)
{
   int y;
   int x;
   if (initialized==0) init_lconio();
   getyx(conio_scr,y,x);
   x++;
   return(x);
}

int wherey (void)
{
   int y;
   int x;
   if (initialized==0) init_lconio();
   getyx(conio_scr,y,x);
   y++;
   return(y);
}

void window (int left,int top,int right,int bottom)
{
   int nlines,ncols;
   if (initialized==0) init_lconio();
   delwin(conio_scr);
   top--;
   left--;
   right--;
   bottom--;
   nlines=bottom-top;
   ncols=right-left;
   if (top==0) nlines++;
   if (left==0) ncols++;
   if ((nlines<1) | (ncols<1)) return;
   conio_scr=newwin(nlines,ncols,top,left);   
   keypad(conio_scr,TRUE);
   meta(conio_scr,TRUE);
   idlok(conio_scr,TRUE);
   scrollok(conio_scr,TRUE);
   wrefresh(conio_scr);
}

/* Linux is cool */


/* added by GLF to support conversions of programs using gflib.h and getln() */
/* equivalents of getl() and getln() which use only lconio + curses calls */
char *cgetlq(char strg[], int n)
{
 int i;
 
 if (initialized==0) init_lconio();

 if (n > 253)
    n = 253;
 
 curs_set(1);
 timeout(-1);

 echo();
 wgetnstr(conio_scr,strg,n);
 noecho();

 timeout(0);
 curs_set(0);
 
 for (i=0; i<n; i++)
   {
    if ((strg[i] == '\n') || (strg[i] == 0))
      {
		 strg[i] = 0;
		 break;
		}
   }
 strg[n] = 0; 
 return strg;
}

char *cgetlnq(char strg[], int n)
{
 int i;
 
 if (initialized==0) init_lconio();

 if (n > 253)
    n = 253;
 
 curs_set(1);
 timeout(-1);

 echo();
 wgetnstr(conio_scr,strg,n);
 noecho();

 timeout(0);
 curs_set(0);

 cprintf("\n\r");
 
 for (i=0; i<n; i++)
   {
    if ((strg[i] == '\n') || (strg[i] == 0))
      {
		 strg[i] = 0;
		 break;
		}
   }
 strg[n] = 0; 
 return strg;
}




/* equivalents of getl() and getln() which use only lconio + curses calls */
static char buffer[260];

char *cgetl(char strg[], int n)
{
 int i;
 char *ptr;
 
 if (n > 253)
    n = 253;
 
 curs_set(1);
 timeout(-1);

 buffer[0] = n;
 ptr = cgets(buffer);

 timeout(0);
 curs_set(0);
 
 if (ptr != NULL)
   {
     for (i=2; i<(n+2); i++)
      {
	   if ((buffer[i] == '\n') || (buffer[i] == 0))
		 {
		  strg[i-2] = 0;
		  break;
		 }
	   strg[i-2] = buffer[i];
	  }
	 strg[n] = 0; 
    return strg;
   }
 else
   {
    strg[0] = 0;
    return NULL;
   }
}

char *cgetln(char strg[], int n)
{
 int i;
 char *ptr;

 if (n > 253)
    n = 253;

 curs_set(1);
 timeout(-1);

 buffer[0] = n;
 ptr = cgets(buffer);

 timeout(0);
 curs_set(0);

 cprintf("\n\r");
 
 if (ptr != NULL)
   {
    for (i=2; i<(n+2); i++)
      {
	    if ((buffer[i] == '\n') || (buffer[i] == 0))
		  {
		   strg[i-2] = 0;
		   break;
		  }
	   strg[i-2] = buffer[i];
	  }
	 strg[n] = 0; 
    return strg;
   }
 else
   {
    strg[0] = 0;
    return NULL;
   }
}



