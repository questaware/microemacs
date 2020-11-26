/*	MSDOS.C:	Operating specific I/O and Spawning functions
			under the MS/PCDOS operating system
			for MicroEMACS 3.10
*/
#include        <stdio.h>
#include        "estruct.h"
#include        "edef.h"
#include	"etype.h"
#define Short short

#include	"elang.h"
			/* The Mouse driver only works with typeahead defined */
#if	MOUSE
static int mexist;	/* is the mouse driver installed? */
static int nbuttons;	/* number of buttons on the mouse */
static int oldbut;	/* Previous state of mouse buttons */
#endif

#if  TURBO
# include <conio.h>
# include <dir.h>
# include <dos.h>
 struct ffblk fileblock;	/* structure for directory searches */
#endif

extern char *getenv();

#if     LATTICE | MSC | DTL | TURBO | AZTEC | MWC
 union REGS rg;		/* cpu register for use of DOS calls */
 struct SREGS segreg;	/* cpu segment registers	     */
#endif

#if	MSC | TURBO
#include <process.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#if USE_UBAR
#define stat _stat
#define O_RDONLY _O_RDONLY
#endif
#endif

#if 0
/*
sprintf()

{ char ch;
  fputs("printf called", stdout);
  read(0, &ch, 1);
}


fprintf()

{ sprintf();
}


printf()

{ sprintf();
}


char * memcpy(t_, s_, len_)
      char * t_, *s_;
      int    len_;
{ register int len = len_;
  register char * t = t_;
  register char * s = s_;
  
  if (t <= s)
    while (--len >= 0)
      *t++ = *s++;
  else 
  { t = &t[len];
    s = &s[len];
    
    while (--len >= 0)
      *--t = *--s;
  }
  return t_;
}
*/
#endif

#define	IBUFSIZE  4		/* must be a power of 2 */

unsigned char in_buf[IBUFSIZE];	/* input character buffer */
Short in_next = 0;		/* pos to retrieve next input character */
Short in_last = 0;		/* pos to place most recent input character */


#define in_init() in_next = in_last = 0   /* initialize the input buffer */

#define in_check() (in_next != in_last)   /* is the input buffer non-empty? */


void Pascal NEAR in_put(event)
	int event;	/* event to enter into the input buffer */
{ in_buf[in_last & (IBUFSIZE - 1)] = event;
  in_last += 1;
}


#define in_get() in_buf[in_next++ & (IBUFSIZE - 1)]
				/* get an event from the input buffer */

	/*
	 * This function is called once to set up the terminal device streams.
	 */
#if	MOUSE

Pascal NEAR ttopen()

{
/*	if (gflags & MD_NO_MMI)
	  return; */
	long miaddr;	/* mouse interupt routine address */

			/* we are not sure of the initial cursor position */
#if 0
	ttrow = 2;
	ttcol = 2;
#endif

	mexist = FALSE;  /* provisionally */
				/* check if the mouse drive exists first */
	rg.x.ax = 0x3533;	/* look at the interrupt 33 address */
#if	MSC | TURBO | DTL | LATTICE | MWC
	int86x(0x21, &rg, &rg, &segreg);
	miaddr = (((long)segreg.es) << 16) + (long)rg.x.bx;
	if (miaddr == 0 || *(char * far)miaddr == 0xcf)
#elif	AZTEC
	sysint(0x21, &rg, &rg);
	miaddr = (((long)rg.x.es) << 16) + (long)rg.x.bx;
	if (miaddr == 0 || *(char *)miaddr == 0xcf)
#endif
	  return;
				/* and then check for the mouse itself */
	rg.x.ax = 0;		/* mouse status flag */
	int86(0x33, &rg, &rg);	/* check for the mouse interupt */
	nbuttons = rg.x.bx;
	in_init();
	mexist = 0;
	rg.x.dx = 0;		                /* top row */
	if (rg.x.ax == 0)
	  return;
       
        mexist = 1;
		        		/* put it in the upper right corner */
	rg.x.ax = 4;			/* set mouse cursor position */
	rg.x.cx = (term.t_ncol - 1) << 3;	/* last col of display */
	int86(0x33, &rg, &rg);
					/* and set its attributes */
	rg.x.ax = 10;		/* set text cursor */
	rg.x.bx = 0;		/* software text cursor please */
	rg.x.cx = 0x77ff;	/* screen mask */
	rg.x.dx = 0x7700;	/* cursor mask */
	int86(0x33, &rg, &rg);
}


