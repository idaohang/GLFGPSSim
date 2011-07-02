/* extract.c -- support for data extraction/conversion */

#include "gflib.h"
#include "obsolete.h"

#include "extract.h"


void clear_data(char data[], int rows)
  {
   memset(data,' ',(rows*80));
  }

void extract_real(char data[], int row, int pos, int len, double *value)
  {
   *value = strd(data,((row-1)*80 + pos - 1),len);
  }

void extract_int(char data[], int row, int pos, int len, int *value)
  {
   *value = stri(data,((row-1)*80 + pos - 1),len);
  }

void extract_strg(char data[], int row, int pos, int len, char *value)
  {
   strs(value,data,((row-1)*80 + pos - 1),len);
  }

void cpydata(char data[], char strg[], int size)
  {
   while ((size > 0) && *strg)
     {
      *data++ = *strg++;
      size--;
     }
  }

void fullstring(char *strg, int size)
  {
   strpad(strg,' ',size);
  }
