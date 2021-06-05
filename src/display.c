/*
 * The functions in this file handle redisplay. There are two halves, the
 * ones that update the virtual display screen, and the ones that make the
 * physical display screen the same as the virtual display screen. These
 * functions use hints that are left in the windows by the commands.
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
/*#include	<malloc.h>*/
#if 0
#include	<io.h>
#endif
#define 	IN_DISPLAY_C
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"logmsg.h"

#if S_MSDOS
#include <windows.h>

#define millisleep(n) Sleep(n)
#endif

extern int   g_cursor_on;

#define MARGIN	8 		/* size of minimim margin and */
#define SCRSIZ	64			/* scroll size for extended lines */

/* Standard terminal interface dispatch table. Most of the fields point into
 * "termio" code.
 */
TERM		term		= {
#if S_MSDOS
	NROW-1,
	NROW-1,
	NCOL,
	NCOL,
#else
	0, 0, 0, 0, /* these four values are set dynamically at open time */
#endif
	MARGIN,
	SCRSIZ,
};

#define CHROM_OFF 0x8000

typedef struct	VIDEO
{ unsigned char v_color;
	unsigned char v_flag; 	/* Flags, must never be '/' */
	short v_text[6];		/* Screen data. */
} VIDEO;

#define VFCHG 0x0001	/* Changed flag 		*/
#define VFEXT 0x0002	/* extended (beyond column 80)	*/
#define VFREV 0x0004	/* reverse video status 	*//* contig these 2*/
#define VFREQ 0x0008	/* reverse video request	*/
#define VFCOL 0x0010	/* color change requested OBSOLETE */
#define VFCMT 0x0020	/* starts in comment		*/
#define VFML	0x0040	/* is a modeline		*/


#if S_WIN32
#define V_BLANK 0x7
#elif S_MSDOS
#define V_BLANK 0x7
#else
#define V_BLANK 0
#endif

#define MEMMAP	S_MSDOS

static VIDEO	 **vscreen; 	/* Virtual screen. */
#if MEMMAP == 0
 static VIDEO 	**pscreen;		/* Physical screen. */
#endif

#if MEMMAP
#undef  pd_hjump
#define pd_hjump 1
#define MEMCT 1
#else
#define MEMCT 2
#endif

const char mdname[NUMMODES][8] = {		/* name of modes		*/
 "WRAP ", "CMODE ", "MS ", "AbC ",	"VW ",
 "OVER ", "RE ", "CRYPT ", "ASAVE ","//","CHGD", "INVS"
 };

static int currow;	/* Cursor row			*/
static int curcol;	/* Cursor column		*/

static void Pascal updateline(int);

#if S_WIN32

/* 0 : No special effect
 * 1 : underline
 * 2 : bold
 * 4 : Use new foreground
 * 8 : Use line foreground
 * An attribute of 0 therefore means the attributes for the line/new foreground
 */
unsigned short refresh_colour(int row, int col)

{ VIDEO * vp = vscreen[row];
  short * scl = vp->v_text;
  unsigned short line_attr = vp->v_color;
  unsigned short attr = line_attr;
  int icol;
  for (icol = -1; ++icol <= col; )
    if (scl[icol] & 0xf000)
		{	int wh = (scl[icol] & 0xff00) >> 8;
      if      (wh & 0x40)
      { attr = line_attr & 0xf0 | wh & 0xf;
	      if (wh & 0x8)												// try it
					attr |= COMMON_LVB_UNDERSCORE;
      }
      else if (wh & 0x80)
        attr = line_attr;
#if 0
      if (wh & 1)
				attr |= COMMON_LVB_UNDERSCORE;
			if (wh & 2)
			  attr |= FOREGROUND_INTENSITY;
#endif
    }

	return attr;
}  

#if 0

void Pascal blankpage()

{ 
	int lim = (term.t_mrowm1+1) * MEMCT; 
	int ix;
	for (ix = 0; ++ix < lim; )
		vscreen[ix]->v_color = V_BLANK;

}

void Pascal scroll_vscr()

{ VIDEO * hold = vscreen[0];

	int lim = (term.t_mrowm1+1) * MEMCT; 
	int ix;
	for (ix = 0; ++ix < lim; )
	{ vscreen[ix-1] = vscreen[ix];
	}
	 
	vscreen[lim-1] = hold;
}

#endif
#endif

/* Initialize this module. The edge vectors to access the screens are set up. 
 * The operating system's terminal I/O channel is set up. 
 * The original window has "WFCHG" set, so that it will get completely
 * redrawn on the first call to "update".
 */
void Pascal vtinit()

{ 	int i;
		VIDEO *vp;

/*	if (pd_gflags & MD_NO_MMI)
			return; */

#if S_WIN32
#else
{   char buf[40];
  	int lines = 0;
		int cols = 80;
		char * v = getenv("COLUMNS");
		if (v == NULL)
		{	FILE * ip = popen("stty size", "r");
			if (ip != NULL)
			{ v = fgets(buf, sizeof(buf), ip);
			  if (v != NULL)
				{	while (*v > ' ')
						++v;
					lines = atoi(buf);
				}
				pclose(ip);
			}
		}
		if (v != NULL)
			cols = atoi(v);

		if (cols > 0)
		{ term.t_ncol = cols;
			term.t_mcol = cols;
			term.t_margin = cols / 10;
		}

		if (lines <= 0)
		{ lines = SCR_LINES;
			v = getenv("LINES");
			if (v != NULL)
				lines = atoi(v);
		}
		lines -= 2;
		if (lines > 0)
		{ term.t_nrowm1 = lines;			
			term.t_mrowm1 = lines;
		}
}
		tcapsetfgbg(0x70);			/* white background */ // should be derived
#endif
		free((char*)vscreen);

#define ncols 

#define UESZ	((sizeof(VIDEO)-12+2*(term.t_mcol+3))*MEMCT)	// 3 extra

		i = (term.t_mrowm1+2) * MEMCT; 
		vscreen = (VIDEO **)mallocz(i * (sizeof(VIDEO*)+UESZ));
#if MEMMAP == 0
		pscreen = &vscreen[term.t_mrowm1+1];
#endif
		vp = (VIDEO *)&vscreen[i];
/*	if (vscreen == NULL)
			meexit(1);*/

		for ( ; --i >= 0; )
		{ vscreen[i] = vp;
			vp->v_color = V_BLANK;
			vp = (VIDEO *)&vp->v_text[term.t_mcol+2];
		}
}


