/* This file contains the command processing functions for a number of random
 * commands. There is no functional grouping here, for sure.
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"logmsg.h"

int g_c_cmt, g_s_cmt, g_f_cmt, g_p_cmt;

/*
 * Set fill column to n.
 */
Pascal setfillcol(int f, int n)

{ fillcol = n;
	mlwrite(TEXT59,n);
/*							"[Fill column is %d]" */
	return TRUE;
}

/* Display the current position of the cursor, in origin 1 X-Y coordinates,
 * the character that is under the cursor (in hex), and the fraction of the
 * text that is before the cursor. The displayed column is not the current
 * column, but the column that would be used on an infinite width display.
 * Normally this is bound to "C-X =".
 */
Pascal showcpos(int f, int n)

{	register LINE 		*lp;						/* current line */
	register Int		numchars = 0; 	/* # of chars in file */
	register int		numlines = 0; 	/* # of lines in file */
					 Int		predchars;			/* # chars preceding point */
					 int		predlines = 0;			/* # lines preceding point */
															/* starting at the beginning of the buffer */
																			/* start counting chars and lines */
	for (lp = curbp->b_baseline; ((lp = lforw(lp))->l_props & L_IS_HD) == 0; )
	{ ++numlines;
		if (lp == curwp->w_dotp)		 /* record we are on the current line */
		{ predlines = numlines;
			predchars = numchars + curwp->w_doto;
		}
																							/* on to the next line */
		numchars += llength(lp) + 1;
	}
																					/* if at end of file, record it */
	if (predlines <= 0)
	{ predlines = numlines;
		predchars = numchars;
	}
															/* Get real column and end-of-line column. */
{	int savepos = curwp->w_doto;
	int cch = curwp->w_dotp->l_used == savepos ? '\r' 
																						 : lgetc(curwp->w_dotp, savepos);
	curwp->w_doto = llength(curwp->w_dotp);
	curwp->w_line_no = predlines;
{	int ecol = getccol();
	curwp->w_doto = savepos;
	if (numchars != 0)
	{ int ratio = (100L*predchars) / numchars;

		mlwrite(TEXT60,
/*							"Line %d/%d Col %d/%d Char %D/%D (%d%%) char = 0x%x" */
						predlines, numlines, getccol(), ecol,
						predchars, numchars, ratio, cch);
	}
	return (int)numchars;
}}}

Pascal setcline() 			/* get the current line number */

{																 /* starting at the beginning of the buffer */
					 LINE *tgt = curwp->w_dotp;
	register LINE *lp;
	register int numlines = 1;
				
	for ( lp = curbp->b_baseline;
			 ((lp = lforw(lp))->l_props & L_IS_HD) == 0 && lp != tgt; )
		++numlines;

	curwp->w_line_no = numlines;
	return numlines;
}

/* This routine, given a pointer to a LINE, and the current cursor goal
 * column, return the best choice for the offset. The offset is returned.
 * Used by "C-N" and "C-P".
 */
Pascal getgoal(LINE * dlp, int offs)
				
{ register int	col = 0;
					 int	dbo;

	for (dbo = -1; ++dbo < llength(dlp); )
	{ register short c = lgetc(dlp, dbo);
		if (c < 0x20 || c == 0x7f)
		{ if (c == '\t')
				col += curbp->b_tabsize  - 1 - (col % curbp->b_tabsize);
			else if (c >= col1ch && c <= col2ch)
			{ col -= 1;
				if (c == 8)
					col -= 1;
			}
			else 
				col += 1;
		}
		col += 1;
		if (col > offs)
			break;
	}
	return dbo;
}


/*
 * Return current column.  Stop at first non-blank given TRUE argument.
 */
int Pascal getccol()

{ register int col = 0;
					 int dbo;

	for (dbo = -1; ++dbo < curwp->w_doto;)
	{ short c = lgetc(curwp->w_dotp, dbo);
		if (c < 0x20 || c == 0x7f)
		{ if (c == '\t')
				col += curbp->b_tabsize  - 1 - (col % curbp->b_tabsize);
			else if (c >= col1ch && c <= col2ch)
			{ col -= 1;
				if (c == 8)
					col -= 1;
			}
			else 
				col += 1;
		}
		col += 1;
	}
	return col;
}

#if FLUFF
/* Twiddle the two characters on either side of dot. If dot is at the end of
 * the line twiddle the two characters before it. Return with an error if dot
 * is at the beginning of line; it seems to be a bit pointless to make this
 * work. This fixes up a very common typo with a single stroke. Normally bound
 * to "C-T". This always works within a line, so "WFEDIT" is good enough.
 */
Pascal twiddle(int f, int n)

{	register LINE 	*dotp = curwp->w_dotp;
	register int		doto = curwp->w_doto;

	if (curbp->b_flag & MDVIEW) 		/* don't allow this command if	*/
		return rdonly();							/* we are in read only mode 		*/
					
	if (doto == llength(dotp))
		--doto;

	if (doto <= 0)
		return FALSE;
{	int cr = lgetc(dotp, doto);
	lputc(dotp, doto, lgetc(dotp, doto-1));
	lputc(dotp, doto-1, cr);
	lchange(WFEDIT);
	return TRUE;
}}

#endif

/* Quote the next character, and insert it into the buffer. All the characters
 * are taken literally, including the newline, which does not then have
 * its line splitting meaning. The character is always read, even if it is
 * inserted 0 times, for regularity. Bound to "C-Q"
 */
int Pascal quote(int f, int n)

{ if (curbp->b_flag & MDVIEW) 		/* don't allow this command if	*/
		return rdonly();							/* we are in read only mode 		*/
{ int c = tgetc();
	if (n < 0)
		return FALSE;
	if (n == 0)
		return TRUE;
	return linsert(n, (char)c);
}}