maxlines(lines)		/* set number of vertical rows for mouse */
	int lines;	/* # of vertical lines */
{
	if (mexist)
	{ rg.x.ax = 8;		/* set min/max vertical cursor position */
	  rg.x.cx = 0;		/* start at 0 */
	  rg.x.dx = (lines - 1)<<3; /* end at the end */
	  int86(0x33, &rg, &rg);
	}
}

#endif



#if 1
/*
 * MS-DOS (use the very very raw console output routine).
 */
Pascal NEAR ttputc(char c)

{
#if     MWC
  putcnb(c);
#else
  bdos(6, c, 0);
#endif
}
#endif

#if 0

/*
 * Flush terminal buffer. Does real work where the terminal output is buffered
 * up. A no-operation on systems where byte at a time terminal I/O is done.
 */
Pascal NEAR ttflush()
{
}

#endif

#if 0
X
Xstatic const char exttbl1[] = 
X{ 
X	'Q',
X	'W',
X	'E',
X	'R',
X	'T',  /* 20 */
X	'Y',
X	'U',
X	'I',
X	'O',
X	'P',  /* 25 */
X	26,
X	27,
X	28,
X	29,
X	'A',	/* 30 */
X	'S',
X	'D',
X	'F',
X	'G',
X	'H',
X	'J',
X	'K',
X	'L',	/* 38 */
X	39,
X	40,
X	41,
X	42,
X	43,	
X	'Z',	/* 44 */
X	'X',
X	'C',
X	'V',
X	'B',
X	'N',
X	'M',	/* 50 */
X	51,
X	52,
X	53,
X	54,
X	55,
X	56,
X	57,
X	58,
X};
X
Xstatic const char exttbl2[] = 
X{
X        '<',	/* HOME */
X	'P',	/* cursor up */
X	'Z',	/* page up */
X	74,
X	'B',	/* cursor left */
X	75,
X	'F',	/* cursor right */
X	76,
X	'>',	/* end */
X	'N',	/* cursor down */
X	'V',	/* page down */
X	'C',	/* insert */
X	'D',	/* delete */
X};
X
Xstatic const char exttbl3[] = 
X{     	'B',	/* control left */
X	'F',	/* control right */
X	'>',	/* control END */
X	'V',	/* control page down */
X	'<',	/* control HOME */
X};
X
Xstatic const short exttbl4[] = 
X{	ALTD | 131,
X        SPEC | CTRL | 'Z',	/* control page up */
X      	SPEC | ':',		/* F11 */
X	SPEC | ';',		/* F12 */
X      	SPEC | SHFT | ':',	/* SHFT + F11 */
X	SPEC | SHFT | ';',	/* SHFT + F12 */
X      	SPEC | CTRL | ':',	/* CTRL + F11 */
X	SPEC | CTRL | ';',	/* CTRL + F12 */
X      	SPEC | ALTD | ':',	/* ALT + F11 */
X	SPEC | ALTD | ';',	/* ALT + F12 */
X};
X
Xstatic const char mod10[] = "1234567890";
X
X/*	extcode:	resolve MSDOS extended character codes
X			encoding the proper sequences into emacs
X			printable character specifications
X*/
Xint extcode(c_)
X	unsigned c_;	/* byte following a zero extended char byte */
X{ register unsigned char c = c_;
X
X  if (c < 15)
X    return ALTD | c;
X					/* backtab */
X  if (c == 15)
X    return SHFT | CTRL | 'I';
X					/* ALTed letter keys */ 
X  if (c <= 58)
X    return ALTD | exttbl1[c - 16]; 
X        				/* function keys 1 through 10 */
X  if (c < 69)
X    return SPEC | mod10[c - 59];
X					/* movement keys */
X  if (c < 84)
X    return SPEC | exttbl2[c-71];
X					/* shifted function keys */
X  if (c < 94)
X    return SPEC | SHFT | mod10[c - 84];
X					/* control function keys */
X  if (c < 104)
X    return SPEC | CTRL | mod10[c - 94];
X					/* ALTed function keys */
X  if (c < 114)
X    return SPEC | ALTD | mod10[c - 104];
X					/* control movement keys */
X  if (c < 120)
X    return SPEC | CTRL | exttbl3[c-115];
X					/* ALTed number keys */
X  if (c <= 130)
X    return ALTD | mod10[c - 120];
X					/* F10, F11 keys */  
X  if (c < 141)
X    return exttbl4[c - 131];
X
X  return ALTD | c;
X}
X
#else

