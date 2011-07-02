/* scandir.h -- framework for directory scanning programs */

#ifndef SCANDIR_H__
#define SCANDIR_H__

/* OS-neutral directory scan module by Gary Flispart (last modifued 11/30/2009) */

/* Version 3 -- Major changes to API and exposed utility functions 11/30/2009 GLF 
                Also fixed bug in extension matching algorithm which sometimes
                indicated match when it shouldn't have */

/* this module performs all directory scanning by examining ALL files in the
   tree as specified, and marks the files found for inclusion or exclusion
   in an externally driven search -- wildcard matching is performed by this 
   module, not by the OS -- if a file name does not match the scan wildcards
   or specs, it's filtering data block is marked with a '!' character so it 
   may be ignored -- unless directory matching is specified, dir names are 
   marked with '!' so that only filenames are passed back for file processing */

/* this module defines a recursive directory scan function which changes current 
   working directory as needed to visit all files and subdirs beginng at the 
   original current dir on first invocation -- on return, current working dir
   is restored to the same point (unless an error aborts the program, in which 
   case the cwd could be anywhere) 
   
   The following function is defined in this library:   

      void ez_dir_scanall()         
      
      the above function in turn repeatedly calls as needed four externally defined 
      (in the user program) functions by name within a recursive directory scan
      of the current working directory (CWD) at time of call:

      extern int dir_process();    performed at beginning of a subdirectory scan
                                   BEFORE performing any process on its files or subdirs
                                   current working dir is set to this dir before call
                                   user may cut off lower level subdir scans by 
                                   returning 0 or FALSE (normally return 1 or TRUE)

      extern void dir_finish();    performed at end of a subdirectory scan AFTER
                                   visiting all subdirectories and AFTER visiting files
                                   current working dir is set to parent dir or
                                   original top-level source dir before call
                                   user may specify modified attributes and may even
                                   delete this directory within this function

      extern void file_process();  performed once per filename 'seen' by match spec
                                   current working directory is set to the folder in 
                                   which this file resides
                                  
      extern void file_finish();   performed at end of a subdirectory scan AFTER
                                   visiting all included files and perhaps subfolders
                                   current working dir is set to this dir before call
                                   

      the parameters passed to each of the above set up the minimum necessary
      context to allow each such process to handle its intended function without
      the user program needing to know details of the the directory tree search
      
      the search order is fixed:
          find all subdirs of this dir (assumed not to change while scanning) 
             
             recursively scan each subdir, then:
             scan all files this dir in order found by the OS-dependent functions

      this order is usually stable and requires minimal overhead resources to 
      complete  -- if necessary to scan files in another (e.g. alphabetical)
      the user may need to compile a list for this dir, then process the files 
      using the sorted list
      
*/

/* There are OS dependencies across Turbo C (MS-DOS), MinGW32 (Windows) and Linux,
   but they are mostly hidden when using the open scandir.h interfaces.  However, 
   the exact structure of type filefilter which is passed by the open interface 
   IS OS-dependent, but in normal use the differences are neutral or transparent 
   due to aliasing of definitions.   This header file (scandir.h) and its support 
   code (scandir.c) uses conditional compilation to isolate the OS dependencies.
*/   

/* time_t is available all OS's -- standardize on its use for timestamps */