#if CLEAN
/*	 free up all the dynamically allocated video structures
*/
int Pascal vtfree()
{
		free((char*)vscreen);
		vscreen = null;
		return 0;
}

#endif

#if  VFCMT != Q_IN_CMT
need equivalence
#endif


#if 0
/* Clean up the virtual terminal system, in anticipation for a return to the
 * operating system. Move down to the last line and clear it out (the next
 * system prompt will be written in the line). Shut down the channel to the
 * terminal.
 */
void Pascal vttidy()
{/*mlerase();
	tcapmove(term.t_nrowm1, 0);
	TTflush();*/
	tcapclose(1);
}
#endif

/* Markup language bold and underline marks must be in pairs here */

static
int find_match(const char * src, int len)

{ char ch = src[0];
  int dbl = len > 1 && src[1] == ch;
  if (dbl)
    ++src;
  while (--len >= 0)
  { if (*++src == ch)
    { if (!dbl || (len > 0 && src[1] == ch))
        return dbl;
    }
  }
  
  return -1;
}


#if S_MSDOS
static
const short ctrans_[] = 	/* ansi to ibm color translation table */
	{0, 4, 2, 6, 1, 5, 3, 7,
	 8, 12, 10, 14, 9, 13, 11, 15};

# define trans(x) ctrans_[x]
#else
# define trans(x) (x)
#endif

#if S_MSDOS
static const int bu_cols[] = { 4, 1 };

static int palcol(int ix)

{ int clr = toupper(palstr[ix]);
	return (in_range(clr,'0','9') ? clr - '0' :
				  in_range(clr,'A','F') ? clr - 'A' + 10 :
				  in_range(clr,'a','f') ? clr - 'a' + 10 : 0) << 8;
}
#else
static const int bu_cols[] = { '4'-1, '1'-1};
#define palcol(c) (((c) + 1) << 8)
#endif
//89:;<=>?                
Short g_clring;

#define CHR_0		 0x0000
#define CHR_UL   0x1000
#define CHR_BOLD 0x2000
#define CHR_NEW  0x4000
#define CHR_OLD  0x8000

/* Set the virtual cursor to the specified row and column on the virtual screen.
 * Bits 8 to 15 control attributes.
 * The background (bits 12 : 15) cannot be changed within a line and the 
 * background values have the following meaning (going forward):
 * 0 : No special effect
 * 1 : underline
 * 2 : bold
 * 4 : Use new foreground
 * 8 : Use line foreground
 * An attribute of 0 therefore means the attributes for the line/new foreground
 */
static VIDEO * Pascal vtmove(int row, int col, int cmt_chrom, LINE * lp)

