/* GFLIB.C -- General Function library -- Gary Flispart  */
/*          Matches GFLIB.H version of 8/14/2000         */

#include "gflib.h"

#ifdef __TURBOC__
/*
   This function is NOT intended to be used!  It is a placeholder which takes
   care of a "FEATURE" of Borland compilers including Turbo C 2.01 which causes
   floating point libraries to remain unlinked (in error) if they are not
   explicitly invoked.  The following function forces the linker to include
   floating point code.

   Thanks to Google for the search engine, and to Jeffrey Carlyle for his FAQ
   website where I found the crucial information:

   http://www.faqs.org/faqs/msdos-programmer-faq/part2/section-5.html
*/

   static void forcefloat(float *p)
     {
      float f = *p;

      forcefloat(&f);
     }

#endif


static char str_____fmt[20];
static char buffer[256];
char *tostring(char chr)
  {
   buffer[0] = chr;
   buffer[1] = '\0';
   return buffer;
  }

int strvi(void *strg)
{
 return *(int*)strg;
}

long strvl(void *strg)
{
 return *(long*)strg;
}

unsigned int strvui(void *strg)
{
 return *(unsigned int*)strg;
}

unsigned long strvul(void *strg)
{
 return *(unsigned long*)strg;
}

float strvf(void *strg)
{
 return *(float*)strg;
}

double strvd(void *strg)
{
 return *(double*)strg;
}

int xstrvi(void *strg, int v)
{
 *(int*)strg = v;
 return v;
}

long xstrvl(void *strg, long v)
{
 *(long*)strg = v;
 return v;
}

unsigned int xstrvui(void *strg, unsigned int v)
{
 *(unsigned int*)strg = v;
 return v;
}

unsigned long xstrvul(void *strg, unsigned long v)
{
 *(unsigned long*)strg = v;
 return v;
}

float xstrvf(void *strg, float v)
{
 *(float*)strg = v;
 return v;
}

double xstrvd(void *strg, double v)
{
 *(double*)strg = v;
 return v;
}

void strfill(char strg[], int lim, char ch)
{
 register int i;
 memset(strg,ch,lim);
 strg[lim] = 0;
}

void strmin(char strg[], int n)
{
 register int i;
 for (i=0; (i < n) && (strg[i]); i++)
   {
   }
 strg[i] = 0;
}

void strmax(char strg[], int lim, char ch)
{
 register int ilen;
 register int i;
 ilen = strlen(strg);
 if (lim < 0)
    lim = 0;
 if (ilen > lim)
    ilen = lim;
 for (i=ilen; i < lim; i++)
   {
    strg[i] = ch;
   }
 strg[lim] = 0;
}

char *strlj(char dstrg[], char strg[], int pos, int len)
{
 register int i;
 register int j;
 int end;
 int nolead;
 end = pos + len;
 nolead = FALSE;
 for (j=0,i=pos; i < end; i++)
   {
    if ((nolead) || (strg[i] != ' '))
      {
       dstrg[j] = strg[i];
       nolead = TRUE;
       j++;
      }
   }
 dstrg[j] = 0;
 return dstrg;
}

void strpad(char strg[], char ch, int lim)
{
 int ilen;
 int i;
 ilen = strlen(strg);
 if (lim < 1)
    lim = 1;
 if (ilen > lim)
    ilen = lim;
 for (i=ilen; i < lim; i++)
   {
    strg[i] = ch;
   }
 strg[lim] = 0;
}

void strpad2(char strg[], char ch, int lim)
{
 int ilen;
 int i;
 ilen = strlen(strg);
 if (lim < 1)
    lim = 0;
 if (ilen > lim)
    return;
 for (i=ilen; i < lim; i++)
   {
    strg[i] = ch;
   }
 strg[lim] = 0;
}

void strunpad(char strg[], char ch, int lim)
{
 int ilen;
 int i;
 strg[lim] = 0;
 for (i=lim; i >= 0; i--)
   {
    if (strg[i] == ch)
      {
       strg[i] = 0;
      }
    if (strg[i]) break;
   }
}

