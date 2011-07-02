/* gftiolin.c -- Linux specific support routines for gftermio.c

           DO NOT USE THIS MODULE DIRECTLY -- it is included within conditional
           code in gftermio.c!
          
           -- adapted to C by Gary Flispart 1988-2008

    NOTE:  as of 3/13/2008 this contains stub code -- need to research 
           Linux calls further

           BUG FIX 20110206 GLF:  Need to actually USE return value of write().
           In tests, fast streams of writes to port without the fix below caused 
           characters to drop out.  The fix below repeatedly retries if error.


           test Linux asynchronous serial port I/O -- Gary Flispart
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



------------------------------------------------------------------------- */

#ifdef __TURBOC__
#else
#ifdef __MINGW32__
#else                   /* assume Linux */

/* The Linux version of GFTERMIO uses POSIX and/or IOCTL calls. COM1 thru COM4 
   are supported simultaneously.
*/
   
#ifndef GFTIOLIN___
#define GFTIOLIN___

#include "gflib.h"

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <sys/types.h>


/* input and output buffer size (each) */
#define BUFFSIZE 6000

#define INTENREG   1
#define INTIDREG   2
#define LINECTRL   3
#define MODEMCTRL  4
#define LINESTAT   5
#define MODEMSTAT  6


/* assume Linux POSIX ports */
/* allow for ports 1-4 corresponding to COM1-COM4 equivalents ttyS0-ttyS3, 
   and extend ports 5-8 to USB serial adapters ttyUSB0-ttyUSB3 */

static int openport[9] =
  {
   -1,
   -1,
   -1,
   -1,
   -1,
   -1,
   -1,
   -1,
   -1
  };   

static const char *portname[9] =
  {
   "",
   "/dev/ttyS0",
   "/dev/ttyS1",
   "/dev/ttyS2",
   "/dev/ttyS3",
   "/dev/ttyUSB0",
   "/dev/ttyUSB1",
   "/dev/ttyUSB2",
   "/dev/ttyUSB3"
  };   

static struct termios oldtio[9];
static struct termios newtio[9];
static struct sigaction saio[9];           /* definition of signal action */

static char buf[9][255];

static int wait_flag[9] = 
  {
   TRUE,
   TRUE,
   TRUE,
   TRUE,
   TRUE,
   TRUE,
   TRUE,
   TRUE,
   TRUE
  };

static int port_data_avail[9] = 
  {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0
  };

static int holddtr[9] = 
  {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0
  };
  
static int holdrts[9] = 
  {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0
  };
  

static int port_data[9] = 
  {
   0,0,0,0,0,0,0,0,0
  };   

static const char *comname[] = 
  {
   "COM1:",  /* don't use, but make sense */
   "COM1:",  
   "COM2:",  
   "COM3:",  
   "COM4:",  
   "COM5:",  
   "COM6:",  
   "COM7:",  
   "COM8:"  
  };   


/* --- signal handlers for each of four comports --------------- */

static void sig_handler1 (int status)
  {
/*   printf("received SIGIO signal.\n"); */
   wait_flag[1] = FALSE;
  }
  

static void sig_handler2 (int status)
  {
/*   printf("received SIGIO signal.\n"); */
   wait_flag[2] = FALSE;
  }
  

static void sig_handler3 (int status)
  {
/*   printf("received SIGIO signal.\n"); */
   wait_flag[3] = FALSE;
  }
  

static void sig_handler4 (int status)
  {
/*   printf("received SIGIO signal.\n"); */
   wait_flag[4] = FALSE;
  }
  

static void sig_handler5 (int status)
  {
/*   printf("received SIGIO signal.\n"); */
   wait_flag[5] = FALSE;
  }
  

static void sig_handler6 (int status)
  {
/*   printf("received SIGIO signal.\n"); */
   wait_flag[6] = FALSE;
  }
  

static void sig_handler7 (int status)
  {
/*   printf("received SIGIO signal.\n"); */
   wait_flag[7] = FALSE;
  }
  

static void sig_handler8 (int status)
  {
/*   printf("received SIGIO signal.\n"); */
   wait_flag[8] = FALSE;
  }
  

static close_com_single(int portspec)

  {
   if ((portspec < 1) || (portspec > 8))
     {
      return;
     }

   if (openport[portspec] >= 0)
     {
      /* restore old port settings */
      tcsetattr(openport[portspec],TCSANOW,&(oldtio[portspec]));

      /* clear any associated buffer */
      port_data_avail[portspec] = FALSE;
      port_data[portspec] = FALSE;
      openport[portspec] = -1;
     }
  }