{	const int BCD = BCCOMT + BCPRL + BCSQL + BCPAS;
	int  markuplen = 1;
	char markupterm = 0;
	int highlite = 0;
	int chrom_on = 0;		/* 1: ul, 2: bold, -1 manual */
	int chrom_nxt = 0;
	char duple = (curbp->b_langprops & BCPAS) == 0 ? '/' : ')';

	int len = llength(lp); 		/* an upper limit */
  unsigned char * str = (unsigned char *)&lp->l_text[-1];
	unsigned char s_props = *str;												/* layout dependent */
	*str = 0;																						/* restored below */

#if _DEBUG
	if (row >= term.t_mrowm1)
	{
	  tcapbeep();
		row = term.t_mrowm1 - 1;
	}
#endif

{	VIDEO *vp = vscreen[row];
	short * tgt = &vp->v_text[0];
	Short  clring = g_clring;
	int mode = cmt_chrom == 0 || clring == 0 || (vp->v_flag & VFML)
								 		? -1 : s_props & VFCMT;
												
	if ((clring & BCD) && (s_props & VFCMT) ||
			(clring & BCFOR) && toupper(str[1]) == 'C' && len > 0)
		chrom_nxt = cmt_chrom;

//int chrom_in_peril = false;

{	short vtc = -col; // window on the horizontally scrolled screen; 0 at screen lhs

	while (--len >= 0)
	{	int chrom = chrom_nxt;
		chrom_nxt = CHR_0;
		if (vtc >= term.t_ncol)
		{ tgt[term.t_ncol - 1] = (short)(chrom + '$');
			continue;
		}
		
	{ int c = *++str;
		if (c != 0 && c == highlight[(++highlite)])
		{ if (highlight[1+highlite] == 0 && vtc - highlite + 1 >= 0)
			{ tgt[vtc-highlite+1] |= palcol(highlight[0]-'1') | CHR_NEW;
				highlite = 0;
			  chrom_on = 1;
				chrom_nxt = mode>0 && mode & (Q_IN_CMT+Q_IN_EOL) ? cmt_chrom : CHR_OLD;
			}
		}
		else
		{ highlite = 0;
			if (c != 0 && c == highlight[1])
				highlite = 1;
		}

		if			(c == '\t') 
		{ c = ' ';
			if (((vtc + 1 + col) % curbp->b_tabsize) != 0)
			{ --str;
				++len;
			}
		}
		else if (c < 0x20 || c == 0x7F)
		{ if (c >= col1ch && c <= col2ch)
			{ 
				chrom_nxt = palcol(c - col1ch); /* this is manual colouring */
				continue;
			}
			else
			{ if (vtc >= 0)
					tgt[vtc] = '^';
				++vtc;
				c ^= 0x40;
			}
		}
		else if (c < '0')
		{ if			(mode < 0 || (mode & Q_IN_EOL))
				;
			else if ((clring & BCML) && (c == '*' || c == '_') && str[-1] != c &&
			         ((str[-1] > ' ' || len > 0 && str[1] > ' ')))
			{ if      (str[-1] == '\\')
			    --vtc;
			  else if (mode & Q_IN_CMT0+Q_IN_CMT)
			  { if (c == markupterm && (markuplen == 0 || str[1] == markupterm))//safe
			    { str += markuplen;
             len -= markuplen;
			      chrom_on = 1;
			      chrom_nxt = CHR_OLD;
			      mode &= ~(Q_IN_CMT0+Q_IN_CMT);
			      continue;
			    }
			  }
			  else
         { int wh = find_match(str, len);
           if (wh >= 0)
           { chrom_nxt = wh ? chrom | CHR_UL : cmt_chrom;
             mode |= wh ? Q_IN_CMT : Q_IN_CMT0;
             str += wh;
             len -= wh;
             markuplen = wh;
             markupterm = c;
             continue;
           }
         }
			}
			else if (mode & (Q_IN_CMT+Q_IN_CMT0))
			{ if (str[-1] == '*' && c == duple)	/* l_props cannot be '*' */
				{ mode = 0;
					chrom_nxt = CHR_OLD;
				}
			}
			else if (c == '"')
				mode ^= Q_IN_STR;
			else if ((mode & (Q_IN_STR+Q_IN_EOL)) == 0)
				if ( 		
						(clring & BCCOMT) && (c == '*' || c == '/') && str[-1] == '/'||
						(clring & BCPAS)	&&	c == '*' && str[-1] == '(' ||
						(clring & BCSQL)	&&  c == '-'  && str[-1] == '-' ||
						(clring & BCFOR)	&&  c == '!' ||
						(clring & BCPRL)	&&  c == '#')
				{ mode = c != '*' ? Q_IN_EOL : Q_IN_CMT;
					chrom = cmt_chrom;
					if (vtc > 0 && (clring & (BCFOR | BCPRL)) == 0)
						tgt[vtc-1] |= cmt_chrom;
				}
		}

//	if (chrom_in_peril && chrom_on > 0)
//	{ chrom_in_peril = false;
//		if (vtc > 1)
//			tgt[vtc-2] |= CHR_OLD;
//		chrom_on = 0;
//	}
//	if (chrom_on > 0)
//		chrom_in_peril = true;
		if (vtc >= 0)
			tgt[vtc] = c | chrom;
		++vtc;
	}}

  --vtc;
{ int ct = term.t_ncol - vtc;

	if (vtc > 0 && chrom_on != 0)
	{
#if S_MSDOS == 0
		if (mode > 0)
#endif
		{ if ((tgt[vtc] & 0xff00) && ct < 0)
				tgt[++vtc] = ' ';
			tgt[vtc] |= CHR_OLD;
		}
	}
	while (--ct > 0)
	{ if (++vtc >= 0)
			tgt[vtc] = ' ';
	}

	lp->l_props = s_props;
	return vp;
}}}}


/************************** Modeline stuff *************************/


LINE *  g_lastlp;								// NULL => reset
static int     g_lastfcol;

/* Redisplay the mode line for the window pointed to by the "wp".
 * This is the only routine that knows how the modeline is formatted.
 * Called by "update" any time there is a dirty window.
 */
void Pascal modeline(WINDOW * wp)
	
