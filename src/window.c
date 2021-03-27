/* Window management. Some of the functions are internal, and some are
 * attached to keys that the user actually types.
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"
#if S_MSDOS == 0 || MSC == 0 || S_WIN32
/*#include        <stdarg.h>*/
#endif
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"logmsg.h"


int Pascal orwindmode(int mode, int wh)
	/* Short 	wh;  ** 0 => all, 1 => for curbp */
{ WINDOW *wp;
  int ct = 0;
				   /* force all windows to redraw */
  for (wp = wheadp; wp != NULL; wp = wp->w_wndp)
    if (wh == 0 || wp->w_bufp == curbp)
    { /* if      (wp->w_changed == null)
        wp->w_changed = curbp->b_dotp;
      else if (wp->w_changed != curbp->b_dotp)
        mode = WFHARD; */
      wp->w_flag |= mode;
      ++ct;
    }

  return ct;
}

void Pascal  openwind(WINDOW * wp_, BUFFER * bp)
	
{ register WINDOW *  wp = wp_;
  wp->w_bufp    = bp;	     /* connect current window to this buffer */
  wp->w_linep   = lforw(bp->b_baseline);
  wp->w_dotp    = bp->b_dotp;
  wp->w_doto    = bp->b_doto;
  wp->w_line_no = 1;
  wp->w_flag    = WFHARD|WFFORCE;
  memset(&wp->mrks, 0, sizeof(MARKS));
}




void Pascal leavewind(WINDOW * wp, int dec)
	
{ BUFFER * bp = wp->w_bufp;

  bp->b_nwnd -= dec;
	bp->b_wlinep = wp->w_linep;
  bp->b_dotp	 = wp->w_dotp;
  bp->b_doto	 = wp->w_doto;
  bp->mrks	 = wp->mrks;
/*bp->b_fcol	 = wp->w_fcol;*/
}

int Pascal openwindbuf(char * bname)
	
{		/* split the current window to make room for the binding list */
  if (splitwind(FALSE, 1) == FALSE)
    return FALSE;
					        
  curbp = bfind(bname, TRUE, 0);		/* and get a buffer for it */
  if (curbp == NULL)
  { mlwrite(TEXT99);
					/* "out of memory " */
    return FALSE;
  }
  if (bclear(curbp))
  { leavewind(curwp, 0);
    openwind(curwp, curbp);
  
    curbp->b_flag &= ~MDVIEW;
    curbp->b_flag |= BFACTIVE;
  }
  return TRUE;
}

/* Reposition dot in the current window to line "n". If the argument is
 * positive, it is that line. If it is negative it is that line from the
 * bottom. If it is 0 the window is centered (this is what the standard
 * redisplay code does). With no argument it defaults to 0. Bound to M-!.
 */
int Pascal reposition(int f, int n)

{   curwp->w_force = f == FALSE ? 0 : n;
    curwp->w_flag |= WFFORCE;
    return TRUE;
}

/*
 * Refresh the screen. With no argument, it just does the refresh. With an
 * argument it recenters "." in the current window. Bound to "C-L".
 */
int Pascal refresh(int f, int n)

{  if (f != FALSE)
      return reposition(0, 0);  /* Center dot. */
    pd_sgarbf = TRUE;
    return pd_sgarbf;
}

/*
 * The command make the next window (next => down the screen) the current
 * window. There are no real errors, although the command does nothing if
 * there is only 1 window on the screen. Bound to "C-X C-N".
 *
 * with an argument this command finds the <n>th window from the top
 *
 */
int Pascal nextwind(int f, int n)

{
	WINDOW *wp;

	leavewind(curwp,0);

	if (! f)
	{ wp = curwp->w_wndp;
		if (wp == NULL)
	    wp = wheadp;
	}
	else
	{ int wct = orwindmode(0, 0);  /* 0,: just count windows */
	
    if (n < 0)
      n = ((wct+n) % wct)+1;  /* the nth window from bottom of the screen */
	  
	  wp = wheadp;
	  while (--n > 0)
	  { wp = wp->w_wndp;
	    if (wp == NULL)
	      wp = wheadp;
	  }
	}
  curwp->w_flag |= WFMODE;		/* Mode line is updated */
	curwp = wp;
	curbp = wp->w_bufp;
	upmode();
	return TRUE;
}

/*
 * This command makes the previous window (previous => up the screen) the
 * current window. There arn't any errors, although the command does not do a
 * lot if there is 1 window.
 */
int Pascal prevwind(int f, int n)

