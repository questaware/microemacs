/* This file contains the command processing functions for a number of random
 * commands. There is no functional grouping here, for sure.
 */
#define IN_RANDOM_C 1

#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"logmsg.h"

int g_c_cmt, g_s_cmt, g_f_cmt, g_p_cmt;

int Pascal setcline() 			/* get the current line number */

{																 /* starting at the beginning of the buffer */
	LINE *tgt = curwp->w_dotp;
	LINE *lp;
	int numlines = 1;
				
	for ( lp = curbp->b_baseline;
			 ((lp = lforw(lp))->l_props & L_IS_HD) == 0 && lp != tgt; )
		++numlines;

	curwp->w_line_no = numlines;
	return numlines;
}


/* This routine, given a pointer to a LINE, and the current cursor goal
 * column, return the best choice for the offset. The offset is returned.
 * Used by next-line and backward-line.
 */
int Pascal getgoal(LINE * dlp, int offs)

{ int col = 0;
	int dbo;
	int lim = offs;
	if (lim >= 0)											// if getccol then result will be 0
		lim = llength(dlp);
	else															// getccol
	{	lim = -lim;
		offs = 0x7fffffff;
	}

	for (dbo = -1; ++dbo < lim; )
	{ short c = lgetc(dlp, dbo);
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

	return (offs & 0x40000000) ? col : dbo;
}


/* Return current column.  Stop at first non-blank given TRUE argument.
 */
int Pascal getccol()

{ return getgoal(curwp->w_dotp, - curwp->w_doto);
}

/* Display the current position of the cursor, in origin 1 X-Y coordinates,
 * the character that is under the cursor (in hex), and the fraction of the
 * text that is before the cursor. The displayed column is not the current
 * column, but the column that would be used on an infinite width display.
 * Normally this is bound to "C-X =".
 */
int Pascal showcpos(int f, int n)

{	LINE 		*lp;						/* current line */
	int		numchars = 0; 	/* # of chars in file */
	int		numlines = 0; 	/* # of lines in file */
	int		predchars;			/* # chars preceding point */
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
					/* "Line %d/%d Col %d/%d Char %D/%D (%d%%) char = 0x%x" */
						predlines, numlines, getccol(), ecol,
						predchars, numchars, ratio, cch);
	}
	return (int)numchars;
}}}



#if FLUFF
/* Twiddle the two characters on either side of dot. If dot is at the end of
 * the line twiddle the two characters before it. Return with an error if dot
 * is at the beginning of line; it seems to be a bit pointless to make this
 * work. This fixes up a very common typo with a single stroke. Normally bound
 * to "C-T". This always works within a line, so "WFEDIT" is good enough.
 */
int Pascal twiddle(int f, int n)

