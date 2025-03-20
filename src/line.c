/* The functions in this file are a general set of line management utilities.
 * They are the only routines that touch the text. They also touch the buffer
 * and window structures, to make sure that the necessary updating gets done.
 * There are routines in this file that handle the kill buffer too. It isn't
 * here for any good reason. Dont find LINE.l_text
 *
 * Note that this code only updates the dot and mark values in the window list.
 * Since all the code acts on the current window, the buffer that we are
 * editing must be being displayed, which means that the dot and mark values 
 * in the buffer headers are nonsense.
*/  
#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"
#include	"base.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"logmsg.h"

#define BSIZE(a)  (a + NBLOCK - 1) & (~(NBLOCK - 1))

#if DO_UNDO

#define SHOW_UNDO 1

Bool   g_inhibit_undo;

#endif

#if 0
	struct LINE * u_lp;		/* Link to LINE before the change */
	struct LINE * u_llost;/* Link to lost lines */
	int 	        u_dcr; 	/* Used(24) spare(6) incomment(1) header(1) */
#endif

/* This routine allocates a block of memory large enough to hold a LINE
 * containing "sz" characters. Return a pointer to the new block, or
 * NULL if there isn't any memory left.
 */

LINE * mk_line(const char * src, int srclen, int used, int extra_cmt)
{
  LINE *lp = (LINE *)mallocz(sizeof(LINE)-sizeof(lp->l_text)+used+(extra_cmt>>2)+0);
  if (lp == NULL)
  { mbwrite(TEXT99);
  	exit(1);
  }

#ifdef _DEBUG	
	if (used < 0)
		mbwrite("Neg Used");
#endif

	lp->l_dcr = (used << (SPARE_SZ+2)) + extra_cmt | 1;
	memcpy(lgets(lp, 0), src, srclen);

	return lp;
}



void Pascal ibefore(LINE * tline, LINE * new_line)

{ new_line->l_fp = (Lineptr)tline;
  new_line->l_bp = tline->l_bp;
  new_line->l_bp->l_fp = new_line;
  tline->l_bp = (Lineptr)new_line;
}

#if 0

char * USE_FAST_CALL point_curr(Lpos_t * spos_ref)

{ Lpos_t spos = *(Lpos_t*)&curwp->w_dotp;	/* original line pointer */
	spos.line_no = llength(spos.curline) - spos.curoff;
	*spos_ref = spos;
	return lgets(spos.curline, spos.curoff);
}

#endif

							// wh : -1, 0, 1, 2, 3

void Pascal rpl_all(int wh, int noffs, int offs, LINE * old, LINE * new_)
	
{ WINDOW *wp;

	for (wp = wheadp; wp != NULL; wp = wp->w_next) 
	{ if ((unsigned)wh <= 1)
			if (wp->w_linep == old)
				wp->w_linep = new_;
	  
	{ int adj;
	  MARK * m;
		for (m = &wp->mrks.c[NMARKS];; )	// all marks plus top of WINDOW/BUFFER
		{	if (--m < &wp->mrks.c[0]) 
			{ if (m < (MARK*)wp)
					break;
				m = (MARK*)wp;
			}

      if (m->markp != old &&
				 (m != (MARK*)wp || (LINE*)(wp->w_bufp) != old))	// for wh == -1
				 continue;

			if			(wh <= 0)
			{ if (wh < 0)
				{ wp->w_flag |= (WFHARD|WFMODE);
				  wp->w_linep = new_;
				}
				m->marko = noffs;
				goto newww;
			}
			else if (wh == 1)
			{	adj = noffs;
				goto neww;
			}
			else if (m->marko < offs)
				;
			else if (wh == 2)
			{	adj = -offs;
				goto neww;
			}
			else
			{ m->marko -= noffs;
				if (m->marko < offs)
					m->marko = offs;
			}
			continue;
neww:
			m->marko += adj;
newww:
			m->markp = new_;
		}
		leavewind(0, wp);
	}}
}

static
LINE * USE_FAST_CALL lextract(int noffs, LINE * lp)

{ LINE * nlp = lforw(lp);
  lback(lp)->l_fp = nlp;
  nlp->l_bp = lp->l_bp;
  rpl_all(0, noffs, noffs, lp, nlp);

	return nlp;
}

