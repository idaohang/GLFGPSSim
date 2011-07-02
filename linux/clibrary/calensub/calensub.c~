/* calensub.c -- GLF historical time/date routines (plus new unix stuff) */

#include <time.h>
#include "gflib.h"
#include "obsolete.h"

#include "calensub.h"

static char *motab[13]    = {"","JANUARY  ","FEBRUARY ","MARCH    ",
                             "APRIL    ","MAY      ","JUNE     ",
                             "JULY     ","AUGUST   ","SEPTEMBER",
                             "OCTOBER  ","NOVEMBER ","DECEMBER "};

static int jultab[13] = {0,0,31,59,90,120,151,181,212,243,273,304,334};

static char cwork[140];

const char *month_name(int mo)
  {
   if ((mo < 1) || (mo > 12))                
      return motab[0];
   return motab[mo];               
  }


/* Fixed leap year function (7/9/1996): */
int leapyr(int iyr)

{
 if ((iyr < 0) || (iyr > 9999)) return FALSE;
 if ((iyr >= 00) && (iyr <= 99)) iyr = iyr + 1900;
 if (iyr % 100 == 0)
   {
    if (iyr % 400 == 0)
       return TRUE;
    return FALSE;
   }
 else
   {
    if (iyr % 4 == 0)
       return TRUE;
   }
 return FALSE;
}


/*============================================*/

void jdvert(int jdat, int jyr, int *imo, int *ida, int *newyr)

{
 int kdat;
 int n;
 int ldat;


 *newyr = jyr;
 if ((*newyr < 0) || (*newyr > 9999)) goto L_9000;
 if ((*newyr >= 00) && (*newyr <= 99)) *newyr = *newyr + 1900;
 kdat = jdat;

L_10:
 if (kdat > 0) goto L_20;
 *newyr = *newyr - 1;
 n = 365;
 if (leapyr(*newyr)) n = 366;
 kdat = kdat + n;
 goto L_10;

L_20:
 n = 365;
 if (leapyr(*newyr)) n = 366;
 if (kdat <= n) goto L_100;
 kdat = kdat - n;
 goto L_20;

L_100:
 ldat = kdat;
 if (kdat > 31) goto L_200;
 *imo = 1;
 *ida = kdat;
 goto L_9000;

L_200:
 if (ldat > 59) goto L_290;
 *imo = 2;
 *ida = kdat - 31;
 goto L_9000;

L_290:
 if (n == 366) ldat = ldat - 1;
 if (ldat > 59) goto L_300;
 *imo = 2;
 *ida = 29;
 goto L_9000;

L_300:
 if (ldat > 90) goto L_400;
 *imo = 3;
 *ida = ldat - 59;
 goto L_9000;

L_400:
 if (ldat > 120) goto L_500;
 *imo = 4;
 *ida = ldat - 90;
 goto L_9000;

L_500:
 if (ldat > 151) goto L_600;
 *imo = 5;
 *ida = ldat - 120;
 goto L_9000;

L_600:
 if (ldat > 181) goto L_700;
 *imo = 6;
 *ida = ldat - 151;
 goto L_9000;

L_700:
 if (ldat > 212) goto L_800;
 *imo = 7;
 *ida = ldat - 181;
 goto L_9000;

L_800:
 if (ldat > 243) goto L_900;
 *imo = 8;
 *ida = ldat - 212;
 goto L_9000;

L_900:
 if (ldat > 273) goto L_1000;
 *imo = 9;
 *ida = ldat - 243;
 goto L_9000;

L_1000:
 if (ldat > 304) goto L_1100;
 *imo = 10;
 *ida = ldat - 273;
 goto L_9000;

L_1100:
 if (ldat > 334) goto L_1200;
 *imo = 11;
 *ida = ldat - 304;
 goto L_9000;

L_1200:
 *imo = 12;
 *ida = ldat - 334;

L_9000:
 ;
}


/*============================================*/

int jdaymo(int kyr, int imo)


{
 int n;


 if ((imo < 1) || (imo > 12)) return 0;
 if ((kyr < 0) || (kyr > 9999)) return 0;
 if ((kyr >= 00) && (kyr <= 99)) kyr = kyr + 1900;
 n = 0;
 if (leapyr(kyr)) n = 1;
 switch (imo)
   {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
       return 31;
    case 2:
       return 28 + n;
    case 4:
    case 6:
    case 9:
    case 11:
       return 30;
    default:
       return 0;
   }
}

/*============================================*/

void intdat(int *iyr, int *imo, int *ida)

