/* extract.h -- support for data extraction/conversion */

#ifndef EXTRACT_H__
#define EXTRACT_H__

#include <string.h>

void clear_data(char data[], int rows);
void extract_real(char data[], int row, int pos, int len, double *value);
void extract_int(char data[], int row, int pos, int len, int *value);
void extract_strg(char data[], int row, int pos, int len, char *value);
void cpydata(char data[], char strg[], int size);
void fullstring(char *strg, int size);

#endif