{	int row;
	int n; 			      	/* cursor position count */
	union 
	{ LINE lc;
	  char c[NLINE+24]; /* buffer for mode line */
	}   tline;

	if (wp->w_dotp == g_lastlp /* and wp->w_next == NULL */)
	{ if (wp->w_fcol == g_lastfcol && (wp->w_flag & WFMODE == 0))
			return;
	}
/*else if (lback(lp) == g_lastlp)
		++g_lastlno;
	else if (lforw(lp) == g_lastlp)
		--g_lastlno;
	else
*/
/*g_lastlno = wp->w_line_no; */

	g_lastlp = wp->w_dotp;
#if S_MSDOS && S_WIN32 == 0
	n = wp == curwp ? 0xcd : /* 173 :  */
#else
	n = wp == curwp ? '='  :
#endif
										'-';
	tline.lc.l_used = term.t_ncol;
	memset(&tline.lc.l_text[0], n, NLINE);

	row = wp->w_toprow+wp->w_ntrows;		/* Location. */

#if _DEBUG
	if ((unsigned)row >= (unsigned)term.t_mrowm1)
	{ row = term.t_mrowm1 - 1;
		mbwrite("toprow+ntrows oor");
	}
#endif
	vscreen[row]->v_flag &= ~VFCMT;
#if S_MSDOS || OWN_MODECOLOUR
	vscreen[row]->v_color = 0x25; 		/* magenta on green */
	vscreen[row]->v_flag |= VFCHG | VFCOL | VFML;/* Redraw next time*/
#else
/*vscreen[row]->v_color = 0x7;			 * black on white */
	vscreen[row]->v_flag |= VFCHG | VFREQ | VFCOL | VFML;/* Redraw next time*/
#endif

{ BUFFER *bp = wp->w_bufp;
	int i = bp->b_flag;
	if (i & BFTRUNC)
		tline.lc.l_text[0] = '#'; 		/* "#" if truncated */

	if (i & BFCHG)
		tline.lc.l_text[1] = '*'; 		/* "*" if changed. */

	if (i & BFNAROW)
	{ tline.lc.l_text[2] = '<'; 		/* "<>" if narrowed */
		tline.lc.l_text[3] = '>';
	}

	if (wp == curwp)
	{ char * s = int_asc((int)wp->w_line_no);
																			/* take up some spaces */
		strcpy(&tline.lc.l_text[4], &s[strlen(s)-6]);
	} 												/* are we horizontally scrolled? */
	g_lastfcol = wp->w_fcol;
	if (g_lastfcol <= 0)
		strcpy(&tline.lc.l_text[10], " (");
	else
		concat(&tline.lc.l_text[10], "[", int_asc(g_lastfcol), "](", null);
					/* display the modes */
	
{ int cpix;
  int	modeset = wp->w_bufp->b_flag >> NUMFLAGS-1;
	for (i = -1; ++i < NUMMODES - 2; ) /* add in mode flags */
	{ if ((modeset = modeset >> 1) & 1) 
			strcat(&tline.lc.l_text[10], mdname[i]);
	}
	cpix = strlen(tline.lc.l_text);
	if (tline.lc.l_text[cpix-1] == ' ')
		--cpix;
	tline.lc.l_text[cpix] = ')'; 
	tline.lc.l_text[cpix+3] = ' ';
  tline.lc.l_text[cpix+1] = n;
  cpix += 4;

{ const char * s = bp->b_fname;

	if (s != NULL)	/* File name. */
	{
		const char * fn = s;
		for (s = &s[strlen(s)]; --s >= fn && *s != '/'; )
			;
						
		if (strcmp(s+1,bp->b_bname) != 0)
			s = NULL;

		cpix += strlen(strpcpy(&tline.lc.l_text[cpix], fn, NLINE - 4 - cpix))+1;
		tline.lc.l_text[cpix-1] = ' ';
	}

	if (s == NULL)
	{ tline.lc.l_text[cpix] = '@';
		strpcpy(&tline.lc.l_text[cpix+1], bp->b_bname, NLINE - 1 - cpix);
		tline.lc.l_text[strlen(tline.lc.l_text)] = ' ';
	}

//tline.lc.l_text[tline.lc.l_used] = 0;
	
	(void)vtmove(row, 0, 0, &tline.lc); 	/* Seek to right line. */
#if 0
	vtputs((wp->w_flag&WFCOLR) != 0  ? "C" : "");
	vtputs((wp->w_flag&WFMODE) != 0  ? "M" : "");
	vtputs((wp->w_flag&WFHARD) != 0  ? "H" : "");
	vtputs((wp->w_flag&WFEDIT) != 0  ? "E" : "");
	vtputs((wp->w_flag&WFFORCE) != 0 ? "F" : "");
#endif
}}}}

/*	updall: update all the lines in a window on the virtual screen */

void Pascal updall(WINDOW * wp, int wh)
																/* wh : < 0 => de-extend logic
																				= 0 => only w_dotp
																				> 0 =? all */
{ 	
	LINE *lp = wp->w_linep;
	int	sline = wp->w_toprow - 1;
	int	zline = sline + wp->w_ntrows;
			
	int color = window_bgfg(wp);
	int cmt_clr = (trans(cmt_colour & 0xf)) << 8 | CHR_NEW;

#if S_MSDOS == 0
	if (color == 0x70)
		color = V_BLANK;
#endif
#if _DEBUG
	if (zline >= term.t_mrowm1)
	{ /* addb(sline); */
		zline = term.t_mrowm1-1;
	}
#endif
	
	while (++sline <= zline)
	{ if      (wh < 0)
		{ if (vscreen[sline]->v_flag & VFEXT)
				if (lp != wp->w_dotp)
				{ VIDEO * vp = vtmove(sline, wp->w_fcol, cmt_clr, lp);
																		/* this line no longer is extended */
					vp->v_flag -= VFEXT;
					vp->v_flag |= VFCHG;
				}
		}	
		else if (wh || lp == wp->w_dotp)	/* and update the virtual line */
		{
			VIDEO * vp = vtmove(sline, wp->w_fcol, cmt_clr, lp);

			vp->v_color = color;
			vp->v_flag &= ~(VFREQ | VFML);
			vp->v_flag |= VFCHG | (lp->l_props & Q_IN_CMT);
		}
		if ((lp->l_props & L_IS_HD) == 0)/* if we are not at the end */
			lp = lforw(lp);
	}
}


#if 0

void Pascal updallwnd(int reload)

{ WINDOW * wp;

	for (wp = wheadp; reload && wp != NULL; wp = wp->w_next)
	{ updall(wp, true);
		modeline(wp);
	}

	for (int i = -1; ++i <= term.t_nrowm1; )
	{
		struct VIDEO *vp1 = vscreen[i]; /* virtual screen image */
		scwrite(i, vp1->v_text, vp1->v_color);
	}
}

#endif

#if MEMMAP
# define pteeol(row)
#else

void Pascal pteeol(int row)

{ 	register short * tgt = &pscreen[row]->v_text[0];
		register short	 sz = term.t_ncol;

		while (--sz >= 0)
			*tgt++ = ' ';
}


