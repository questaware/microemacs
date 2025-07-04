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



WINDOW * Pascal window_ct(BUFFER* bp)

{ WINDOW * wp;
  WINDOW * res = NULL;
	int ct = 0;
  for (wp = wheadp; wp != NULL; wp = wp->w_next)
  	if (wp->w_bufp == bp)
  	{ res = wp;
  		++ct;
  	}
  	
  bp->b_window_ct = ct;
  return res;
}



int Pascal orwindmode(int mode)

{ WINDOW *wp;
  int ct = 0;
				   /* force all windows to redraw */
  for (wp = wheadp; wp != NULL; wp = wp->w_next)
  { wp->w_flag |= (mode /*& (WFONLY-1)*/);
    ++ct;
  }

  return ct;
}

void Pascal  openwind(WINDOW * wp)
	
{ BUFFER * bp = curbp;
	*(WUFFER*)wp = *(WUFFER*)bp;
	wp->w_bufp    = bp;	     /* connect current window to this buffer */
	wp->w_line_no = 1;
//wp->w_linep   = lforw(bp->b_baseline);
//wp->w_dotp    = bp->b_dotp;
//wp->w_doto    = bp->b_doto;
  wp->w_flag    = WFMODE|WFHARD|WFFORCE;
//memset(&wp->mrks, 0, sizeof(MARKS));
}


WINDOW * USE_FAST_CALL leavewind(int dec, WINDOW * wp_)
	
{ WINDOW * wp = wp_ == NULL ? curwp : wp_;
  BUFFER * bp = wp->w_bufp;
	if (bp != NULL)
	  *(WUFFER*)bp = *(WUFFER*)wp;

	if (dec)
		free((char *)wp);

/*bp->b_fcol	 = wp->w_fcol;*/
	return wp;
}

extern int g_top_luct;

void openwindbuf(char * bname)
	
{		/* split the current window to make room for the binding list */
  if (splitwind(FALSE, 1) == FALSE)
    return;
					        
{	BUFFER * bp = bfind(bname, TRUE);		/* and get a buffer for it */
  if (bp == NULL)
    return;

	curbp = bp;

  if (bclear(bp))
  { bp->b_flag &= ~MDVIEW;
//  bp->b_flag |= BFACTIVE;
		bp->b_luct = ++g_top_luct;
    
  { WINDOW* wp = leavewind(0, NULL);
  	curwp = wp->w_next;
    openwind(curwp);
  }}
  
#if DO_UNDO
	g_inhibit_undo = TRUE;
#endif
}}

/*
 * Refresh the screen. With no argument, it just does the refresh.
 * otherwise the line becomes line n from the top/bottom.  Bound to M-^L.
 */
int Pascal refresh(int f, int n)