/* Undo cannot undo a previous change if you change the line before it. */
/* Attempting such an undo will result in a beep */


/* Delete line "lp". Fix all of the links that might point at it (they are
 * moved to offset 0 of the next line. Unlink the line from whatever buffer it
 * might be in. Release the memory. The buffers are updated too; the magic
 * conditions described in the above comments don't hold here.
 */
 
LINE * Pascal USE_FAST_CALL lfree(int noffs, LINE * lp)

{	LINE * plp = lback(lp);	
	LINE * nlp = lextract(noffs, lp);

{
#if DO_UNDO
	UNDO * ud = curbp->b_undo;
	if (ud)	
	{	//free(ud->u_held_lp);		allow the memory leak
	  ud->u_held_lp = lp;
		for (; ud != NULL; ud = ud->u_bp)
			if (ud->u_lp == lp)
				ud->u_lp = plp;
	}
  else
#endif
	  free((char *) lp);

  return nlp;
}}

static
void Pascal USE_FAST_CALL lunlink(int noffs, LINE * lp)

{	LINE * nlp = lextract(noffs, lp);
  UNDO * ud = curbp->b_undo;
	if (ud)
	{ // if (ud->u_lp == lp)
		//	 ud->u_dcr = 0;			// Disable it
			
	{ LINE * llost = ud->u_llost;
		ud->u_llost = lp;
		lp->l_fp = llost;
	}}
}

static 
int g_inhibit_scan = 0;
static
int g_header_scan = 0;
/*
 * This routine gets called when a character is changed in place in the current
 * buffer. It updates all of the required flags in the buffer and window
 * system. The flag used is passed as an argument (WFEDIT, or WFHARD); 
 * if the buffer is displayed in more than 1 window we change EDIT to HARD.
 * Set MODE if the mode line needs to be updated (the "*" has to be set).
 */
int Pascal lchange(int flag)

{ BUFFER * bp = curbp;
  if ((bp->b_flag & BFCHG) == 0)	/* First change, so	*/
	{	bp->b_flag |= BFCHG;
	  curwp->w_flag |= WFMODE;
  /*mbwrite(bp->b_fname);*/
    tcapbeep();
//  flag = WFHARD;
  }
												   /* make sure all the needed windows get this flag */ 
  window_ct(bp);
  (void)orwindmode(bp->b_window_ct > 1 ? WFHARD : flag);

  if (g_inhibit_scan <= 0)
	{	int all = 0;
		init_paren("\000", 0);

  {	LINE * lp = curwp->w_dotp;
		int ct = g_header_scan + 24;     							/* just 24 more */
		int ct_ = ct + 6;
		while (!l_is_hd(lp) && --ct >= 0)
		  lp = lback(lp);

		g_paren.in_mode = (lp->l_dcr & Q_IN_CMT);
		g_header_scan = 0; // ct + 6;

    while (--ct_ > 0)
		{ int mode = scan_par_line(lp);
		  lp = lforw(lp);
	  	if (l_is_hd(lp))
	    	break;
		
		  if ((lp->l_dcr ^ mode) & Q_IN_CMT)
		  { lp->l_dcr ^= Q_IN_CMT;
		    all = 1; // wp->w_flag |= WFEDIT;
		  }
		}

		updall(all, curwp);
  }}
  
	return TRUE;
}

#define EXPANSION_SZ 8

/* Insert a newline into the buffer at the current location of dot in the
 * current window. The funny ass-backwards way it does things is not a botch;
 * it just makes the last line in the file not a special case. Return TRUE if
 * everything works out and FALSE on error (memory allocation failure). The
 * update of dot and mark is a bit easier then in the above case, because the
 * split forces more updating.
 */
int Pascal USE_FAST_CALL lnewline(int wh)