#if 0
X
XPascal psuppress(mrow)
X 	 int	mrow;
X{ int row = ttrow;
X  int col = ttcol;
X  tcapmove(mrow, 0);
X  tcapeeol();
X  pteeol(ttrow);
X  tcapmove(row, col);
X}
#endif

#endif


#if MEMMAP
# define pscroll(dir, stt, len)
#else

static void Pascal pscroll(int dir, int stt, int lenm1)
		/*	dir;			** < 0 => window down */
		/*	stt;			** start target location */
		/*	lenm1;									*/
{ int n = 1;
	int src = stt;
	int tgt = stt;

	if (dir < 0)
	{ ++src;
		n = 0;
	}
	else
		++tgt;

{ VIDEO *	vp = pscreen[stt+n*lenm1];
	memmove(&pscreen[tgt], &pscreen[src], lenm1 * sizeof(VIDEO*));
	pscreen[stt+(n ^ 1) * lenm1] = vp;

	n = term.t_ncol;
	while (--n >= 0)
		vp->v_text[n] = ' ';
}}

#endif


static void Pascal scrollupdn(int set, WINDOW * wp)/* UP == window UP text DOWN */
	 
{ int n = 1;
	int lenm1 = wp->w_ntrows-2;
	int stt = wp->w_toprow;

//if (wp->w_flag & WFMODE)
//	--lenm1;
//vtscroll(n, wp->w_toprow, lenm1);
	VIDEO * vp;
	int src = stt;
	int tgt = stt;

	if (set & WFTXTD)
	{ 
#if S_MSDOS == 0
		ttrow -= 2;								/* a trick */
#endif
		++src;
		n = 0;
	}
	else
		++tgt;

	vp = vscreen[stt+n*lenm1];
	vp->v_flag = VFCHG;
	memmove(&vscreen[tgt], &vscreen[src], lenm1 * sizeof(VIDEO*));
	vscreen[stt + (n ^ 1) * lenm1] = vp;

#if S_MSDOS == 0
	n = n * 2 - 1;
	tcapmove(ttrow - n + 1, ttcol);
	tcapscreg(stt, stt+lenm1);
	ttscupdn(n);
	pscroll(n, stt, lenm1);
#endif
	updall(wp, 0);
/* updateline(n < 0 ? wp->w_toprow : wp->w_toprow+lenm1);*/
#if 0 && MEMMAP == 0
X if (wp->w_ntrows < term.t_mrowm1 - 3)
X 	if ( n > 0)
X 	{ pteeol(0);
X 		if (wp->w_toprow == 0)
X 			psuppress(wp->w_ntrows+1);
X 		else
X 		{ psuppress(0);
X 			updateline(wp->w_toprow-1);
X 		}
X 	}
X 	else
X 		if (wp->w_toprow == 0)
X 		{ pteeol(term.t_mrowm1-2);
X 			psuppress(term.t_mrowm1-2);
X 		}
#endif
}
	 


int Pascal window_bgfg(WINDOW * wp)

{ BUFFER * bp = wp->w_bufp;
  int clr = bp == NULL ? g_colours : bp->b_color;
  
	return (trans((clr >> 4) & 0xf)<< 4) | trans(clr & 0xf);

//return (trans((wp->w_color >>8) & 0x7)<< 4) | trans(wp->w_color & 7);
}