{ if (f != FALSE)
	{	curwp->w_force = n;
    curwp->w_flag |= WFFORCE;
  }
 
	upwind(TRUE);

  return TRUE;
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

{ WINDOW *wp = leavewind(0, NULL);		// curwp
  wp->w_flag |= WFMODE;		/* Mode line is updated */

	if (! f)
	{ wp = wp->w_next;
		if (wp == NULL)
	    wp = wheadp;
	}
	else
	{ int wct = orwindmode(0);  /* 0,: just count windows */
	
    if (n < 0)
      n = ((wct+n) % wct)+1;  /* the nth window from bottom of the screen */
	  
	  wp = wheadp;
	  while (--n > 0)
	  { wp = wp->w_next;
	    if (wp == NULL)
	      wp = wheadp;
	  }
	}
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
	curwp = (WINDOW*)prevele(0,(BUFFER*)curwp);
	return TRUE;
}


	/* get screen offset of current line in current window */

int Pascal getwpos()

{ LINE *lp;
  int sline = curwp->w_ntrows;
  
  for (lp = curwp->w_linep;
       lp != curwp->w_dotp;
       lp = lforw(lp))
  { if (l_is_hd(lp))
      return -2;				// An error
    if (--sline <= 0)
	    return -1;
  }

  return curwp->w_ntrows - sline;
}


/* Move the current window up by "arg" lines. Recompute the new top line of
 * the window. Look to see if "." is still on the screen. If it is, you win.
 * If it isn't, then move "." to center it in the new framing of the window
 * (this command does not really move "."; it moves the frame). Bound to
 * "C-X C-P".
 */
int Pascal mvupwind(int notused, int n)

{   int wpos = getwpos();
#if S_MSDOS == 0
                                      // W ^ : n == 1   goto end, 1 down
                                      // W V : n == -1  goto top, 1 up
    if (n == 1 || n == -1)
    { Lpos_t lpos = *(Lpos_t*)&curwp->w_dotp;
      int offs = lpos.curoff;
      int mv = n > 0 ? -wpos : curwp->w_ntrows - wpos - 1;
      (void)forwbyline(mv);
      update(FALSE);
      forwbyline(-n);
      update(FALSE);
      if      (wpos == 0 && n < 0)        // keep line in window
      { lpos.curline = lforw(lpos.curline);
        lpos.curoff = 0;
        lpos.line_no += 1;
      }
      else if (curwp->w_ntrows == wpos + 1 && n > 0)
      { lpos.curline = lback(lpos.curline);
        lpos.curoff = 0;
        lpos.line_no -= 1;
      }
      rest_l_offs(&lpos);
      (void)forwbyline(1);			// strange
      if (offs)
      	forwbychar(offs);
    }
		else
#endif
    { WINDOW * wp = curwp;
    	LINE * lp = wp->w_linep;
      int i = n;

      if (i < 0)
      { --i;
        while (!l_is_hd(lp) && ++i < 0)
          lp = lforw(lp);
      }
      else
      { LINE * p = lforw(&wp->w_bufp->b_baseline);
        ++i;
        while (--i > 0 && lp != p)
          lp = lback(lp);
      }

      wp->w_flag |= WFHARD;
      wp->w_linep = lp;
      
      if (getwpos() < 0 && wpos >= 0)
      {	wp->w_dotp = lp;
        wp->w_line_no += i - n - wpos;
	      (void)forwbyline(wp->w_ntrows >> 1);
	    }
    }
	  return TRUE;
}


/*
 * This command moves the current window down by "arg" lines. Recompute the
 * top line in the window. The move up and move down code is almost completely
 * the same; most of the work has to do with reframing the window, and picking
 * a new dot. We share the code by having "move down" just be an interface to
 * "move up". Magic. Bound to "C-X C-N".
 */
int Pascal mvdnwind(int f, int n)

{
  return mvupwind(-n, -n);
}


static
int Pascal USE_FAST_CALL dowind(int wh)			/* 0: only window, 1: del window */

{	WINDOW *wp;														  /* window to receive deleted space */
	WINDOW *nwp = NULL;
	WINDOW * next;
	WINDOW * cwp = curwp;
													     		/* find receiving window and give up space */
	int top = cwp->w_toprow;
	int nxttop = top + cwp->w_ntrows + 1;

	WINDOW *pwp = backbyfield(g_wheadp_ref, WINDOW, w_next);		/* previous */
        
	for (wp = wheadp; wp != NULL; wp = next)
	{ next = wp->w_next;
		if (next == cwp)
			pwp = wp;
		if (wh == 0)
		{	if (wp != cwp)
		   	leavewind(1, wp);
	    continue;
	  }
																	 							 /* find window before curwp */
	  if ((wp->w_toprow + wp->w_ntrows + 1) == top)
	    nwp = wp;
	  if (wp->w_toprow == nxttop)
	  { nwp = wp;
	    nwp->w_toprow = top;
	  }
	}

	if (wh == 0)
	{	*g_wheadp_ref = cwp;
		cwp->w_next = NULL;
		cwp->w_toprow = 0;
		cwp->w_ntrows = term.t_nrowm1-1;
		cwp->w_flag  |= WFMODE|WFHARD;
	}
	else
	{	if (nwp == NULL)
	  	return FALSE;
		pwp->w_next = cwp->w_next;			/* unlink the current window */
														   					
		nwp->w_ntrows += 1 + cwp->w_ntrows;
		nwp->w_flag |= WFHARD | WFMODE;				/* update all lines */

		leavewind(1,NULL);

		curwp = nwp;
		curbp = nwp->w_bufp;
	/*refresh(0, 0); */
	}

	return TRUE;
}

	
/*
 * This command makes the current window the only window on the screen. Bound
 * to "C-X 1". Try to set the framing so that "." does not have to move on the
 * display. Some care has to be taken to keep the values of dot and mark in
 * the buffer structures right if the destruction of a window makes a buffer
 * become undisplayed.
 */
int Pascal onlywind(int notused, int n)

{	return dowind(0);
}


/*
 * Delete the current window, placing its space in the window above,
 * or, if it is the top window, the window below. Bound to C-X 0.
 */
int Pascal delwind(int notused, int n)
				/* arguments are ignored for this command */
{	return dowind(1);
}

/*
Split the current window.  A window smaller than 3 lines cannot be
split.	(Two line windows can be split when mode lines are disabled) An
argument of 1 forces the cursor into the upper window, an argument of
-1 forces the cursor to the lower window.  The only other error that
is possible is a "malloc" failure allocating the structure for the new
window.  Bound to "C-X 2". 
*/
int Pascal splitwind(int f, int n)

{	WINDOW * wp = (WINDOW *) mallocz(sizeof(WINDOW));
	if (wp == NULL)
	  return FALSE;

	curwp->w_flag |= WFMODE|WFHARD;
	*wp = *curwp;

{	int	ntra = wp->w_ntrows;
				     /* make sure we have enough space */
	if (ntra <= 4)
	{ mlwrite(TEXT205, ntra);
					/* "Cannot split a %d line window" */
	  return FALSE;
	}

{	int	ntru = ntra >> 1; 	/* Upper size */
	int	ntrl = ntra - ntru - 1;	/* Lower size */

	ntra = getwpos() - 1 - ntru;

	if (1 ||(f == FALSE ? ntra <= 0 : n > 0))
	{ 					     									/* Old is upper window. */
	  wp->w_toprow += ntru+1;
	  wp->w_ntrows = ntrl;
	  wp->w_next = curwp->w_next;
	  curwp->w_next = wp;
//	ntrl = ntru;
	} 
	else					      							/* Old is lower window */
	{ WINDOW * wp1 = (WINDOW*)prevele(0,(BUFFER*)curwp);
	  if (wp1 == null)
	    *g_wheadp_ref = wp;
	  else
	    wp1->w_next = wp;

	  wp->w_next = curwp;
	  wp->w_ntrows = ntru;
	  ntra = ntru;
	  curwp->w_toprow += ntru + 1;
	}
	curwp->w_ntrows = ntru;
{	LINE* lp = curwp->w_linep;
	for  (; ntra-- >= 0;)
	  lp = lforw(lp);

	wp->w_linep = lp;					/* if necessary.	*/
	curbp->b_wlinep = lp;
	curwp->w_linep = lp;			/* Adjust the top lines */
{ 
#if S_MSDOS == 0
	extern LINE * g_lastlp;		// -ve => reset
	g_lastlp = NULL;
#endif
//modeline(curwp);
	// g_lastlp = NULL;
//modeline(wp);
	pd_winnew = 1;
	return TRUE;
}}}}}


/* Enlarge the current window. Find the window that loses space. Make sure it
 * is big enough. If so, hack the window descriptions, and ask redisplay to do
 * all the hard work. You don't just set "force reframe" because dot would
 * move. Bound to "C-X Z".
 */
int Pascal enlargewind(int notused, int n)

{   WINDOW * wp = curwp;
    WINDOW * adjwp;

    adjwp = wp->w_next;
    if (adjwp == NULL)
    	adjwp = wheadp;

    if (n < 0)
    { n = -n;
      wp = adjwp;	
      adjwp = curwp;
    }

    if (wheadp->w_next == NULL)
    { // mlwrite(TEXT207);
						/* "Impossible change" */
      return FALSE;
    }

    if (adjwp->w_ntrows - n <= 0)
    	return FALSE;
    	
    adjwp->w_ntrows = adjwp->w_ntrows - n;
    wp->w_ntrows += n;
    wp->w_flag |= WFMODE|WFHARD;

{		int wh = adjwp == wheadp;
		LINE * p;

    if (wh)    						// Shrink above
		{	adjwp = wp;
      n = -n;
    }
    adjwp->w_toprow += n;
		p = &adjwp->w_bufp->b_baseline;
		if (wh)
		{	p = lforw(p);
			n = -n;
		}

{		LINE * lp = adjwp->w_linep;

		while (--n >= 0 && lp != p)
     	lp = lmove(lp,wh);

    adjwp->w_linep  = lp;
    adjwp->w_flag |= WFMODE|WFHARD;
    return TRUE;
}}}

/*
 * Shrink the current window. Find the window that gains space. Hack at the
 * window descriptions. Ask the redisplay to do all the hard work. Bound to
 * "C-X C-Z".
 */
int Pascal shrinkwind(int f, int n)

{	int n_ = n == 0 ? -1 : -n;
	return enlargewind(n_,n_);
}

			 /* Resize the current window to the requested size */
int Pascal resize(int f, int n)

{	int diff = n - curwp->w_ntrows;/* current # of lines in window */
	
									/* must have a non-default argument, else ignore call */
	if (! f)
		return TRUE;

	return enlargewind(diff, diff);
}

#if 0

/* Pick a window for a pop-up. Split the screen if there is only one window.
 * Pick the uppermost window that isn't the current window. An LRU algorithm
 * might be better. Return a pointer, or NULL on error.
 */

WINDOW *Pascal wpopup()

{	register WINDOW *wp;

	if (wheadp->w_next == NULL		/* Only 1 window	*/
	  && splitwind(FALSE, 0) == FALSE)	/* and it won't split	*/
	  return NULL;
					      /* Find window to use   */
	for (wp = wheadp; wp != NULL && wp == curwp; wp = wp->w_next)
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
const static char text209[] = TEXT209;
#endif

void Pascal USE_FAST_CALL newdims(int wid, int dpth)	/* resize screen re-writing the screen */

{ // int inr = true;
                      					     /* make sure it's in range */
	if (dpth > NROW)
	  dpth = NROW;

	if (dpth < 3)
	  dpth = term.t_nrowm1+1;

	if (wid > NCOL)
	  wid = NCOL;

	if (wid < 10)
		wid = term.t_ncol;

	vtinit(wid,dpth-1);

	if (term.t_ncol == wid && term.t_nrowm1 == dpth - 1)
		return;

	term.t_nrowm1 = dpth-1;
	term.t_ncol = wid;
#if MEMMAP == 0
	term.t_margin = wid / 10;
	term.t_scrsiz = wid - (term.t_margin * 2);
#endif
//#if S_WIN32 == 0
	tcapsetsize(wid,dpth);
//#endif
#if 0
	curwp->w_ntrows = term.t_nrowm1-1;
#else
  onlywind(1, 1);		/* can only make this work */
#endif
	upwind(TRUE);
//  tcapepage();
#if S_WIN32
	{ char buf[35];
	  mlwrite(strpcpy(&buf[0], lastmesg, 30));
	}
#endif
//  pd_sgarbf = TRUE;			  /* screen is garbage */

#if 0				       
	if (!inr)
	{ mlwrite(text209);
					/* "Impossible screen size" */
	}
#endif
}