{	   /* if we have an argument, we mean the nth window from the bottom */
	if (f)
	  return nextwind(f, -n);

  if (wheadp == curwp)
  	return nextwind(f, 0);
	curwp = (WINDOW*)prevele((BUFFER*)wheadp,(BUFFER*)curwp);
	return TRUE;
}

/*
 * Move the current window up by "arg" lines. Recompute the new top line of
 * the window. Look to see if "." is still on the screen. If it is, you win.
 * If it isn't, then move "." to center it in the new framing of the window
 * (this command does not really move "."; it moves the frame). Bound to
 * "C-X C-P".
 */
int Pascal mvupwind(int f, int n)

{   LINE * lp = curwp->w_linep;
    LINE * p = curbp->b_baseline;
    int i = n;

    if (i < 0)
    { while (lp != p && ++i <= 0)
				lp = lforw(lp);
      --i;
    }
    else
    { p = lforw(p);
      while (lp != p && --i >= 0)
				lp = lback(lp);
      ++i;
    }

    if (i != n)
      curwp->w_flag |= WFHARD;		/* Mode line is OK. */

{   int wpos = getwpos();
    curwp->w_linep = lp;

    if (getwpos() < 0 && (n > 0 || wpos != -2))
    { curwp->w_dotp = lp;
      curwp->w_line_no += i - n - wpos;
      (void)forwline(0, curwp->w_ntrows >> 1);
    }
    return TRUE;
}}


/*
 * This command moves the current window down by "arg" lines. Recompute the
 * top line in the window. The move up and move down code is almost completely
 * the same; most of the work has to do with reframing the window, and picking
 * a new dot. We share the code by having "move down" just be an interface to
 * "move up". Magic. Bound to "C-X C-N".
 */
int Pascal mvdnwind(int f, int n)

{
  return mvupwind(f, -n);
}


/*
 * This command makes the current window the only window on the screen. Bound
 * to "C-X 1". Try to set the framing so that "." does not have to move on the
 * display. Some care has to be taken to keep the values of dot and mark in
 * the buffer structures right if the destruction of a window makes a buffer
 * become undisplayed.
 */
int Pascal onlywind(int f, int n)

{	WINDOW *wp;
	WINDOW *nwp;
	if (curbp == NULL)
	  return FALSE;

	for (wp = wheadp; wp != NULL; wp = nwp)
	{ nwp = wp->w_wndp;
	  if (wp != curwp)
	  { //tcapbeep();
	    if (wheadp == wp)
	      wheadp = wheadp->w_wndp;
	    leavewind(wp, 1);
	    free((char *)wp);
	  }
	}
#if 0
  (void)backline(0, curwp->w_toprow);
#endif
	wp = wheadp;
	curwp = wp;
	wp->w_wndp = NULL;
	wp->w_toprow = 0;
	wp->w_ntrows = term.t_nrowm1-1;
	wp->w_flag  |= WFMODE|WFHARD;
	return TRUE;
}

/*
 * Delete the current window, placing its space in the window above,
 * or, if it is the top window, the window below. Bound to C-X 0.
 */
int Pascal delwind(int f, int n)
				/* arguments are ignored for this command */
{
	WINDOW *wp;		/* window to recieve deleted space */
	WINDOW *pwp = backbyfield(&wheadp, WINDOW, w_wndp);		/* previous */
	WINDOW *nwp = NULL;

	if (wheadp->w_wndp == NULL)
	{ mlwrite(TEXT204);
/*			"Can not delete this window" */
	  return FALSE;
	}
			     /* find receiving window and give up our space */
{	int top = curwp->w_toprow;
	int nxttop = top + curwp->w_ntrows + 1;
        
	for (wp = wheadp; wp != NULL; wp = wp->w_wndp)
	{ if (wp->w_wndp == curwp)
	    pwp = wp;
				 /* find window before curwp in linked list */
	  if ((wp->w_toprow + wp->w_ntrows + 1) == top)
	    nwp = wp;
	  if (wp->w_toprow == nxttop)
	  { nwp = wp;
	    nwp->w_toprow = top;
	  }
	}

	if (nwp == NULL)
	  return FALSE;
	nwp->w_ntrows += 1 + curwp->w_ntrows;
	nwp->w_flag |= WFHARD | WFMODE;
	leavewind(curwp, 1);
					   /* get rid of the current window */
	pwp->w_wndp = curwp->w_wndp;

	free((char *)curwp);
	curwp = nwp;
	curbp = nwp->w_bufp;
	/*updall(curwp)*/nwp->w_flag |= WFHARD;	/* update all lines */
	/* refresh(0, 0); */
	return TRUE;
}}

