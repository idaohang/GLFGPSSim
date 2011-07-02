/* gft_tc.c  -- Turbo C 2.0 DOS specific support routines for gftermio.c

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

#ifdef __TURBOC__

/* The Turbo C DOS version of GFTERMIO is limited to COM1 thru COM4 only,
   but up to two can be open simltaneously (prior TERMIO was limited to 
   1 port, but allowed for COM1 thru COM4).  COM1 and COM3 may not be open 
   simultaneously, neither may COM2 and COM4 (same IRQs used).
   DOS and hardware I/O calls and interrupts are used.
*/   

#ifndef GFTIOTC___
#define GFTIOTC___

#include "gflib.h"
#include <dos.h>

#define BUFFSIZE 6000

#define BITS5 0
#define BITS6 1
#define BITS7 2
#define BITS8 3
#define STOPBIT1 0
#define STOPBIT2 4
#define NOPARITY 0
#define PARITY 8
#define EVENPARITY 16
#define DTRTRUE 1
#define RTSTRUE 2
#define BIT3TRUE 8


#define IRQ4       0x0C
#define IRQ3       0x0B
#define EOI        0x20
#define COM1BASE   0x03F8
#define COM2BASE   0x02F8
#define COM3BASE   0x03E8
#define COM4BASE   0x02E8
#define INTENREG   1
#define INTIDREG   2
#define LINECTRL   3
#define MODEMCTRL  4
#define LINESTAT   5
#define MODEMSTAT  6

static int com_bullet_proofed = 0;


int com1_triggered = 0;  /* 1 signals other interrupt processes data is in */

static void interrupt (*holdvector1)();

static int com1_is_open = 0;
static int com1port = COM1BASE;
static char intbuffer1[BUFFSIZE];
static int head1 = 0;
static int tail1 = 0;
static char holdreadchar1 = 0;
/* MODEMCTRL is a write-only register -- retain "last set" for DTR and RTS */
static int holddtr1 = 0;
static int holdrts1 = 0;

int com2_triggered = 0;  /* 1 signals other interrupt processes data is in */

static void interrupt (*holdvector2)();

static int com2_is_open = 0;
static int com2port = COM2BASE;
static char intbuffer2[BUFFSIZE];
static int head2 = 0;
static int tail2 = 0;
static char holdreadchar2 = 0;
/* MODEMCTRL is a write-only register -- retain "last set" for DTR and RTS */
static int holddtr2 = 0;
static int holdrts2 = 0;

int com3_triggered = 0;  /* 1 signals other interrupt processes data is in */

static void interrupt (*holdvector3)();

static int com3_is_open = 0;
static int com3port = COM3BASE;
static char intbuffer3[BUFFSIZE];
static int head3 = 0;
static int tail3 = 0;
static char holdreadchar3 = 0;
/* MODEMCTRL is a write-only register -- retain "last set" for DTR and RTS */
static int holddtr3 = 0;
static int holdrts3 = 0;

int com4_triggered = 0;  /* 1 signals other interrupt processes data is in */

static void interrupt (*holdvector4)();

static int com4_is_open = 0;
static int com4port = COM4BASE;
static char intbuffer4[BUFFSIZE];
static int head4 = 0;
static int tail4 = 0;
static char holdreadchar4 = 0;
/* MODEMCTRL is a write-only register -- retain "last set" for DTR and RTS */
static int holddtr4 = 0;
static int holdrts4 = 0;

/* -------------------------------------------------------------------------
      inthandlerN() is the interrupt handler for the numbered comport COMn:
 Registers are automatically saved and restored by the compiler due to the
 "interrupt" keyword in the function declaration.
 -------------------------------------------------------------------------- */

static void interrupt inthandler1() /*int bp, int di, int si, int ds,
                          int es, int dx, int cx, int bx,
                          int ax, int ip, int cs, int flags) */

/*  NOTE: This interrupt handler refers to variables outside of its
          local environment, and must use far references.  For this reason,
          this code requires compilation under LARGE memory model */