void Pascal updline(int force)
/*	updpos: update the position of the hardware cursor and handle extended
		lines. This is the only update for simple moves.
*/
{
 static LINE * up_lp = NULL;
				int flag = 0;
		    WINDOW * wp = curwp;
				int fcol = wp->w_fcol;
			  LINE * lp;
				int col = getccol() - fcol; 	/* ensure not off the left of the screen */
				int j = (col + 1 - pd_hjump);
				
	if (j < 0 && pd_hjump > 0)
	{	fcol += (j  / pd_hjump) * pd_hjump;;
		flag = 1;
	}

	j = minfcol - fcol;
	if      (j > 0)
	{	fcol += j;
	  col -= j;
		if (col < 0)
			col = 0;
	}
	else if (up_lp != wp->w_dotp && 
			     col + fcol < term.t_ncol)
	{ col += fcol;
		fcol = 0;
		flag = 1;
	}

	currow = wp->w_toprow;

	up_lp = wp->w_dotp;

	for (lp = wp->w_linep; 
			 lp != up_lp;
			 lp = lforw(lp))
		++currow;

{ int i;
	int cmt_clr = (trans(cmt_colour & 0xf)) << 8 | CHR_NEW;
	int rhs = col - term.t_ncol + 1;
										
	if (pd_hjump)						/* if horizontal scrolling is enabled, shift if needed */
	{	if (rhs >= 0)
		{ flag = ((rhs + pd_hjump - 1) / pd_hjump) * pd_hjump;
			fcol += flag;
			col -= flag;
		}
	}
	else								// Not used in Windows
	{	if (rhs < 0)
			lbound = 0;
		else		 /*  updext: update the extended line which the cursor is currently
								 on at a column greater than the terminal width. The line
								 will be scrolled right or left to let the user see where
								 the cursor is. Called only in non Hscroll mode.
							*/
		{ 				/* calculate what column the real cursor will end up in */
			lbound = col - ((rhs - 1) % term.t_scrsiz) - term.t_margin + 1;

											/* scan through the line copying to the virtual screen*/
											/* once we reach the left edge						*/
											/* start scanning offscreen */
		{ VIDEO * vp = vtmove(currow, lbound + fcol, cmt_clr, up_lp);
			
//		vp->v_text[0] = /*BG(C_BLUE)+FG(C_WHITE)+*/'$';
								 			/* and put a '$' in column 1 */
			vp->v_flag |= (VFEXT | VFCHG);
		}}
	}

	wp->w_fcol = fcol;
	if (flag)
	  wp->w_flag |= WFHARD | WFMODE | WFMOVE;

					 /* update the current window if we have to move it around */
	if (wp->w_flag & WFHARD)
		updall(wp, 1);

	curcol = col;
	if (col < term.t_ncol - 1)			// allow deextension on this line
		wp->w_dotp = NULL;
																/*	upddex: de-extend any line that deserves it */

	for (wp = wheadp; wp != NULL; wp = wp->w_next)
	{	updall(wp,-1);

		wp->w_dotp = up_lp;
		modeline(wp);
		wp->w_flag = 0; 		/* we are done */
	}

/*	updupd: update the physical screen from the virtual screen	*/
{
#if MEMMAP == 0
	if (pd_sgarbf != FALSE)
	{  
	 /*tcapmove(0, 0);		** Erase the screen. */
		 tcapepage();			 /* Erase-page, also clears the message area. */
		 mpresf = FALSE;		 
	}
#endif

	for (i = -1; ++i <= term.t_nrowm1; )
	{
		VIDEO * vp = vscreen[i];
#if MEMMAP == 0
		if (pd_sgarbf)
		{ 
#if REVSTA && 0
//		vscreen[i]->v_flag &= ~VFREV;
#endif
			pteeol(i);
		}
#endif
				/* for each line that needs to be updated*/
		if (pd_sgarbf || (vp->v_flag & VFCHG))
		{ 
#if GOTTYPAH && S_MSDOS == 0 && 0
//		if (force == FALSE && typahead())
//			break;			/* this prob. breaks updgar !! */
#endif
#if MEMMAP
							/* UPDATELINE specific code for the IBM-PC and other compatables */
			vp->v_flag &= ~VFCHG;
			scwrite(i, vp->v_text, vp->v_color);
#else
			updateline(i);
#endif
		}
	}
	pd_sgarbf = FALSE;
}}}

											/* reframe:	check to see if the cursor is on in the window
												and re-frame it if needed or wanted 	*/
static 
int Pascal reframe(WINDOW * wp)

{	LINE *lp;			 /* search pointer */
//LINE *rp;			 /* reverse search pointer */

	LINE * top = wp->w_linep;
	int nlines = wp->w_ntrows;
	int centre = wp->w_force;
  int flags = wp->w_flag;
	wp->w_force = 0;
												/* if not a requested reframe, check for a needed one */
	if ((flags & WFFORCE) == 0)
	{ int i;
	  lp = top;
		for (i = nlines; --i >= 0; ) 
		{												 		/* if the line is in the window, no reframe */
			if (lp == wp->w_dotp)
				return flags;
																/* if we are at the end of the file, reframe */
			if (lp->l_props & L_IS_HD)
				break;
				
			lp = lforw(lp);						/* on to the next line */
		}
		if	(sscroll && (flags & WFHARD) == 0)
		{ if		  (wp->w_dotp == lp)
			{ lp = lforw(top);
				flags |= WFTXTD;
				goto ret;
			}
			else if ((wp->w_dotp->l_props & L_IS_HD) == 0 &&
							 lforw(wp->w_dotp) == top)
			{ lp = wp->w_dotp;
				flags |= WFTXTU;
				goto ret;
			}
			centre = (nlines >> 2) + 1;
		}
	}
#if 0
														 /* reaching here, we need a window refresh */
														 /* search thru the buffer looking for the point */
		{ int clamp = 0x7fff;  /* why is this being done? */
			rp = lp;
			
			while (lp != wp->w_dotp && rp != wp->w_dotp)
			{ if			((lp->l_props & L_IS_HD) == 0)
				lp = lforw(lp);
				else if (rp->l_props & L_IS_HD)
					break;
				if ((rp->l_props & L_IS_HD) == 0)
					rp = lback(rp);

				if (--clamp == 0)
				{ adb(51);
					break;
				}
			}
		}
#endif
															/* how far back to reframe? */
	if      (centre > 0) 				/* only one screen worth of lines max */
	{ if (centre > nlines)
			centre = nlines;
	} 
	else if (centre < 0)	/* negative update???? */
	{ centre += nlines - 1;
	} 
	else
		centre = nlines >> 1;
																			/* backup to new line at top of window */
	for (lp = wp->w_dotp; 
			 ((lp = lback(lp))->l_props & L_IS_HD) == 0 && --centre >= 0; )
		;

						 /* and reset the current line at top of window */
	pd_sgarbf = 1;
	flags |= WFHARD;
	lp = lforw(lp);
ret:
	wp->w_linep = lp;

	return flags;
}


/* Make sure that the display is right. This is a three part process.
 * First scan through all of the windows looking for dirty ones. 
 * Check the framing and refresh the screen. 
 * Second ensure that "currow" and "curcol" are correct for the current window.
 * Third make the virtual and physical screens the same.
 */
