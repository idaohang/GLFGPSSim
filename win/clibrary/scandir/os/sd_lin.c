/* sd_lin.c -- Linux-specific module for:

   scandir.c -- framework for directory scanning programs
                works within current directory OR within parameter 1 path 
*/

/* DO NOT use this module directly -- allow scandir.c to call it using 
   conditional compilation
*/   

/* this module performs all directory scanning by examining ALL files in the
   tree as specified, and marks the files found for inclusion or exclusion
   in an externally driven search -- wildcard matching is performed by this 
   mmodule, not by the OS -- if a file name does not match the scan wildcards
   or specs, it's filtering data block is marked with a '!' or ':' character 
   so it may be ignored -- unless directory matching is specified, dir names are 
   marked with '!' so that only filenames are passed back for file processing 
   (The proper technique for file matching is to accept data if marked ' ').

   NOTE:  the commentary in scandir.c has been limited to keep file size below 
   64K for Turbo C. 
*/

#if !defined(__TURBOC__) && !defined(__MINGW32__)   /* failsafe for #include */
/* assume Linux */

#include "../scandir.h"
#include "calensub.h"   /* needed for Turbo C unixtime conversion only */
#include "obsolete.h"

/* assume Linux-style dir scanning, Linux or Windows MinGW32 */

#include <unistd.h>

/* hide OS dependencies for RMDIR */
int os_rmdir(const char *path)
  {
   return rmdir(path); 
  }




/* hide OS dependencies for CHDIR */
int os_chdir(const char *path)
  {
   /* NOTE: the Windows chdir() allows a slash at end of pathname -- it is 
            convenient to keep a trailing slash it for other reasons, so 
            count on that fact here. Also, under Windows, if a drive letter
            is specified in the pathname, the drive is also changed (different 
            than in MS-DOS/TC) so count on that behavior, too. */                
            
   return chdir(path);
  }

/* hide OS dependencies for GETCWD */
char *os_getcwd(char *buf, int buflen)
  {
   return getcwd(buf,buflen);                
  }



/* hide OS dependencies for file length of open file */
long os_filelength(FILE *hfile)   /* find the length of an open file stream */
  {
   long lcurpos, lendpos;

   /* Check for valid file pointer */
   if (hfile == NULL)
     {
      return -1;
     }
 
   #if defined(__TURBOC__) || defined(__MINGW32__)
      return filelength(fileno(hfile));
   #else
      /* assume Linux glibc -- no filelength() function */
      /* Store current position */
      lcurpos = ftell(hfile);
      /* Move to the end and get position */
      fseek(hfile, 0, SEEK_END);
      lendpos = ftell(hfile);
      /* Restore the file pointer */
      fseek(hfile, lcurpos, SEEK_SET);
      return lendpos;
   #endif 
  }



/* assume OS-Neutral dir scanning, DOS, Linux or Windows MinGW32 */
/* --------------------------------------------------------------------- */

/* global data assumed not to change during any full dir scan cycle */
/* initialized during call to ez_dir_scan() and maintained until return throughout
   many calls to user defined functions -- if either original dest and source set to
   empty string (""), it will be replaced with current working dir (CWD) */

/* assumed to be compatible with output of cwd() function */

#define MAXPATHNAMESIZE 999 

char source_origpath[MAXPATHNAMESIZE+1] = "";
char dest_origpath[MAXPATHNAMESIZE+1] = "";

char source_fullname[MAXPATHNAMESIZE+1] = "";
char dest_fullname[MAXPATHNAMESIZE+1] = "";

char sdir_fullname[MAXPATHNAMESIZE+1] = "";
char ddir_fullname[MAXPATHNAMESIZE+1] = "";

/* maintain knowedge of user intentions for proactive creation of destination
   subdirs, necessary if the scan is used to copy a tree */
   
static int create_subtree_on_scan = FALSE;

/* ------------------------------------------------------------------------ */
/* support functions for building fully qualified filenames from context */

static void strlimcpy(char dest[], const char src[], int lim)
  {
   if (lim > MAXPATHNAMESIZE)
      lim = MAXPATHNAMESIZE;
          
   dest[0] = 0;
   
   if (lim >= 0)
     {
      strncpy(dest,src,lim);
      dest[lim] = 0;
     }
  }

/* converts relative pathnames to fully qualified pathnames as needed */
int copy_resolved_pathname(char dest[], const char name[], int lim)
   {
    int i,j;
    
    /* sample valid name styles:   C:\prg\dos
                                   \prg
                                   .
                                   ..
                                   ./prg
                                   ..\prg
   
                            slashes may be / or \ 
    */                              
   
    dest[0] = 0;     /* empty string if not clean */
   
    /* verify name format isn't hopeless... */
    if (!clean_os_pathname(name,FALSE))
      {
       return FALSE;                                
      }

    switch (name[0])
      {
       case 0:
         {
          /* empty string -- assume specifies CWD itself (scandir.h rules) */  
          os_getcwd(dest,lim);   
          return TRUE;
          break;  
         }    
       case '.':
         {
          /* assume relative to CWD     Ex:   ../src  or ./src */  
          os_getcwd(dest,lim);
          remove_final_slash(dest);
   
          if (name[1] == '.')   
            {
             /* assume relative to parent of CWD    Ex:   ../src  
                there may be more of these ".." pieces -- for each one found, back 
                up to next earlier slash in string (loop until done)
             */  
   
             j = 0;
             i = strlen(dest);  
             while ((name[j] == '.') && (name[j+1] == '.'))  /* first case already exists */
               {
                /* back up in dest to prior slash */
                for (i=strlen(dest); i >= 0; i--)
                  {
                   if ((dest[i] == '/') || (dest[i] == '\\'))
                     {
                      dest[i] = 0;  /* always leaves dest without final slash */
                      break;          
                     }                  
                  }
                  
                /* move past ".." in name to next slash */
                j += 2;
                if ((name[j] == '\\') || (name[j] == '/'))
                  {
                   j += 1;         
                  }
                else
                  {
                   /* assume have found the end of the ".." sets  -- append the 
                      remainder of the name to what is left of CWD */
                   break;
                  }  
               }
   
             strncat(dest,"/",lim);         
             strncat(dest,name+j,lim);
    
             return TRUE;  
            }
          else
            {
             /* assume relative to CWD    Ex:   .\code    */
             switch (name[1])
               {
                case 0:
                  {
                   /* this is CWD */
                   strncat(dest,"/",lim);         
                   return TRUE;
                   break;  
                  }      
                case '/':
                case '\\':
                  {
                   /* append remainder of name to CWD */
                   strncat(dest,name+1,lim);
                   return TRUE;         
                   break;  
                  }      
                default:
                  {
                   /* something is weird! assume hidden file of unix form .whatever 
                      append it to CWD (should be illegal in Windows/DOS) */
                   strncat(dest,"/",lim);         
                   strncat(dest,name,lim);
                   return TRUE;         
                  }         
               }
            }  
          break;  
         }    
       case '/':
       case '\\':
         {
          /* assume name is already acceptably resolved (on current drive) */
          strlimcpy(dest,name,lim);
          return TRUE;
          break;  
         }    
       default:
         {
          if (name[1] == ':')
            {
             /* drive letter form   Ex:   C:  */ 
             dest[0] = toupper(name[0]);
             dest[1] = ':';
             dest[2] = '/';
             dest[3] = 0;       
             
             if ((name[2] == '\\') || (name[2] == '/'))
               {
                strncat(dest,name+3,lim); 
               }
             else
               {
                strncat(dest,name+2,lim); 
               }  
             return TRUE;  
            }     
          else
            {
             /* assume relative to CWD   Ex:   prg\dev   */     
             os_getcwd(dest,lim);
             remove_final_slash(dest);
             strncat(dest,"/",lim);         
             strncat(dest,name,lim);
             return TRUE;         
            }  
         }    
      }
    return FALSE;
   }



