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

void USE_FAST_CALL adb(int n)

{ mlwrite("%p%bADB(%d)", n);
}


char * USE_FAST_CALL mallocz(int n)

{ char * res = malloc(n);
  if (res == NULL)
    mlwrite(TEXT99);
					/* "out of memory " */
	else
    res = memset(res, 0, n);
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

int Pascal gotobol(int notused, int n)

{ curwp->w_doto = 0;
  return TRUE;
}

int Pascal gotoeol(int notused, int n)

{ curwp->w_doto = llength(curwp->w_dotp);
  return TRUE;
}



/* nextch -- retrieve the current/previous character in the buffer,
 *					 and advance/retreat the point.
 *      		 The asymmetry of forward and back is mysterious!!
 */
int Pascal nextch(Lpos_t * lpos, int dir)

{	int adj = 0;
	LINE * lp = lpos->curline;
  int	off = lpos->curoff;
	char c = '\n';

  if (dir >= 0)
	{	if (dir == 0)
			dir = 1;
  	while (--dir >= 0)
  	{ if (off < llength(lp)) 	    	/* if at EOL */
	      c = lgetc(lp, off++);				/* get the char */
	    else
	    { off = 0;
	      adj += 1;
	      lp = lforw(lp);	/* skip to next line */
	      if (l_is_hd(lp))
	      	return -1;
			}
    }
  }
  else		       								/* Reverse.*/
  { dir = -dir;
    while (--dir >= 0)
  	{ if (off > 0)
				c = lgetc(lp, --off);
	    else
			{	adj -= 1;
				lp = lback(lp);
	      off = llength(lp);
	      if (l_is_hd(lp))
	      	return -1;
	    }
		}
  }
	lpos->curline = lp;
 	lpos->curoff = off;
	lpos->line_no += adj;

  return c & 0xff;
}


int Pascal forwchar(int notused, int n)

{ WINDOW * wp = curwp;
#if 1
  wp->w_flag |= WFMOVE;
//{	LINE * lp = wp->w_dotp;
//if (n < 0 && wp->w_doto == 0)
// 	lp = lback(lp);
  return /* (lp->l_props & L_IS_HD) ? FALSE : */
  			 n == 0 ? TRUE : nextch((Lpos_t*)wp, n) >= 0;
//}
#else
	if (n < 0)
	{ while (n++)
	    if (wp->w_doto > 0)
	    	wp->w_doto--;
			else
	    { LINE * lp = lback(wp->w_dotp);
	      if (l_is_hd(lp))
					return FALSE;
	      wp->w_dotp  = lp;
	      wp->w_doto  = llength(lp);
	      wp->w_line_no -= 1;
	    }
	}
	else
	  while (n--) 
		{ if (l_is_hd(wp->w_dotp))
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



int Pascal backchar(int notused, int n)

{ return forwchar(-n, -n);
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

int Pascal gotobob(int notused, int n)

{ return gotobob_();
}


int Pascal gotoline(int f, int n)	/* move to a particular line.
																	   The argument n must be a positive integer 
																   	 for this to actually do anything */
{ 
	/* get an argument if one doesnt exist */
  if (f)
  { if (n < 0)									/* goto -1 : goto last mark */
  		return gotomark(n, n);
  }
  else
  { char arg[20];
  	int cc = mlreply(TEXT7, arg, sizeof(arg)-1);
									/* "Line to GOTO: " */
    if (cc <= FALSE)
    { // mlwrite(TEXT8);
			return ctrlg(0,0);			/* "[Aborted]" */
      // return cc;
    }
		
		setmark(-1,-1);		/* set the last mark */
    n = atoi(arg);
  }

  if (n < 1)		/* if a bogus argument...then leave */
    return FALSE;
				/* first, we go to the start of the buffer */
  gotobob_();
  return forwline(n-1, n-1);
}


int Pascal gotoeob(int notused, int n)

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


int Pascal forwline(int notused, int n_)
							/* if we are on the last line as we start....fail the command */
{ int n = n_;
  LINE * lp = curwp->w_dotp;
  LINE * lim = &curbp->b_baseline;
//int inc = 0;

//thisflag |= CFCPCN;			/* flag this command as a line move */

  if (n < 0)
	{	lim = lforw(lim);
//	inc = 1;
  }

  if (lp == lim)
    return FALSE;
																/* move the point down */
  for (; lp != lim && n != 0; )
    if (n < 0)
    {	++n;
    	lp = lback(lp);
    }
    else
		{	--n;
    	lp = lforw(lp);
    }

{	static int g_curgoal;					/* if the last command was not a line move,
																	 reset the goal column */
	if (pd_keyct == 1)
	{
    g_curgoal = getccol();
	}

  curwp->w_doto  = getgoal(g_curgoal, lp);
  curwp->w_dotp  = lp;							/* Can be b_baseline meaning at end */
  curwp->w_flag |= WFMOVE;
  curwp->w_line_no += n_ - n;

  return TRUE;
}}



int Pascal backline(int notused, int n)

{ return forwbyline(-n);
}

#if	WORDPRO

int Pascal gotobop(int notused, int n) /* go back to beginning of current paragraph
										   here look for a <NL><NL> or <NL><TAB> or <NL><SPACE>
										 combination to delimit the beginning of a paragraph  */
{
  return gotoeop(-n, -n);
}

int Pascal gotoeop(int notused, int n)  /* go forword to end of current paragraph
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
    while ((suc = forwbychar(dir)) && !inword())
      ;
	{ WINDOW * wp = curwp;								/* and go to the B-O-Line */
    ln = wp->w_dotp;										/* of next line if not at EOF */
    if (dir > 0 && suc)				
      ln = lforw(ln);
				 		     				/* and scan forword until we hit a <NL><NL> or <NL><TAB>
				  			  				 or a <NL><SPACE>				*/
    while (!l_is_hd(ln))
    { if (llength(ln) == 0 ||
				  (suc = lgetc(ln, 0)) == '\t' || suc == ' ')
				break;
			ln = lmove(ln, (dir >> 1) & 1);
    }

    wp->w_dotp = ln;
    wp->w_doto = 0;											
	  wp->w_flag |= WFMOVE; /* force screen update */
 
    if (dir < 0 && l_is_hd(ln))
      return gotobob_();
    
    				/* and then backward until we are in a word */
    while (forwchar(FALSE, -dir) && !inword())
      ;

    if (dir > 0)
	    gotoeol(0,0);
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
	upwind(TRUE);
//pd_sgarbf = -1;
#endif

  return mvupwind(FALSE, -n);
}


int Pascal backpage(int f, int n)

{
#if S_MSDOS == 0
  if (n > 0 && l_is_hd(lback(curwp->w_linep)))
  {/*tcapbeep();*/
    return TRUE;
  }
#endif
  return forwpage(f, -n);
}


#if FLUFF
 static MARK  g_rem;
#endif

int Pascal setmark(int f, int n)

{ n = (unsigned)n & (NMARKS-1);
	if (f == FALSE)
		n = 0; 
	if (f >= 0)
	  mlwrite(TEXT9, n);
			  /* "[Mark %d set]" */
{	MARK * mrk = &curwp->mrks.c[n];

#if FLUFF
  g_rem = *mrk;
#endif
  *mrk = *(MARK*)&curwp->w_dotp;

  return TRUE;
}}



int Pascal remmark(int f, int n)

{ n = f == FALSE ? 0 : (unsigned)n & (NMARKS-1);
  curwp->mrks.c[n].markp = NULL;
  mlwrite(TEXT10, n);
				/* "Mark %d removed]" */
  return TRUE;
}

#if FLUFF

int Pascal swapmark(int f, int n)
 
{ (void)setmark(f, n);

  if (g_rem.markp == NULL)
  { mlwrite(TEXT11, n);
					/* No mark %d in this window" */
    return FALSE;
  }

  *(MARK*)&curwp->w_dotp = g_rem;
  curwp->w_flag |= WFMOVE;
  return TRUE;
}

#endif

int Pascal gotomark(int f, int n)

{ n = f == FALSE ? 0 : (unsigned)n & (NMARKS-1); /* make sure it is in range */

{ WINDOW * wp = curwp;
	MARK * mrk = &wp->mrks.c[n];
  if (mrk->markp == NULL)
  { mlwrite(TEXT11, n);
					/* "No mark %d in this window" */
    return FALSE;
  }
  *(MARK*)&wp->w_dotp = *mrk;
  wp->w_flag |= WFMOVE;
  setcline();
  return TRUE;
}}

