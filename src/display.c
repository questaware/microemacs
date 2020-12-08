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
#define   IN_DISPLAY_C
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"logmsg.h"


#if _WINDOWS
#define NROW	60			/* Max Screen size.		*/
#define NCOL 148                     /* Edit if you want to.         */
#elif S_WIN32
#define NROW	75			/* Max Screen size.		*/
#define NCOL 148                     /* Edit if you want to.         */
#else
#define NROW	80			/* Max Screen size.		*/
#define NCOL 134                     /* Edit if you want to.         */
#endif
#define	MARGIN	8			/* size of minimim margin and	*/
#define	SCRSIZ	64			/* scroll size for extended lines */


int Pascal myttgetc();
int Pascal myvvv();

/* Standard terminal interface dispatch table. Most of the fields point into
 * "termio" code.
 */
TERM    term    = {
#if S_MSDOS
	NROW-1,
	NROW-1,
	NCOL,
	NCOL,
#else
	0, 0, 0, 0,	/* these four values are set dynamically at open time */
#endif
	MARGIN,
	SCRSIZ,
};

#define CHROM_OFF 0x8000

typedef struct	VIDEO
{	unsigned char   v_color;
	unsigned char	v_flag; 	/* Flags, must never be '/' */
	short	v_text[6];		/* Screen data. */
}	VIDEO;

#define VFCHG	0x0001	/* Changed flag 		*/
#define VFEXT	0x0002	/* extended (beyond column 80)	*/
#define VFREV	0x0004	/* reverse video status 	*//* contig these 2*/
#define VFREQ	0x0008	/* reverse video request	*/
#define VFCOL	0x0010	/* color change requested OBSOLETE */
#define VFCMT 0x0020	/* starts in comment		*/
#define VFML  0x0040	/* is a modeline		*/


#if S_WIN32
#define V_BLANK 0x7
#elif S_MSDOS
#define V_BLANK 0x7
#else
#define V_BLANK 0
#endif

#define MEMMAP	IBMPC


static VIDEO   **vscreen;		/* Virtual screen. */
#if MEMMAP == 0
 static VIDEO   **pscreen;		/* Physical screen. */
#endif

#if MEMMAP
#define MEMCT 1
#else
#define MEMCT 2
#endif


#if S_MSDOS
const short ctrans_[] =		/* ansi to ibm color translation table */
	{0, 4, 2, 6, 1, 5, 3, 7,
	 8, 12, 10, 14, 9, 13, 11, 15};

# define trans(x) ctrans_[x]
#else
# define trans(x) (x)
#endif

const char mdname[NUMMODES][8] = { 		/* name of modes		*/
 "WRAP ", "CMODE ", "MS ", "AbC ",  "VW ",
 "OVER ", "RE ", "CRYPT ", "ASAVE ","//" 
 };

static char fmtstr[LFSTR+1]; 		/* last message posted		*/

void Pascal updateline(int);


#if S_WIN32

short * Pascal get_vscr_line(int row)

{ 
  return vscreen[row]->v_text;
}


char Pascal get_vscr_colors(int row)

