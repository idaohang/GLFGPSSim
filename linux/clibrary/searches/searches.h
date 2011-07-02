/* searches.h -- support for building, insertion sorting, and searching
                 indirect string lists (char *strglist[])                 */

#ifndef SEARCHES_H__
#define SEARCHES_H__

#include <string.h>

typedef enum
  {
   SEE_ANY,
   SEE_FIRST,
   SEE_LAST,
   SEE_FIRST_OR_MORE,
   SEE_LAST_OR_LESS
  }
   search_type;


int build_list_space(char *list[], int maxlist, int listwidth, int *nlist);

       /* returns number of items successfully allocated */

void clear_list(char *list[], int maxlist, int *nlist);

int insert_item(char *list[], int maxlist, int *n,
               char *strg, int position);

int replace_item(char *list[], int maxlist, int nlist,
               char *strg, int position);

int insert_sort_item(char *list[], int maxlist, int *n,
               int(*comparefunction)(char *a, char *b),
               char *strg, search_type code);

int binary_search_item(char *list[], int nlist,
               int(*comparefunction)(char *a, char *b), char *strg);

int seq_search_item(char *list[], int nlist,
                      int(*comparefunction)(char *a, char *b),
                      char *strg, search_type code, int lastfound);

double linear_interpolate(double x0, double y0, double x1, double y1, double x);

#endif
