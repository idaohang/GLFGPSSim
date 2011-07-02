/* gftiomgw.c -- MinGW32 Windows specific support routines for gftermio.c

          DO NOT USE THIS MODULE DIRECTLY -- it is included within conditional
          code in gftermio.c!
          
          -- adapted to C by Gary Flispart 1988-2008
          original routines were written in Turbo Pascal and derive from
          files downloaded from Compuserve Information Service on 24 Feb 87.
          The following header comments are included as acknowledgement:

          TITLE : Interrupt handler
          PRODUCT : Turbo Pascal
          VERSION : All
          OS : Ms DOS 1.00 or greater
          DATE : 2/1/85

          Written by,
          Jim McCarthy
          Technical Support
          Borland International

        and
          Andy Batony
          Teleware Incorporated

------------------------------------------------------------------------- */

#ifndef __TURBOC__

/* The MinGW version of GFTERMIO uses Windows API calls (as opposed to direct 
   hardware I/O used in the Turbo C DOS version).  COM1 thru COM4 are supported
   simultaneously.
*/
   
#ifndef GFTIOMGW___
#define GFTIOMGW___

#include "gflib.h"
#include <windows.h>

/* input and output buffer size (each) */
#define BUFFSIZE 6000

#define INTENREG   1
#define INTIDREG   2
#define LINECTRL   3
#define MODEMCTRL  4
#define LINESTAT   5
#define MODEMSTAT  6


/* assume a MinGW Windows port */
/* ---------------------------------------------------------------------
   The following opens or reopens a single com port
   --------------------------------------------------------------------- */

static HANDLE openport[5] =
  {
   INVALID_HANDLE_VALUE,
   INVALID_HANDLE_VALUE,
   INVALID_HANDLE_VALUE,
   INVALID_HANDLE_VALUE,
   INVALID_HANDLE_VALUE
  };   

static int port_data_avail[5] = 
  {
   FALSE,
   FALSE,
   FALSE,
   FALSE,
   FALSE
  };

static int port_data[5] = 
  {
   0,0,0,0,0
  };   

static const char *comname[] = 
  {
   "COM1:",  /* don't use, but make sense */
   "COM1:",  
   "COM2:",  
   "COM3:",  
   "COM4:"  
  };   

void close_com(int portspec)

  {
   if ((portspec < 1) || (portspec > 4))
     {
      portspec = 1;   /* default to COM1 */
     }

   if (openport[portspec] != INVALID_HANDLE_VALUE)
     {
      /* clear any associated buffer */
      port_data_avail[portspec] = FALSE;
      port_data[portspec] = FALSE;
      CloseHandle(openport[portspec]);
      openport[portspec] = INVALID_HANDLE_VALUE;
     }
  }


unsigned int status_com(int portspec)
  {
   unsigned int comport;
   int statreg;
   DWORD dwModemStatus;

   if ((portspec < 1) || (portspec > 4))
     {
      portspec = 1;   /* default to COM1 */
     }

   if (openport[portspec] == INVALID_HANDLE_VALUE)
     {
      return FALSE;   /* if port not valid, suggest no char available */
     }

   /* emulate modem status register with WIN32 calls */
   /* return inportb(comport+MODEMSTAT); */    /* Get the modem status */

   if (!GetCommModemStatus(openport[portspec], &dwModemStatus))
     {
      /* Error in GetCommModemStatus */
      return 0;
     }

   /* the following ignores status change flags and only returns flag states */
   statreg = 0;
   if (MS_RLSD_ON & dwModemStatus)
     {
      statreg = 0x80;             /* DCD */
     }
   if (MS_RING_ON & dwModemStatus)
     {
      statreg = statreg & 0x40;   /* RI */
     }
   if (MS_DSR_ON & dwModemStatus)
     {
      statreg = statreg & 0x20;   /* DSR */
     }
   if (MS_CTS_ON & dwModemStatus)
     {
      statreg = statreg & 0x10;   /* CTS */
     }

   return statreg;
  }