/* Set tab size if given non-default argument (n <> 1).  Otherwise, insert a
 * tab into file.  If given argument, n, of zero, change to hard tabs.
 * If n > 1, simulate tab stop every n-characters using spaces. This has to be
 * done in this slightly funny way because the tab (in ASCII) has been turned
 * into "C-I" (in 10 bit code) already. Bound to "C-I".
 */
int Pascal settabsize(int f, int n)

{ int tabsz = curbp->b_tabsize;
  int ct = tabsz - (getccol() % tabsz);

	return ct <= 0 ? OK : linsert(ct,	' ');
}

#if 		AEDIT

int Pascal detabline()

{ curwp->w_doto = 0;		/* start at the beginning */

	while (true)
	{ register int offs = curwp->w_doto;
		if (offs >= llength(curwp->w_dotp))
			break;
			
		if (lgetc(curwp->w_dotp, offs) == '\t')
		{ lputc(curwp->w_dotp, offs, ' ');
			insspace(TRUE, curbp->b_tabsize - (offs % curbp->b_tabsize) - 1);
		}
		forwchar(FALSE, 1);
	}
	return OK;
}

int do_entab = 0;

Pascal detab(int f, int n) /* change tabs to spaces */

{ if (curbp->b_flag & MDVIEW) 		/* don't allow this command if	*/
		return rdonly();							/* we are in read only mode 		*/

	if (f == FALSE)
		n = reglines();

{ register int fspace;								/* pointer to first space of a run */
	register int ccol;									/* current cursor column */
					 int inc = n > 0 ? 1 : -1;	/* increment to next line [sgn(n)] */
					 int tbsz = curbp->b_tabsize;

	for (; n; n -= inc)
	{ detabline();									/* advance/or back to the next line */
		if (do_entab)
		{ 												/* now, entab the resulting spaced line */
			curwp->w_doto = 0;						/* start at the beginning */
			fspace = -1;									/* entab the entire current line */
			ccol = 0;
			while (true)
			{ 														/* see if it is time to compress */
				if (fspace >= 0 && ccol >= nextabp(fspace,tbsz))
				{ if (ccol - fspace >= 2)
					{ backchar(TRUE, ccol - fspace);
						ldelchrs((Int)(ccol - fspace), FALSE);
						linsert(1, '\t'); 			
					}
					fspace = -1;
				}
				if (curwp->w_doto >= llength(curwp->w_dotp))
					break;
																						/* get the current character */
				if (lgetc(curwp->w_dotp, curwp->w_doto) != ' ')
					fspace = -1;
				else
					if (fspace < 0)
						fspace = ccol;
				ccol++;
				forwchar(FALSE, 1);
			}
		}
														/* advance/or back to the next line */
		forwline(TRUE, inc);
	}
	curwp->w_doto = 0;			/* to the begining of the line */
	thisflag &= ~CFCPCN;		/* flag that this resets the goal column */
	lchange(WFEDIT);				/* yes, we have made at least an edit */
	return TRUE;
}}


Pascal entab(int f, int n) /* change spaces to tabs where posible */

{
	do_entab = 1;
{	int res = detab(f, n);
	do_entab = 0;
	return res;
}}			

/* trim:				trim trailing whitespace from the point to eol
								with no arguments, it trims the current region
*/
Pascal trim_white(int f, int n)

{
	if (curbp->b_flag & MDVIEW) 		/* don't allow this command if	*/
		return rdonly();							/* we are in read only mode 		*/

	if (f == FALSE)
		n = reglines();

{ register LINE *lp;									/* current line pointer */
/*register int length;								** current length */
	register int inc = n > 0 ? 1 : -1; /* increment to next line [sgn(n)] */

	for (; n; n -= inc)
	{ 			
		lp = curwp->w_dotp; 						/* find current line text */
																		/* trim the current line */
#if 0
		int offset = curwp->w_doto;

		for (length = lp->l_used; length > offset; --length)
			if (lgetc(lp, length-1) != ' ' && 
					lgetc(lp, length-1) != '\t')
				break;

		lp->l_used = length;
#else
	{ int offset = lp->l_used;
					
			(void)trimstr(lp->l_text,&offset);
			lp->l_used = offset;
	}
#endif
																				/* advance/or back to the next line */
			forwline(TRUE, inc);
	}
	lchange(WFEDIT);
	thisflag &= ~CFCPCN;		/* flag that this resets the goal column */
	return TRUE;
}}
#endif


char * Pascal skipspaces(char * s, char * limstr)
				
				/* char * 		limstr; 									* the last char */
{ 				 int lim = limstr - s;
	register int ix;
	register char ch;
	
	for (ix = -1; ++ix < lim && ((ch=s[ix]) == 'L'-'@' ||
																ch == ' ' || ch == '\t');)
		;
		
	return &s[ix];
}


/* Insert a newline, then enough tabs and spaces to duplicate the indentation
 * of the previous line. Tabs are every tabsize characters. Quite simple.
 * Figure out the indentation of the current line. Insert a newline by calling
 * the standard routine. Insert the indentation by inserting the right number
 * of tabs and spaces. Return TRUE if all ok. Return FALSE if one of the
 * subcomands failed. Normally bound to "C-J".
 * doesnt work so I replaced it with cinsert semantics -- PJS
 */
Pascal indent(int f, int n)

{	if (curbp->b_flag & MDVIEW) 		/* don't allow this command if	*/
		return rdonly();							/* we are in read only mode 		*/
	if (n < 0)
		return FALSE;
{ 				 char *src = &curwp->w_dotp->l_text[0];
	register char *eptr = skipspaces(&src[0],
																	 &src[curwp->w_doto]);
	if (lnewline() == FALSE)				/* put in the newline */
		return FALSE;

{	char schar = eptr[0];
	eptr[0] = 0;

	linstr(src);									 /* and the saved indentation */
	eptr[0] = schar;

	return TRUE;
}}}