int sub_path(char orig_path[], char cur_path[], char subpath[])
  {
   /* "subtract" original path from current path to yield subpath to append
       to assumed destination path -- if correct, orig always has
       slash at end, full current path always has slash at end */

   int i;
   int j;

   subpath[0] = 0;    

   for (i=0; orig_path[i]; i++)
     {
      if (cur_path[i])
        {
         if (orig_path[i] != cur_path[i])
           {
            /* found char just past leading matched substring */
            break;             
           }             
        }
      else  /* current is shorter -- error */
        {
         return FALSE;      
        }         
     }

   /* must have at least ONE matching char from original */
   if (i <= 0)  
     {
      return FALSE;      
     }
     
   /* if properly formatted data, first char of subpath is NEVER a slash */  
   if (cur_path[i] == '/')  
     {  
      return FALSE;      
     }
     
    /* copy remaining chars to subpath */
   for (j=0; cur_path[i]; j++,i++)
     {
      subpath[j] = cur_path[i];       
     }   
     
   /* tie off string */  

   subpath[j] = 0;
    
   return TRUE;  
  }


void remove_final_slash(char name[])
  {
   int slen;
   
   /* name assumed to be pathname -- if last char is a slash, remove it */
   slen = strlen(name);
   if (slen > 0)
     {
      if ((name[slen-1] == '/') || (name[slen-1] == '/'))
        {
         name[slen-1] = 0;               
        }
     }  
  }


/* in future, make this OS-sensitive for style */

/* verify pathname format for DOS style -- if req_root is TRUE, require
   root folder or drive letter */

/* pathname style specific to linux -- no drive letters */   
int clean_os_pathname(const char oldname[], int req_root)

{
 int i;
 int kar;
 int state = 0;
 
 int root_satisfied = TRUE;
 
 /* sample valid name styles (NOTE -- no DOS-style drive letters):
                                \prg
                                .
                                ..
                                ./prg
                                ..\prg

                         slashes may be / or \ -- converted locally
 */                              

 for(i=0; oldname[i]; i++)
   {
    kar = oldname[i];          
    if (kar == '\\')
       kar = '/';        /* use Linux style slash only */
       
    switch (state)
      {
       case 0:      /* 1st char of pathname -- expect period or slash */
         {
          /* wild not allowed */
          if ((kar == '*') || (kar == '?'))   
            {
             return FALSE;      
            }
          if (kar == '/')
            {
             state = 2;    /* no drive letter, root satisfied -- OK */ 
            }
          else
            {
             if (req_root)   /* relative path not valid if root required */
               {
                return FALSE;          
               }
             if (kar == '.')
               {
                state = 3;   /* OK so far -- check for followup . or \ */     
               }      
             else
               {
                if ((kar == '*') || (kar == '?'))   /* wild not allowed */
                  {
                   return FALSE;
                  }
                /* else is default -- normal filename char */ 
                state = 2; 
               }  
            }    
          break;  
         }      
       case 2:      /* normal pathname -- still OK */
         {
          /* wild not allowed */
          if ((kar == '*') || (kar == '?'))   
            {
             return FALSE;      
            }
          if (kar == '.')
            {
             state = 3;   /* OK so far -- check for followup . or \ */     
            }      
          else
            {        
             /* else is default -- normal filename char */  
             state = 2;   
            }
          break;  
         }      
       case 3:      /* have a . -- check for followup . or anything else */
         {
          /* wild not allowed */
          if ((kar == '*') || (kar == '?'))   
            {
             return FALSE;      
            }
          if (kar == '.')
            {
             state = 4;   /* next must be slash or done */     
            }      
          else
            { 
             /* else is default -- normal filename char */  
             state = 2;   
            }      
          break;  
         }      
       case 4:      /* have .. -- check for followup slash */
         {
          if (kar != '/')  /* MUST have slash at this point unless done */
            {
             return FALSE;      
            }      

          /* else is default -- normal filename char */
          state = 2;   
          break;  
         }
      }          
   }
   
 /* if name did not cut off but got this far, is OK */
 return TRUE;
}


/* make all dirs from root onward to ensure creation of otherwise disconnected 
   deeply nested new directories */
   
int os_mkalldirs(const char dirpath[])  /* OS-neutral interface -- hide dependencies */
                                        /* Assume Turbo C 2.01 / MS-DOS */

  {
   int i;
   int retval;
   char temp[MAXPATHNAMESIZE+1];
   int origdrive;
   int drivenumber;
   int slen;

   strcpy(temp,dirpath);
   
   /* verify LINUX format of pathname */
   if (!clean_os_pathname(temp,TRUE))  /* TRUE = require path to include 
                                             root and/or drive */
     {
      return FALSE;                                 
     }

   slen = strlen(temp);

   if (slen >= 1)  /* don't need to recreate root, anything else needs at least 1 char */
     {
      /* The following assumes a slash follows final directory to be created */
      for (i=1; temp[i]; i++)  /* if root, don't start at 0 -- else if relative, 
                                  must have at least 1 char in name before slash,
                                  therefore always start at 1 */
        {
         if (temp[i] == '/')
           {
            temp[i] = 0;       /* temporarily cut string to leftward portion... */
            mkdir(temp,S_IRWXU | S_IRWXG | S_IRWXO);    
                               /* ...make the rightmost full dir so far... */
            temp[i] = '/';  /* ...restore the slash, then continue adding one 
                                     folder at atime from the left until done */
           }
        }
     }   

   return TRUE;
  }


