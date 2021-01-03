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

#define is_space(ch) (ch == ' ' || ch == '\t')

#define A_LEND (-2)


int Pascal thischar()

{ return curwp->w_doto == llength(curwp->w_dotp)
				? A_LEND : lgetc(curwp->w_dotp, curwp->w_doto);
}


/*
 * Return TRUE if the character at dot is a character that is considered to be
 * part of a word. The word character list is hard coded. Should be setable.
 */
int Pascal inword()

{ return curwp->w_doto == llength(curwp->w_dotp)
		       ? FALSE : isword(lgetc(curwp->w_dotp, curwp->w_doto));
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

{
	register int cnt;	/* size of word wrapped to next line */
	register int c; 	/* charector temporary */
					    /* backup from the <NL> 1 char */
	if (!backchar(FALSE, 1))
	  return FALSE;
				   /* back up until we aren't in a word,
				      make sure there is a break in the line */
	cnt = 0;
	while (((c = lgetc(curwp->w_dotp, curwp->w_doto)) != ' ')
				&& (c != '\t'))
	{ cnt++;
	  if (!backchar(FALSE, 1))
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
	while (cnt-- > 0)
	  if (forwchar(FALSE, 1) == FALSE)
	    return FALSE;
			/* make sure the display is not horizontally scrolled */
	if (curwp->w_fcol != 0)
	{ curwp->w_fcol = 0;
	  curwp->w_flag |= WFHARD | WFMOVE | WFMODE;
	}

	return TRUE;
}


Int b_f_size;
char b_stop_eol;

/*
 * Move the cursor backward by "n" words. All of the details of motion are
 * performed by the "backchar" and "forwchar" routines. Error if you try to
 * move beyond the buffers.
 */
int Pascal backword(int f, int n)

{ register int ch;
  
  if (n < 0)
    return nextword(f, -n);

  b_f_size = -1;

  while (true) 
  { while (TRUE)
    { if (backchar(FALSE, 1) == FALSE)
        return FALSE;
      b_f_size += 1;
      ch = thischar();
      if (! is_space(ch) && (ch != A_LEND || b_stop_eol))
				break;
    }

    if (--n < 0)
      break;

  { int in_word = inword();

    if (n > 0 || ! b_stop_eol)      
      for (;!in_word ? !is_space(ch) && ch != A_LEND && 
      	               !isletter(ch) && ! in_range(ch,'0','9')
	                   : inword();
	          ch = thischar())
      { if (backchar(FALSE, 1) == FALSE)
				  return TRUE;
        b_f_size += 1;
      }
    if (n <= 0)
      break;
  }}
  return forwchar(FALSE, 1);
}


/*
 * Move the cursor forward by the specified number of words. All of the motion
 * is done by "forwchar". Error if you try and move beyond the buffer's end.
 */
int Pascal nextword(int f, int n)

{ register int ch;

	if (n < 0)
	  return backword(f, -n);

  b_f_size = 0;
	
	while (n--) 
	{ int in_word = inword();
	  
	  ch = thischar();
	  for (;in_word ? inword()
	                : !is_space(ch) && ch != A_LEND && 
	                  !isletter(ch) && ! in_range(ch,'0','9');
		ch = thischar())
	  { if (forwchar(FALSE, 1) == FALSE)
	      return FALSE;
			b_f_size += 1;
	  }

	  for (; is_space(ch) || ch == A_LEND; ch = thischar())
	  { if (forwchar(FALSE, 1) == FALSE)
	      return FALSE;
      b_f_size += 1;
	  }
	}
	return TRUE;
}


static int Pascal ccaseword(int n, int low)

{   if (low != 3 && (curbp->b_flag & MDVIEW)) /* disallow this command if */
      return rdonly();	                 /* we are in read only mode */
    if (n < 0)
      return FALSE;

    while (n--)
    { register int lo = low;
      while (!inword())
        if (forwchar(FALSE, 1) == FALSE)
          return FALSE;
      
      while (inword())
      
      { int c = lgetc(curwp->w_dotp, curwp->w_doto);
        if (lo == 0 && !islower(c) ||
            lo < 3  &&  islower(c))
        { lputc(curwp->w_dotp,curwp->w_doto, chcaseunsafe(c));
          lchange(WFEDIT);
        }
        if (lo == 2)
          lo = 3;  
        if (forwchar(FALSE, 1) == FALSE)
          return FALSE;
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
	if ((lastflag & CFKILL) == 0)
	  kdelete(0, 0);

	thisflag |= CFKILL;	/* this command is a kill */
        
	b_stop_eol = true;
  (void)nextword(f, n);
	b_stop_eol = false;
			/* restore the original position and delete the words */
	if (n > 0)
	  rest_l_offs(&save);

	return ldelchrs(b_f_size, TRUE);
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

void Pascal reform(char * para)	/* reformat a paragraph as stored in a string */
						/* string buffer containing paragraph */
{
	register int sp = -1;		  /* string scan pointer */
	         int lastbreak = -0x3fff; /* ix of last word break */
	register int col = 0;		  /* current column position */
	register int ch;

		/* scan string, replacing some whitespace with newlines */
	for (; (ch = para[++sp]) != 0; ++col)
	{ 
	  if (isspace(ch))			  /* white space.... */
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
}


Pascal fillpara(int f, int n)/* Fill the current paragraph according to the
				   current fill column */
{
 register char *pp;	/* ptr into paragraph being reformed */
 	  char *para;	/* malloced buffer for paragraph */
 register LINE *lp;	/* ptr to current line */
 register int lsize;	/* bytes in current line */
 register char *txtptr;	/* ptr into current line */
	LINE *ptline = curwp->w_dotp; /* line the point started on */
	int ptoff = curwp->w_doto;    /* offset of original point */
	int back;		/* # of characters from origin point to eop */
	int psize;		/* byte size of paragraph */
	LINE *bop;		/* ptr to beg of paragraph */
	LINE *eop;		/* pointer to line just past EOP */

	if (curbp->b_flag & MDVIEW)
	  return rdonly();

	if (fillcol == 0)	/* no fill column set */
	{ mlwrite(TEXT98);
/*			"No fill column set" */
	  return FALSE;
	}
			/* record the pointer to the line just past the EOP */
	gotoeop(FALSE, 1);
	eop = lforw(curwp->w_dotp);
		        
	gotobop(FALSE, 1);    /* and back top the beginning of the paragraph */
	bop = curwp->w_dotp;
					   /* ok, how big is this paragraph? */
	psize = 0;
	for (lp = bop; lp != eop; lp = lforw(lp))
	  psize += lp->l_used + 1;
					/* create a buffer to hold this stuff */
	para = (char*)malloc(psize + 100);	/***** THIS IS TEMP *****/
	if (para == NULL)
	  return FALSE;
				/* now, grab all the text into a string */
	back = 0;	/* counting the distance to backup when done */
	lp = bop;
	pp = para;
	while (lp != eop)
	{ lsize = lp->l_used + 1;
	  if	    (back != 0)
	    back += lsize;
	  else if (lp == ptline)
	    back = lsize - ptoff;
	  
	  txtptr = lp->l_text;
	  while (--lsize)			/* copy a line */
	    *pp++ = *txtptr++;
	  *pp++ = ' ';			/* turn the NL to a space */
	  lp = lfree(lp,0);			/* free the old line */
	}
	*(--pp) = 0;	/* truncate the last space */
				     /* reformat the paragraph in the buffer */
	reform(para);

	    /* insert the reformatted paragraph back into the current buffer */
{	int cc = linstr(para);
	lnewline();		/* add the last newline to our paragraph */
	if (cc == TRUE)	/* reposition us to the same place */
	  cc = backchar(FALSE, back);

			/* make sure the display is not horizontally scrolled */
	curwp->w_flag |= WFHARD | WFMOVE | WFMODE;

	free(para);
	return cc;
}}


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

{				/* make sure we have a region to count */
	Int nchars = getregion();
	Int size = nchars;
{	register LINE *lp = region.r_linep;
	register int offset = region.r_offset;
							/* count up things */
	int inword = FALSE;
	Int nwords = 0;
	Int nlines = 1;
  char ch;
	
	while (size--)
	{					/* get the current character */
		++offset;
		if (offset <= llength(lp))	/* end of line */
		 	ch = lgetc(lp, offset);
		else
		{	lp = lforw(lp);
			offset = 0;
			++nlines;
			ch = '\r';
		}
		
		if (in_range(ch & 0xdf, 'A', 'Z') ||
		    in_range(ch, '0', '9'))
		{ if (!inword)
		    ++nwords;
		  inword = TRUE;
		}
		else 
		  inword = FALSE;
	}
						/* and report on the info */
	mlwrite(TEXT100,
/*		"Words %D Chars %D Lines %d Avg chars/word %f" */
					nwords, nchars, nlines, 
					nwords <= 0 ? 0 : (int)((100L * nchars) / nwords));
	return TRUE;
}}

#endif

extern int g_txt_woffs;

char * Pascal getwtxt(int wh, char * buf, int lim)
	  
{ if (wh == 'B'-'@')
    return getkill();
  if (wh == 'N'-'@')
    return fixnull(curbp->b_fname);
  if (wh == 'S'-'@' || wh == 'S')
    return fixnull(patmatch);

{ int offs = wh == 'W'-'@' || wh == 'K'-'@' ? g_txt_woffs : 0;
	int lct = llength(curwp->w_dotp) - offs;
  if (lct > lim)
    lct = lim;

{ register char * t = buf;
  register int	c;

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