{
 long tnum;
 char strg[30];
 char mostrg[4];

 *iyr = 0;
 *imo = 0;
 *ida = 0;
 time(&tnum);
 strcpy(strg,ctime(&tnum));
 strlwr(strs(mostrg,strg,4,3));
 *ida = stri(strg,8,2);
 *iyr = stri(strg,20,4);
 if (strcmp(mostrg,"jan") == 0)
    *imo = 1;
 if (strcmp(mostrg,"feb") == 0)
    *imo = 2;
 if (strcmp(mostrg,"mar") == 0)
    *imo = 3;
 if (strcmp(mostrg,"apr") == 0)
    *imo = 4;
 if (strcmp(mostrg,"may") == 0)
    *imo = 5;
 if (strcmp(mostrg,"jun") == 0)
    *imo = 6;
 if (strcmp(mostrg,"jul") == 0)
    *imo = 7;
 if (strcmp(mostrg,"aug") == 0)
    *imo = 8;
 if (strcmp(mostrg,"sep") == 0)
    *imo = 9;
 if (strcmp(mostrg,"oct") == 0)
    *imo = 10;
 if (strcmp(mostrg,"nov") == 0)
    *imo = 11;
 if (strcmp(mostrg,"dec") == 0)
    *imo = 12;
}

/*============================================*/

void datlin(char kstr[], int iyr, int imo, int ida, int ihr, int imin)


{
 strcpy(kstr,"(ILLEGAL DATE)");
 if ((iyr < 0) || (iyr > 9999)) return;
 if ((iyr >= 00) && (iyr <= 99)) iyr = iyr + 1900;
 if ((imo < 1) || (imo > 12)) return;
 if ((ida < 1) || (ida > 31)) return;
 if ((ihr < 0) || (ihr > 23)) return;
 if ((imin < 0) || (imin > 59)) return;
 sprintf(kstr,"%s %2d, %4d (%02d:%02d)",motab[imo],ida,iyr,ihr,imin);
}

/*============================================*/

int juldat(iyr,imo,ida)

{
 int n;


 if ((imo < 1) || (imo > 12)) return 0;
 if ((ida < 1) || (ida > 31)) return 0;
 if ((iyr < 0) || (iyr > 9999)) return 0;
 if ((iyr >= 00) && (iyr <= 99)) iyr = iyr + 1900;
 n = 0;
 if (leapyr(iyr))
    n = 1;
 if (imo <= 2) n = 0;
 return (jultab[imo] + n + ida);
}

/*============================================*/

long ldifmo(int ayr, int amo, int ada, int byr, int bmo, int bda)


{
 long jdiff;
 long moyra,moyrb;
 int layr;
 int lbyr;
 int idaya;
 int idayb;
 int idfact;
 int r_ldifmo;


 layr = ayr;
 lbyr = byr;
 if ((layr >= 00) && (layr <= 99)) layr = 1900 + layr;
 if ((lbyr >= 00) && (lbyr <= 99)) lbyr = 1900 + lbyr;
 jdiff = 0;
 moyra = layr * 12 + amo;
 moyrb = lbyr * 12 + bmo;
 if (moyra < moyrb) goto L_3000;
 if (moyra > moyrb) goto L_2000;
 goto L_9000;

L_2000:
 idaya = ada;
 idayb = bda;
 if (idaya >= jdaymo(ayr,amo)) idaya = 31;
 idfact = 0;
 if (idayb > idaya) idfact =  - 1;
 jdiff = moyra - moyrb + idfact;
 goto L_9000;

L_3000:
 idaya = ada;
 idayb = bda;
 if (idayb >= jdaymo(byr,bmo)) idayb = 31;
 idfact = 0;
 if (idaya > idayb) idfact =  - 1;
 jdiff = moyrb - moyra + idfact;
 jdiff =  - jdiff;
 goto L_9000;

L_9000:
 r_ldifmo = jdiff;
 return r_ldifmo;
}

/*============================================*/

long ldifda(int ayr, int amo, int ada, int byr, int bmo, int bda)