{
  return vscreen[row]->v_color;
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

{    register int i;
    register VIDEO *vp;

/*  if (gflags & MD_NO_MMI)
      return; */

#if S_WIN32
#else
    tcapsetfgbg(0x7000);      /* white background */ // should be derived
#endif
    if (vscreen != null)
      free((char*)vscreen);

#define ncols term.t_mcol

#define UESZ  ((sizeof(VIDEO)+2*(ncols+2)-10)*MEMCT)  // 10 --- 5 extra

    i = (term.t_mrowm1+2) * MEMCT; 
    vscreen = (VIDEO **)aalloc(i * (sizeof(VIDEO*)+UESZ));
#if MEMMAP == 0
    pscreen = &vscreen[term.t_mrowm1+1];
#endif
    vp = (VIDEO *)&vscreen[i];
/*  if (vscreen == NULL)
      meexit(1);*/

    for ( ; --i >= 0; )
    { vscreen[i] = vp;
#if S_WIN32
			memset(&vp->v_color, 0, (ncols+3)*2);
#else
      vp->v_flag = 0;
#endif
			vp->v_color = V_BLANK;
			vp = (VIDEO *)&vp->v_text[ncols+2];
    }
}


#if CLEAN
/*	 free up all the dynamically allocated video structures
*/
int Pascal vtfree()
{
    if (vscreen != null)
    {	free((char*)vscreen);
      vscreen = null;
    }
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
{ /* mlerase();
     tcapmove(term.t_nrowm1, 0);
     TTflush();*/
     tcapclose(1);
}
#endif


#if S_MSDOS
static const int bu_cols[] = { 4, 1 };
#define palcol(c) (((trans(palstr[(c)*2]-'0')<<4) + trans(palstr[(c)*2+1]-'0'))<<8)
#else
static const int bu_cols[] = { '4'-1, '1'-1};
#define palcol(c) (((c) + 1) << 8)
#endif

Short g_clring;

/* Set the virtual cursor to the specified row and column on the virtual
 * screen. There is no checking for nonsense values; this might be a good
 * idea during the early stages.
 */
static Cc Pascal vtmove(int row, int col, int cmt_col8, LINE * lp)

{ if (row >= term.t_mrowm1)
  { tcapbeep();
    return -1;
  }
  vtrow = row;
  vtcol = -col;

{ unsigned char * str = (unsigned char *)&lp->l_text[-1];
  VIDEO *vp = vscreen[row];
	Short  clring = g_clring;
	const int BCD = BCCOMT + BCPRL + BCSQL + BCPAS;
  int mode = cmt_col8 == 0 || clring == 0 || 
                        (vp->v_flag & VFML) ? -1 
											           				    : lp->l_props & VFCMT;
	register short vtc = vtcol;
  register int c;
	 	       int chrom_on = 0;		/* 1: ul, 2: bold, -1 manual */
				   int chrom_in_peril = false;
				   char duple = (curbp->b_langprops & BCPAS) == 0 ? '/' : ')';

	register short * tgt = &vp->v_text[0];
  				 int chrom_nxt = 0;
     		   int    len = llength(lp);			/* an upper limit */
       		 int highlite = 0;
  /* tgt[0] &= 0xff; ?? */

  while (--len >= 0)
  {
    c = *++str;

    if      (vtc >= term.t_ncol)
      tgt[term.t_ncol - 1] = (short)(CHROM_OFF+'$');
    else
    { if (c != 0 && c == highlight[(++highlite)])
      { if (highlight[1+highlite] == 0 && vtc >= highlite-1)
        { tgt[vtc-highlite+1] |= palcol(highlight[0]-col1ch);
				  highlite = 0;
          if (! (mode & (Q_IN_CMT+Q_IN_EOL)))
				    chrom_nxt = CHROM_OFF;
	  			else 
	    			chrom_nxt = cmt_col8;
        }
      }
      else
      { highlite = 0;
				if (c != 0 && c == highlight[1])
				  highlite = 1;
      }

      if      (c == '\t') 
      { c = ' ';
				if (((vtc + 1 + col) % curbp->b_tabsize) != 0)
				{ --str;
				  ++len;
				}
      }
      else if (c < 0x20 || c == 0x7F)
      { if (c >= col1ch && c <= col2ch)
        { if (c == 'H' - '@' && vtc > 0 && len > 0)
				  { int nc = (str[1] == str[-1]);
	    			if      (str[1]  == '_' ||
	          			   str[-1] == '_' || nc)
            { chrom_in_peril = false;
              if (chrom_on != nc + 1)
              { chrom_on = nc + 1;
				       	tgt[vtc-1] |= palcol(bu_cols[nc]);
              }
              if (str[-1] == '_')
              { tgt[vtc-1] &= 0xff00;		/* switch them */
                tgt[vtc-1] |= str[1]; 
              }
              ++str;
	    			  --len;
				      continue;
            }
          }
          chrom_on = -1;
				  c -= col1ch;			/* this is manual colouring */
				  chrom_nxt = palcol(c);
				  continue;
				}
        else
        { if (vtc >= 0)
            tgt[vtc] = '^' | chrom_nxt;
          ++vtc;
          c ^= 0x40;
        }
      }
      else if (c < '0')
      { if      (mode < 0 || (mode & Q_IN_EOL))
          ;
        else if (mode & Q_IN_CMT)
        { if ((mode & Q_IN_CMT) &&
              str[-1] == '*' && c == duple)	/* l_props must not be '*' */
          { mode = 0;
            c |= CHROM_OFF;
          }
        }
        else
        { if (c == '"')
            mode ^= Q_IN_STR;
        
          if ((mode & (Q_IN_STR+Q_IN_EOL)) == 0)
            if ((clring & BCCOMT) && (c == '*' || c == '/') && str[-1] == '/' ||
         				(clring & BCPAS)  &&  c == '*' && str[-1] == '(' ||
                (clring & BCSQL)  && c == '-'  && str[-1] == '-' ||
                (clring & BCFOR)  && c == '!' ||
                (clring & BCPRL)  && c == '#'
               )
            { mode = c != '*' ? Q_IN_EOL : Q_IN_CMT;
              if      (vtc == 0)
                c |= cmt_col8;
              else if (vtc > 0)
                tgt[vtc-1] |= cmt_col8;
            }
        }
      }

      if (chrom_in_peril && chrom_on > 0)
      { chrom_in_peril = false;
        if (vtc > 1)
          tgt[vtc-2] |= CHROM_OFF;
        chrom_on = 0;
      }
      if (chrom_on > 0)
        chrom_in_peril = true;
      if (vtc >= 0)
        tgt[vtc] = c | chrom_nxt;
      ++vtc;
      chrom_nxt = 0;
    }
  }
 /* fix at t-mob */
  if ((clring & BCD) && (lp->l_props & VFCMT) ||
      (clring & BCFOR) && toupper(lp->l_text[0]) == 'C' && llength(lp) > 0)
    tgt[0] |= cmt_col8;

  if (vtc > 0)
  {
    if (
#if S_MSDOS == 0
        mode > 0 ||
#endif
        chrom_on != 0 && vtc > 1)
    { if ((tgt[vtc-1] & 0xff00) && vtc < term.t_ncol)
        tgt[vtc++] = ' ';
      tgt[vtc-1] |= CHROM_OFF;
    }
  }
  c = term.t_ncol - vtc;
  while (--c >= 0)
  { if (vtc >= 0)
      tgt[vtc] = ' ';
    ++vtc;
  }

  vtcol = vtc;
  return OK;
}}


/************************** Modeline stuff *************************/


static WINDOW *lastwp;
static LINE *  lastlp;
static int     lastfcol;

void Pascal resetlcache()

{ lastwp  = null;
}

/* Redisplay the mode line for the window pointed to by the "wp".
 * This is the only routine that knows how the modeline is formatted.
 * Called by "update" any time there is a dirty window.
 */
void Pascal modeline(WINDOW * wp)
	
{
	int row;
	register int n; 	    /* cursor position count */
#define modeset n		    /* set of modes */
#define cpix n
	register BUFFER *bp;
	char * s;
	register int i;
	         union 
	         { LINE lc;
	           char c[NLINE+16]; /* buffer for part of mode line */
	         } tline;

	if (vscreen == null || !modeflag)/* don't bother if there is none*/
	  return;

  if ((wp->w_flag & WFMODE) || wp != lastwp)
	{ lastwp = wp;
    lastlp = null;
  }

  if (wp->w_dotp == lastlp /* and wp->w_wndp == NULL */)
	{ if (wp->w_fcol == lastfcol)
	    return;
	}
/*
	else if (lback(lp) == lastlp)
	  ++lastlno;
	else if (lforw(lp) == lastlp)
	  --lastlno;
	else
*/
/*	lastlno = wp->w_line_no; */

	lastlp = wp->w_dotp;
#if S_MSDOS && S_WIN32 == 0
	n = wp == curwp ? 0xcd : /* 173 :  */
#else
	n = wp == curwp ? '='  :
#endif
#if	REVSTA
	    revexist    ? ' ' :
#endif
			          '-';
	memset(&tline.lc.l_text[0], n, NLINE);

	n = wp->w_toprow+wp->w_ntrows;		/* Location. */
	if ((unsigned)n >= (unsigned)term.t_mrowm1)
	{ n = term.t_mrowm1 - 1;
	  mbwrite("toprow+ntrows oor");
	}
	vscreen[n]->v_flag &= ~VFCMT;
#if S_MSDOS || OWN_MODECOLOUR
	vscreen[n]->v_color = 0x25;			/* magenta on green */
	vscreen[n]->v_flag |= VFCHG |         VFCOL | VFML;/* Redraw next time*/
#else
/*	vscreen[n]->v_color = 0x7;			 * black on white */
	vscreen[n]->v_flag |= VFCHG | VFREQ | VFCOL | VFML;/* Redraw next time*/
#endif
	row = n;

	bp = wp->w_bufp;
	i = bp->b_flag;
	if (i & BFTRUNC)
	  tline.lc.l_text[0] = '#';			/* "#" if truncated */

	if (i & BFCHG)
	  tline.lc.l_text[1] = '*';			/* "*" if changed. */

	if (i & BFNAROW)
	{ tline.lc.l_text[2] = '<';			/* "<>" if narrowed */
	  tline.lc.l_text[3] = '>';
	}

	if (wp == curwp)
  { s = int_asc((int)wp->w_line_no);
        														  /* take up some spaces */
	  strcpy(&tline.lc.l_text[4], &s[strlen(s)-6]);
  }													/* are we horizontally scrolled? */
	lastfcol = wp->w_fcol;
	if (lastfcol <= 0)
		strcpy(&tline.lc.l_text[10], " (");
	else
	  concat(&tline.lc.l_text[10], "[", int_asc(lastfcol), "](", null);
					/* display the modes */
	
	modeset = wp->w_bufp->b_flag >> NUMFLAGS;
	for (i = -1; ++i < NUMMODES && modeset != 0; ) /* add in mode flags */
	{ if (modeset & 1) 
	    strcat(&tline.lc.l_text[10], mdname[i]);
	  modeset = modeset >> 1;
	}
  cpix = strlen(tline.lc.l_text);
  if (i != 0)
    --cpix;
	tline.lc.l_text[cpix] = ')'; 
	tline.lc.l_text[cpix+3] = ' ';
	tline.lc.l_text[cpix+1] = tline.lc.l_text[NLINE-1];

  s = bp->b_fname;

	if (s != NULL)	/* File name. */
  {
    Char * fn = s;

    for (s = &s[strlen(s)]; --s >= fn && *s != '/'; )
      ;
            
    if (strcmp(s+1,bp->b_bname) != 0)
      s = NULL;

#if 0
    strcpy(&tline.lc.l_text[cpix+4], bp->b_flag & MDDIR ? TEXT179 : TEXT34);
/*		     " Diry:" : " File:" */
#endif
	  strpcpy(&tline.lc.l_text[cpix+4], fn, NLINE - 4 - cpix);

    cpix = strlen(tline.lc.l_text)+1;
	  tline.lc.l_text[cpix-1] = ' ';
	}

  if (s == NULL)
	{ tline.lc.l_text[cpix] = '@';
	  strpcpy(&tline.lc.l_text[cpix+1], bp->b_bname, NLINE - 1 - cpix);
	  tline.lc.l_text[strlen(tline.lc.l_text)] = ' ';
	}

	tline.lc.l_used = term.t_ncol;
	tline.lc.l_text[tline.lc.l_used] = 0;
	
	vtmove(row, 0, 0, &tline.lc);		/* Seek to right line. */
#if 0
	vtputs((wp->w_flag&WFCOLR) != 0  ? "C" : "");
	vtputs((wp->w_flag&WFMODE) != 0  ? "M" : "");
	vtputs((wp->w_flag&WFHARD) != 0  ? "H" : "");
	vtputs((wp->w_flag&WFEDIT) != 0  ? "E" : "");
	vtputs((wp->w_flag&WFMOVE) != 0  ? "V" : "");
	vtputs((wp->w_flag&WFFORCE) != 0 ? "F" : "");
#endif
}

#if MEMMAP
# define pteeol(row)
#else

void Pascal pteeol(int row)

{   register short * tgt = &pscreen[row]->v_text[0];
    register short   sz = term.t_ncol;

    while (--sz >= 0)
			*tgt++ = ' ';
}


#if 0
X
XPascal psuppress(mrow)
X    int  mrow;
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
    /*  dir;			** < 0 => window down */
    /*  stt;			** start target location */
    /*  lenm1;                  */
{ register VIDEO * vp;

  if (dir < 0)
  { vp = pscreen[stt];
    memmove(&pscreen[stt], &pscreen[stt+1], lenm1 * sizeof(VIDEO*));
    pscreen[stt+lenm1] = vp;
  }
  else 
  { vp = pscreen[stt+lenm1];
    memmove(&pscreen[stt+1], &pscreen[stt], lenm1 * sizeof(VIDEO*));
    pscreen[stt] = vp;
  }
  lenm1 = term.t_ncol;
  while (--lenm1 >= 0)
    vp->v_text[lenm1] = ' ';
}

#endif


static void Pascal scrollupdn(int set, WINDOW * wp)/* UP == window UP text DOWN */
	 
{          int stt = wp->w_toprow;
  register int lenm1 = wp->w_ntrows-1;
  if (wp->w_flag & WFMODE)
    --lenm1;

{ int n = 1;

/*  vtscroll(n, wp->w_toprow, lenm1); */
  VIDEO * vp;

  if (set & WFTXTD)
  { n = -1;
    vp = vscreen[stt];
    memmove(&vscreen[stt], &vscreen[stt+1], lenm1 * sizeof(VIDEO*));
	  vscreen[stt+lenm1] = vp;
#if S_MSDOS == 0
    ttrow -= 2;
    tcapmove(ttrow + 2, ttcol);			/* a trick */
#endif
  }
  else 
  { vp = vscreen[stt+lenm1];
    memmove(&vscreen[stt+1], &vscreen[stt], lenm1 * sizeof(VIDEO*));
	  vscreen[stt] = vp;
#if S_MSDOS == 0
    tcapmove(ttrow, ttcol);
#endif
  }
 /*lenm1 += stt;*/
  vp->v_flag = VFCHG;
#if S_MSDOS == 0
  tcapscreg(stt, stt+lenm1);
  ttscupdn(n);
#endif
  pscroll(n, stt, lenm1);
  updall(wp, 0);
/* updateline(n < 0 ? wp->w_toprow : wp->w_toprow+lenm1);*/
#if 0 && MEMMAP == 0
X if (wp->w_ntrows < term.t_mrowm1 - 3)
X   if ( n > 0)
X   { pteeol(0);
X     if (wp->w_toprow == 0)
X       psuppress(wp->w_ntrows+1);
X     else
X     { psuppress(0);
X       updateline(wp->w_toprow-1);
X     }
X   }
X   else
X     if (wp->w_toprow == 0)
X     { pteeol(term.t_mrowm1-2);
X       psuppress(term.t_mrowm1-2);
X     }
#endif
}}
	 

void Pascal updline(int force)
/*	updpos: update the position of the hardware cursor and handle extended
		lines. This is the only update for simple moves.
*/
{
	register int i;
 static LINE * up_lp = NULL;
	register LINE * lp;
		 WINDOW * wp = curwp;
	
		      /* make sure it is not off the left side of the screen */
	while ((i = getccol() - wp->w_fcol) < 0)
	{	if (wp->w_fcol >= hjump)
		  wp->w_fcol -= hjump;
		wp->w_flag |= WFHARD | WFMODE | WFMOVE;
	}

	if (up_lp != wp->w_dotp && 
	    i + wp->w_fcol < term.t_ncol &&
	    minfcol == 0)
	{	i += wp->w_fcol;
		wp->w_fcol = 0;
		wp->w_flag |= WFHARD | WFMODE | WFMOVE;
	}
#if 1
  if (minfcol > wp->w_fcol)
  { i += wp->w_fcol - minfcol;
    if (i < 0)
      i = 0;
    wp->w_fcol = minfcol;
  }
#endif

	curcol = i;
	currow = wp->w_toprow;

  up_lp = wp->w_dotp;

	for (lp = wp->w_linep; 
	     lp != up_lp;
	     lp = lforw(lp))
		++currow;

{	int color = window_bgfg(curwp);
  int cmt_clr = ((color & 0x70) | (cmt_colour & 7)) << 8;

		    /* if horizontal scrolling is enabled, shift if needed */
	if (hscroll)
		while (curcol >= term.t_ncol - 1)
		{	curcol -= hjump;
			wp->w_fcol += hjump;
			wp->w_flag |= WFHARD | WFMODE | WFMOVE;
		}
	else
	{	i = curcol - term.t_ncol;
		if (i < -1)
		  lbound = 0;
		else
	/* updext: update the extended line which the cursor is currently
		   on at a column greater than the terminal width. The line
		   will be scrolled right or left to let the user see where
		   the cursor is. Called only in non Hscroll mode.
	*/
		{   /* calculate what column the real cursor will end up in */
		  lbound = curcol - (i % term.t_scrsiz) - term.t_margin + 1;

	  		/* scan through the line copying to the virtual screen*/
			/* once we reach the left edge			      */
		  vtmove(currow, lbound + wp->w_fcol, cmt_clr, wp->w_dotp);
						/* start scanning offscreen */
		  
		  vscreen[currow]->v_text[0] = /*BG(C_BLUE)+FG(C_WHITE)+*/'$';
					       /* and put a '$' in column 1 */
		  vscreen[currow]->v_flag |= (VFEXT | VFCHG);
		}
	}
		       /* update the current window if we have to move it around */
	if (wp->w_flag & WFHARD)
	  updall(wp, 1);

/*	upddex: de-extend any line that deserves it		*/

	for (wp = wheadp; wp != NULL; wp = wp->w_wndp)
	{ lp = wp->w_linep;
	  i = wp->w_toprow;
	{ int zline = i + wp->w_ntrows;       /* zero based */
	  if (modeflag == FALSE)
	    zline++;
	  if (zline >= term.t_mrowm1)
	  { /* addb(sline); */
	    zline = term.t_mrowm1-1;
	  }

	  for (; i < zline; ++i)
	  { if (vscreen[i]->v_flag & VFEXT)
	    { if ((wp != curwp) || (lp != wp->w_dotp) ||
				   (curcol < term.t_ncol - 1))
	      { vtmove(i, wp->w_fcol, cmt_clr, lp);
					/* this line no longer is extended */
      		vscreen[i]->v_flag &= ~VFEXT;
      		vscreen[i]->v_flag |= VFCHG;
	      }
	    }
	    lp = lforw(lp);
	  }
	  if (wp->w_flag & WFMODE)
	  {
	    modeline(wp);
	  }
	  wp->w_flag = 0;			/* we are done */
	}}

/*	updupd: update the physical screen from the virtual screen	*/

#if MEMMAP == 0
	if (sgarbf != FALSE)
	{  
	 /*tcapmove(0, 0);		** Erase the screen. */
	   tcapeeop();			/* Erase-page clears the message area. */
#if 0
	   mlerase();			/* needs to be cleared if colored */
#endif
	   mpresf = FALSE; 		 
        }
#endif

	for (i = -1; ++i <= term.t_nrowm1; )
  {
#if	MEMMAP == 0
    if (sgarbf)
	  { 
#if	REVSTA && 0
	    vscreen[i]->v_flag &= ~VFREV;
#endif
	    pteeol(i);
    }
#endif
				/* for each line that needs to be updated*/
	  if (sgarbf || (vscreen[i]->v_flag & VFCHG))
	  { 
#if GOTTYPAH && S_MSDOS == 0 && 0
	    if (force == FALSE && typahead())
	      break;			/* this prob. breaks updgar !! */
#endif
#if MEMMAP
/* UPDATELINE specific code for the IBM-PC and other compatables */

	  { struct VIDEO *vp1 = vscreen[i];	/* virtual screen image */
	    scwrite(i, vp1->v_text, vp1->v_color);
  	    vp1->v_flag &= ~VFCHG;	/* flag this line as changed */
	  }
#else
	    updateline(i);
#endif
	  }
	}
	sgarbf = FALSE;
}}

/* Make sure that the display is right. This is a three part process.
 * First scan through all of the windows looking for dirty ones. 
 * Check the framing and refresh the screen. 
 * Second ensure that "currow" and "curcol" are correct for the current window.
 * Third make the virtual and physical screens the same.
 */
int /*Pascal*/ update(int force)
	/* int force;	** force update past type ahead? */
{
    register WINDOW *wp;

    if (vscreen != NULL

#if	GOTTYPEAH || VISMAC == 0
            && (force
#endif
#if	GOTTYPAH
	      || ! typahead() 
#endif
#if	VISMAC == 0
	      || ! kbd_play(kbdmode)
#endif
#if	GOTTYPEAH || VISMAC == 0
       )
#endif
     )
  {			   	/* update any windows that need refreshing */
    for (wp = wheadp; wp != NULL; wp = wp->w_wndp)
	    if (wp->w_flag) 	     /* if the window has changed, service it */
  	  { register int set = reframe(wp) & ~( WFMOVE+WFMODE); 
              							 /* check the framing */
  	    if      (set & (WFTXTU+WFTXTD))
  	      scrollupdn(set, wp);  
  	    else if (set == WFEDIT)
  	    { updall(wp, 0);			/* update EDITed line */
  	      if (wp != curwp || ! (wp->w_flag & WFMODE))
  	        set = 0;
  	    } 
  	    else
  	    { if (set)
  	        updall(wp, 1);		        /* update all lines */
  	      set = (wp == curwp);		/* 1 is WFFORCE */
  	    }
      /* wp->w_changed = null; */
  	    wp->w_force = 0;
	      wp->w_flag &= ~WFMODE;
	      if (set && modeflag)
	        wp->w_flag = WFMODE;
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


/*	reframe:	check to see if the cursor is on in the window
			and re-frame it if needed or wanted		*/

int Pascal reframe(WINDOW * wp)

{
	register LINE *lp;		   /* search pointer */
	register LINE *rp;		   /* reverse search pointer */
	register int i; 		   /* general index/# lines to scroll */

	int nlines = wp->w_ntrows;
	int centre = wp->w_force;
	if (! modeflag)
	  ++nlines;
{	int flags = wp->w_flag;
									      /* if not a requested reframe, check for a needed one */
	if ((flags & WFFORCE) == 0)
	{ lp = wp->w_linep;
	  for (i = nlines; --i >= 0; ) 
	  {			/* if the line is in the window, no reframe */
	    if (lp == wp->w_dotp)
	      return flags;
				/* if we are at the end of the file, reframe */
	    if (lp->l_props & L_IS_HD)
	      break;
				/* on to the next line */
	    lp = lforw(lp);
	  }
		if	(sscroll && (flags & (WFFORCE+WFHARD)) == 0)
		{ if	  (wp->w_dotp == lp 
			    && screxist)
		  { wp->w_linep = lforw(wp->w_linep);
		    return flags | WFTXTD;
		  }
		  else if (lforw(wp->w_dotp) == wp->w_linep
			    && screxist
			    && (wp->w_dotp->l_props & L_IS_HD) == 0)
		  { wp->w_linep = wp->w_dotp;
		    return flags | WFTXTU;
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
	    { if      ((lp->l_props & L_IS_HD) == 0)
    		lp = lforw(lp);
	      else if (rp->l_props & L_IS_HD)
	        break;
	      if ((rp->l_props & L_IS_HD) == 0)
		      rp = lback(rp);
						/* problems????? */
	      if (lback(rp) == null)
	      { adb(57);
	        break;
	      }
	      if (--clamp == 0)
	      { adb(51);
      		break;
	      }
	    }
	  }
#endif
										/* how far back to reframe? */
	if (centre > 0)					/* only one screen worth of lines max */
	{ if (centre > nlines)
	    centre = nlines;
	  centre -= 1;
	} 
	else if (centre < 0)	/* negative update???? */
	{ centre += nlines;
	  if (centre < 0)
	    centre = 0;
	} 
	else
	  centre = nlines >> 1;
				    /* backup to new line at top of window */
	rp = lforw(wp->w_bufp->b_baseline);
	for (lp = wp->w_dotp; --centre >= 0 && lp != rp; lp = lback(lp))
	  ;
			       /* and reset the current line at top of window */
	wp->w_linep = lp;
	sgarbf = 1;
	return flags | WFHARD;
}}

#undef  centre
#undef  clamp

int Pascal window_bgfg(WINDOW * wp)

{
  return (trans((wp->w_color >>8) & 0x7)<< 4) | trans(wp->w_color & 7);
}


/*	updall: update all the lines in a window on the virtual screen */

void Pascal updall(WINDOW * wp, int all)
	
{		
	register LINE *lp = wp->w_linep;
	register int  sline = wp->w_toprow;
	register int  zline = sline + wp->w_ntrows;
      
	int color = window_bgfg(wp);
  int cmt_clr = ((color & 0x70) | (cmt_colour & 7)) << 8;

#if S_MSDOS == 0
	if (color == 0x70)
	  color = V_BLANK;
#endif
	if (! modeflag)
	  ++zline;
  if (zline >= term.t_mrowm1)
	{ /* addb(sline); */
	  zline = term.t_mrowm1-1;
	}
	
	for (; sline < zline; ++sline)
	{ if (all || lp == wp->w_dotp)	/* and update the virtual line */
	  {
	    vtmove(sline, wp->w_fcol, cmt_clr, lp);

	    vscreen[sline]->v_color = color;
	    vscreen[sline]->v_flag &= ~(VFREQ | VFML);
	    vscreen[sline]->v_flag |= VFCHG | (lp->l_props & Q_IN_CMT);
	  }
	  if ((lp->l_props & L_IS_HD) == 0)/* if we are not at the end */
	    lp = lforw(lp);
				  /* make sure we are on screen */
	  if (! all && vtcol < 0)
	    vtcol = 0;
	}
}

#ifdef _WINDOWS

void Pascal updallwnd(int reload)

{ WINDOW * wp;

  for (wp = wheadp; reload && wp != NULL; wp = wp->w_wndp)
  { updall(wp, true);
    int slfc = lastfcol;
    ++lastfcol;
    modeline(wp);
    lastfcol = slfc;
  }

  for (int i = -1; ++i <= term.t_nrowm1; )
  {
    struct VIDEO *vp1 = vscreen[i];	/* virtual screen image */
    scwrite(i, vp1->v_text, vp1->v_color);
  }
}

#endif

/* Update a single line. This does not know how to use insert or delete
 * character sequences; we are using VT52 functionality. Update the physical
 * row and column variables. It does try and exploit erase to end of line.
 */
#if  MEMMAP == 0

void Pascal updateline(int row)
   /* row;		** row of screen to update */
{
  register short *ph1 = &pscreen[row]->v_text[0];

  struct VIDEO *vp1 = vscreen[row];	/* virtual screen image */

  register short *cp1 = &vp1->v_text[0];
  register short *cp9 = &cp1[term.t_ncol];
				   short *cpend = cp9-1;
					 short prechrom = vp1->v_color << 8;
					 int bg = (vp1->v_flag & VFML) == 0 ? 0x7000
					              : (prechrom & 0x7000);
					 int postchrom = CHROM_OFF;
					 int revreq = vp1->v_flag & VFREQ; 		/* reverse video flags */
					 int caution = ((revreq ^ pscreen[row]->v_flag) & VFREQ) ||
									   		vp1->v_color != pscreen[row]->v_color     ||
									   		(vp1->v_flag & VFML) != 0;

  vp1->v_flag &= ~(VFCHG+VFREQ/*+VFML*/);/* flag this line is unchanged */
  pscreen[row]->v_flag = vp1->v_flag;
  pscreen[row]->v_color = vp1->v_color;

  if (sgarbf)
    caution = FALSE;
				
  if (!caution)			             /* skip common chars at the left */
  { short *ph9 = &ph1[term.t_ncol];
    while (cp1 < cp9 && cp1[0] == ph1[0])
    { if (*cp1 & 0x7f00)
      	prechrom = *cp1;
      if (*cp1 & CHROM_OFF)
      	prechrom = CHROM_OFF;
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
        revreq = -revreq;                /* non-blanks to the right */
    }
  }

  tcapmove(row, cp1 - &vp1->v_text[0]);

  if (prechrom != 0)
    tcapchrom((prechrom & 0x8700) | bg );

#if REVSTA
  if (revreq)
    tcaprev(TRUE);
#endif

  if (caution)
		tcapeeol();

  if (revreq > 0)
  { while (cp9 > cp1 && cp9[0] == ' ')
      --cp9;

    if (cpend <= cp9 + 3)		/* Use only if erase is */
      cp9 = cpend;			/* fewer characters. */
  }
  
  --cp1;
  while (++cp1 <= cpend)		/* Ordinary. */
  { prechrom = *cp1;
    if (cp1 <= cp9)
    { if (prechrom & 0x7f00)
        tcapchrom((prechrom & 0x8700) | bg );

      ttputc(prechrom);
      ++ttcol;
      if (prechrom & CHROM_OFF)
        tcapchrom(CHROM_OFF);
    }
    else
      if (prechrom & (0xff00))
        postchrom = prechrom;
        
    *ph1++ = prechrom;
  }

  if (postchrom & 0x7f00)
    tcapchrom((postchrom & 0x8700) | bg );
/*if (postchrom & CHROM_OFF)
    if (true || !REVERSE_VIDEO) tcapchrom(CHROM_OFF); else tcaprev(TRUE);*/

#if REVSTA
  if (revreq != 0)
    tcaprev(FALSE);
  else
#endif

  if ( revreq > 0 && !sgarbf && cp9 < cpend)
  { if (row != term.t_nrowm1 || 1)
      tcapeeol();
    else
      while (++ttcol < term.t_ncol - 1)
        ttputc(' ');
  }
  if (postchrom & CHROM_OFF)
    if (true || !REVERSE_VIDEO) tcapchrom(CHROM_OFF); else tcaprev(TRUE);
}

#endif

/* ################## end of window based routines #################### */

void Pascal upmode()	/* update all the mode lines */

{ orwindmode(WFMODE, 0);
}


void Pascal upwind()	/* force hard updates on all windows */

{ orwindmode(WFMODE | WFHARD, 0);
}

static int fmtstr_ix;

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
  { 
    if (discmd < 0)
    {  fmtstr[(fmtstr_ix++) & 0x7f] = c;
       return;
    }
    
    if (ttcol + 1 >= term.t_ncol)
      return;
    lastmesg[ttcol] = c;
  }
  if (discmd > 0)
  { int notused = discmd;
    lastmesg[ttcol+1] = 0;
    
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

{
  register char ch;

  while ((ch = *s++) != 0)
  { mlout(ch); 
    if (--mlputli_width == 0)
      break;
  } 
    
  while (--mlputli_width >= 0)
    mlout(' ');
    
  mlputli_width = 0;
}


				/* output integer as radix r */
static void Pascal mlputli(Int l, int r)

{
  static char hexdigits[] = "0123456789ABCDEF";
    register Int q;

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
	int f = s - integ * 100;	/* fractional portion of number */

	mlputli(integ, 10);
	mlout('.');
  integ = f / 10;
	mlout((char)('0' + integ));
	mlout((char)('0' + (f - integ * 10)));
}       

#define get_arg(type,ap) va_arg(ap, type)

int mlwrite(const char * fmt, ...)
{
	  Bool scoo;
	  int sdiscmd = discmd;
 register int ch;
#define fmtch '%'

 va_list ap;
 va_start(ap, fmt);

  mlputli_width = 0;
  fmtstr_ix = 0;

  if (discmd == 0)
    return 0;

  scoo = cursor_on_off(false);

  if (*fmt != '\001')
  { --fmt;
#if COLOR && 0
    tcapbfcol(V_BLANK); /* set up the proper colors for the command line */
#endif
#if S_WIN32
    memset(&vscreen[term.t_nrowm1]->v_text[0], 0, term.t_ncol*2);
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
    { int radix = 8;
      switch (ch+'0')
      {  case 'w': TTflush();
                   millisleep(4800);
				 when '>': discmd = -1;
         when 'x': radix += 6;
         case 'd': radix += 2;
         case 'c': 
         case 'o': if (ch == 'c'-'0')
		  						   mlout((char)get_arg(int,ap));
                   else
                     mlputli((Int)get_arg(int,ap), radix);
				 when 'D': mlputli(get_arg(Int,ap), 10);
				 when 's': mlputs(get_arg(char *, ap));
#if S_MSDOS == 0
				 when 'f': mlputf(get_arg(double, ap));
#else
				 when 'f': mlputf(get_arg(long, ap));
#endif
				 otherwise mlout((char)('0'+ch));
      }
      mlputli_width = 0;
    }}
  }
#if S_MSDOS != 0
			/* if we can not erase to end-of-line, do it manually */
  if      (vscreen == null)
    millisleep(4800);
  else
    TTflush();
#elif S_LINUX
  TTflush();
#endif
  mpresf = TRUE;
  if (discmd < 0)
  { fmtstr[(fmtstr_ix & 0x7f)] = 0;		/* terminate lastmesg[] */
    (void)linstr(fmtstr);
    loglog1("mlw %s", fmtstr);
  }
  else
  {
#if _WINDOWS
    if (vscreen != NULL)
      for (int mr = ttcol-1; ++mr < term.t_ncol; )
        vscreen[ttrow]->v_text[mr] = ' ';
#endif
    loglog1("mlw %s", lastmesg);
  }
    
  ch = discmd;
  discmd = sdiscmd;
  (void)cursor_on_off(scoo);
  return ch < 0 ? fmtstr_ix : ttcol; 				/* Number of characters */
}

		/* Force a string out to the message line regardless of the
		   current $discmd setting. This is needed when $debug is TRUE
		   and for the write-message and clear-message-line commands
		*/
void Pascal mlforce(const char * s)

{ int oldcmd = discmd;
  discmd = TRUE;
  if (gflags & MD_NO_MMI)
    discmd = FALSE;
#if S_BORLAND
  mlwrite(const_cast<char *>(s));
#else
  mlwrite(s);
#endif
  discmd = oldcmd;
}


/* Erase the message line. This is a special routine because the message line
 * is not considered to be part of the virtual screen. It always works
 * immediately; the terminal buffer is flushed via a call to the flusher.
 */
Cc Pascal mlerase()

{
  mlwrite("");
  mpresf = FALSE;
  return OK;
}


