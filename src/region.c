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

static
REGION g_region;

/* This routine figures out the bounds of the region in the current
 * window, and fills in the fields of the "REGION" structure pointed to by
 * "rp". Because the dot and mark are usually very close together, we scan
 * outward from dot looking for mark. This should save time. 
 */
REGION * Pascal getregion()

{	int lines = 1;
	int wh = 1;
	int offset = curwp->w_doto;
	int size = (int)curwp->mrks.c[0].marko - offset;
	int bsize = -size;

{	LINE* flp = curwp->w_dotp;
	LINE* lp = flp;
	LINE* blp = flp;

  LINE * mark_line = curwp->mrks.c[0].markp;
  if (mark_line == NULL)
  {	mlwrite(TEXT76);
					/* "No mark set in this window" */
	  size = 0; 
	}
	else
		while (true)
		{	lines -= 1;

			if (blp == mark_line)
			{	lp = blp;
				wh = -1;
				size = bsize;
				if (size > 0 || lines < 0)
					offset = (int)curwp->mrks.c[0].marko;
				break;
			}

			if (wh > 0)
			{	blp = lback(blp);
				wh = blp->l_dcr & 1;
				bsize += llength(blp) + wh;
			}

			if (flp == mark_line)
				break;

			if (!l_is_hd(flp))
			{	size += llength(flp) + 1;
				flp = lforw(flp);
			}
		}

	g_region.r_lines = -lines;
	g_region.r_size = size > 0 ? size : -size;		// case: markp == w_dotp 
	g_region.r_offset = offset;
	g_region.r_up = wh < 0;
	g_region.r_linep = lp;

	return &g_region;
}}

													/*	reglines:	how many lines in the current region */
static
int Pascal reglines(Bool ask)

{	REGION * r = getregion();	   							/* check for a valid region first */
	if (r->r_size == 0)
	{ if (!ask || g_macargs > 0)
			return 0;								
	{ int rc = mlyesno(TEXT180);
		if (rc <= 0)		/* Use Whole file? */
			return 0;
		
	  curwp->w_dotp = curwp->w_bufp->b_baseline.l_fp;
  	curwp->w_doto = 0;

		return 10000000;
	}}
											  						/* place us at the beginning of the region */
#if 0
  curwp->w_dotp = r->r_linep;
  curwp->w_doto = r->r_offset;
#else
  memcpy(curwp, r, sizeof(LINE*) + sizeof(int));
#endif

	return r->r_lines;
}


int ask_region()

{	++g_inhibit_undo;
{	Bool rc = rdonly();
	--g_inhibit_undo;
	if (rc)
	  return -1;

{	int lines = reglines(TRUE);
	rdonly();
	return lines;
}}}

/*
 * Kill the region. Ask "getregion" to figure out the bounds of the region.
 * Move "." to the start, and kill the characters into the n.th kill buffer.
 * Not Bound.
 */
int Pascal killregion(int f, int n)

{ int lines = ask_region();
	if (lines < 0)
	  return FALSE;
 	
	if (g_region.r_up)
	{	curwp->w_line_no -= lines;
		curwp->w_flag |= WFMODE;
	}

/*if ((g_lastflag&CFKILL) == 0)		** This is a kill type	**
    kdelete() */;								/* command, so do magic */

	kinsert_n = chk_k_range(n);
	return ldelchrs(g_region.r_size, TRUE, TRUE);
}

/*	Narrow-to-region (^X-<) makes all but the current region in
	the current buffer invisable and unchangable
*/

int Pascal narrow(int f, int n)

{													/* find buffer and ensure it's not already narrow */
	BUFFER* bp = curwp->w_bufp;
	if (bp->b_flag & BFNAROW)
	{ mlwrite(TEXT71); /* "%%Buffer already narrowed" */
		return FALSE;
	}
									       /* find the boundries of the current region */
{ int	lines = reglines(FALSE);
	if (lines == 0)
	{ mlwrite(TEXT72); /* "%%Must narrow at least 1 full line" */
		return FALSE;
	}
	   								/* move forward to the end of this region		
		  					   		  	 (a long number of bytes perhaps) */
	forwbychar(g_region.r_size);
//curwp->w_doto = 0;										/* only by full lines please! */

	bp->b_flag |= BFNAROW;								/* remember we are narrowed */
	bp->b_baseline.l_fp = g_region.r_linep;
	bp->b_narlims[0] = lback(g_region.r_linep);
	lback(g_region.r_linep) = &bp->b_baseline;

	bp->b_baseline.l_bp = curwp->w_dotp;
	bp->b_narlims[1] = lforw(curwp->w_dotp);
	lforw(curwp->w_dotp) = &bp->b_baseline;
#if 1
	rpl_all(-1, 0, 0, (LINE*)bp, g_region.r_linep);
#else
{ WINDOW *wp;
																	   /* let all the proper windows be updated */
	for (wp = wheadp; wp; wp = wp->w_next) 
	  if (wp->w_bufp == bp)
	  { MARK * m;
	    wp->w_linep = g_region.r_linep;
	    wp->w_dotp = g_region.r_linep;
	    wp->w_doto = 0;
	    wp->w_flag |= (WFHARD|WFMODE);
	    for (m = &wp->mrks.c[NMARKS - 1]; m >= &wp->mrks.c[0]; --m)
	    { m->marko = 0;
	      m->markp = g_region.r_linep;
	    }
	  }
}
#endif
//setcline();		Dont use narrowed numbers

	mlwrite(TEXT73); /* "[Buffer is narrowed]" */
				 
	return TRUE;
}}

/*	widen-from-region (^X->) restores a narrowed region	*/

int Pascal widen(int notused, int n)

{		      								/* find the proper buffer and ensure we are narrow */
	BUFFER * bp = curbp;    // curwp->w_bufp;
	if ((bp->b_flag & BFNAROW) == 0)
	{ mlwrite(TEXT74);
					/* "%%This buffer is not narrowed" */
	  return FALSE;
	}

	bp->b_flag &= (~BFNAROW);
{	LINE * bl = &bp->b_baseline;
	int iter;
	for (iter = 2; --iter >= 0; )
	{	int niter = 1 - iter;
	  LINE * nl = bp->b_narlims[niter];
//  if (nl == NULL)
//  	continue;
	{	LINE *lp;
  	for (lp = nl;
	       lmove(lp,iter) != bl;  lp = lmove(lp,iter))
	     ;
#if 1
		lmove(nl,niter) = lmove(bl,niter);
		lmove(lmove(nl,niter),iter) = nl;
		lmove(bl,niter) = lp;
#else	
	   if (iter)			 						/* recover the top fragment */
	   { nl->l_fp = bl->l_fp;
	     lforw(nl)->l_bp = nl;
	     bl->l_fp = lp;
	   }
	   else			      		 			/* recover the bottom fragment */
	   { nl->l_bp = bl->l_bp;
	   	 lback(bl)->l_fp = nl;
	     bl->l_bp = lp;
	   }
#endif
//    *nlims = NULL;
	}}

	setcline();
	upwind(1);
//orwindmode(WFHARD|WFMODE);

	mlwrite(TEXT75);
				/* "[Buffer is widened]" */
	return TRUE;
}}

