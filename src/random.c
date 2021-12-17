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

static int g_s_cmt, g_ps_cmt;

Paren_t paren;
Paren_t g_sparen;

int Pascal init_paren(const char * str,
										  int len
										 )
{	Paren_t p = {0,};
	int p_cmt = (curbp->b_langprops & BCPAS);
	int f_cmt = (curbp->b_langprops & (BCFOR+BCSQL+BCPAS));
	int s_cmt = (f_cmt							& BCSQL);
	int c_cmt = (curbp->b_langprops & BCCOMT);
		
	p.olcmt = c_cmt ? '/' :
								s_cmt ? '-' : 
								p_cmt ? ')' : (char)-1;
	p.complex = true;
	p.sdir = 1;
//p.in_mode = 0;
//p.prev = 0;
//p.nest = len < 0 ? 0 : 1;
	if (len >= 0)
		++p.nest;
{	char nch = len <= 1 ? 0 : toupper(str[1]);
	p.ch = toupper(*str);
	
{ char ch = p.ch;
																						 /* setup proper matching fence */
	switch (ch)
	{
#if CHARCODE == ASCII
	  case '(':
		case ')': ch ^= 1;
							p.complex = false;
		when '[':
		case '{':
		case ']': 
		case '}': ch ^= 6;
							p.complex = false;
#else
		when '[': ch = ']';
							p.complex = false;
		when '{': ch = '}';
							p.complex = false;
		when ']': ch = '[';
							p.complex = false;
		when '}': ch = '{';
							p.complex = false;
#endif
		when '#': if			(len > 2 && nch == 'I' &&  str[2] == 'f')
								;
							else if (len > 2 && nch == 'E' && (str[2] =='n'|| str[2] =='l'))
								p.sdir = -1;
							else
							{ p.nest = 0;
								ch = 0;
							}
		when 'T':
		case 'S':
		case 'D':
		case 'L': 
							if (s_cmt)
								return 0;
		case 'B':
							if (nch == 0 ||
							   (ch == 'T' ? nch - 'H' : 
								  ch == 'S' ? nch - 'E' :
								  ch == 'B' ? nch - 'E' :
															nch - 'O') ||		// DO, LOOP
									ch == 'L' && len > 2 && toupper(str[2]) != 'O')
								return 0;
							ch = 'E';

		when 'C':	if (len < 2)
								return 0;

							if (s_cmt + p_cmt)
							{ if (nch != 'A')
									return 0;
								p.ch = 'B';
								ch = 'E';
							}

		when 'E': 
//						if (nch != 'L')
							{ ch = 'T';
								p.sdir = -1;
							}
//						else
//						{ p.ch = 'T';
//							p.fence = 'E';
//							p.sdir = 1;
//							return 1;
//						}
							
		otherwise p.nest = 0;
							p.sdir = 0;
	}
	p.nestclamped = p.nest;
	p.fence = ch;
	if (ch < p.ch)			// not complex
	{	p.sdir = -1;
		if (p_cmt)
			p.olcmt = '(';
			
	}
{	g_sparen = paren;
	paren = p;	
	return p.sdir;
}}}}



int Pascal setcline(void) 			/* get the current line number */

{																 /* starting at the beginning of the buffer */
	LINE *tgt = curwp->w_dotp;
	LINE *lp;
	int numlines = 1;
				
	for ( lp = &curbp->b_baseline;
			 ((lp = lforw(lp))->l_props & L_IS_HD) == 0 && lp != tgt; )
		++numlines;

	curwp->w_line_no = numlines;
	return numlines;
}


/* This routine, given a pointer to a LINE, and the current cursor goal
 * column, return the best choice for the offset. The offset is returned.
 * Used by next-line and backward-line.
 */
int Pascal USE_FAST_CALL getgoal(int offs, LINE * dlp)

