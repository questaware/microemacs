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

static REGION g_region;

/* This routine figures out the bounds of the region in the current
 * window, and fills in the fields of the "REGION" structure pointed to by
 * "rp". Because the dot and mark are usually very close together, we scan
 * outward from dot looking for mark. This should save time. Return a
 * standard code. Callers of this routine should be prepared to get an
 * "ABORT" status; we might make this have the confirm thing later.
 */
REGION * Pascal getregion()
{
  if (curwp->mrks.c[0].markp == NULL)
  {	mlwrite(TEXT76);
					/* "No mark set in this window" */
	  return NULL;
	}

{	int wh = 0;
	int _lines = 0;
	int _size = (int)curwp->mrks.c[0].marko - curwp->w_doto;

  LINE * flp = curwp->w_dotp;
	LINE * blp = flp;
	g_region.r_linep = flp;

	if (flp == curwp->mrks.c[0].markp)
	{	_lines = 1;
		if (_size < 0)
		{ _size = - _size;
			wh = 1;
		}
	}
	else
	{	int bsize = -_size;

		while (true)
		{ _lines += 1;

			if (blp == curwp->mrks.c[0].markp)
			{ wh = 1;
				g_region.r_linep = blp;
				_size = bsize;
				break;
			}

			blp = lback(blp);
			if ((blp->l_props & L_IS_HD) == 0)
				bsize += llength(blp)+1;

			if (flp == curwp->mrks.c[0].markp)
				break;

			if      ((flp->l_props & L_IS_HD) == 0)
			{ _size += llength(flp)+1;
				flp = lforw(flp);
			}
			else if ((blp->l_props & L_IS_HD) != 0)
			{	adb(99);
				break;
			}
		}
	}

	g_region.r_lines = _lines;
	g_region.r_size = _size;
	g_region.r_offset = wh == 0 ? curwp->w_doto : curwp->mrks.c[0].marko;

	return &g_region;
}}

static char * g_doregt;
/* doregion */

static
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
		char ch;

		while (offs >= 0 && 
               ((ch = lp[offs]) == '_' || ch == '-' || isalnum(ch)))
			--offs;

    while (++offs < len && 
               ((ch = lp[offs]) == '_' || ch == '-' || isalnum(ch)))
    { int cc = kinsert(ch);
	    if (cc <= FALSE)
	      return cc;
    }
  }
  else
	{ LINE  *linep;
	  int	 loffs;
		int  space = NSTRING-1;
	  REGION * ion = getregion();
	  if (ion == NULL)
	    return 0;

	  linep = ion->r_linep; 		/* Current line.	*/

		    /* don't let the region be larger than a string can hold */
	  if (wh == 0 && ion->r_size >= NSTRING)
	    ion->r_size = NSTRING - 1;

	  for (loffs = ion->r_offset; g_region.r_size--; ++loffs)
	  { int ch;
	  	if (loffs != llength(linep))		/* End of line. 	*/
	      ch = lgetc(linep, loffs);
	    else
	    { linep = lforw(linep);
	      loffs = -1;
	      ch = '\n';
	    } 
	    if      (wh == 0)
	    { if (--space >= 0)
	        *g_doregt++ = ch;
	    }
	    else if (wh == 1) 
	    { ch = kinsert(ch);
	      if (ch <= FALSE)
	        return ch;
	    }
	    else
	    { if (isalpha(ch) && isupper(ch) == wh - 2)
	      { lchange(WFHARD);
	        lputc(linep, loffs, chcaseunsafe(ch));
	      }
	    }
	  }
	  if (wh == 0)
	    *g_doregt = 0;
	}
	return TRUE;
}

/* return some of the contents of the current region
*/
const char *Pascal getreg(char * t)

{ g_doregt = t;
  return doregion(0) <= FALSE ? g_logm[2] : t;
}


static
int to_kill_buff(int wh, int n)