static const short exttbl[] = 
{ 
        SHFT | CTRL | 'I',
	ALTD | 'Q',
	ALTD | 'W',
	ALTD | 'E',
	ALTD | 'R',
	ALTD | 'T',  /* 20 */
	ALTD | 'Y',
	ALTD | 'U',
	ALTD | 'I',
	ALTD | 'O',
	ALTD | 'P',  /* 25 */
	ALTD | 26,
	ALTD | 27,
	ALTD | 28,
	ALTD | 29,
	ALTD | 'A',	/* 30 */
	ALTD | 'S',
	ALTD | 'D',
	ALTD | 'F',
	ALTD | 'G',
	ALTD | 'H',
	ALTD | 'J',
	ALTD | 'K',
	ALTD | 'L',	/* 38 */
	ALTD | 39,
	ALTD | 40,
	ALTD | 41,
	ALTD | 42,
	ALTD | 43,	
	ALTD | 'Z',	/* 44 */
	ALTD | 'X',
	ALTD | 'C',
	ALTD | 'V',
	ALTD | 'B',
	ALTD | 'N',
	ALTD | 'M',	/* 50 */
	ALTD | 51,
	ALTD | 52,
	ALTD | 53,
	ALTD | 54,
	ALTD | 55,
	ALTD | 56,
	ALTD | 57,
	ALTD | 58,
	SPEC | '1',
	SPEC | '2',
	SPEC | '3',
	SPEC | '4',
	SPEC | '5',
	SPEC | '6',
	SPEC | '7',
	SPEC | '8',
	SPEC | '9',
	SPEC | '0',
        ALTD | 69,
	ALTD | 70,
        SPEC | '<',	/* HOME */
	SPEC | 'P',	/* cursor up */
	SPEC | 'Z',	/* page up */
	SPEC | 74,
	SPEC | 'B',	/* cursor left */
	SPEC | 75,
	SPEC | 'F',	/* cursor right */
	SPEC | 76,
	SPEC | '>',	/* end */
	SPEC | 'N',	/* cursor down */
	SPEC | 'V',	/* page down */
	SPEC | 'C',	/* insert */
	SPEC | 'D',	/* delete */
	SPEC | SHFT | '1',
	SPEC | SHFT | '2',
	SPEC | SHFT | '3',
	SPEC | SHFT | '4',
	SPEC | SHFT | '5',
	SPEC | SHFT | '6',
	SPEC | SHFT | '7',
	SPEC | SHFT | '8',
	SPEC | SHFT | '9',
	SPEC | SHFT | '0',
	SPEC | CTRL | '1',
	SPEC | CTRL | '2',
	SPEC | CTRL | '3',
	SPEC | CTRL | '4',
	SPEC | CTRL | '5',
	SPEC | CTRL | '6',
	SPEC | CTRL | '7',
	SPEC | CTRL | '8',
	SPEC | CTRL | '9',
	SPEC | CTRL | '0',
	SPEC | ALTD | '1',
	SPEC | ALTD | '2',
	SPEC | ALTD | '3',
	SPEC | ALTD | '4',
	SPEC | ALTD | '5',
	SPEC | ALTD | '6',
	SPEC | ALTD | '7',
	SPEC | ALTD | '8',
	SPEC | ALTD | '9',
	SPEC | ALTD | '0',
	ALTD | 114,
     	SPEC | CTRL | 'B',	/* control left */
	SPEC | CTRL | 'F',	/* control right */
	SPEC | CTRL | '>',	/* control END */
	SPEC | CTRL | 'V',	/* control page down */
	SPEC | CTRL | '<',	/* control HOME */
	ALTD | '1',
	ALTD | '2',
	ALTD | '3',
	ALTD | '4',
	ALTD | '5',
	ALTD | '6',
	ALTD | '7',
	ALTD | '8',
	ALTD | '9',
	ALTD | '0',
	ALTD | 130,
	ALTD | 131,
	ALTD | 132,
      	SPEC | ':',		/* F11 */
	SPEC | ';',		/* F12 */
      	SPEC | SHFT | ':',	/* SHFT + F11 */
	SPEC | SHFT | ';',	/* SHFT + F12 */
      	SPEC | CTRL | ':',	/* CTRL + F11 */
	SPEC | CTRL | ';',	/* CTRL + F12 */
      	SPEC | ALTD | ':',	/* ALT + F11 */
	SPEC | ALTD | ';',	/* ALT + F12 */
        SPEC | CTRL | 'P',	/* ALTD | 0x8d,*/
        ALTD | 0x8e,
        ALTD | 0x8f,
        ALTD | 0x90,
        SPEC | CTRL | 'N',	/* ALTD | 0x91,*/
	SPEC | CTRL | 'C',	/* insert */
	SPEC | CTRL | 'D',	/* delete */
};