/* attribute list is kept in a neutral format -- string with 10 chars 
   (9 data + 0) with attributes reflecting Linux and DOS styles as follows:

   string generally reflects results of scan matching a given entry
   perhaps using wildcards and including directories these allow easy display 
   of file attributes consistent with all supported OS's

      default on creation = 9 blanks + 0 terminator
      
      exattrib[0] = '!' or blank   '!' means DO NOT use this entry -- it failed
                                   to match the search criteria (e.g. wildcards)
      
      (need to find out how to determine HIDDEN and SYSTEM attributes --
       for now, simulate them in Linux/Mingw32 -- in Linux, a file is hidden from 
       "ls" if preceded by a '.' character, while DOS uses a flag apparently not 
       visible to MinGW32 stat interface -- SYS flag has no equivalent in Linux,
       except as inferred from privileged permissions on the file) 
     
      ezattrib[0] = '!' or blank     '!' means NOT MATCHED to filter selection
      ezattrib[1] = 'D' or blank     'D' means directory
      ezattrib[2] = 'R' or blank     'R' means read-only
      ezattrib[3] = 'W' or blank     'X' means executable; simulated in DOS if
      ezattrib[4] = 'X' or blank     'X' means executable; simulated in DOS if
                                      EXE, COM, BAT, or DLL file; EXEC flag 
                                      is meaningful in Linux
      ezattrib[5] = 'H' or blank     'H' means hidden (only used for MS-DOS/TC)
      ezattrib[6] = 'S' or blank     'S' means system (only used for MS-DOS/TC)
      ezattrib[7] = 'A' or blank     'A' means archive (only used for MS-DOS/TC)
      ezattrib[8] = RESERVED, blank      not yet defined (room for expansion)
      ezattrib[9] =  0 string terminator
                                            
   In routine use, the caller should not need to change anything in the ezattrib 
   string field.  However, sometimes it makes sense for a program to alter a flag,
   and the ezattrib field can be considered a two-way communication -- if the 
   ezattrib string is altered during file_process() or dir_finish(), it is taken 
   as a signal to change the file or directory attributes.  Not all attributes 
   may be changed this way.  Only R and W are supported on all OS's.  Under MS-DOS
   (using Turbo C) and Windows (MinGW32) the additional H, S, and A flags are 
   supported.  X is not changed by this method under DOS and Windows, since 'X' 
   as an independent attribute is a Unix concept.  Instead, 'X' is tied to 
   certain extensions (EXE, COM, BAT, and DLL), and consistent with the Unix 
   meaning for directories that can be scanned by users, 'X' is assumed set for 
   all Windows or DOS directories.  For Linux, the 'X' flag is set according to
   the OS.  The DOS READONLY flag is handled as 'R ' as opposed to 'RW'.  It 
   remains to be seen which attributes may be safely altered for directories.

*/

/* ------- OS-dependent includes and definitions ---------------------- */
#include "gflib.h"

#include <string.h>
#include <math.h>

#include "osdep.h"      /* needed for _stken expansion and system() in TC-DOS */

#if defined(__TURBOC__)

   #include <dir.h>
   #include <dos.h>
   #include <io.h>
   #include <time.h>
   #include <sys/stat.h>
   #include <ctype.h>

#else
/* assume Linux-style dir scanning, Linux or Windows MinGW32 */

   #include <ctype.h>
   #include <utime.h>
   #include <stdio.h>
   #include <stdlib.h>
   #include <malloc.h>
   
   #if defined(__MINGW32__)
      #include <io.h>
      #include <dir.h>
      #include <dos.h>
   #endif

#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#pragma pack(1)

#endif

#ifndef FALSE
   #define FALSE 0
#endif
#ifndef TRUE
   #define TRUE 1
#endif

/* --------------------------------------------------------------------- */

/* global data assumed not to change during any full dir scan cycle */
/* assumed to be initialized and compatible with output of cwd() function */

#define MAXPATHNAMESIZE 999     

#ifdef __TURBOC__
#define MAXFILEONLYSIZE 16
#else
#define MAXFILEONLYSIZE 260
#endif


#if defined(__TURBOC__)
/* assume Turbo C and DOS-style dir scanning */
/* no definitions yet 12/14/2008 GLF */

/* OS dependent */
/* Since the Turbo C stat() function yields bad file timestamps, it is necesssary
   to override any stat() info with data found during findfirst/findnext scans.
   That data needs to be passed to the filtername() function when it is called.
   Therefore, extra OS-specific fields have been added to the filefilter struct
   under TC to hold that data.  The filtername() function therefore may only be
   validly called once poer file or directory within any given findfirst/findnext
   scan, while that data is still valid.
*/   


/* Use of filefilter type provides OS neutral interface though it contains
   OS-dependent elements */