void close_com(int portspec)

  {
   /* special case -- if portspec == 0, close all com ports */
   if ((portspec < 1) || (portspec > 8))
     {
      close_com_single(1);           
      close_com_single(2);           
      close_com_single(3);           
      close_com_single(4);    
      close_com_single(5);    
      close_com_single(6);    
      close_com_single(7);    
      close_com_single(8);    
      return;       
     }

   close_com_single(portspec);
  }



/* ------- NOTES: from tty_ioctl(4) - Linux man page: ---------------------
             
   Modem control
   
   TIOCMGET int *argp
       get the status of modem bits. 
   TIOCMSET const int *argp
       set the status of modem bits. 
   TIOCMBIC const int *argp
       clear the indicated modem bits. 
   TIOCMBIS const int *argp
       set the indicated modem bits.
   
   Bits used by these four ioctls:
   
   TIOCM_LE        DSR (data set ready/line enable)
   TIOCM_DTR       DTR (data terminal ready)
   TIOCM_RTS       RTS (request to send)
   TIOCM_ST        Secondary TXD (transmit)
   TIOCM_SR        Secondary RXD (receive)
   TIOCM_CTS       CTS (clear to send)
   TIOCM_CAR       DCD (data carrier detect)
   TIOCM_CD         see TIOCM_CAR
   TIOCM_RNG       RNG (ring)
   TIOCM_RI         see TIOCM_RNG
   TIOCM_DSR       DSR (data set ready)
   
   Marking a line as local
   
   TIOCGSOFTCAR int *argp
       ("Get software carrier flag") Get the status of the CLOCAL flag in the 
           c_cflag field of the termios structure. 
   TIOCSSOFTCAR const int *argp
       ("Set software carrier flag") Set the CLOCAL flag in the termios structure 
           when *argp is non-zero, and clear it otherwise.
   
   If the CLOCAL flag for a line is off, the hardware carrier detect (DCD) signal 
   is significant, and an open(2) of the corresponding tty will block until DCD 
   is asserted, unless the O_NONBLOCK flag is given. If CLOCAL is set, the line 
   behaves as if DCD is always asserted. The software carrier flag is usually 
   turned on for local devices, and is off for lines with modems.
   
--------------------------------------------------------------------------*/


unsigned int status_com(int portspec)
  {
   int serstat;
   int dcd = 0x80;
   int ri  = 0x40;
   int dsr = 0x20;
   int cts = 0x10;

   if ((portspec < 1) || (portspec > 8))
     {
      return 0;   
     }

   if (openport[portspec] < 0)
     {
      return 0;  
     }

   ioctl(openport[portspec], TIOCMGET, &serstat); /* Get all status bits */

   /* mimic the UART modem status register (upper 4 bits only) */
   if (!(serstat & TIOCM_CD))
     {
      dcd = 0;         
     }
   if (!(serstat & TIOCM_RI))
     {
      ri = 0;         
     }
   if (!(serstat & TIOCM_DSR))
     {
      dsr = 0;         
     }
   if (!(serstat & TIOCM_CTS))
     {
      cts = 0;         
     }
     
   return (dcd | ri | dsr | cts);
  }


void set_com_dtrrts(int portspec, int dtr, int rts)  /* set DTR and RTS state */
  {
   int sercmd;
     
   if ((portspec < 1) || (portspec > 8))
     {
      return;   
     }

   if (openport[portspec] < 0)
     {
      return;  
     }

   /* save last set state of DTR and RTS for no-effort read of status */
   holddtr[portspec] = dtr;  
   holdrts[portspec] = rts;  

   if (dtr)
     {
      sercmd = TIOCM_DTR;
      ioctl(openport[portspec], TIOCMBIS, &sercmd); /* Set the DTR pin */
     }
   else
     {
      sercmd = TIOCM_DTR;
      ioctl(openport[portspec], TIOCMBIC, &sercmd); /* Clear the DTR pin */
     }  

   if (rts)
     {
      sercmd = TIOCM_RTS;
      ioctl(openport[portspec], TIOCMBIS, &sercmd); /* Set the RTS pin */
     }
   else
     {
      sercmd = TIOCM_RTS;
      ioctl(openport[portspec], TIOCMBIC, &sercmd); /* Clear the RTS pin */
     }  
  }



