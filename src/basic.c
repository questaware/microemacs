/*
 * The routines in this file move the cursor around on the screen. They
 * compute a new value for the cursor, then adjust ".". The display code
 * always updates the cursor location, so only moves between lines, or
 * functions that adjust the top line in the window and invalidate the
 * framing, are hard.
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"

void adb(int n)

{ mlwrite("\007ADB(%d)", n);
#if S_WIN32
	mbwrite(NULL);
#else
  ttgetc();
#endif
}


char * mallocz(int n)

{ char * res = malloc(n);
  if (res == NULL)
    mlwrite(TEXT99);
					/* "out of memory " */
	else
    memset(res, 0, n);
  return res;
}


char * Pascal remallocstr(char * * res_ref, const char * val, int len)

{ if (len == 0)
    len = val == NULL ? NSTRING : strlen(val)+1;
{ char * res = (char*)mallocz(len);

  if (res != NULL && val != NULL)
  	strcpy(res, val);

  free(*res_ref);
  return *res_ref = res;
}}



BUFFER * Pascal prevele(BUFFER * bl, BUFFER * bp)

{
  while (bl != NULL && bl->b_next != bp)
    bl = bl->b_next;
  return bl;
}

int Pascal gotobol(int f, int n)

{ curwp->w_doto = 0;
  return TRUE;
}

int Pascal gotoeol(int f, int n)

{ curwp->w_doto = llength(curwp->w_dotp);
  return TRUE;
}



/* nextch -- retrieve the current/previous character in the buffer,
 *					 and advance/retreat the point.
 *      		 The asymmetry of forward and back is mysterious!!
 */
int Pascal nextch(Lpos_t * lpos, int dir)

{	char c = '\n';
  LINE * lp = lpos->curline;
  int	off = lpos->curoff;
  int ct = dir == 0 ? 1 : dir;

  if (dir >= FORWARD)
  { while (--ct >= 0)
  	{	if (off != llength(lp)) 	    	/* if at EOL */
	      c = lgetc(lp, off++);					/* get the char */
	    else
	    { lp = lforw(lp);	/* skip to next line */
	      lpos->line_no += 1;
	      off = 0;
			}
    }
  }
  else		       								/* Reverse.*/
  { while (++ct <= 0)
	  { if (off != 0)
				c = lgetc(lp, --off);
	    else
			{	lpos->line_no -= 1;
				lp = lback(lp);
	      off = llength(lp);
	    }
		}
  }
  lpos->curoff = off;
  lpos->curline = lp;
  return lp->l_props & L_IS_HD ? -1 : c & 0xff;
}


int Pascal forwchar(int f, int n)

{ WINDOW * wp = curwp;
#if 1
  wp->w_flag |= WFMOVE;
{	LINE * lp = wp->w_dotp;
  if (n < 0 && wp->w_doto == 0)
  	lp = lback(lp);
  return (lp->l_props & L_IS_HD) ? FALSE
  															 : nextch((Lpos_t*)wp, n) >= 0;
}
#else
	if (n < 0)
	{ while (n++)
	    if (wp->w_doto > 0)
	    	wp->w_doto--;
			else
	    { LINE * lp = lback(wp->w_dotp);
	      if (lp->l_props & L_IS_HD)
					return FALSE;
	      wp->w_dotp  = lp;
	      wp->w_doto  = llength(lp);
	      wp->w_line_no -= 1;
	    }
	}
	else
	  while (n--) 
		{ if (wp->w_dotp->l_props & L_IS_HD)
				return FALSE;
	    if (++(wp->w_doto) > llength(wp->w_dotp)) 
	    { wp->w_dotp  = lforw(wp->w_dotp);
	      wp->w_doto  = 0;
	      wp->w_line_no += 1;
	    }
		} 

  return TRUE;
#endif
}



int Pascal backchar(int f, int n)

{ return forwchar(f, -n);
}


static
int Pascal gotobob_()

{ WINDOW * wp = curwp;
  
  wp->w_flag |= WFHARD;
  wp->w_dotp  = lforw(&curbp->b_baseline);
  wp->w_linep = wp->w_dotp;
  wp->w_doto  = 0;
  wp->w_line_no = 1;
  return TRUE;
}

int Pascal gotobob(int f, int n)

{ return gotobob_();
}


int Pascal gotoline(int f, int n)	/* move to a particular line.
				   argument (n) must be a positive integer for
				   this to actually do anything 	*/
{ char arg[20];

	/* get an argument if one doesnt exist */
  if (!f)
  { int cc = mlreply(TEXT7, arg, sizeof(arg)-1);
									/* "Line to GOTO: " */
    if (cc <= FALSE)
    { mlwrite(TEXT8);
						/* "[Aborted]" */
      return cc;
    }
    n = atoi(arg);
  }

  if (n < 1)		/* if a bogus argument...then leave */
    return FALSE;
				/* first, we go to the start of the buffer */
  gotobob_();
  return forwline(f, n-1);
}


int Pascal gotoeob(int f, int n)

{ WINDOW * wp = curwp;
  int ct;
  LINE * lp = &wp->w_bufp->b_baseline;
  wp->w_dotp = lp;
  wp->w_doto = 0;
  wp->w_flag |= WFHARD;
  setcline();
  for (ct = wp->w_ntrows; --ct >= 0; )
    lp = lp->l_bp;

  wp->w_linep = lp;

  return TRUE;
}


extern KEYTAB * prevbind;

