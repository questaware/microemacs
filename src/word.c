/* The routines in this file implement commands that work word or a
 * paragraph at a time.  There are all sorts of word mode commands.  If I
 * do any sentence mode commands, they are likely to be put in this file. 
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"base.h"
																				// All other characters are positive
#define A_LEND       (-1)


int Pascal thischar()

{ return curwp->w_doto == llength(curwp->w_dotp)
				? A_LEND : lgetc(curwp->w_dotp, curwp->w_doto);
}


/*
 * Return TRUE if the character at dot is a character that is considered to be
 * part of a word. The word character list is hard coded. Should be setable.
 */
int Pascal inword()

{ return isword(thischar());
}

/* Word wrap on n-spaces. Back-over whatever precedes the point on the current
 * line and stop on the first word-break or the beginning of the line. If we
 * reach the beginning of the line, jump back to the end of the word and start
 * a new line.	Otherwise, break the line at the word-break, eat it, and jump
 * back to the end of the word. Make sure we force the display back to the
 * left edge of the current window
 * Returns TRUE on success, FALSE on errors.
 */
int Pascal wrapword(int f, int n)

{ 					             /* backup from the <NL> 1 char */
	if (!forwchar(FALSE, -1))
	  return FALSE;
              				   /* back up until we aren't in a word,
              				      make sure there is a break in the line */
{ int	cnt = 0;
  int c;
	while (((c = lgetc(curwp->w_dotp, curwp->w_doto)) != ' ')
				&& (c != '\t'))
	{ ++cnt;
	  if (!forwchar(FALSE, -1))
	    return FALSE;
                  		  /* if we make it to the beginning, start a new line */
	  if (curwp->w_doto == 0)
	  { gotoeol(FALSE, 0);
	    return lnewline();
	  }
	}
            					  /* delete the forward white space */
	if (!forwdel(0, 1) || !lnewline())
	  return FALSE;
          						  /* and past the first word */
	while (--cnt >= 0)
	  if (forwchar(FALSE, 1) == FALSE)
	    return FALSE;
                			  /* make sure the display is not horizontally scrolled */
	if (curwp->w_fcol != 0)
	{ curwp->w_fcol = 0;
	  curwp->w_flag |= WFHARD | WFMOVE | WFMODE;
	}

	return TRUE;
}}


static int  g_f_size;

/*
 * Move the cursor backward by "n" words. All of the details of motion are
 * performed by the "backchar" and "forwchar" routines. Error if you try to
 * move beyond the buffers.
 */
int Pascal backword(int f, int n)

{ // if (n < 0 || 1)
  return nextword(f, -n);
#if 0
{ int f_size = -1;
  
  while (--n >= 0)
  { int ch;
  	while (TRUE)													// Skip spaces and usually A_LEND
    { if (forwchar(FALSE, -1) == FALSE)
        return FALSE;
      f_size += 1;
      ch = thischar();
      if (!is_space(ch) && ch != A_LEND)
				break;
    }

//  if (n <= 0 && g_ignore >= 0)
//   	break;
	  																			// skip non spaces
    for ( ;!is_space(ch) && ch != A_LEND ; ch = thischar())
    { if (forwchar(FALSE, -1) == FALSE)
			  return TRUE;
      f_size += 1;
		}
  } 

  g_f_size = f_size;
  return forwchar(FALSE, 1);
}
#endif
}


/*
 * Move the cursor forward by the specified number of words. All of the motion
 * is done by "forwchar". Error if you try and move beyond the buffer's end.
 */
int Pascal nextword(int f, int n)

{
//if (n < 0)
//  return backword(f, -n);
	int mask = 0;
	int dir = 1;
	int ct = n;
	if (ct < 0)
	{ ct = -ct;
		dir = -1;
		mask = 1;
	}

{ int f_size = 0;

  while (--ct >= 0)
  { int ch;
  	int a = inword();
  	while (TRUE)													// Skip non spaces and usually A_LEND
    { if (forwchar(FALSE, dir) == FALSE)
        return FALSE;
      f_size += 1;
      ch = thischar();
      if ((is_space(ch) != 0 || ch == A_LEND) ^ mask)
				break;
    	if (!mask && a != isword(ch))
    		break;
    }

//  if (ct <= 0 && g_ignore >= 0)
//   	break;

		a = isword(ch);
	  																			// skip spaces resp. non spaces
    for ( ;((is_space(ch) != 0 || ch == A_LEND) ^ mask) &&
    			  a == isword(ch)
    			; ch = thischar())
    { if (forwchar(FALSE, dir) == FALSE)
			  return TRUE;
      f_size += 1;
    }
  }

	g_f_size = f_size - mask;
	return mask == 0 ? TRUE : forwchar(FALSE, 1);
}}


