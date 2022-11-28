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

#if S_MSDOS && 0
#include <windows.h>

#define millisleep(n) Sleep(n)
#endif

//extern int   g_cursor_on;

#define MARGIN	8 		/* size of minimim margin and */
#define SCRSIZ	64			/* scroll size for extended lines */

/* Standard terminal interface dispatch table. Most of the fields point into
 * "termio" code.
 */
TERM		term		= {
#if S_MSDOS
	NROW-1,
	NCOL,
#else
	0, 0, /* these four values are set dynamically at open time */
#endif
#if MEMMAP == 0
	MARGIN,
	SCRSIZ,
#endif
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


#if S_MSDOS
#define V_BLANK 0x7
#else
#define V_BLANK 0
#endif

#define MEMMAP	S_MSDOS

static VIDEO	 **vscreen; 	/* Virtual screen. */
#if MEMMAP == 0
 static VIDEO 	**pscreen;		/* Physical screen. */
 static char      g_cmd_line[sizeof(lastmesg)+1];
#endif

#if MEMMAP
#undef  pd_hjump
#define pd_hjump 1
#define MEMCT 1
#else
#define MEMCT 2
#endif

const char mdname[NUMMODES][8] = {		/* name of modes		*/
 "WRAP ", "CMODE ", "MS ", "Aa ",	"VW ",
 "OVER ", "RE ", "CRYPT ", "ASAVE ","//","CHGD", "INVS"
 };

//static int g_currow;	/* Cursor row			*/
//static int g_curcol;	/* Cursor column		*/

static void Pascal updateline(int);

#if MEMMAP

/* 0 : No special effect
 * 1 : underline
 * 2 : bold
 * 4 : Use new foreground
 * 8 : Use line foreground
 * An attribute of 0 therefore means the attributes for the line/new foreground
 */
unsigned short USE_FAST_CALL refresh_colour(int row, int col)

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
#if S_WIN32 == 0
											// Does not work in wincon. does not compile in vs2019
	      if (wh & 0x8)											// try it
					attr |= COMMON_LVB_UNDERSCORE;
#endif
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
	int lim = (term.t_nrowm1+1) * MEMCT; 
	int ix;
	for (ix = 0; ++ix < lim; )
		vscreen[ix]->v_color = V_BLANK;

}

void Pascal scroll_vscr()

{ VIDEO * hold = vscreen[0];

	int lim = (term.t_nrowm1+1) * MEMCT; 
	int ix;
	for (ix = 0; ++ix < lim; )
	{ vscreen[ix-1] = vscreen[ix];
	}
	 
	vscreen[lim-1] = hold;
}

#endif
#endif

#if MEMMAP == 0

static
char * use_cmd(char * buf, int bufsz, const char * cmd)

{ char * v = NULL;
	FILE * ip = popen(cmd, "r");
	if (ip != NULL)
	{ v = fgets(buf, bufsz-1, ip);
		pclose(ip);
	}
	
	return v;
}

#endif

/* Initialize this module. The edge vectors to access the screens are set up. 
 * The operating system's terminal I/O channel is set up. 
 * The original window has "WFCHG" set, so that it will get completely
 * redrawn on the first call to "update".
 */
void Pascal vtinit(int cols, int dpthm1)

{
/*	if (pd_gflags & MD_NO_MMI)
			return; */
//  millisleep(16000);
#if MEMMAP == 0
{   char buf[40];
  	int lines = 0;
		char * v = getenv("COLUMNS");
		v = v != NULL ? v : use_cmd(buf, sizeof(buf), "tput cols");
		cols = v == NULL ? NCOL : atoi(v);
		
		if (cols > 0)
		{ term.t_ncol = cols;
			term.t_margin = cols / 10;
		}

		v = getenv("LINES");
		v = v != NULL ? v : use_cmd(buf, sizeof(buf), "tput lines");
		lines = v == NULL ? SCR_LINES : atoi(v);
		
		lines -= 2;
		if (lines < 10)
			lines = 10;
		term.t_nrowm1 = lines;
		dpthm1 = lines;
}
		tcapsetfgbg(0x70);			/* white background */ // should be derived
#endif
		free((char*)vscreen);

#define UESZ	((sizeof(VIDEO)-12+2*(cols+3))*MEMCT)	// 3 extra

{	  int	i = (dpthm1+2) * MEMCT; 
		vscreen = (VIDEO **)mallocz(i * (sizeof(VIDEO*)+UESZ));
/*	if (vscreen == NULL)
			meexit(1);*/
#if MEMMAP == 0
		pscreen = &vscreen[dpthm1+1];
#endif
{		VIDEO *vp = (VIDEO *)&vscreen[i];

		for ( ; --i >= 0; )
		{ vscreen[i] = vp;
			vp->v_color = V_BLANK;
			vp = (VIDEO *)&vp->v_text[cols+2];
		}

 		for ( ; --cols >= 0; )
 			vscreen[dpthm1]->v_text[cols] = ' ';
}}}


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