{	curwp->w_line_no += wh;
{ LINE * lp = curwp->w_dotp;   /* Get the address and  */
  int   l_dcr = lp->l_dcr;
	int   in_cmt = l_dcr & (Q_IN_CMT+1);
	int   used = lused(l_dcr); 
	int   doto = curwp->w_doto;   /* offset of "."        */
	int   nsz = used - doto;
 	if (l_dcr)
	{ l_dcr += (nsz << 2);
		lp->l_dcr = (l_dcr & 0xfc) + (doto << (SPARE_SZ+2)) + in_cmt;
 //  					 + (((nsz << 2) + l_dcr) & SPARE_MASK) + in_cmt;	
	}

	while (--wh >= 0)
	{	LINE * inslp = mk_line(&lp->l_text[doto], nsz, nsz, EXPANSION_SZ*4+in_cmt);

    rpl_all(2, 0, doto, lp, inslp);
																						/* trim line and update spare */
    ibefore(!l_dcr ? lp : lforw(lp), inslp);
    nsz = 0;
  }
  return lchange(WFHARD);
}}

#if 0

static
void line_openclose(LINE * to, LINE * from, int gap, int len)

{	if (len > 0)
	{ int src 0;
	  int tgt = 0;
		if (gap > 0)
			tgt = gap
		else
			src = -gap
		to->l_dcr += (gap << (SPARE_SZ+2)) - (gap << 2);
//	if (to->l_spare > 255)
//		to->l_spare = 255;
		memmove(&to->l_text[tgt], &from->[src], len)
	}
}

#endif

/* Insert n copies of the character "c" at the current location of dot. 
 * In the easy case all that happens is that the text is stored in the line.  
 * In the hard case, the line has to be reallocated.  When the window list is 
 * updated take special care;  You always update dot in the current window.  
 * You update mark, and a dot in another window, if it is greater than the 
 * place where you did the insert.  Return TRUE if all is well.
 */
int Pascal linsert(int ins, char c)

{	if (rdonly())
		return FALSE;
/*if (! (curbp->b_flag & BFCHG)	&& pd_discmd > 0)
    TTbeep();		
*/
	if (ins <= 0)
		return TRUE;

  if (c == '\n')
  { if (g_inhibit_scan)				/* Go back by more when reenabled */
  		++g_header_scan;
    return lnewline(ins);
  }

{	int overmode = (curbp->b_flag & MDOVER) >> 5;
	int tabsize = curbp->b_tabsize;
	if (tabsize < 0)
	{	tabsize = - tabsize;
		if (ins == 1 && c == '\t')
		{ ins = tabsize - (getccol() % tabsize);
			c = ' ';
		}
	}

{	int doto = curwp->w_doto;
  LINE * lp = curwp->w_dotp;		/* Current line */
	int used = lused(lp->l_dcr);

#if DO_UNDO
	if (g_inhibit_undo <= 0)
		run_make(lp);
#endif
	if ( doto < used  && overmode >= ins &&
			(lgetc(lp, doto) != '\t' ||
			(unsigned short)doto % tabsize == (tabsize - 1)))
	{	lp->l_text[doto] = c;
	  curwp->w_doto = doto + 1;
  }
	else
  {	LINE * newlp = lp;
  	if (ins * 4 <= (lp->l_dcr & (((1 << SPARE_SZ)-1) << 2)))
	  	lp->l_dcr += (ins << (SPARE_SZ+2)) - (ins * 4);		// more used less spare
	  else
	  	newlp = mk_line(&lp->l_text[0], used, used + ins, EXPANSION_SZ*4+(lp->l_dcr & Q_IN_CMT));
	  
//  line_openclose(newlp, lp, ins, (Int)lp->l_used-doto);
  	if (used - doto > 0)
  	{
   		memmove(&newlp->l_text[doto+ins],&lp->l_text[doto],used-doto);
		}
	  rpl_all(1, ins, doto, lp, newlp);
	  memset(&newlp->l_text[doto],c,ins);

	  if (lp != newlp)
		{	ibefore(lp, newlp);				/* Link in */
			if (!l_is_hd(lp))					/* remove lp */
	   		lfree(0, lp);					/* No mark points to lp */
  	}
	}

  return lchange(WFEDIT);
}}}


#if _MSC_VER < 1600 && 0

_CRTIMP void *  __cdecl memmove(void * tgt, const void * src, size_t len)

