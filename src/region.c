/* The routines in this file
 * deal with the region, that magic space
 * between "." and mark. Some functions are
 * commands. Some functions are just for
 * internal use.
 */
#include <stdio.h>
#include "estruct.h"
#include "edef.h"
#include "etype.h"
#include "elang.h"

REGION region;

/* This routine figures out the bounds of the region in the current
 * window, and fills in the fields of the "REGION" structure pointed to by
 * "rp". Because the dot and mark are usually very close together, we scan
 * outward from dot looking for mark. This should save time. Return a
 * standard code. Callers of this routine should be prepared to get an
 * "ABORT" status; we might make this have the confirm thing later.
 */
int Pascal getregion()
{
  if (curwp->mrks.c[0].markp != NULL)
  {	register LINE * flp = curwp->w_dotp;

	region.r_linep = flp;
	region.r_lines = 0;
        region.r_offset = curwp->w_doto;
	region.r_size = (Int)curwp->mrks.c[0].marko-region.r_offset;
  {	Int   bsize = -region.r_size;
	register LINE * blp = flp;

	while (true)
	{ region.r_lines += 1;

	  if (flp == curwp->mrks.c[0].markp)
	  { if (region.r_size < 0)
	    { region.r_offset = curwp->mrks.c[0].marko;
	      region.r_size = -region.r_size;
	    }
	    return region.r_size;
	  }
	  if      ((flp->l_props & L_IS_HD) == 0)
	  { region.r_size += llength(flp)+1;
	    flp = lforw(flp);
	  }

	  if ((lback(blp)->l_props & L_IS_HD) == 0)
	  { blp = lback(blp);
	    bsize += llength(blp)+1;
	    if (blp == curwp->mrks.c[0].markp)
	    { region.r_linep = blp;
	      region.r_offset = curwp->mrks.c[0].marko;
	      region.r_size = bsize;
	      return bsize;
	    }
	  }
	  else if ((flp->l_props & L_IS_HD) &&
	           flp != curwp->mrks.c[0].markp)
	    break;
	}
	adb(99);
   }}
   mlwrite(TEXT76);
		/*	"No mark set in this window" */
   return 0;
}

static char * doregt;
/* doregion */

Pascal doregion(int wh)
	
{ 	
	if (wh > 1 && curbp->b_flag & MDVIEW)	/* disallow this command if */
	  return rdonly();		        /* we are in read only mode */

	if (wh < 0)
	{ 
		int offs =  curwp->w_doto;
		LINE * ln = curwp->w_dotp;
		int len = ln->l_used;
		char * lp = lgets(ln, 0);
		register int s;

		while (offs >= 0 && 
                 (isalpha(lp[offs]) || isdigit(lp[offs]) || lp[offs] == '_'))
			--offs;

    while (++offs < len && 
                 (isalpha(lp[offs]) || isdigit(lp[offs]) || lp[offs] == '_'))
    { s = kinsert(lp[offs]);
	    if (s != TRUE)
	      return s;
    }
  }
  else
	{ register LINE  *linep;
	  register int	loffs;
		   int    space = NSTRING-1;
	  register int s = getregion();
	  if (! s)
	    return s;

	  linep = region.r_linep; 		/* Current line.	*/

		    /* don't let the region be larger than a string can hold */
	  if (wh == 0 && region.r_size >= NSTRING)
	    region.r_size = NSTRING - 1;

	  for (loffs = region.r_offset; region.r_size--; ++loffs)
	  { if (loffs == llength(linep))		/* End of line. 	*/
	    { s = '\n';
	      linep = lforw(linep);
	      loffs = -1;
	    } 
	    else					/* Middle of line.	*/
	      s = lgetc(linep, loffs);
	    if      (wh == 0)
	    { if (--space >= 0)
	        *doregt++ = s;
	    }
	    else if (wh == 1) 
	    { s = kinsert(s);
	      if (s != TRUE)
	        return s;
	    }
	    else
	    { if (isalpha(s) && isupper(s) == wh - 2)
	      { lchange(WFHARD);
	        lputc(linep, loffs, chcaseunsafe(s));
	      }
	    }
	  }
	  if (wh == 0)
	    *doregt = 0;
	}
	return TRUE;
}

