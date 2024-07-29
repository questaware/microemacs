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


Paren_t g_paren;

int init_paren(const char * str, int len)

{	Paren_t p;
  *(int*)&p = 0;
/*		
	p.olcmt = s_cmt ? '-' :
						p_cmt ? ')' :
						y_cmt ? '#' :
						c_cmt ? '/' : (char)-1;
*/
	p.sdir = 1;
//p.in_mode = 0;
//p.prev = 0;
//p.nest = len < 0 ? 0 : 1;
	if (len > 0)
		p.nest = 1;
	
	p.lang = curbp->b_langprops;
{	int s_cmt = (p.lang	& BCSQL);
  char ch = toupper(*str);
	p.ch = ch;
																						 /* setup proper matching fence */
	switch (ch)
	{
#if CHARCODE == ASCII
	  case '(':
		case ')': ch ^= 1;
							if (0)
		case '[':
		case '{':
		case ']': 
		case '}': 	ch ^= 6;
#else
		case '[': ch = ']';
							if (0)
		case '{':   ch = '}';
							if (0)
		case ']':   ch = '[';
							if (0)
		case '}': ch = '{';
#endif
							goto simple;
	}
	
	p.complex = true;
	if (len <= 2)
		return 0;
			
{	char nch = str[1] | 0x20;
	char ch2 = str[2] | 0x20;

	switch (ch)
	{
		when '#': if			(nch == 'i' &&  ch2 == 'f')
								;
							else if (nch == 'e')
								if (ch2 =='n')
									p.sdir -= 2;
//							if (ch2 =='l'))
//								p.sdir -= 2;
//						else
//						{ p.nest = 0;
//							ch = 0;
//						}
		when 'L': if (ch2 != 'o')
								return 0;
		case 'D':	nch -= 'o';
							if (0)
		case 'T':   nch -= 'h';
							if (0)
		case 'S':
		case 'B':	  nch -= 'e';
							if (nch /* | s_cmt */)
								return 0;
							ch = 'E';
		when 'C':	
//						if (s_cmt + p_cmt)
							{ if (nch != 'a')
									return 0;
								p.ch = 'B';
								ch = 'E';
							}

		when 'E': 
//						if (nch != 'L')
							{ ch = 'T';
								p.sdir -= 2;
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
simple:	
	
	p.nestclamped = p.nest;
	p.fence = ch;
	if (ch < p.ch)			// not complex
	{	p.sdir = -1;
//	if (p_cmt)
//		p.olcmt = '(';
	}

	g_paren = p;	
	return p.sdir;
}}}


#if 0
	 CMT0 				after the first /
	 CMT
	 CMT_ 				On the final / of /* xxx */
	 EOL					Comment to end of line
#endif


int Pascal USE_FAST_CALL scan_paren(char ch)
				
{ // Paren_t gg_paren = g_paren;
  int dir =  g_paren.sdir;
	int lang = g_paren.lang;
	const char g_beg_cmt[16] = "\000/#///#/(/#///#/";
	int beg_s1 = '\'';
	int beg_cmt = g_beg_cmt[g_clring & 15];
//int beg_cmt = g_clring & BCPAS ? '(' :
//							g_clring & BCPRL && dir > 0 ? '#' :
//              g_clring & (BCCOMT+BCSQL) ? '/' : 0;
	
	int end_cmt = beg_cmt == '(' ? ')' : '/';
	if (beg_cmt != '#')
	  beg_s1 = 0;
	else						// Python
	{	end_cmt = 0;
	
		beg_cmt = dir < 0 ? 0 : '\'';
	}

{	int mode = g_paren.in_mode;

	do
	{ if      (mode & (Q_IN_CMT + Q_IN_CMT0))
		{ 
			if      (mode & Q_IN_CMT)
			{ if (mode & Q_IN_EOL)
				{ if (ch == '\n')
						mode = 0;
					break;
				}
				else if ((ch == beg_s1 || ch == '"') && ch == g_paren.prev)
				{ if (mode & Q_IN_CMTL)
						mode = 0;
				  mode |= Q_IN_CMTL;
					break;
				}
				else if (ch == end_cmt && g_paren.prev == '*')
				{ mode = 0;
					break;
				}
			}
			else
			{ mode = beg_s1 												 ? Q_IN_CMT + Q_IN_EOL :
							 ch == '*'											 ? Q_IN_CMT 					 :
							 ch == '"'											 ? Q_IN_STR					   :
							 ch == '\''											 ? Q_IN_CHAR					 :
							 ch == beg_cmt &&
							 ch == g_paren.prev  && dir >= 0 ? Q_IN_CMT + Q_IN_EOL : 0;
			}
		} 
		else if (mode & (Q_IN_STR + Q_IN_CHAR))
		{ if			(ch == '\n' && (lang & BCSTRNL) == 0)
				mode = 0;
			else if (dir >= 0 && ((mode & Q_IN_ESC) || ch == '\\'))
				mode ^= Q_IN_ESC;
			else if (ch == "'\""[(mode >> Q_LOG_STR) & 1])
				mode = beg_s1 && ch == g_paren.prev ? Q_IN_CMT0 : 0;
			break;
		}
		else
		{ mode = ch == '\'' 	 ? Q_IN_CHAR :
						 ch == '"'		 ? Q_IN_STR	:
						 ch == beg_cmt ? Q_IN_CMT0 :
						 ch == '\\' && dir < 0 
												&& (g_paren.prev == '\'' || g_paren.prev == '"')
													? g_paren.prev == '"' ? Q_IN_STR : Q_IN_CHAR : 0;
		}

		if (mode == 0)
		{ int adj = 0;
			if (ch == g_paren.ch)
				++adj;
			if (toupper(ch) == g_paren.fence)
				--adj;
										/* -ve srchdeffile needs relative and clamped nestings */
			g_paren.nest += adj;
			g_paren.nestclamped += adj;
//		if (g_paren.nestclamped <= 0)
//			g_paren.nestclamped = 1;
		}
	} while (0);

/* if (mode & Q_IN_EOL)
		 mode &= ~(Q_IN_CMT+Q_IN_EOL);
*/
//*(int*)&g_paren = (((int)ch << 8 | (gg_paren.nestclamped & 0xff)) << 8 | 
//																	 (gg_paren.nest & 0xff) << 8) | mode;
  g_paren.prev = ch;
  g_paren.in_mode = mode;
	return mode;
}}



int Pascal scan_par_line(LINE * lp)

{	int cplim = lused(lp->l_dcr);
	int ix = -1;
	int mode = g_paren.in_mode;
	if ((g_clring & BCPRL) == 0)
		while (++ix <= cplim)
		{ mode = scan_paren((char)(ix == cplim ? '\n' : (char)lgetc(lp, ix)));
		}
	else
	{ cplim -= 2;
		for (ix = -1; ++ix < cplim; )
			if (lp->l_text[ix] == '"' || lp->l_text[ix] == '\'')
				if (lp->l_text[ix+1] == lp->l_text[ix] && lp->l_text[ix+2] == lp->l_text[ix])
					mode ^= Q_IN_CMT;

		g_paren.in_mode = mode; 
	}

	return mode;
}

												/* returns pointing to eol or
												 * the first / of / / */
int Pascal USE_FAST_CALL scan_for_sl(LINE * lp)

{	Paren_t s_paren = g_paren;
	init_paren("{", 0);

{	int cplim = lused(lp->l_dcr);
	int ix;

	for (ix = -1; ++ix < cplim && !(scan_paren(lp->l_text[ix]) & Q_IN_EOL); )
		;

	g_paren = s_paren;
	return ix == cplim ? ix : ix - 1;
}}



int Pascal setcline(void) 			/* get the current line number */

{																 /* starting at the beginning of the buffer */
	int numlines = bufferposition(-1, -1);
	curwp->w_line_no = numlines;
	return numlines;
}


/* This routine, given a pointer to a LINE, and the current cursor goal
 * column, return the best choice for the offset. The offset is returned.
 * Used by next-line and backward-line.
 */
int Pascal USE_FAST_CALL getgoal(int offs, LINE * dlp)

{	int lim = offs;
	if (lim >= 0)											// if getccol then result will be 0
		lim = llength(dlp);
	else															// getccol
	{	lim = -lim;
		offs = 0x7fffffff;
	}

{ int col = 0;
	int dbo;
	int tabsize = curbp->b_tabsize;
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


/* Return current column without using w_fcol.
 */
int Pascal getccol()

{ return getgoal(- curwp->w_doto, curwp->w_dotp); // only -ve from here
}

/* Display the current position of the cursor, in origin 1 X-Y coordinates,
 * the character that is under the cursor (in hex), and the fraction of the
 * text that is before the cursor. The displayed column is not the current
 * column, but the column that would be used on an infinite width display.
 * Normally this is bound to "C-X =".
 */

int Pascal bufferposition(int f, int n)

{	int		numchars = 0; 	/* # of chars in file */
	int		numlines = 0; 	/* # of lines in file */
	int		predchars;			/* # chars preceding point */
	int		predlines = 0;	/* # lines preceding point */
	int   savepos = curwp->w_doto;
	int		fcol = curwp->w_fcol;
	LINE * tgt = curwp->w_dotp;
  LINE * lp = &curbp->b_baseline;						/* current line */
															/* starting at the beginning of the buffer */
	do
	{ ++numlines;
		numchars += llength(lp) + 1;
		lp = lforw(lp);
		if (lp == tgt)		 /* record we are on the current line */
		{ predlines = numlines;
			predchars = numchars + savepos;
		}
	} while (!l_is_hd(lp));

{	int len = llength(tgt);

	if (f >= 0)
		mlwrite(TEXT60,
					/* "Line %d/%d Col %d/%d Char %D/%D (%d%%) char = 0x%x" */
					predlines, numlines, getccol(), getgoal(- len, tgt) + fcol, predchars, 
					numchars, 
					(int)((100L*predchars) / (numchars + 1)), 
					len == savepos ? '\r' : lgetc(tgt, savepos));

	return (int)predlines;
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

{ int c = tgetc();
	int tabsz = curbp->b_tabsize;
	if (c == 'I'-'@')
		curbp->b_tabsize = 1;
{	int cc = linsert(n, (char)c);
	curbp->b_tabsize = tabsz;	
	return cc;
}}

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

	if ((f & 1) == FALSE)
		n = reglines(TRUE);

{	int inc = n > 0 ? 1 : -1;				/* increment to next line [sign(n)] */

	for (; n; n -= inc)
	{	int tabsz = curbp->b_tabsize > 0 ? curbp->b_tabsize : -curbp->b_tabsize;
		LINE * dotp = curwp->w_dotp;
		char ch;
		int l_dcr = dotp->l_dcr;
		int  offs;											/* detab line */

		if (l_dcr == 0)
			break;
		for (offs = -1; ++offs < lused(l_dcr); )
			if ((ch = lgetc(dotp, offs)) == '\t')
			{ lputc(dotp, offs, ' ');
				curwp->w_doto = offs;
			{	int ins_ct = tabsz - (offs % tabsz) - 1;
				offs += ins_ct;
				linsert(ins_ct,' ');
			}}

 		if ((f & 2))											/* entab the resulting spaced line */
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
			dotp->l_dcr = (outcol << (SPARE_SZ+2)) + (dotp->l_dcr & SPARE_MASK) + 1;
		}

		forwbyline(inc);		/* advance/or back to the next line */
	}
		
	curwp->w_doto = 0;			/* to the begining of the line */
//thisflag &= ~CFCPCN;		/* flag that this resets the goal column */
	return lchange(WFEDIT);	/* yes, we have made at least an edit */
}}


int Pascal entab(int f, int n) /* change spaces to tabs where posible */

{	return detab(f | 2, n);
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
		lp = curwp->w_dotp; 						/* trim the current line */
																		/* lose the spare */
		lp->l_dcr = (trimstr(lused(lp->l_dcr),lp->l_text) << (SPARE_SZ+2)) 
		      		+ (lp->l_dcr & ((1 << (SPARE_SZ+2))-1));
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

	for (ix = -1; ++ix < lim && ((ch=s[ix]) != 0 && ch <= ' ');)
		;

	return &s[ix];
}


#if 0

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
	if (linsert(1,'\n') == FALSE)				/* put in the newline */
		return FALSE;

{	char schar = eptr[0];
	eptr[0] = 0;

	linstr(src);									 /* and the saved indentation */
	eptr[0] = schar;

	return TRUE;
}}}

#endif

#if FLUFF

int Pascal cinsert()				/* insert a newline and indentation for C */

{	LINE *lp = curwp->w_dotp;
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
	if (linsert(1,'\n') == FALSE)
		return FALSE;
														/* if the new line is not blank... don't indent it! */
	lp = curwp->w_dotp;
	if (lused(lp->l_dcr) > 0)
		return TRUE;
								 						/* find last non-blank line to get indentation from */
	while ((offset = lused(lp->l_dcr)) == 0)
		lp = lback(lp);
													/* grab a pointer to text to copy indentation from */
{	char *cptr = skipspaces(&lp->l_text[0],
													&lp->l_text[offset]);
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
	while ((s = lnewline(1)) > FALSE && --i > 0)
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

{				/* If a newline was typed, fill column is defined, the argument is non-
				 * negative, wrap mode is enabled, and we are now past fill column,
				 * and we are not read-only, perform word wrap.
				 */
	short lang = curbp->b_langprops & (BCCOMT+BCPRL+BCFOR+BCSQL+BCPAS);

	if ((curbp->b_flag & MDWRAP) && 
			getccol() > pd_fillcol)
		execwrap(1); // wraphook
{																						/* insert some lines */
	int doto = curwp->w_doto;
  LINE * lp = curwp->w_dotp;
	char ch = '\n';
  int ix;
	for (ix = -2; ++ix < doto; )
	{	int s = linsert(n,ch);
		if (s <= FALSE)
			return s;
		if (!lang)
			break;
		n = 1;
		ch = lp->l_text[ix+1];
		if (ch > ' ')
			break;
	}

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

	g_thisflag = CFKILL;

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

#if 0
	if ((g_lastflag & CFKILL) == 0) 					/* Clear kill buffer if */
		kdelete(0,0); 												/* last wasn't a kill.	*/

	if (last_was_yank)
	{ last_was_yank = false;
		mbwrite("Discard KB");
		kdelete(0, kinsert_n);
	}
#endif
	kinsert_n = 0;
//if (n < 0)
//{ kinsert_n = -n;
//	n = 1;
//}
	g_thisflag = CFKILL;
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
	{ if (n < 0)
		{ kinsert_n = -n;
			n = 1;
		}
	
	{ LINE *nextp = curwp->w_dotp;

		while (--n >= 0)
		{ if (l_is_hd(nextp))
				return FALSE;
			chunk += llength(nextp)+1;
			nextp = lforw(nextp);
		}
	}} 
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

{	int index = -1;
	int bestmatch = 0;
	int ix = NUMMODES + NCOLORS;							/* loop index */
	
	for (; --ix >= 0;)												/* first the colours */
	{ const char * goal = attrnames[ix];
		int match = strmatch(goal, cbuf) - goal;
		if (cbuf[match] != 0)
			continue;
			
		if (match > bestmatch || goal[match] <= ' ')
		{	bestmatch = match;
			index = ix;
			continue;
		}
		if (match == bestmatch)
			index = -1;
	}
		
	if (index >= 0)
	{ if (index >= NUMMODES)
		{ 
#if COLOR
			int mask = in_range(cbuf[0], 'A', 'Z') ? 0xf : 0xf0; // lc is ink
			short * t = global ? &g_bat_b_color : &curbp->b_color;			// TBD: make toggle work
			*t &= mask;
			*t |= (index - NUMMODES) << (4 & mask);
			curwp->w_flag |= WFCOLR;
#endif
		}
		else 
		{ int x = (MDSTT << index); 			/* finding a match, we process it */
			if ((index - 10) >= 0)
			{	x = (index - 10)== 0 ? BFCHG : BFINVS;
				global = 0;
			} 
		{	int md = global ? g_gflag : curbp->b_flag;
			if			(kind == 0)
					md &= ~x;
			else if (kind == 1)
					md |= x;
			else
					md ^= x;
			if (global)
				g_gflag = md;
			else
				curbp->b_flag = md;
#if CRYPT
			if (x == MDCRYPT)
			{ char ** k = g_gflag & MDCRYPT ? &g_bat_b_key : &curbp->b_key;
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
int USE_FAST_CALL wordmatch(int add, const char * t)

{ int ix = curwp->w_doto+add;
	const char * s = &curwp->w_dotp->l_text[ix];
	int len = llength(curwp->w_dotp) - ix;
	char ch;

	for (; (ch = *t) != 0 && --len >= 0 && ((ch ^ *s) * 0xef) == 0; ++s)
		++t;

	return ch == 0 && (len < 0 || (!isalnum(*s) && *s != '_'));
}


typedef struct
{ int  koffs; 			/* out*/
	char blk_type[2];		/* in */
} fdcr;


// static int g_s_cmt, g_ps_cmt;

/* -1: else by itself
	  1: normal fence
	  2: else fence
 */
static
int USE_FAST_CALL got_f_key(fdcr * fd, int tabsz, int fs_cmt)			// Fortran or SQL

{ LINE * lp = curwp->w_dotp;
	int offs = curwp->w_doto;
	int pos = offs+1;
	int choffs = 0;
  int start;
	char ch;

	for (start = -1; ++start < pos && (ch = lp->l_text[start]) <= ' '; )
	{ 
		choffs = choffs + 1;
		if (ch == '\t')
			choffs = (choffs + tabsz - 1) & -tabsz;
	}

	fs_cmt &= BCSQL;

	if (! fs_cmt)
	{	if (choffs < 6)
		{ if (choffs < 5)
				 return -3;
			choffs = -1;
			++start;
		}

		fd->koffs = choffs;

		if (/*toupper*/(lp->l_text[offs] | 0x20) == 't')
		{ /*char buf[2];*/
			if (wordmatch(0,"THEN") == 0)
			{/*buf[1] = 0;
				buf[0] = 'A'+len;
				mbwrite(buf);*/
				return 0;
			}  

		{ int clamp = 6;
			while (TRUE)
			{ 
				while (--pos >= start)
				{ ch = lp->l_text[pos-1];		// byte before start == 0
					if (lp->l_text[pos] == ';')
						return 0;
				  if ((pos == start  || (!isalpha(ch) && ch != '_'))
				   && /*toupper*/(lp->l_text[pos] | 0x20) == 'i' && 
				      /*toupper*/(lp->l_text[pos+1] | 0x20) == 'f'
					 && (lp->l_text[pos+2] <= ' ' || lp->l_text[pos+2] == '('))
						return 1;
				}
				
				if (--clamp <= 0) break;

				lp = lp->l_bp;
				pos = llength(lp)-3;
				start = 0;
			}
		/*mbwrite("NOIF");*/
			return 0;
		}}
	}

{ char * lbl = fd->blk_type[0] == 'D' ? "DO" 	 :
							 fd->blk_type[0] == 'L' ? "LOOP"  :
							 fd->blk_type[0] == 'S' ? "SELECT": 
						/* fd->blk_type[0] == 'B' ? */ "BEGIN"; // : null;// Only val for s_cmt.
							 
	if (lbl[0] == fd->blk_type[0])
	{	int rc;
		if (! fs_cmt)
		{	if (choffs > 0)												// at beginning of line
				return 0;
		}
		else
		{	if (lp->l_text[offs-1] == '.')	// byte before start == 0
				return 0;
		  rc = wordmatch(0,"CASE");
			if (rc)
			  return rc;
		}
		
		rc = wordmatch(0,lbl);
		if (rc)
			return rc;
	}

	if (/*toupper*/(lp->l_text[offs] | 0x20)=='e')
	{ int len = llength(lp) - offs;

		if (fs_cmt)
		{ if (wordmatch(0,"END"))
		  { return len <= 4 || lp->l_text[offs+3] > ' ' ||
		  	 			 wordmatch(4, "IF") == 0;
		  }
	  }
		else
		{ int ix = 3;
			if (len >= 4)
			{	if (len > 4 && lp->l_text[offs+4] <= ' ' || lp->l_text[offs+3] <= ' ')
					++ix;
				if (*strmatch("ELSE", &lp->l_text[offs]) == 0 && g_paren.ch != 'D') 
				{ if (g_paren.nest == 0 && ix == 3)
						return 1;
					++ix;
					if (len <= ix)
						return 0;
				}
				else
				{ if (*strmatch("END", &lp->l_text[offs]) != 0)
						return 0;
				}

			{ int d = wordmatch(ix, "DO");
				int i = wordmatch(ix, "IF");
			/*char buf[2];
				buf[1] = 0;
				buf[0] = '5' + g_paren.nest;

				mbwrite(offs);
				mbwrite(buf);*/

				return lbl == null ? (/*toupper*/(lp->l_text[offs+1]| 0x20) == 'l' ? -1 : 0) + i * 3 :
							 *lbl == 'D' ? d												 						     :
							 *lbl == 'B' ? d + i == 0  ? 1 : 0 		 							     :
														 wordmatch(ix, lbl);
			}}
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

	int ct = 0;
	Lpos_t spos = *(Lpos_t*)&curwp->w_dotp;						/* original line pointer */
	int len;
	char * lstr;
		
	while (1)
	{ len = llength(curwp->w_dotp) - curwp->w_doto;
  	lstr = lgets(curwp->w_dotp, curwp->w_doto);
  	if (len <= 0 || ct)
  		break;		
  
		if (!f_cmt)
			break;

		ct = -100;

		if (/*toupper*/(lstr[0] | 0x20) == 'i' 
		 && /*toupper*/(lstr[1] | 0x20) == 'f')
		{		/*mbwrite("looking for THEN");*/
			while (++ct < 0 && nextword(1, 1))
				if (wordmatch(0,"THEN") != 0)
					break;

			if (ct == 0)
			{ mbwrite(TEXT34);
				goto beeper;
			}
		}
	}

{	char answer[10];
  int rc = -1;
	int lastko = -1;

	Paren_t s_paren = g_paren;
	
	int dir = init_paren(lstr,len);										// The only call with len > 1
  char ch;
	fdcr fd;

	n *= 128;

//if (g_paren.fence == '#')
//	c_cmt = 1;

	if (g_paren.complex)
	{	if (n <= 0)
			dir = -dir;
	}
	else
	{	if      (s_cmt) 	// search backwards for begin
		{ --dir;
			g_paren.complex = true;
		  g_paren.ch = 'E';
		  g_paren.fence = 'B';
		  g_paren.nest = 1;
		}
		else if (dir)
		{	if (n <= 0)
				dir = init_paren((char*)&g_paren.fence,0);	// little endian
		}
		else
		{	mlwrite(TEXT35);
			answer[0] = ttgetc();
			dir = init_paren(answer, 1);
		}
	}

	while (g_paren.complex && len >= 2) /* ! simple */	 // once only 
	{ char ch_ = lstr[0];
		int ix = ch_ == '#';
		if (c_cmt > ix)
		{ dir = 0;
			break;
		}
			
		if ((lstr[ix] | 0x20) == 'e' &&
				wordmatch(ix,"ELSE") + wordmatch(ix,"ELSEIF"))
		{ /*mbwrite("GotELSEIF");*/
			fd.blk_type[0] = 'T';
			g_paren.ch = 'T';
			g_paren.fence = 'E';
			dir = 1;
		}
		else		
	  {	if (c_cmt)					// (not) Fortran or Pascal or SQL
				break;

		{ char ch = lstr[ix] | 0x20;	// lower
			fd.blk_type[0] = ch & ~0x20;	// upper
				
			if (fd.blk_type[0] == 'E' && ch == 'n')   // END DO, END LOOP, etc
			{ int nix = 0;
				if      (len < 3)
				{	if ((lstr[2] | 0x20) != 'd')
						rc = false;
				}
				else if (len > 3)
					nix = lstr[3] == ' ' ? 4 : 3;
					
			{	char fch = lstr[nix] & ~0x20;	// upper
				if (fch == 'I')
					fch = 'B';
	
			  if			(ps_cmt)
			  	fch = 'B';
				else if	(!wordmatch(nix,fch == 'D' ? "DO" :
														    fch == 'L' ? "LOOP" : 
														    fch == 'S' ? "SELECT" : "IF"))
				  fch = (char)g_paren.fence;
				g_paren.fence = (short)fch;
				fd.blk_type[0] = fch;
			}}
			else if (fd.blk_type[0] == 'I' && lstr[ix+1] == 'f')   // END DO, END LOOP, etc
			{	fd.blk_type[0] = 'E';
				g_paren.fence = (short)'E';
			}
			else
			{	if (fd.blk_type[0] == 'C')
					rc = fd.blk_type[0] = ch != 'a' ? 0 : 'B';
				if (fd.blk_type[0] == 'T' ?  ch != 'h' :
						fd.blk_type[0] == 'B' ? (ch != 'e' || ps_cmt == 0 ) :
						fd.blk_type[0] == 'S' ? ch != 'e'  : 
																    ch != 'o')
					rc = false;
			}
		}}
	  if (f_cmt & BCFOR)
		{ char * start = lgets(spos.curline, -1);
//		char * ls = start + spos.curoff;
			int ct = spos.curoff;
			char ch;
			lastko = 0;

			while (--ct >= 0 && ((ch = *++start) <= ' ' || lastko == 5))
			{ 
				lastko = lastko + 1;
				if (ch == '\t')
					lastko = (lastko + tabsz - 1) & -tabsz;
			}
		}
		break;
	}

	g_paren.sdir = dir;

{	int stt_ko = lastko;

/*mbwrite("Lkg");
	mbwrite(&fd.blk_type);*/
													 			/* until we find it or reach the end of file */
	while (rc && g_paren.nest > 0 && (rc = forwbychar(dir)))
	{ LINE * lp = curwp->w_dotp;
		int len = llength(lp);
		int offs = curwp->w_doto;
		if (offs == len)
		{ scan_paren('\n');
			if (dir < 0)
			{	if (c_cmt + s_cmt != 0)
					curwp->w_doto = scan_for_sl(lp);
				else if (f_cmt != 0)
				{ if (offs > 0 && /*toupper*/(lp->l_text[0] | 0x20) == 'c')
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
			&& in_range(/*toupper*/(lp->l_text[offs-1] | 0x20),'a','z'))			// inside word
				continue;

		if (ps_cmt && ch == 'C')
				ch = 'B';

		if (scan_paren(ch))
			continue;

		if (g_paren.nest <= 0 && !g_paren.complex)	/* simple */
		{	g_paren.nest = 1;
			if (++n >= 0)
		  	break;

			spos = *(Lpos_t*)&curwp->w_dotp;						// last match
		}

/*	  char buf[6];
			buf[2] = 0;
			buf[4] = 0;*/																					// Fortran comment
		if			(lastko >= 0 && ch == 'C' && offs == 0 && dir > 0)	
		{ curwp->w_doto = len;
		}
		else if (ch == g_paren.ch)
		{ 
			if (c_cmt && offs + 2 < len)
			{ char ch1 = (lp->l_text[offs+1] | 0x20) - 'i';
			  char ch2 = (lp->l_text[offs+2] | 0x20);
				if ((ch1 == 0       &&  ch2 =='f'
					|| ch1 == 'e'-'i' && (ch2 =='n'|| ch2=='l' && dir > 0 && g_paren.nest == 1)))
				{ g_paren.nest += (2*(ch1 == 0)-1)*dir;				// do it, c_cmt no # => dir == 0;
				/*buf[0] = 'f' + g_paren.nest;
					mbwrite(buf);*/
				}
			}
			else if (f_cmt)
			{ ch = got_f_key(&fd, tabsz, f_cmt);
				if (ch < 0 && ch > -2 && g_paren.nest == 2 
				 && fd.koffs - 9 <= lastko)
				{ 
					lastko = fd.koffs;
					break;
				}
				else if (ch <= 0 || s_cmt == 0 && fd.koffs - 9 > lastko)
				{ --g_paren.nest;  // undo it
				/*buf[0] = 'n' + g_paren.nest;
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
		else if (ch == g_paren.fence && f_cmt != 0)
		{ ch = got_f_key(&fd, tabsz, f_cmt);
			if (ch <= 0 || s_cmt == 0 && fd.koffs - 8 > lastko)
			{ ++g_paren.nest;  // undo it
			/*buf[0] = 'W' + g_paren.nest;
				mbwrite(buf);*/
			}
			else if (lastko >= 0 && fd.koffs >= 0)
				lastko = fd.koffs;
		}
	}	/* while */
	
  g_paren = s_paren;

	if (n < 0)
	{ rc = TRUE;
		rest_l_offs(&spos);
	}
																				
	if (rc)														// not at EOF
	{ curwp->w_flag |= WFMOVE;
		if (stt_ko >= 0)
		{	int offs = curwp->w_doto;
			LINE * lp = curwp->w_dotp;
			int len = lused(lp->l_dcr);
			int ix = -1;
		  int ko = 0;
		  
		  char ch;

			while (--len >= 0 && ((ch = lp->l_text[++ix]) <= ' ' || lastko >= 0 && ko == 5))
			{ ko = ko + 1;
				if (ch == '\t')
					ko = (ko + tabsz - 1) & -tabsz;
			}

			if (*lstr != '(' && *lstr != ')' && *lstr != '[' && *lstr != ']')
			if (in_range(ko - stt_ko,-4,4) && (lp->l_text[offs] != 		// l_dcr[3] == 0
																				  lp->l_text[offs-1] ))
				goto keepbeeper;
		}
	}

beeper:																/* restore the current position */
	if (rc <= 0)
	{	rest_l_offs(&spos);
keepbeeper:
		if (f >= 0)
			tcapbeep();
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

#define CALCSTACK 100

#if OPT_CALCULATOR

double evalexpr(char * s, int * adv_ref)

{ *adv_ref = -1;
{ char ch = *s;

//if (ch == '-')
//	ch = *++s;
	if (ch == '%')
	{ char buf[120];
		int v_adv = 0;
	  strpcpy(buf, s+1, sizeof(buf-1));
	  while (in_range(buf[v_adv], 'a', 'z') || in_range(buf[v_adv], '0', '9'))
	  	++v_adv;
	  buf[v_adv] = 0;
	  
	{ const char * ss = gtusr(buf);
	  if (ss != NULL)
		{ int that_adv;
		  double res = evalexpr(strpcpy(buf,ss,119), &that_adv);
	  	if (that_adv > 0)
		  	*adv_ref = v_adv+1;
	  	return res;
	  }
	}}
{ int tot_adv = 0;
	for (; ch == ' '; ch = s[++tot_adv])
		;
	for (; in_range(ch, '0', '9') || ch == '.' || ch == ','; ch = s[++tot_adv])
		if (ch == ',')
		{
			memmove(s+tot_adv, s+tot_adv+1, 20);
			--tot_adv;
		}

	if (tot_adv != 0)
	{	*adv_ref = tot_adv;
		return atof(s);
	}

	if (ch != '(')
		return 0.0;
			
{ double res = evalexpr(s+1, &tot_adv);
	if (tot_adv < 0) // || s[tot_adv+1] == 0)
		return 0.0;

	++tot_adv;

	while (1)
	{ char op = s[tot_adv++];
		if (op == ' ')
			continue;
	 	if (op == 0 || op == ')')
		{	*adv_ref = tot_adv;
			return res;
		}

	{	int adv2;
	  double res2 = evalexpr(s+tot_adv, &adv2);
		if (adv2 < 0)
			return 0.0;

		tot_adv += adv2;

		switch (op)
		{	case '-':
		 		res -= res2;
		 	when '*':
	 			res *= res2;
		 	when '/':
				if (res2 > -0.00000001 && res2 < 0.00000001)
					*adv_ref = -2;
				else
					res /= res2;
			when '+':
		 		res += res2;
		 	otherwise
				return 0.0;
		}
	}}
}}}}

#endif

																/* increment or decrement a number */
int Pascal arith(int f, int n)
				/* int f, n;		** not used */
{	endword(1,1);
	endword(-1,-1);
//update(1);
  copyword(0,1);
{	char* src = getkill();
	if (in_range(*src,  '0', '9') || 
			in_range(src[1],'0', '9'))
	{	int olen = strlen(src);
		int val = atoi(src);
		forwdel(0,olen);
	{	char * tgt = int_asc(val+n);
		if ((int)strlen(tgt) > olen)
		{ int offs = curwp->w_doto;
			if (offs > 0 && curwp->w_dotp->l_text[offs-1] <= ' ')
				forwdel(0,-1);
		}
		linstr(tgt);
	}}
	return TRUE;
}}

#if OPT_CALCULATOR
																/* increment or decrement a number */
int Pascal calculator(int f, int n)
				/* int f, n;		** not used */
 
{	while (1)
	{	Lpos_t spos = *(Lpos_t*)&curwp->w_dotp;	/* original line pointer */
		char * s = lgets(spos.curline, spos.curoff);
	  char buf[256*2+1];

		int ix = 0;
		int len = llength(spos.curline) - spos.curoff;
		if (len > 256) 
			len = 256;

		if (len <= 0)
			return TRUE;
		
		while (++ix < len)
	  	if (s[ix] == '=')
			{	((char*)memcpy(buf+1, s, ix))[ix] = 0;	// save variable
	  		s += ix + 1;														// new source
	  		len -= ix;
	  		ix = 0;
	  		break;
	  	}
		
	{	int adj;
		((char*)memcpy(buf+256+1, s, len))[len] = 0;
	  buf[0] = '%';
		buf[256] = '(';
	{	double res = evalexpr(buf+256, &adj);
		const char * all = adj <= 0 ? "Error" : float_asc(res);

		if (ix <= 0)
		{	set_var(buf, all);
			if (!forwline(1,1))
				return TRUE;
		}	
		else
		{	if (!f)
				mbwrite(all);
			kdelete(0, 0);
		{	int rc = kinsstr(all, strlen(all));
	#if S_MSDOS
			ClipSet(0);
	#endif
	
			return rc;
		}}
	}}}
}

#endif