/*
Split the current window.  A window smaller than 3 lines cannot be
split.	(Two line windows can be split when mode lines are disabled) An
argument of 1 forces the cursor into the upper window, an argument of
two forces the cursor to the lower window.  The only other error that
is possible is a "malloc" failure allocating the structure for the new
window.  Bound to "C-X 2". 
*/
int Pascal splitwind(int f, int n)

{	WINDOW * wp = (WINDOW *) aalloc(sizeof(WINDOW));
	if (wp == NULL)
	  return FALSE;

	curwp->w_flag |= WFMODE|WFHARD;
	*wp = *curwp;

{	int	ntrd = (curwp->w_ntrows-1);
				     /* make sure we have enough space */
	if (ntrd <= 3)
	{ mlwrite(TEXT205, ntrd+1);
					/* "Cannot split a %d line window" */
	  return FALSE;
	}

{	int	ntru = ntrd >> 1; 	/* Upper size */
	int	ntrl = ntrd - ntru;	/* Lower size */

	ntrd = getwpos() - 1 - ntru;

	if (f == FALSE ? ntrd <= 0 : n == 1)
	{ 					     									/* Old is upper window. */
	  wp->w_toprow += ntru+1;
	  wp->w_ntrows = ntrl;
	  wp->w_wndp = curwp->w_wndp;
	  curwp->w_wndp = wp;
		ntrl = ntru;
	} 
	else					      							/* Old is lower window */
	{ WINDOW * wp1 = (WINDOW*)prevele((BUFFER*)wheadp,(BUFFER*)curwp);
	  if (wp1 == null)
	    wheadp = wp;
	  else
	    wp1->w_wndp = wp;

	  wp->w_wndp = curwp;
	  wp->w_ntrows = ntru;
	  ntrd = ntru;
	  curwp->w_toprow += ntru + 1;
	}
	curwp->w_ntrows = ntrl;
{	LINE* lp = curwp->w_linep;
	for  (; ntrd-- >= 0;)
	  lp = lforw(lp);

	wp->w_linep = lp;					/* if necessary.	*/
	curwp->w_linep = lp;			/* Adjust the top lines */
	curbp->b_wlinep = lp;
	++curbp->b_nwnd;					/* Displayed twice.	*/
{ extern LINE * g_lastlp;		// -ve => reset
	g_lastlp = NULL;
	modeline(curwp);
	g_lastlp = NULL;
	modeline(wp);
	return TRUE;
}}}}}


/* Enlarge the current window. Find the window that loses space. Make sure it
 * is big enough. If so, hack the window descriptions, and ask redisplay to do
 * all the hard work. You don't just set "force reframe" because dot would
 * move. Bound to "C-X Z".
 */
int Pascal enlargewind(int f, int n)

{   WINDOW * wp = curwp;
    WINDOW * adjwp;

    if (wheadp->w_wndp == NULL)
      return FALSE;

    adjwp = wp->w_wndp;
    if (adjwp == NULL)
    	adjwp = wheadp;

    if (n < 0)
    { n = -n;
      wp = adjwp;	
      adjwp = curwp;
    }
    if (adjwp->w_ntrows - n <= 0)
    { mlwrite(TEXT207);
						/* "Impossible change" */
      return FALSE;
    }

    wp->w_ntrows += n;
    wp->w_flag |= WFMODE|WFHARD;
    adjwp->w_flag |= WFMODE|WFHARD;
    adjwp->w_ntrows -= n;

{		int wh = adjwp == wheadp;
		LINE * p;

    if (wh)    						// Shrink above
		{	adjwp = wp;
      n = -n;
    }
    adjwp->w_toprow += n;
		p = adjwp->w_bufp->b_baseline;
		if (wh)
		{	p = lforw(p);
			n = -n;
		}

{		LINE * lp = adjwp->w_linep;

		while (--n >= 0 && lp != p)
     	lp = lmove(lp,wh);
    adjwp->w_linep  = lp;
    return TRUE;
}}}

/*
 * Shrink the current window. Find the window that gains space. Hack at the
 * window descriptions. Ask the redisplay to do all the hard work. Bound to
 * "C-X C-Z".
 */
int Pascal shrinkwind(int f, int n)