{ kinsert_n = chk_k_range(n);
  if (kinsert_n < 0)
    return 1;

	if (wh < 0)
	  (void)kdelete(0, n);

{ int cc = doregion(wh);
  if (cc <= FALSE)
    return cc;
  
#if S_WIN32
  if (kinsert_n == 0)
  { Char * src = getkill();
    ClipSet(src);
  }
#endif
  mlwrite(TEXT70);
				/* "[region copied]" */
  g_thisflag |= CFKILL;
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


													/*	reglines:	how many lines in the current region */
int Pascal reglines()

{	REGION * r = getregion();	   							/* check for a valid region first */
	if (r == NULL)
		return 0;
									  							 /* place us at the beginning of the region */
  curwp->w_dotp = r->r_linep;
  curwp->w_doto = r->r_offset;

	return r->r_lines;
}

/*
 * Kill the region. Ask "getregion" to figure out the bounds of the region.
 * Move "." to the start, and kill the characters into the n.th kill buffer.
 * Not Bound.
 */
int Pascal killregion(int f, int n)

{	if (curbp->b_flag & MDVIEW)	/* don't allow this command if	*/
	  return rdonly();		/* we are in read only mode	*/

{	int s = reglines();
	if (s == 0)
	  return FALSE;

/*if ((g_lastflag&CFKILL) == 0)		** This is a kill type	**
    kdelete() */;								/* command, so do magic */
	g_thisflag |= CFKILL;			/* kill buffer stuff.	*/
	kinsert_n = chk_k_range(n);
	return ldelchrs(g_region.r_size, TRUE);
}}

/*	Narrow-to-region (^X-<) makes all but the current region in
	the current buffer invisable and unchangable
*/

int Pascal narrow(int f, int n)

{													/* find buffer and ensure it's not already narrow */
	BUFFER* bp = curwp->w_bufp;
	if (bp->b_flag & BFNAROW)
	{ mlwrite(TEXT71);
				 /* "%%Buffer already narrowed" */
	  return FALSE;
	}
									       /* find the boundries of the current region */
{	int status = reglines();
	if (status <= 1)
	{ mlwrite(TEXT72);
					/* "%%Must narrow at least 1 full line" */
	  return FALSE;
	}										   								/* move forward to the end of this region
										  					   		  	 (a long number of bytes perhaps) */
	forwchar(TRUE, g_region.r_size);
//curwp->w_doto = 0;										/* only by full lines please! */

	bp->b_flag |= BFNAROW;								/* remember we are narrowed */
	bp->b_baseline.l_fp = g_region.r_linep;
	bp->b_narlims[0] = lback(g_region.r_linep);
	lback(g_region.r_linep) = &bp->b_baseline;

	bp->b_baseline.l_bp = curwp->w_dotp;
	bp->b_narlims[1] = lforw(curwp->w_dotp);
	lforw(curwp->w_dotp) = &bp->b_baseline;
#if 1
	rpl_all(-1, 0, (LINE*)bp, g_region.r_linep, 0);
#else
{ WINDOW *wp;
																	   /* let all the proper windows be updated */
	for (wp = wheadp; wp; wp = wp->w_next) 
	  if (wp->w_bufp == bp)
	  { MARK * m;
	    wp->w_linep = g_region.r_linep;
	    wp->w_dotp = g_region.r_linep;
	    wp->w_doto = 0;
	    for (m = &wp->mrks.c[NMARKS - 1]; m >= &wp->mrks.c[0]; --m)
	    { m->markp = g_region.r_linep;
	      m->marko = 0;
	    }
	    wp->w_flag |= (WFHARD|WFMODE);
	  }
}
#endif
	setcline(0);

	mlwrite(TEXT73);
				/* "[Buffer is narrowed]" */
	return TRUE;
}}

/*	widen-from-region (^X->) restores a narrowed region	*/

int Pascal widen(int f, int n)

{		      								/* find the proper buffer and ensure we are narrow */
	BUFFER * bp = curwp->w_bufp;
	if ((bp->b_flag & BFNAROW) == 0)
	{ mlwrite(TEXT74);
					/* "%%This buffer is not narrowed" */
	  return FALSE;
	}

	bp->b_flag &= (~BFNAROW);
{	LINE * bl = &bp->b_baseline;
	LINE ** nlims = bp->b_narlims-1;
	int iter;
	for (iter = 2; --iter >= 0; )
	{	LINE *lp;
		++nlims;
		if (*nlims != NULL)
	  { LINE * nl = *nlims;
	  	for (lp = nl;
	         lmove(lp,iter) != bl;  lp = lmove(lp,iter))
	      ;
	    if (iter)			 						/* recover the top fragment */
	    { nl->l_fp = (Lineptr)bl->l_fp;
	      lforw(nl)->l_bp = (Lineptr)nl;
	      bl->l_fp = (Lineptr)lp;
	    }
	    else			      		 			/* recover the bottom fragment */
	    { nl->l_bp = bl->l_bp;
	    	bl->l_bp->l_fp = nl;
	      bl->l_bp = (Lineptr)lp;
	    }
//    *nlims = NULL;
	  }
	}

	setcline(0);
	orwindmode(WFHARD|WFMODE, 0);

	mlwrite(TEXT75);
				/* "[Buffer is widened]" */
	return TRUE;
}}

