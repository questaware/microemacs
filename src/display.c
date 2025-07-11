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
#define MEMMAP	S_MSDOS
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

#define MARGIN	8 		  /* size of minimim margin and */
#define SCRSIZE	64			/* scroll size for extended lines */

#if MEMMAP
#define T_MARGIN 0
#define T_SCRSIZE 0
#else
#define T_MARGIN term.t_margin
#define T_SCRSIZE term.t_scrsiz
#endif

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
	SCRSIZE,
#endif
};

#define CHROM_OFF 0x8000

typedef struct	VIDEO
{ unsigned char v_color;
	unsigned char v_flag; 	/* Flags, must never be '/' */
	short				v_text[6];		/* Screen data. */
} VIDEO;

#define VFCHG 0x0001	/* Changed flag 		*/
#define VFCMT 0x0002	/* starts in comment*/
#define VFEXT 0x0004	/* extended (beyond column 80)	*/ 
#define VFREV 0x0008	/* reverse video status 	*//* contig these 2*/
#define VFREQ 0x0010	/* reverse video request	*/
#define VFCOL 0x0020	/* color change requested OBSOLETE */
#define VFML	0x0040	/* is a modeline		*/


#if S_MSDOS
#define V_BLANK 0x7
#else
#define V_BLANK 0
#endif

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