#if FLUFF

Pascal cinsert()				/* insert a newline and indentation for C */

{ register int bracef;				 /* was there a brace at the end of line? */
	register LINE *lp = curwp->w_dotp;
																		 /* trim the whitespace before the point */
	register int offset = curwp->w_doto;
	while (--offset >= 0 &&
					lgetc(lp, offset) == ' ' ||
					lgetc(lp, offset) == '\t')
	{ backdel(FALSE, 1);
	}
																												/* check for a brace */
	if (offset >= 0 && lgetc(lp, offset) == '{')
		offset = -1;
																											 /* put in the newline */
	if (lnewline() == FALSE)
		return FALSE;
												 /* if the new line is not blank... don't indent it! */
	lp = curwp->w_dotp;
	if (lp->l_used)
		return TRUE;

								 /* hunt for the last non-blank line to get indentation from */
	while (lp->l_used == 0 && (lp->l_props & L_IS_HD) == 0)
		lp = lback(lp);
													/* grab a pointer to text to copy indentation from */
{	char *cptr = skipspaces(&lp->l_text[0],
													&lp->l_text[lp->l_used]);
		
	char schar = *cptr;
	*cptr = 0;				
	linstr(lp->l_text); 							 /* insert this saved indentation */
	*cptr = schar;
																			
	if (offset >= 0)												/* one more tab for a brace */
		settabsize(FALSE, 1);

	return TRUE;
}}


/* Open up some blank space. The basic plan is to insert a bunch of newlines,
 * and then back up over them. Everything is done by the subcommand
 * processors. They even handle the looping. Normally this is bound to "C-O".
 */
Pascal openline(int f, int n)

{	register int		i;
	register int		s;

	if (curbp->b_flag & MDVIEW) 		/* don't allow this command if	*/
		return rdonly();							/* we are in read only mode 		*/
	if (n <= 0)
		return n == 0;

	i = n;																	/* Insert newlines. 		*/
	while ((s = lnewline()) == TRUE && --i > 0)
		;

	if (i > 0)															/* Then back up overtop */
		return s; 														/* of them all. 				*/
	return backchar(f, n);
}

#endif /* FLUFF */

/* Insert a newline. Bound to "C-M". If we are in CMODE, do automatic
 * indentation as specified.
 */
Pascal ins_newline(int f, int n)
{
	register int s;

	if (curbp->b_flag & MDVIEW) 		/* don't allow this command if	*/
		return rdonly();							/* we are in read only mode 		*/
	if (n < 0)
		return FALSE;
#if FLUFF
													/* if we are in C mode and this is a default <NL> */
	if (n == 1 && (curbp->b_flag & MDCMOD) &&
			(curwp->w_dotp->l_props & L_IS_HD) == 0)
		return cinsert();
#endif
				/* If a newline was typed, fill column is defined, the argument is non-
				 * negative, wrap mode is enabled, and we are now past fill column,
				 * and we are not read-only, perform word wrap.
				 */
	if ((curbp->b_flag & MDWRAP) && 
			fillcol > 0 && getccol() > fillcol)
		execkey(&wraphook, FALSE, 1);
																						/* insert some lines */
	while (n--)
		if ((s=lnewline()) != TRUE)
			return s;

	return TRUE;
}

/* Delete forward. This is real easy, because the basic delete routine does
 * all of the work. Watches for negative arguments, and does the right thing.
 * If any argument is present, it kills rather than deletes, to prevent loss
 * of text if typed with a big argument. Normally bound to "C-D".
 */
Pascal forwdel(int f, int n)
{
	if (curbp->b_flag & MDVIEW) 		/* don't allow this command if	*/
		return rdonly();							/* we are in read only mode 		*/
		
	if (f != FALSE) 											/* Really a kill. 			*/
	{ if ((lastflag&CFKILL) == 0)
			kdelete(0,0);
		thisflag |= CFKILL;
	}
	if (n < 0)
	{ n = -n;
	{ int s = backchar(f, n);
		if (s != TRUE)
			return s;
	}}
	return ldelchrs((Int)n, f);
}

/* Delete backwards. This is quite easy too, because it's all done with other
 * functions. Just move the cursor back, and delete forwards. Like delete
 * forward, this actually does a kill if presented with an argument. Bound to
 * both "RUBOUT" and "C-H".
 */
Pascal backdel(int f, int n)
{
				return forwdel(f, -n);
}

/* Kill text. If called without an argument, it kills from dot to the end of
 * the line, unless it is at the end of the line, when it kills the newline.
 * If called with an argument of 0, it kills from the start of the line to dot.
 * If called with a positive argument, it kills from dot forward over that
 * number of newlines. If called with a negative argument it kills to the 
 * n.th kill buffer. Normally bound to "C-K".
 */
Pascal killtext(int f, int n)
{
	extern char last_was_yank;

	if (curbp->b_flag & MDVIEW) 		/* don't allow this command if	*/
		return rdonly();							/* we are in read only mode 		*/

	kinsert_n = 0;
	if (n < 0)
	{ kinsert_n = -n;
		n = 1;
	}
#if 0
	if ((lastflag & CFKILL) == 0) 					/* Clear kill buffer if */
		kdelete(0,0); 												/* last wasn't a kill.	*/

	if (last_was_yank)
	{ last_was_yank = false;
		mbwrite("Discard KB");
		kdelete(0, kinsert_n);
	}
#endif
	thisflag |= CFKILL;
{ register Int chunk = -curwp->w_doto;

	if			(f == FALSE)
	{ chunk += llength(curwp->w_dotp);
		if (chunk == 0)
			chunk = 1;
	} 
	else if (n == 0)
	{ chunk = -chunk;
		curwp->w_doto = 0;
	}
	else if (n > 0)
	{ register LINE *nextp = curwp->w_dotp;

		while (--n >= 0)
		{ if (nextp->l_props & L_IS_HD)
				return FALSE;
			chunk += llength(nextp)+1;
			nextp = lforw(nextp);
		}
	} 
	return ldelchrs(chunk, TRUE);
}}