{ char * t = (char*)tgt;
	char * s = (char*)src;
  if      (t <= s)
	{	while (--len >= 0)
			*t++ = *s++;
	}
	else if (len > 0)
		  mbwrite("Gotcha");
	return tgt;
}

#endif

#if FLUFF

int Pascal insspace(int f, int n)/* insert spaces forward into text */

{	Cc cc = linsert(n, ' ');
	if (cc <= OK)
		return cc;
	return backbychar(n);
}

#endif

/*
 * linstr -- Insert a string at the current point
 */
int Pascal linstr(const char * instr)
	
{	int status = TRUE;

	if (*instr != 0)
	{ //g_header_scan = 1;
	  g_inhibit_scan -= 1;
	  
	  while (*instr)
	  { status = linsert(1, *instr);
																							/* Insertion error? */
/*	  if (! status)
	    { mlwrite(TEXT99);
 *					"%%Can not insert string" *
	      break;
	    } */
	    instr++;
	  }
	  g_inhibit_scan += 1;
	}
	return status;
}

const char * stoi_msg[] = {TEXT68, TEXT69};

int Pascal istring(int f, int n)	/* ask for and insert a string into the
																	   current buffer at the current point */
{	int overmode = (curbp->b_flag & MDOVER) >> 5;
  char tstring[NPAT+10];	/* string to add */

	int status = mlreply(stoi_msg[overmode], tstring, NPAT);
											/* "String to insert<META>: " */
											/* "String to overwrite<META>: " */
	if (status > FALSE)
	{	if (f == FALSE)
		  n = 1;

		if (n < 0)
		  n = - n;

		while (--n >= 0 && (status = linstr(tstring)))
		  ;
	}
	return status;
}

#if FLUFF

int Pascal ovstring(int f, int n_) /* ask for and overwite a string into the current
			       buffer at the current point */
				/* ignored arguments */
{	int overmode = curbp->b_flag;
	curbp->b_flag |= MDOVER;
{ int cc = istring(f, n_);
	curbp->b_flag = overmode;
  return cc;
}}

#endif

static
int get_chunk(int l_dcr, int doto, int lim)

{	int used = lused(l_dcr);
	int chunk = used - doto;  /* Size of chunk. */
	return chunk <= lim ? chunk : lim;
}



int kinsert_n;		/* parameter to kinsert, used in region.c */

/* This function deletes "n" bytes, starting at dot. It understands how to deal
 * with end of lines, etc. It returns TRUE if all of the characters were
 * deleted, and FALSE if they were not (because dot ran into the end of the
 * buffer. The "tokill" is TRUE if the text should be put in the kill buffer.
 */