char *strrj(char dstrg[], char strg[], int pos, int len)
  {
   register int i;
   register int j;
   int slen;
   if (len < 0)
      len = -len;
   strlj(dstrg,strg,pos,len);
   slen = strlen(dstrg);
   strunpad(dstrg,' ',slen);
   slen = strlen(dstrg);
   for (i=slen-1,j=len-1; i >=0; i--,j--)
     {
      dstrg[j] = dstrg[i];
     }
   for(; j >= 0; j--)
     {
      dstrg[j] = ' ';
     }
   dstrg[len] = 0;
   return dstrg;
  }

int strcount(char *strings[])
{
 int i;
 i=0;
 while (strings[i] != NULL)
    {
     i++;
     if (i == 0)
        return -1;     /* limit to 65535 tries */
    }
 return i;
}

int strpos;
int strbegin = 0;

char *strfmt(int len, int dp, char chr)
{
 char work[8];
 int siz;
 if (len < 0)
   {
    len = (len < -255 ? -255 : len);
    if (dp > -len)
       dp = -len;
   }
 else
   {
    len = (len > 255 ? 255 : len);
    if (dp > len)
       dp = len;
   }
 strcpy(str_____fmt,"%");
 if (len != 0)
   {
    sprintf(work,"%d",len);
    strcat(str_____fmt,work);
    if (dp >= 0)
      {
       sprintf(work,".%d",dp);
       strcat(str_____fmt,work);
      }
   }
 switch (toupper(chr))
   {
    case 'X':
      {
       if (chr == 'X')  /* if capital, use long */
          strcat(str_____fmt,"l");
       chr = 'X';    /* force capital form of hex digits */
       break;
      }
    case 'D':
    case 'I':
    case 'U':
    case 'E':
    case 'F':
    case 'G':
    case 'C':
    case 'S':
      {
       if (toupper(chr) == chr)  /* if capital, use long */
         {
          strcat(str_____fmt,"l");
          chr = tolower(chr);
         }
       break;
      }
    default:
      {
       chr = 's';
      }
   }
 siz = strlen(str_____fmt);
 str_____fmt[siz] = chr;
 siz++;
 str_____fmt[siz] = 0;
 return str_____fmt;
}

char *strs(char dstrg[], char strg[], int pos, int len)
{
 int j;
 int end;
 pos = pos - strbegin;
 if (len > 0)
   {
    end = pos + len;
    j = 0;
    while (pos < end)
      {
       dstrg[j] = strg[pos];
       pos++;
       j++;
      }
    dstrg[j] = 0;
   }
 else
   {
    if (len == 0)    /* get string until trailing whitespace */
      {
       pos = pos + strspn(strg+pos," ");   /* skip leading blanks */
       j=0;
       while (strg[pos] && (strg[pos] != ' ') && (strg[pos] != ','))
         {
          dstrg[j] = strg[pos];
          j++;
          pos++;
         }
       dstrg[j] = 0;
       if (strg[pos] == ',')
          pos++;
      }
    else            /* get string until inconsistent char (end of string) */
      {
       strcpy(dstrg,strg+pos);
       pos = strlen(strg);
      }
   }
 strpos = pos + strbegin;
 return dstrg;
}

char strc(char strg[], int pos, int len)
{
 int bpos;
 strpos = pos;
 if (len <= 0)
   {
    pos = pos - strbegin;
    bpos = strspn(strg+pos," ,");
    if ((strg[pos] == ' ') || (strg[pos] == ','))
      {
       strpos = strpos + bpos;
       return ' ';
      }
    pos = pos + bpos;
    strpos = pos + strbegin;
    if (bpos > 0) return ' ';
   }
 if (strg[pos])
   {
    strpos++;
   }
 return strg[pos];
}


#ifndef __TURBOC__
#ifndef __MINGW32__
/* assume Linux */

/* if Linux, glibc does not define strlwr() or strupr() functions -- provide here */

#include <ctype.h>

char *strupr(char *s)
  {
   char *t = s;
    
   while (*s)
     {
      *s = toupper(*s);
      s++;
     }

   return t;
  }