#if 0
/*	extcode:	resolve MSDOS extended character codes
			encoding the proper sequences into emacs
			printable character specifications
*/
int extcode(c_)
	unsigned c_;	/* byte following a zero extended char byte */
{ register short c = c_ - 15;

  return c > 147 - 15 ? ALTD | c : exttbl[c];
}

#endif
#endif


#define KBRD 7

#if 1

extern int confd;

static int Pascal NEAR doschar()	/* call the dos to get a char */

{	register unsigned int c;	/* extended character to return */ 

#if S_MSDOS && 0
	if (confd != 0)		/* doesnt work on XP */
	{ char ch;
	  c = read(0, &ch, 1);
	  if (c < 1)
	    return -1;
	  if (ch != 0)
	    return ch && 255;
	  c = read(0, &ch, 1);
	  if (c < 1)
	    return -1;
	  c = ch & 255;
	  c = c > 147 - 15 ? ALTD | c : exttbl[c];
	  in_put(c >> 8);	/* prefix byte */
	  in_put(c & 255);	/* event code byte */
	  return 0;		/* extended escape sequence */
	}
#endif
	rg.h.ah = KBRD;		/* dos Direct Console Input call */
	intdos(&rg, &rg);
	c = rg.h.al & 0xff;
	if (c != 0)
	  return c == 0xdd ? 0x7c : c;

	  				/* function key!! */
	rg.h.ah = KBRD;		/* get the next character */
	intdos(&rg, &rg);
	c = rg.h.al - 15;
	c = c > 147 - 15 ? ALTD | c : exttbl[c];

	in_put(c >> 8);	/* prefix byte */
	in_put(c & 255);	/* event code byte */
	return 0;		/* extended escape sequence */
}

#endif


static int  eaten_char = -1;		 /* Re-eaten char */


void Pascal NEAR reeat(c)
	int	c;
{
    eaten_char = c;			/* save the char for later */
}


