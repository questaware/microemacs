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

/*
null_proc()

{
}
*/


void adb(int n)

{ mlwrite("\007ADB(%d)", n);
  ttgetc();
}


char * mallocz(int n)

{ char * res = malloc(n);
  if (res != NULL)
    memset(res, 0, n);
  return res;
}


char * Pascal remallocstr(char * * res_ref, const char * val, int len)

{ char * res = null;

  if (val != null)
  { int n = len;
    if (n == 0)
      n = strlen(val)+1;
    res = (char*)malloc(n);
    if (res != null)
    { memcpy(res, val, n);
      if (*res_ref != null)
        free(*res_ref);

      *res_ref = res;
    }
  }

  return res;
}



BUFFER * Pascal prevele(BUFFER * bh, BUFFER * bp)

{ register BUFFER * bl = bh;

  while (bl != NULL && bl->b_bufp != bp)
    bl = bl->b_bufp;
  return bl;
}

#if 0
X
XLINE * spos_line;
XShort  spos_offs;
X
XPascal save_pos()
X
X{ spos_line = curwp->w_dotp
X  spos_offs = curwp->w_doto;
X}
X
X
X
XPascal rest_pos()
X
X{ curwp->w_dotp = spos_line;
X  curwp->w_doto = spos_offs;
X}
X
#endif

int Pascal gotobol(int f, int n)

{ curwp->w_doto  = 0;
  return TRUE;
}

int Pascal gotoeol(int f, int n)

{ curwp->w_doto  = llength(curwp->w_dotp);
  return TRUE;
}



int Pascal forwchar(int f, int n)

{ register WINDOW * wp = curwp;

 if (n < 0)
  while (n++)
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
 else
  while (n--) 
    if (wp->w_doto < llength(wp->w_dotp)) 
      wp->w_doto++;
    else
    { if (wp->w_dotp->l_props & L_IS_HD)
				return FALSE;
      wp->w_dotp  = lforw(wp->w_dotp);
      wp->w_doto  = 0;
      wp->w_line_no += 1;
    } 

  wp->w_flag |= WFMOVE;
  return TRUE;
}



int Pascal backchar(int f, int n)

{ return forwchar(f, -n);
}


int Pascal gotobob_()

