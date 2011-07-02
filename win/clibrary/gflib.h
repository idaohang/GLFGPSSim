/* GFLIB.H -- Header file for LIB.C -- General Function library -- GLF */
/*            Matches LIB.C version of 8/14/2000                       */

#ifndef GFLIB_H_
#define GFLIB_H_

/* the following are very common and are included here for convenience */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#undef TRUE
#define TRUE 1

#undef FALSE
#define FALSE 0

/* GNU word size macros */
#undef BIT16
#define BIT16 short int

#undef BIT32
#define BIT32 long int

/* the following converts a char to a temporary string */
char *tostring(char chr);

/* the following are functions to extract/insert data from/to char strings */
int strvi(void *strg);
long strvl(void *strg);
unsigned int strvui(void *strg);
unsigned long strvul(void *strg);
float strvf(void *strg);
double strvd(void *strg);
int xstrvi(void *strg, int v);
long xstrvl(void *strg, long v);
unsigned int xstrvui(void *strg, unsigned int v);
unsigned long xstrvul(void *strg, unsigned long v);
float xstrvf(void *strg, float v);
double xstrvd(void *strg, double v);
void strfill(char strg[], int lim, char ch);
void strmin(char strg[], int n);
void strmax(char strg[], int lim, char ch);
char *strlj(char dstrg[], char strg[], int pos, int len);
void strpad(char strg[], char ch, int lim);
void strpad2(char strg[], char ch, int lim);
void strunpad(char strg[], char ch, int lim);
char *strrj(char dstrg[], char strg[], int pos, int len);
int strcount(char *strings[]);
extern int strpos;
extern int strbegin;
char *strfmt(int len, int dp, char chr);
char *strs(char dstrg[], char strg[], int pos, int len);
char strc(char strg[], int pos, int len);
int strtoken(char strg[], int pos, int len, char *tokens[]);
long strl(char strg[], int pos, int len);
long strxl(char strg[], int pos, int len);
double strd(char strg[], int pos, int len);
unsigned long strul(char strg[], int pos, int len);
unsigned long strxul(char strg[], int pos, int len);
int stri(char strg[], int pos, int len);
unsigned int strui(char strg[], int pos, int len);
int strxi(char strg[], int pos, int len);
float strf(char strg[], int pos, int len);
long dtofix(int dp, double temp);
double fixtod(int dp, long xfix);
long strfix(char strg[], int pos, int len, int dp);
int xstrs(char strg[], int pos, int len, int dp, char xstrg[]);
int xstrc(char strg[], int pos, int len, int dp, char xchar);
int xstrtoken(char strg[], int pos, int len, int dp, char *tokens[], int xkey);
int xstrl(char strg[], int pos, int len, int dp, long xlong);
int xstrul(char strg[], int pos, int len, int dp, unsigned long xlong);
int xstrxl(char strg[], int pos, int len, int dp, long xlong);
int xstrulx(char strg[], int pos, int len, int dp, unsigned long xlong);
int xstrd(char strg[], int pos, int len, int dp, double xdouble);
int xstri(char strg[], int pos, int len, int dp, int xint);
int xstrui(char strg[], int pos, int len, int dp, unsigned int xint);
int xstrxi(char strg[], int pos, int len, int dp, int xint);
int xstrf(char strg[], int pos, int len, int dp, float xfloat);
int xstrfix(char strg[], int pos, int len, int dp, long xfix);

/* the following are a well-behaved set of i/o routines */
char *getl(char strg[], int n);
char *getln(char strg[], int n);
char *getbyte(char strg[], int n);
char *fgetl_notab(FILE *stream, char strg[], int n);
char *fgetl(FILE *stream, char strg[], int n);
int fgetb(FILE *stream, long offset, char buffer[], int n);
void pput(char strg[]);
void pputl(char strg[]);
void pputb(char strg[], int n);
void put(char strg[]);
void putl(char strg[]);
void putb(char strg[], int n);
void fput(FILE *stream, char strg[]);
void fputl(FILE *stream, char strg[]);
int fputb(FILE *stream, long offset, char buffer[], int n);
long fend(FILE *stream);

/* the following is a dump for debugging and/or data display */
void dumpb(unsigned char *buffer, int size, long ref);

/* the following make up a batch-style command list parsing driver */
extern int batchlevel;
char *batchproclist(char *strglist[], char *parser());
char *batchprocfile(char name[], char *parser());

#if !defined(__TURBOC__) && !defined(__MINGW32__)
   /* add commonly used functions missing from glibc */
   char *strupr(char *s);
   char *strlwr(char *s);
#endif


#endif