/*
 * Read a character from the terminal, performing no editing and doing no echo
 * at all. Also mouse events are forced into the input stream here.
 */
Pascal NEAR ttgetc()

{   register unsigned int c;	/* extended character to return */ 

    if (eaten_char != -1)
    { int c = eaten_char;
      eaten_char = -1;
      return c;
    }

    while (TRUE)			/* return any keystrokes waiting in */
    {   if (in_check())			/* the type ahead buffer */
				  return in_get();
			/* with no mouse, this is a simple get char routine */
			if (
#if MOUSE
			    mexist == FALSE || mouseflag == FALSE
#else
			    1
#endif
#if TYPEAH
			 || typahead()
#endif
	  		)
			{ return doschar();
#if 0
			  rg.h.ah = KBRD;		/* dos Direct Console Input call */
			  intdos(&rg, &rg);
			  c = rg.h.al;
			  if (c != 0)
			    return c & 255;
	  										  /* function key!! */
			  rg.h.ah = KBRD;		/* get the next character */
			  intdos(&rg, &rg);
			  c = rg.h.al - 15;
			  c = c > 147 - 15 ? ALTD | c : exttbl[c];
			  in_put(c >> 8);	/* prefix byte */
			  in_put(c & 255);	/* event code byte */
			  return 0;		/* extended escape sequence */
#endif
			}

#if	MOUSE
			rg.x.ax = 1;	        	/* Show Cursor */
			int86(0x33, &rg, &rg);
				        				/* loop waiting for something to happen */
			while (TRUE)
			{ 
#if TYPEAH
			  if (typahead())
			    break;
#endif
			  if (checkmouse())
			    break;
			}
												/* turn the mouse cursor back off */
			rg.x.ax = 2;			/* Hide Cursor */
			int86(0x33, &rg, &rg);
#endif
    }
}


#if	MOUSE
int checkmouse()

{	register int k;		/* current bit/button of mouse */
	register int event;	/* encoded mouse event */

			  /* check to see if any mouse buttons are different */
	rg.x.ax = 3;	/* Get button status and mouse position */
	int86(0x33, &rg, &rg);

{	int newbut   = rg.x.bx;		    /* new state of the mouse buttons */
	int mousecol = rg.x.cx >> 3;	    /* current mouse column */
	int mouserow = rg.x.dx >> 3;	    /* current mouse row */
				
	rg.h.ah = 2;			/* get the shift key status as well */
	int86(0x16, &rg, &rg);
	
{	int sstate = rg.h.al;

	for (k=1; k != (1 << nbuttons); k = k<<1)
					/* For each button on the mouse */
	  if ((oldbut&k) != (newbut&k))
	  {			/* This button changed, generate an event */
		in_put(0);
		in_put(MOUS >> 8);
		in_put(mousecol);
		in_put(mouserow);
		event = (newbut & k) != 0;	/* up or down? */
		if      (k == 2)		/* center button? */
		  event += 4;
		else if (k == 4)		/* right button? */
		  event += 2;
		if      (sstate & 3)		/* shifted */
		  event += 'A';
		else if (sstate & 4)		/* controled? */
		  event += 1;
		else
		  event += 'a';			/* plain */
		in_put(event);
		oldbut = newbut;
		return TRUE;
	  }

	return FALSE;
}}}
#endif

#if	TYPEAH
	/* typahead:	See if any characters are already in the keyboard buffer
	*/
Pascal NEAR typahead()

{	rg.x.ax = 0x4406;	/* IOCTL input status */
	rg.x.bx = 0;		/* File handle = stdin */
#if	MSC | DTL
	/*int86(0x21,&rg,&rg);*/
	intdos(&rg,&rg);
	return rg.h.al != 0;
#else
#if	LATTICE | AZTEC | TURBO
	return intdos(&rg, &rg) != 0;
#else
	intcall(&rg, &rg, 0x21);
	return rg.x.flags != 0;
#endif
#endif
}
#endif