{ register WINDOW * wp = curwp;
  
  wp->w_flag |= WFHARD;
  wp->w_dotp  = lforw(curbp->b_baseline);
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
/*				      "Line to GOTO: " */
    if (cc != TRUE)
    { mlwrite(TEXT8);
/*				"[Aborted]" */
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

{
#if 1
  (void)gotobob_();
#else
  register WINDOW * wp = curwp;
 
  wp->w_doto  = 0;
  wp->w_flag |= WFHARD;
#endif
{ register int ct;
  LINE * lp = curbp->b_baseline;
  curwp->w_dotp  = lp;
  setcline();
  resetlcache();
  for (ct = curwp->w_ntrows; --ct > 0; )
    lp = lp->l_bp;

  curwp->w_linep = lp;

  return TRUE;
}}


extern KEYTAB * prevbind;


int Pascal forwline(int f, int n_)
		/* if we are on the last line as we start....fail the command */
{ register int n = n_;
  register LINE *  dlp = curwp->w_dotp;
           LINE * lim = curbp->b_baseline;
  if (n < 0)
    lim = lforw(lim);
  if (dlp == lim)
    return FALSE;
				/* move the point down */
  for (; dlp != lim && n != 0; )
    if (n < 0)
    { dlp = lback(dlp);
      ++n;
    }
    else
    { dlp = lforw(dlp);
      --n;
    }
				/* if the last command was not note a line move,
				    reset the goal column */
  if (keyct == 1 && prevbind != NULL && prevbind->k_ptr.fp != backline)
    curgoal = getccol();

  thisflag |= CFCPCN;			/* flag this command as a line move */

  curwp->w_dotp  = dlp;		/* reseting the current position */
  curwp->w_doto  = getgoal(dlp, curgoal);
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
#if 1
  return gotoeop(f, -n);
#else
  register LINE * ln;
  register char ch;

  if (n < 0)	/* the other way...*/
    return gotoeop(f, -n);

  while (n-- > 0)	/* for each one asked for */
																/* first scan back until we are in a word */
  { while (backchar(FALSE, 1) && !inword())
      ;
    curwp->w_doto = 0;			  /* go to the B-O-Line */
								  /* and scan back until we hit a <NL><NL> or <NL><TAB>
											or a <NL><SPACE>			*/
    for (ln = curwp->w_dotp; 
         (lback(ln)->l_props & L_IS_HD) == 0;
         ln = lback(ln))
    { if (llength(ln) == 0 ||
          (ch = lgetc(ln, 0)) == '\t' || ch == ' ')
				break;
    }
				/* to be w_line_no'ed */
    curwp->w_dotp = ln;
        
    while (forwchar(FALSE, 1) && !inword()) /* forward until we are in a word */
      ;
  }
  curwp->w_flag |= WFMOVE;		/* force screen update */
  return TRUE;
#endif
}

int Pascal gotoeop(int f, int n)  /* go forword to end of current paragraph
										    here look for a <NL><NL> or <NL><TAB> or <NL><SPACE>
											  combination to delimit the beginning of a paragraph */
{ register WINDOW * wp = curwp;
  register LINE * ln;
  register char suc;	/* success of last backchar */
  register char dir = 1;

  if (n < 0)	/* the other way...*/
  { dir = -1;
    n = -n;
  }

  while (n-- > 0)	/* for each one asked for */
  {															/* first scan until we are in a word */
    while ((suc = forwchar(FALSE, dir)) && !inword())
      ;
						/* and go to the B-O-Line */
    wp->w_doto = 0;				/* of next line if not at EOF */
    ln = wp->w_dotp;
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
      suc = 0;
    }

    wp->w_dotp = ln;
 
    if (dir < 0 && (ln->l_props & L_IS_HD))
      gotobob_();
    else
    {				/* and then backward until we are in a word */
      while ((suc = backchar(FALSE, dir)) && !inword())
        ;
    }

    if (dir > 0)	        
      wp->w_doto = llength(wp->w_dotp);	/* and to the EOL */
  }
  wp->w_flag |= WFMOVE; /* force screen update */
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
  sgarbf = -1;
#endif
{
#if 0
  LINE * p = lforw(curbp->b_baseline);
  register LINE *  lp = wp->w_linep;
  if (n < 0)
    for ( ; n++ && lp != p; lp = lback(lp))
      ;
  else  
    for ( ; n-- && (lp->l_props & L_IS_HD) == 0; lp = lforw(lp))
      ;
  wp->w_linep = lp;
  wp->w_dotp  = lp;
  wp->w_doto  = 0;
  wp->w_flag |= WFHARD;
  return TRUE;
#else
  return mvupwind(FALSE, -n);
#endif
}}


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


static LINE * remdotp;
static int    remdoto;
 

int Pascal setmark(int f, int n)

{ n = f == FALSE ? 0 : (unsigned)n & (NMARKS-1);
  mlwrite(TEXT9, n);
/*		"[Mark %d set]" */
{	MARK * mrk = &curwp->mrks.c[n];
  remdotp = mrk->markp;
  remdoto = mrk->marko;
  mrk->markp = curwp->w_dotp;
  mrk->marko = curwp->w_doto;
  return TRUE;
}}



int Pascal remmark(int f, int n)

{ n = f == FALSE ? 0 : (unsigned)n & (NMARKS-1);
  mlwrite(TEXT10, n);
/*		"[Mark %d removed]" */
  curwp->mrks.c[n].markp = NULL;
  return TRUE;
}

static const char text11[] = TEXT11;

int Pascal swapmark(int f, int n)
 
{ setmark(f, n);
  if (remdotp == NULL)
  { mlwrite(text11, n);
/*			"No mark %d in this window" */
    return FALSE;
  }
  curwp->w_dotp = remdotp;
  curwp->w_doto = remdoto;
  curwp->w_flag |= WFMOVE;
  return TRUE;
}


int Pascal gotomark(int f, int n)

{ register WINDOW * wp = curwp;
  n = f == FALSE ? 0 : (unsigned)n & (NMARKS-1); /* make sure it is in range */

{ LINE * dotp = wp->mrks.c[n].markp;
  if (dotp == NULL)
  { mlwrite(text11, n);
/*			"No mark %d in this window" */
    return FALSE;
  }
  wp->w_dotp  = dotp;
  wp->w_doto  = wp->mrks.c[n].marko;
  wp->w_flag |= WFMOVE;
  setcline();
  return TRUE;
}}