{
 register int tbyte;

 tbyte = inportb(com1port);                /* Get the character in the port */
 inportb(com1port + LINESTAT);             /* Get the status of the port    */
 if ( head1 < BUFFSIZE )                   /* Check bounds of the ring      */
    head1++;                               /* buffer,  and if smaller then  */
 else                                     /* increment by one otherwise    */
    head1 = 0;                             /* set to the first element      */
 intbuffer1[head1] = tbyte;                 /* load the buffer w/ the char.  */
 com1_triggered = 1;       /* set to signal other interrupts com data is in */
                          /* -- readcom resets later when buffer is empty  */
 outportb(0x20,0x20);
}


static void interrupt inthandler2() /*int bp, int di, int si, int ds,
                          int es, int dx, int cx, int bx,
                          int ax, int ip, int cs, int flags) */

/*  NOTE: This interrupt handler refers to variables outside of its
          local environment, and must use far references.  For this reason,
          this code requires compilation under LARGE memory model */

{
 register int tbyte;

 tbyte = inportb(com2port);                /* Get the character in the port */
 inportb(com2port + LINESTAT);             /* Get the status of the port    */
 if ( head2 < BUFFSIZE )                   /* Check bounds of the ring      */
    head2++;                               /* buffer,  and if smaller then  */
 else                                     /* increment by one otherwise    */
    head2 = 0;                             /* set to the first element      */
 intbuffer2[head2] = tbyte;                 /* load the buffer w/ the char.  */
 com2_triggered = 1;       /* set to signal other interrupts com data is in */
                          /* -- readcom resets later when buffer is empty  */
 outportb(0x20,0x20);
}



static void interrupt inthandler3() /*int bp, int di, int si, int ds,
                          int es, int dx, int cx, int bx,
                          int ax, int ip, int cs, int flags) */

/*  NOTE: This interrupt handler refers to variables outside of its
          local environment, and must use far references.  For this reason,
          this code requires compilation under LARGE memory model */

{
 register int tbyte;

 tbyte = inportb(com3port);                /* Get the character in the port */
 inportb(com3port + LINESTAT);             /* Get the status of the port    */
 if ( head3 < BUFFSIZE )                   /* Check bounds of the ring      */
    head3++;                               /* buffer,  and if smaller then  */
 else                                     /* increment by one otherwise    */
    head3 = 0;                             /* set to the first element      */
 intbuffer3[head3] = tbyte;                 /* load the buffer w/ the char.  */
 com3_triggered = 1;       /* set to signal other interrupts com data is in */
                          /* -- readcom resets later when buffer is empty  */
 outportb(0x20,0x20);
}


static void interrupt inthandler4() /*int bp, int di, int si, int ds,
                          int es, int dx, int cx, int bx,
                          int ax, int ip, int cs, int flags) */

/*  NOTE: This interrupt handler refers to variables outside of its
          local environment, and must use far references.  For this reason,
          this code requires compilation under LARGE memory model */

{
 register int tbyte;

 tbyte = inportb(com4port);                /* Get the character in the port */
 inportb(com4port + LINESTAT);             /* Get the status of the port    */
 if ( head4 < BUFFSIZE )                   /* Check bounds of the ring      */
    head4++;                               /* buffer,  and if smaller then  */
 else                                     /* increment by one otherwise    */
    head4 = 0;                             /* set to the first element      */
 intbuffer4[head4] = tbyte;                 /* load the buffer w/ the char.  */
 com4_triggered = 1;       /* set to signal other interrupts com data is in */
                          /* -- readcom resets later when buffer is empty  */
 outportb(0x20,0x20);
}



static void setrate(int portspec, int ratespec)