{	register LINE *dotp = curwp->w_dotp;
	register int	 doto = curwp->w_doto;

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

{ int c = tgetc();
	if (n < 0)
		return FALSE;
	return linsert(n, (char)c);
}


  /* If given an argument then if > 0 expand the tab
     Otherwise if softtab is on then expand the tab (inserting spaces).
   */
int Pascal handletab(int f, int n)

{ int tabsz_ = curbp->b_tabsize;
  int tabsz = tabsz_ == 0 ? 1 : tabsz_;
  unsigned char mode = curbp->b_mode;

  if (f && n > 0)
  	mode = BSOFTTAB;

	return mode & BSOFTTAB ? linsert(tabsz - (getccol() % tabsz),' ')
                         : linsert(1,'\t');
}

#if 		AEDIT
/*
int Pascal detabline()

{ LINE * dotp = curwp->w_dotp;
	int    llen = llength(dotp);
	int 	 tabsz = curbp->b_tabsize;
	int    offs;
	
	for (offs = -1; ++offs < llen)
		if (lgetc(dotp, offs) == '\t')
		{ lputc(dotp, offs, ' ');
			insspace(TRUE, tabsz - (offs % tabsz) - 1);
		}

	return OK;
}
*/


int Pascal detab(int f, int n) /* change tabs to spaces */

{ if (curbp->b_flag & MDVIEW) 		/* don't allow this command if	*/
		return rdonly();							/* we are in read only mode 		*/

	if ((f & 0x7fff) == FALSE)
		n = reglines();

{	int inc = n > 0 ? 1 : -1;				/* increment to next line [sgn(n)] */
	int tabsz = curbp->b_tabsize;

	for (; n; n -= inc)
	{ LINE * dotp = curwp->w_dotp;
		char ch;
		int    offs;											/* detab line */
		for (offs = -1; ++offs < llength(dotp); )
			if ((ch = lgetc(dotp, offs)) == '\t')
			{ lputc(dotp, offs, ' ');
				curwp->w_doto = offs;
			{	int ins_ct = tabsz - (offs % tabsz) - 1;
				offs += ins_ct;
				insspace(TRUE, ins_ct);
				dotp = curwp->w_dotp;
			}}

		if ((f & 0x8000))											/* entab the resulting spaced line */
		{ int tab_ct = tabsz;
			int sp_ct = 0;
			int incol;
			int outcol = -1;
			for (incol = -1; ++incol < offs; )
			{ 
				if (incol - tab_ct == 0)
				{ tab_ct += tabsz;
					if (sp_ct > 1)
					{ outcol -= sp_ct;
						dotp->l_text[++outcol] = '\t';
					}
					
					sp_ct = 0;
				}
			{	int ch = dotp->l_text[incol];
				dotp->l_text[++outcol] = ch;
				++sp_ct;
				if (ch != ' ')
					sp_ct = 0;
			}}
			dotp->l_used = outcol+1;
		}
														/* advance/or back to the next line */
		forwline(TRUE, inc);
	}
		
	curwp->w_doto = 0;			/* to the begining of the line */
//thisflag &= ~CFCPCN;		/* flag that this resets the goal column */
	lchange(WFEDIT);				/* yes, we have made at least an edit */
	return TRUE;
}}


int Pascal entab(int f, int n) /* change spaces to tabs where posible */

{	return detab(f | 0x8000, n);
}			

/* trim:				trim trailing whitespace from the point to eol
								with no arguments, it trims the current region
*/
int Pascal trim_white(int f, int n)

{
	if (curbp->b_flag & MDVIEW) 		/* don't allow this command if	*/
		return rdonly();							/* we are in read only mode 		*/

	if (f == FALSE)
		n = reglines();

{ LINE *lp;									/* current line pointer */
/*int length;								** current length */
	int inc = n > 0 ? 1 : -1; /* increment to next line [sgn(n)] */

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
		lp->l_used = trimstr(lp->l_text,lp->l_used);
#endif
																				/* advance/or back to the next line */
		forwline(TRUE, inc);
	}
	lchange(WFEDIT);
//thisflag &= ~CFCPCN;		/* flag that this resets the goal column */
	return TRUE;
}}
#endif


char * Pascal skipspaces(char * s, char * limstr)			
																/* char * limstr; * the last char */
{ int lim = limstr - s;
	int ix;
	char ch;
	
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
int Pascal indent(int f, int n)

{	if (n < 0)
		return FALSE;
{ char *src = &curwp->w_dotp->l_text[0];
	char *eptr = skipspaces(&src[0],&src[curwp->w_doto]);
	if (lnewline() == FALSE)				/* put in the newline */
		return FALSE;

{	char schar = eptr[0];
	eptr[0] = 0;

	linstr(src);									 /* and the saved indentation */
	eptr[0] = schar;

	return TRUE;
}}}

#if FLUFF

int Pascal cinsert()				/* insert a newline and indentation for C */

{ int bracef;				 								/* was there a brace at the end of line? */
	LINE *lp = curwp->w_dotp;
																		/* trim the whitespace before the point */
	int offset = curwp->w_doto;
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
		handletab(FALSE, 1);

	return TRUE;
}}


/* Open up some blank space. The basic plan is to insert a bunch of newlines,
 * and then back up over them. Everything is done by the subcommand
 * processors. They even handle the looping. Normally this is bound to "C-O".
 */
int Pascal openline(int f, int n)

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
int Pascal ins_newline(int f, int n)

{	int s;

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
			getccol() > pd_fillcol)
		execkey(&wraphook, FALSE, 1);
																						/* insert some lines */
	while (--n >= 0)
		if ((s=lnewline()) != TRUE)
			return s;

	return TRUE;
}

/* Delete forward. This is real easy, because the basic delete routine does
 * all of the work. Watches for negative arguments, and does the right thing.
 * If any argument is present, it kills rather than deletes, to prevent loss
 * of text if typed with a big argument. Normally bound to "C-D".
 */
