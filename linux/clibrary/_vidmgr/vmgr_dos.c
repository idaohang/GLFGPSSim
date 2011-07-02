/* +++Date last modified: 05-Jul-1997 */

/*
 *  VMGR_DOS.C  VidMgr module for MS-DOS compilers.  Release 1.3.
 *
 *  This module written in March 1996 by Andrew Clarke and released to the
 *  public domain.  Last modified in October 1996.
 */

#if defined(__TURBOC__)

#include <string.h>
#include <dos.h>
#include "vidmgr.h"


/* --- From SNIPPETS OPSYS.C  & OPSYS.H  ----- */
#define OS_MSDOS     0  /* MS-DOS */
#define OS_OS2       1  /* IBM OS/2 */
#define OS_WINDOWS   2  /* Microsoft Windows */
#define OS_DESQVIEW  3  /* Quarterdeck DESQview */
#define OS_DBLDOS    4  /* DoubleDOS */
#define OS_NETWARE   5  /* Novell Netware */

int opsysDetect(void);
void opsysTimeSlice(void);
#ifndef __EMX__
unsigned short opsysGetVideoSeg(unsigned short assumed_base);
#endif

int opsys_id = OS_MSDOS;

int opsysDetect(void)
{
#if defined(__TURBOC__)
    /* detect DESQview */
    _AX = 0x2B01;
    _CX = 0x4445;
    _DX = 0x5351;
    geninterrupt(0x21);
    if (_AL != 0xFF)
    {
        opsys_id = OS_DESQVIEW;
        return opsys_id;
    }

    /* detect OS/2 */
    _AH = 0x30;
    geninterrupt(0x21);
    if (_AL >= 10)
    {
        opsys_id = OS_OS2;
        return opsys_id;
    }

    /* detect Windows */
    _AX = 0x160A;
    geninterrupt(0x2F);
    if (_AX == 0)
    {
        opsys_id = OS_WINDOWS;
        return opsys_id;
    }
#else
    union REGS regs;

    /* detect DESQview */
#if defined(__WATCOMC__) && defined(__386__)
    regs.w.ax = 0x2B01;
    regs.w.cx = 0x4445;
    regs.w.dx = 0x5351;
    int386(0x21, &regs, &regs);
#else
    regs.x.ax = 0x2B01;
    regs.x.cx = 0x4445;
    regs.x.dx = 0x5351;
#ifdef __EMX__
    _int86(0x21, &regs, &regs);
#else
    int86(0x21, &regs, &regs);
#endif
#endif
    if (regs.h.al != 0xFF)
    {
        opsys_id = OS_DESQVIEW;
        return opsys_id;
    }

    /* detect OS/2 */
    regs.h.ah = 0x30;
#if defined(__WATCOMC__) && defined(__386__)
    int386(0x21, &regs, &regs);
#else
#ifdef __EMX__
    _int86(0x21, &regs, &regs);
#else
    int86(0x21, &regs, &regs);
#endif
#endif
    if (regs.h.al >= 10)
    {
        opsys_id = OS_OS2;
        return opsys_id;
    }

    /* detect Windows */
#if defined(__WATCOMC__) && defined(__386__)
    regs.w.ax = 0x160A;
    int386(0x2F, &regs, &regs);
    if (regs.w.ax == 0)
    {
        opsys_id = OS_WINDOWS;
        return opsys_id;
    }
#else
    regs.x.ax = 0x160A;
#ifdef __EMX__
    _int86(0x2F, &regs, &regs);
#else
    int86(0x2F, &regs, &regs);
#endif
    if (regs.x.ax == 0)
    {
        opsys_id = OS_WINDOWS;
        return opsys_id;
    }
#endif
#endif

    /* must be MS-DOS */
    opsys_id = OS_MSDOS;
    return opsys_id;
}