char *strlwr(char *s)
  {
   char *t = s;
    
   while (*s)
     {
      *s = tolower(*s);
      s++;
     }

   return t;
  }

#endif
#endif

int strtoken(char strg[], int pos, int len, char *tokens[])
{
 int bpos;
 int blen;
 int bct;
 int bend;
 len = (len > 255 ? 255 : len);
 strpos = pos;
 pos = pos - strbegin;
 if (len <= 0)
   {
    bpos = strspn(strg+pos," ");
    pos = pos + bpos;
    blen = strcspn(strg+pos," ,");
    blen = (blen > 255 ? 255 : blen);
   }
 else
   {
    blen = len;
   }
 bct = 0;
 strncpy(buffer,strg+pos,blen);
 buffer[blen] = 0;
 strlwr(buffer);
 blen = strlen(buffer);
 bpos = strspn(buffer," ");
 bend = bpos + strcspn(buffer+bpos," ,");
 buffer[bend] = 0;
 while (tokens[bct] != NULL)
   {
    if (strcmp(buffer+bpos,tokens[bct]) == 0)  /* match to token */
      {
       if (len < 0)
         {
          pos = pos + strlen(tokens[bct]);
         }
       if (len == 0)
         {
          pos = pos + strcspn(strg+pos," ");  /* span to trailing whitespace */
         }
       if (len > 0)
         {
          pos = pos + blen;
         }
       if (strg[pos] == ',')
          pos++;
       strpos = pos + strbegin;
          return bct;
      }
    bct++;
    if (bct == 0)
       break;      /* limit to 65535 */
   }
 return -1;
}

long strl(char strg[], int pos, int len)
{
 int base = 10;
 char *ptr;
 long val;
 int bpos;
 if (len < 0)
   {
    pos = pos - strbegin;
    bpos = strspn(strg+pos," ");
    pos = pos + bpos;
    strncpy(buffer,strg+pos,40);
    pos = pos + strbegin;
    buffer[41] = 0;
   }
 else
   {
    strs(buffer,strg,pos,(len > 255 ? 255 : len));
   }
 bpos = strspn(buffer," ");
 if ((buffer[bpos] == '0') && (toupper(buffer[bpos+1]) == 'X'))
   {
    bpos += 2;
    base = 16;
   }
 if (buffer[bpos] == '$')
   {
    bpos++;
    base = 16;
   }
 val = strtol(buffer+bpos,&ptr,base);
 if (len < 0)      /* len < 0: stop at first inconsistent char */
    strpos = pos + (int)(ptr - buffer);
 return val;
}

long strxl(char strg[], int pos, int len)
{
 char *ptr;
 long val;
 int bpos;
 if (len < 0)
   {
    pos = pos - strbegin;
    bpos = strspn(strg+pos," ");
    pos = pos + bpos;
    strncpy(buffer,strg+pos,40);
    pos = pos + strbegin;
    buffer[41] = 0;
   }
 else
   {
    strs(buffer,strg,pos,(len > 255 ? 255 : len));
   }
 bpos = strspn(buffer," ");
 if ((buffer[bpos] == '0') && (toupper(buffer[bpos+1]) == 'X'))
   {
    bpos += 2;
   }
 if (buffer[bpos] == '$')
   {
    bpos++;
   }
 val = strtol(buffer+bpos,&ptr,16);
 if (len < 0)      /* len < 0: stop at first inconsistent char */
    strpos = pos + (int)(ptr - buffer);
 return val;
}

double strd(char strg[], int pos, int len)
{
 char *ptr;
 double val;
 int bpos;
 int hex;
 long lval;
 if (len < 0)
   {
    pos = pos - strbegin;
    bpos = strspn(strg+pos," ");
    pos = pos + bpos;
    strncpy(buffer,strg+pos,40);
    pos = pos + strbegin;
    buffer[41] = 0;
   }
 else
   {
    strs(buffer,strg,pos,(len > 255 ? 255 : len));
   }
 bpos = strspn(buffer," ");
 hex = FALSE;
 if ((buffer[bpos] == '0') && (toupper(buffer[bpos+1]) == 'X'))
   {
    bpos += 2;
    hex = TRUE;
   }
 if (buffer[bpos] == '$')
   {
    hex = TRUE;
    bpos++;
   }
 if (hex)
   {
    val = strtol(buffer+bpos,&ptr,16);
   }
 else
   {
    val = strtod(buffer+bpos,&ptr);
   }
 if (len < 0)      /* len < 0: stop at first inconsistent char */
    strpos = pos + (int)(ptr - buffer);
 return val;
}