int Pascal forwline(int f, int n_)
							/* if we are on the last line as we start....fail the command */
{ int n = n_;
  LINE * dlp = curwp->w_dotp;
  LINE * lim = &curbp->b_baseline;
//int inc = 0;

	static int g_curgoal;

//thisflag |= CFCPCN;			/* flag this command as a line move */

  if (n < 0)
	{	lim = lforw(lim);
//	inc = 1;
  }

  if (dlp == lim)
    return FALSE;
																/* move the point down */
  for (; dlp != lim && n != 0; )
    if (n < 0)
    {	++n;
    	dlp = lback(dlp);
    }
    else
		{	--n;
    	dlp = lforw(dlp);
    }
																		/* if the last command was not a line move,
																			 reset the goal column */
  if (keyct == 1 && prevbind != NULL && prevbind->k_ptr.fp != backline)
    g_curgoal = getccol();

  curwp->w_doto  = getgoal(dlp,g_curgoal);
  curwp->w_dotp  = dlp;							/* Can be b_baseline meaning at end */
  curwp->w_flag |= WFMOVE;
  curwp->w_line_no += n_ - n;

  return TRUE;
}



int Pascal backline(int f, int n)

{ return forwline(f, -n);
}

#if	WORDPRO

int Pascal gotobop(int f, int n) /* go back to beginning of current paragraph
										   here look for a <NL><NL> or <NL><TAB> or <NL><SPACE>
										 combination to delimit the beginning of a paragraph  */
{
  return gotoeop(f, -n);
}

int Pascal gotoeop(int f, int n)  /* go forword to end of current paragraph
													    			 look for <NL><NL>, <NL><TAB> or <NL><SPACE>
																		 to delimit the beginning of a paragraph */
{ LINE * ln;
  int dir = 1;

  if (n < 0)	/* the other way...*/
  { dir = -1;
    n = -n;
  }

  while (--n >= 0)
  {	char suc;														/* first scan until we are in a word */
    while ((suc = forwchar(FALSE, dir)) && !inword())
      ;
	{ WINDOW * wp = curwp;								/* and go to the B-O-Line */
    ln = wp->w_dotp;										/* of next line if not at EOF */
    if (dir > 0 && suc)				
      ln = lforw(ln);
				 		     				/* and scan forword until we hit a <NL><NL> or <NL><TAB>
				  			  				 or a <NL><SPACE>				*/
    while ((ln->l_props & L_IS_HD) == 0)
    { if (llength(ln) == 0 ||
				  (suc = lgetc(ln, 0)) == '\t' || suc == ' ')
				break;
      if (dir > 0)
        ln = lforw(ln);
      else
        ln = lback(ln);
    }

    wp->w_dotp = ln;
    wp->w_doto = 0;											
	  wp->w_flag |= WFMOVE; /* force screen update */
 
    if (dir < 0 && (ln->l_props & L_IS_HD))
      return gotobob_();
    
    				/* and then backward until we are in a word */
    while (forwchar(FALSE, -dir) && !inword())
      ;

    if (dir > 0)	        
      wp->w_doto = llength(wp->w_dotp);	/* and to the EOL */
  }}

  return TRUE;
}
#endif


/* Scroll forward by a specified number of lines, or by a full page if no
 * argument. Bound to "C-V". The "2" in the arithmetic on the window size is
 * the overlap; this value is the default overlap value in ITS EMACS. Because
 * this zaps the top line in the display window, we have to do a hard update.
 */
int Pascal forwpage(int f, int n)

{ register WINDOW * wp = curwp;

  if (!f) 
  { int n_ = n;
    n = wp->w_ntrows - 2;	 /* Default scroll.	 */
    if (n <= 0) 		    /* Forget the overlap   */
       n = 1;			    /* if tiny window.	    */
    if (n_ < 0)
      n = -n;
  } 
#if	CVMVAS
  else					  /* Convert from pages   */
    n *= wp->w_ntrows;		       /* to lines.	       */
#endif

#if 1
  pd_sgarbf = -1;
#endif

  return mvupwind(FALSE, -n);
}


int Pascal backpage(int f, int n)

{
#if S_MSDOS == 0
  if (n > 0 && (lback(curwp->w_linep)->l_props & L_IS_HD))
  {/*TTbeep();*/
    return TRUE;
  }
#endif
  return forwpage(f, -n);
}


static MARK  g_rem;

int Pascal setmark(int f, int n)

{ n = f == FALSE ? 0 : (unsigned)n & (NMARKS-1);
{	MARK * mrk = &curwp->mrks.c[n];
  g_rem = *mrk;
  *mrk = *(MARK*)&curwp->w_dotp;

  mlwrite(TEXT9, n);
			  /* "[Mark %d set]" */
  return TRUE;
}}



int Pascal remmark(int f, int n)

{ n = f == FALSE ? 0 : (unsigned)n & (NMARKS-1);
  curwp->mrks.c[n].markp = NULL;
  mlwrite(TEXT10, n);
				/* "Mark %d removed]" */
  return TRUE;
}

static const char text11[] = TEXT11;

int Pascal swapmark(int f, int n)
 
{ (void)setmark(f, n);

  if (g_rem.markp == NULL)
  { mlwrite(text11, n);
					/* No mark %d in this window" */
    return FALSE;
  }

  *(MARK*)&curwp->w_dotp = g_rem;
  curwp->w_flag |= WFMOVE;
  return TRUE;
}


int Pascal gotomark(int f, int n)

{ n = f == FALSE ? 0 : (unsigned)n & (NMARKS-1); /* make sure it is in range */

{ WINDOW * wp = curwp;
	MARK * mrk = &wp->mrks.c[n];
  if (mrk->markp == NULL)
  { mlwrite(text11, n);
					/* "No mark %d in this window" */
    return FALSE;
  }
  *(MARK*)&wp->w_dotp = *mrk;
  wp->w_flag |= WFMOVE;
  setcline();
  return TRUE;
}}