{	
	return enlargewind(f, n == 0 ? -1 : -n);
#if 0
X	if (wheadp->w_wndp == NULL)
X	{ mlwrite(TEXT206);
X/*			"Only one window" */
X	  return FALSE;
X	}
X	adjwp = curwp->w_wndp;
X	if (adjwp == NULL)
X
X	if ((curwp->w_ntrows) <= n)
X	{ mlwrite(ichg_msg);
X/*			"Impossible change" */
X	  return FALSE;
X	}
X	if (curwp->w_wndp != adjwp)	
X	{ lp = curwp->w_linep;		/* Grow above.		*/
X	  for (i = n; --i >= 0 && lp != curbp->b_baseline; )
X	    lp = lforw(lp);
X	  curwp->w_linep  = lp;
X	  curwp->w_toprow += n;
X	}
X	else				/* Grow below.	       */
X	{ LINE * p = lforw(adjwp->w_bufp->b_baseline);
X	  lp = adjwp->w_linep;
X	  for (i = n; --i >= 0 && lp != p; )
X	    lp = lback(lp);
X	  adjwp->w_linep  = lp;
X	  adjwp->w_toprow -= n;
X	} 
X	curwp->w_ntrows -= n;
X	adjwp->w_ntrows += n;
X	curwp->w_flag |= WFMODE|WFHARD;
X	adjwp->w_flag |= WFMODE|WFHARD;
X	return TRUE;
#endif
}

			 /* Resize the current window to the requested size */
int Pascal resize(int f, int n)

{
	int clines = curwp->w_ntrows;/* current # of lines in window */
	
		/* must have a non-default argument, else ignore call */
				      /* find out what to do */
	return ! f || clines == n ? TRUE	 /* already the right size? */
			   	                  : enlargewind(TRUE, n - clines);
}

#if 0

/* Pick a window for a pop-up. Split the screen if there is only one window.
 * Pick the uppermost window that isn't the current window. An LRU algorithm
 * might be better. Return a pointer, or NULL on error.
 */

WINDOW *Pascal wpopup()

{	register WINDOW *wp;

	if (wheadp->w_wndp == NULL		/* Only 1 window	*/
	  && splitwind(FALSE, 0) == FALSE)	/* and it won't split	*/
	  return NULL;
					      /* Find window to use   */
	for (wp = wheadp; wp != NULL && wp == curwp; wp = wp->w_wndp)
	  ;
	return wp;
}

#endif

int Pascal nextup(int f, int n)	/* scroll the next window up (back) a page */

{	nextwind(FALSE, 1);
	backpage(f, n);
	return prevwind(FALSE, 1);
}

int Pascal nextdown(int f, int n)	/* scroll next window down (forward) a page*/

{	return nextup(f, -n);
}

#if 0
																												// This code is not safe
static WINDOW *swindow = NULL; 	/* saved window pointer 	*/

int Pascal savewnd(int f, int n)	/* save ptr to current window */

{	swindow = curwp;
	return TRUE;
}

int Pascal restwnd(int f, int n)	/* restore the saved screen */

{
	WINDOW *wp;
					     /* find the window */
	for (wp = wheadp; wp != NULL; wp = wp->w_wndp) 
		if (wp == swindow)
		{	curwp = wp;
			curbp = wp->w_bufp;
			upmode();
			return TRUE;
		}

	mlwrite(TEXT208);
				/* "[No such window exists]" */
	return FALSE;
}

#endif

#if 0				       
const static char text209[] = TEXT209;
#endif

int Pascal newdims(int wid, int dpth)	/* resize screen re-writing the screen */

{ // int inr = true;
                      					     /* make sure it's in range */
	if (! in_range(dpth, 3, 62))
	  dpth = 62;

	if (! in_range(wid, 10, 134))
	  wid = 134;

	if (term.t_nrowm1 != dpth - 1 ||
	    term.t_ncol != wid)
	{ term.t_ncol = wid;
	  term.t_margin = wid / 10;
	  term.t_scrsiz = wid - (term.t_margin * 2);
		term.t_nrowm1 = dpth - 1;			
		term.t_mrowm1 = dpth - 1;			
	  vtinit();
	  tcapsetsize(wid,dpth);
#if 0
	  curwp->w_ntrows = term.t_nrowm1-1;
#else
    onlywind(FALSE, 1);		/* can only make this work */
#endif
		upwind();
//  tcapepage();
#if S_WIN32
	{ char buf[35];
	  mlwrite(strpcpy(&buf[0], lastmesg, 30));
	}
#endif
	  pd_sgarbf = TRUE;			  /* screen is garbage */
	}
#if 0				       
	if (!inr)
	{ mlwrite(text209);
					/* "Impossible screen size" */
	}
#endif
	return true /*inr*/;
}


	/* get screen offset of current line in current window */

int Pascal getwpos()

{ LINE *lp;
  int sline = curwp->w_ntrows;
  
  for (lp = curwp->w_linep;
       lp != curwp->w_dotp;
       lp = lforw(lp))
  { if (lp->l_props & L_IS_HD)
      return -2;
    if (--sline <= 0)
      return -1;
  }

  return curwp->w_ntrows - sline;
}