unsigned long strul(char strg[], int pos, int len)
{
 return (unsigned long)strl(strg,pos,len);
}

unsigned long strxul(char strg[], int pos, int len)
{
 return (unsigned long)strxl(strg,pos,len);
}

int stri(char strg[], int pos, int len)
{
 return (int)strl(strg,pos,len);
}

unsigned int strui(char strg[], int pos, int len)
{
 return (unsigned int)strl(strg,pos,len);
}

int strxi(char strg[], int pos, int len)
{
 return (int)strxl(strg,pos,len);
}

float strf(char strg[], int pos, int len)
{
 return (float)strd(strg,pos,len);
}

long dtofix(int dp, double temp)
{
 char tstrg[35];
 int i;
 int zlen;
 if (dp > 9)
    dp = 9;
 if (dp < 0)
    dp = 0;
 if (temp > 999999999)
    temp = 999999999;
 if (temp < -999999999)
    temp = -999999999;
 zlen = dp;
 if (zlen < 1)
    zlen = 1;
 sprintf(tstrg,strfmt((13+zlen),zlen,'F'),temp);
 for (i=12; i<27; i++)
   {
    tstrg[i] = tstrg[i+1];
   }
 return strl(tstrg,dp,12);
}

double fixtod(int dp, long xfix)
{
 int ibeg;
 int iend;
 double temp;
 int hold;
 int i;
 temp = xfix;
 if (temp > 999999999)
    temp = 999999999;
 if (temp < -999999999)
    temp = -999999999;
 sprintf(buffer,"%14.1lf",temp);
 if (dp > 9)
    dp = 9;
 if (dp < 0)
    dp = 0;
 buffer[12] = 0;
 ibeg = 12 - dp;
 iend = 13;
 for (i=iend; i>ibeg; i--)
   {
    buffer[i] = buffer[i-1];
   }
 buffer[i] = '.';
 hold = strpos;
 temp = strd(buffer,0,0);
 strpos = hold;
 return temp;
}

long strfix(char strg[], int pos, int len, int dp)
{
 double temp;
 char tstrg[35];
 int i;
 temp = strd(strg,pos,len);
 return dtofix(dp,temp);
}

static int xstr_____s(char strg[], int pos, int len, char xstrg[])
{
 int end;
 int i;
 pos = pos - strbegin;
 if (len < 0)
    len = -len;
 if (len == 0)
  {
    end = pos + strlen(xstrg);
    strpad2(strg,' ',end);
    i = 0;
    while (xstrg[i])
      {
       strg[pos] = xstrg[i];
       i++;
       pos++;
      }
   }
 else
   {
    end = pos + len;
    strpad2(strg,' ',end);
    for (i=0; i<len; i++)
      {
       if (xstrg[i])
         {
          strg[pos] = xstrg[i];
          pos++;
         }
       else
          break;
      }
   }
 return (strpos = end + strbegin);
}

int xstrs(char strg[], int pos, int len, int dp, char xstrg[])
{
 sprintf(buffer,strfmt(len,dp,'s'),xstrg);
 return xstr_____s(strg,pos,len,buffer);
}

int xstrc(char strg[], int pos, int len, int dp, char xchar)
{
 sprintf(buffer,strfmt(len,dp,'c'),xchar);
 return xstr_____s(strg,pos,len,buffer);
}

int xstrtoken(char strg[], int pos, int len, int dp, char *tokens[], int xkey)
{
 if ((xkey >= strcount(tokens)) || (xkey < 0))
   {
    buffer[0] = 0;
   }
 else
   {
    sprintf(buffer,strfmt(len,dp,'s'),tokens[xkey]);
   }
 return xstr_____s(strg,pos,len,buffer);
}