/* the following function relies on global variables assumed not to change 
   during any given directory scan :
          
       source_origpath      full source pathname of starting directory
                     compatible with output of cwd() function 

       dest_origpath      full destination pathname of starting directory
                     compatible with output of cwd() function 
          
*/
          
int build_copy_name(char finalname[], 
                    char src_origpath[], char dest_origpath[],
                    char filename[], char cur_path[])
  {
   char subpathname[MAXPATHNAMESIZE];    /* this is filled in below */
                    
   finalname[0] = 0;
   if (sub_path(src_origpath,cur_path,subpathname)) /* subpath filled in here */
     {
      strcpy(finalname,dest_origpath);
      
      /* guarantee slash inserted before subpath */
      remove_final_slash(finalname);  /* remove if any */
      strcat(finalname,"/");         /* add it back */
                              
      strcat(finalname,subpathname); /* sub NEVER has leading slash */
      strcat(finalname,filename);
      return TRUE;
     }
   return FALSE;  
  }                   


/* if strg is empty, init with current working dir, else override */
/* also initializes ddir_fullname and dest_fullname */
static int init_sourcepaths(const char strg[], int lim)    /* return TRUE if OK */
  {
   char work[MAXPATHNAMESIZE+1];    

   source_origpath[0] = 0;
   sdir_fullname[0] = 0;
   source_fullname[0] = 0;

   if (lim < 1)
     {
      return FALSE;     
     }
     
   if (!(strg[0]))
     {  
      os_getcwd(work,lim);
      work[lim-1] = 0;
      remove_final_slash(work);
      strcat(work,"/");
     } 
   else
     {
      /* 11/30/2009 GLF -- was if (clean_os_pathname()) followed by strlimcpy 
                           change to fully qualified pathname -- improved */                 
      if (copy_resolved_pathname(work,strg,lim))
        {
         remove_final_slash(work);
         strcat(work,"/");
        }
      else
        {
         return FALSE;                               
        }
     }  


   /* initialize global dest pathname and dest filename */
   strcpy(source_origpath,work);
   strcpy(sdir_fullname,work);
   strcpy(source_fullname,"");
   return TRUE;         

  }

/* if strg is empty, init with current working dir, else override */
/* also initializes ddir_fullname and dest_fullname */
static int init_destpaths(const char strg[], int lim)    /* return TRUE if OK */
  {   
   char work[MAXPATHNAMESIZE+1];    

   dest_origpath[0] = 0;
   ddir_fullname[0] = 0;
   dest_fullname[0] = 0;

   if (lim < 1)
     {
      return FALSE;     
     }
     
   if (!(strg[0]))
     {  
      os_getcwd(work,lim);
      work[lim-1] = 0;
      remove_final_slash(work);
      strcat(work,"/");
     } 
   else
     {
      /* 11/30/2009 GLF -- was if (clean_os_pathname()) followed by strlimcpy 
                           change to fully qualified pathname -- improved */                 
      if (copy_resolved_pathname(work,strg,lim))
        {
         remove_final_slash(work);
         strcat(work,"/");
        }
      else
        {
         return FALSE;                               
        }
     }  


   /* initialize global dest pathname and dest filename */
   strcpy(dest_origpath,work);
   strcpy(ddir_fullname,work);
   strcpy(dest_fullname,"");
   return TRUE;         
  }

/* ---------------------- OS-neutral time and touch functions -------- */

/* time_t type, <time.h>, time() and ctime() functions assumed available across
   OS types */

/* ctimestrg is assumed equivalent to ctime() return string:
   Ex:      Mon Nov 21 11:31:54 2008\n\0     */

time_t *time_ctime(time_t *ptime, char ctimestrg[])

{
 char mostrg[4];

 time_t tnum;

 int iyr;
 int imo;
 int ida;
 int ihr;
 int imin;
 int isec;


 ida = stri(ctimestrg,8,2);
 iyr = stri(ctimestrg,20,4);

 strlwr(strs(mostrg,ctimestrg,4,3));
 if (strcmp(mostrg,"jan") == 0)
    imo = 1;
 if (strcmp(mostrg,"feb") == 0)
    imo = 2;
 if (strcmp(mostrg,"mar") == 0)
    imo = 3;
 if (strcmp(mostrg,"apr") == 0)
    imo = 4;
 if (strcmp(mostrg,"may") == 0)
    imo = 5;
 if (strcmp(mostrg,"jun") == 0)
    imo = 6;
 if (strcmp(mostrg,"jul") == 0)
    imo = 7;
 if (strcmp(mostrg,"aug") == 0)
    imo = 8;
 if (strcmp(mostrg,"sep") == 0)
    imo = 9;
 if (strcmp(mostrg,"oct") == 0)
    imo = 10;
 if (strcmp(mostrg,"nov") == 0)
    imo = 11;
 if (strcmp(mostrg,"dec") == 0)
    imo = 12;
    
 ihr = stri(ctimestrg,11,2);
 imin = stri(ctimestrg,14,2);
 isec = stri(ctimestrg, 17,2);

 tnum = (time_t)unixtime(iyr,imo,ida,ihr,imin,isec);
 *ptime = tnum;
 
 return ptime;
}

/* the following function touch_file is OS_neutral, with encapsulated 
   OS dependencies */

/* assume Linux or MinGW32+Windows */

int touch_file(char filename[], time_t timestamp)
  {
   time_t curtime;
   struct utimbuf utb;

   curtime = time(NULL);
   utb.actime = curtime;        /* not sure this is needed */
   utb.modtime = timestamp;

   if (0 == utime(filename,&utb))
     {
      return TRUE;
     }
   return FALSE;
  }



static char cfdate_strg[12];