int Pascal USE_FAST_CALL ldelchrs(Int ct, int tokill, int killflag)
								 /* Int n; 		  * # of chars to delete */
								 /* int tokill;	* put killed text in kill buffer flag */
{	if (rdonly())
		return FALSE;

	if (killflag)
		g_thisflag = CFKILL;			/* this command is a kill */
	
//--g_overmode;
//++g_inhibit_scan;

{	LINE * dotp = curwp->w_dotp;
  int    doto = curwp->w_doto;
	int res = TRUE;

	if (ct == 0)
		return res;

 	if (tokill)
  {	int n = ct;
		while (n > 0)
	  {	int l_dcr = dotp->l_dcr;
	  	if (l_dcr == 0)					   /* Hit end of buffer.   */
	      break;

		{	int chunk = get_chunk(l_dcr, doto, n);
	    n -= chunk + 1;
			res &= kinsstr(&dotp->l_text[doto], chunk, kinsert_n);

	    if (n < 0)
	    	break;

    	doto = 0;
    	dotp = lforw(dotp);

	    res &= kinsert('\n');
    }}
    
		if (!res)
  		return res;
#if S_WIN32
	  if (kinsert_n <= 0)
  	  ClipSet(0);
#endif
  }

	dotp = curwp->w_dotp;
  doto = curwp->w_doto;

{	int l_dcr = dotp->l_dcr;
	int used = lused(l_dcr);
	int chunk = get_chunk(l_dcr, doto, ct);

  int spare = (l_dcr & (SPARE_MASK)) + (chunk << 2);	// increase spare
  if (spare > SPARE_MASK)
		spare = SPARE_MASK;
   
 	memmove(&dotp->l_text[doto], &dotp->l_text[doto+chunk],
 				  used-chunk-doto);

{ int n = ct - chunk;
	Bool all = n == 1 && doto == 0;
	if (!all)
		dotp->l_dcr = (used - chunk) << (SPARE_SZ+2) | spare | (dotp->l_dcr & 3);
	
  if (n <= 0)
   	rpl_all(3, chunk, doto, dotp, dotp);

	if (n > 0)
	{	LINE * nlp = all ? dotp : lforw(dotp);
		n += all * chunk;

		while (n > 0 && !l_is_hd(nlp))
		{	LINE * nlp_ = lforw(nlp);
			dotp = nlp;
			n -= llength(dotp) + 1;
			if (n >= 0)
				lunlink(0, dotp);
			if (n <= 0)
				break;
		 	nlp = nlp_;
		}

		if (!all || n != 0 && !l_is_hd(nlp))
		{	n += llength(dotp);
		{	int len2 = llength(nlp) - n;
			if (len2 > 0)
			{	LINE * ilp1 = curwp->w_dotp;
				if (all)
					ilp1 = nlp;

			{	int nsz = doto + len2;
				LINE * inslp =  mk_line(&ilp1->l_text[0], doto, nsz, nlp->l_dcr & Q_IN_CMT);

				memcpy(&inslp->l_text[doto], &nlp->l_text[n], len2);
				ibefore(nlp, inslp);
//			run_move(lp, inslp);
				rpl_all(0, doto, doto, ilp1, inslp);
				lunlink(doto, nlp);
				lfree(doto, ilp1);
			}}
		}}
		if (all)
		{ UNDO * ud = curbp->b_undo;
			ud->u_dcr = 0;						// dont replace
		}
	}

  lchange(WFEDIT|WFHARD);
//++g_overmode;
//--g_inhibit_scan;
  return res;
}}}}

/* getctext:	grab and return a string with the text of
		the current line
*/
char * Pascal getctext(char * t)

{		     /* find the contents of the current line and its length */
	LINE * lp = curwp->w_dotp;
	char * sp = lp->l_text;
	int size = lused(lp->l_dcr);
	if (size > NSTRING - 1)
	  size = NSTRING - 1;

	return memcpy(t, sp, size);
}

// extern REGION g_region;

/* The kill buffer abstraction */

typedef struct t_kill
{ char * mem;
  int    size;
  int    kfree;		/* # of bytes used in kill buffer*/
} t_kill;

static t_kill kills[NOOKILL+1];

static
int Pascal doregion(int wh, char * t)
	
{ if (wh < 0)
	{	int offs =  curwp->w_doto + 1;
		LINE * ln = curwp->w_dotp;
		int len = lused(ln->l_dcr);
		int dir = -1;
		char ch;

		while ((offs += dir) < len)
    {	if (offs < 0 || (ch = ln->l_text[offs]) != '_' && !isalnum(ch))
    	{	if (dir > 0)
    			break;
    		dir = 1;
    		continue;
    	}
    	if (dir < 0)
    		continue;
		{ int cc = kinsert(ch);
	    if (cc <= FALSE)
	      return cc;
    }}
#if 0
		while (offs >= 0 && 
               ((ch = ((char*)ln)[offs+fieldoffs(LINE*,l_text)]) == '_' || isalnum(ch)))
			--offs;

    while (++offs < len && 
               ((ch = ((char*)ln)[offs+fieldoffs(LINE*,l_text)]) == '_' || isalnum(ch)))
    { int cc = kinsert(ch);
	    if (cc <= FALSE)
	      return cc;
    }
#endif
  }
	else if (--wh > 0 && rdonly())	/* disallow this command if */
	  return FALSE;		        		/* we are in read only mode */
  else
	{	int  space = NSTRING-1;
		REGION * ion = getregion();
	  LINE  *linep = ion->r_linep; 		/* Current line.	*/
		int  sz = ion->r_size;
	  if (sz == 0)
	    return 0;

	{ int	 loffs;
				    /* don't let the region be larger than a string can hold */
//  if (wh == 0 && sz > NSTRING - 1)
//    sz = NSTRING - 1;

	  for (loffs = ion->r_offset; sz--; ++loffs)
	  { int ch;
	  	if (loffs < llength(linep))		/* End of line. 	*/
	      ch = lgetc(linep, loffs);
	    else
	    { linep = lforw(linep);
	      loffs = -1;
	      ch = '\n';
	    } 
	    if (wh == 0)		// was 1
	    { ch = kinsert((char)ch);
	      if (ch <= FALSE)
	        return ch;
	    }
	    else
	    { if (isalpha(ch) && (ch & 0x20) == (wh & ~2)) /* isupper */
	      {	lputc(linep, loffs, chcaseunsafe(ch));
	      	lchange(WFHARD);
	      }
	    }
	  }
	  if (t != NULL)
	    *t = 0;
	}}
	return TRUE;
}