{
 char tclr;
 int tdl = 0;

 int comport = 0;

 if (portspec == 1)
   {
    comport = com1port;
   }

 if (portspec == 2)
   {
    comport = com2port;
   }

 if (portspec == 3)
   {
    comport = com3port;
   }

 if (portspec == 4)
   {
    comport = com4port;
   }

 if (comport == 0)
   {
    return;
   }


 if (ratespec > 0)
    tdl = (int)(115200L / (long)ratespec); /* calculate divisor for rate */
 else
   {
    if (ratespec == 0)
       tdl = 12;       /* 0 --> 9600 baud */
    else
      {
       tdl = 3;        /* if negative, make 38400 baud the default */
       if (ratespec == -2)
          tdl = 2;     /* 57600 baud */
       if (ratespec == -3)
          tdl = 3;     /* 115200 baud */
      }
   }

 outportb(comport+LINECTRL,(tclr = inportb(comport+LINECTRL) | 0x80));
 outport(comport,tdl);
 outportb(comport+LINECTRL,(tclr & 0x7f));
                                           /* Get the Line control register*/
                                           /* Set Divisor Latch Access Bit */
                                           /* in order to access divisor   */
                                           /* latches, then store the      */
                                           /* value for the desired baud  */
                                           /* rate                         */

                                           /* then clear the DLAB in order */
                                           /* to access to the receiver    */
                                           /* buffer                       */
}



static int calcprotocol(int bits, int parity, int stopbits)

  {
   int value;

   switch (bits)
     {
      case 5:
        {
         value = BITS5;
         break;
        }
      case 6:
        {
         value = BITS6;
         break;
        }
      case 7:
        {
         value = BITS7;
         break;
        }
      case 8:
        {
         value = BITS8;
         break;
        }
      default:
        {
         value = BITS8;
        }
     }
   switch (stopbits)
     {
      case 1:
        {
         value += STOPBIT1;
         break;
        }
      case 2:
        {
         value += STOPBIT2;
         break;
        }
      default:
        {
         value += STOPBIT1;
        }
     }
   switch (parity)
     {
      case 0:
        {
         value += NOPARITY;
         break;
        }
      case 1:
        {
         value += PARITY;
         break;
        }
      case 2:
        {
         value += EVENPARITY;
         break;
        }
      case 3:
        {
         value = value + PARITY + EVENPARITY;
         break;
        }
      default:
        {
         value += NOPARITY;
        }
     }
   return value;
  }




/* ---------------------------------------------------------------------
   The following closes a single com port
   --------------------------------------------------------------------- */


static void close_com_single(int portspec)

{
 int dtr,rts;      
 char tbyte;

 int comport = 0;

 if (portspec == 1)
   {
    comport = com1port;
    head1 = 0;
    tail1 = 0;
    com1_triggered = 0;
    if (!com1_is_open)
      {
       return;
      }
    com1_is_open = 0;
    dtr = holddtr1;
    rts = holdrts1;
   }

 if (portspec == 2)
   {
    comport = com2port;
    head2 = 0;
    tail2 = 0;
    com2_triggered = 0;
    if (!com2_is_open)
      {
       return;
      }
    com2_is_open = 0;
    dtr = holddtr2;
    rts = holdrts2;
   }

 if (portspec == 3)
   {
    comport = com3port;
    head3 = 0;
    tail3 = 0;
    com3_triggered = 0;
    if (!com3_is_open)
      {
       return;
      }
    com3_is_open = 0;
    dtr = holddtr3;
    rts = holdrts3;
   }

 if (portspec == 4)
   {
    comport = com4port;
    head4 = 0;
    tail4 = 0;
    com4_triggered = 0;
    if (!com4_is_open)
      {
       return;
      }
    com4_is_open = 0;
    dtr = holddtr4;
    rts = holdrts4;
   }

 if (comport == 0)
   {
    return;
   }

 /* normalize dtr and rts flags to correct bit position -- 
    if already FALSE (0), let be */
 if (dtr)
   {
    dtr = 0x01;     
   }
 if (rts)
   {
    rts = 0x02;
   }
  
 /* NOTE: it is important that the code below to UNDO installation of
          the interrupt handler NOT BE PERFORMED unless it is KNOWN
          that the interrupt handler was actually installed -- hence
          the test for com_is_open above
*/          
 /* MODEMCTRL bit 3 must be set to 1 to enable interrupts on the port */  
 /* turn modem interrupt off here (bit 3 set to 0)                    */ 
 outportb(comport+MODEMCTRL, (dtr | rts));


 disable();                                 /* disable interrupts */
 outportb(comport+INTENREG,0);              /* Disable COM interrupts       */
 if ((portspec == 1) || (portspec == 3))
   {
    setvect(IRQ4,holdvector1);      /* If using COM1: or COM3: restore IRQ4  */
   }
 else /* assume comport2 or comport4 */
   {
    setvect(IRQ3,holdvector2);      /* If using COM2: or COM4: restore IRQ3  */
   }
 outportb(0x21,0);   /* enable all hardware interrupts */
 enable();
}


