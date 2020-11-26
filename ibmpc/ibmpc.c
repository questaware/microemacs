/*
 * The routines in this file provide support for the IBM-PC and other
 * compatible terminals. It goes directly to the graphics RAM to do
 * screen output. It compiles into nothing if not an IBM-PC driver
 * Supported monitor cards include CGA, MONO and EGA.
 */

#define	termdef	1			/* don't define "term" external */

#include        <stdio.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include        "epredef.h"
#include	"elang.h"

extern char sres[];		/* current screen resolution	*/


#define C_WHITE 7
#define C_BLACK 0

#define HIL_CUR 0x8800

extern union REGS rg;			/* cpu register for use of DOS calls */

#define NROW	50			/* Max Screen size.		*/
#define NCOL    80                      /* Edit if you want to.         */
#define	MARGIN	8			/* size of minimim margin and	*/
#define	SCRSIZ	64			/* scroll size for extended lines */
#define BEL     0x07                    /* BEL character.               */
#define ESC     0x1B                    /* ESC character.               */
#define	SPACE	32			/* space character		*/

#define	SCADC	0xb8000000L		/* CGA address of screen RAM	*/
#define	SCADM	0xb0000000L		/* MONO address of screen RAM	*/
#define SCADE	0xb8000000L		/* EGA/VGA address of screen RAM*/

#define MONOCRSR 0x0B0D			/* monochrome cursor		*/
#define CGACRSR 0x0607			/* CGA cursor			*/
#define EGACRSR 0x0709			/* EGA/VGA cursor		*/

#define	CDCGA	0			/* color graphics card		*/
#define	CDMONO	1			/* monochrome text card		*/
#define	CDEGA	2			/* EGA color adapter		*/
#define	CDVGA	3			/* VGA color adapter		*/
#define	CDSENSE	9			/* detect the card type		*/


int dtype = -1;				/* current display type		*/
const char drvname[][4] =		/* screen resolution names	*/
       { "CGA", "MONO", "EGA", "VGA"};
       
long   scadd;				/* address of screen ram	*/
#define scptr(i) ((int*)(scadd + NCOL * 2 * i))


static short sctop = 0;		/* zero based */
static short scbot = 25;  	/* zero based */ /* set by tcapopen */


int	cfcolor = C_WHITE;		/* current forground color */
int	cbcolor = C_BLACK;		/* current background color */
const int ctrans[] =		/* ansi to ibm color translation table */
	{0, 4, 2, 6, 1, 5, 3, 7,
	 8, 12, 10, 14, 9, 13, 11, 15};
/* editor variable: cmt_colour;		   comment colour in ibm */

					/* screen initialisation code */
static void Pascal NEAR biosfn(ax, n)
	int ax;
	int  n;
{ rg.x.ax = ax;
  int86(n, &rg, &rg);
}



#if	COLOR

void Pascal NEAR tcapbfcol(color)	/* set the current output color */
	unsigned int color;
{ cbcolor = ctrans[color>>8];
  cfcolor = ctrans[color & 0xf];
}
#endif


void Pascal NEAR tcapmove(row, col)
	int  row, col;
{/* if (row == ttrow and col == ttcol)
      return; */
  scptr(ttrow)[ttcol] &= ~HIL_CUR;
  if (row < term.t_nrowm1)
    scptr(row)[col] |= HIL_CUR;
  ttrow = row;
  rg.h.dh = row;
  ttcol = col;
  rg.h.dl = col;
/*if (row < 0 || row > 24)
      addb(5);
*/
  rg.h.bh = 0;		/* set screen page number */
#define  SCPFNCODE 0x200		/* set cursor position function code */
  biosfn(SCPFNCODE, 0x10);
}


int Pascal NEAR millisleep(n)
	unsigned int n;
{ long ct = (long)n << 6; 

  while (--ct > 0)
 		/* wait for vertical retrace to be off */
  { 
    if ((inp(0x3da) & 8))
      ;
#if 0
	       /* and to be back on */
    while ((inp(0x3da) & 8) == 0)
      ;
#endif
  }
}			


int Pascal NEAR tcapbeeol(row, col)	/* erase to the end of the line */
         int  row, col;
{		/* build the word and setup the screen pointer */
  unsigned int ch = (((cbcolor << 4) | cfcolor) << 8) | SPACE;

  unsigned int *lnptr = scptr(row)+col;
  register short i;
  for (i = term.t_ncol-col; --i >= 0; )
    *lnptr++ = ch;
}


void Pascal NEAR tcapeeol()	/* erase to the end of the line */

{ tcapbeeol(ttrow, ttcol);		
}


#if 0

int Pascal NEAR addb(n)

{ tcapbeep();
}

#endif



#define VS_SCROLL_UP(n) (0x600+(n)) /* # of lines to scroll by */