/* return some of the contents of the current region
*/
const char *Pascal getreg(char * t)

{ return doregion(1,t) <= FALSE ? g_logm[2] : t;
}


/* Append all of the characters in the region to the n.th kill buffer. 
 * Don't move dot at all. 
 * Bound to "^XC".
 */
int copyregion(int f, int n)

{ return to_kill_buff(1, n);
}


/* Copy all of the characters in the current word to the n.th kill buffer.
 * Don't move dot at all. 
 * Bound to "A-W".
 */
int copyword(int f, int n)

{ return to_kill_buff(-1, n);
}



/* Lower case region. Zap all of the upper
 * case characters in the region to lower case. Use
 * the region code to set the limits. Scan the buffer,
 * doing the changes. Call "lchange" to ensure that
 * redisplay is done in all buffers. Bound to
 * "C-X C-L".
 */
int Pascal lowerregion(int f, int n)

{ return doregion(2 + 1, NULL);
}

/* Upper case region. Zap all of the lower
 * case characters in the region to upper case. Use
 * the region code to set the limits. Scan the buffer,
 * doing the changes. Call "lchange" to ensure that
 * redisplay is done in all buffers. Bound to
 * "C-X C-U".
 */
int Pascal upperregion(int f, int n)

{ return doregion(0x20 + 1, NULL);
}

//															wh != 0
int to_kill_buff(int wh, int n)

{ Cc cc;
	if (wh == -2)
		n = NOOKILL;

	if (wh < 0)
		if ((cc = kdelete(wh, n)) < 0)
			return cc;

  kinsert_n = chk_k_range(n);
	if (kinsert_n < 0)
		return kinsert_n;  

{ int cc = doregion(wh, NULL);
  if (cc <= FALSE)
    return cc;
  
#if S_WIN32
  if (kinsert_n == 0)
  	ClipSet(0);
#endif
  mlwrite(TEXT70);
				/* "[region copied]" */
  g_thisflag = CFKILL;
  return cc;
}}
 


int Pascal USE_FAST_CALL chk_k_range(int n)

{ if (! in_range(n,0,NOOKILL))
  { mlwrite(TEXT23);
					/* "Out of range" */
    return -1;
  }
  return n == 0 ? 0 : n-1;
}

/* the value of n gives which kill buffer to clear 0,1 => 1.
 * Delete all of the text saved in the kill buffer. Called by commands when a
 * new kill context is being created. The kill buffer array is released.
 * No errors.
 */
int Pascal kdelete(int not_used, int n)

{				/* first, delete all the chunks */
  n = chk_k_range(n);
  if (n < 0)
   	return FALSE;

#if S_MSDOS
  if (n == 0)
  { ClipSet(-1);
  }
#endif
  kinsert_n = n;

  free(kills[n].mem);
																			/* and reset all the kill buffer pointers */
  memset(&kills[n], 0, sizeof(kills[0]));
  return TRUE;
}


/* Insert a character to the kill buffer, allocating new chunks as needed.
 * Return TRUE if all is well, and FALSE on errors.
 */
