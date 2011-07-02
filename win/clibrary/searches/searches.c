/* searches.c -- support for building, insertion sorting, and searching
                 indirect string lists (char *strglist[])                 */

#include "gflib.h"
#include "obsolete.h"

#include "searches.h"


int build_list_space(char *list[], int maxlist, int listwidth, int *nlist)

       /* returns number of items successfully allocated */

  {
   int i;

   *nlist = 0;
   for (i=0; i<maxlist; i++)
     {
      if (NULL != (list[i] = malloc(listwidth)))
        {
         *(list[i]) = 0;
        }
      else
        {
         break;
        }
     }
   return i;
  }


void clear_list(char *list[], int maxlist, int *nlist)

  {
   int i;

   *nlist = 0;
   for (i=0; i<maxlist; i++)
     {
      *(list[i]) = 0;
     }
  }



int insert_item(char *list[], int maxlist, int *n,
               char *strg, int position)

  {
   int j;
   int nlist;

   nlist = *n;
   if (nlist >= maxlist)
     {
     /* only appends or inserts if list isn't full */
      return 0;
     }

   if ((position < 0) || (position >= nlist))  /* if append... */
     {
      strcpy(list[nlist],strg);
      nlist++;
      *n = nlist;
      return 1;  /* successful append */
     }

   /* else make room for insertion here, then insert */
   for (j=nlist; j>position; j--)
     {
      strcpy(list[j],list[j-1]);
     }
   strcpy(list[position],strg);
   nlist++;
   *n = nlist;
   return 1;   /* TRUE if insertion was done */
  }


int replace_item(char *list[], int maxlist, int nlist,
               char *strg, int position)

  {
   if ((nlist > maxlist) || (position < 0) || (position > nlist))
     {
     /* only replaces if position within range and list properly exists */
      return 0;
     }

   strcpy(list[position],strg);
   return 1;   /* TRUE if replacement was done */
  }



int insert_sort_item(char *list[], int maxlist, int *n,
               int(*comparefunction)(char *a, char *b),
               char *strg, search_type code)

  {
   int i;
   int j;
   int nlist;
   int comp;

   nlist = *n;
   if (nlist < maxlist)  /* only inserts if list isn't full */
     {
      for (i=0; i<=nlist; i++)
        {
         if (i == nlist)
           {
            strcpy(list[nlist],strg);
            nlist++;
            break;
           }
         else
           {
            comp = comparefunction(strg,list[i]);
            if (comp < 0)
              {
               /* make room for insertion here, then insert */
               for (j=nlist; j>i; j--)
                 {
                  strcpy(list[j],list[j-1]);
                 }
               strcpy(list[i],strg);
               nlist++;
               break;
              }
            else
              {
               if (comp == 0)
                 {
                  /* handle uniqueness specification cases */
                  switch (code)
                    {
                     case SEE_FIRST:
                       {
                        return 1;  /* pretend a real insertion was made */
                       }
                     case SEE_LAST:
                       {
                        strcpy(list[i],strg);   /* replace with last version */
                        return 1;
                       }
                     default:  /* SEE_ANY: items not forced to be unique */
                       {
                       }
                    }
                 }
              }
           }
        }
      *n = nlist;
      return 1;   /* TRUE if insertion was done */
     }
   else
     {
      return 0;   /* FALSE if no insertion was done */
     }
  }


int binary_search_item(char *list[], int nlist,
               int(*comparefunction)(char *a, char *b), char *strg)

  {
   int ntry;
   int low;
   int high;
   int comp;

   low = 0;
   high = nlist;
   ntry = (nlist / 2) + 1;

   /* if strg is found in list, return index of list item found */

   while ((ntry > low) && (ntry <= high))
     {
      comp = comparefunction(list[ntry-1],strg);
      if (comp == 0)
        {
         return (ntry-1);
        }
      if (comp < 0)     /* look higher in list */
        {
         low = ntry;
         ntry = ((high + low) / 2) + 1;
        }
      else              /* look lower in list */
        {
         high = ntry - 1;
         ntry = ((high + low) / 2) + 1;
        }
     }

   return -1;   /* if nlist was 0 or strg not found */
  }


int seq_search_item(char *list[], int nlist,
                      int(*comparefunction)(char *a, char *b),
                      char *strg, search_type code, int lastfound)

  {
   int ntry;
   int begin;

   if (nlist < 1)
     {
      return -1;
     }

   begin = -1;
   if ((code == SEE_LAST) || (code == SEE_LAST_OR_LESS))
     {
      begin = nlist;
     }

   if ((lastfound >= 0) && (lastfound < nlist))
     {
      begin = lastfound;
     }

   /* if strg is found in list, index of first matching list item found */

   switch (code)
     {
      case SEE_LAST:
        {
         for (ntry=begin-1; ntry>=0; ntry--)
           {
            if (comparefunction(list[ntry],strg) == 0)
              {
               return ntry;
              }
           }
         break;
        }
      case SEE_FIRST_OR_MORE:
        {
         for (ntry=begin+1; ntry<nlist; ntry++)
           {
            if (comparefunction(list[ntry],strg) >= 0)
              {
               return ntry;
              }
           }
         break;
        }
      case SEE_LAST_OR_LESS:
        {
         for (ntry=begin-1; ntry>=0; ntry--)
           {
            if (comparefunction(list[ntry],strg) <= 0)
              {
               return ntry;
              }
           }
        }
      default:  /* SEE_FIRST assumed */
        {
         for (ntry=begin+1; ntry<nlist; ntry++)
           {
            if (comparefunction(list[ntry],strg) == 0)
              {
               return ntry;
              }
           }
        }
     }

   return -1;   /* if nlist was 0 or strg not found */
  }



double linear_interpolate(double x0, double y0, double x1, double y1, double x)

  {
   double diffx;
   double diffy;
   double posx;

   diffx = x1 - x0;
   diffy = y1 - y0;
   posx = x - x0;

   if ((diffx < 0.0000001) && (diffx > -0.0000001))
     {
      /* avoid division by zero */
      if (posx >= 0.0000001)
        {
         return y1;
        }
      return y0;
     }

   return +(y0 + (diffy * (posx / diffx)));
  }

