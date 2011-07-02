/* asynctst.c -- test Linux asynchronous serial port I/O -- Gary Flispart
                 based on sample code found on 5 March 2008 at:

                    http://tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html

                    Serial Programming HOWTO
                    Gary Frerking

                    gary@frerking.org
      
                    Peter Baumann
                    Revision History
                    Revision 1.01	2001-08-26	Revised by: glf (NOTE: not THIS GLF)
                    New maintainer, converted to DocBook
                    Revision 1.0	1998-01-22	Revised by: phb
                    Initial document release

                    1.1. Copyright Information

                    This document is copyrighted (c) 1997 Peter Baumann, 
                    (c) 2001 Gary Frerking and is distributed under the terms 
                    of the Linux Documentation Project (LDP) license, stated below.

                    Unless otherwise stated, Linux HOWTO documents are copyrighted by 
                    their respective authors. Linux HOWTO documents may be reproduced 
                    and distributed in whole or in part, in any medium physical 
                    or electronic, as long as this copyright notice is retained 
                    on all copies. Commercial redistribution is allowed and 
                    encouraged; however, the author would like to be notified of 
                    any such distributions.

                    All translations, derivative works, or aggregate works 
                    incorporating any Linux HOWTO documents must be covered 
                    under this copyright notice. That is, you may not produce a 
                    derivative work from a HOWTO and impose additional 
                    restrictions on its distribution. Exceptions to these rules 
                    may be granted under certain conditions; please contact 
                    the Linux HOWTO coordinator at the address given below.

                    In short, we wish to promote dissemination of this 
                    information through as many channels as possible. However, 
                    we do wish to retain copyright on the HOWTO documents, and 
                    would like to be notified of any plans to redistribute 
                    the HOWTOs.

                    If you have any questions, please contact 
                    <linux-howto@metalab.unc.edu>

*/

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
  
/* #define BAUDRATE B38400 */
/* #define MODEMDEVICE "/dev/ttyS1" */

#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
  
volatile int stop = FALSE; 
  
int wait_flag=TRUE;                    /* TRUE while no signal received */

/***************************************************************************
* signal handler. sets wait_flag to FALSE, to indicate above loop that     *
* characters have been received.                                           *
***************************************************************************/
     
static void signal_handler_IO (int status)
  {
   printf("received SIGIO signal.\n");
   wait_flag = FALSE;
  }
       

static int fd,c;
static struct termios oldtio,newtio;
static struct sigaction saio;           /* definition of signal action */
static char buf[255];
static char comport_fname[40] =  "/dev/ttyS1";
  

int open_comport(int portspec, int baud)
  {
   if ((portspec < 1) || (portspec > 4))
     {
      return FALSE;           
     }             
          
   /* prepare the comport filename */
   sprintf(comport_fname,"/dev/ttyS%d",portspec);

   /* open the device to be non-blocking (read will return immediatly) */
   fd = open(comport_fname, O_RDWR | O_NOCTTY | O_NONBLOCK);
   if (fd < 0) 
     {
      return FALSE;
     }
  
   /* install the signal handler before making the device asynchronous */
   saio.sa_handler = signal_handler_IO;
   saio.sa_mask = 0;
   saio.sa_flags = 0;
   saio.sa_restorer = NULL;
   sigaction(SIGIO,&saio,NULL);
    
   /* allow the process to receive SIGIO */
   fcntl(fd, F_SETOWN, getpid());

   /* Make the file descriptor asynchronous (the manual page says only 
     O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
   fcntl(fd, F_SETFL, FASYNC);
  
   tcgetattr(fd,&oldtio); /* save current port settings */

   /* set new port settings for canonical input processing */
   newtio.c_cflag = baud | CRTSCTS | CS8 | CLOCAL | CREAD;
   newtio.c_iflag = IGNPAR | ICRNL;
   newtio.c_oflag = 0;
   newtio.c_lflag = ICANON;
   newtio.c_cc[VMIN]=1;
   newtio.c_cc[VTIME]=0;
   tcflush(fd, TCIFLUSH);
   tcsetattr(fd,TCSANOW,&newtio);
   
   return TRUE;
  }  
  
int close_comport(int portspec)
  {
   /* restore old port settings */
   tcsetattr(fd,TCSANOW,&oldtio);
  } 
  
  
int read_com(int portspec)
  {
   if (wait_flag==FALSE) 
     { 
      res = read(fd,buf,255); 
      buf[res]=0;
      printf(":%s:%d\n", buf, res);
      if (res==1)
        {
         stop = TRUE; /* stop loop if only a CR was input */
        }
      wait_flag = TRUE;      /* wait for new input */
     }
  }
  
  
int avail_com(int portspec)
  {
  }  
  
  
main()
  {
   int c,res;
  
   if (open_comport(1))
     {
      /* loop while waiting for input. normally we would do something
         useful here */ 
      while (stop == FALSE) 
        {
         printf(".\n");usleep(100000);
         /* after receiving SIGIO, wait_flag = FALSE, input is available
            and can be read */
         if (wait_flag==FALSE) 
           { 
            res = read(fd,buf,255); 
            buf[res]=0;
            printf(":%s:%d\n", buf, res);
            if (res==1)
              {
               stop = TRUE; /* stop loop if only a CR was input */
              }
            wait_flag = TRUE;      /* wait for new input */
           }
        }
     }
  }
     