/* utility function to format file date */
char *afiledate(char *strg, time_t timestamp)
  {
   time_t gftime;
   struct tm *ptmbuf;

   int gfyr;
   int gfmo;
   int gfday;

   ptmbuf = localtime(&timestamp);

   gfmo = (ptmbuf->tm_mon) + 1;
   gfday = ptmbuf->tm_mday;
   gfyr = (ptmbuf->tm_year) + 1900;

   sprintf(strg,"%04d-%02d-%02d",gfyr,gfmo,gfday);

   return strg;
  }

void ifiledate(int *yr, int *mo, int *day, time_t timestamp)
  {
   struct tm *ptmbuf;

   ptmbuf = localtime(&timestamp);

   *mo = (ptmbuf->tm_mon) + 1;
   *day = ptmbuf->tm_mday;
   *yr = (ptmbuf->tm_year) + 1900;
  }

char *cfiledate(time_t timestamp)
  {
   return afiledate(cfdate_strg,timestamp);
  }

static char cftime_strg[10];

/* utility function to format file date */
char *afiletime(char *strg, time_t timestamp)
  {
   time_t gftime;
   struct tm *ptmbuf;

   int gfhr;
   int gfmin;
   int gfsec;

   ptmbuf = localtime(&timestamp);

   gfhr = ptmbuf->tm_hour;
   gfmin = ptmbuf->tm_min;
   gfsec = ptmbuf->tm_sec;

   sprintf(strg,"%02d:%02d:%02d",gfhr,gfmin,gfsec);

   return strg;
  }

void ifiletime(int *hr, int *min, int *sec, time_t timestamp)
  {
   struct tm *ptmbuf;

   ptmbuf = localtime(&timestamp);

   *hr = ptmbuf->tm_hour;
   *min = ptmbuf->tm_min;
   *sec = ptmbuf->tm_sec;
  }

char *cfiletime(time_t timestamp)
  {
   return afiletime(cftime_strg,timestamp);
  }


/* 11/28/2009 GLF -- newly exposed utility functions */


/* 11/28/2009 GLF empty extension redefined to NOT MATCH 
  to an empty string, but MATCH if string is at least 1 
  char long -- pos is set to position of last dot or
  string length in that case. */

int has_extension(const char strg[], const char ext[])
  {
   int exlen, slen;
   int i, j;
   int lastdot;
   
   slen = strlen(strg);            
   exlen = strlen(ext);            
                   
   /* 11/28/2009 GLF -- change definition of return value to POSITION 
      of extension within string -- still works as TRUE value (C rules)
      but also helps with copy_without_extension() below.  Since it is 
      for an extension to begin at 0, a 0 return value is equivalent to 
      FALSE.
   */   

   /* 1/28/2010 GLF change -- add special coded extension "." to recognize
      request to find . position for ANY extension OR NON-extension */
      
   lastdot = 0;
   for (j=0; strg[j]; j++)
     {
      if (strg[j] == '.')  
        {
         lastdot = j;
        }
     }                
      
   if (!lastdot)    /* if filename has NO extension... */
     {
      if (ext[0] == 0)
        {
         return slen;   /* define to match whole string */
        }                
 
      if (strcmp(ext,".") == 0)
        {
         return slen;                 
        }
        
      /* no dot in filename strg but extension isn't null -- no match */ 
      return 0;  
     }
     

   /* string has at least one dot, last one at position = lastdot */
   if (strcmp(ext,".") == 0) /* if specification is "dot position"... */
     {
      return lastdot;                 
     }
        
   
   /* extension in filname and ext must have same length to match */
   if ((slen-lastdot) != (exlen+1))  /* pretend ext has leading dot */
     {
      return 0;      /* different lengths -- no match */      
     }
     
   /* work forwards from last dot */      
   for (j=lastdot+1,i=0; strg[j] && ext[i]; j++,i++)
     {
      if (tolower(strg[j]) != tolower(ext[i]))
        {
         return 0;       /* failed match */  
        }   
     }
     
   return lastdot;                
  }



/* the following asssumes dest is large enough to hold lim + 1 chars */
void copy_without_extension(char dest[], const char strg[], int lim)
  {
   int exlen, slen;
   int i, j;
   int nodot = TRUE;
   int pos;
   
   strlimcpy(dest,strg,lim);        
   pos = has_extension(dest,".");    /* NOTE: fixed 1/28/2010 GLF */
   dest[pos] = 0;    /* chop off dest filename at position of LAST dot */
  }


/* the following asssumes dest is large enough to hold lim + 1 chars */
void copy_name_from_fullpath(char dest[], const char strg[], int lim)
  {
                   /* gets "name.txt" from "\dir\subdir\name.txt" or "name.txt"
                      or  "subdir"  from  "\dir\subdir\" or "subdir"
                      or   ""       from    "\"  or from ""
                      treats forward or back slashes the same */
   int i, j;
   int priorslash, lastslash;
   
   priorslash = -1;
   lastslash = -1;
   for (j=0; strg[j]; j++)
     {
      if ((strg[j] == '/') || (strg[j] == '\\'))  
        {
         priorslash = lastslash;          
         lastslash = j;
        }
     }                
     
   if (strg[lastslash+1] == 0)   /* if last slash is at end... */
     {
      lastslash = priorslash;  /* ignore slash at end, get prior slash */      
     }            
     
   dest[0] = 0;  
   
   for (i=0,j=lastslash+1; strg[j]; i++,j++)
     {
      if (i >= lim)
        {
         break;   
        }                   
      if ((strg[j] == '/') || (strg[j] == '\\'))  
        {
         break;          
        }      
      dest[i] = strg[j];
      dest[i+1] = 0;                   
     }  
  }    
  


 typedef struct 
   {
    char name[MAXFILEONLYSIZE+1];     
   }
    dirlist_type;

/* assume Linux-style dir scanning, Linux or Windows MinGW32 -- stat() gets file
   info based on file name only, independent of dir scanning status or time */


/* ------ filtername function -- OS neutral interface (dependencies hidden ) -- */
/* the following function assumes olddirentry->name specifies a file in CURRENT DIR */

/* NOTE:  the TRUE return status of this function does NOT indicate whether
          the wildcard pattern matches, only that the function completed all
          file operations and therefore continuing to process makes sense.
          The function indicates pattern match status by setting
          (filefilter) var->ezattrib[0] = ' ' if matched, '!' or ':' if not.
*/