typedef struct
  {
   char ezattrib[10];
   long modstamp;
   long fsize;
   char filtname[22];
   char filenameonly[MAXFILEONLYSIZE+1];

   short osattrib;
  }
   filefilter;

#else
/* assume Linux-style dir scanning, Linux or Windows MinGW32 */

/* Use of filefilter type provides OS neutral interface though it contains
   OS-dependent elements */
typedef struct
  {
   char ezattrib[10];
   time_t modstamp;
   size_t fsize;
   char filtname[22];
   char filenameonly[MAXFILEONLYSIZE+1];

   unsigned int osattrib;
  }
   filefilter;

#endif


/* ----- Externally visible user interface ---------------------------- */

/* ONLY ONE DIRECTORY SCAN MAY BE ACTIVE AT A TIME!  However, one may perform
   as many new scans as desired after prior scans are finished. It will usually
   be necessary to reset source_origpath and/or dest_origpath before any new 
   scan (including the first) */

/* The ez_dir_scanall() function relies the following on global variables 
   assumed not to change during any given directory scan:

       source_origpath    full source pathname of starting directory
                          compatible with output of cwd() function

       dest_origpath      full destination pathname of starting directory
                          compatible with output of cwd() function

   On entry to ez_dir_scanall(), if either or both of the above contain the empty
   string (""), they will be replaced as needed by the current working directory.

   The global variables below are maintained by the ez_dir_scanall() methods
   and are available to the user functions to keep track of which file or 
   directory is being scanned or copied at each step:

       source_fullname    full source pathname of current file being handled
                          by file_process() function (supplied by user). Always
                          contains the file currently being scanned during 
                          file_process().  During dir_process() or dir_finish() 
                          contains empty string.

       dest_fullname      full destination pathname of current file being handled 
                          by file_process() function (supplied by user). Contains
                          the filename (if meaningful) to which the current source
                          file (above) would be copied to maintain the subtree 
                          stucture of the source directory, during file_process().
                          During dir_process() or dir_finish() contains empty string.
                          Meaningful only if dest_origpath is preset to a meaningful 
                          destination directory before calling ez_dir_scanall().

       sdir_fullname      full source directory pathname only of current dir handled
                          by file_process(), dir_process(), or dir_finish() functions
                          (supplied by user).  Always contains the directory currently 
                          being scanned.

       ddir_fullname      full destination directory pathname only of current dir handled
                          by file_process(), dir_process(), or dir_finish() functions
                          (supplied by user) to which the current source file (above) 
                          would be copied to maintain the subtree stucture of the source 
                          directory.
                          
       ddir_fullname      full destination directory pathname only of current dir handled 
                          by file_process() function (supplied by user). Contains
                          the filename (if meaningful) to which the current source
                          file (above) would be copied to maintain the subtree 
                          stucture of the source directory.

       The scan algorithm guarantees each file (as filtered by selection criteria) 
       will be visited exactly once with file_process(), and each directory under
       source_origpath will be visited once each for dir_process() (before file scan)
       and for dir_finish() (after files have been scanned).

*/

extern char source_origpath[MAXPATHNAMESIZE+1];
extern char dest_origpath[MAXPATHNAMESIZE+1];

extern char source_fullname[MAXPATHNAMESIZE+1];
extern char dest_fullname[MAXPATHNAMESIZE+1];

extern char sdir_fullname[MAXPATHNAMESIZE+1];
extern char ddir_fullname[MAXPATHNAMESIZE+1];



/* --- IMPORTANT! The following functions must be defined in user program! --- */

/* in all the following user-defined functions, userstrg is a mechanism
   to pass "commands" down to the file and directory process functions
   from the calling program -- userstrg is passed unaltered through all
   scan levels and can be examined to tailor function behavior */
   
/* wildcard is similarly passed through all functions without change */
   
/* process a single file -- name found in filtdata->filenameonly */
extern void file_process(filefilter *filtdata, char path[],
                                            const char wildcard[]);

/* process a single subdirectory -- name found in filtdata->filenameonly 
   returns 1 if further recursive scan of subdirectory is desired
   returns 0 if no further lower-level subdir processing required */