int xstrl(char strg[], int pos, int len, int dp, long xlong)
{
 sprintf(buffer,strfmt(len,dp,'D'),xlong);
 return xstr_____s(strg,pos,len,buffer);
}

int xstrul(char strg[], int pos, int len, int dp, unsigned long xlong)
{
 sprintf(buffer,strfmt(len,dp,'U'),xlong);
 return xstr_____s(strg,pos,len,buffer);
}

int xstrxl(char strg[], int pos, int len, int dp, long xlong)
{
 sprintf(buffer,strfmt(len,dp,'X'),xlong);
 return xstr_____s(strg,pos,len,buffer);
}

int xstrulx(char strg[], int pos, int len, int dp, unsigned long xlong)
{
 sprintf(buffer,strfmt(len,dp,'X'),xlong);
 return xstr_____s(strg,pos,len,buffer);
}

int xstrd(char strg[], int pos, int len, int dp, double xdouble)
{
 sprintf(buffer,strfmt(len,dp,'F'),xdouble);
 return xstr_____s(strg,pos,len,buffer);
}

int xstri(char strg[], int pos, int len, int dp, int xint)
{
 sprintf(buffer,strfmt(len,dp,'d'),xint);
 return xstr_____s(strg,pos,len,buffer);
}

int xstrui(char strg[], int pos, int len, int dp, unsigned int xint)
{
 sprintf(buffer,strfmt(len,dp,'u'),xint);
 return xstr_____s(strg,pos,len,buffer);
}

int xstrxi(char strg[], int pos, int len, int dp, int xint)
{
 sprintf(buffer,strfmt(len,dp,'x'),xint);
 return xstr_____s(strg,pos,len,buffer);
}

int xstrf(char strg[], int pos, int len, int dp, float xfloat)
{
 sprintf(buffer,strfmt(len,dp,'f'),xfloat);
 return xstr_____s(strg,pos,len,buffer);
}

int xstrfix(char strg[], int pos, int len, int dp, long xfix)
{
 int ibeg;
 int iend;
 double temp;
 int i;
 temp = xfix;
 if (temp > 999999999)
    temp = 999999999;
 if (temp < -999999999)
    temp = -999999999;
 sprintf(buffer,"%14.1lf",temp);
 if (dp > 9)
    dp = 9;
 if (dp < 0)
    dp = 0;
 buffer[12] = 0;
 ibeg = 12 - dp;
 iend = 13;
 for (i=iend; i>ibeg; i--)
   {
    buffer[i] = buffer[i-1];
    if (buffer[i] == ' ')
       buffer[i] = '0';
   }
 buffer[i] = '.';
 if (len != 0)
    ibeg = 13 - abs(len);
 if (ibeg < 0)
    ibeg = 0;
 if (len < 0)
   {
    while (buffer[ibeg] == ' ')
      {
       ibeg++;
      }
   }
 return xstr_____s(strg,pos,len,buffer+ibeg);
}

char *getl(char strg[], int n)
{
 int i;
 char *ptr;
 if (n > 253)
    n = 253;
 ptr = fgets(buffer,253,stdin);
 if (ptr != NULL)
   {
     for (i=0; i<n; i++)
      {
	   if ((buffer[i] == '\n') || (buffer[i] == 0))
		 {
		  strg[i] = 0;
		  break;
		 }
	   strg[i] = buffer[i];
	  }
    return strg;
   }
 else
   {
    strg[0] = 0;
    return NULL;
   }
}

char *getln(char strg[], int n)
{
 int i;
 char *ptr;

 if (n > 253)
    n = 253;
 ptr = fgets(buffer,253,stdin);

 printf("\n\r");
 if (ptr != NULL)
   {
    for (i=0; i<n; i++)
      {
	    if ((buffer[i] == '\n') || (buffer[i] == 0))
		  {
		   strg[i] = 0;
		   break;
		  }
	   strg[i] = buffer[i];
	  }
    return strg;
   }
 else
   {
    strg[0] = 0;
    return NULL;
   }
}