int Pascal USE_FAST_CALL kinsert(char ch)
			 	/* character to insert in the kill buffer */
{ int n = kinsert_n;
  if (n < 0)
    return FALSE;

{	int inc = (ch == '\n' && n == 0) + 1;
  int sz = (kills[n].size += inc);
  int kf = (kills[n].kfree -= inc);
  if (kf > 0)			     /* kf can legally go negative a little */
  { 
  }
  else
  { kills[n].kfree = KBLOCK-1;

	  if (remallocstr(&kills[n].mem, kills[n].mem, sz + kills[n].kfree + 2) == NULL)
      return FALSE;
#if 0
    mem = (char *)malloc(sz + kills[n].kfree + 2);
    if (mem == NULL)
      return FALSE;
    if (kills[n].mem != NULL)
      memcpy(&mem[0], kills[n].mem, sz);

		free(kills[n].mem);
    kills[n].mem = mem;
#endif
  }
  
{ char * mem = kills[n].mem;
  mem[sz] = 0;
  mem[sz-inc] = '\r';
  mem[sz-1] = ch;

  return TRUE;
}}}



int kinsstr(const char * s, int len, int bno)

{	if (len < 0)
	{	len = strlen(s);
		if (kdelete(bno, bno) <= 0)
    	return FALSE;
  }

  while (--len >= 0)
    if (kinsert(*s++) == FALSE)
      return ABORT;
  
#if S_WIN32
  if (bno == 0)
 		ClipSet(0);
#endif

  return TRUE;
} 


int to_kill(int not_used, int n)

{ char val[133];
	int cc = mlreply(TEXT53, val, 132);
	if (cc < 0)
  	return cc;

	getval(val,val);

{ int ix = chk_k_range(n);
	return kinsstr(val, -1, ix);
}}


/* shift up all the kill buffers
 */
int Pascal shiftkill(int f, int n)

{ int ix;
	for (ix = NOOKILL-1; --ix >= 0; )
	{ t_kill skill = kills[ix+1];
		kills[ix+1] = kills[ix];
		kills[ix] = skill;
	}
	return TRUE;
}


/* return contents of the kill buffer 
 */
char *Pascal getkill()
	
{	
  return kills[kinsert_n].mem == NULL ? "" : kills[kinsert_n].mem;
}


// char last_was_yank;

/* When (N = -n) > 0 then the repeat count else the kill buffer to use.
 * Yank text back from the kill buffer. Bound to "C-Y".
 */
int Pascal yank(int notused, int n)

{ int ix = 0;

	if (rdonly())
		return FALSE;

  if (n <= 0)
    n = -n;
  else
  { ix = chk_k_range(n);
    if (ix < 0)
      return FALSE;
    n = 1;
  }
  
//last_was_yank = true;
				/* make sure there is something to yank */
  while (--n >= 0)
  { g_header_scan = 1;
    g_inhibit_scan += 1;

  { char * sp;			/* pointer into string to insert */
    int len;

#if S_WIN32
    if (ix == 0 && gtusr("NOPASTE") == NULL)
    { sp = ClipPasteStart();
      if (sp != NULL)
	    { len = strlen(sp);
	    	goto gots;
	    }
    }
#endif
  	sp = kills[ix].mem;			/* pointer into string to insert */
    len = kills[ix].size;
gots: 
    while (--len >= 0)
    { char ch = *sp;
    	++sp;
      if (ch == 'M' - '@' && sp[0] == '\n')
        continue;
      if (linsert(1, ch) == FALSE)
        return FALSE;
    }

    g_inhibit_scan -= 1;
	  lchange(WFEDIT);
#if S_WIN32
    if (ix == 0)
    	ClipPasteEnd();
#endif
  }}
  return TRUE;
}


int rdonly()

{ if (curbp->b_flag & MDVIEW)
	{
		mlwrite(TEXT109);
	/*		"VIEW mode" */
		return TRUE;
	}

#if DO_UNDO
	if (g_inhibit_undo <= 0)
	{	run_make(curwp->w_dotp);
 		run_trim(curbp, 32);
	}
#endif
	
	return FALSE;
}


#if DO_UNDO

#if SHOW_UNDO == 0
#define run_var_update(n)
#else

static
void run_var_update(int ct)

{	int nv = predefvars[EVUNDOS].i + ct;
//if (nv < 0)
//	nv = 0;
	predefvars[EVUNDOS].i = nv;
}

#endif

static Cc run_validate(LINE * lp)