{
 long jdiff;
 long isum;
 int layr;
 int lbyr;
 int jdays;
 int noinyr;
 int leftb;
 int ina;
 int ibegyr;
 int iendyr;
 int i;
 int lefta;
 int inb;
 int r_ldifda;


 layr = ayr;
 lbyr = byr;
 if ((layr >= 00) && (layr <= 99)) layr = 1900 + layr;
 if ((lbyr >= 00) && (lbyr <= 99)) lbyr = 1900 + lbyr;
 jdiff = 0;
 if (layr == lbyr) goto L_5000;
 if (layr < lbyr) goto L_3000;
 goto L_2000;

L_2000:
 jdays = juldat(lbyr,bmo,bda);
 noinyr = 365;
 if (leapyr(lbyr)) noinyr = 366;
 leftb = noinyr - jdays;
 ina = juldat(layr,amo,ada);
 ibegyr = lbyr + 1;
 iendyr = layr - 1;
 isum = 0;
 if (ibegyr > iendyr) goto L_2200;
 for (i = ibegyr; i <= iendyr; i++)
   {
    noinyr = 365;
    if (leapyr(i)) noinyr = 366;
    isum = isum + noinyr;
   L_2100:
    ;
   }

L_2200:
 jdiff = leftb + isum + ina;
 goto L_9000;

L_3000:
 jdays = juldat(layr,amo,ada);
 noinyr = 365;
 if (leapyr(layr)) noinyr = 366;
 lefta = noinyr - jdays;
 inb = juldat(lbyr,bmo,bda);
 ibegyr = layr + 1;
 iendyr = lbyr - 1;
 isum = 0;
 if (ibegyr > iendyr) goto L_3200;
 for (i = ibegyr; i <= iendyr; i++)
   {
    noinyr = 365;
    if (leapyr(i)) noinyr = 366;
    isum = isum + noinyr;
   L_3100:
    ;
   }

L_3200:
 jdiff = lefta + isum + inb;
 jdiff =  - jdiff;
 goto L_9000;

L_5000:
 jdiff = juldat(layr,amo,ada) - juldat(lbyr,bmo,bda);
 goto L_9000;

L_9000:
 r_ldifda = jdiff;
 return r_ldifda;
}

/*============================================*/

int dayofw(int yr, int mo, int day)


{
 long ldiff;
 long long7 = {7};
 int idayw;


 ldiff = ldifda(yr,mo,day,84,1,1);
 idayw = ldiff % long7;
 if (idayw < 0) idayw = idayw + 7;
 return idayw;
}

/*============================================*/

int hivday(int yr, int mo, int day)


{
 long ldiff;
 long long6 = {6};
 int idayw;


 ldiff = ldifda(yr,mo,day,84,1,1);
 idayw = ldiff % long6;
 if (idayw < 0) idayw = idayw + 6;
 return idayw;
}


/*============================================*/

int inweek(int yr, int mo, int day, int wyr, int wmo, int wday)


{
 long ldiff;
 int syr,smo,sday;
 int iyr;
 int imo;
 int iday;
 int jyr;
 int jmo;
 int jday;
 int iadj;
 int i;

 int r_inweek;


 iyr = yr;
 imo = mo;
 iday = day;
 if ((imo < 1) || (imo > 12)) goto L_8000;
 if ((iyr < 0) || (iyr > 9999)) goto L_8000;
 if ((iyr >= 00) && (iyr <= 99)) iyr = iyr + 1900;
 if ((iday < 1) || (iday > jdaymo(iyr,imo))) goto L_8000;
 jyr = wyr;
 jmo = wmo;
 jday = wday;
 if ((jmo < 1) || (jmo > 12)) goto L_8000;
 if ((jyr < 0) || (jyr > 9999)) goto L_8000;
 if ((jyr >= 00) && (jyr <= 99)) jyr = jyr + 1900;
 if ((jday < 1) || (jday > jdaymo(jyr,jmo))) goto L_8000;
 iadj = dayofw(jyr,jmo,jday);
 syr = jyr;
 smo = jmo;
 sday = jday;
 i = iadj;

L_1300:
 if (i < 1) goto L_2000;
 sday = sday - 1;
 if (sday >= 1) goto L_1900;
 smo = smo - 1;
 if (smo >= 1) goto L_1700;
 syr = syr - 1;
 smo = 12;

L_1700:
 sday = jdaymo(syr,smo);

L_1900:
 i = i - 1;
 goto L_1300;

L_2000:
 ldiff = ldifda(iyr,imo,iday,syr,smo,sday);
 if ((ldiff < 0) || (ldiff > 6)) goto L_8000;
 r_inweek = ldiff;
 goto L_9000;

L_8000:
 r_inweek =  - 1;
 goto L_9000;

L_9000:
 return r_inweek;
}


/*============================================*/

/* added 3/18/2005 to support UNIX time setting */
/* FIXED! 01/21/2009 GLF -- error in 16-bit vs. long math (TC only) caused 
   hours to calculate wrong # of seconds */
   
time_t unixtime(int yr, int mo, int day, int hr, int min, int sec)
  {
   long tval = 0;
   
   tval = ldifda(yr,mo,day,1970,1,1) * 86400L;
   tval += ((long)hr)*3600L;  /* MUST keep this long, else TC wraps if > 9 hrs */
   tval += min*60;
   tval += sec;
   
   return (time_t)tval;
  }