int /*Pascal*/ update(int force)
	/* int force; ** force update past type ahead? */
{
	WINDOW *wp;

	if (vscreen != NULL

#if GOTTYPEAH || VISMAC == 0
						&& (force || ( 1 
#endif
#if GOTTYPAH
				&& ! typahead() 
#endif
#if VISMAC == 0
				&& pd_kbdrep == 0
#endif
#if GOTTYPEAH || VISMAC == 0
			 ))
#endif
		 )
	{ 				/* update any windows that need refreshing */
		for (wp = wheadp; wp != NULL; wp = wp->w_next)
			if (wp->w_flag) 			 				/* if the window has changed, service it */
			{ int set = reframe(wp) & ~( WFMOVE+WFMODE); 
														 				/* check the framing */
				if			(set & (WFTXTU+WFTXTD))
					scrollupdn(set, wp);	
				else if (set == WFEDIT)
				{ updall(wp, 0);			/* update EDITed line */
					if (wp != curwp || ! (wp->w_flag & WFMODE))
						set = 0;
				} 
				else
				{ if (set)
						updall(wp, 1);						/* update all lines */
				}
			/* wp->w_changed = null; */
				wp->w_flag &= ~WFMODE;
//			if (set && 1)
//				wp->w_flag = WFMODE;
			}
					/* recalc the current hardware cursor location */
		updline(force);
				/* update physical screen from virtual screen */
					/* update the cursor and flush the buffers */
		tcapmove(currow, curcol - lbound);
		TTflush();	/* there may be cause to reverse move and flush */
	}
	return TRUE;
}


/* upscreen:	user routine to force a screen update
		always finishes complete update.
*/
int Pascal upscreen(int f, int n)

{ return update(TRUE);
}

/* Update a single line. This does not know how to use insert or delete
 * character sequences; we are using VT52 functionality. Update the physical
 * row and column variables. It does try and exploit erase to end of line.
 */
#if  MEMMAP == 0

static
void Pascal updateline(int row)
	 /* row;		** row of screen to update */
{
	short *ph1 = &pscreen[row]->v_text[0];

	struct VIDEO *vp1 = vscreen[row]; /* virtual screen image */

	short *cp1 = &vp1->v_text[0];
	short *cp9 = &cp1[term.t_ncol];
	short *cpend = cp9-1;
	int revreq = vp1->v_flag & VFREQ;		/* reverse video flags */
	int caution = ((revreq ^ pscreen[row]->v_flag) & VFREQ) ||
										vp1->v_color != pscreen[row]->v_color;
								// (vp1->v_flag & VFML) != 0;

	vp1->v_flag &= ~(VFCHG+VFREQ/*+VFML*/);/* flag this line is unchanged */

{	short prechrom = vp1->v_color;
	int bg = (vp1->v_flag & VFML) == 0 ? 0x70
																		 : (prechrom & 0x70);
	pscreen[row]->v_color = prechrom;
	pscreen[row]->v_flag = vp1->v_flag;

	if (pd_sgarbf)
		caution = FALSE;
				
/* 0 : No special effect
 * 1 : underline
 * 2 : bold
 * 4 : Use new foreground
 * 8 : Use line foreground */

	if (!caution) 								 /* skip common chars at the left */
	{ short *ph9 = &ph1[term.t_ncol];
		while (cp1 < cp9 && cp1[0] == ph1[0])
		{ if (*cp1 & 0xf000)
			{ int wh = (*cp1) >> 8;
				if      (wh & 0x40)
					prechrom = prechrom & 0xf0 | (wh & 0xf);
				else if (wh & 0x80)
					prechrom = vp1->v_color;
				else if (wh & 0x10)
					prechrom |= 0x8;																
				else if (wh & 0x20)
					prechrom |= 0x8;				// cannot do bold
			}
			++cp1;
			++ph1;
		}
			 /* Only called on changed lines but the following can still occur. 
				* A hard update is always done when a line splits, a massive 
				* change is done, or a buffer is displayed twice. */
		if (cp1 >= cp9)
			return;
															/* Erase to EOL ? */
		while (*--cp9 == *--ph9)	/* find out if there is a match on the right */
		{ if (cp9[0] != ' ')
				revreq = -revreq; 							 /* non-blanks to the right */
		}
	}

	tcapmove(row, cp1 - &vp1->v_text[0]);

{	short prechrom_ = prechrom;
	if (prechrom_ != 0)
		tcapchrom(prechrom_ | bg );

#if REVSTA
	if (revreq)
		tcaprev(TRUE);
#endif

	if (caution)
		tcapeeol();

	if (revreq > 0)
	{ while (cp9 > cp1 && cp9[0] == ' ')
			--cp9;

		if (cpend <= cp9 + 3) 	/* Use only if erase is */
			cp9 = cpend;					/* fewer characters. */
	}
	
	--cp1;
	while (++cp1 <= cpend)		/* Ordinary. */
	{	*ph1++ = *cp1;
		if (*cp1 & 0xf000)
		do
		{ int wh = (*cp1) >> 8;
			if      (wh & 0x40)
				prechrom = prechrom & 0xf0 | wh & 0xf;
			else if (wh & 0x80)
				prechrom = vp1->v_color;
			else if (wh & 0x10)
				prechrom |= 0x8;			// bold
			else if (wh & 0x20)
				prechrom |= 0x8;			// cannot do bold
			tcapchrom(prechrom | bg );
		}
		while (0);

		if (cp1 <= cp9)
		{ ttputc(*cp1);
			++ttcol;
		}
	}

#if REVSTA
	if (revreq != 0)
		tcaprev(FALSE);
	else
#endif

	if ( revreq > 0 && !pd_sgarbf && cp9 < cpend)
	{ if (row != term.t_nrowm1 || 1)
			tcapeeol();
		else
			while (++ttcol < term.t_ncol - 1)
				ttputc(' ');
	}

	if (prechrom != 0)
		tcapchrom(CHROM_OFF);
}}}

#endif

/* ################## end of window based routines #################### */

void Pascal upmode()	/* update all the mode lines */

{ orwindmode(WFMODE, 0);
}