int Pascal forwdel(int f, int n)
{
	if (curbp->b_flag & MDVIEW) 		/* don't allow this command if	*/
		return rdonly();							/* we are in read only mode 		*/
		
	if (f != FALSE) 											/* Really a kill. 			*/
	{ if ((g_lastflag&CFKILL) == 0)
			kdelete(0,0);
	}
	if (n < 0)
	{ n = -n;
	{ int s = backchar(f, n);
		if (s != TRUE)
			return s;
	}}

	g_thisflag |= CFKILL;
	return ldelchrs((Int)n, f);
}

/* Delete backwards. This is quite easy too, because it's all done with other
 * functions. Just move the cursor back, and delete forwards. Like delete
 * forward, this actually does a kill if presented with an argument. Bound to
 * both "RUBOUT" and "C-H".
 */
int Pascal backdel(int f, int n)
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
int Pascal killtext(int f, int n)
{
//extern char last_was_yank;

	kinsert_n = 0;
	if (n < 0)
	{ kinsert_n = -n;
		n = 1;
	}
#if 0
	if ((g_lastflag & CFKILL) == 0) 					/* Clear kill buffer if */
		kdelete(0,0); 												/* last wasn't a kill.	*/

	if (last_was_yank)
	{ last_was_yank = false;
		mbwrite("Discard KB");
		kdelete(0, kinsert_n);
	}
#endif
	g_thisflag |= CFKILL;
{ Int chunk = -curwp->w_doto;

	if			(f == FALSE)
	{ chunk += llength(curwp->w_dotp);
		if (chunk == 0)
			chunk = 1;
	} 
	else if (n == 0)
	{ chunk = -chunk;
		curwp->w_doto = 0;
	}
	else
	{ LINE *nextp = curwp->w_dotp;

		while (--n >= 0)
		{ if (nextp->l_props & L_IS_HD)
				return FALSE;
			chunk += llength(nextp)+1;
			nextp = lforw(nextp);
		}
	} 
	return ldelchrs(chunk, TRUE);
}}

static 
int Pascal adjustmode(int kind, int global) /* change the editor mode status */
				/* int kind;		** 0 = delete, 1 = set, 2 = toggle */
				/* int global;	** true = global flag,	false = current buffer flag */
{
	int status;											/* error return on input */
	char cbuf[NPAT];								/* buffer to recieve mode name into */

	concat(&cbuf[0], global ? TEXT62 : TEXT63,
/*														"Global mode to " */
/*														"Mode to " */
				
				TEXT64a[kind], null);
/*														 "delete: " */
/*														 "add: " */
/*														 "toggle: " */

	status = mlreply(cbuf, cbuf, NPAT - 1);
	if (status != TRUE)
		return status;

//mlerase();

{	int iter;
	int index = -1;
	int bestmatch = 0;
	int ix = NUMMODES;							/* loop index */
	
	for (iter = 2; --iter >= 0 && index < 0; )	/* test against the modes */
	{	for (; --ix >= 0;)												/* then against the colours */
		{ int best = 0;
			const char * goal = iter > 0 ? mdname[ix] : cname[ix];
			int match = strmatch(goal, cbuf) - goal;
			if (cbuf[match] != 0)
				continue;
			if (match < bestmatch)
				continue;
			if (match == bestmatch)
			{	index = -1;
				continue;
			}

			bestmatch = match;
			index = iter * 1024 + ix;		// big => mode
		}
		ix = NCOLORS;
	}
		
	if (index >= 0)
	{ if (index < NCOLORS)
		{ 
#if COLOR
			int mask = in_range(cbuf[0], 'A', 'Z') ? 0xf : 0xf0; // lc is ink
			int * t = global ? &g_colours : &curbp->b_color;
			*t &= mask;
			*t |= index << (4 & mask);
			curwp->w_flag |= WFCOLR;
#endif
		}
		else 
		{ int x = (MDSTT << (index-1024)); 			/* finding a match, we process it */
			if ((index - 1024 - 10) >= 0)
			{	x = (index-1024 - 10)== 0 ? BFCHG : BFINVS;
				global = 0;
			} 
		{	int md = global ? g_gmode : curbp->b_flag;
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
			{ char ** k = g_gmode & MDCRYPT ? &g_ekey : &curbp->b_key;
				if ((md & MDCRYPT) && *k == null)
				{ setekey(k);
					curbp->b_flag |= BFCHG;
				}
			}
#endif																	
			if (global == 0)						/* display new mode line */
				upmode();
		}}
		/*mlerase();*/
		return TRUE;
	}

	mlwrite(TEXT66);
/*							"No such mode!" */
	return FALSE;
}}