void set_com_dtrrts(int portspec, int dtr, int rts)  /* set DTR and RTS state */
  {
   unsigned int comport;
   int statreg;

   statreg = 0;

   if ((portspec < 1) || (portspec > 4))
     {
      portspec = 1;   /* default to COM1 */
     }

   if (openport[portspec] == INVALID_HANDLE_VALUE)
     {
      return;   /* if port not valid, suggest no char available */
     }

   /* manipulate modem control register with WIN32 calls */

   if (dtr)
     {
      EscapeCommFunction(openport[portspec],SETDTR);
     }
   else
     {
      EscapeCommFunction(openport[portspec],CLRDTR);
     }

   if (rts)
     {
      EscapeCommFunction(openport[portspec],SETRTS);
     }
   else
     {
      EscapeCommFunction(openport[portspec],CLRRTS);
     }

   return;
  }



int open_com_dtrrts(int portspec, int ratespec, int bits, int parity,
                    int stopbits, int dtr, int rts)

  {
   DCB dcb;
   COMMTIMEOUTS CommTimeouts;

   int baud;
   int sb;

   if ((portspec < 1) || (portspec > 4))
     {
      portspec = 1;   /* default to COM1 */
     }

   port_data_avail[portspec] = FALSE;
   port_data[portspec] = FALSE;

   if (openport[portspec] == INVALID_HANDLE_VALUE)
     {
      openport[portspec] = CreateFile(comname[portspec],
                                      GENERIC_READ | GENERIC_WRITE,
                                      0,NULL,OPEN_EXISTING,0,NULL);
      if (openport[portspec] == INVALID_HANDLE_VALUE)
        {
         /* port didn't open */
         return FALSE;
        }
     }   
   else
     {  
      /* if port already open, clear it and reset baudrate, etc. */   
      port_data_avail[portspec] = FALSE;
      port_data[portspec] = FALSE;
     }     

   if (GetCommState(openport[portspec], &dcb))
     {
      /* make ratespec compatible with old GLF TERMIO routines */  
      if (ratespec > 0)
        {
         baud = ratespec;  /* assume is a valid baudrate */
        }
      else
        {  
         if (ratespec == 0)
           {
            baud = 9600;       /* 0 --> 9600 baud */
           }     
         else
           {
            baud = 38400;        /* if negative, make 38400 baud the default */
            if (ratespec == -2)
               baud = 57600;     /* 57600 baud */
            if (ratespec == -3)
               baud = 115200;     /* 115200 baud */
           }      
        }

      if ((bits < 4) || (bits > 8))
        {
         bits = 8;  
        }      

      switch (stopbits)
        {
         case 1:
           {
            sb = 0;  
            break;  
           }        
         case 2:
           {
            sb = 2;  
            break;
           }        
         case 3:
           {
            sb = 1;   /* 1.5 stopbits */
            break;  
           }        
         default:
           {
            sb = 0;  
           }
        }     
        
      if ((parity < 0) || (parity > 4))
        {
         parity = 0;  
        }
        
      dcb.BaudRate = baud;
      dcb.fBinary = 1;          /* 1=binary mode, no EOF check  */

      if (parity > 0)
        {
         dcb.Parity = parity;   /* 0-4=no,odd,even,mark,space     */
         dcb.fParity = 1;       /* 1=enable parity checking       */
        }   
      else
        {
         dcb.Parity = 0;        /* 0-4=no,odd,even,mark,space     */
         dcb.fParity = 0;       /* 1=enable parity checking       */
        }     

      dcb.fOutX = 0;            /* 1=XON/XOFF out flow control    */
      dcb.fInX = 0;             /* 1=XON/XOFF in flow control     */
      dcb.fNull = 0;            /* 1=enable null stripping        */
      dcb.fAbortOnError = 1;    /* 1=abort reads/writes on error  */

      dcb.ByteSize = bits;      /* number of bits/byte, 4-8       */
      dcb.StopBits = sb;        /* 0,1,2 = 1, 1.5, 2 */

      if (dtr)
        {
         dcb.fDtrControl = DTR_CONTROL_ENABLE;  /* DTR - Dome Direction ON */
        }
      else
        {  
         dcb.fDtrControl = DTR_CONTROL_DISABLE; /* DTR - Dome Direction OFF */
        } 

      if (rts)
        {
         dcb.fRtsControl = RTS_CONTROL_ENABLE;  /* RTS - Dome MOTOR ON */
        }
      else
        {  
         dcb.fRtsControl = RTS_CONTROL_DISABLE; /* RTS - Dome MOTOR OFF */
        } 
      
      if (SetCommState(openport[portspec],&dcb))
        {
         if (!SetupComm(openport[portspec],
                       BUFFSIZE,   
                       BUFFSIZE)) 
           {
            CloseHandle(openport[portspec]);
            openport[portspec] = INVALID_HANDLE_VALUE;
            return FALSE;
           }   
         
         /* the following sets up to allow ReadFile() to test for buffer empty 
            because it returns immediately whether character is received or not */ 
         CommTimeouts.ReadIntervalTimeout = MAXDWORD; 
         CommTimeouts.ReadTotalTimeoutMultiplier = 0; 
         CommTimeouts.ReadTotalTimeoutConstant = 0;
         CommTimeouts.WriteTotalTimeoutMultiplier = 0; 
         CommTimeouts.WriteTotalTimeoutConstant = 0;
   
         if (!SetCommTimeouts(openport[portspec],&CommTimeouts))
           {
            CloseHandle(openport[portspec]);	
            openport[portspec] = INVALID_HANDLE_VALUE;
            return FALSE;
           }     

         /* port is open and ready to read/write */  
         return TRUE;
        }   
     	else
     	  {
         /* error setting DCB -- close port */
     	  }   
     }
   else
     {
      /* error reading DCB -- close port */
     }   

   CloseHandle(openport[portspec]);	
   openport[portspec] = INVALID_HANDLE_VALUE;
   return FALSE;
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
   char dat;
   char readbuf[2];
   DWORD bytesread;
     
   if ((portspec < 1) || (portspec > 4))
     {
      portspec = 1;   /* default to COM1 */   
     }   

   if (openport[portspec] == INVALID_HANDLE_VALUE)
     {
      return 0;   /* if port not valid, default to NULL char */ 
     }
        
   /* pull first from one-byte "last read" buffer, then look for more */
   if (port_data_avail[portspec])
     {
      dat = port_data[portspec];
      port_data_avail[portspec] = FALSE;
      port_data[portspec] = 0;
      return dat;
     }
   else
     {
      /* see if a char is available in file buffer -- timeouts should 
         ensure that this function will return immediately regardless */
      if (ReadFile(openport[portspec],&readbuf,1,&bytesread,NULL))
        {
         if (bytesread == 1)
           {  
            return readbuf[0];  
           }   
        }
     }      
   
   /* either this is an error or no char exists */
   return 0;
  }