void close_com(int portspec)

{
/* special case -- if portspec is 0, close all ports -- (usually atexit()) */ 
 if (portspec == 0)
   {
    close_com_single(1);          
    close_com_single(2);          
    close_com_single(3);          
    close_com_single(4);          
    return;
   }
 close_com_single(portspec);          
}



/* ----------------------------------------------------------------------
   The following are error handlers installed when com port is first
   opened to help "bulletproof" a com-port program -- help prevent a program
   from terminating without de-installing the comm interrupt
   ---------------------------------------------------------------------- */

static int hard_err_handler(int errval, int ax, int bp, int si)

  {
   hardretn(0);  /* cause to ignore all hard errors -- count on file open
                    errors to catch missing disks */
  }


static int c_brk_handler(void)

  {
   close_com(0);
   return(0);  /* cause program to be aborted */
  }


static atexit_t com_exit_handler(void)

  {
   close_com(0);
  }



/*-----------------------------------------------------------------------------
      If the ring buffer indexes are not equal then ReadCom returns the
 char from COM1:, COM2:, COM3:, or COM4: port.  The character is read from the
 ring buffer and is stored in the FUNCTION result.
-----------------------------------------------------------------------------*/

char read_com(int portspec)

{
 if (portspec == 1)
   {
    if (!com1_is_open)
      {
       return 0;
      }
    if ( head1 != tail1 )            /* Check for ring buffer character   */
      {
       if ( tail1 < BUFFSIZE )     /* Check the limits of the ring      */
          tail1++;                 /* and set tail accordingly          */
       else
          tail1 = 0;
       holdreadchar1 = intbuffer1[tail1];  /* Get the character                 */
      }

    if (head1 == tail1)     /* if buffer WAS or was JUST MADE empty ... */
      {
       com1_triggered = 0; /* reset to signal other interrupts data is not in   */
                          /* interrupt process sets when data loaded to buffer */
      }

    return holdreadchar1;  /* if ring buffer empty, returns last char found */
   }

 if (portspec == 2)
   {
    if (!com2_is_open)
      {
       return 0;
      }
    if ( head2 != tail2 )            /* Check for ring buffer character   */
      {
       if ( tail2 < BUFFSIZE )     /* Check the limits of the ring      */
          tail2++;                 /* and set tail accordingly          */
       else
          tail2 = 0;
       holdreadchar2 = intbuffer2[tail2];  /* Get the character                 */
      }

    if (head2 == tail2)     /* if buffer WAS or was JUST MADE empty ... */
      {
       com2_triggered = 0; /* reset to signal other interrupts data is not in   */
                          /* interrupt process sets when data loaded to buffer */
      }

    return holdreadchar2;  /* if ring buffer empty, returns last char found */
   }

 if (portspec == 3)
   {
    if (!com3_is_open)
      {
       return 0;
      }
    if ( head3 != tail3 )            /* Check for ring buffer character   */
      {
       if ( tail3 < BUFFSIZE )     /* Check the limits of the ring      */
          tail3++;                 /* and set tail accordingly          */
       else
          tail3 = 0;
       holdreadchar3 = intbuffer3[tail3];  /* Get the character                 */
      }

    if (head3 == tail3)     /* if buffer WAS or was JUST MADE empty ... */
      {
       com3_triggered = 0; /* reset to signal other interrupts data is not in   */
                          /* interrupt process sets when data loaded to buffer */
      }

    return holdreadchar3;  /* if ring buffer empty, returns last char found */
   }

 if (portspec == 4)
   {
    if (!com4_is_open)
      {
       return 0;
      }
    if ( head4 != tail4 )            /* Check for ring buffer character   */
      {
       if ( tail4 < BUFFSIZE )     /* Check the limits of the ring      */
          tail4++;                 /* and set tail accordingly          */
       else
          tail4 = 0;
       holdreadchar4 = intbuffer4[tail4];  /* Get the character                 */
      }

    if (head4 == tail4)     /* if buffer WAS or was JUST MADE empty ... */
      {
       com4_triggered = 0; /* reset to signal other interrupts data is not in   */
                          /* interrupt process sets when data loaded to buffer */
      }

    return holdreadchar4;  /* if ring buffer empty, returns last char found */
   }

 return 0;
}