#if MEMMAP
static
const short ctrans_[] = 	/* ansi to ibm color translation table */
	{0, 4, 2, 6, 1, 5, 3, 7,
	 8, 12, 10, 14, 9, 13, 11, 15};

# define trans(x) ctrans_[x]
#else
# define trans(x) (x)
#endif

#if MEMMAP

static int palcol(int ix)

{ int clr = pd_palstr[ix] | 0x20;
	return (in_range(clr,'0','9') ? clr - '0' :
 				  in_range(clr,'a','F') ? clr - 'a' + 10 : 0) << 8;
}
#else
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
static VIDEO * USE_FAST_CALL vtmove(int row, int col, int cmt_chrom, LINE * lp)

{	const int BCD = BCCOMT + BCPRL + BCSQL + BCPAS;
#if _DEBUG
	if (row >= term.t_nrowm1)
	{
	  tcapbeep();
		row = term.t_nrowm1 - 1;
	}
#endif

{	VIDEO *vp = vscreen[row];
	short * tgt = &vp->v_text[-1];
	char * high[] = {"","","","",};
	int len = llength(lp); 		/* an upper limit */
  unsigned char * str = (unsigned char *)&lp->l_text[-1];
	unsigned char s_props = *str;												/* layout dependent */
	*str = 0;																						/* restored below */
	if ((vp->v_flag & VFML) == 0)
		memcpy(&high[0], &pd_hlight1, sizeof(char*)*4);

	if (cmt_chrom)
		cmt_chrom = (trans(pd_cmt_colour & 0xf)) << 8 | CHR_NEW;

{ // const char c_beg_cmt[16] = "`/`///`/(/`///`/";
	// const char c_sec_cmt[16] = " *#***#***#***#*";
 	Short  clring = g_clring;
	int mode = clring == 0 || cmt_chrom == 0 || (vp->v_flag & VFML)
								 		? -1 : s_props & VFCMT;
	int chrom_nxt = 0;
//int beg_cmt = c_beg_cmt[clring & 15];
//int sec_cmt = c_sec_cmt[clring & 15];
	
	if ((clring & BCD) && (s_props & VFCMT) ||
			(clring & BCFOR) && toupper(str[1]) == 'C' && len > 0)
		chrom_nxt = cmt_chrom;

//int chrom_in_peril = false;

{	short vtc = -col; // window on the horizontally scrolled screen; 0 at screen lhs
	int  markuplen = 1;
	char markupterm = 0;
	int highix[] = {0,0,0,0,};
//int chrom_on = 0;		/* 1: ul, 2: bold, -1 manual */
	char duple = curbp->b_langprops & BCPAS 				? ')' :
							 curbp->b_langprops &(BCCOMT+BCSQL) ? '/' : 0;
	int tabsize = curbp->b_tabsize;
	if (tabsize < 0)
		tabsize = - tabsize;

	while (--len >= 0)
	{	int chrom = chrom_nxt;
		chrom_nxt = CHR_0;
		if (vtc >= term.t_ncol)
		{ tgt[term.t_ncol] = (short)(chrom + '$');
			break;
		}
		
	{ int wix = 0;
	  int tgtix = 0;
	  int c = *++str;
		if (c == 0)
		{ highix[0] = 0;
			highix[1] = 0;
			highix[2] = 0;
			highix[3] = 0;
		}
		else
		{ if (c != high[0][(++highix[0])])
				highix[0] = (c == high[0][1]);
			else
			{ if (high[0][1+highix[0]] == 0 && vtc-highix[0]+2 > 0)
				{ tgtix = vtc-highix[0]+2;
				}
			}
			if (c != high[1][(++highix[1])])
				highix[1] = (c == high[1][1]);
			else
			{ if (high[1][1+highix[1]] == 0 && vtc-highix[1]+2 > 0)
				{ tgtix = vtc-highix[1]+2;
					wix = 1;
				}
			}
			if (c != high[2][(++highix[2])])
				highix[2] = (c == high[2][1]);
			else
			{ if (high[2][1+highix[2]] == 0 && vtc-highix[2]+2 > 0)
				{ tgtix = vtc-highix[2]+2;
					wix = 2;
				}
			}
			if (c != high[3][(++highix[3])])
				highix[3] = (c == high[3][1]);
			else
			{ if (high[3][1+highix[3]] == 0 && vtc-highix[3]+2 > 0)
				{ tgtix = vtc-highix[3]+2;
					wix = 3;
				}
			}
			if (tgtix > 0)
			{ tgt[tgtix] |= palcol(high[wix][0]-'1') | CHR_NEW;
				highix[wix] = 0;
				chrom_nxt = mode>0 && mode & (Q_IN_CMT+Q_IN_EOL) ? cmt_chrom : CHR_OLD;
			}
		}

		if			(c == '\t') 
		{ c = ' ';
			if (((vtc + 1 + col) % tabsize) != 0)
			{ --str;
				++len;
			}
		}
		else if (c < 0x20 || c == 0x7F)
		{ if (c <= pd_col2ch && c - pd_col1ch >= 0)
			{ 
				chrom_nxt = palcol(c - pd_col1ch); /* this is manual colouring */
				continue;
			}
			else
			{ if (++vtc > 0)
					tgt[vtc] = '^';
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
			    { len -= markuplen;
            str += markuplen;
//		      chrom_on = 1;
			      chrom_nxt = CHR_OLD;
			      mode = 0;				// &= ~(Q_IN_CMT0+Q_IN_CMT);
			      continue;
			    }
			  }
			  else
        { int wh = find_match(str, len);
          if (wh >= 0)
					{	chrom_nxt = wh ? chrom | CHR_UL : cmt_chrom;
            mode |= wh ? Q_IN_CMT : Q_IN_CMT0;
						markuplen = wh;
            len -= wh;
            str += wh;
            markupterm = c;
            continue;
          }
        }
			}
			else if (mode & (Q_IN_CMT+Q_IN_CMT0))
			{ 
			  if (str[-1] == '*' && c == duple	/* l_props cannot be '*', '"' */
				|| (c == '\'' || c == '"') && c == str[-1] && c == str[-2])
				{ mode = 0;
					chrom_nxt = CHR_OLD;
				}
			}
			else if (c == '"')
				mode ^= Q_IN_STR;
			else if ((mode & (Q_IN_STR+Q_IN_EOL)) == 0)
#if 1
				if ((clring & BCCOMT+BCSQL) && (c == '*' || c == '/') && str[-1] == '/'
				||	(clring & BCPAS)				&&	c == '*' && str[-1] == '('
				||	(clring & BCSQL)				&&  c == '-' && str[-1] == c
				||	(clring & BCPRL)				&& (c == '#' ||
																			 (c == '\'' || c == '"') && c == str[-1] && c == str[-2])
				||	(clring & BCFOR)				&&  c == '!')
#else
				if (c == sec_cmt & (str[-1] == beg_cmt || (clring & BCPRL))
				||  c == '/'  && c == str[-1] && (clring & BCCOMT)
				||	c == '\'' && str[-1] == c && str[-2] == c && (clring & BCPRL)
				||  c == '-'  && str[-1] == c && (clring & BCSQL)
				||  c == '!'  && (clring & BCFOR))
#endif
				{ mode = c != '*' ? Q_IN_EOL : Q_IN_CMT;
					if (c != '#' && (clring & BCPRL))
						mode = Q_IN_CMT;
					chrom = cmt_chrom;
//				if (((clring & (BCFOR | BCPRL)) == 0) && vtc-1 >= 0)
						tgt[vtc] |= chrom;
				}
		}

//	if (chrom_in_peril && chrom_on > 0)
//	{ chrom_in_peril = false;
//		if (vtc > 1)
//			tgt[vtc-1] |= CHR_OLD;
//		chrom_on = 0;
//	}
//	if (chrom_on > 0)
//		chrom_in_peril = true;
		if (++vtc > 0)
			tgt[vtc] = c | chrom;
	}}

{ int ct = term.t_ncol - vtc;

	if (vtc > 1 && 0 /* && chrom_on != 0 */)
	{
#if MEMMAP
		if ((tgt[vtc] & 0xff00) && ct > 0)
#else
		if (ct >= 0)
#endif
			tgt[++vtc] = ' ';
		tgt[vtc] |= CHR_OLD;
	}

//--vtc;
	while (--ct >= 0)
	{ if (++vtc >= 0)
			tgt[vtc] = ' ';
	}

	lp->l_props = s_props;
	return vp;
}}}}}