/* return some of the contents of the current region
*/
char *Pascal getreg(char * t)

{ doregt = t;
  return doregion(0) != TRUE ? errorm : t;
}



int to_kill_buff(int wh, int n)

{ kinsert_n = chk_k_range(n);
  if (kinsert_n < 0)
    return 1;

	if (wh < 0)
	  (void)kdelete(0, n);

{ int cc = doregion(wh);
  if (cc != TRUE)
    return cc;
  
#if S_WIN32
  if (kinsert_n == 0)
  { Char * src = getkill();
    ClipSet(src);
  }
#endif
  mlwrite(TEXT70);
/*		"[region copied]" */
  thisflag |= CFKILL;
  return cc;
}}
 


/* Append all of the characters in the region to the n.th kill buffer. 
 * Don't move dot at all. 
 * Bound to "^XC".
 */
int copyregion(int f, int n)

{ return to_kill_buff(1, n);
}


/* Copy all of the characters in the current word to the n.th kill buffer.
 * Don't move dot at all. 
 * Bound to "A-W".
 */
int copyword(int f, int n)

{ return to_kill_buff(-1, n);
}



/* Lower case region. Zap all of the upper
 * case characters in the region to lower case. Use
 * the region code to set the limits. Scan the buffer,
 * doing the changes. Call "lchange" to ensure that
 * redisplay is done in all buffers. Bound to
 * "C-X C-L".
 */
Pascal lowerregion(int f, int n)

{ return doregion(2 + true);
}

/* Upper case region. Zap all of the lower
 * case characters in the region to upper case. Use
 * the region code to set the limits. Scan the buffer,
 * doing the changes. Call "lchange" to ensure that
 * redisplay is done in all buffers. Bound to
 * "C-X C-U".
 */
int Pascal upperregion(int f, int n)

{ return doregion(2 + false);
}


/*	reglines:	how many lines in the current region
			used by the trim/entab/detab-region commands
*/

int Pascal reglines()

{				      /* check for a valid region first */
	if (getregion())
	{			  /* place us at the beginning of the region */
	  curwp->w_dotp = region.r_linep;
	  curwp->w_doto = region.r_offset;
	}
	return region.r_lines;
}

/*
 * Kill the region. Ask "getregion" to figure out the bounds of the region.
 * Move "." to the start, and kill the characters into the n.th kill buffer.
 * Not Bound.
 */
int Pascal killregion(int f, int n)

{	if (curbp->b_flag & MDVIEW)	/* don't allow this command if	*/
	  return rdonly();		/* we are in read only mode	*/
{	register int s = reglines();
	if (s == 0)
	  return FALSE;
  /*if ((lastflag&CFKILL) == 0)		** This is a kill type	**
	    kdelete() */;			/* command, so do magic */
	thisflag |= CFKILL;			/* kill buffer stuff.	*/
	kinsert_n = chk_k_range(n);
	return ldelchrs(region.r_size, TRUE);
}}

/*	Narrow-to-region (^X-<) makes all but the current region in
	the current buffer invisable and unchangable
*/

int Pascal narrow(int f, int n)