char *getbyte(char strg[], int n)
{
 int i;
 char *ptr;
 if (n > 253)
    n = 253;
 ptr = fgets(buffer,253,stdin);
 if (ptr != NULL)
   {
    for (i=0; i<n; i++)
      {
	   if ((buffer[i] == '\n') || (buffer[i] == 0))
		 {
		  strg[i] = 0;
		  break;
		 }
	   strg[i] = buffer[i];
	  }
    strg[i] = 0;
    return strg;
   }
 else
   {
    strg[0] = 0;
    return NULL;
   }
}

/* fgetl() modified 11/5/92 to allow unlimited string length (was 253 limit) */
char *fgetl(FILE *stream, char *strg, int limit)
  {
   char *ptr;
   int kar;
   int havecrlf;
   ptr = strg;
   havecrlf = FALSE;
   if (limit > 0)
     {
      for (; limit; limit--)
        {
         kar = fgetc(stream);
         if (kar == EOF)
           {
            ptr = NULL;
            break;
           }
         if (kar == '\n')
           {
            havecrlf = TRUE;
            break;
           }
         if (kar == '\r')
           {
            havecrlf = TRUE;
           }
         if (!havecrlf)
           {
            *strg = kar;
            strg++;
           }
        }
     }
   *strg = 0;
   while (!havecrlf)
     {
      kar = fgetc(stream);
      if (kar == EOF)
        {
         ptr = NULL;
         break;
        }
      if (kar == '\n')
        {
         break;
        }
     }
   return ptr;
  }

/* expands all tabs to blanks */
char *fgetl_notab(FILE *stream, char *strg, int limit)
  {
   int i;
   int tabrem;
   char *ptr;
   int kar;
   int havecrlf;
   ptr = strg;
   havecrlf = FALSE;
   tabrem = 0;
   if (limit > 0)
     {
      for (i=0; limit; limit--,i++)
        {
		 if (!tabrem)
		   {
            kar = fgetc(stream);
		   }
	     else
		   {
		    tabrem--;
		   }
		 if (kar == 0x09) /* TAB */
		   {
			kar = ' ';
	    	tabrem = 7 - +(i & 0x0007);
		   }
         if (kar == EOF)
           {
            ptr = NULL;
            break;
           }
         if (kar == '\n')
           {
            havecrlf = TRUE;
            break;
           }
         if (kar == '\r')
           {
            havecrlf = TRUE;
           }
         if (!havecrlf)
           {
            *strg = kar;
            strg++;
           }
        }
     }
   *strg = 0;
   while (!havecrlf)
     {
      kar = fgetc(stream);
      if (kar == EOF)
        {
         ptr = NULL;
         break;
        }
      if (kar == '\n')
        {
         break;
        }
     }
   return ptr;
  }

/* 10/5/92 replacement for fgetb which doesn't bomb on > 512 bytes */
int fgetb(FILE *stream, long offset, char *strg, int n)
  {
   int numin;
   int numout;
   int z;
   int x;
   if (offset >= 0L)
     {
      if (fseek(stream,offset,0))
        {
         return 0;
        }
     }
   numout = 0;
   for (numin=n; numin > 0; numin -= 256)
     {
      z = numin;
      if (numin >= 256)
        {
         z = 256;
        }
      x = fread(strg,1,z,stream);
      numout += x;
      if (x != z)
        {
         return numout;
        }
      strg += z;
     }
   return numout;
  }


void put(char strg[])
{
 printf("%s",strg);
}

void putl(char strg[])
{
 put(strg);
 put("\r\n");
}

void putb(char strg[], int n)
{
 int i;
 for (i=0; i < n; i++)
   {
    putchar(strg[i]);
   }
}

void fput(FILE *stream, char strg[])
{
 fputs(strg,stream);
}

void fputl(FILE *stream, char strg[])
{
 fput(stream,strg);
 fput(stream,"\n");
}