/*  ----- NOTES:  From glibc help files (termios structure):

   Noncanonical mode offers special parameters called MIN and TIME for controlling 
   whether and how long to wait for input to be available. You can even use them 
   to avoid ever waiting—to return immediately with whatever input is available, 
   or with no input.
   
   The MIN and TIME are stored in elements of the c_cc array, which is a member 
   of the struct termios structure. Each element of this array has a particular role, 
   and each element has a symbolic constant that stands for the index of that 
   element. VMIN and VMAX are the names for the indices in the array of the 
   MIN and TIME slots.
   
   — Macro: int VMIN
   
       This is the subscript for the MIN slot in the c_cc array. Thus, 
       termios.c_cc[VMIN] is the value itself.
   
       The MIN slot is only meaningful in noncanonical input mode; it specifies 
       the minimum number of bytes that must be available in the input queue in 
       order for read to return. 
   
   — Macro: int VTIME
   
       This is the subscript for the TIME slot in the c_cc array. Thus, 
       termios.c_cc[VTIME] is the value itself.
   
       The TIME slot is only meaningful in noncanonical input mode; it specifies 
       how long to wait for input before returning, in units of 0.1 seconds. 
   
   The MIN and TIME values interact to determine the criterion for when read 
   should return; their precise meanings depend on which of them are nonzero. 
   There are four possible cases:
   
       (examine only one relevant case)
   
       * Both MIN and TIME are zero.
   
         In this case, read always returns immediately with as many characters 
         as are available in the queue, up to the number requested. 
         If no input is immediately available, read returns a value of zero.


   ---  From <termios.h> -----------------------
        
   Baud Rate Selection
      The input and output baud rates are stored in the termios structure. 
      These are the valid values for objects of type speed_t. The following 
      values are defined, but not all baud rates need be supported by the 
      underlying hardware.
      
      B0
          Hang up 
      B50
          50 baud 
      B75
          75 baud 
      B110
          110 baud 
      B134
          134.5 baud 
      B150
          150 baud 
      B200
          200 baud 
      B300
          300 baud 
      B600
          600 baud 
      B1200
          1200 baud 
      B1800
          1800 baud 
      B2400
          2400 baud 
      B4800
          4800 baud 
      B9600
          9600 baud 
      B19200
          19200 baud 
      B38400
          38400 baud 
      B57600
          57600 baud 
      B115200
          115200 baud 
      
   Control Modes
      The c_cflag field describes the hardware control of the terminal; 
      not all values specified are required to be supported by the 
      underlying hardware:
      
      CSIZE
          Character size:
      
          CS5
              5 bits. 
          CS6
              6 bits. 
          CS7
              7 bits. 
          CS8
              8 bits. 
      
      CSTOPB
          Send two stop bits, else one.
      
      CREAD
          Enable receiver.
      
      PARENB
          Parity enable.
      
      PARODD
          Odd parity, else even.
      
      HUPCL
          Hang up on last close.
      
      CLOCAL
          Ignore modem status lines.
      
   Local Modes
      The c_lflag field of the argument structure is used to control various 
      terminal functions:
      
      ECHO
          Enable echo. 
      ECHOE
          Echo erase character as error-correcting backspace. 
      ECHOK
          Echo KILL. 
      ECHONL
          Echo NL. 
      ICANON
          Canonical input (erase and kill processing). 
      IEXTEN
          Enable extended input character processing. 
      ISIG
          Enable signals. 
      NOFLSH
          Disable flush after interrupt or quit. 
      TOSTOP
          Send SIGTTOU for background output. 
      XCASE
          Canonical upper/lower presentation (LEGACY). 
   
------------------------------------------------------------------------- */