static int Pascal ccaseword(int n, int low)

{   if (low != 3 && (curbp->b_flag & MDVIEW)) /* disallow this command if */
      return rdonly();	                 /* we are in read only mode */
    if (n < 0)
      return FALSE;

    while (--n >= 0)
    { int lo = low;
      while (!inword())
        if (forwchar(FALSE, 1) == FALSE)
          return FALSE;

      while (TRUE)
      { int ch = thischar();
        if (lo == 0 && !islower(ch) ||
            lo < 3  &&  islower(ch))
        { lputc(curwp->w_dotp,curwp->w_doto, chcaseunsafe(ch));
          lchange(WFEDIT);
        }
        if (lo == 2)
          lo = 3;  
        if (forwchar(FALSE, 1) == FALSE)
          return FALSE;

				if (isword(ch))
					break;
      }
    }
    return TRUE;
}

/* Move the cursor forward by the specified number of words. As you move
 * convert characters to lower case. Error if you try and move over the end of
 * the buffer. Bound to "M-L".
 */
int Pascal lowerword(int f, int n)

{ return ccaseword(n, 0);
}

/*
 * Move the cursor forward by the specified number of words. As you move,
 * Convert any characters to upper case. Error if you try and move beyond the
 * end of the buffer. Bound to "M-U".
 */
int Pascal upperword(int f, int n)

{ return ccaseword(n, 1);
}

/*
 * Move the cursor forward by the specified number of words. As you move
 * Convert the first character of the word to upper case, and subsequent
 * characters to lower case. Error if you try and move past the end of the
 * buffer. Bound to "M-C".
 */
int Pascal capword(int f, int n)

{ return ccaseword(n, 2);
}

/*
 * Move forward to the end of the nth next word. Error if you move past
 * the end of the buffer.
 */
int Pascal endword(int f, int n)

{ return n < 0 ? nextword(f, n)
	  			     : ccaseword(n, 3);
}

/*
 * Kill forward by "n" words. Remember the location of dot. Move forward by
 * the right number of words. Put dot back where it was and issue the kill
 * command for the right number of characters. With a zero argument do nothing.
 * Bound to "M-D".
 */
Pascal delfword(int f, int n)

{	Lpos_t save = *(Lpos_t*)&curwp->w_dotp;

	if (curbp->b_flag & MDVIEW)
	  return rdonly();

	kinsert_n = 0;

	if (n == 0)
	  return TRUE;
								      /* Clear the kill buffer if last command wasn't a kill */
	if ((g_lastflag & CFKILL) == 0)
	  kdelete(0, 0);

	g_thisflag |= CFKILL;	/* this command is a kill */
        
//g_ignore = 256;
  (void)nextword(f, n);
//g_ignore = A_LEND;
											/* restore the original position and delete the words */
	if (n > 0)
	  rest_l_offs(&save);

	return ldelchrs(g_f_size, TRUE);
}

/*
 * Kill backwards by "n" words. Move backwards by the desired number of words,
 * counting the characters. When dot is finally moved to its resting place,
 * fire off the kill command. Bound to "M-Rubout" and to "M-Backspace".
 */

int Pascal delbword(int f, int n)

{ return delfword(f, -n);                  
}

#if	WORDPRO

static 
char * Pascal reform(char * para)	/* reformat a paragraph */
						/* string buffer containing paragraph */
{
	int sp = -1;		  				/* string scan pointer */
	int lastbreak = -0x3fff;  /* ix of last word break */
	int col = 0;		  /* current column position */
	int ch;
	int fillcol = pd_fillcol != 0 ? pd_fillcol : term.t_ncol;

											/* scan string, replacing some whitespace with newlines */
	for (; (ch = para[++sp]) != 0; ++col)
	{ 
	  if (is_space(ch))			  /* white space.... */
	  { if (ch == '\t')
	      col = (col + 7) & ~7;
						  /* break on whitespace? */
	    lastbreak = sp;
	  }

	  if (col >= fillcol)
	  {				 /* line break here! */
	    if (sp - lastbreak < fillcol) 
	    { para[lastbreak] = '\n';
	      sp = lastbreak;
	      col = -1;
	    }
	  }
	}
	return para;
}