{		/* find the proper buffer and ensure it's not already narrow */
	register BUFFER* bp = curwp->w_bufp;
	if (bp->b_flag & BFNAROW)
	{ mlwrite(TEXT71);
/*			"%%This buffer is already narrowed" */
	  return FALSE;
	}
			       /* find the boundries of the current region */
{	int status = reglines();
	if (status == 0)
	  return FALSE;

	curwp->w_doto = 0;			/* only by full lines please! */
	region.r_size += (Int)region.r_offset;
	if (region.r_size <= (Int)llength(curwp->w_dotp))
	{ mlwrite(TEXT72);
/*			"%%Must narrow at least 1 full line" */
	  return FALSE;
	}
			   				/* move forward to the end of this region
				     		   (a long number of bytes perhaps) */
	while (region.r_size > (Int)32000)
	{ forwchar(TRUE, 32000);
	  region.r_size -= (Int)32000;
	}
	forwchar(TRUE, (int)region.r_size);
	curwp->w_doto = 0;			    /* only full lines! */

					       /* archive the top fragment */
	if (lforw(bp->b_baseline) != region.r_linep)
	{ bp->b_narlims[0] = lforw(bp->b_baseline);
	  lback(region.r_linep)->l_fp = (Lineptr)0L;
	  bp->b_baseline->l_fp = (Lineptr)region.r_linep;
	  region.r_linep->l_bp = (Lineptr)bp->b_baseline;
	}
					      
	if (bp->b_baseline != curwp->w_dotp)     /* archive the bottom fragment */
	{ bp->b_narlims[1] = curwp->w_dotp;
	  lback(bp->b_narlims[1])->l_fp = (Lineptr)bp->b_baseline;
	  lback(bp->b_baseline)->l_fp = (Lineptr)0L;
	  bp->b_baseline->l_bp = bp->b_narlims[1]->l_bp;
	}
#if 1
	rpl_all((LINE*)bp, region.r_linep, -1, 0, 0);
#else
{  	register WINDOW *wp;
				   /* let all the proper windows be updated */
	for (wp = wheadp; wp; wp = wp->w_wndp) 
	  if (wp->w_bufp == bp)
	  { MARK * m;
	    wp->w_linep = region.r_linep;
	    wp->w_dotp = region.r_linep;
	    wp->w_doto = 0;
	    for (m = &wp->mrks.c[NMARKS - 1]; m >= &wp->mrks.c[0]; --m)
	    { m->markp = region.r_linep;
	      m->marko = 0;
	    }
	    wp->w_flag |= (WFHARD|WFMODE);
	  }
}
#endif
				       
	bp->b_flag |= BFNAROW;		/* and now remember we are narrowed */
	mlwrite(TEXT73);
/*		"[Buffer is narrowed]" */
	return TRUE;
}}

/*	widen-from-region (^X->) restores a narrowed region	*/

int Pascal widen(int f, int n)

{	register LINE *lp;	/* temp line pointer */
	int iter;

		      /* find the proper buffer and make sure we are narrow */
	BUFFER * bp = curwp->w_bufp;		/* find the right buffer */
	if ((bp->b_flag & BFNAROW) == 0)
	{ mlwrite(TEXT74);
/*			"%%This buffer is not narrowed" */
	  return FALSE;
	}
	for (iter = -1; ++iter < 2; )
	{ if (bp->b_narlims[iter] != NULL)
	  { for (lp = bp->b_narlims[iter];  
	         lp->l_fp != (Lineptr)0L;  lp = lforw(lp))
	      ;
	    if (iter == 0)			  /* recover the top fragment */
	    { lp->l_fp = (Lineptr)bp->b_baseline->l_fp;
	      lforw(lp)->l_bp = (Lineptr)lp;
	      bp->b_baseline->l_fp = (Lineptr)bp->b_narlims[0];
	      bp->b_narlims[0]->l_bp = (Lineptr)bp->b_baseline;
	    }
	    else			       /* recover the bottom fragment */
	    { lp->l_fp = (Lineptr)bp->b_baseline;
	      lback(bp->b_baseline)->l_fp = (Lineptr)bp->b_narlims[1];
	      bp->b_narlims[1]->l_bp = bp->b_baseline->l_bp;
	      bp->b_baseline->l_bp = (Lineptr)lp;
	    }
	    bp->b_narlims[iter] = NULL;
	  }
	}
				   /* let all the proper windows be updated */
	bp->b_flag &= (~BFNAROW);
	orwindmode(WFHARD|WFMODE, 0);
				     /* and now remember we are not narrowed */
	mlwrite(TEXT75);
/*		"[Buffer is widened]" */
	return TRUE;
}