/*----------------------------------------------------------------------------
      This procedure outputs directly to the communications port the
 character to be sent.
----------------------------------------------------------------------------*/

void write_com(int portspec, char ch)

{
 if (portspec == 1)
   {
    if (!com1_is_open)
      {
       return;
      }
    outportb(com1port,ch);
   }

 if (portspec == 2)
   {
    if (!com2_is_open)
      {
       return;
      }
    outportb(com2port,ch);
   }

 if (portspec == 3)
   {
    if (!com3_is_open)
      {
       return;
      }
    outportb(com3port,ch);
   }

 if (portspec == 4)
   {
    if (!com4_is_open)
      {
       return;
      }
    outportb(com4port,ch);
   }
}


/*---------------------------------------------------------------------------
      When the interrupt routine is called because of a com port interrupt
 the head index is incremented by one,  but does not increment the tail
 index.  This causes the two indexes to be unequal,  and avail_com() to
 become true.
---------------------------------------------------------------------------*/

int avail_com(int portspec)

{
 if (portspec == 1)
   {
    if (!com1_is_open)
      {
       return 0;
      }
    return ((head1 != tail1) ? 1 : 0);
   }

 if (portspec == 2)
   {
    if (!com2_is_open)
      {
       return 0;
      }
    return ((head2 != tail2) ? 1 : 0);
   }

 if (portspec == 3)
   {
    if (!com3_is_open)
      {
       return 0;
      }
    return ((head3 != tail3) ? 1 : 0);
   }

 if (portspec == 4)
   {
    if (!com4_is_open)
      {
       return 0;
      }
    return ((head4 != tail4) ? 1 : 0);
   }
 return 0;
}

/*--------------------------------------------------------------------------*/

unsigned int status_com(int portspec)
  {
   int comport = 0;

   if (portspec == 1)
     {
      comport = com1port;
      if (!com1_is_open)
        {
         return 0;
        }
     }

   if (portspec == 2)
     {
      comport = com2port;
      if (!com2_is_open)
        {
         return 0;
        }
     }

   if (portspec == 3)
     {
      comport = com3port;
      if (!com3_is_open)
        {
         return 0;
        }
     }

   if (portspec == 4)
     {
      comport = com4port;
      if (!com4_is_open)
        {
         return 0;
        }
     }

   if (comport == 0)
     {
      return 0;
     }

   /* Get modem status -- ignores status change flags, returns 
      DCD, RI, DSR, CTS in upper 4 bits */
   return inportb(comport+MODEMSTAT) & 0xF0;  
  }


void set_com_dtrrts(int portspec, int dtr, int rts)  /* set DTR and RTS state */
  {
   int comport = 0;

   if (portspec == 1)
     {                                                                       
      comport = com1port;
      if (!com1_is_open)
        {
         return;
        }
      holddtr1 = dtr;  
      holdrts1 = rts;  
     }

   if (portspec == 2)
     {
      comport = com2port;
      if (!com2_is_open)
        {
         return;
        }
      holddtr2 = dtr;  
      holdrts2 = rts;  
     }

   if (portspec == 3)
     {
      comport = com3port;
      if (!com3_is_open)
        {
         return;
        }
      holddtr3 = dtr;  
      holdrts3 = rts;  
     }

   if (portspec == 4)
     {
      comport = com4port;
      if (!com4_is_open)
        {
         return;
        }
      holddtr4 = dtr;  
      holdrts4 = rts;  
     }

   if (comport == 0)
     {
      return;
     }

   /* normalize dtr and rts flags to correct bit position -- 
      if already FALSE (0), let be */
   if (dtr)
     {
      dtr = 0x01;     
     }
   if (rts)
     {
      rts = 0x02;
     }
  
   /* MODEMCTRL bit 3 must be set to 1 to enable interrupts on the port */  
   outportb(comport+MODEMCTRL, ((dtr | rts) | BIT3TRUE));
  }