static int Pascal NEAR usehost(wh, line)
	char 	 wh;
	char *   line;
{
  int cc;
  if (!clexec)
    tcapmove(term.t_nrowm1, 0);           /* Seek to last line.   */
/*  mlwrite(""); */
  tcapclose();
  ttputc('\n');
  /* write(0, "\n", 1);*/
  sgarbf = TRUE;
#if MOUSE == 0
  return shellprog(wh, line);	/* wrapper to execprg() */
#else
  /*if (wh == '!')*/
  cc = shellprog(wh, line);	/* wrapper to execprg() */
#if MOUSE
  ttopen();
#endif
  return cc;
#endif
}


static Pascal NEAR gen_spawn(f, n, prompt)
	char *   prompt;
{          char line [150];
  register int s;

  if (restflag)
    return resterr();
  
  s=mlreply(prompt, line, NLINE);
  if (s != TRUE)
    return s;

  write(1, "\r", 1);

  s = usehost(*prompt, line);

  ttcol = 0;  		   /* otherwise we shall not see the message */
			   /* if we are interactive, pause here */
  if (!clexec)
  { mlwrite(s == 0 ? TEXT188 : "[Failed]");
/*             "[End]" */
    while ((s = tgetc()) != '\r' && s != ' ')
      ;
    mlerase();
  }
  return TRUE;
#undef line
}

	/*
	 * Create a subjob with a copy of the command intrepreter. When the
	 * command interpreter exits, mark the screen as garbage so that you 
	 * do a full repaint. Bound to "^X C".
	 */
Pascal NEAR spawncli(int f, int n)

{	if (restflag)		/* don't allow this command if restricted */
	  return resterr();

        usehost('!', "");
        return TRUE;
}

	 /* Run a one-liner in a subjob. When the command returns, wait for 
	  * a single character to be typed, then mark the screen as garbage 
	  * so a full repaint is done. Bound to "C-X !".
	  */
Pascal NEAR spawn(int f, int n)

{ return gen_spawn(f, n, "!");
}

	/* Run an external program with arguments. When it returns, wait 
	 * for a single character to be typed, then mark the screen as 
	 * garbage so a full repaint is done. Bound to "C-X $".
	 */
Pascal NEAR execprg(int f, int n)
				
{ return gen_spawn(f, n, "$");
}

        char prompt[2];

	/* Pipe a one line command into a window
	 * Bound to ^X @ or ^X #
	 */