extern const char attrnames [][8];

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
	{	int wh = (scl[icol] & 0xffff) >> 8;
    if (wh & 0xf0)
    { if      (wh & 0x40)
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
			if (wh & 2)
			  attr |= FOREGROUND_INTENSITY;
      if (wh & 1)
				attr |= COMMON_LVB_UNDERSCORE;
#endif
    }
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
		cols = (v == NULL ? NCOL : atoi(v))-1;
		
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

//	for ( ; --cols >= 0; )
//		vscreen[dpthm1]->v_text[cols] = ' ';
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

#if Q_IN_CMT0 != 1
error error
#endif

// -1: not found, 0: single, 1: double

static
int USE_FAST_CALL find_match(const char * src, int len)

{ char ch = src[0];
  int dbl = len > 1 && src[1] == ch;
  src += dbl;
  while (--len >= 0)
  { if (*++src == ch)
    { if (!dbl || (len > 0 && src[1] == ch))
        return dbl;
    }
  }
  
  return len;
}


#if MEMMAP
# define trans(x) (x)
#else
static
const unsigned char ctrans_[] = 	/* ansi to ibm color translation table */
	{0,  4,  2,  6, 1,  5,  3, 7,
	 8, 12, 10, 14, 9, 13, 11, 15};
# define trans(x) ctrans_[x]
#endif

#if MEMMAP

#define palcol(c) (((c) + 1) << 8)
#else

static int palcol(int ix)

{ int clr = pd_palstr[(ix & 7)*2+1] | 0x20;
	return (in_range(clr,'0','9') ? clr - '0' :
 				  in_range(clr,'a','f') ? clr - 'a' + 10 : 0) << 8;
}
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

//unsigned char s_flags = *str;												/* layout dependent */
//*str = 0;																						/* restored below */

{ // const char c_beg_cmt[16] = "`/`///`/(/`///`/";
	// const char c_sec_cmt[16] = " *#***#***#***#*";
 	Short  clring = g_clring;
	int mode = clring == 0 ? 0 : cmt_chrom * (lp->l_dcr & VFCMT);
	cmt_chrom *= (trans(pd_cmt_colour & 0xf)) << 8 | CHR_NEW;

//int beg_cmt = c_beg_cmt[clring & 15];
//int sec_cmt = c_sec_cmt[clring & 15];
	
{	int triad = clring & BCFOR ? -'C' : 0;
  int chrom_nxt = CHR_0;
	if ((clring & BCD) && mode)
		chrom_nxt = cmt_chrom;

//int chrom_in_peril = false;

{	short vtc = -col; // window on the horizontally scrolled screen; 0 at screen lhs
  int atbeg = 1;
	int markuplen = 1;
	char markupterm = 0;
	int hix[30] = {0};
	const char** high = ((const char**)&pd_hlight1);
//int chrom_on = 0;		/* 1: ul, 2: bold, -1 manual */
  int langprops = curbp->b_langprops;
  char duple = langprops & BCPAS        ? ')' :
               langprops &(BCCOMT+BCSQL) ? '/' : 0;
	int tabsize = curbp->b_tabsize;
	if (tabsize < 0)
		tabsize = - tabsize;

//memset(hix, 0, sizeof(hix));

{	VIDEO *vp = vscreen[row];
	short * tgt = &vp->v_text[-1];
	int is_ml = ((char*)tgt)[1] & VFML;
	int len = llength(lp); 		/* an upper limit */
  unsigned char * str = (unsigned char *)&lp->l_text[-1];

	while (--len >= 0)
	{	int chrom = chrom_nxt;
		if (vtc >= 0)
			chrom_nxt = CHR_0;
	{ int c = *++str;
		if (c == 0)
			;
		else if (is_ml)
			goto nop;
		else
		{	if	(c == '\t') 
			{ c = ' ';
				if (((vtc + 1 + col) % tabsize) != 0)
				{ --str;
					++len;
				}
			}
		}
		if      (c < 0x20 || c == 0x7F)
		{ if (c <= pd_col2ch && c - pd_col1ch >= 0)
			{ 
				chrom_nxt = palcol(c - pd_col1ch) | CHR_NEW; 	/* this is manual colouring */
				continue;
			}
			else
			{ if (++vtc > 0)
					tgt[vtc] = '^';
				c ^= 0x40;
			}
		}
		else if (c < '0')
		{ triad = (c == '\'' || c == '"') && c == str[-1] && c == str[-2];
			if			(mode & (Q_IN_EOL+Q_IS_NEG))
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
						markuplen = wh;
            len -= wh;
            str += wh;
            mode |= wh+1;
            markupterm = c;
            continue;
          }
        }
			}
			else if (mode & (Q_IN_CMT+Q_IN_CMT0))
			{ 
			  if (str[-1] == '*' && c == duple	/* l_props cannot be '*', '"' */
				 || triad)
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
				||	(clring & BCPRL)				&& (c == '#' || triad)
				||	(clring & BCFOR)				&& (c == '!'))
#else
				if (c == sec_cmt & (str[-1] == beg_cmt || (clring & BCPRL))
				||  c == '/'  && c == str[-1] && (clring & BCCOMT)
				||	c == '\'' && str[-1] == c && str[-2] == c && (clring & BCPRL)
				||  c == '-'  && str[-1] == c && (clring & BCSQL)
				||  c == '!'  && (clring & BCFOR))
#endif
				{ 
eolcmt:
					mode = c != '*' && !triad ? Q_IN_EOL : Q_IN_CMT;
					chrom = cmt_chrom;
					chrom_nxt = chrom;
					if (((clring & (BCFOR | BCPRL)) == 0) && vtc-1 >= 0)
						tgt[vtc] |= chrom_nxt;
				}
		}
		if (atbeg > 0 && toupper(c)== -triad)
		{	atbeg = 0;
			goto eolcmt;
		}

	{	int foll = str[1] - ' ';
		int bef = str[-1] <= ' ' || atbeg > 0;
		do 
	  {	int ch = atbeg > 0 ? ' ' : langprops & BCFOR ? tolower(c) : c;
			int wix;
			for (wix = (mode & Q_IN_CMT) ? 1 : sizeof(hix)/sizeof(hix[0]); --wix >= 0; )
			{ // if (wix > 0 && hix[wix] == 0 && str[-1] > ' ')
				//	continue;
			  if (ch == high[wix][1+hix[wix]])
				{ hix[wix] += 1;
				  if (high[wix][1+hix[wix]] == 0)
						break;
					if (high[wix][1+hix[wix]] == ' ' && 
					    high[wix][2+hix[wix]] == 0   && foll <= 0)
						break;					
				}
				else
					hix[wix] = (ch == high[wix][1]) && (ch == ' ' || wix == 0 || bef);
			}

			if (wix >= 0
			 && (wix == 0 || (mode & (Q_IN_CMT+Q_IN_EOL+Q_IN_STR)) == 0)) // 1st or ! in cmt
			{ int tgtix = vtc-hix[wix]+2;
				if (tgtix <= 0)
					tgtix = 1;
			  tgt[tgtix] |= palcol(high[wix][0]-'1') | CHR_NEW;
//			memset(hix, 0, sizeof(hix));
				hix[wix] = 0;
				chrom_nxt = CHR_OLD;
			}
		} while (--atbeg >= 0);

//	if (chrom_in_peril && chrom_on > 0)
//	{ chrom_in_peril = false;
//		if (vtc > 1)
//			tgt[vtc-1] |= CHR_OLD;
//		chrom_on = 0;
//	}
//	if (chrom_on > 0)
//		chrom_in_peril = true;
nop:
		if (++vtc > 0)
			tgt[vtc] = c | chrom;

		if (term.t_ncol - vtc <= 0 && len > 0)
		{
#if MEMMAP == 0
//	  tgt[vtc] = (short)(chrom + '$');
		  tgt[vtc] |= 0x100 | CHR_NEW;
#else
			tgt[vtc] |= 0xf000 | CHR_NEW; // palcol(3)
#endif
			break;
		}
	}}}

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

//lp->l_text[-1] = s_flags;
	return vp;
}}}}}}