#if 0
X
Xint Pascal NEAR ttputc(ch) /* put a character at the current position in the
X		   		current colors */
X	int ch;
X{			/* if its a newline, we have to move the cursor */
X#define VS_READ_POS 0x300
X  biosfn(VS_READ_POS, 0x10);
X  ttcol = rg.h.dl;
X  ttrow = rg.h.dh;
X
X  if (ch != '\n' && ch != '\r')
X  { if (ch != '\b')
X      ttcol += 1;
X    else
X    { ttcol -= 1;
X      if (ttcol < 0)
X        ttcol = 0;
X    }
X  }
X  else
X  {
X#if 0
X    if (ttrow == term.t_nrowm1)
X    { /* tcapmove(20, 0);		/* we must scroll the screen */
X      adb(ttrow);
X#if 0
X      rg.h.bh = cfcolor; /* attribute for blank line */
X      rg.x.cx = 0;	/* upper left corner of scroll */
X      rg.x.dx = (term.t_nrowm1 << 8) + term.t_ncol - 1; /* lower right */
X      biosfn(VS_SCROLL_UP(1), 0x10);
X      --ttrow;
X#endif
X    }
X#endif
X    tcapmove(ttrow+1, 0);
X    return 0;
X  }
X
X				/* write char to screen with current attrs */
X  rg.x.bx = cfcolor;
X  biosfn(14*256+ch, 0x10);
X}
X
#endif



void Pascal NEAR tcapeeop()

#if 1
{ 
  rg.x.cx = 0;		/* upper left corner of scroll */
  rg.x.dx = (term.t_nrowm1 << 8) | (term.t_ncol - 1);
				/* lower right corner of scroll */
#if  COLOR
  rg.h.bh = ((ctrans[gbcolor] & 15)<<4)|(ctrans[gfcolor] & 15);
#else
  rg.h.bh = 7;
#endif
					/* # = 0 lines to scroll (clear it) */
  biosfn(VS_SCROLL_UP(0), 0x10);
}
#else
{ register int row;
/* awaitgap(); */
  for (row = term.t_nrowm1; --row >= 0; )
    tcapbeeol(row, 0);
}
#endif


void Pascal NEAR tcapscreg(row1, row2)
     int row1, row2;
{ sctop = row1;
  scbot = row2;
}


void Pascal NEAR ttscupdn(n)                  /* direction the window moves */
        int   n;   
{ int * t;
           int nabs = n;
  register int norz = n;
  if (norz < 0)
  { nabs = -norz;
    norz = 0;
  }
  t = &scptr(ttrow)[ttcol];
  *t &= ~HIL_CUR;
  memmove(scptr(sctop+norz),  scptr(sctop+nabs-norz), 
           		(scbot - sctop + 1 - nabs) * NCOL * 2);

  *t |= HIL_CUR;
}



int Pascal NEAR tcapcres(char * res) /* change screen resolution */

{
  register int i;

  for (i = upper_index(drvname)+1; --i >= 0; )
    if (*(long*)res == *(long*)drvname[i])
    { scinit(i);
      return TRUE;
    }
    
  return FALSE;
}


#if 1

int Pascal NEAR spal()	/* reset the pallette registers */

{
  /* nothin here now..... */
}

#endif




int Pascal NEAR tcapbeep()
{
#if	MWC
	ttputc(BEL);
#else
	bdos(6, BEL, 0);
#endif
}

void Pascal NEAR tcapopen()
{
/*  if (gflags & MD_NO_MMI)
    return; */
  scinit(CDSENSE);
  screxist = TRUE;
  revexist = TRUE;
}



/*
 * This function gets called just before we go back home to the command
 * interpreter.
 */
int Pascal NEAR tcapclose()

{
/* if (gflags & MD_NO_MMI)
    return 0; */
#if	COLOR
  tcapbfcol(7);
#endif
	/* if we had the EGA open... close it */
  if (dtype == CDEGA || dtype == CDVGA)
    egaclose();

#if (HP150 == 0) & LATTICE
  rg.h.ah = 0x33;		/* control-break check dos call */
  rg.h.al = 1;		/* set the current state */
  rg.h.dl = 1;		/* set it ON */
  intdos(&rg, &rg);
#endif
}

#if 0

int Pascal NEAR tcapkopen()	/* open the keyboard */

{
/*  if (gflags & MD_NO_MMI)
    return 0; */
#if 0
  rg.x.ax = 0x3300;	/* read CTRL+C check status */
  rg.h.dl = 0x00;
  intdos(&rg, &rg);
  if (rg.h.dl)
    tcapbeep();
#endif
#if 0
  rg.x.ax = 0x3301;	/* set CTRL+C check status */
  rg.x.dx = 0x00;	/* OFF */
  intdos(&rg, &rg);
#endif
}

#endif


/*int Pascal NEAR tcapkclose() ** close the keyboard **

{
}*/