Pascal adjustmode(int kind, int global) /* change the editor mode status */
				/* int kind;		** 0 = delete, 1 = set, 2 = toggle */
				/* int global;	** true = global flag,	false = current buffer flag */
{
	register int status;						/* error return on input */
	char cbuf[NPAT];								/* buffer to recieve mode name into */

	concat(&cbuf[0], global ? TEXT62 : TEXT63,
/*														"Global mode to " */
/*														"Mode to " */
				
				TEXT64a[kind], null);
/*														 "add: " */
/*														 "delete: " */
/*														 "toggle: " */

	status = mlreply(cbuf, cbuf, NPAT - 1);
	if (status != TRUE)
		return status;

	mlerase();

	if (!global)
	{ if (*strmatch("CHGD", cbuf) == 0)
		{ curbp->b_flag &= ~BFCHG;
			upmode();
			return TRUE;
		}
		if (*strmatch("INVS", cbuf) == 0)
		{ if			(kind == 0)
				curbp->b_flag &= ~BFINVS;
			else if (kind == 1)
				curbp->b_flag |= BFINVS;
			else
				curbp->b_flag ^= BFINVS;
			return TRUE;
		}
	}

{	register int index;
	int bestmatch = 0;
#define iter status
	int bestiter;
	int ix = NUMMODES;							/* loop index */
	
	for (iter = 2; --iter >= 0; )
	{ 														/* test it first against the modes we know */
																/* then test it against the colours we know */
		for (; --ix >= 0;)
		{ int best = 10;
			const char * goal = iter > 0 ? mdname[ix] : cname[ix];
			const char * sm= strmatch(goal, cbuf);
			if (*sm > ' ')
			{ best = sm - goal;
				if (best <= bestmatch)
					continue;
			}
			bestmatch = best;
			bestiter = iter;
			index = ix;
		}
		ix = NCOLORS;
	}
		
	if (bestmatch > 0)
	{ if (bestiter == 0)
		{
#if COLOR
			if (in_range(cbuf[0], 'A', 'Z'))
				if (global)
					gfcolor = index;
				else
				{ curwp->w_color &= 0xff00;
					curwp->w_color |= index;
				}
			else
				if (global)
					gbcolor = index;
				else
				{ curwp->w_color &= 0xff;
					curwp->w_color |= index << 8;
				}
			curwp->w_flag |= WFCOLR;
#endif
		}
		else
		{ int x = (MDSTT << index); 	/* finding a match, we process it */
			int md = global ? g_gmode : curbp->b_flag;
			if			(kind == 0)
					md &= ~x;
			else if (kind == 1)
					md |= x;
			else
					md ^= x;
			if (global)
				g_gmode = md;
			else
				curbp->b_flag = md;
#if CRYPT
			if (x == MDCRYPT)
			{ char ** k = g_gmode & MDCRYPT ? &ekey : &curbp->b_key;
				if ((md & MDCRYPT) && *k == null)
				{ setekey(k);
					curbp->b_flag |= BFCHG;
				}
			}
#endif																	
			if (global == 0)						/* display new mode line */
				upmode();
		}
		/*mlerase();*/
		return TRUE;
	}

	mlwrite(TEXT66);
/*							"No such mode!" */
	return FALSE;
}}


Pascal delmode(int f, int n)		/* prompt and delete an editor mode */

{
	return adjustmode(0, FALSE);
}

Pascal setmod(int f, int n) 		/* prompt and set an editor mode */

{
	return adjustmode(1, FALSE);
}


Pascal togmod(int f, int n) 		/* prompt and set an editor mode */

{
	return adjustmode(2, FALSE);
}

Pascal delgmode(int f, int n) 	/* prompt and delete a global editor mode */

{
	return adjustmode(0, TRUE);
}

Pascal setgmode(int f, int n) 	/* prompt and set a global editor mode */

{
	return adjustmode(1, TRUE);
}

Pascal toggmode(int f, int n) 	/* prompt and set a global editor mode */

{
	return adjustmode(2, TRUE);
}



/*			This function simply clears the message line,
								mainly for macro usage									*/

Pascal clrmes(int f, int n)

{
	mlforce("");
	return TRUE;
}

/*			This function writes a string on the message line
								mainly for macro usage									*/

Pascal writemsg(int f, int n)

{
	char buf[NSTRING];			/* buffer to recieve message into */

	register int status = mlreply(TEXT67, buf, NSTRING - 1);
	if (status != TRUE)
/*														"Message to write: " */
		return status;

	ostring(buf);
	return TRUE;
}

Paren_t paren;