int open_com_dtrrts(int portspec, int ratespec, int bits, int parity,
                    int stopbits, int dtr, int rts)

  {
   int baud = B9600;
   int outsize = CS8;
   
   if ((portspec < 1) || (portspec > 8))
     {
      return FALSE;
     }

   port_data_avail[portspec] = FALSE;
   port_data[portspec] = FALSE;

   if (openport[portspec] >= 0)  /* if already open... */
     {
      return FALSE;                    
     }

   /* prepare valid baudrates */
   switch (ratespec)
     {
      case 300:
        {
         baud = B300;  
         break;  
        } 
      case 600:
        {
         baud = B600;  
         break;  
        } 
      case 1200:
        {
         baud = B1200;  
         break;  
        } 
      case 2400:
        {
         baud = B2400;  
         break;  
        } 
      case 4800:
        {
         baud = B4800;  
         break;  
        } 
      case 9600:
        {
         baud = B9600;  
         break;  
        } 
      case 19200:
        {
         baud = B19200;  
         break;  
        } 
      case -1:  
      case 38400:
        {
         baud = B38400;  
         break;  
        }       
      case -2:
      case 57600:
        {
         baud = B57600;  
         break;  
        }     
      case -3:  
      case 115200:
        {
         baud = B115200;  
         break;  
        }      
      }

   /* prepare valid character sizes */
   switch (bits)
     {
      case 8:
        {
         outsize = CS8;  
         break;  
        } 
      case 7:
        {
         baud = CS7;  
         break;  
        } 
      case 6:
        {
         baud = CS6;  
         break;  
        } 
      case 5:
        {
         baud = CS5;  
         break;  
        } 
     }

   /* open the device to be non-blocking (read will return immediatly) */
   openport[portspec] = open(portname[portspec], O_RDWR | O_NOCTTY | O_NONBLOCK);
   if (openport[portspec] < 0) 
     {
      return FALSE;
     }
  
   /* install the signal handler before making the device asynchronous */
   switch (portspec)
     {
      case 1:
        {
         saio[portspec].sa_handler = sig_handler1;
         break;  
        }    
      case 2:
        {
         saio[portspec].sa_handler = sig_handler2;
         break;  
        }    
      case 3:
        {
         saio[portspec].sa_handler = sig_handler3;
         break;  
        }    
      case 4:
        {
         saio[portspec].sa_handler = sig_handler4;
         break;  
        }    
      case 5:
        {
         saio[portspec].sa_handler = sig_handler5;
         break;  
        }    
      case 6:
        {
         saio[portspec].sa_handler = sig_handler6;
         break;  
        }    
      case 7:
        {
         saio[portspec].sa_handler = sig_handler7;
         break;  
        }    
      case 8:
        {
         saio[portspec].sa_handler = sig_handler8;
         break;  
        }    
     }
   
   sigemptyset(&(saio[portspec].sa_mask));
/*   saio[portspec].sa_mask = 0; */
   saio[portspec].sa_flags = 0;

   saio[portspec].sa_restorer = NULL;
   sigaction(SIGIO,&(saio[portspec]),NULL);
    
   /* allow the process to receive SIGIO */
   fcntl(openport[portspec], F_SETOWN, getpid());

   /* SET UP NONCANONICAL ASYNCHRONOUS I/O (raw mode) */

   /* Make the file descriptor asynchronous (the manual page says only 
     O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
   fcntl(openport[portspec], F_SETFL, FASYNC);
  
   tcgetattr(openport[portspec],&(oldtio[portspec])); /* save current port settings */

   /* set most port settings for NON-canonical input processing */
   cfmakeraw(&(newtio[portspec]));   /* easy way to set RAW SERIAL attributes */

   newtio[portspec].c_cflag = newtio[portspec].c_cflag | baud | 
                                                 outsize | CLOCAL | CREAD;
   newtio[portspec].c_oflag = 0;
   newtio[portspec].c_lflag = 0;

   /* set parity and stop bits */
   if (stopbits == 2)    /* default to 1 stop bit, 2 means 2 stop bits */
     {
      newtio[portspec].c_cflag = newtio[portspec].c_cflag | CSTOPB;
     } 
   
   if (parity == 1)    /* default to no parity, 1 means odd, 2 even */
     {
      newtio[portspec].c_cflag = newtio[portspec].c_cflag | PARENB | PARODD;
     } 
   
   if (parity == 2)    /* default to no parity, 1 means odd, 2 even */
     {
      newtio[portspec].c_cflag = newtio[portspec].c_cflag | PARENB;
     } 
   
   /* disable BREAK on this port */
   newtio[portspec].c_iflag = newtio[portspec].c_iflag | IGNBRK;
   
   /* set up timing to force immediate character reads one at a time -- 
                                            see notes above */
   newtio[portspec].c_cc[VMIN] = 0;
   newtio[portspec].c_cc[VTIME] = 0;
   
   tcflush(openport[portspec], TCIOFLUSH);
   tcsetattr(openport[portspec],TCSANOW,&(newtio[portspec]));
   
   wait_flag[portspec] = TRUE;
   
   /* set the initial DTR and RTS states */
   set_com_dtrrts(portspec,dtr,rts);
   return TRUE;
  }   


int open_com(int portspec, int ratespec, int bits, int parity, int stopbits)
  {
   return open_com_dtrrts(portspec,ratespec,bits,parity,stopbits,1,1);
  }
  