static int filtername(filefilter *newname, dirlist_type *olddirentry, 
                                        const char filter[], char direcname[])
{
 int i;
 int j;
 int nomore;
 int kar;
 

 struct stat statbuf;   /* not used by Turbo C but defined */

 int havedot;
 char predotstate;

 char patwork[22];
 char filwork[22];

 for (i=0; i<20; i++)
   {
    newname->filtname[i] = ' ';
    patwork[i] = ' ';
    newname->filtname[i] = ' ';
   }
 newname->filtname[20] = 0;
 patwork[20] = 0;
 patwork[16] = '.';
 newname->filtname[20] = 0;
 newname->filtname[16] = '.';

 strcpy(newname->ezattrib,"         "); /* fill with 9 blanks by default */
                                      /* order: !DRHSX with 0 terminator */  

 strlimcpy(newname->filenameonly,olddirentry->name,MAXFILEONLYSIZE);

 /* set up filtering for wildcards -- MS-DOS style patterns
    allowed up to 19 chars (extended from DOS 8.3 to 16.3)*/

 for (i=0,j=0; ((i<20) && filter[i]); i++,j++)
   {
    switch (filter[i])
      {
       case '*':
         {
          for (; j<16; j++)
            {
             patwork[j] = '?';
            }
          patwork[16] = '.';
          if (j > 16)
            {
             while (j < 20)
               {
                patwork[j] = '?';
                j++;
               }
             i = 20;      /* forces loop to terminate later */
            }
          break;
         }
       case '?':
         {
          if (j < 16)
            {
             patwork[j] = '?';
            }

          /* 16 is always supposed to be a '.' -- hold j at 16 until a dot found */
          if (j == 16)
             j--;

          if (j > 16)
            {
             patwork[j] = '?';
            }
          break;
         }
       case '.':
         {
          if (i == 0)
            {
             /* Assume this is . or .. directory entry
                                (.xxx without leading name not allowed) */
             newname->ezattrib[0] = ':';
             i = 20;
            }
          else
            {
             /* prevent wildcard match beyond this point for left side */
             for (; j<16; j++)
               {
                patwork[j] = ' ';
               }
             patwork[16] = '.';
             j = 16;      /* next loop will look at extension */
            }
          break;
         }
       default:
         {
          if (j < 16)
            {
             patwork[j] = toupper(filter[i]);
            }

          /* ignore all chars beyond first 16 in body of name --
             reset when extension found */

          if (j == 16)
            {
             patwork[j] = '.';
             j--;
            }

          if ((j > 16) && (j < 20))
            {
             patwork[j] = toupper(filter[i]);
            }
         }
      }
   }
 /* finish off pattern if scan terminated short of alloted block */
 for (; j<20; j++)
   {
    patwork[j] = ' ';
   }
 patwork[16] = '.';
 i = 20;

 /* set up to handle multi-dot extensions by skipping intermediates */
 havedot = 0;
 predotstate = ' ';

 /* scan filename chars the same way the pattern was built -- no actual
    wildcards chars will be in the real filenames -- also retain original
    character case in filtname */

 for (i=0,j=0; ((i<20) && olddirentry->name[i]); i++,j++)
   {
    switch (olddirentry->name[i])
      {
       case '.':
         {
          if (i == 0)
            {
             /* Assume this is . or .. directory entry
                                (.xxx without leading name not allowed) */
             newname->ezattrib[0] = ':';
             i = 20;
            }
          else
            {
             /* prevent wildcard match beyond this point for left side */
             for (; j<16; j++)
               {
                newname->filtname[j] = ' ';
               }
             newname->filtname[16] = '.';
             j = 16;      /* next loop will look at extension */
            }
          break;
         }
       default:
         {
          if (j < 16)
            {
             newname->filtname[j] = olddirentry->name[i];
            }

          /* ignore all chars beyond first 16 in body of name --
             reset when extension found */

          if (j == 16)
            {
             newname->filtname[j] = '.';
             j--;
            }

          if ((j > 16) && (j < 20))
            {
             newname->filtname[j] = olddirentry->name[i];
            }
         }
      }
   }
 /* finish off pattern if scan terminated short of alloted block */
 for (; j<20; j++)
   {
    newname->filtname[j] = ' ';
   }
 newname->filtname[16] = '.';
 i = 20;

/* at this point patwork and newname->filtname have been scanned built the same way,
   except that newname->filtname uses original characters instead of '?' */

 /* scan thru all restructured filename chars and match to pattern */
 for (i=0; newname->filtname[i]; i++)
   {
    if (i < 20)
      {
       /* filter test -- as long as case-insensitive chars match, filter
                      continues to pass;  if wildcard exists, also passes */
       if (toupper(newname->filtname[i]) != patwork[i]) /* patwork already upper case */
         {
          if (patwork[i] != '?')
            {
             newname->ezattrib[0] = '!';
            }
         }
      }
   }



 /* NOTE: newname->ezattrib[0] is reserved for '!' or ':' char if pattern does NOT match */

 /* get file attributes if possible */
 newname->fsize = 0;
 newname->osattrib = 0;
 newname->modstamp = 0;

 /* assume either Windows/MinGW32 or Linux */

 if (0 == stat(olddirentry->name,&statbuf))
   {
    newname->fsize = statbuf.st_size;   
    newname->modstamp = (long)statbuf.st_mtime;
    /* NOTE: MinGW32 stat.h suggests that for dors, the timestamp may be invalid 
             -- need to check this out sometime -- GLF */  
    /* 09/24/2006 GLF kludge patch to prevent bombing on bad file date
       (maybe bug in stat()? )  -- forces bad timestamp to 0 */
    if ((int)(newname->modstamp) == -1)
      {
       newname->modstamp = 0;
      } 
    newname->osattrib = statbuf.st_mode;

    /* OS-DEPENDENT! stat() function and struct stat aren't identical across OS */

    /* Note: the following turned up in a web search, which explains a lot when using
         stat() and directories in Turbo C:

            A problem (feature? bug?) with the Turbo C/Borland C "stat" function
            always returning directories with a "read-only" attribute prevented
            the "TEMP" directory from being used. The Write permission for the
            directory is no longer checked.
    */

    /* assume OS neutral at this point, since stat() results are simulated
        and/or copied above */
    if ((newname->osattrib & S_IFDIR))  /* directory flag */
      {
       newname->ezattrib[1] = 'D';
      }

    newname->ezattrib[2] = 'R';   /* assume readable until know otherwise */
    if (0 == (newname->osattrib & S_IREAD))  /* correct for RO flag */
      {
       newname->ezattrib[2] = ' ';   /* OK, NOT readable */
      }

    newname->ezattrib[3] = 'W';   /* assume writeable until know otherwise */
    if (0 == (newname->osattrib & S_IWRITE))  
      {
       newname->ezattrib[3] = ' ';   /* OK, NOT writeable */
                                     /* also implies DOS READONLY flag */                          
      }

    if ((newname->osattrib & S_IEXEC))  /* EXEC flag simulated if DOS or Windows */
      {
       newname->ezattrib[4] = 'X';
      }

    return 1;  /* successful completion */
   }

 return 0;  /* 0 indicates file attributes not completed */
}