int Pascal init_paren(const char * str,
										  int len
										 )
{ int c_cmt = (curbp->b_langprops & BCCOMT);
	int f_cmt = (curbp->b_langprops & (BCFOR+BCSQL+BCPAS));
	int s_cmt = (f_cmt							& BCSQL);
	int p_cmt = (curbp->b_langprops & BCPAS);
		
	paren.nest = 1;
/*paren.nestclamped = 1;*/
	paren.sdir = 1;
	paren.in_mode = 0;
	paren.prev = 0;
	paren.complex = false;
	paren.olcmt = c_cmt ? '/' :
								s_cmt ? '-' : 
								p_cmt ? ')' : (char)-1;
	paren.ch = *str;
	if (len > 0 && f_cmt != 0)
		paren.ch = toupper(paren.ch);
	
{ register char ch = paren.ch;
																						 /* setup proper matching fence */
	switch (ch)
	{ case '(': ch = ')';
		when ')': ch = '(';
#if CHARCODE == ASCII
		when '[':
		case '{': ch += 2;
		when ']': 
		case '}': ch -= 2;
#else
		when '[': ch = ']';
		when '{': ch = '}';
		when ']': ch = '[';
		when '}': ch = '{';
#endif
		when '#': paren.complex = true;
							if			(len > 2 && str[1] == 'i' &&  str[2] == 'f')
								;
							else if (len > 2 && str[1] == 'e' && (str[2] =='n'|| str[2] =='l'))
								paren.sdir = -1;
							else
							{ paren.nest = 0;
								ch = 0;
							}
		when 'T':
		case 'S':
		case 'D':
		case 'L': 
							if (s_cmt)
								return 0;
		case 'B':
							if (len < 2)
								return 0;
							if (ch == 'T' ? toupper(str[1]) != 'H' : 
									ch == 'S' ? toupper(str[1]) != 'E' :
									ch == 'B' ? toupper(str[1]) != 'E' :
															toupper(str[1]) != 'O' ? true :
																len > 2 && ch == 'L' && toupper(str[2]) != 'O')
								return 0;
							paren.complex = true;
							paren.fence = 'E';
							return paren.sdir;

		when 'C':	if (len < 2)
								return 0;

							if (s_cmt + p_cmt)
							{ if (toupper(str[1]) != 'A')
									return 0;
								paren.complex = true;
								paren.ch = 'B';
								paren.fence = 'E';
								return paren.sdir;					
							}

		when 'E': paren.complex = true;
//						if (toupper(str[1]) != 'L')
							{ paren.fence = 'T';
								paren.sdir = -1;
								return paren.sdir;
							}
//						else
//						{ paren.ch = 'T';
//							paren.fence = 'E';
//							paren.sdir = 1;
//							return 1;
//						}
							
		otherwise paren.nest = 0;
							paren.sdir = 0;
	}
	paren.fence = ch;
	if (ch < paren.ch)			// not complex
	{	paren.sdir = -1;
		if (p_cmt)
			paren.olcmt = '(';
			
	}
	return paren.sdir;
}}

#if 0
	 CMT0 				after the first /
	 CMT
	 CMT_ 				On the final / of /* xxx */
	 EOL					Comment to end of line
#endif

int Pascal scan_paren(char ch)
				
{ register int mode = paren.in_mode;
					 int beg_cmt = g_clring & BCPAS ? '(' : '/';
					 int end_cmt = g_clring & BCPAS ? ')' : '/';

	do
	{ if (mode & (Q_IN_CMT + Q_IN_CMT0))
		{ 
			if (mode & Q_IN_CMT)
			{ if (mode & Q_IN_EOL)
				{ if (ch == '\n')
						mode = 0;
					break;
				}
				else if (ch == end_cmt && paren.prev == '*')
				{ mode = Q_IN_CMT_;
					break;
				}
			}
			else // if (mode & Q_IN_CMT0)
			{ mode = ch == '*'													  ? Q_IN_CMT 					 :
							 ch == paren.olcmt &&
							 ch == paren.prev  && paren.sdir >= 0 ? Q_IN_CMT + Q_IN_EOL : 0;
			} 			
		} 
		else if ((mode & (Q_IN_STR + Q_IN_CHAR)) != 0)
		{ if			(ch == '\n' && paren.olcmt != '-')
				mode = 0;
			else if ((mode & Q_IN_ESC) && paren.sdir >= 0)
				mode &= ~Q_IN_ESC;
			else if (ch == '\\' && paren.sdir >= 0)
				mode |= Q_IN_ESC;
			else if ((mode & Q_IN_STR) && ch == '"' ||
							 (mode & Q_IN_CHAR) && ch == '\'') 
				mode = 0;
			break;
		}
		else // if	((mode & ~Q_IN_CMT_) == 0)
		{ mode = ch == '\'' 	 ? Q_IN_CHAR :
						 ch == '"'		 ? Q_IN_STR	:
						 ch == beg_cmt ? Q_IN_CMT0 :
						 ch == paren.olcmt ? Q_IN_CMT0 :
						 ch == '\\' && paren.sdir < 0 
												&& (paren.prev == '\'' || paren.prev == '"')
													? paren.prev == '"' ? Q_IN_STR : Q_IN_CHAR : 0;
		}

		if (mode == 0)
		{
			if (ch == paren.ch)
			{ /*loglog2("INC %d %s", paren.nest, str);*/
				++paren.nest;
				++paren.nestclamped;
				if (paren.nestclamped <= 0)
					paren.nestclamped = 1;
			}
			if (toupper(ch) == paren.fence)
			{ /*loglog2("DEC %d %s", paren.nest, str);*/
				--paren.nest; /* srchdeffile needs relative and clamped nestings */
				--paren.nestclamped;
			}
		}
	} while (0);

/* if (mode & Q_IN_EOL)
		 mode &= ~(Q_IN_CMT+Q_IN_EOL);
*/
	paren.prev = ch;
	paren.in_mode = mode;
	return mode;
}



int Pascal scan_par_line(LINE * lp)

{ register int ix;
	for (ix = -1; ++ix < lp->l_used; )
		scan_paren((char)lgetc(lp, ix));
	return scan_paren('\n');
}


												/* returns pointing to eol or
												 * the first / of / / */
int Pascal scan_for_sl(LINE * lp)