void opsysTimeSlice(void)
{
#if defined(__TURBOC__)
    switch (opsys_id)
    {
    case OS_MSDOS:
        geninterrupt(0x28);
        break;
    case OS_OS2:
    case OS_WINDOWS:
        _AX = 0x1680;
        geninterrupt(0x2F);
        break;
    case OS_DESQVIEW:
        _AX = 0x1000;
        geninterrupt(0x15);
        break;
    case OS_DBLDOS:
        _AX = 0xEE01;
        geninterrupt(0x21);
        break;
    case OS_NETWARE:
        _BX = 0x000A;
        geninterrupt(0x7A);
        break;
    default:
        break;
    }
#else
    union REGS regs;
    switch (opsys_id)
    {
    case OS_MSDOS:
#if defined(__WATCOMC__) && defined(__386__)
        int386(0x28, &regs, &regs);
#else
#ifdef __EMX__
        _int86(0x28, &regs, &regs);
#else
        int86(0x28, &regs, &regs);
#endif
#endif
        break;

    case OS_OS2:
    case OS_WINDOWS:
#if defined(__WATCOMC__) && defined(__386__)
        regs.w.ax = 0x1680;
        int386(0x2F, &regs, &regs);
#else
        regs.x.ax = 0x1680;
#ifdef __EMX__
        _int86(0x2F, &regs, &regs);
#else
        int86(0x2F, &regs, &regs);
#endif
#endif
        break;

    case OS_DESQVIEW:
#if defined(__WATCOMC__) && defined(__386__)
        regs.w.ax = 0x1000;
        int386(0x15, &regs, &regs);
#else
        regs.x.ax = 0x1000;
#ifdef __EMX__
        _int86(0x15, &regs, &regs);
#else
        int86(0x15, &regs, &regs);
#endif
#endif
        break;

    case OS_DBLDOS:
#if defined(__WATCOMC__) && defined(__386__)
        regs.w.ax = 0xEE01;
        int386(0x21, &regs, &regs);
#else
        regs.x.ax = 0xEE01;
#ifdef __EMX__
        _int86(0x21, &regs, &regs);
#else
        int86(0x21, &regs, &regs);
#endif
#endif
        break;

    case OS_NETWARE:
#if defined(__WATCOMC__) && defined(__386__)
        regs.w.bx = 0x000A;
        int386(0x7A, &regs, &regs);
#else
        regs.x.bx = 0x000A;
#ifdef __EMX__
        _int86(0x7A, &regs, &regs);
#else
        int86(0x7A, &regs, &regs);
#endif
#endif
        break;

    default:
        break;
    }
#endif
}

#ifndef __EMX__

unsigned short opsysGetVideoSeg(unsigned short assumed_base)
{
    if (opsys_id == OS_DESQVIEW)
    {
#if defined(__TURBOC__)
        _ES = assumed_base;
        _DI = 0;
        _AX = 0xFE00;
        geninterrupt(0x10);
        return _ES;
#else
        union REGS regs;
        struct SREGS sregs;
        sregs.es = assumed_base;
#if defined(__WATCOMC__) && defined(__386__)
        regs.w.di = 0;
        regs.w.ax = 0xFE00;
        int386x(0x10, &regs, &regs, &sregs);
#else
        regs.x.di = 0;
        regs.x.ax = 0xFE00;
        int86x(0x10, &regs, &regs, &sregs);
#endif
        return sregs.es;
#endif
    }
    return assumed_base;
}

#endif

/* ----------------------- */

#if defined(__POWERC) || (defined(__TURBOC__) && !defined(__BORLANDC__)) || \
  (defined(__ZTC__) && !defined(__SC__))
#define FAR far
#else
#if defined(__MSDOS__) || defined(MSDOS) || defined(DOS)
#define FAR _far
#else
#define FAR
#endif
#endif

#ifndef MK_FP
#define MK_FP(seg,off) \
  ((void FAR *)(((unsigned long)(seg) << 16)|(unsigned)(off)))
#endif

static int vm_iscolorcard(void);
static char vm_getscreenmode(void);
static void vm_setscreenmode(char mode);
static void vm_setcursorsize(char start, char end);
static void vm_getcursorsize(char *start, char *end);
static void vm_getkey(unsigned char *chScan, unsigned char *chChar);