static int Pascal NEAR scinit(type) /* initialize the screen head pointers */
	int type;	/* type of adapter to init for */
{ 
	/* if asked...find out what display is connected */
  if (type == CDSENSE)
    type = getboard();
				  /* if we have nothing to do....don't do it */
  if (dtype == type)
    return TRUE;

/*  cmt_colour = 0x71;	** 0x80:blink,  0x8:intense */
  dtype = type;
  *(long*)sres = *(long*)drvname[dtype];/* reset $sres environment variable*/
			      /* and set up the various parameters as needed */
  switch (dtype)
  { default:
    case CDMONO: scadd = SCADM;		/* Monochrome adapter */
		 cfcolor = 0x7;
		 cbcolor = 0;
		 cmt_colour = 0x7f;	/* 0x80:blink,  0x7 bg_white, */
					/* 0x08:intense,0x7 fg_bright, 0x1 ul */
    when CDCGA:	 scadd = SCADC;		/* Color graphics adapter */
    when CDEGA:	 
    case CDVGA:	 scadd = SCADE;		/* Enhanced graphics adapter */
		 egaopen();
  }

  term.t_mrowm1 = *(char*)0x484;
  if (term.t_mrowm1 >= NROW)
    term.t_mrowm1 = NROW - 1;
  scbot = term.t_nrowm1;

  newdims(term.t_ncol, term.t_mrowm1+1);
#if	MOUSE
  maxlines(term.t_nrowm1+1);
#endif  
  return TRUE;
}


/* getboard:	Determine which type of display board is attached.
		Current known types include:

		CDMONO	Monochrome graphics adapter
		CDCGA	Color Graphics Adapter
		CDEGA	Extended graphics Adapter
		CDVGA	Video Graphics Array

		if MONO	set to MONO
		   CGA	set to CGA	EGAexist = FALSE VGAexist = FALSE
		   EGA	set to CGA	EGAexist = TRUE  VGAexist = FALSE
		   VGA	set to CGA	EGAexist = TRUE  VGAexist = TRUE
*/
static int Pascal NEAR getboard()

{ register int type = CDCGA;	/* board type to return */

  biosfn(0, 0x11);
  if ((rg.x.ax & 0x30) == 0x30)
    type = CDMONO;
#if 0
					/* test if EGA present */
  rg.x.bx = 0xff10;			/* If EGA, bh=0-1 and bl=0-3 */
  biosfn(0x1200, 0x10);			
  if (!(rg.x.bx & 0xfefc))	/* Yes, it's EGA */
  {		       /* Adapter says it's an EGA. We'll get the same response
			             from a VGA, so try to tell the two apart */
    biosfn(0x1a00, 0x10);		/* read display combination */
    vgaexist = rg.h.al == 0x1a && (rg.h.bl == 7 || rg.h.bl == 8);
		      /* Function is supported and it's a PS/2 50,60,80 with
  			   	      analog display, so it's VGA (I hope!) */
  }
#endif
  return type;
}


static int Pascal NEAR egaclose()

{			/* put the beast into 80 column mode */
  biosfn(3, 16);
}



static int Pascal NEAR egaopen() /* init the computer to EGA or VGA */

{			/* put the beast into EGA 43/VGA 50 line mode */
  egaclose();

  rg.h.bl = 0;		/* block 0                           */
  biosfn(0x1112, 16);	/* set char. generator function code */
			/*  0x12: to 8 by 8 double dot ROM         */

  rg.h.bl = 32;		/* alt. print screen routine         */
  biosfn(0x1200, 16);	/* 0x12: alternate select function code    */
			/* 0x00: clear AL for no good reason       */

			/* set cursor size function code */
  rg.x.cx = 0x0507;	/* turn cursor on code ; was 607 */
  biosfn(0x100, 0x10);
#if 0
  outp(0x3d4, 10);	/* video bios bug patch */
  outp(0x3d5, 6);
#endif
}



int Pascal NEAR scwrite(row, outstr, color)	/* write a line out */
	int    row;		/* row of screen */
	short *outstr;		/* string to output (must be term.t_ncol long)*/
	int    color;		/* background, foreground */
{		    /* build the attribute byte and setup the screen pointer */

  unsigned int attr = color << 8;
  unsigned int attr_ = attr;

  unsigned int *lnptr = scptr(row);
  register int i;

  for (i = -1; ++i < term.t_ncol; )
  { if (outstr[i] & 0x7f00)
    { attr_ = attr;		/* push */
      attr = outstr[i] & 0x7f00;
    }
    *lnptr++ = (outstr[i] & 0xff) | attr;
    if (outstr[i] & 0x8000)	/* pop */
      attr = attr_;
  }
}


#if	FLABEL
int Pascal NEAR fnclabel(f, n)	/* label a function key */
	int f,n;	/* default flag, numeric argument [unused] */
{		/* on machines with no function keys...don't bother */
  return TRUE;
}
#endif