/* ------------ WARNING! RECURSIVE! -------------------------------------- */
/*    scan_dir_r is recursive and OS dependent enough that versions for
      Turbo C and Windows (and Linux) are coded separately below, though they
      share a common OS-neutral interface
*/
      
/* assume Linux-style dir scanning, Linux or Windows MinGW32 */

/* ------------ WARNING! RECURSIVE! -------------------------------------- */

#define MAX_DIRLIST 700

#define MAX_NEST 30
static int dirtree_pos = 0;


/* OS-dependent process to change attribute flags for file or directory 
   IF POSSIBLE -- only R and W flags changeable at this time */

static int ezattrib_chmod(const char ezattr[], const char filefullname[])
  {
   char temp[MAXPATHNAMESIZE+1];
   int slen;
   unsigned allchg;
   int need_ro = FALSE;
   
   strcpy(temp,filefullname);
   slen = strlen(temp);
   if (slen > 1)    /* at least 2 so won't accidentally kill slash if root... */
     {
      if (temp[slen-1] == '/')
        {
         temp[slen-1] = 0;
        }
     }   
   
   /* the following should work under Windows or Linux as well as MS-DOS/TC */    
   if (ezattr[3] == 'W')
     {
      if (ezattr[2] == 'R')
        {
         if (chmod(temp,S_IREAD | S_IWRITE))
           {
            return FALSE;                      
           }
        }
      else  
        {
         if (chmod(temp,S_IWRITE))
           {
            return FALSE;                      
           }
        }
     }
   else
     {
      if (ezattr[2] == 'R')
        {
         if (chmod(temp,S_IREAD))
           {
            return FALSE;                      
           }
        }
      else
        {
         if (chmod(temp,0))
           {
            return FALSE;                      
           }
        }  

      need_ro = TRUE;
     }  

   return TRUE;                      
  }