void vm_init(void)
{
    vm_getinfo(&vm_startup);
    vm_setattr(vm_startup.attr);
    opsysDetect();
}

void vm_done(void)
{
    if (vm_getscreenmode() != vm_startup.mode)
    {
        vm_setscreenmode(vm_startup.mode);
    }
    vm_setcursorsize(vm_startup.cur_start, vm_startup.cur_end);
}

void vm_getinfo(struct vm_info *v)
{
    v->ypos = vm_wherey();
    v->xpos = vm_wherex();
    v->attr = vm_getattrxy(v->xpos, v->ypos);
    v->mode = vm_getscreenmode();
    v->height = vm_getscreenheight();
    v->width = vm_getscreenwidth();
    vm_getcursorsize(&v->cur_start, &v->cur_end);
}

static int vm_iscolorcard(void)
{
    return vm_getscreenmode() != 7;
}

#if defined(__WATCOMC__) && defined(__386__)

char *vm_screenptr(char x, char y)
{
    char *ptr;
    if (vm_iscolorcard())
    {
        ptr = (char *)(opsysGetVideoSeg(0xB800) << 4);
    }
    else
    {
        ptr = (char *)(opsysGetVideoSeg(0xB000) << 4);
    }
    return ptr + (y * vm_getscreenwidth() * 2) + (x * 2);
}

#else

char FAR *vm_screenptr(char x, char y)
{
    char FAR *ptr;
    if (vm_iscolorcard())
    {
        ptr = (char FAR *)MK_FP(opsysGetVideoSeg(0xB800), 0x0000);
    }
    else
    {
        ptr = (char FAR *)MK_FP(opsysGetVideoSeg(0xB000), 0x0000);
    }
    return ptr + (y * vm_getscreenwidth() * 2) + (x * 2);
}

#endif

static char vm_getscreenmode(void)
{
#if defined(__WATCOMC__) && defined(__386__)
    return *((char *)0x0449);
#else
    return *((char FAR *)MK_FP(0x0040, 0x0049));
#endif
}

static void vm_setscreenmode(char mode)
{
#if defined(__TURBOC__)
    _AH = 0x00;
    _AL = mode;
    geninterrupt(0x10);
#else
    union REGS regs;
    regs.h.ah = 0x00;
    regs.h.al = mode;
#if defined(__WATCOMC__) && defined(__386__)
    int386(0x10, &regs, &regs);
#else
    int86(0x10, &regs, &regs);
#endif
#endif
}

char vm_getscreenwidth(void)
{
#if defined(__WATCOMC__) && defined(__386__)
    return *((char *)0x044a);
#else
    return *((char FAR *)MK_FP(0x0040, 0x004a));
#endif
}

char vm_getscreenheight(void)
{
#if defined(__WATCOMC__) && defined(__386__)
    return (char)(*((char *)0x0484) + 1);
#else
    return (char)(*((char FAR *)MK_FP(0x0040, 0x0084)) + 1);
#endif
}

short vm_getscreensize(void)
{
#if defined(__WATCOMC__) && defined(__386__)
    return *((short *)0x044c);
#else
    return *((short FAR *)MK_FP(0x0040, 0x004c));
#endif
}

void vm_gotoxy(char x, char y)
{
#if defined(__TURBOC__)
    _AH = 0x02;
    _BH = 0;
    _DH = y - 1;
    _DL = x - 1;
    geninterrupt(0x10);
#else
    union REGS regs;
    regs.h.ah = 0x02;
    regs.h.bh = 0;
    regs.h.dh = (unsigned char)(y - 1);
    regs.h.dl = (unsigned char)(x - 1);
#if defined(__WATCOMC__) && defined(__386__)
    int386(0x10, &regs, &regs);
#else
    int86(0x10, &regs, &regs);
#endif
#endif
}

char vm_wherex(void)
{
#if defined(__WATCOMC__) && defined(__386__)
    return (char)(*((char *)0x0450) + 1);
#else
    return (char)(*((char FAR *)MK_FP(0x0040, 0x0050)) + 1);
#endif
}