Pascal NEAR pipefilter(wh)
	 char    wh;
{
 static       int  bix;
 static       char bname [10] = "_command";

 static const char filnam1[] = "fltinp";
 static const char filna2[] = " <fltinp >fltout";
#define filnam2 &filna2[10]

        char tmpnam_[128];
 extern char deltaf[HICHAR];
        char line[NSTRING+1];       /* command line send to shell */

	if (restflag)		/* don't allow this command if restricted */
	  return resterr();

	prompt[0] = wh;
{	int s;
	wh -= '@';			
	if (wh != 0 and (curbp->b_flag & MDVIEW)) /* disallow this command if */
	  return rdonly();			  /* we are in read only mode */

        if (mlreply(prompt, line, NLINE) != TRUE)
          return FALSE;

	if (wh == 0)
	{ sprintf(bname,"cmd%d\n",++bix);
					/* get the command to pipe in */
			/* get rid of the command output buffer if it exists */
	{ BUFFER * bp = bfind(bname, FALSE, 0);
          if (bp != null) 	    /* try to make sure we are off screen */
	  { BUFFER * sbp = curbp;
	    s = orwindmode(0, 1);
	    curbp = sbp;
	    if (s > 0)
	      onlywind(FALSE, 1);
				   /* get rid of the existing command buffer */
	    if (zotbuf(bp) != TRUE)
	      return FALSE;
	  }
	  concat(&tmpnam_[0], " >", fixnull(getenv("TMP")), "\\", bname, null);
          strcat(&line[0], tmpnam_);
	}}
	else				/* setup the proper file names */
	{ if (writeout(filnam1) != TRUE)/* write it out, checking for errors */
	  { mlwrite(TEXT2);
   					/* "[Cannot write filter file]" */
	    return FALSE;
	  }
          strcat(&line[0], filna2);
	}
        s = usehost('!', line);
        if (s != 0)
	{ mlwrite(TEXT3);
		/* "[Execution failed]" */
//	  return FALSE;
	}
			 	       /* did the output file get generated? */
{/* 	int fid = open(tmpnam, O_RDONLY);
	if (fid < 0)
	  return FALSE;
	close(fid);
*/
	if (wh == 0)
	{ if (splitwind(FALSE, 1) == FALSE)
	    return FALSE;

        { BUFFER * bp = bufflink(&tmpnam_[2], clexec);	/* and read the stuff in */
	  if (bp == NULL)
	    return FALSE;
	  
	  swbuffer(bp);
       /* linstr(tmpnam); */
		      /* make this window in VIEW mode, update all mode lines */
	  curwp->w_bufp->b_flag |= MDVIEW;
	  (void)orwindmode(WFMODE, 0);
					/* and get rid of the temporary file */
	  s = TRUE;
	}}
	else				/* on failure, escape gracefully */	
	{ char * sfn = curbp->b_fname;
	  curbp->b_fname = null;
	  s = readin(filnam2, FALSE);
          curbp->b_fname = sfn;		/* restore name */
	  curbp->b_flag |= BFCHG;	/* flag it as changed */

	  unlink(filnam1);		/* and get rid of the temporary file */
	}
	unlink(	wh == 0 ? &tmpnam_[2] : filnam2 );
	return s != FALSE;
}}}


	/* Pipe a one line command into a window
	 * Bound to ^X @
	 */
Pascal NEAR pipecmd(int f, int n)

{ return pipefilter('@');
}

	/*
	 * filter a buffer through an external DOS program
	 * Bound to ^X #
	 */
Pascal NEAR filter(int f, int n)

{ return pipefilter('#');
}

#if 0
X
X		    /* return a system dependant string with the current time */
Xchar *Pascal NEAR timeset()
X
X{
X#if	MWC | TURBO | MSC
X	register char *sp;	/* temp string pointer */
X	char buf[16];		/* time data buffer */
Xextern char *ctime();
X
X	time(buf);
X	sp = ctime(buf);
X	sp[strlen(sp)-1] = 0;
X	return sp;
X#else
X	return errorm;
X#endif
}

#endif





Char * Pascal NEAR LFN_to_8dot3(int dir,/* 1 to 8.3; else 2*/
				int up, /* 0 dont xlate, else dirs up in res*/
				char *from, char *to)
{
  union REGS r;
  struct SREGS s;

   /*   memset(&s, 0, sizeof(struct SREGS)); */
  s.ds = (unsigned)((unsigned long)from >> 16);
  r.x.si = (unsigned)from;
  s.es = (unsigned)((unsigned long)to >> 16);
  r.x.di = (unsigned)to;
  r.x.cx = dir;
  r.x.ax = 0x7160;

{ register int end;
	   int end_ = 0;

  for (end = 0; from[++end] != 0; )
    if (from[end-1] == '/' || from[end-1] == '\\')
    { end_ = 1;
    }
  
  if (end_ == 0)
    up = 1;

  int86x(0x21, &r,&r,&s);
  if (r.x.cflag || end <= 8)
  {
  	return from;
  }
              
  for (end = -1; to[++end] != 0; )
    ;

  end_= 0;
  if (to[--end] == '\\')
    to[end] = '/';

  for (; --end >= 0; ) /* ignore trailing / */
    if (/*to[end] == '/' || */ to[end] == '\\')
    { to[end] = '/';
      if (--up == 0)
        end_ = end+1;
    }

  return to + end_;
}}