int Pascal delmode(int f, int n)		/* prompt and delete an editor mode */

{
	return adjustmode(0, FALSE);
}

int Pascal setmod(int f, int n) 		/* prompt and set an editor mode */

{
	return adjustmode(1, FALSE);
}


int Pascal togmod(int f, int n) 		/* prompt and set an editor mode */

{
	return adjustmode(2, FALSE);
}

int	Pascal delgmode(int f, int n) 	/* prompt and delete a global editor mode */

{
	return adjustmode(0, TRUE);
}

int Pascal setgmode(int f, int n) 	/* prompt and set a global editor mode */

{
	return adjustmode(1, TRUE);
}

int Pascal toggmode(int f, int n) 	/* prompt and set a global editor mode */

{
	return adjustmode(2, TRUE);
}



/*			This function simply clears the message line,
								mainly for macro usage									*/

int Pascal clrmes(int f, int n)

{
	mlwrite("%!");
	return TRUE;
}

/*	This function writes a string on the message line
			mainly for macro usage									*/

int Pascal writemsg(int f, int n)

{
	char buf[NSTRING];

	int status = mlreply(TEXT67, buf, NSTRING - 1);
															/* "Message to write: " */
	if (status == TRUE)
		mlwrite(buf);

	return status;
}

Paren_t paren;