/* --- recursively scans a subdirectory specified in scan_path --- */
static void scan_dir_r(char path[], const char pattern[], int scan_subs)  
{
 /* since this is recursive, stacked variables are implied here and
    for all parameters */

 char dpath[MAXPATHNAMESIZE+1];  /* local recursive storage for path */
 char hpath[MAXPATHNAMESIZE+1];  /* local storage for path */

 DIR *dirf; 

 struct dirent *pdir;
 struct stat statbuf;
 mode_t gfattrib;
 char strg[30];

 filefilter filtdata;

 char hold_ezattrib[10];

 int i;



 /* temporary fixed-limit dynamic data to hold list of directpry names
    (and attributes under Turbo C) */
 
 dirlist_type *dirlist[MAX_DIRLIST + 1];
 dirlist_type singledir;

 int cnt;
 int n;

 /* recursively scans a subdirectory specified in:  path */
 strncpy(dpath,path,MAXPATHNAMESIZE);  /* this is where the parameter gets copied to stack */
 dpath[MAXPATHNAMESIZE] = 0;
 remove_final_slash(dpath);
 strcat(dpath,"/"); 
               /* if last char isn't slash... */
               /* ... then make it slash */
 cnt = 0;

 if ((dirtree_pos >= 0) && (dirtree_pos < MAX_NEST))
   {
    dirtree_pos++;  

    /* 
    Three steps:  1) build list of subdirectories
                  2) process subdirectories
                  3) process files
                  
    By processing in this order, list is smaller (fewer dirs than files).
    Also when copying dir trees, subdirs are created first, which makes it 
    faster to traverse and easier to read a DIR command.
    */

    /* APPARENTLY in Mingw32, one can only scan current dir */
    os_chdir(dpath);   /* this is where dir path goes down 1 level */

    /* Pass 1: scan and build dir list */
    if (NULL != (dirf = opendir(dpath)))
      {
       while (NULL != (pdir = readdir(dirf)))
         {
          /* look at directory entry, determine file type */
          if (0 == stat(pdir->d_name,&statbuf))
            {
             /* retain only DIR entries in list */
             if (S_ISDIR(statbuf.st_mode))  /* directory flag */
               {
                if ((strcmp(pdir->d_name,".") != 0) && (strcmp(pdir->d_name,"..") != 0))
                  {
                   /* try to store subdir name in list (if not full) */
                   if (cnt < MAX_DIRLIST)
                     {
                      /* dynamically allocate list memory */
                      if (NULL != (dirlist[cnt] = (dirlist_type *)malloc(sizeof(dirlist_type))))
                        {
                         /* store file name in list */
                         strlimcpy(dirlist[cnt]->name,pdir->d_name,MAXFILEONLYSIZE);
                         cnt++;  /* successful dir entry, incr count */
                        }
                     }
                  }
               }
            }
         }
       closedir(dirf);

       /* closing the above dirlist handle before processing subdirs ensures 
          only ONE such handle is active at any time within the recursive scan */

       /* loop to process dirlist[] built above */
       for (n=0;  n < cnt; n++)
         {
          /* apply wildcard filter and assess file attributes if possible */
          if (filtername(&filtdata,dirlist[n],pattern,dpath))
            {
             if (filtdata.ezattrib[1] == 'D') /* only allows dirs below... */
               {
                /* process subdirectory if NOT . or .. */
                if (filtdata.ezattrib[0] != ':') /* filter excludes . or .. */
                  {
                   /* call to user-defined function to process a subdirectory */

                   /* construct the source dirname */
                   strcpy(sdir_fullname,dpath);
                   strcat(sdir_fullname,filtdata.filenameonly);
                   remove_final_slash(sdir_fullname);
                   strcat(sdir_fullname,"/");

                   /* construct the dest dirname by inspecting differences
                      between original source and destination paths and
                      current source path */

                   if (build_copy_name(ddir_fullname,source_origpath,
                           dest_origpath,filtdata.filenameonly,dpath))
                     {
                      /* ---- allow user-provided begin dir process ---- */
                      remove_final_slash(ddir_fullname);
                      strcat(ddir_fullname,"/"); 
      
                      if (create_subtree_on_scan)
                        {
                         /* mkalldirs(ddir_fullname);    a better alternative for the first one */
                       
                         /* The following expects the path exists down to the dir level ABOVE the 
                            final one specified, otherwise it fails to create the dir. */
                         /* mkdir(ddir_fullname);   */
                         
                         /* ONLY make new subfolders if subtree scan is specified */
                         if (scan_subs)
                           {
                            os_mkalldirs(ddir_fullname);   
                           } 
                        }     

                      strcpy(hpath,dpath);
                      remove_final_slash(hpath); /* if last char isn't backslash... */
                      strcat(hpath,"/");        /* ... then make it backslash */


                      /* globally can shut off subdir scans */
                      if (scan_subs)
                        {
                         /* at each dir level, dir_process() can cut off subdir scans */
                         if (dir_process(&filtdata,sdir_fullname,pattern))
                           {
                            /* (not 0) user-defined function specified recursive 
                                                    scanning of subdirectory */   
                            /* go down one dir level */
                            strcpy(hpath,dpath);
                            strcat(hpath,dirlist[n]->name);
                            remove_final_slash(hpath); /* if last char isn't slash... */
                            strcat(hpath,"/");        /* ... then make it slash */
      
                            /* RECURSIVE CALL! -- assume will change current dir */
                            scan_dir_r(hpath, pattern, scan_subs); 
   
                            strcpy(sdir_fullname,hpath);
                            if (build_copy_name(ddir_fullname,source_origpath,
                                    dest_origpath,dirlist[n]->name,dpath))
                              {
                               remove_final_slash(ddir_fullname); /* if last char isn't slash... */
                               strcat(ddir_fullname,"/");         /* ... then make it slash */
                              }
         						}
                        }
                      os_chdir(dpath);   /* maybe redundant, but otherwise necessary */
                     }
                  }   
               }     
            }
          else
            {
             /* stat() NOT OK */  
            }          
         }   
         
       /* free any memory allocated for dirlist[] */
       for (n = (cnt - 1); n >= 0; n--)
         {
          free(dirlist[n]);  
         }   
       cnt = 0;  
      }

    /* since scanning of subdirectories is done, restore current dir */
    /* APPARENTLY in Mingw32, one can only scan current dir */
    os_chdir(dpath);

    /* now that subdirectories have been scanned, go back and process files */
    if (NULL != (dirf = opendir(dpath))) 
      {
       while (NULL != (pdir = readdir(dirf)))
         {
          strcpy(singledir.name,pdir->d_name);
             
          /* apply wildcard filter and assess file attributes if possible */  
          if (filtername(&filtdata,&singledir,pattern,dpath))
            {
             if (filtdata.ezattrib[1] != 'D') /* NOT directory flag */
               {
                if (filtdata.ezattrib[0] == ' ') /* apply wildcard filter -- only matching files */
                  {
                   /* build and verify format of source and dest pathnames */

                   /* construct the source filename */
                   strcpy(source_fullname,dpath);
                   strcat(source_fullname,filtdata.filenameonly);
                   
                   /* construct the dest dirname by inspecting differences
                      between original source and destination paths and 
                      current source filename */

                   strcpy(sdir_fullname,dpath);
                   if (build_copy_name(ddir_fullname,source_origpath,
                           dest_origpath,"",dpath))
                     {
                      remove_final_slash(ddir_fullname); /* if last char isn't slash... */
                      strcat(ddir_fullname,"/");        /* ... then make it slash */
                     }
                        
                   if (build_copy_name(dest_fullname,source_origpath, 
                           dest_origpath,filtdata.filenameonly,dpath))
                     {
                      /* copy ezattrib so changes can be detected */  
                      strcpy(hold_ezattrib,filtdata.ezattrib);

                      /* call to user-defined function to process a file */
                      file_process(&filtdata,dpath,pattern);

                      if (strcmp(filtdata.ezattrib,hold_ezattrib) != 0)
                        {
                         /* difference found -- assume user request to change flags */                                
                         ezattrib_chmod(filtdata.ezattrib,source_fullname);
                        }
                     }       
                  }     
               }   
            }      

          /* clear these out since filenames aren't valid 
             after final file is processed... */
          source_fullname[0] = 0;
          dest_fullname[0] = 0;
         }   
       closedir(dirf);     

       strcpy(hpath,dpath);
       remove_final_slash(hpath); /* if last char isn't backslash... */
       strcat(hpath,"/");        /* ... then make it backslash */

       /* 11/28/2009 GLF -- add a call to process any file list  --
                            called even if no files found -- CWD is still
                            valid for this dir to allow writing sunmary file(s) */
                            
       filelist_finish(&filtdata,dpath,pattern);  

       /* 11/28/2009 GLF -- redefine dir_finish() to handle 
                            coming out of subdir AFTER file scan and 
                            AFTER CWD changed to allow routine to alter
                            or even remove subdir name in the function */
                            
       /* restore CWD to prior state (back up one level) */ 
       os_chdir(dpath);   /* maybe redundant, but otherwise necessary */

       /* copy ezattrib so changes can be detected */  
       strcpy(hold_ezattrib,filtdata.ezattrib);
      
       dir_finish(&filtdata,hpath,pattern);  

       if (strcmp(filtdata.ezattrib,hold_ezattrib) != 0)
         {
          /* difference found -- assume user request to change flags */                                
          ezattrib_chmod(filtdata.ezattrib,hpath);
         }

       /* NOTE: it is legitimate for dir_finish() to alter flags or even delete
          the subdir to which it applies -- therefore, at this point hpath my
          no longer exist, or if it does, the user may have changed flags, though
          legality of changing those flags is OS-dependent */

       /* at this point hpath still is the directory just finished -- 
          check ezattrib to see if it changed -- if so, change the flags 
          in the real directory IF POSSIBLE (is it possible in Linux???) */

       /* restore CWD to prior state (back up one level) */ 
       os_chdir(dpath);   /* maybe redundant, but otherwise necessary */
      }   

    /* go back down the stack */
    dirtree_pos--;
   }
}


/* OS-dependent init code here */
static void os_init(void)          /* MinGW32 / Windows */
  {
  }   


/* ---------------------------------------------------------------- */