char vm_wherey(void)
{
#if defined(__WATCOMC__) && defined(__386__)
    return (char)(*((char *)0x0451) + 1);
#else
    return (char)(*((char FAR *)MK_FP(0x0040, 0x0051)) + 1);
#endif
}

static void vm_setcursorsize(char start, char end)
{
#if defined(__TURBOC__)
    _AH = 0x01;
    _CH = start;
    _CL = end;
    geninterrupt(0x10);
#else
    union REGS regs;
    regs.h.ah = 0x01;
    regs.h.ch = start;
    regs.h.cl = end;
#if defined(__WATCOMC__) && defined(__386__)
    int386(0x10, &regs, &regs);
#else
    int86(0x10, &regs, &regs);
#endif
#endif
}

static void vm_getcursorsize(char *start, char *end)
{
#if defined(__TURBOC__)
    _AH = 0x03;
    _BH = 0;
    geninterrupt(0x10);
    *start = _CH;
    *end = _CL;
#else
    union REGS regs;
    regs.h.ah = 0x03;
    regs.h.bh = 0;
#if defined(__WATCOMC__) && defined(__386__)
    int386(0x10, &regs, &regs);
#else
    int86(0x10, &regs, &regs);
#endif
    *start = regs.h.ch;
    *end = regs.h.cl;
#endif
}

int vm_kbhit(void)
{
    static unsigned short counter = 0;
    if (counter % 10 == 0)
    {
        opsysTimeSlice();
    }
    counter++;
#if defined(_MSC_VER) || defined(_QC) || defined(__SC__)
    _asm
    {
    	mov    ah,0x01
    	int    16h
    	jz     nokey
    }
    return 1;
nokey:
    return 0;
#elif defined(__TURBOC__)
    _AH = 0x01;
    geninterrupt(0x16);
    return !(_FLAGS & 0x40);
#else
    {
        union REGPACK regpack;
        memset(&regpack, 0, sizeof regpack);
        regpack.h.ah = 0x01;
        intr(0x16, &regpack);
        return !(regpack.x.flags & 0x40);
    }
#endif
}

static void vm_getkey(unsigned char *chScan, unsigned char *chChar)
{
#if defined(__TURBOC__)
    _AH = 0x00;
    geninterrupt(0x16);
    *chScan = _AH;
    *chChar = _AL;
#else
    union REGS regs;
    regs.h.ah = 0x00;
#if defined(__WATCOMC__) && defined(__386__)
    int386(0x16, &regs, &regs);
#else
    int86(0x16, &regs, &regs);
#endif
    *chScan = regs.h.ah;
    *chChar = regs.h.al;
#endif
}

int vm_getch(void)
{
    unsigned char chChar, chScan;

    while (!vm_kbhit())
    {
        /* nada */
    }

    vm_getkey(&chScan, &chChar);
    if (chChar == 0xe0)
    {
        if (chScan)
        {
            chChar = 0;         /* force scan return */
        }
        else
        {                       /* get next block */
            chChar = 0;

            vm_getkey(&chScan, &chChar);
            if (!chScan)
            {                   /* still no scan? */
                chScan = chChar;  /* move new char over */
                chChar = 0;     /* force its return */
            }
            else
            {
                chChar = 0;     /* force new scan */
            }
        }
    }
    if (chScan == 0xe0)
    {
        if (!chChar)
        {
            chScan = 0;

            vm_getkey(&chScan, &chChar);
            if (!chScan)
            {                   /* still no scan? */
                chScan = chChar;  /* move new char over */
                chChar = 0;     /* force its return */
            }
            else
            {
                chChar = 0;     /* force new scan */
            }
        }
        else
        {
            chScan = 0;         /* handle 0xe00d case */
        }
    }
    if (chChar)
    {
        chScan = 0;
    }

    return (int)((chScan << 8) + (chChar));
}

