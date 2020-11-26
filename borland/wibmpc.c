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


#if	COLOR

int PASCAL NEAR tcapbfcol(color)	/* set the current output color */
	unsigned int color;
{ cbcolor = ctrans[color>>8];
  cfcolor = ctrans[color & 0xf];
}
#endif


void PASCAL NEAR tcapmove(row, col)
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


int PASCAL NEAR millisleep(n)
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


int PASCAL NEAR tcapbeeol(row, col)	/* erase to the end of the line */
         int  row, col;
{		/* build the word and setup the screen pointer */
  unsigned int ch = (((cbcolor << 4) | cfcolor) << 8) | A_SP;

  unsigned int *lnptr = scptr(row)+col;
  register short i;
  for (i = term.t_ncol-col; --i >= 0; )
    *lnptr++ = ch;
}


void PASCAL NEAR tcapeeol()	/* erase to the end of the line */

{ tcapbeeol(ttrow, ttcol);		
}


#if 0

int PASCAL NEAR addb(n)

{ tcapbeep();
}

#endif



#define VS_SCROLL_UP(n) (0x600+(n)) /* # of lines to scroll by */

#if 0

int PASCAL NEAR ttputc(ch) /* put a character at the current position in the
		   		current colors */
	int ch;
{			/* if its a newline, we have to move the cursor */
#define VS_READ_POS 0x300
  biosfn(VS_READ_POS, 0x10);
  ttcol = rg.h.dl;
  ttrow = rg.h.dh;

  if (ch != '\n' && ch != '\r')
  { if (ch != '\b')
      ttcol += 1;
    else
    { ttcol -= 1;
      if (ttcol < 0)
        ttcol = 0;
    }
  }
  else
  {
#if 0
    if (ttrow == term.t_nrowm1)
    { /* tcapmove(20, 0);		/* we must scroll the screen */
      adb(ttrow);
#if 0
      rg.h.bh = cfcolor; /* attribute for blank line */
      rg.x.cx = 0;	/* upper left corner of scroll */
      rg.x.dx = (term.t_nrowm1 << 8) + term.t_ncol - 1; /* lower right */
      biosfn(VS_SCROLL_UP(1), 0x10);
      --ttrow;
#endif
    }
#endif
    tcapmove(ttrow+1, 0);
    return 0;
  }

				/* write char to screen with current attrs */
  rg.x.bx = cfcolor;
  biosfn(14*256+ch, 0x10);
}

#endif



void PASCAL NEAR tcapeeop()

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


void PASCAL NEAR tcapscreg(row1, row2)
     int row1, row2;
{ sctop = row1;
  scbot = row2;
}


void PASCAL NEAR ttscupdn(n)                  /* direction the window moves */
        int   n;   
{ int * t;
           int nabs = n;
  register int norz = n;
  if (norz < 0)
  { nabs = -norz;
    norz = 0;
  }
  /*
  if (TRUE || flickcode && (dtype == CDCGA))
    awaitgap();
  */ 
  t = &scptr(ttrow)[ttcol];
  *t &= ~HIL_CUR;
  memmove(scptr(sctop+norz),  scptr(sctop+nabs-norz), 
           		(scbot - sctop + 1 - nabs) * NCOL * 2);

  *t |= HIL_CUR;
}



int PASCAL NEAR tcapcres(res) /* change screen resolution */
	char *res;	/* resolution to change to */
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

int PASCAL NEAR spal()	/* reset the pallette registers */

{
  /* nothin here now..... */
}

#endif




int PASCAL NEAR tcapbeep()
{
#if	MWC
	ttputc(BEL);
#else
	bdos(6, BEL, 0);
#endif
}

void PASCAL NEAR tcapopen()
{
/*  if (gflags & MD_NO_MMI)
    return; */
  scinit(CDSENSE);
  screxist = TRUE;
  revexist = TRUE;
  vtinit();
}



/*
 * This function gets called just before we go back home to the command
 * interpreter.
 */
int PASCAL NEAR tcapclose()

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

int PASCAL NEAR tcapkopen()	/* open the keyboard */

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


/*int PASCAL NEAR tcapkclose() ** close the keyboard **

{
}*/



int PASCAL NEAR scinit(type) /* initialize the screen head pointers */
	int type;	/* type of adapter to init for */
{ 
	/* if asked...find out what display is connected */
  if (type == CDSENSE)
    type = getboard();
				  /* if we have nothing to do....don't do it */
  if (dtype == type)
    return TRUE;

  cmt_colour = 0x71;	/* 0x80:blink,  0x8:intense */
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
int PASCAL NEAR getboard()

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


static int PASCAL NEAR egaclose()

{			/* put the beast into 80 column mode */
  biosfn(3, 16);
}



static int PASCAL NEAR egaopen() /* init the computer to EGA or VGA */

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



int PASCAL NEAR scwrite(row, outstr, color)	/* write a line out */
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
int PASCAL NEAR fnclabel(f, n)	/* label a function key */
	int f,n;	/* default flag, numeric argument [unused] */
{		/* on machines with no function keys...don't bother */
  return TRUE;
}
#endif