extern int dir_process(filefilter *filtdata, char path[],
                                             const char wildcard[]);

/* process the end of a recursive scan of a subdirectory AFTER files visited */
extern void dir_finish(filefilter *filtdata, char path[],
                                             const char wildcard[]);

/* process the end of a file list in a single subdirectory */
extern void filelist_finish(filefilter *filtdata, char path[],
                                             const char wildcard[]);

/* ---------------------------------------------------------------------- */


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

int is_overlap_unsafe(const char src[], const char dest[], int lim);





/* externally visible call to scan a directory or tree -- 
   includes setup and safety checks and maintenanbce of global variables */

/* the following function relies on global variables assumed not to change 
   during any given directory scan :
          
       source_origpath      full source pathname of starting directory
                     compatible with output of cwd() function 

       dest_origpath      full destination pathname of starting directory
                     compatible with output of cwd() function 
          
*/

int ez_dir_scanall(const char orig_srcpath[], const char orig_destpath[], 
                   const char wildpat[], int create_dest_dir, int scan_subs);

          
/* ----------------------------------------------------------------------- */          
/* externally visible utility functions */          

/* time_t type, <time.h>, time() and ctime() functions assumed available across
   OS types */

/* ctimestrg is assumed equivalent to ctime() return string:
   Ex:      Mon Nov 21 11:31:54 2008\n\0     */

/* the following functions are OS_neutral, with encapsulated OS dependencies */
time_t *time_ctime(time_t *ptime, char ctimestrg[]); /* convert ctime back to time */
int touch_file(char filename[], time_t timestamp);   /* alter file timestamp */
int os_rmdir(const char *path);              /* hides OS dependencies for rmdir */
int os_chdir(const char *path);              /* hides OS dependencies for chdir */
char *os_getcwd(char *buf, int buflen);      /* hide OS dependencies for GETCWD */
long os_filelength(FILE *hfile);             /* find the length of an open file stream */

/* support functions for building fully qualified filenames from context */
int sub_path(char orig_path[], char cur_path[], char subpath[]);
void remove_final_slash(char name[]);

/* verify pathname format for DOS style -- if req_root is TRUE, require
   root folder or drive letter */
int clean_os_pathname(const char oldname[], int req_root);
int os_mkalldirs(const char dirpath[]); /* OS-neutral interface -- hide dependencies */
int build_copy_name(char finalname[], 
                    char src_source_origpath[], char dest_source_origpath[],
                    char filename[], char cur_path[]);

/* 11/28/2009 GLF -- newly exposed utility functions */
int has_extension(const char strg[], const char ext[]);  
                 /* returns 0 if strg (filename) has extension which does not 
                    match ext -- if it DOES match, returns position of LAST dot in
                    strg (Ex:  has_extension("filename.c","c") returns 8 */
                    
/* the following asssumes dest is large enough to hold lim + 1 chars */
void copy_without_extension(char dest[], const char strg[], int lim);
                   /* copies up to LAST dot */

/* the following asssumes dest is large enough to hold lim + 1 chars */
void copy_name_from_fullpath(char dest[], const char strg[], int lim);
                   /* gets "name.txt" from "\dir\subdir\name.txt" or "name.txt"
                      or  "subdir"  from  "\dir\subdir\" or "subdir"
                      or   ""       from    "\"  or from ""
                      treats forward or back slashes the same */

/* converts relative pathnames to fully qualified pathnames as needed */
int copy_resolved_pathname(char dest[], const char name[], int lim);


/* useful functions to format file date */
char *afiledate(char strg[], time_t timestamp);  /* allow at least 11 */
void ifiledate(int *yr, int *mo, int *day, time_t timestamp);
char *afiletime(char strg[], time_t timestamp);  /* allow at least 9 */
void ifiletime(int *hr, int *min, int *sec, time_t timestamp);
char *cfiledate(time_t timestamp);
char *cfiletime(time_t timestamp);


#endif  /* SCANDIR_H__ */