/************************** Modeline stuff *************************/

#if MEMMAP == 0

LINE *  g_lastlp;								// NULL => reset
static int     g_lastfcol;

#endif

/* Redisplay the mode line for the window pointed to by the "wp".
 * This is the only routine that knows how the modeline is formatted.
 * Called by "update" any time there is a dirty window.
 */
void Pascal modeline(WINDOW * wp)
	
{
#if MEMMAP == 0
	if (wp->w_dotp == g_lastlp /* and wp->w_next == NULL */)
	{ if (wp->w_fcol == g_lastfcol && (wp->w_flag & WFMODE == 0))
			return;
	}

	g_lastlp = wp->w_dotp;
#endif

{	int row;
	int n; 			      	/* cursor position count */
	union 
	{ LINE lc;
	  char c[NLINE+24]; /* buffer for mode line */
	}   tline;


	tline.lc.l_used = term.t_ncol;

#if S_MSDOS
	n = wp == curwp ? 0x5f : /* 0xcdm 173 :  */
#else
	n = wp == curwp ? '='  :
#endif
										'-';
	memset(&tline.lc.l_text[0], n, NLINE);

	row = wp->w_toprow+wp->w_ntrows;		/* Location. */

#if _DEBUG
	if ((unsigned)row >= (unsigned)term.t_nrowm1)
	{ row = term.t_nrowm1 - 1;
		mbwrite("toprow+ntrows oor");
	}
#endif
	vscreen[row]->v_flag &= ~VFCMT;
#if MEMMAP || OWN_MODECOLOUR
	vscreen[row]->v_color = 0x25; 		/* magenta on green */
	vscreen[row]->v_flag |= VFCHG | VFCOL | VFML;/* Redraw next time*/
#else
/*vscreen[row]->v_color = 0x7;			 * black on white */
	vscreen[row]->v_flag |= VFCHG | VFREQ | VFCOL | VFML;/* Redraw next time*/
#endif

	wp->w_flag &= ~WFMODE;
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
#if S_MSDOS == 0
	g_lastfcol = wp->w_fcol;
#endif
	if (wp->w_fcol <= 0)
		strcpy(&tline.lc.l_text[10], " (");
	else
		concat(&tline.lc.l_text[10], "[", int_asc(wp->w_fcol), "](", null);
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

{	int numbuf = lastbuffer(0,0);
	if (numbuf > 9)
		tline.lc.l_text[tline.lc.l_used-3] = '0' + numbuf / 10;
	tline.lc.l_text[tline.lc.l_used-2] = '0' + numbuf % 10;
	
//tline.lc.l_text[tline.lc.l_used] = 0;
	
	(void)vtmove(row, 0, 0, &tline.lc); 	/* Seek to correct line. */
#if 0
	vtputs((wp->w_flag&WFCOLR) != 0  ? "C" : "");
	vtputs((wp->w_flag&WFMODE) != 0  ? "M" : "");
	vtputs((wp->w_flag&WFHARD) != 0  ? "H" : "");
	vtputs((wp->w_flag&WFEDIT) != 0  ? "E" : "");
	vtputs((wp->w_flag&WFFORCE) != 0 ? "F" : "");
#endif
}}}}}}