void vm_setcursorstyle(int style)
{
    if (vm_iscolorcard())
    {
        switch (style)
        {
        case CURSORHALF:
            vm_setcursorsize(4, 7);
            break;
        case CURSORFULL:
            vm_setcursorsize(0, 7);
            break;
        case CURSORNORM:
            vm_setcursorsize(7, 8);
            break;
        case CURSORHIDE:
            vm_setcursorsize(32, 32);
            break;
        default:
            break;
        }
    }
    else
    {
        switch (style)
        {
        case CURSORHALF:
            vm_setcursorsize(8, 13);
            break;
        case CURSORFULL:
            vm_setcursorsize(0, 13);
            break;
        case CURSORNORM:
            vm_setcursorsize(11, 13);
            break;
        case CURSORHIDE:
            vm_setcursorsize(32, 32);
            break;
        default:
            break;
        }
    }
}

char vm_getchxy(char x, char y)
{
    char FAR *p;
    p = vm_screenptr((char)(x - 1), (char)(y - 1));
    return *p;
}

char vm_getattrxy(char x, char y)
{
    char FAR *p;
    p = vm_screenptr((char)(x - 1), (char)(y - 1));
    return *(p + 1);
}

void vm_xgetchxy(char x, char y, char *attr, char *ch)
{
    char FAR *p;
    p = vm_screenptr((char)(x - 1), (char)(y - 1));
    *ch = *p;
    *attr = *(p + 1);
}

void vm_putch(char x, char y, char ch)
{
    char FAR *p;
    p = vm_screenptr((char)(x - 1), (char)(y - 1));
    *p = ch;
}

void vm_puts(char x, char y, char *str)
{
    char FAR *p;
    p = vm_screenptr((char)(x - 1), (char)(y - 1));
    while (*str)
    {
        *p++ = *str++;
        p++;
    }
}

void vm_xputch(char x, char y, char attr, char ch)
{
    char FAR *p;
    p = vm_screenptr((char)(x - 1), (char)(y - 1));
    *p = ch;
    *(p + 1) = attr;
}

void vm_xputs(char x, char y, char attr, char *str)
{
    char FAR *p;
    p = vm_screenptr((char)(x - 1), (char)(y - 1));
    while (*str)
    {
        *p++ = *str++;
        *p++ = attr;
    }
}

void vm_putattr(char x, char y, char attr)
{
    char FAR *p;
    p = vm_screenptr((char)(x - 1), (char)(y - 1));
    *(p + 1) = attr;
}

void vm_paintclearbox(char x1, char y1, char x2, char y2, char attr)
{
    char x, y;
    for (y = y1; y <= y2; y++)
    {
        for (x = x1; x <= x2; x++)
        {
            vm_xputch(x, y, attr, ' ');
        }
    }
}

void vm_paintbox(char x1, char y1, char x2, char y2, char attr)
{
    char x, y;
    for (y = y1; y <= y2; y++)
    {
        for (x = x1; x <= x2; x++)
        {
            vm_putattr(x, y, attr);
        }
    }
}

void vm_clearbox(char x1, char y1, char x2, char y2)
{
    char x, y;
    for (y = y1; y <= y2; y++)
    {
        for (x = x1; x <= x2; x++)
        {
            vm_putch(x, y, ' ');
        }
    }
}

void vm_fillbox(char x1, char y1, char x2, char y2, char ch)
{
    char x, y;
    for (y = y1; y <= y2; y++)
    {
        for (x = x1; x <= x2; x++)
        {
            vm_putch(x, y, ch);
        }
    }
}

void vm_gettext(char x1, char y1, char x2, char y2, char *dest)
{
    char x, y;
    for (y = y1; y <= y2; y++)
    {
        for (x = x1; x <= x2; x++)
        {
            vm_xgetchxy(x, y, dest + 1, dest);
            dest += 2;
        }
    }
}

void vm_puttext(char x1, char y1, char x2, char y2, char *srce)
{
    char x, y;
    for (y = y1; y <= y2; y++)
    {
        for (x = x1; x <= x2; x++)
        {
            vm_xputch(x, y, *(srce + 1), *srce);
            srce += 2;
        }
    }
}

void vm_horizline(char x1, char x2, char row, char attr, char ch)
{
    char x;
    for (x = x1; x <= x2; x++)
    {
        vm_xputch(x, row, attr, ch);
    }
}

#endif