int Pascal init_paren(const char * str,
										  int len
										 )
{ int c_cmt = (curbp->b_langprops & BCCOMT);
	int f_cmt = (curbp->b_langprops & (BCFOR+BCSQL+BCPAS));
	int s_cmt = (f_cmt							& BCSQL);
	int p_cmt = (curbp->b_langprops & BCPAS);
		
	paren.nest = 1;							// reinitialised by sinc.cpp
//paren.nestclamped = 1;			// initialised by sinc.cpp
	paren.sdir = 1;
	paren.in_mode = 0;
	paren.prev = 0;
	paren.complex = false;
	paren.olcmt = c_cmt ? '/' :
								s_cmt ? '-' : 
								p_cmt ? ')' : (char)-1;
	paren.ch = toupper(*str);
	
{ char ch = paren.ch;
																						 /* setup proper matching fence */
	switch (ch)
	{
#if CHARCODE == ASCII
	  case '(':
		case ')': ch ^= 1;
		when '[':
		case '{':
		case ']': 
		case '}': ch ^= 6;
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
				
{ int mode = paren.in_mode;
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
			{ 
				++paren.nest;
				++paren.nestclamped;
				if (paren.nestclamped <= 0)
					paren.nestclamped = 1;
			}
			if (toupper(ch) == paren.fence)
			{ 
				--paren.nest; 			/* srchdeffile needs relative and clamped nestings */
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

{ int ix;
	for (ix = -1; ++ix < lp->l_used; )
		scan_paren((char)lgetc(lp, ix));
	return scan_paren('\n');
}

												/* returns pointing to eol or
												 * the first / of / / */
int Pascal scan_for_sl(LINE * lp)

{	Paren_t sparen = paren;
	
	init_paren("{",0);

{	int cplim = lp->l_used;
	int ix;

	for (ix = -1; ++ix < cplim && !(scan_paren(lp->l_text[ix]) & Q_IN_EOL); )
		;

	paren = sparen;
	return ix == cplim ? ix : ix - 1;
}}

#if CFENCE

#if 0
char wordmatch(const char * t, int add)

{ int ix = curwp->w_doto + add;
	const char * s_ = &curwp->w_dotp->l_text[ix];
	int len = llength(curwp->w_dotp) - ix;
	
	while (TRUE)
	{	const char * s = s_;
		char res = *t;
	  char ch = '0';
	  int ct;

		for (ct = len;
				--ct >= 0 && (ch = *t) > 1 && (ch | 0x20) == (*s | 0x20); ++s)
			++t;

		if (ch <= 1 &&(ct < 0 || !(isalnum(*s) || *s == '_')))
			return res;
			
		while (*t >= 1)
			++t;
			
		if (*t++ == 0)
			return 0;
	}
}
#endif

static
int wordmatch(const char * t_, int add)

{ const char * t = t_;
	int ix = curwp->w_doto+add;
	const char * s = &curwp->w_dotp->l_text[ix];
	int len = llength(curwp->w_dotp) - ix;
	char ch;

	for (; --len >= 0 && (ch = *t) != 0 && (ch | 0x20) == (*s | 0x20); ++s)
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

		if (toupper(pos[0]) == 'T')
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
					 && (is_space(pos[2]) || pos[2] == '(')
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
	{	if (!g_s_cmt)
		{	if (pos != start)												// at beginning of line
				return 0;
		}
		else
			if (curwp->w_doto != 0 && *(pos-1) == '.')
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
		  { return len <= 4 || !is_space(pos[3]) ||
		  	 			 wordmatch("IF", 4) == 0;
		  }
	  }
		else
		{	if (len >= 4)
			{ int ix = 3;
				if (is_space(pos[3]) || len > 4 && is_space(pos[4]))
					++ix;
				if (*strmatch("ELSE", pos) == 0 && paren.ch != 'D') 
				{ if (paren.nest == 0 && (len == 4 || is_space(pos[4])))
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

					return lbl == null ? i ? (toupper(pos[1]) == 'L' ? 2 : 1)  :
																	 (toupper(pos[1]) == 'L' ? -1 : 0) :
								 *lbl == 'D' ? d												 						 :
								 *lbl == 'B' ? d + i == 0  ? 1 : 0 		 							 :
															 wordmatch(lbl, ix);
				}
			}
		}
	}

	return 0;
}}


																/* the cursor is moved to a matching fence */
int Pascal getfence(int f, int n)
				/* int f, n;		** not used */
{ int tabsz = curbp->b_tabsize;
	int c_cmt = (curbp->b_langprops & BCCOMT) >> 1;
	int p_cmt = (curbp->b_langprops & BCPAS);
	int f_cmt = (curbp->b_langprops & (BCFOR+BCSQL));
	int s_cmt = (f_cmt							& BCSQL);
	char ch;
	fdcr fd;

	g_s_cmt = s_cmt;
	g_f_cmt = f_cmt;
	g_p_cmt = p_cmt;
	g_c_cmt = c_cmt;

{	Lpos_t s = *(Lpos_t*)&curwp->w_dotp;						/* original line pointer */
	char * lstr = lgets(s.curline, s.curoff);
	int rem = llength(s.curline) - s.curoff - 1;

	if (lstr[0] != '#')
	{ if (rem < 0)
			return FALSE;
	}
	else
	{ c_cmt = 1;
		f_cmt = 0;
		if (rem < 2)
			return FALSE;
		if (rem > 3 && lstr[1] == 'e' && lstr[4] == 'e' && 
									 lstr[2] == 'l' && lstr[3] == 's')
			forwchar(FALSE, 1);
	}

	if (rem >= 0 && f_cmt != 0 && toupper(lstr[0])=='I' && toupper(lstr[1]) == 'F')
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

{ int ch;
	Paren_t sparen = paren;
	int rc = true;
	int stt_ko = -1;
	int lastko = -1;
	int len = llength(curwp->w_dotp) - curwp->w_doto;
	int dir = init_paren(lstr,len);

	if (n != 1 && paren.fence == '}')
		dir = init_paren("}",0); 
	
	if (s_cmt && dir == 0 && paren.complex == 0)	// search backwards for begin
	{ paren.complex = true;
	  paren.sdir = -1;
	  dir = -1;
	  paren.nest = 1;
	  paren.ch = 'E';
	  paren.fence = 'B';
		fd.blk_type = paren.fence;
	}

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
				wordmatch("ELSE", ix) + wordmatch("ELSEIF", ix))
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
			{ int ix = len <= 3 ? 0 : lstr[3] == ' ' ? 4 : 3;

				rc = wordmatch("END",0);

			  if			(s_cmt || p_cmt)
			  	paren.fence = 'B';
			  else if (ix <= 0)
			  	;
				else if	(wordmatch("DO", ix) ||
								 wordmatch("LOOP" ,ix) ||
								 wordmatch("SELECT", ix))
				  paren.fence = toupper(lstr[ix]);
				else if (wordmatch("IF", ix) == 0)
					paren.fence = 'B';
				fd.blk_type = paren.fence;
			}
			else
			{ if (!(fd.blk_type == 'T' && ch == 'H' ||
							fd.blk_type == 'C' && ch == 'A' ||
							fd.blk_type == 'B' && ch == 'E' && s_cmt + p_cmt != 0 ||
							( !s_cmt &&
							 (fd.blk_type == 'S' && ch == 'E' ||
							  fd.blk_type == 'D' && ch == 'O' ||
							  fd.blk_type == 'L' && ch == 'O'))))
					rc = FALSE;
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
	{ LINE * lp = curwp->w_dotp;
		int offs = curwp->w_doto;
		if (offs == llength(lp))
		{ scan_paren('\n');
			if (dir < 0)
			{	if (c_cmt + s_cmt != 0)
					curwp->w_doto = scan_for_sl(lp);
				else if (f_cmt != 0)
				{ if (offs > 0 && toupper(lp->l_text[0]) == 'C')
						curwp->w_doto = 0;
				}
			}
			continue;
		}
		ch = toupper(lp->l_text[offs]);
		if (ch <= ' ') 
			continue;
		if (offs > 0)
		 if ((in_range(ch,'A','Z') || ch == '_')
			&& in_range(toupper(lp->l_text[offs-1]),'A','Z'))// inside word
				continue;

		if (s_cmt + p_cmt && ch == 'C')
				ch = 'B';

		if (scan_paren(ch) == 0 && paren.complex)
		{/*char buf[6];
			buf[2] = 0;
			buf[4] = 0;*/																					// Fortran comment
			if			(f_cmt > s_cmt && ch == 'C' && offs == 0 && dir > 0)	
			{ curwp->w_doto = llength(lp);
			}
			else if (ch == paren.ch)
			{ 
				const char * w = &lp->l_text[offs];
				if			(c_cmt != 0 && llength(lp) - offs > 2 &&
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
	}	/* while */
												/* if count is zero, we have a match, move the sucker */
	if (paren.nest != 0)
		rc = FALSE;
	else
	{ curwp->w_flag |= WFMOVE;
		if (stt_ko >= 0)
		{ LINE * lp = curwp->w_dotp;
			int offs = curwp->w_doto;
			char * start = lgets(lp, 0);
			int len = lp->l_used;
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
			if (in_range(diff,1,4) && (offs == 0 ||
																 lp->l_text[offs] != 
																  lp->l_text[offs-1] ))
				TTbeep();
		}}
	}

  paren = sparen;

beeper:																/* restore the current position */
	if (!rc)
	{	rest_l_offs(&s);
		TTbeep();
	}
	return rc;
}}}

#endif


#if 0
/*			Close fences are matched against their partners, and if
				on screen the cursor briefly lights there 							*/

int Pascal fmatch(ch)
				char ch;				/* fence type to match against */
{
	Lpos_t s = *(Lpos_t*)&curwp->w_dotp; /* original line pointer */

	LINE * toplp = lback(curwp->w_linep);	 /*top line in curr window*/
	int depth = 1;
													/* setup proper open fence for passed close fence */
	char opench = ch == ')' ? '(' :
								ch == '}' ? '{' : '[';
	update(FALSE);
	backchar(FALSE, 2);

					 /* scan back until we find it, or reach past the top of the window */
	while (depth > 0 && curwp->w_dotp != toplp)
	{ char c = curwp->w_doto == llength(curwp->w_dotp) 
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
int Pascal arith(int f, int n)
				/* int f, n;		** not used */
{ 
	Lpos_t s = *(Lpos_t*)&curwp->w_dotp;	/* original line pointer */
	char * lstr = lgets(s.curline, s.curoff);
	int len = llength(s.curline) - s.curoff;
	
	static int rads[] = {1,10,100,1000,10000,100000,1000000,10000000};
	
	if (len > 10) 
		len = 10;
	
	if (len > 0 && in_range(*lstr,'0','9'))
	{	
		char buf[11];
		char * t = buf - 1;
		char * e;
		for (e = lstr-1; --len >= 0 && in_range(*++e,'0','9');)
			*++t = *e;
		*++t = 0;
		
	{	int val = atoi(buf) + n;
		int	olen = t - buf;
#if 1
		char * src = int_asc(val);
		len = strlen(src);
#else
#define src buf
		int q,r;
		e = buf - 1;
		if (val < 0)
		{ *++e = '-';
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
			*++e = '0' + q;
		}
		*++e = 0;
		len = e - buf;
#endif

		if (len > olen)
		{ if (s.curoff > 0 && *(lstr-1) == ' ')
				forwdel(f, -1);
		}
		forwdel(f,olen);
		linstr(src);
#undef src
	}}

	return TRUE;
}