{ int col = 0;
	int dbo;
	int lim = offs;
	if (lim >= 0)											// if getccol then result will be 0
		lim = llength(dlp);
	else															// getccol
	{	lim = -lim;
		offs = 0x7fffffff;
	}

{	int tabsize = curbp->b_tabsize;
	if (tabsize < 0)
		tabsize = - tabsize;

	for (dbo = -1; ++dbo < lim; )
	{ short c = lgetc(dlp, dbo);
		if (c < 0x20 || c == 0x7f)
		{ if (c == '\t')
				col += tabsize - 1 - (col % tabsize);
			else if (c >= pd_col1ch && c <= pd_col2ch)
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
}}


/* Return current column.  Stop at first non-blank given TRUE argument.
 */
int Pascal getccol()

{ return getgoal(- curwp->w_doto, curwp->w_dotp);
}

/* Display the current position of the cursor, in origin 1 X-Y coordinates,
 * the character that is under the cursor (in hex), and the fraction of the
 * text that is before the cursor. The displayed column is not the current
 * column, but the column that would be used on an infinite width display.
 * Normally this is bound to "C-X =".
 */

int Pascal showcpos(int notused, int n)

{	int		numchars = 0; 	/* # of chars in file */
	int		numlines = 0; 	/* # of lines in file */
	int		predchars;			/* # chars preceding point */
	int		predlines = -1;			/* # lines preceding point */
	int   savepos = curwp->w_doto;
	LINE * tgt = curwp->w_dotp;
  LINE * lp;						/* current line */
															/* starting at the beginning of the buffer */
																			/* start counting chars and lines */
	for (lp = &curbp->b_baseline; ((lp = lforw(lp))->l_props & L_IS_HD) == 0; )
	{ ++numlines;
		if (lp == tgt)		 /* record we are on the current line */
		{ predlines = numlines;
			predchars = numchars + savepos;
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
{	int len = llength(tgt);
	int ecol = getgoal(- len, tgt);

	mlwrite(TEXT60,
					/* "Line %d/%d Col %d/%d Char %D/%D (%d%%) char = 0x%x" */
						predlines, numlines, getccol(), ecol,
						predchars, numchars, (int)((100L*predchars) / (numchars + 1)), 
						len == savepos ? '\r' : lgetc(tgt, savepos));

	return (int)numchars;
}}


#if FLUFF
/* Twiddle the two characters on either side of dot. If dot is at the end of
 * the line twiddle the two characters before it. Return with an error if dot
 * is at the beginning of line; it seems to be a bit pointless to make this
 * work. This fixes up a very common typo with a single stroke. Normally bound
 * to "C-T". This always works within a line, so "WFEDIT" is good enough.
 */
int Pascal twiddle(int f, int n)

{	if (rdonly())
		return FALSE;
					
{	LINE *dotp = curwp->w_dotp;
	int	 doto = curwp->w_doto;

	if (doto == llength(dotp))
		--doto;

	if (doto <= 0)
		return FALSE;
{	int cr = lgetc(dotp, doto);
	lputc(dotp, doto, lgetc(dotp, doto-1));
	lputc(dotp, doto-1, cr);
	return lchange(WFEDIT);
}}}

#endif

/* Quote the next character, and insert it into the buffer. All the characters
 * are taken literally, including the newline, which does not then have
 * its line splitting meaning. The character is always read, even if it is
 * inserted 0 times, for regularity. Bound to "C-Q"
 */
int Pascal quote(int notused, int n)

{	if (n < 0)
		return FALSE;
{ int c = tgetc();
	int tabsz = curbp->b_tabsize;
	if (c == 'I'-'@')
		curbp->b_tabsize = 1;
{	int cc = linsert(n, (char)c);
	curbp->b_tabsize = tabsz;	
	return cc;
}}}

#if 0
  /* If given an argument then if > 0 expand the tab
     Otherwise if softtab is on then expand the tab (inserting spaces).
   */
int Pascal handletab(int f, int n)

{ int tabsz_ = curbp->b_tabsize;
  int tabsz = tabsz_ == 0 ? 1 : tabsz_ < 0 ? - tabsz_ : tabsz_;
  unsigned char mode = curbp->b_mode;

  if (f && n > 0)
  	mode = BSOFTTAB;

	return mode & BSOFTTAB ? linsert(tabsz - (getccol() % tabsz),' ')
                         : linsert(1,'\t');
}

#endif

#if 		AEDIT

int Pascal detab(int f, int n) /* change tabs to spaces */

{	if (rdonly())
		return FALSE;
{	int tabsz = curbp->b_tabsize;

	if ((f & 0x7fff) == FALSE)
		n = reglines(TRUE);

{	int inc = n > 0 ? 1 : -1;				/* increment to next line [sgn(n)] */

	for (; n; n -= inc)
	{ LINE * dotp;
		char ch;
		int    offs;											/* detab line */
		if (curwp->w_dotp->l_props & L_IS_HD)
			break;
		for (offs = -1; ++offs < llength((dotp = curwp->w_dotp)); )
			if ((ch = lgetc(dotp, offs)) == '\t')
			{ lputc(dotp, offs, ' ');
				curwp->w_doto = offs;
			{	int ins_ct = tabsz - (offs % tabsz) - 1;
				offs += ins_ct;
				insspace(TRUE, ins_ct);
			}}

 		if ((f & 0x8000))											/* entab the resulting spaced line */
		{ int tab_ct = tabsz;
			int sp_ct = 0;
			int outcol = 0;
			int incol;
			for (incol = -1; ++incol < offs; )
			{	if (incol - tab_ct == 0)
				{ tab_ct += tabsz;
					if (sp_ct > 1)
					{	outcol -= sp_ct;
						dotp->l_text[++outcol - 1] = '\t';
					}
					sp_ct = 0;
				}
			{	int ch = dotp->l_text[incol];
				dotp->l_text[++outcol - 1] = ch;
				++sp_ct;
				if (ch != ' ')
					sp_ct = 0;
			}}
			dotp->l_used = outcol;
		}
														/* advance/or back to the next line */
		forwbyline(inc);
	}
		
	curwp->w_doto = 0;			/* to the begining of the line */
//thisflag &= ~CFCPCN;		/* flag that this resets the goal column */
	return lchange(WFEDIT);	/* yes, we have made at least an edit */
}}}


int Pascal entab(int f, int n) /* change spaces to tabs where posible */

{	return detab(f | 0x8000, n);
}			

/* trim:				trim trailing whitespace from the point to eol
								with no arguments, it trims the current region
*/
int Pascal trim_white(int f, int n)

{	if (rdonly())
		return FALSE;

	if (f == FALSE)
		n = reglines(TRUE);

{ LINE *lp;									/* current line pointer */
/*int length;								** current length */
	int inc = n > 0 ? 1 : -1; /* increment to next line [sgn(n)] */

	for (; n; n -= inc)
	{ 			
		lp = curwp->w_dotp; 						/* find current line text */
																		/* trim the current line */
		lp->l_used = trimstr(lp->l_used,lp->l_text);
																				/* advance/or back to the next line */
		forwbyline(inc);
	}
//thisflag &= ~CFCPCN;		/* flag that this resets the goal column */
	return lchange(WFEDIT);
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
int Pascal indent(int notused, int n)

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
								 						/* find last non-blank line to get indentation from */
	while (lp->l_used == 0 && (lp->l_props & L_IS_HD) == 0)
		lp = lback(lp);
													/* grab a pointer to text to copy indentation from */
{	char *cptr = skipspaces(&lp->l_text[0],
													&lp->l_text[lp->l_used]);
	char schar = *cptr;
	*cptr = 0;				
	linstr(lp->l_text); 							 /* insert this saved indentation */
	*cptr = schar;
																			
	return TRUE;
}}


/* Open up some blank space. The basic plan is to insert a bunch of newlines,
 * and then back up over them. Everything is done by the subcommand
 * processors. They even handle the looping. Normally this is bound to "C-O".
 */
int Pascal openline(int notused, int n)

{	if (rdonly())
		return FALSE;

	if (n <= 0)
		return n == 0;

{	int		s;
	int i = n;																	/* Insert newlines. 		*/
	while ((s = lnewline()) > FALSE && --i > 0)
		;

	if (i > 0)															/* Then back up overtop */
		return s; 														/* of them all. 				*/
	return backbychar(n);
}}

#endif /* FLUFF */

/* Insert a newline. Bound to "C-M". If we are in CMODE, do automatic
 * indentation as specified.
 */
int Pascal ins_newline(int notused, int n)

{	if (rdonly())
		return FALSE;

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

{	int s;
																						/* insert some lines */
	while (--n >= 0)
		if ((s = lnewline()) <= FALSE)
			return s;

	return TRUE;
}}

/* Delete forward. This is real easy, because the basic delete routine does
 * all of the work. Watches for negative arguments, and does the right thing.
 * If any argument is present, it kills rather than deletes, to prevent loss
 * of text if typed with a big argument. Normally bound to "C-D".
 */
int Pascal forwdel(int f, int n)

{	if (rdonly())
		return FALSE;

	if (f != FALSE) 											/* Really a kill. 			*/
	{ if ((g_lastflag & CFKILL) == 0)
			kdelete(0,0);
	}
	if (n < 0)
	{ n = -n;
	{ int s = backbychar(n);
		if (s <= FALSE)
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
			++chunk;
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
int USE_FAST_CALL adjustmode(int kind, int global) /* change the editor mode */
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
	if (status <= FALSE)
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
			short * t = global ? &g_colours : &curbp->b_color;
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

int Pascal clrmes(int notused, int n)

{
	mlwrite("%!");
	return TRUE;
}

/*	This function writes a string on the message line
			mainly for macro usage									*/

int Pascal writemsg(int notused, int n)

{	char buf[NSTRING];

	int status = mlreply(TEXT67, buf, NSTRING - 1);
															/* "Message to write: " */
	if (status > FALSE)
		mlwrite(buf);

	return status;
}

#if 0
	 CMT0 				after the first /
	 CMT
	 CMT_ 				On the final / of /* xxx */
	 EOL					Comment to end of line
#endif

int Pascal scan_paren(char ch)
				
{	int beg_cmt = g_clring & BCPAS ? '(' :
								g_clring & BCPRL ? '#' :
	              g_clring & (BCCOMT+BCSQL) ? '/' : 257;
	int end_cmt = g_clring & BCPAS ? ')' : '/';
	int mode = paren.in_mode;
	int dir = paren.sdir;

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
							 ch == paren.prev  && dir >= 0 ? Q_IN_CMT + Q_IN_EOL : 0;
			} 			
		} 
		else if ((mode & (Q_IN_STR + Q_IN_CHAR)) != 0)
		{ if			(ch == '\n' && paren.olcmt != '-')
				mode = 0;
			else if ((mode & Q_IN_ESC) && dir >= 0)
				mode &= ~Q_IN_ESC;
			else if (ch == '\\' && dir >= 0)
				mode |= Q_IN_ESC;
			else if ((mode & Q_IN_STR) && ch == '"' ||
							 (mode & Q_IN_CHAR) && ch == '\'') 
				mode = 0;
			break;
		}
		else // if	((mode & ~Q_IN_CMT_) == 0)
		{ mode = ch == '\'' 	 ? Q_IN_CHAR :
						 ch == '"'		 ? Q_IN_STR	:
						 ch == beg_cmt ? beg_cmt == '#' ? Q_IN_CMT + Q_IN_EOL : Q_IN_CMT0 :
						 ch == paren.olcmt ? Q_IN_CMT0 :
						 ch == '\\' && dir < 0 
												&& (paren.prev == '\'' || paren.prev == '"')
													? paren.prev == '"' ? Q_IN_STR : Q_IN_CHAR : 0;
		}

		if (mode == 0)
#if 0
    {   if (ch == paren.ch)
        { /*loglog2("INC %d %s", paren.nest, str);*/
          ++paren.nest;
          ++paren.nestclamped;
//        if (paren.nestclamped <= 0)
//          paren.nestclamped = 1;
        }
        if (ch == paren.fence)
        { /*loglog2("DEC %d %c", paren.nest, ch);*/
          --paren.nest;	/* srchdeffile needs relative and clamped nestings */
          --paren.nestclamped;
        }
    }
#else
		{ int adj = 0;
			if (ch == paren.ch)
				++adj;
			if (toupper(ch) == paren.fence)
				--adj;
										/* -ve srchdeffile needs relative and clamped nestings */
			paren.nest += adj;
			paren.nestclamped += adj;
//		if (paren.nestclamped <= 0)
//			paren.nestclamped = 1;
		}
#endif
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
int Pascal USE_FAST_CALL scan_for_sl(LINE * lp)

{	init_paren("{", 0);

{	int cplim = lp->l_used;
	int ix;

	for (ix = -1; ++ix < cplim && !(scan_paren(lp->l_text[ix]) & Q_IN_EOL); )
		;

	paren = g_sparen;
	return ix == cplim ? ix : ix - 1;
}}

#if CFENCE

#if 0
char wordmatch(int add, const char * t)

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
int USE_FAST_CALL wordmatch(int add, const char * t_)

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
static
int got_f_key(fdcr * fd, int tabsz)

{ const char * start = curwp->w_dotp->l_text-1;
	const char * pos = &start[curwp->w_doto+1];
	int choffs = 0;

	while (++start < pos && *start <= ' ')
	{ 
		choffs = choffs + 1;
		if (*start == '\t')
			choffs = (choffs + tabsz - 1) & -tabsz;
	}

	if (! g_ps_cmt)
	{	if (choffs < 6)
		{ if (choffs < 5)
				 return -3;
			choffs = -1;
			++start;
		}

		fd->koffs = choffs;

		if (toupper(pos[0]) == 'T')
		{ /*char buf[2];*/
			if (wordmatch(0,"THEN") == 0)
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
		
		if (wordmatch(0,lbl) != 0)
			return 1;
		if (g_ps_cmt && wordmatch(0,"CASE") != 0)
		  return 1;
	}

	if (toupper(pos[0])=='E')
	{ int len = llength(curwp->w_dotp) - curwp->w_doto;

		if (g_ps_cmt)
		{ if (wordmatch(0,"END"))
		  { return len <= 4 || !is_space(pos[3]) ||
		  	 			 wordmatch(4, "IF") == 0;
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
				{ int d = wordmatch(ix, "DO");
					int i = wordmatch(ix, "IF");
			/*	char buf[2];
					buf[1] = 0;
					buf[0] = '5' + paren.nest;

					mbwrite(pos);
					mbwrite(buf);*/

					return lbl == null ? i ? (toupper(pos[1]) == 'L' ? 2 : 1)  :
																	 (toupper(pos[1]) == 'L' ? -1 : 0) :
								 *lbl == 'D' ? d												 						 :
								 *lbl == 'B' ? d + i == 0  ? 1 : 0 		 							 :
															 wordmatch(ix, lbl);
				}
			}
		}
	}

	return 0;
}}


																/* the cursor is moved to a matching fence */
int Pascal getfence(int f, int n)
				/* int f;		** not used */
{ int tabsz = curbp->b_tabsize;
	int c_cmt = (curbp->b_langprops & BCCOMT);
	int f_cmt = (curbp->b_langprops & (BCFOR+BCSQL));
	int p_cmt = (curbp->b_langprops & BCPAS);
	int s_cmt = (f_cmt							& BCSQL);
	int ps_cmt = s_cmt+p_cmt;
	fdcr fd;

	g_ps_cmt = ps_cmt;
	g_s_cmt = s_cmt;

{	Lpos_t s = *(Lpos_t*)&curwp->w_dotp;						/* original line pointer */
	int rem = llength(s.curline) - s.curoff - 1;
	char * lstr = lgets(s.curline, s.curoff);

	if (lstr[0] != '#')
	{ if (rem < 0)
			return FALSE;
	}
	else
	{ c_cmt = 1;
		f_cmt = 0;
		if (rem < 2)
			return FALSE;
	}

	if (rem >= 0 && f_cmt && toupper(lstr[0])=='I' && toupper(lstr[1]) == 'F')
	{ int ct = 1000;
	/*mbwrite("looking for THEN");*/
		while (--ct > 0 && nextword(1, 1))
			if (wordmatch(0,"THEN") != 0)
				break;

		if (ct <= 0)
		{ mbwrite("Cant find THEN");
			goto beeper;
		}
	}

  lstr = lgets(curwp->w_dotp, curwp->w_doto);

{	int len = llength(curwp->w_dotp) - curwp->w_doto;
	int dir = init_paren(lstr,len);
  int ch;
	int rc = true;
	int stt_ko = -1;
	int lastko = -1;

	if (n <= 0)
		if (paren.complex == 0)
			dir = init_paren(&paren.fence,0);
		else
			dir = -dir;
	
	if (s_cmt && dir == 0 && paren.complex == 0)	// search backwards for begin
	{ paren.complex = true;
	  paren.ch = 'E';
	  paren.fence = 'B';
		fd.blk_type = paren.fence;
	  paren.nest = 1;
	  dir = -1;
	}

	while (paren.complex)						//once only
	{ int ix = lstr[0] == '#';
		if (c_cmt > ix || len < 2)
		{ dir = 0;
			break;
		}
			
		if (ps_cmt)													// Fortran or Pascal or SQL
		{	if (toupper(lstr[ix]) == 'E' &&
					wordmatch(ix,"ELSE") + wordmatch(ix,"ELSEIF"))
			{ /*mbwrite("GotELSEIF");*/
				fd.blk_type = 'T';
				paren.ch = 'T';
				paren.fence = 'E';
				dir = 1;
			}
			else		
			{ char ch = toupper(lstr[1]);
				fd.blk_type = toupper(lstr[0]);
				
				if (fd.blk_type == 'E' && ch == 'N')   // END DO, END LOOP, etc
				{ int ix = len <= 3 ? 0 : lstr[3] == ' ' ? 4 : 3;
					char fch = toupper(lstr[ix]);
	
					rc = wordmatch(0,"END");
	
				  if			(ps_cmt)
				  	paren.fence = 'B';
				  else if (ix <= 0)
				  	;
					else if	(wordmatch(ix,fch == 'D' ? "DO" :
															  fch == 'L' ? "LOOP" : 
															  fch == 'S' ? "SELECT" : "IF"))
					  paren.fence = fch == 'I' ? 'B' : fch;
					fd.blk_type = paren.fence;
				}
				else
				{ if (fd.blk_type == 'T' && ch != 'H' ||
							fd.blk_type == 'C' && ch != 'A' ||
							fd.blk_type == 'B' && ( (ch != 'E') || ps_cmt == 0 ) ||
							fd.blk_type == 'S' && ch != 'E' ||
							fd.blk_type == 'D' && ch != 'O' ||
							fd.blk_type == 'L' && ch != 'O')
						rc = FALSE;
					if (fd.blk_type == 'C')
						fd.blk_type = 'B';
				}
			}
		}
	  if (f_cmt > s_cmt)
		{ char * start = lgets(s.curline, 0);
			char * ls = start + s.curoff;
			lastko = 0;

			while (start < ls && (*start <= ' ' || lastko == 5))
			{ 
				lastko = lastko + 1;
				if (*start++ == '\t')
					lastko = (lastko + tabsz - 1) & -tabsz;
			}
			stt_ko = lastko;
		}
		break;
	}

	paren.sdir = dir;

/*mbwrite("Lkg");
	mbwrite(&fd.blk_type);*/
													 /* scan until we find it, or reach the end of file */
	while (paren.nest > 0 && forwbychar(dir))
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

		if (ps_cmt && ch == 'C')
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
				{ paren.nest += (2*(toupper(w[1]) == 'I')-1)*dir;				// do it
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

			if (*lstr != '(' && *lstr != ')' && *lstr != '[' && *lstr != ']')
			if (in_range(ko - stt_ko,-4,4) && (offs == 0 ||
																				  lp->l_text[offs] != 
																				   lp->l_text[offs-1] ))
				TTbeep();					/* indent error */
		}
	}

  paren = g_sparen;

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
	backbychar(2);

					 /* scan back until we find it, or reach past the top of the window */
	while (depth > 0 && curwp->w_dotp != toplp)
	{ char c = curwp->w_doto == llength(curwp->w_dotp) 
																	 ? '\r' : lgetc(curwp->w_dotp, curwp->w_doto);
		if (c == ch)
			++depth;
		if (c == opench)
			--depth;
		backbychar(1);
	}
								/* if depth is zero, we have a match, display the sucker */
								/* there is a real machine dependant timing problem here we have
									 yet to solve......... */
	if (depth == 0)
	{
		forwbychar(1);
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
#if 1
	endword(1,1);
	endword(-1,-1);
//update(1);
  copyword(0,1);
{	char* src = getkill();
	if (in_range(*src,  '0', '9') || 
			in_range(src[1],'0', '9'))
	{	int val = atoi(src);
		int olen = strlen(src);
		char * tgt = int_asc(val+n);
		int len = strlen(tgt);
		int offs = curwp->w_doto;
		if (offs > 0 && isspace(curwp->w_dotp->l_text[offs-1]) &&
		    len > olen)
			forwdel(0,-1);
		forwdel(0,olen);
		linstr(tgt);
	}
}
#else
	Lpos_t s = *(Lpos_t*)&curwp->w_dotp;	/* original line pointer */
	char * lstr = lgets(s.curline, s.curoff);
	int len = llength(s.curline) - s.curoff;
	
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
		char * src = int_asc(val);
		len = strlen(src);

		if (len > olen)
		{ if (s.curoff > 0 && *(lstr-1) == ' ')
				forwdel(f, -1);
		}
		forwdel(f,olen);
		linstr(src);
	}}
#endif
	return TRUE;
}
