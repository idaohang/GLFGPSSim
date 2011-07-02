/* sidtime.h --  Real-time sidereal clock functions -- looks up or displays UT
                 and calculates sidereal time
                 written as sid_clk by Gary Flispart   11/08/2005
                 separated to library functions        10/17/2007

                 Original functions written by GLF for Commodore 64 from
                 information in an article in Sky & Telescope magazine,
                 later translated to IBM PC-compatible as SIDCLOCK.BAS,
                 converted core functions to C on 11/08/2005

                 06/21/2007 -- add feature: show "clock" position of
                               Polaris in Meade polar axis finder
*/

/* compile using Turbo C 2.0 and HUGE memory model or Dev-C++ */

#ifndef SIDTIME_H__
#define SIDTIME_H__

#include <string.h>
#include <math.h>

/* NOTE:  ticks are defined as 1/60 second increments,
          such that 1 day = 5184000 ticks
*/
#define TICKSPERDAY 5184000L

#define SM_TWO_PI   6.283185307179586476925286766559
#define SM_ONE_PI   3.1415926535897932384626433832795
#define SM_HALF_PI  1.5707963267948966192313216916398
#define SM_VERY_SMALL_NUM  0.0000000000000000001
   
double sm_normalize_angle(double angle);
double sm_safe_sqrt(double v);
void sm_rotate(double *x, double *y, double theta);
double sm_find_angle(double x, double y);
void sm_calc_precess(double *newra, double *newdec, double ra, double dec, 
                             int epochyr, int curyr, int curmo, int curday);


long tick_clock(void);  /* returns current tick (or jiffy) count (24 hr loop) */
void ms_delay(long ms);      /* delays number of milliseconds -- initially 
                                reasonable, but improve with calibrate_delay() */
void calibrate_delay(void);  /* 5-second test of actual time for delay, then 
                                adjust loop count to compensate */  


extern int modaytab[];

int is_ly(int yr);
double jd_sid(int yy, int mm, int dd);
long fracday_to_ticks(double dayfraction);
double ticks_to_fracday(long ticks);
void fracday_to_hms(double dayfraction, int *hd, int *md, int *sd);
double hms_to_frac(int lh, int lm, int ls);
double dms_to_frac(int ld, int lm, int ls);
long gmt_secs(void);
long arbitrary_secs(int hi, int mi, int si);
double sidereal_fracday(int yr, int mo, int da, long ticks, double longfrac);

typedef struct
  {
   int utyr;
   int utmo;
   int utda;
   int uthr;
   int utmin;
   int utsec;
   long utticks;
   int sidhr;
   int sidmin;
   int sidsec;
   long sidticks;
  }
   sidclock;

void ut_sid_time(sidclock *sc, int ld, int lm, int ls);
void sid_time(sidclock *sc, int ld, int lm, int ls,
                            int yr, int mo, int da,
                            int hh, int mm, int ss);

void s_ymd(char strg[], int yy, int mm, int dd);   /* CAUTION! Make sure strg[] is big enough! */
void s_hms(char strg[], int hd, int md, int sd);   /* CAUTION! Make sure strg[] is big enough! */
void s_jultime(char strg[], double juliantime);    /* CAUTION! Make sure strg[] is big enough! */

extern double polaris_ra;
extern double polaris_dec;

double polaris_finder(sidclock sc);    /* clockface position of polaris in polar finder 
                                          at given siderweal time */

#endif