/************************** Modeline stuff *************************/

#if MEMMAP == 0

LINE *  g_lastlp;								// NULL => reset
static int     g_lastfcol;

#endif

static int set_field(char * tgt, const char * src, int maxlen)

{ maxlen = -maxlen;
{ int fnlen = strlen(src);
  if (fnlen > maxlen)
		fnlen = maxlen;
	
	((char*)memcpy(tgt, src, fnlen))[fnlen] = ' ';
	return fnlen;
}}

/* Redisplay the mode line for the window pointed to by the "wp".
 * This is the only routine that knows how the modeline is formatted.
 * Called by "update" any time there is a dirty window.
 */
static 
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

	wp->w_flag &= ~WFMODE;
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

#if S_MSDOS
	n = wp == curwp ? 0x5f : /* 0xcdm 173 :  */
#else
	n = wp == curwp ? '='  :
#endif
										'-';
	memset(&tline.lc.l_text[0], n, NLINE);

	if (n != '-')
	{ char * s = int_asc((int)wp->w_line_no);
																			/* take up some spaces */
		strcpy(&tline.lc.l_text[4], &s[strlen(s)-6]);
	} 												/* are we horizontally scrolled? */

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

#if S_MSDOS == 0
	g_lastfcol = wp->w_fcol;
#endif
	if (wp->w_fcol <= 0)
		strcpy(&tline.lc.l_text[10], " (");
	else
		concat(&tline.lc.l_text[10], "[", int_asc(wp->w_fcol), "](", null);
					/* display the modes */
	