{ LINE * probef = curbp->b_dotp;
	LINE * probeb = probef;
	LINE * lpstop = probeb;
	while (TRUE)
	{ probeb = lback(probeb);
		probef = lforw(probef);
		if (probef == lp || probeb == lp)
			break;
		if (probeb != lpstop)
			continue;
			
//	adb(33);
		tcapbeep();
		return 1;
	}
	
	return OK;
}


UNDO * run_destroy(UNDO * ud)

{	free(ud->u_held_lp);
{	LINE * lp = ud->u_llost;
	while (lp != NULL)
	{	LINE * nlp = lp->l_fp;
#ifdef _DEBUG
		if (l_is_hd(lp))
		{ adb(88);
			break;
		}
#endif
		free(lp);
		lp = nlp;
	}

{	UNDO * nud = ud->u_bp;
	free(ud);
	run_var_update(-1);

	return nud;
}}}


int undochange(int notused, int n)

{ UNDO * ud = curbp->b_undo;
	if (ud == NULL)
		return 1;

{ LINE * lb = ud->u_lp;
	if (run_validate(lb) != OK)
		return 1;

{ LINE * lp = lforw(lb);
	LINE * next = lforw(lp);
	LINE * newln = ud->u_llost;
	if (newln == NULL)
		newln = lback(lp);		

{	Bool prepend = (~ud->u_dcr) & 1;
	if (ud->u_dcr)
	{	int len = ud->u_dcr >> (SPARE_SZ+2);
		
		newln  = mk_line(ud->u_text, len, len, ud->u_dcr & Q_IN_CMT);
		if (run_validate(next) != OK)
			goto esc;

		ibefore(lp, newln);
		lextract(ud->u_offs,lp);
			
		lfree(0,lp);
	}
	
	if (prepend)
		next = lback(next);

	lp = ud->u_llost;
	while (lp)
	{	LINE * nlp = lp->l_fp;
		ibefore(next, lp);
		next = lp;
		lp = nlp;
	}
	ud->u_llost = NULL;
esc:
{	int offs = ud->u_offs < llength(newln) ? ud->u_offs : 0;

	curwp->w_dotp = lp;
  rpl_all(1, 0, offs, lp, newln);		// Calls leavewind()

//curwp->w_doto = offs;

	curbp->b_undo = ud->u_bp;
	run_destroy(ud);
	return lchange(WFEDIT|WFHARD);
}}}}}


void run_make(LINE * lp)

{	LINE * lb = lback(lp);
	if (/*g_inhibit_undo > 0 || */ curbp->b_undo && curbp->b_undo->u_lp == lb)
		return;

	run_var_update(1);

{ int len = llength(lp);
  UNDO *ud = (UNDO *)mallocz(sizeof(UNDO)-sizeof(ud->u_text)+len+4); // 4 say
  if (ud == NULL)
  	return -1;

	ud->u_lp = lb;
	ud->u_dcr = lp->l_dcr;
	memcpy(ud->u_text, lp->l_text, len);
	ud->u_offs = curwp->w_doto;
	ud->u_bp = curbp->b_undo;
	curbp->b_undo = ud;
}}


static
void run_move(LINE * olp, LINE * lp)

{ if (curbp->b_undo && curbp->b_undo->u_lp == olp)
		curbp->b_undo->u_lp = lp;
}


//void run_release(int ct)

//{ while (--ct >= 0)
//  { UNDO * ud = curbp->b_undo;
//    if (ud == NULL)
//    	break;
//    curbp->b_undo = run_destroy(ud);
//	}
//}


void run_trim(BUFFER * bp, int lim)

{	UNDO * cud = bp->b_undo;
	int deduct = -lim;
	while (cud != NULL)
	{ ++deduct;
		cud = cud->u_bp;
	}

  while (--deduct >= 0)
	{ UNDO * ud = bp->b_undo;
		UNDO * pud = NULL;
		while (ud != NULL)
		{ if (ud->u_bp == NULL)
				break;
			pud = ud;
			ud = ud->u_bp;
		}
		
		run_destroy(ud);
		if (!pud)
			bp->b_undo = NULL;
		else
			pud->u_bp = NULL;
	}
}

#endif