/* 10/5/92 replacement for fputb which doesn't bomb on > 512 bytes */
int fputb(FILE *stream, long offset, char *strg, int n)
  {
   int numin;
   int numout;
   int z;
   int x;
   if (offset >= 0L)
     {
      if (fseek(stream,offset,0))
        {
         return 0;
        }
     }
   numout = 0;
   for (numin=n; numin > 0; numin -= 256)
     {
      z = numin;
      if (numin >= 256)
        {
         z = 256;
        }
      x = fwrite(strg,1,z,stream);
      numout += x;
      if (x != z)
        {
         return numout;
        }
      strg += z;
     }
   return numout;
  }

long fend(FILE *stream)
{
 if (fseek(stream,0L,2))
   {
    return -1L;
   }
 return ftell(stream);
}

void dumpb(unsigned char *strg, int size, long ref)
{
 int i;
 int j;
 unsigned char chr;
 unsigned char chlist[17];
 put("\n");
 j = 0;
 for (i=0; i < size; i++)
   {
    if (j == 0)
      {
       printf("%08lX  ",ref);
      }
    chr = *strg++;
    printf(" %02X",chr);
    if ((chr < ' ') || (chr > 0x7E))
      {
       chr = '.';
      }
    chlist[j] = chr;
    j++;
    chlist[j] = 0;
    if (j > 15)
      {
       j = 0;
       ref += 16;
      }
    if (j == 0)
      {
       printf("   %s\n",chlist);
      }
   }
 if (j > 0)
   {
    for (i=j; i<16; i++)
      {
       put("   ");
      }
    printf("   %s\n",chlist);
   }
 put("\n");
}

int batchlevel = 0;
static char procbuffer[256];
static FILE *procfile[11];
static int level = 0;
static char *batch_____proc(char strg[], char *parser())
{
 char *ptr;
 while (level > 0)
   {
    if (fgetl(procfile[level],procbuffer,255) != NULL)
       break;
    else
      {
       fclose(procfile[level]);
       level--;
       batchlevel = level;
       if (level == 0)
          return "";
      }
   }
 if (level == 0)
   {
    strncpy(procbuffer,strg,255);
    procbuffer[255] = 0;
   }
 batchlevel = level;
 ptr = parser(procbuffer);
 if (*ptr != 0)
   {
    if (*ptr == '?')    /* ? in first column indicates quit request */
      {
       while (level > 0)
         {
          fclose(procfile[level]);
          level--;
         }
       level = -1;
       batchlevel = level;
       return ptr;
      }
    /* else is assumed to be #INCLUDE filename */
    level++;
    if (level > 10)
       level = 10;
    else
      {
       procfile[level] = fopen(ptr,"r");
       if (procfile[level] == NULL)
          level--;
      }
   }
 batchlevel = level;
 return ptr;
}

char *batchproclist(char *strglist[], char *parser())
{
 char *ptr;
 int curstrg = 0;
 level = 0;
 batchlevel = level;
 while (TRUE)
   {
    if (level == 0)
      {
       if (strglist[curstrg] != NULL)
         {
          strncpy(procbuffer,strglist[curstrg],255);
          procbuffer[255] = 0;
          curstrg++;
         }
       else
         {
          procbuffer[0] = 0;
          break;
         }
      }
    ptr = batch_____proc(procbuffer,parser);
    if (level < 0)
       return ptr;
   }
 while (level > 0)
   {
    fclose(procfile[level]);
    level--;
   }
 level = -1;
 batchlevel = level;
 procbuffer[0] = 0;
 return parser(procbuffer);
}

char *batchprocfile(char name[], char *parser())
{
 FILE *unit;
 char *ptr;
 level = -1;
 batchlevel = level;
 unit = fopen(name,"r");
 if (unit == NULL)
    return "? Unable to open input file";
 level = 0;
 batchlevel = level;
 while (TRUE)
   {
    if (level == 0)
      {
       ptr = fgetl(unit,procbuffer,255);
       if (ptr == NULL)
          break;
      }
    ptr = batch_____proc(procbuffer,parser);
    if (level < 0)
       return ptr;
   }
 while (level > 0)
   {
    fclose(procfile[level]);
    level--;
   }
 fclose(unit);
 level = -1;
 batchlevel = level;
 procbuffer[0] = 0;
 return parser(procbuffer);
}