Pascal fillpara(int f, int n)/* Fill the current paragraph according to the
															  current fill column */
{
	LINE *ptline = curwp->w_dotp; /* line the point started on */
	int  ptoff = curwp->w_doto;    /* offset of original point */

	if (curbp->b_flag & MDVIEW)
	  return rdonly();
													/* record the pointer to the line just past the EOP */
	gotoeop(FALSE, 1);
	
			/* make sure the display is not horizontally scrolled */
	curwp->w_flag |= WFHARD | WFMOVE | WFMODE;

{	LINE * eop = lforw(curwp->w_dotp);
		        
	gotobop(FALSE, 1);    	/* and back top the beginning of the paragraph */

{	LINE * bop = curwp->w_dotp;
											  	/* ok, how big is this paragraph? */
	int	psize = 0;
	LINE * lp;
	for (lp = bop; lp != eop; lp = lforw(lp))
	  psize += lp->l_used + 1;
																/* now, grab all the text into a string */
{	int	back = 0;									/* counting the distance to backup when done */
	char * para = (char*)malloc(psize + 100);	/***** THIS IS TEMP *****/
	char * pp = para;
	if (pp == NULL)
	  return FALSE;
	
	while (bop != eop)
	{ int lsize = bop->l_used + 1;
	  if	    (back != 0)
	    back -= lsize;
	  else if (bop == ptline)
	    back = ptoff - lsize;
	  
	{	char * txtptr = bop->l_text-1;
	  while (--lsize > 0)					/* copy a line */
	    *++pp = *++txtptr;
	  *++pp = ' ';						/* turn the NL to a space */
	  bop = lfree(bop,0);				/* free the old line */
	}}
	*pp = 0;									/* truncate the last space */

					    /* insert the reformatted paragraph back into the current buffer */
{	int cc = linstr(reform(para+1));
	lnewline();		/* add the last newline to our paragraph */
	if (cc == TRUE)	/* reposition us to the same place */
	  cc = forwchar(FALSE, back);

	free(para);
	return cc;
}}}}}


#if 0

Pascal killpara(int f, int n)/* delete n paragraphs starting with the current one */

{
	register int cc;

	while (n--)		/* for each paragraph to delete */
	{	           /* mark out the end and beginning of the para to delete */
		gotoeop(FALSE, 1);
							 /* set the mark here */
		curwp->mrks.c[0].markp = curwp->w_dotp;
		curwp->mrks.c[0].marko = curwp->w_doto;
				     /* go to the beginning of the paragraph */
		gotobop(FALSE, 1);
		curwp->w_doto = 0;	/* force us to the beginning of line */
							    /* and delete it */
		cc = killregion(FALSE, 1);
		if (cc != TRUE)
		  return cc;
					   /* and clean up the 2 extra lines */
		ldelchrs(2L, TRUE);
	}
	return TRUE;
}

#endif


/*	wordcount:	count the # of words in the marked region,
								along with average word sizes, # of chars, etc,
								and report on them.			*/

int Pascal wordcount(int f, int n)

{	Int nchars = 0;
	Int nwords = 0;
	Int nlines = 1;

	REGION * r = getregion();
	if (r != NULL)
	{	Int size = r->r_size;
		LINE *lp = 	 r->r_linep;
		int offset = r->r_offset;
								/* count up things */
		int in_wd = FALSE;
	  
	  nchars = size;
		
		while (--size >= 0)
		{	char ch;										/* get the current character */
			++offset;
			if (offset <= llength(lp))	/* end of line */
			 	ch = lgetc(lp, offset);
			else
			{	lp = lforw(lp);
				offset = 0;
				++nlines;
				ch = '\r';
			}
			
			if (isword(ch))
			{ if (!in_wd)
			    ++nwords;
			  in_wd = TRUE;
			}
			else 
			  in_wd = FALSE;
		}
	}
							/* and report on the info */
	mlwrite(TEXT100,
					/* "Words %D Chars %D Lines %d Avg chars/word %f" */
					nwords, nchars, nlines, 
					nwords <= 0 ? 0 : (int)((100L * nchars) / nwords));
	return TRUE;
}

#endif

extern int g_txt_woffs;

char * Pascal getwtxt(int wh, char * buf, int lim)
	  
{ if (wh == 'B'-'@')
    return getkill();
  if (wh == 'N'-'@')
    return fixnull(curbp->b_fname);
  if (wh == 'S'-'@' || wh == (ALTD | 'S'))
    return fixnull(patmatch);

{ int offs = wh == 'W'-'@' || wh == 'K'-'@' ? g_txt_woffs : 0;
	int lct = llength(curwp->w_dotp) - offs;
  if (lct > lim)
    lct = lim;

{ char * t = buf;
  int	c;
  while (--lct >= 0)
  { c = lgetc(curwp->w_dotp, offs);
    if (! isword(c) && t != buf && wh == (int)'W'-'@')
      break;
    *t++ = c;
    offs += 1;
  }

  if (wh == 'W'-'@')
    g_txt_woffs = offs;

  *t = 0;
  return buf;
}}}