/*-----------------------------------------------------------------------------
      If the ring buffer indexes are not equal then ReadCom returns the
 char from either the COM1: or COM2: port.  The character is read from the
 ring buffer and is stored in the FUNCTION result.
-----------------------------------------------------------------------------*/

char read_com(int portspec)

  {
   if ((portspec < 1) || (portspec > 8))
     {
      return 0;
     }   

   if (openport[portspec] < 0)
     {
      return 0;   /* if port not valid, default to NULL char */ 
     }
        
   if ((wait_flag[portspec] == FALSE) &&
       (port_data_avail[portspec] == 0))
     /* if IO signal received (data is in driver buffer but not read yet)
                 and no data has been test-read yet, read it now... */
     { 
      /* the port has been set up with MIN = 0 and TIME = 0, so a read will return
      immediately with either 0 or N characters however, the wait_flag was set
      FALSE by the signal handler -- need to determine if a prior SUCCESSFUL read 
      attempt was made, indicating data already in the buffer */

      /* port_data_avail[portspec] will be at least 1 if prior read succeeded */

      if (port_data_avail[portspec] == 0)  /* if haven't read yet... */
        {
         port_data_avail[portspec] = read(openport[portspec],buf[portspec],1); 
        }
     }
   wait_flag[portspec] = TRUE;            /* reset signal indicator */

   /* the above section ensures an IO signal gets processed at least once -- the next 
      forces a read attempt in case the caller didn't wait for the signal */
   if (port_data_avail[portspec] == 0)  /* if haven't read yet... */
     {
      port_data_avail[portspec] = read(openport[portspec],buf[portspec],1); 
     }
     
   /* read should have returned a 0 or a 1, with none, or a single raw char
      in the buffer */

   if (port_data_avail[portspec] == 0)
     {
      return 0;    /* pretend a NULL was received */ 
     }

   buf[portspec][1] = 0;   /* limit buffer to max 1 char strings */
   port_data_avail[portspec] = 0;  /* reset to force read next time */
   
   return buf[portspec][0];    /* return only 1 char */
  }


/*----------------------------------------------------------------------------
      This procedure outputs directly to the communications port the
 character to be sent.
----------------------------------------------------------------------------*/

void write_com(int portspec, char ch)

  {
   char tbuf[4];
   int res;
     
   if ((portspec < 1) || (portspec > 8))
     {
      return;    
     }   

   if (openport[portspec] < 0)
     {
      return; 
     }

   tbuf[0] = ch;
   tbuf[1] = 0;
   
   /* BUG FIX 20110206 GLF:  Need to actually USE return value of write().
      In tests, fast streams of writes to port without the fix below caused 
      characters to drop out.  The fix below repeatedly retries if error. */

   /* If at first you don't succeed, write, write again (easiest way I know 
      to check "port busy" -- GLF) */
   res = -1;
   while (res < 0)
     {
      res = write(openport[portspec],tbuf,1); 
     }

   return; 
  }      


/*---------------------------------------------------------------------------
      When the interrupt routine is called because of a com port interrupt
 the head index is incremented by one,  but does not increment the tail
 index.  This causes the two indexes to be unequal,  and ModemInput to
 become true.
---------------------------------------------------------------------------*/

int avail_com(int portspec)

  {
   if ((portspec < 1) || (portspec > 8))
     {
      return FALSE;
     }   

   if (openport[portspec] < 0)
     {
      return FALSE; 
     }
        
   if (port_data_avail[portspec] != 0) /* already got buffered data, 
                                          haven't pulled it yet 
                                          (shouldn't happen) */
     {
      return TRUE;                              
     }
           
   if (wait_flag[portspec] == FALSE) /* received IO signal, 
                                          haven't read to buffer yet */
     {
      return TRUE;                              
     }
           
   /* no prior read was successful, no signal returned -- do an asynchronous 
      poll at this time -- if not, you might skip multiple characters if signal 
      was set several times but left unread */

   /* the port has been set up with MIN = 0 and TIME = 0, so a read will return
      immediately with either 0 or N characters */

   port_data_avail[portspec] = read(openport[portspec],buf[portspec],1); 

   /* should either return a 0 or a 1, with none, or a single raw char
         in the buffer */

   if (port_data_avail[portspec] == 0)
     {
      return FALSE;    /* no data available yet */ 
     }
     
   buf[portspec][1]=0;

   return TRUE;
  }

#endif  /* ifndef GFTIOLIN___ (and assume Linux) */



#endif  /* ifdef __MINGW32__ */
#endif  /* ifdef __TURBOC__ */