/*	updall: update all the lines in a window on the virtual screen */

void Pascal USE_FAST_CALL updall(int wh, WINDOW * wp)
																/* wh : < 0 => de-extend logic
																				= 0 => only w_dotp
																				> 0 =? all */
{ LINE *lp = wp->w_linep;
	int	sline = wp->w_toprow - 1;
	int	zline = sline + wp->w_ntrows;
			
	int color = window_bgfg(wp);
//int cmt_clr = (trans(pd_cmt_colour & 0xf)) << 8 | CHR_NEW;

#if MEMMAP == 0
	if (color == 0x70)
		color = V_BLANK;
#endif
#if _DEBUG
	if (zline >= term.t_nrowm1)
	{ /* addb(sline); */
		zline = term.t_nrowm1-1;
	}
#endif
	
	while (++sline <= zline)
	{ if      (wh < 0)
		{ if (vscreen[sline]->v_flag & VFEXT)
			{ VIDEO * vp = vtmove(sline, wp->w_fcol, 1, lp);
																		/* this line no longer is extended */
				if (lp != wp->w_dotp)
				{	vp->v_flag -= VFEXT;
					vp->v_flag |= VFCHG;
				}
			}
		}	
		else if (wh || lp == wp->w_dotp)	/* and update the virtual line */
		{
			VIDEO * vp = vtmove(sline, wp->w_fcol, 1, lp);

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
	{ updall(1, wp);
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
# define pscroll(dir, stt, len)
#else

void Pascal pteeol(int row)

{ 	register short * tgt = &pscreen[row]->v_text[0];
		register short	 sz = term.t_ncol;

		while (--sz >= 0)
			*tgt++ = ' ';
}


void Pascal ptclear()

{	Int row;
	for (row = term.t_nrowm1+1; --row >= 0; )
		pteeol(row);
}



static void Pascal pscroll(int dir, int stt, int lenm1)
		/*	dir;			** < 0 => window down */
		/*	stt;			** start target location */
		/*	lenm1;									*/
{ int n = 1;
	if (dir < 0)
		n = 0;

{	int src = stt+1-n;
	int tgt = stt+n;

  VIDEO *	vp = pscreen[stt+n*lenm1];
	memmove(&pscreen[tgt], &pscreen[src], lenm1 * sizeof(VIDEO*));
	pscreen[stt+(n ^ 1) * lenm1] = vp;

	n = term.t_ncol;
	while (--n >= 0)
		vp->v_text[n] = ' ';
}}

#endif


static void Pascal scrollupdn(int set, WINDOW * wp)/* UP == window UP text DOWN */
	 
{ int n = 1;
	int lenm1 = wp->w_ntrows-1;

	if (set & WFTXTD)
	{ 
#if MEMMAP == 0
		ttrow -= 2;								/* a trick */
#endif
		n = 0;
	}

{	int stt = wp->w_toprow;
	int src = stt+1-n;
	int tgt = stt+n;

	VIDEO * vp = vscreen[stt+n*lenm1];
	vp->v_flag = VFCHG;
	memmove(&vscreen[tgt], &vscreen[src], lenm1 * sizeof(VIDEO*));
	vscreen[stt + (n ^ 1) * lenm1] = vp;

#if MEMMAP == 0
	n = n * 2 - 1;
	tcapmove(ttrow - n + 1, ttcol);
	tcapscreg(stt, stt+lenm1);
	ttscupdn(n);
	pscroll(n, stt, lenm1);
#endif
	updall(0, wp);
/* updateline(n < 0 ? wp->w_toprow : wp->w_toprow+lenm1);*/
}}
	 


int Pascal window_bgfg(WINDOW * wp)

{ BUFFER * bp = wp->w_bufp;
  int clr = bp == NULL ? g_colours : bp->b_color;
  
	return (trans((clr >> 4) & 0xf)<< 4) | trans(clr & 0xf);

//return (trans((wp->w_color >>8) & 0x7)<< 4) | trans(wp->w_color & 7);
}


static
void Pascal updline()
/*	updpos: update the position of the hardware cursor and handle extended
		lines. This is the only update for simple moves.
*/
{	WINDOW * wp = curwp;
 static LINE * g_up_lp = NULL;
				int flag = 
#if MEMMAP
				   g_up_lp != wp->w_dotp ? WFHARD /*| WFTXTU|WFTXTD*/ | WFMODE | WFMOVE :
#endif
										0;
	g_up_lp = wp->w_dotp;
{	int row = wp->w_toprow;
 	LINE * lp;

	for (lp = wp->w_linep; 
			 lp != g_up_lp && row < term.t_nrowm1 - 1;
			 lp = lforw(lp))
		++row;

{	int fcol = wp->w_fcol; // < pd_fcol ? pd_fcol : wp->w_fcol;
	int col = getccol() - fcol;
	int diff = (col + 1 - pd_hjump);
				
	if (diff < 0 && pd_hjump > 0)
	{	diff = (diff / pd_hjump) * pd_hjump;						// always -ve
		fcol += diff;
//	if (fcol < pd_fcol)
//		fcol = pd_fcol;
		col -= diff;
		flag = WFHARD | WFMODE | WFMOVE;
	}
	
	diff = pd_fcol - fcol;
	if (diff > 0)
	{ fcol += diff;
		col -= diff;
		if (col < 0)
			col = 0;
	}

//g_currow = row;

{	// int cmt_clr = (trans(pd_cmt_colour & 0xf)) << 8 | CHR_NEW;
#if MOUSE == 0
	int g_lbound = 0;		/* leftmost column of line being displayed */
#endif							
	int rhs = col - term.t_ncol + 1;
  if (rhs >= 0)
	{	if (pd_hjump)				/* if horizontal scrolling is enabled, shift if needed */
		{ int mv = rhs + pd_hjump;
			fcol += mv;
			col -= mv;
			rhs -= mv;
			flag = WFHARD | WFMODE | WFMOVE;
		}
		else								// Not used in Windows
						 /*  updext: update the extended line which the cursor is currently
								 on at a column greater than the terminal width. The line
								 will be scrolled right or left to let the user see where
								 the cursor is. Called only in non Hscroll mode.
							*/
		{ 				/* calculate what column the real cursor will end up in */
			g_lbound = col - ((rhs - 1) % term.t_scrsiz) - term.t_margin + 1;
			col -= g_lbound;

											/* scan through the line copying to the virtual screen*/
											/* once we reach the left edge						*/
											/* start scanning offscreen */
		{ VIDEO * vp = vtmove(row, g_lbound + fcol, 1, g_up_lp);
			
//		vp->v_text[0] = /*BG(C_BLUE)+FG(C_WHITE)+*/'$';
								 			/* and put a '$' in column 1 */
			vp->v_flag |= (VFEXT | VFCHG);
		}}
	}

	wp->w_fcol = fcol;
	wp->w_flag |= flag;
	if (rhs < 0)			// allow deextension on this line
		wp->w_dotp = NULL;

					 /* update the current window if we have to move it around */
{	int hard = wp->w_flag & WFHARD;
	if (hard)
		updall(1, wp);

//g_curcol = col;
																/*	upddex: de-extend any line that deserves it */
{	int i;
  WINDOW * p;
	for (p = wheadp; p != NULL; p = p->w_next)
	{	updall(-1, p);
		if (p == wp)
			p->w_dotp = g_up_lp;
		if (p->w_flag & WFMODE)
			modeline(p);
		p->w_flag = 0; 		/* we are done */
	}

/*	updupd: update the physical screen from the virtual screen	*/

#if MEMMAP == 0
	if (pd_sgarbf)
	{  
		 tcapepage();			 /* Erase-whole page, also clears the message area. */
		 ptclear();
		 pd_got_msg = FALSE;		 
	}
#endif

	for (i = -1; ++i < term.t_nrowm1; )
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
							  /* UPDATELINE specific code for IBM-PC and other compatables */
			vp->v_flag &= ~VFCHG;
			scwrite(i, vp->v_text, vp->v_color);
#else
			updateline(i);
#endif
		}
	}
#if MEMMAP == 0
	if (pd_sgarbf)
		mlwrite(g_cmd_line);
#endif
	pd_sgarbf = FALSE;
	tcapmove(row, col);
	TTflush();	/* there may be cause to reverse move and flush */
}}}}}}

											/* reframe:	check to see if the cursor is on in the window
												and re-frame it if needed or wanted 	*/
static 
int Pascal reframe(WINDOW * wp)

{	int nlines = wp->w_ntrows;
	int centre = wp->w_force;
  int flags = wp->w_flag;
	LINE * top = wp->w_linep;
	LINE *lp = top;			 /* search pointer */
												/* if not a requested reframe, check for a needed one */
	if ((flags & WFFORCE) == 0)
	{ int i;
		for (i = nlines; --i >= 0; ) 
		{												 		/* if the line is in the window, no reframe */
			if (lp == wp->w_dotp)
				goto nop_here;
																/* if we are at the end of the file, reframe */
			if (lp->l_props & L_IS_HD)
				break;
				
			lp = lforw(lp);						/* on to the next line */
		}
		if	((flags & WFHARD) == 0)
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
	{ centre += nlines + 1;
	} 
	else
		centre = nlines >> 1;
																			/* backup to new line at top of window */
	for (lp = wp->w_dotp; 
			 ((lp = lback(lp))->l_props & L_IS_HD) == 0 && --centre > 0; )
		;

						 /* and reset the current line at top of window */
	lp = lforw(lp);
	flags |= WFHARD;
	pd_sgarbf = 1;
ret:
	wp->w_linep = lp;
nop_here:
	wp->w_force = 0;

	return flags;
}


/* Make sure that the display is right. This is a three part process.
 * First scan through all of the windows looking for dirty ones. 
 * Check the framing and refresh the screen. 
 * Second ensure that "g_currow" and "g_curcol" are correct for the current window.
 * Third make the virtual and physical screens the same.
 */
int /*Pascal*/ update(int force)
	/* int force; ** force update past type ahead? */
{
	WINDOW *wp;

	if (TRUE

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
	{ 															/* update any windows that need refreshing */
		for (wp = wheadp; wp != NULL; wp = wp->w_next)
			if (wp->w_flag) 			 				/* if the window has changed, service it */
			{ int set = reframe(wp) & ~( WFMOVE+WFMODE);
														 				/* check the framing */
				if			(set & (WFTXTU+WFTXTD))
					scrollupdn(set, wp);	
				else if (set & WFHARD)
					updall(1, wp);			/* update all lines */
				else if (set)
					updall(0, wp);			/* update EDITed line */
			}
								/* update physical screen from virtual screen */
								/* update the cursor and flush the buffers */
		updline();
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
	VIDEO *ph = pscreen[row];
	short *ph1 = &ph->v_text[0];

	struct VIDEO *vp1 = vscreen[row]; /* virtual screen image */

	short *cp1 = &vp1->v_text[0];
	short *cp9 = &cp1[term.t_ncol];
	int revreq = vp1->v_flag & VFREQ;		/* reverse video flags */
	int caution = (((revreq ^ ph->v_flag) & VFREQ) ||
										vp1->v_color != pscreen[row]->v_color) && !pd_sgarbf;

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
	{ while (cp1 < cp9 && cp1[0] == ph1[0])
		{ if (*cp1 & 0xff00)
			{ int wh = (*cp1) >> 8;
				if      (wh & 0x40)
					prechrom = prechrom & 0xf0 | (wh & 0xf);
				else if (wh & 0x80)
					prechrom = vp1->v_color;
				else if (wh & 0x10)
					prechrom |= 0x8;																
				else if (wh & 0x20)
					prechrom |= 0x8;				// cannot do bold
				else
					prechrom = wh & 7;
			}
			++cp1;
			++ph1;
		}

						 /* Only called on changed lines but the following can still occur. 
							* A hard update is always done when a line splits, a massive 
							* change is done, or a buffer is displayed twice. */
		if (cp1 >= cp9)
			return;
	}

	tcapmove(row, cp1 - &vp1->v_text[0]);

{	short prechrom_ = prechrom;
	if (prechrom_ != 0)
		tcapchrom(prechrom_ | bg);

#if REVSTA
	if (revreq)
		tcaprev(TRUE);
#endif
															
{ int same_ct = 0;
  short *phz = &ph->v_text[term.t_ncol];
  short *ph9 = phz;
														/* terminals cannot do this */
#if 1
	while (cp9 > cp1)
	{	if (*--cp9 != ' ' || *--ph9 != ' ')	/* find out if theres a match on the right */
			break;
	}
	++cp9;
	++ph9;
#endif
 	--cp1;
	while (++cp1 <= cp9 /* cpend */)		/* Ordinary. */
	{	*ph1++ = *cp1;
		if (*cp1 & 0xff00)
		{ int wh = (*cp1) >> 8;
			do
			{	if      (wh & 0x40)
					prechrom = prechrom & 0xf0 | wh & 0xf;
				else if (wh & 0x80)
					prechrom = vp1->v_color;
				else if (wh & 0x10)
					prechrom |= 0x8;			// bold
				else if (wh & 0x20)
					prechrom |= 0x8;			// cannot do bold
				else
					prechrom = wh & 7;
				tcapchrom(prechrom | bg );
			}
			while (0);		// only one of at present
		}

		ttputc(*cp1);
		++ttcol;
	}

#if REVSTA
	if (revreq != 0)
		tcaprev(FALSE);
	else
#endif

	if (prechrom != 0)
		tcapchrom(CHROM_OFF);

	if ( same_ct == 0 &&
      (vp1->v_flag & VFML) == 0)
	{ extern short scbot;

		while (++ph1 < phz /* cpend */)		/* Ordinary. */
			*ph1++ = *++cp1;
	//millisleep(300);
		tcapeeol();
		if (row < scbot)					/* CRLF is required so that highlight- 		 */
		{	//millisleep(300);			/* copy can recognise line ends. 					 */
		  ttputc(13);							/* It cannot be done ahead of the modeline */
		  ttputc(10);
		  ++ttrow;
		  ttcol = 0;
 		}
	}
}}}}

#endif

/* ################## end of window based routines #################### */

void Pascal upmode()	/* update all the mode lines */

{ orwindmode(WFMODE);
}


#if MEMMAP
void Pascal upwind_()	/* force hard updates on all windows */
#else
void Pascal upwind(int garbage)	/* force hard updates on all windows */
#endif
{ 
#if MEMMAP == 0
	pd_sgarbf |= garbage;
#endif
	orwindmode(WFMODE | WFHARD); //  | WFMOVE);
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
		ttcol -= 1;
	}
	else
	{ if (ttcol + 1 >= term.t_ncol)
			return;
		lastmesg[ttcol] = c;
		lastmesg[++ttcol] = 0;
	}
	if (pd_discmd > 0)
	{ int notused = pd_discmd;
		ttputc(c);
	}
}


		/* Write out a string. Update the physical cursor position.
		 * This assumes that the characters in the string all have width "1";
		 * if this is not the case things will get screwed up a little.
		 */ 
void Pascal mlputs(int wid, const char * s)

{	if (s != NULL)
	{	char ch;
		while ((ch = *s++) != 0)
		{ mlout(ch); 
			if (--wid == 0)
				break;
		} 
	}
		
	while (--wid >= 0)
		mlout(' ');
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

{	int  ch;
	int  s_discmd = pd_discmd++;
	Bool popup = false;
//--g_cursor_on;

	if (*fmt != '\001')
	{ --fmt;
#if COLOR && 0
		tcapbfcol(V_BLANK); /* set up the proper colors for the command line */
#endif
		tcapmove(255, 0);
		tcapeeol();
	}

	while ((ch = *++fmt) != 0)
	{ if (ch != fmtch)
			mlout((char)ch);
		else 
		{	int width = 0;
			while (((unsigned)(ch = *++fmt - '0')) <= 9)
				width = width * 10 + ch;
		{ int radix = 10;
			char * sp = NULL;
//		int v = get_arg(int,ap);
			switch (ch+'0')
			{  case '>':  pd_discmd = -1;
				 when 'b':	(void)tcapbeep();
#if MEMMAP == 0
				 when 'w':  TTflush();
									  millisleep(4800);
#endif
				 when 'o':  radix = 8 - 6;
				 case 'x':  radix += 6;
				 case 'd':  
				 case 'f':  sp = int_radix_asc(get_arg(int,ap), radix);
				 				  	if (ch == 'f' - '0')
										{ int sl = strlen(sp);
										  sp[sl] = sp[sl-1];
										  sp[sl-1] = sp[sl-2];
										  sp[sl-2] = '.';
										}
				 when 'p':  popup = true;
				 when 's':  sp = get_arg(char *, ap);
				 when 'c':  mlout((char)get_arg(int,ap));
//			 otherwise	mlout((char)(ch+'0'));
			}
			if (sp != NULL)
				mlputs(width, sp);
		}}
	}
	TTflush();

	pd_got_msg = TRUE;
	if (pd_discmd < 0)
	{ (void)linstr(lastmesg);
		loglog1("mlw %s", fmtstr);
	}
#if MEMMAP
	else
	{ int scol = ttcol;
		mlputs(term.t_ncol-ttcol,NULL);
		tcapmove(ttrow, scol);
//	memset(&vscreen[term.t_nrowm1]->v_text[ttcol], 0, (term.t_ncol-ttcol)*2);
	}
#else
	strpcpy(g_cmd_line, lastmesg, sizeof(lastmesg));
#endif

	loglog1("mlw %s", lastmesg);

	if (popup)
#if MEMMAP
		mbwrite(lastmesg);
#else
		(void)ttgetc();
#endif

	pd_discmd = s_discmd;
//++g_cursor_on;
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
#if MEMMAP == 0
	g_cmd_line[0] = 0;
#endif
	pd_got_msg = FALSE;
}