void Pascal upwind()	/* force hard updates on all windows */

{ orwindmode(WFMODE | WFHARD | WFMOVE, 0);
}

/* Write a message into the message line. Keep track of the physical cursor
 * position. A small class of printf like format items is handled. 
 * Set the "message line" flag TRUE.	
 * Don't write beyond the end of the current terminal width.
 */
void Pascal mlout(char c)

{ if (c == '\b')
	{ if (ttcol <= 0)
			return;
		ttcol -= 2;
	}
	else
	{ if (ttcol + 1 >= term.t_ncol)
			return;
		lastmesg[ttcol] = c;
		lastmesg[ttcol+1] = 0;
	}
	if (pd_discmd > 0)
	{ int notused = pd_discmd;
		
		ttputc(/*(char)*/c);
#if S_WIN32
		vscreen[ttrow]->v_text[ttcol] = c;
#endif
	}
	ttcol += 1;
}


				/* inherited */ /* always zero outside display.c */
static int mlputli_width;

		/* Write out a string. Update the physical cursor position.
		 * This assumes that the characters in the string all have width "1";
		 * if this is not the case things will get screwed up a little.
		 */ 
void Pascal mlputs(const char * s)

{ int wid = mlputli_width;

	if (s != NULL)
	{	char ch;
		while ((ch = *s++) != 0)
		{ mlout(ch); 
			if (--wid == 0)
				break;
		} 
	}
		
	while (--wid >= 0)
		mlout(' ');
		
	mlputli_width = 0;
}


				/* output integer as radix r */
static void Pascal mlputli(Int l, int r)

{
	static char hexdigits[] = "0123456789ABCDEF";
		Int q;

		if (l < 0)
		{ l = -l;
			mlout('-');
		}

		--mlputli_width;
		q = l / r;
		if (q != 0)
			mlputli(q, r);

		for ( ++mlputli_width; --mlputli_width > 0; )
			mlout(' '); 		 
		
		mlout(hexdigits[(int)(l-q*r)]);
}



			/* write out a scaled integer with two decimal places */
void Pascal mlputf(int s)
			/* scaled integer to output */
{
	int integ = s / 100;
	int frac = s - integ * 100;	/* fractional portion of number */

	mlputli(integ, 10);
	mlout('.');
	integ = frac / 10;
	mlout((char)('0' + integ));
	mlout((char)('0' + (frac - integ * 10)));
} 			

#define get_arg(type,ap) va_arg(ap, type)

int mlwrite(const char * fmt, ...)
{
#define fmtch '%'

 va_list ap;
 va_start(ap, fmt);

	if      (fmt[0] == '%' && fmt[1] == '!')			/* force display */
		fmt += 2;
	else if (pd_discmd == 0)
		return 0;

	if (*fmt == 7)
	{ ++fmt;
		(void)tcapbeep();
	}

	mlputli_width = 0;

{	int  ch;
	int  s_discmd = pd_discmd++;
	Bool popup = false;
	--g_cursor_on;

	if (*fmt != '\001')
	{ --fmt;
#if COLOR && 0
		tcapbfcol(V_BLANK); /* set up the proper colors for the command line */
#endif
		tcapmove(term.t_nrowm1, 0);
		tcapeeol();
	}

	while ((ch = *++fmt) != 0)
	{ if (ch != fmtch)
			mlout((char)ch);
		else 
		{ while (((unsigned)(ch = *++fmt - '0')) <= 9)
				mlputli_width = mlputli_width * 10 + ch;
		{ int radix = 10;
			switch (ch+'0')
			{  case '>': pd_discmd = -1;
#if S_WIN32 == 0
				 when 'w': TTflush();
									 millisleep(4800);
#endif
				 when 'c': mlout((char)get_arg(int,ap));
				 when 'o': radix = 8 - 6;
				 case 'x': radix += 6;
				 case 'd':
									 mlputli((int)get_arg(int,ap), radix);
				 when 'p': popup = true;
				 when 's': mlputs(get_arg(char *, ap));
#if S_MSDOS == 0
				 when 'f': mlputf(get_arg(double, ap));
#else
				 when 'f': mlputf(get_arg(long, ap));
#endif
				 otherwise mlout((char)(ch+'0'));
			}
			mlputli_width = 0;
		}}
	}
	TTflush();

	mpresf = TRUE;
	if (pd_discmd < 0)
	{ (void)linstr(lastmesg);
		loglog1("mlw %s", fmtstr);
	}
#if S_WIN32
	else
		memset(&vscreen[term.t_nrowm1]->v_text[ttcol], 0, (term.t_ncol-ttcol)*2);
#endif

	loglog1("mlw %s", lastmesg);
		
	pd_discmd = s_discmd;
	if (popup)
		mbwrite(lastmesg);

	++g_cursor_on;
	return ttcol;															/* Number of characters */
}}

#if 0
		/* Force a string out to the message line regardless of the
			 current $discmd setting. This is needed when $debug is TRUE
			 and for the write-message and clear-message-line commands
		*/
void Pascal mlforce(const char * s)

{ ++pd_discmd;
//if (pd_gflags & MD_NO_MMI)
//	pd_discmd = FALSE;
#if S_BORLAND
	mlwrite(const_cast<char *>(s));
#else
	mlwrite(s);
#endif
	--pd_discmd;
}

#endif

/* Erase the message line. This is a special routine because the message line
 * is not considered to be part of the virtual screen. It always works
 * immediately; the terminal buffer is flushed via a call to the flusher.
 */
void Pascal mlerase()

{
	mlwrite("");
	mpresf = FALSE;
}