/* ---------------------------------------------------------------------
   The following opens or reopens a single com port and sets DTR and RTS

   The following allows up to two ports of COM1, COM2, COM3 or COM4, but may 
   not use more than one per IRQ -- that us, COM1 and COM3 may NOT be open
   simultaneously, likewise COM2 and COM4.
   --------------------------------------------------------------------- */

int open_com_dtrrts(int portspec, int ratespec, int bits, int parity, 
                          int stopbits, int dtr, int rts)

{
 char tbyte;                            /* Temporary byte buffer        */
 int i;                                 /* counter                      */

 int comport = 0;

 if (portspec == 1)
   {
    if (com3_is_open)
       return FALSE;          

    comport = com1port;
    holddtr1 = dtr;  
    holdrts1 = rts;  
   }

 if (portspec == 2)
   {
    if (com4_is_open)
       return FALSE;          

    comport = com2port;
    holddtr2 = dtr;  
    holdrts2 = rts;  
   }

 if (portspec == 3)
   {
    if (com1_is_open)
       return FALSE;          

    comport = com3port;
    holddtr3 = dtr;  
    holdrts3 = rts;  
   }

 if (portspec == 4)
   {
    if (com2_is_open)
       return FALSE;          

    comport = com4port;
    holddtr4 = dtr;  
    holdrts4 = rts;  
   }

 if (comport == 0)
   {
    return FALSE;
   }

 close_com(portspec);  /* cancel previous comm arrangements and reset buffer */

 if (!com_bullet_proofed)
   {
                                          /* protects interrupt code: */
    harderr(hard_err_handler);            /* for missing disks */
    ctrlbrk(c_brk_handler);               /* for ctrl-break */
    atexit((atexit_t)&com_exit_handler);  /* for normal end-of-program */
    com_bullet_proofed = 1;
   }
/* normalize dtr and rts flags to correct bit position -- 
   if already FALSE (0), let be */

 if (dtr)
   {
    dtr = 0x01;     
   }
 if (rts)
   {
    rts = 0x02;
   }

 tbyte = inportb(comport);               /* Read the ports to clear any  */
 tbyte = inportb(comport + LINESTAT);    /* error conditions             */
 setrate(portspec,ratespec);                     /* Set the baud rate            */
 outportb(comport + LINECTRL,calcprotocol(bits,parity,stopbits));
                                        /* Set the protocol            */
                                        
 /* MODEMCTRL bit 3 must be set to 1 to enable interrupts on the port */  
 outportb(comport+MODEMCTRL, ((dtr | rts) | BIT3TRUE));


 disable();
 outportb(comport + INTENREG,1);        /* Enable com port interrupts   */
 if (portspec == 1)
   {
    holdvector1 = getvect(IRQ4);    /* save original vector          */
    setvect(IRQ4,inthandler1);      /* install new interrupt handler */
    com1_is_open = 1;
   }
 if (portspec == 2)
   {
    holdvector2 = getvect(IRQ3);    /* save original vector          */
    setvect(IRQ3,inthandler2);      /* install new interrupt handler */
    com2_is_open = 1;
   }
 if (portspec == 3)
   {
    holdvector3 = getvect(IRQ4);    /* save original vector          */
    setvect(IRQ4,inthandler3);      /* install new interrupt handler */
    com3_is_open = 1;
   }
 if (portspec == 4)
   {
    holdvector4 = getvect(IRQ3);    /* save original vector          */
    setvect(IRQ3,inthandler4);      /* install new interrupt handler */
    com4_is_open = 1;
   }
 outportb(0x21,0);  /* enable all hardware interrupts */
 enable();

/* set_com_dtrrts(portspec,dtr,rts); */
 return TRUE;
}



int open_com(int portspec, int ratespec, int bits, int parity, int stopbits)
  {
   return open_com_dtrrts(portspec,ratespec,bits,parity,stopbits,1,1);
  }




#endif  /* ifndef GFT_TC___ */

#endif  /* ifdef __TURBOC__ */