{ int cplim = lp->l_used;
	int ix;
	char cmt = paren.olcmt;
	
	init_paren("{",0);

	for (ix = -1; ++ix < cplim; )
	{ 
		if (!(scan_paren(lp->l_text[ix])
						& (Q_IN_STR+Q_IN_CHAR+Q_IN_CMT+Q_IN_CMT_))
				&& lp->l_text[ix] == cmt && lp->l_text[ix+1] == cmt)
			break;
	}

	return ix;
}

#if 		CFENCE

int wordmatch(const char * t_, int add)

{ const char * t = t_;
	int ix = curwp->w_doto+add;
	const char * s = &curwp->w_dotp->l_text[ix];
	int len = llength(curwp->w_dotp) - ix;
	register char ch;

	for (; --len >= 0 && (ch = *t) != 0 && (ch | 0x20) == (*s | 0x20) && *s; ++s)
		++t;

	return *t != 0															 ? 0 :
				 len >= 0 &&(isalnum(*s) || *s == '_') ? 0 : 1;
}



typedef struct
{ int  koffs; 		/* out*/
	char blk_type;	/* in */
} fdcr;


/* -1: else by itself
	  1: normal fence
	  2: else fence
 */
int got_f_key(fdcr * fd, int tabsz)

{ const char * start = curwp->w_dotp->l_text-1;
	const char * pos = &start[curwp->w_doto+1];
	int choffs = 0;

	if (g_s_cmt + g_p_cmt)
		while (++start < pos && *start <= ' ')
			;
	else
	{	while (++start < pos && *start <= ' ')
		{ 
			choffs = choffs + 1;
			if (*start == '\t')
				choffs = (choffs + tabsz - 1) & -tabsz;
		}

		if (choffs < 6)
		{ if (choffs < 5)
				 return -3;
			choffs = -1;
			++start;
		}

		fd->koffs = choffs;

		if (toupper(pos[0]) == 'T' && g_s_cmt)
		{ /*char buf[2];*/
			if (wordmatch("THEN",0) == 0)
			{/*buf[1] = 0;
				buf[0] = 'A'+len;
				mbwrite(buf);*/
				return 0;
			}  

		{ LINE * pl = curwp->w_dotp;
			int clamp = 6;
			while (TRUE)
			{ 
				while (--pos >= start)
				{ if (toupper(*pos) == 'I' && toupper(pos[1]) == 'F' 
					 && (isspace(pos[2]) || pos[2] == '(')
					 && (pos == start || (!isalpha(*(pos-1)) && *(pos-1) != '_')))
						return 1;
					if (*pos == ';')
						return 0;
				}
				
				if (--clamp <= 0) break;

				pl = pl->l_bp;
				pos = &pl->l_text[llength(pl)-3];
				start = &pl->l_text[0];
			}}
		/*mbwrite("NOIF");*/
			return 0;
		}
	}

{ char * lbl = fd->blk_type == 'D' ? "DO" 	 :
							 fd->blk_type == 'L' ? "LOOP"  :
							 fd->blk_type == 'S' ? "SELECT": 
							 fd->blk_type == 'B' ? "BEGIN" : null;// Only val for s_cmt,p_cmt.
							 
	if (lbl != null)
	{	if (g_s_cmt && curwp->w_doto != 0 && *(pos-1) == '.')
			return 0;
		if (!g_s_cmt && pos != start)												// at beginning of line
			return 0;
		
		if (wordmatch(lbl,0) != 0)
			return 1;
		if (g_s_cmt + g_p_cmt && wordmatch("CASE", 0) != 0)
		  return 1;
	}

	if (toupper(pos[0])=='E')
	{ int len = llength(curwp->w_dotp) - curwp->w_doto;

		if (g_s_cmt + g_p_cmt)
		{ if (wordmatch("END", 0))
		  { return len <= 4 || !isspace(pos[3]) ||
		  	 			 wordmatch("IF", 4) == 0;
		  }
	  }
		else
		{	if (len >= 4)
			{ register int ix = 3;
				if (isspace(pos[3]) || len > 4 && isspace(pos[4]))
					++ix;
				if (*strmatch("ELSE", pos) == 0 && paren.ch != 'D') 
				{ if (paren.nest == 0 && (len == 4 || isspace(pos[4])))
						return 1;
					++ix;
					if (len <= ix)
						return 0;
				}
				else
				{ if (*strmatch("END", pos) != 0)
						return 0;
					if (g_s_cmt && (len == 3 || pos[3] < '0'))
					  return 1;															// is 1 correct?
				}

				if (g_s_cmt == 0)
				{ int d = wordmatch("DO", ix);
					int i = wordmatch("IF", ix);
			/*	char buf[2];
					buf[1] = 0;
					buf[0] = '5' + paren.nest;

					mbwrite(pos);
					mbwrite(buf);*/

					return lbl == null ? i != 0 						 ? 
																	toupper(pos[1]) == 'L' ? 2 : 1 :
																	toupper(pos[1]) == 'L' ? -1 : 0 :
								 *lbl == 'D' ? d												 :
								 *lbl == 'B' ? d == 0 && i == 0  ? 1 : 0 :
																									 wordmatch(lbl, ix);
				}
			}
		}
	}

	return 0;
}}


																/* the cursor is moved to a matching fence */
