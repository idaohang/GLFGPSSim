/* calensub.h */

#ifndef CALENSUB_H__
#define CALENSUB_H__

#include <string.h>
#include <math.h>
#include <time.h>

const char *month_name(int mo);
int leapyr(int iyr);
void jdvert(int jdat, int jyr, int *imo, int *ida, int *newyr);
int jdaymo(int kyr, int imo);
void intdat(int *iyr, int *imo, int *ida);
void datlin(char kstr[], int iyr, int imo, int ida, int ihr, int imin);
int juldat(int iyr, int imo, int ida);
long ldifmo(int ayr, int amo, int ada, int byr, int bmo, int bda);
long ldifda(int ayr, int amo, int ada, int byr, int bmo, int bda);
int dayofw(int yr, int mo, int day);
int hivday(int yr, int mo, int day);
int inweek(int yr, int mo, int day, int wyr, int wmo, int wday);

/* added 3/18/2005 to allow setting UNIX time values */
time_t unixtime(int yr, int mo, int day, int hr, int min, int sec);

#endif