/*----------------------------------------------------------------------------
      This procedure outputs directly to the communications port the
 character to be sent.
----------------------------------------------------------------------------*/

void write_com(int portspec, char ch)

  {
   char wr_buf[2];
   DWORD byteswritten;
     
   if ((portspec < 1) || (portspec > 4))
     {
      portspec = 1;   /* default to COM1 */   
     }   

   if (openport[portspec] == INVALID_HANDLE_VALUE)
     {
      return;   /* if port not valid, ignore */ 
     }

   wr_buf[0] = ch;
   wr_buf[1] = 0;     
   if (WriteFile(openport[portspec],wr_buf,1,&byteswritten,NULL))
	  {
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
   char readbuf[2];
   DWORD bytesread;

   /* the port timeouts have been intentionally set to force ReadFile
      to return immediately if no data exists in the buffer */
   if ((portspec < 1) || (portspec > 4))
     {
      portspec = 1;   /* default to COM1 */   
     }   

   if (openport[portspec] == INVALID_HANDLE_VALUE)
     {
      return FALSE;   /* if port not valid, suggest no char available */ 
     }
   
   /* pull first from one-byte "last read" buffer, then look for more */
   if (port_data_avail[portspec])
     {
      return TRUE;
     }
   else
     {
      /* see if a char is available in file buffer -- timeouts should 
         ensure that this function will return immediately regardless */
      if (ReadFile(openport[portspec],&readbuf,1,&bytesread,NULL))
        {
         if (bytesread == 1)
           {
            port_data[portspec] = readbuf[0];  
            port_data_avail[portspec] = TRUE;    
            return TRUE;  
           }   
        }
     }      
   
   /* either this is an error or no char exists */
   return FALSE;
  }

#endif  /* ifndef GFT_MINGW */

#endif  /* ifndef __TURBOC__ */