Pascal getfence(int f, int n)
				/* int f, n;		** not used */
{
	Lpos_t s = *(Lpos_t*)&curwp->w_dotp;	/* original line pointer */
	char * lstr = lgets(s.curline, s.curoff);

	int c_cmt = (curbp->b_langprops & BCCOMT);
	int f_cmt = (curbp->b_langprops & (BCFOR+BCSQL));
	int s_cmt = (f_cmt							& BCSQL);
	int p_cmt = (curbp->b_langprops & BCPAS);
	register char ch;
	fdcr fd;

	g_c_cmt = c_cmt;
	g_f_cmt = f_cmt;
	g_s_cmt = s_cmt;
	g_p_cmt = p_cmt;

	if (									s.curoff	 >= llength(s.curline) || 
			lstr[0] == '#' && s.curoff+3 >= llength(s.curline))
		return FALSE;
		
	if (lstr[0] == '#')
	{ c_cmt = 1;
		f_cmt = 0;
	}

	if (f_cmt != 0 && toupper(lstr[0]) == 'I' && toupper(lstr[1]) == 'F')
	{ int ct = 1000;
	/*mbwrite("looking for THEN");*/
		while (--ct > 0 && nextword(FALSE, 1))
			if (wordmatch("THEN",0) != 0)
				break;

		if (ct <= 0)
		{ mbwrite("Cant find THEN");
			goto beeper;
		}
		lstr = &curwp->w_dotp->l_text[curwp->w_doto];
	}

{ int tabsz = curbp->b_tabsize;
  int stt_ko = -1;
	int lastko = -1;
	int len = llength(curwp->w_dotp) - curwp->w_doto;
	int dir = init_paren(lstr,len);
	
	if (s_cmt && dir == 0 && paren.complex == 0)	// search backwards for begin
	{ paren.complex = true;
	  paren.sdir = -1;
	  dir = -1;
	  paren.nest = 1;
	  paren.ch = 'E';
	  paren.fence = 'B';
		fd.blk_type = paren.fence;
	}
	else
	while (paren.complex)						//once only
	{ int ix = lstr[0] == '#';
		if (c_cmt > ix)
		{ dir = 0;
			break;
		}
			
		if (len < 2)
		{ dir = 0;
			break;
		}

		if (s_cmt + p_cmt == 0 && toupper(lstr[ix]) == 'E' &&
				(wordmatch("ELSE", ix) || wordmatch(/*s_cmt ? "ELSIF" : */ "ELSEIF", ix)))
		{ /*mbwrite("GotELSEIF");*/
			fd.blk_type = 'T';
			paren.ch = 'T';
			paren.fence = 'E';
			paren.sdir = 1;
			dir = 1;
		}
		else if (f_cmt + p_cmt != 0)											// Fortran or Pascal or SQL
		{ char ch = toupper(lstr[1]);
			fd.blk_type = toupper(lstr[0]);
			
			if (fd.blk_type == 'E' && ch == 'N')   // END DO, END LOOP, etc
			{ register int ix = len <= 3 ? 0 : lstr[3] == ' ' ? 4 : 3;

				if (wordmatch("END",0) == 0)
					return FALSE;

			  if			(s_cmt || p_cmt)
			  	paren.fence = 'B';
			  else if (ix <= 0)
			  	;
				else if	(wordmatch("DO", ix) != 0
							|| wordmatch("LOOP", ix) != 0
							|| wordmatch("SELECT", ix) != 0)
				  paren.fence = toupper(lstr[ix]);
				else if (wordmatch("IF", ix) == 0)
					paren.fence = 'B';
				fd.blk_type = paren.fence;
			}
			else
			{ if			(fd.blk_type == 'T' && ch == 'H') ;
				else if (fd.blk_type == 'S' && ch == 'E' && s_cmt == 0) ;
				else if (fd.blk_type == 'D' && ch == 'O' && s_cmt == 0) ;
				else if (fd.blk_type == 'L' && ch == 'O' && s_cmt == 0) ;
				else if (fd.blk_type == 'B' && ch == 'E' && s_cmt + p_cmt != 0) ;
				else if (fd.blk_type == 'C' && ch == 'A') ;
				else
					return FALSE;
				if (fd.blk_type == 'C')
					fd.blk_type = 'B';
			}
		}
	  if (f_cmt)
		{ char * start = lgets(s.curline, 0);
			char * ls = start + s.curoff;
			lastko = 0;

			while (start < ls && (*start <= ' ' || f_cmt > s_cmt && lastko == 5))
			{ 
				lastko = lastko + 1;
				if (*start++ == '\t')
					lastko = (lastko + tabsz - 1) & -tabsz;
			}
			stt_ko = lastko;
		}
		break;
	}

/*mbwrite("Lkg");
	mbwrite(&fd.blk_type);*/
													 /* scan until we find it, or reach the end of file */
	while (paren.nest > 0 && forwchar(FALSE, dir))
	{ if (curwp->w_doto == llength(curwp->w_dotp))
		{ scan_paren('\n');
			if (dir < 0)
			{ if (c_cmt + s_cmt != 0)
				{	Paren_t sparen = paren;
					curwp->w_doto = scan_for_sl(curwp->w_dotp);
					paren = sparen;
				}
				if (f_cmt != 0)
				{ if (curwp->w_doto > 0 && toupper(curwp->w_dotp->l_text[0]) == 'C')
						curwp->w_doto = 0;
				}
			}
			continue;
		}
		ch = toupper(curwp->w_dotp->l_text[curwp->w_doto]);
		if (ch <= ' ') continue;
		if (curwp->w_doto > 0)
		 if ((in_range(ch,'A','Z') || ch == '_')
			&& in_range(toupper(curwp->w_dotp->l_text[curwp->w_doto-1]),'A','Z'))		// inside word
																													continue;

		if (s_cmt + p_cmt && ch == 'C')
				ch = 'B';

		if (scan_paren(ch) == 0 && paren.complex)
		{/*char buf[6];
			buf[2] = 0;
			buf[4] = 0;*/
			if			(f_cmt > s_cmt && ch == 'C' && curwp->w_doto == 0 && dir > 0)		// Fortran comment
			{ curwp->w_doto = llength(curwp->w_dotp);
			}
			else if (ch == paren.ch)
			{ 
				const char * w = &curwp->w_dotp->l_text[curwp->w_doto];
				if			(c_cmt != 0 && llength(curwp->w_dotp) - curwp->w_doto > 2 &&
								 (toupper(w[1]) == 'I' && toupper(w[2]) == 'F' 
							 || toupper(w[1]) == 'E' && toupper(w[2]) == 'N'))
				{ paren.nest += (2*(toupper(w[1]) == 'I')-1)*paren.sdir;	// do it
				/*buf[0] = 'f' + paren.nest;
					mbwrite(buf);*/
				}
				else if (f_cmt)
				{ ch = got_f_key(&fd, tabsz);
					if (ch < 0 && ch > -2 && paren.nest == 2 
					 && ( s_cmt || fd.koffs - 9 <= lastko))
					{ /*mbwrite("GotElse");*/
						paren.nest = 0;
					 /*mbwrite("Zero");*/
						lastko = fd.koffs;
					}
					else if (ch <= 0 || s_cmt == 0 && fd.koffs - 9 > lastko)
					{ --paren.nest;  // undo it
					/*buf[0] = 'n' + paren.nest;
						buf[1] = 'x'+ch;
						buf[2] = '@'+ko;
						buf[3] = '@'+lastko;
						mbwrite(buf);
						mbwrite(w);*/
					}
					else if (fd.koffs >= 0)
						lastko = fd.koffs;
				}
			}
			else if (ch == paren.fence && f_cmt != 0)
				
			{ ch = got_f_key(&fd, tabsz);
				if (ch <= 0 || s_cmt == 0 && fd.koffs - 8 > lastko)
				{ ++paren.nest;  // undo it
				/*buf[0] = 'W' + paren.nest;
					mbwrite(buf);*/
				}
				else if (f_cmt > s_cmt && fd.koffs >= 0)
					lastko = fd.koffs;
			}
		}
	}
												/* if count is zero, we have a match, move the sucker */
	if (paren.nest == 0)
	{ curwp->w_flag |= WFMOVE;
		if (stt_ko >= 0)
		{ char * start = lgets(curwp->w_dotp, 0);
			int len = curwp->w_dotp->l_used;
		  int ko = 0;

			while (--len >= 0 && (*start <= ' ' || f_cmt > s_cmt && ko == 5))
			{ ko = ko + 1;
				if (*start++ == '\t')
					ko = (ko + tabsz - 1) & -tabsz;
			}

		{ int diff = ko - stt_ko;
			if (diff < 0)
				diff = -diff;
			if (*lstr != '(' && *lstr != ')' && *lstr != '[' && *lstr != ']')
			if (in_range(diff,1,4) && (curwp->w_doto == 0 ||
						curwp->w_dotp->l_text[curwp->w_doto] != 
						curwp->w_dotp->l_text[curwp->w_doto-1] ))
				TTbeep();
		}}
		return TRUE;
	}

beeper:
																/* restore the current position */
	rest_l_offs(&s);
	TTbeep();
	return FALSE;
}}