{ int cpix;
  int	modeset = wp->w_bufp->b_flag >> (NUMFLAGS-1);
	for (i = -1; ++i < NUMMODES - 2; ) /* add in mode flags */
	{ if ((modeset = modeset >> 1) & 1) 
			strcat(&tline.lc.l_text[10], attrnames[i]);
	}
	cpix = strlen(tline.lc.l_text);
	if (tline.lc.l_text[cpix-1] == ' ')
		--cpix;
	tline.lc.l_text[cpix] = ')'; 
	tline.lc.l_text[cpix+3] = ' ';
  tline.lc.l_text[cpix+1] = n;
//cpix += 4;

{ const char * fname = bp->b_fname;
	if (1 || fname != NULL)	/* File name. */
	{
		int fnlen = set_field(&tline.lc.l_text[cpix+4], fname, cpix - NLINE - 8);
		cpix += ++fnlen;

		while (--fnlen >= 0 && fname[fnlen] != '/')
			;
						
		if (strcmp(fname+fnlen+1,bp->b_bname)== 0)
			goto no_bn;
	}

	tline.lc.l_text[cpix+4] = '@';
	(void)set_field(&tline.lc.l_text[cpix+5], bp->b_bname, cpix - NLINE - 5);

no_bn:
{	int numbuf = lastbuffer(0,0) & 0xff;
	if (numbuf > 9)
		tline.lc.l_text[term.t_ncol-3] = '0' + numbuf / 10;
	
	tline.lc.l_text[term.t_ncol-2] = '0' + numbuf % 10;
	tline.lc.l_dcr = term.t_ncol << (SPARE_SZ+2);

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

static
void Pascal USE_FAST_CALL updall(WINDOW * wp, int wh)
																/* wh : < 0 => de-extend logic
																				= 0 => only w_dotp
																				> 0 =? all */
{	int color = window_bgfg(wp);
	LINE *lp = wp->w_linep;
	int	row = wp->w_toprow - 1;
	int	zrow = row + wp->w_ntrows;
			
//int cmt_clr = (trans(pd_cmt_colour & 0xf)) << 8 | CHR_NEW;

#if MEMMAP == 0
	if (color == 0x70)
		color = V_BLANK;
#endif
#if _DEBUG
	if (zrow >= term.t_nrowm1)
	{ /* addb(row); */
		zrow = term.t_nrowm1-1;
	}
#endif
	
	while (++row <= zrow)
	{	
#if MEMMAP == 0
		if      (wh < 0)
		{ if (vscreen[row]->v_flag & VFEXT)
			{ VIDEO * vp = vtmove(row, wp->w_fcol, 1, lp);
																		/* this line no longer is extended */
				if (lp != wp->w_dotp)
				{	vp->v_flag -= VFEXT;
					vp->v_flag |= VFCHG;
				}
			}
		}	
		else
#endif
		     if (wh || lp == wp->w_dotp)	/* and update the virtual line */
		{
			VIDEO * vp = vtmove(row, wp->w_fcol, 1, lp);

			vp->v_color = color;
			vp->v_flag &= ~(VFREQ | VFML);
			vp->v_flag |= VFCHG | (lp->l_dcr & Q_IN_CMT);
		}
		if (!l_is_hd(lp))/* if we are not at the end */
			lp = lforw(lp);
	}
}


#if 0

void Pascal updallwnd(int reload)

{ WINDOW * wp;

	for (wp = wheadp; reload && wp != NULL; wp = wp->w_next)
	{ updall(wp, 1);
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



static void pscroll(int dir, int stt, int lenm1)
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


static void scrollupdn(WINDOW * wp, int set)/* UP == window UP text DOWN */
	 
{ int n = set & WFTXTU;

#if MEMMAP == 0
	ttrow -= 2 * n;								/* a trick */
#endif

{	int lenm1 = wp->w_ntrows-1;
	int ct = lenm1;
	int stt = wp->w_toprow;
	int tgt = stt + n * lenm1;

	VIDEO * vp = vscreen[tgt];

	int dec = n * 2 - 1;

	while (--ct >= 0)
	{	tgt -= dec;
		vscreen[tgt + dec] = vscreen[tgt];
	} 

	vp->v_flag = VFCHG;
	vscreen[tgt] = vp;

#if MEMMAP == 0
	n = n * 2 - 1;
	tcapmove(ttrow - n + 1, ttcol);
	tcapscreg(stt, stt+lenm1);
	ttscupdn(n);
	pscroll(n, stt, lenm1);
#endif
	updall(wp, 0);
/* updateline(n < 0 ? wp->w_toprow : wp->w_toprow+lenm1);*/
}}
	 


int Pascal window_bgfg(WINDOW * wp)

{ BUFFER * bp = wp->w_bufp;
  int clr = bp == NULL ? g_bat_b_color : bp->b_color;
  
	return (trans((clr >> 4) & 0xf)<< 4) | trans(clr & 0xf);

//return (trans((wp->w_color >>8) & 0x7)<< 4) | trans(wp->w_color & 7);
}


static
void Pascal updline()
/*	updpos: update the position of the hardware cursor and handle extended
		lines. This is the only update for simple moves.
*/
{	WINDOW * wp = curwp;
	LINE * dotp = wp->w_dotp;
 static LINE * g_up_lp = NULL;
	int flag = WFHARD | WFMODE | WFMOVE; /*| WFTXTU|WFTXTD*/
{	int fcol = wp->w_fcol; // < pd_fcol ? pd_fcol : wp->w_fcol;
	int row = wp->w_toprow;
 	LINE * lp;

	for (lp = wp->w_linep; 
			 lp != dotp && row + 1 < term.t_nrowm1;
			 lp = lforw(lp))
		row = row + 1;

{	int col = getccol() - fcol;
	int diff = (col + 1 - pd_hjump);
				
	if (diff < 0 && pd_hjump > 0)
	{	diff = (diff / pd_hjump) * pd_hjump;						// always -ve
		fcol += diff;
//	if (fcol < pd_fcol)
//		fcol = pd_fcol;
		col -= diff;
	}
	else if (g_up_lp == dotp)
		flag = 0;
	
	g_up_lp = dotp;

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
	{	flag = WFHARD | WFMODE | WFMOVE;
	{	if (pd_hjump)				/* if horizontal scrolling is enabled, shift if needed */
		{ int mv = rhs + pd_hjump;
			fcol += mv;
			col -= mv;
			rhs -= mv;
		}
		else								// Not used in Windows
						 /*  updext: update the extended line which the cursor is currently
								 on at a column greater than the terminal width. The line
								 will be scrolled right or left to let the user see where
								 the cursor is. Called only in non Hscroll mode.
							*/
		{ 				/* calculate what column the real cursor will end up in */
			g_lbound = col - ((rhs - 1) % T_SCRSIZE) - T_MARGIN + 1;
			col -= g_lbound;

											/* scan through the line copying to the virtual screen*/
											/* once we reach the left edge						*/
											/* start scanning offscreen */
		{ VIDEO * vp = vtmove(row, g_lbound + fcol, 1, dotp);
			
//		vp->v_text[0] = /*BG(C_BLUE)+FG(C_WHITE)+*/'$';
								 			/* and put a '$' in column 1 */
			vp->v_flag |= (VFEXT | VFCHG);
		}}
	}}

	if (rhs < 0)			// allow deextension on this line
		wp->w_dotp = NULL;
	wp->w_fcol = fcol;
	wp->w_flag |= flag;

					 /* update the current window if we have to move it around */
{	int hard = wp->w_flag & WFHARD;
	if (hard)
		updall(wp, 1);

//g_curcol = col;
																/*	upddex: de-extend any line that deserves it */
{	int i;
  WINDOW * p;
	for (p = wheadp; p != NULL; p = p->w_next)
	{
#if MEMMAP == 0
		updall(p, -1);
#endif
		if (p == wp)
			p->w_dotp = dotp;
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

{	LINE * top = wp->w_linep;
	LINE *lp = top;			 /* search pointer */
	int nlines = wp->w_ntrows;
	int centre = wp->w_force;
  int flags = wp->w_flag;
	wp->w_force = 0;
												/* if not a requested reframe, check for a needed one */
	if ((flags & WFFORCE) == 0)
	{ int i;
		for (i = nlines; --i >= 0; ) 
		{												 		/* if the line is in the window, no reframe */
			if (lp == wp->w_dotp)
				goto nop_here;
																/* if we are at the end of the file, reframe */
			if (l_is_hd(lp))
				break;
				
			lp = lforw(lp);						/* on to the next line */
		}

		if	((flags & WFHARD) == 0)
		{ if		  (wp->w_dotp == lp)
			{ lp = lforw(top);
				flags |= WFTXTD;
				goto ret;
			}
			else if (!l_is_hd(wp->w_dotp) &&
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
			{ if			(!l_is_hd(lp))
					lp = lforw(lp);
				else if (l_is_hd(rp))
					break;
				if (!l_is_hd(rp))
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
			 !l_is_hd((lp = lback(lp))) && --centre > 0; )
		;

						 /* and reset the current line at top of window */
	lp = lforw(lp);
	flags |= WFHARD;
	pd_sgarbf = 1;
ret:
	wp->w_linep = lp;
nop_here:

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
	{	WINDOW *wp;
 																		/* update any windows needing refreshing */
		for (wp = wheadp; wp != NULL; wp = wp->w_next)
			if (wp->w_flag) 			 				/* if the window has changed, service it */
			{ int set = reframe(wp) & ~( WFMOVE+WFMODE);
														 				/* check the framing */
				if			(set & (WFTXTU+WFTXTD))
					scrollupdn(wp, set);
				else if (set & WFHARD)
					updall(wp, 1);			/* update all lines */
				else if (set)
					updall(wp, 0);			/* update EDITed line */
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
										vp1->v_color != pscreen[row]->v_color) && (curwp->w_flag & WFHARD);

	vp1->v_flag &= ~(VFCHG+VFREQ/*+VFML*/);/* flag this line is unchanged */

{	short prechrom = vp1->v_color;
	int bg = (vp1->v_flag & VFML) == 0 ? 0x70 : (prechrom & 0x70);

	pscreen[row]->v_color = prechrom;
	pscreen[row]->v_flag = vp1->v_flag;
#if 0
	if (pd_sgarbf)
		caution = FALSE;
#endif
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
															
{ short *phz = &ph->v_text[term.t_ncol];
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
	while (++cp1 < cp9 /* cpend */)		/* Ordinary. */
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

	if ((vp1->v_flag & VFML) == 0 )
	{ extern short scbot;
		int doeol = ph1 + 1 < phz;

		while (++ph1 < phz /* cpend */)		/* Ordinary. */
			*ph1++ = *++cp1;
	//millisleep(300);
		if (doeol)
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

int Pascal upmode()	/* update all the mode lines */

{ return orwindmode(WFMODE);
}

																/* force hard updates on all windows */
#if MEMMAP
int Pascal upwind_()			/* drop the parameter */
#else
int Pascal upwind(int garbage)
#endif
{ 
#if MEMMAP == 0
	pd_sgarbf |= garbage;
#endif
	return orwindmode(WFMODE | WFHARD); //  | WFMOVE);
}

/* Write a message into the message line. Keep track of the physical cursor
 * position. A small class of printf like format items is handled. 
 * Set the "message line" flag TRUE.	
 * Don't write beyond the end of the current terminal width.
 */
void Pascal mlout(UNSIGNED char c)

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
void Pascal mlputs(int wid, const UNSIGNED char * s)

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

#if S_WIN32

#define reverse_cursor(x) 

#else

static COORD g_tcursor;

void reverse_cursor(int notused)

{   tcaprev(1);
//  tcapchrom(8+4);
{   COORD tc = g_coords;
    VIDEO * vp = vscreen[tc.Y];
    mlout(vp->v_text[tc.X]);
    if (*(int*)&g_tcursor == *(int*)&tc)
      tcaprev(0);
    else
    { vp = vscreen[g_tcursor.Y];
      tcapmove(g_tcursor.Y,g_tcursor.X);
      tcaprev(0);
      mlout(vp->v_text[g_tcursor.X]);
      tcapmove(g_tcursor.Y,g_tcursor.X);
      g_tcursor = tc;
    }
}}

#endif

#define get_arg(type,ap) va_arg(ap, type)

int mlwrite(const char * fmt, ...)
{
#define FMTCH '%'

	va_list ap;
 	va_start(ap, fmt);

{ int s_discmd = pd_discmd;
	if      (fmt[0] == '%' && fmt[1] == '!')			/* force display */
		fmt += 2;
	else if (s_discmd <= 0)
		return 0;

{	int  ch;
	int popup = 0;
//--g_cursor_on;

	pd_discmd = 1;
	if (*fmt != '\001')
	{ --fmt;
#if COLOR && 0
		tcapbfcol(V_BLANK); /* set up the proper colors for the command line */
#endif
    if (fmt[1] <= 0)
    	/*tcaprev(0)*/;
    else
      reverse_cursor(0);
		tcapmove(255, 0);
		tcapeeol();
	}

	while ((ch = *++fmt) != 0)
	{ if (ch != FMTCH)
			mlout((char)ch);
		else 
		{	UNSIGNED char * sp = NULL;
			int width = 0;
			while (((unsigned)(ch = *++fmt - '0')) <= 9)
				width = width * 10 + ch;
		{ int radix = 10;
//		int v = get_arg(int,ap);
			switch (ch+'0')
			{  case '>':  pd_discmd = -1;
				 when 'p':  popup = 1;
				 when 'b':	(void)tcapbeep();
#if MEMMAP == 0
				 when 'w':  TTflush();
									  millisleep(100);
#endif
				 when 'o':  radix = 8 - 6;
				 case 'x':  radix += 6;
				 case 'd':  
				 case 'f':  sp = int_radix_asc(get_arg(int,ap), radix, ' ');
				 				  	if (ch == 'f' - '0')
										{ int sl = strlen(sp);
										  sp[sl] = sp[sl-1];
										  sp[sl-1] = sp[sl-2];
										  sp[sl-2] = '.';
										}
										goto mlp;
				 case 's':  sp = get_arg(char *, ap);

mlp:								mlputs(width, sp);
				 when 'c':  mlout((char)get_arg(int,ap));
//			 otherwise	mlout((char)(ch+'0'));
			}
		}}
	}
	TTflush();

{ int scol = ttcol;
	pd_got_msg = TRUE;
	if (pd_discmd < 0)
	{ (void)linstr(lastmesg);
	}
#if MEMMAP
	else
	{	tcapeeol();
	// mlputs(term.t_ncol-scol,NULL);
  	tcapmove(ttrow, scol);
//	memset(&vscreen[term.t_nrowm1]->v_text[ttcol], 0, (term.t_ncol-ttcol)*2);
	}
#else
 	tcapmove(ttrow, scol);
	strpcpy(g_cmd_line, lastmesg, sizeof(lastmesg));
#endif

	if (popup > 0)
#if MEMMAP
		mbwrite(lastmesg);
#else
		(void)ttgetc();
#endif

	pd_discmd = s_discmd;
//++g_cursor_on;
	return scol;															/* Number of characters */
}}}}


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