/* NOTE -- SAFETY PROBLEMS for overlapping source and destination pathnames

        When auto-creation of directory trees is specified, it is UNSASFE 
        for source and destination trees to overlap, that is, if destination is 
        the same as the source, or is at a lower level than source on the same 
        tree.  In the first case, input-output can occur over the same files
        or recursive deeply nested trees can be spontaneouly created in error.
        In the second case, side-by-side folders are created WHILE the original 
        scan is in progress and this will probably cause failure or erroneous 
        side effects.  ez_dir_scanall() below will automatically fail and exit
        if such overlap is detected WHEN auto-create flag is specified.  
        Otherwise it is the user's responsibility to detect a possible problem.  
        In many cases, where only source is used, the overlap is immaterial.
        in others, the user may wish to prevent possible problems by detection 
        of this issue.  Rather than check for overlap in all cases, the function
        is_overlap_unsafe() below allows user to check before calling the scan.
*/        

/* the local overlap test assumes fully resolved pathmnames */
static int overlapunsafe(const char src[], const char dest[])
  {
   int i;
   
   for (i=0; src[i]; i++)
     {
      if (src[i] != dest[i])
        {
         return FALSE;   /* SAFE! -- dest is separate from src */         
        }       
     }    
   return TRUE;   /* UNSAFE! -- dest is same as or lower level than src */  
  }   
   
   
/* this is the exposed function -- resolves pathnames first, then compares */   
int is_overlap_unsafe(const char src[], const char dest[], int lim)
  {
   char srcwork[MAXPATHNAMESIZE+1];                         
   char destwork[MAXPATHNAMESIZE+1];
   
   if (!copy_resolved_pathname(srcwork,src,lim))
     {
      return FALSE;                                          
     }                         
   if (!copy_resolved_pathname(destwork,dest,lim))
     {
      return FALSE;                                          
     }                         
   return overlapunsafe(srcwork,destwork);    /* TRUE if unsafe */
  }   
   
/* ---------------------------------------------------------------- */

/* externally visible call to scan a directory or tree -- 
                                     includes setup and safety checks */


/* nonrecursive interface wrapper which assumes standard global
   fully resolved source and dest filename variables are preset 
   and maintained across recursive internal calls
*/
   
int ez_dir_scanall(const char orig_srcpath[], const char orig_destpath[], 
                   const char wildpat[], int create_dest_dir, int scan_subs)
{
 char orig_cwd_path[MAXPATHNAMESIZE+1];
 char path[MAXPATHNAMESIZE+1];
 char pattern[33];
 int i;
 int continue_scan_subs;
 int unsafe;

 dirlist_type dirlist; /* special -- only holds original source path entry */
 filefilter filtdata;

 char hold_ezattrib[10];


 /* assumes calling user has preset source_origpath and dest_origpath */

 os_init();
 orig_cwd_path[0] = 0;

 /* store original current dir to allow restoring later */
 os_getcwd(orig_cwd_path,MAXPATHNAMESIZE);
 orig_cwd_path[MAXPATHNAMESIZE] = 0;
 
 /* source and dest both default to cwd below if path specified is empty */
 /* validate source and dest or fail */  

 /* validate and set source paths to that preset in orig_srcpath parameter */
 if (!init_sourcepaths(orig_srcpath,MAXPATHNAMESIZE))
   {
    return FALSE;                             
   }                           
   
 /* validate and set dest paths to that preset in dest_origpath */
 if (!init_destpaths(dest_origpath,MAXPATHNAMESIZE)) 
   {
    return FALSE;                             
   }                                                 

 /* validate and set dest paths to that preset in orig_destpath parameter */
 if (!init_destpaths(orig_destpath,MAXPATHNAMESIZE)) 
   {
    return FALSE;                             
   }                                                 

 /* if source and dest trees overlap, recursive trees, failure, or catastrophic 
    side effects may occur -- detect if names are unsafe and deal with that here
    -- the local call expects a comparison between fully resolved names, 
    which were just created above 
 */

 unsafe = overlapunsafe(source_origpath,dest_origpath);
 
 create_subtree_on_scan = create_dest_dir;
 if (create_subtree_on_scan)
   {
    /* always automatically fail if unsafe and trying to create dirs */                        
    if (unsafe)                        
      {
       return FALSE;   /* prevents recursive mkalldirs() */                
      }
      
    /* mkalldirs(ddir_fullname);  a better alternative to mkdir(ddir_fullname) */
  
    /* The following expects the path exists down to the dir level ABOVE the 
       final one specified, otherwise it fails to create the dir. */
    
    os_mkalldirs(ddir_fullname);    /* the FIRST dir MUST be there -- others 
                                       controlled by scan_subs flag */
   }     

 /* if no directory creation -- user may consider it OK or failure if dest and src 
    are the same or overlap -- user may call is_unsafe_overlap() to detect and
    decide what to do -- if a copy or file creation is happening, user should be 
    careful not to corrupt input files by accident, but if no copy is occurring 
    or dest not used, should routinely be safe */
      
 /* validate and process wildcard */
 strcpy(pattern,"*.*");  
 if (wildpat[0] != 0)
   {
    strncpy(pattern,wildpat,32);  
    pattern[32] = 0;
   }
      
 /* 11/28/2009 GLF -- add one extra call to dir_process() at beginning to match 
    call to dir_subfinish() at end of procerssing original source directory --
    ignore return value since first directory is ALWAYS scanned */

 strlimcpy(dirlist.name,sdir_fullname,MAXFILEONLYSIZE);
 remove_final_slash(dirlist.name);  /* remove if any */

 /* apply but ignore wildcard filter and assess file attributes if possible */
 if (filtername(&filtdata,&dirlist,"*.*",sdir_fullname))
   {
    /* ignore results -- above call used only to set filter data */                                                           
   }

 continue_scan_subs = dir_process(&filtdata,sdir_fullname,pattern);
 /* first dir_process() must allow file scans but may cut off subdirs */
 if (scan_subs)
   {
    scan_subs = continue_scan_subs;  
   }
   
 /* recursive call needs a local (stack) copy of current path */     
 strcpy(path,source_origpath);  

 /* clear stack for directory scan */
 dirtree_pos = 0;  /* 0 is empty stack */

 /* RECURSIVE CALL! -- scans a subdirectory specified in path */
 scan_dir_r(path,pattern,scan_subs); 


 if (orig_cwd_path[0])  /* if original path was stored */
   {
    /* restore current dir to original JUST IN CASE was altered */
    os_chdir(orig_cwd_path);
   }   
}

#endif    /* Linux */