#endif


#if 0
/*			Close fences are matched against their partners, and if
				on screen the cursor briefly lights there 							*/

Pascal fmatch(ch)
				char ch;				/* fence type to match against */
{
					 Lpos_t s = *(Lpos_t*)&curwp->w_dotp; /* original line pointer */

	register LINE * toplp = lback(curwp->w_linep);	 /*top line in curr window*/
	register int depth = 1;
													 /* setup proper open fence for passed close fence */
	register char opench = ch == ')' ? '(' :
												 ch == '}' ? '{' : '[';
	register char c;			/* current character in scan */

	update(FALSE);
	backchar(FALSE, 2);

					 /* scan back until we find it, or reach past the top of the window */
	while (depth > 0 && curwp->w_dotp != toplp)
	{ c = curwp->w_doto == llength(curwp->w_dotp) 
																	? '\r' : lgetc(curwp->w_dotp, curwp->w_doto);
		if (c == ch)
			++depth;
		if (c == opench)
			--depth;
		backchar(FALSE, 1);
	}
								/* if depth is zero, we have a match, display the sucker */
								/* there is a real machine dependant timing problem here we have
									 yet to solve......... */
	if (depth == 0)
	{
		forwchar(FALSE, 1);
	}
																						/* restore the current position */
	rest_l_offs(&s);
	return TRUE;
}

#endif



																/* increment or decrement a number */
Pascal arith(int f, int n)
				/* int f, n;		** not used */
{ 
	Lpos_t s = *(Lpos_t*)&curwp->w_dotp;	/* original line pointer */
	char * lstr = lgets(s.curline, s.curoff);
	int len = llength(s.curline) - s.curoff;
	int olen;
	int val,q,r;
	char * e;
	char buff[11];
	static int rads[] = {1,10,100,1000,10000,100000,1000000,10000000};
	
	if (len > 10) len = 10;
	
	if (len > 0 && in_range(*lstr,'0','9'))
	{ for (e = lstr; --len >= 0 && in_range(*e,'0','9'); ++e)
			;
		strpcpy(buff,lstr,e - lstr);
		buff[e - lstr] = 0;
		val = atoi(lstr) + n;
		
		olen = e - lstr;

		e = buff;
		if (val < 0)
		{ *e++ = '-';
			val = -val;
		}

		len = 1;
		r = 10;
		while (r <= val)
		{ ++len;
			r = r * 10;
		}
		while (--len >= 0)
		{ q = val / rads[len];
			val = val - q * rads[len];
			*e++ = '0' + q;
		}
		*e = 0;

		len = e - buff;
		if (len > olen)
		{ if (s.curoff > 0 && *(lstr-1) == ' ')
			{ --lstr;
				forwdel(f, -1);
			}
		}
		forwdel(f,olen);
		linstr(buff);
	}

	return TRUE;
}

