/* The functions in this file are a general set of line management utilities.
 * They are the only routines that touch the text. They also touch the buffer
 * and window structures, to make sure that the necessary updating gets done.
 * There are routines in this file that handle the kill buffer too. It isn't
 * here for any good reason.
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

/* This routine allocates a block of memory large enough to hold a LINE
 * containing "sz" characters. Return a pointer to the new block, or
 * NULL if there isn't any memory left.
 */

LINE *Pascal mk_line(const char * src, int used, int extra_cmt)
{
  LINE *lp = (LINE *)mallocz(sizeof(LINE)-sizeof(lp->l_text)+used+(extra_cmt>>2));
  if (lp != NULL)
  {
#ifdef _DEBUG	
		if (used < 0)
			mbwrite("Neg Used");
#endif

		lp->l_dcr = (used << (SPARE_SZ+2)) + extra_cmt | 1;
		memcpy(lgets(lp, 0), src, used);
	}
	return lp;
}



void Pascal ibefore(LINE * tline, LINE * new_line)

{ new_line->l_fp = (Lineptr)tline;
  new_line->l_bp = tline->l_bp;
  new_line->l_bp->l_fp = new_line;
  tline->l_bp = (Lineptr)new_line;
}

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
			else if (m->marko < offs)
				;
			else if (wh == 1)
			{	adj = noffs;
				goto neww;
			}
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

/* Delete line "lp". Fix all of the links that might point at it (they are
 * moved to offset 0 of the next line. Unlink the line from whatever buffer it
 * might be in. Release the memory. The buffers are updated too; the magic
 * conditions described in the above comments don't hold here.
 */
LINE * Pascal USE_FAST_CALL lfree(int noffs, LINE * lp)

{ LINE * nlp = lforw(lp);
  lback(lp)->l_fp = nlp;
  nlp->l_bp = lp->l_bp;
  rpl_all(0, noffs, noffs, lp, nlp);

  free((char *) lp);
  return nlp;
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

{ if ((curbp->b_flag & BFCHG) == 0)	/* First change, so	*/
	{	curbp->b_flag |= BFCHG;
	  curwp->w_flag |= WFMODE;
  /*mbwrite(curbp->b_fname);*/
    tcapbeep();
//  flag = WFHARD;
  }
												   /* make sure all the needed windows get this flag */ 
  window_ct(curbp);
  (void)orwindmode(curbp->b_window_ct > 1 ? WFHARD : flag);

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

/*
 * Delete a newline. Join the current line with the next line. If the next line
 * is the magic header line always return TRUE; merging the last line with the
 * header line can be thought of as always being a successful operation, even
 * if nothing is done, and this makes the kill buffer work "right". Easy cases
 * can be done by shuffling data around. Hard cases require that lines be moved
 * about in memory. Return FALSE on error and TRUE if all looks ok. Called by
 * "ldelchrs" only.
 */
#if 0

static int Pascal ldelnewline()

{	LINE * lp = curwp->w_dotp;
	int    in_cmt = lp->l_dcr & Q_IN_CMT;
	LINE * lp2 = lforw(lp);
	int used1 = lused(lp->l_dcr); 

	if (used1 > 0)
	{ if (l_is_hd(lp2))		/* not at buffer end.	*/
			return TRUE;
	{ int nsz = lused(lp2->l_dcr) + used1;
		LINE * lp3 =  mk_line(&lp->l_text[0], nsz, in_cmt);
	  if (lp3 == NULL)
	    return FALSE;

	  memcpy(&lp3->l_text[used1], &lp2->l_text[0], lused(lp2->l_dcr)); // nsz - used1
	  ibefore(lp2, lp3);

    lfree(used1,lp2);
	}}

  lfree(used1,lp);
  return lchange(WFHARD);
}

#endif

#define EXPANSION_SZ 8

/* Insert a newline into the buffer at the current location of dot in the
 * current window. The funny ass-backwards way it does things is not a botch;
 * it just makes the last line in the file not a special case. Return TRUE if
 * everything works out and FALSE on error (memory allocation failure). The
 * update of dot and mark is a bit easier then in the above case, because the
 * split forces more updating.
 */
int Pascal USE_FAST_CALL lnewline(int wh)

{ LINE * lp = curwp->w_dotp;   /* Get the address and  */
  int    l_dcr = lp->l_dcr;
	int    in_cmt = l_dcr & (Q_IN_CMT + 1);
	int    used = lused(l_dcr); 
	if (wh == 0)													// ldelnewline
	{	
		if (used > 0)
		{	LINE * lp2 = lforw(lp);
		  if (l_is_hd(lp2))		/* not at buffer end.	*/
				return TRUE;
		{ int nsz = lused(lp2->l_dcr) + used;
			LINE * inslp =  mk_line(&lp->l_text[0], nsz, in_cmt);
		  if (inslp == NULL)
		    return FALSE;

		  memcpy(&inslp->l_text[used], &lp2->l_text[0], lused(lp2->l_dcr)); // nsz - used

		  ibefore(lp2, inslp);

	    lfree(used,lp2);
		}}
	  lfree(used,lp);
	}
	else
	{	curwp->w_line_no += 1;
	{	int doto = curwp->w_doto;   /* offset of "."        */
		int nsz = used - doto;
	 	if (l_dcr)
		{ lp->l_dcr = (doto << (SPARE_SZ+2))
		  					 + (((nsz << 2) + l_dcr) & SPARE_MASK) + in_cmt;	
		}
		while (--wh >= 0)
		{	LINE * inslp = mk_line(&lp->l_text[doto], nsz, EXPANSION_SZ*4+in_cmt);
		  if (inslp == NULL)				                      /* New first half line  */
		    return FALSE;
		
		  rpl_all(2, 0, doto, lp, inslp);
																						/* trim line and update spare */
		  ibefore(!l_dcr ? lp : lforw(lp), inslp);
		  nsz = 0;
		}
  }}
  return lchange(WFHARD);
}

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

int g_overmode;

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

{	int tabsize = curbp->b_tabsize;
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

	if ( doto < used  && g_overmode >= ins &&
			(lgetc(lp, doto) != '\t' ||
			(unsigned short)doto % tabsize == (tabsize - 1)))
	{	lp->l_text[doto] = c;
	  curwp->w_doto = doto + 1;
  }
	else
  {	LINE * newlp = lp;
  	if (ins * 4 <= (lp->l_dcr & (((1 << SPARE_SZ)-1) << 2)))
	  {	lp->l_dcr += (ins << (SPARE_SZ+2)) - (ins * 4);		// more used less spare
	  }
	  else
	  {	newlp = mk_line(&lp->l_text[0], used + ins, EXPANSION_SZ*4+(lp->l_dcr & Q_IN_CMT));
	    if (newlp == NULL)
	      return FALSE;
	  }

//  line_openclose(newlp, lp, ins, (Int)lp->l_used-doto);
  	if (used - doto > 0)
  	{
#if 1
   		memmove(&newlp->l_text[doto+ins],&lp->l_text[doto],used-doto);
#else
			int offs = used - doto;
			while (--offs >= 0)
			{ newlp->l_text[doto+ins+offs] = lp->l_text[doto+offs];
			}
#endif
		}
	  memset(&newlp->l_text[doto],c,ins);
	  rpl_all(1, ins, doto, lp, newlp);

	  if (lp != newlp)
		{	ibefore(lp, newlp);				/* Link in */
			if (!l_is_hd(lp))					/* remove lp */
	   		lfree(0, lp);						/* No mark points to lp */
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
{ char tstring[NPAT+1];	/* string to add */

	int status = mlreply(stoi_msg[g_overmode & 1], tstring, NPAT);
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
{ g_overmode = true;
{ int cc = istring(f, n_);
  g_overmode = false;
  return cc;
}}

#endif

int kinsert_n;		/* parameter to kinsert, used in region.c */


/* This function deletes "n" bytes, starting at dot. It understands how to deal
 * with end of lines, etc. It returns TRUE if all of the characters were
 * deleted, and FALSE if they were not (because dot ran into the end of the
 * buffer. The "tokill" is TRUE if the text should be put in the kill buffer.
 */
int Pascal USE_FAST_CALL ldelchrs(Int n, int tokill)
								 /* Int n; 		  * # of chars to delete */
								 /* int tokill;	* put killed text in kill buffer flag */
{	if (rdonly())
		return FALSE;

//--g_overmode;
//++g_inhibit_scan;

{ int res = TRUE;
/* if (! (curbp->b_flag & BFCHG) && pd_discmd > 0)
     TTbeep();             
*/
  while (n > 0)
  { int  doto = curwp->w_doto;
    LINE * dotp = curwp->w_dotp;
    int l_dcr = dotp->l_dcr;
    if (l_dcr == 0)					   /* Hit end of buffer.   */
      break;

  {	int used = lused(l_dcr);
    int chunk = used - doto;  /* Size of chunk.       */
    if (chunk > n)
      chunk = n;
    if (chunk <= 0)           /* End of line, merge.  */
    { --n;
    	if (lnewline(chunk) == FALSE
       || tokill && kinsert('\n') == FALSE)
      { res = FALSE;
        break;
      }
      continue;
    }

    n -= chunk;
    if (n <= 0)
      rpl_all(3, chunk, doto, dotp, dotp);

    if (tokill)
  	{ char * cp1 = &dotp->l_text[doto];
			int delct;
    	for (delct = chunk; --delct >= 0; )
        if (kinsert(*cp1++) == FALSE)
          res = ABORT;
    }

	{ int spare = (l_dcr & (SPARE_MASK)) + (chunk << 2);	// increase spare
    if (spare > SPARE_MASK)
			spare = SPARE_MASK;
    dotp->l_dcr = (used - chunk) << (SPARE_SZ+2) | spare | (l_dcr & 3);
    
    memmove(&dotp->l_text[doto], &dotp->l_text[doto+chunk],
    				  used-chunk-doto);
  }}}

#if S_WIN32
  if (tokill && kinsert_n <= 0)
    ClipSet(0);
#endif
  lchange(WFEDIT);
//++g_overmode;
//--g_inhibit_scan;
  return res;
}}

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

extern REGION g_region;

/* The kill buffer abstraction */

typedef struct t_kill
{ char * mem;
  int    size;
  int    kfree;		/* # of bytes used in kill buffer*/
} t_kill;

static t_kill kills[NOOKILL+1];


/* doregion */

static
Pascal doregion(int wh, char * t)
	
{ if (wh < 0)
	{ 
		int offs =  curwp->w_doto;
		LINE * ln = curwp->w_dotp;
		int len = lused(ln->l_dcr);
		char * lp = lgets(ln, 0);
		char ch;

		while (offs >= 0 && 
               ((ch = lp[offs]) == '_' || isalnum(ch)))
			--offs;

    while (++offs < len && 
               ((ch = lp[offs]) == '_' || isalnum(ch)))
    { int cc = kinsert(ch);
	    if (cc <= FALSE)
	      return cc;
    }
  }
	else if (--wh > 0 && rdonly())	/* disallow this command if */
	  return FALSE;		        		/* we are in read only mode */
  else
	{ REGION * ion = getregion();
	  if (ion == NULL)
	    return 0;

	{ int	 loffs;
	  LINE  *linep = ion->r_linep; 		/* Current line.	*/

		int  space = NSTRING-1;
		int  sz = ion->r_size;
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
	    if      (wh < 0)		// was 0
	    { if (--space >= 0)
	        *t++ = ch;
	    }
	    else if (wh == 0)		// was 1
	    { ch = kinsert(ch);
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
Pascal lowerregion(int f, int n)

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


int to_kill_buff(int wh, int n)

{ if (wh == -2)
	{	kinsert_n = NOOKILL;
	  kills[NOOKILL].size = 0;
	}
	else
	{ kinsert_n = chk_k_range(n);
	  if (kinsert_n < 0)
	    return 1;
	}
	
	if (wh < 0)
	  (void)kdelete(wh+1, kinsert_n);

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
int Pascal kdelete(int f, int n)

{				/* first, delete all the chunks */
  n = chk_k_range(n);
  if (n < 0)
   	return FALSE;

#if S_MSDOS
  if (n == 0)
  { ClipSet(-1);
  }
#endif
  free(kills[n].mem);
																			/* and reset all the kill buffer pointers */
  memset(&kills[n], 0, sizeof(kills[0]));
  return TRUE;
}


/* Insert a character to the kill buffer, allocating new chunks as needed.
 * Return TRUE if all is well, and FALSE on errors.
 */
int Pascal kinsert(char ch)
			 	/* character to insert in the kill buffer */
{ int n = kinsert_n;
  if (n < 0)
    return FALSE;

  kills[n].size += 1;
{ char * mem = kills[n].mem;
  int kf = --kills[n].kfree;
  if (kf > 0)			     /* kf can legally go negative a little */
  { 
  }
  else
  { kills[n].kfree = KBLOCK-1;
    mem = (char *)malloc(kills[n].size + kills[n].kfree + 2);
    if (mem == NULL)
      return FALSE;
    if (kills[n].mem != NULL)
      memcpy(&mem[0], kills[n].mem, kills[n].size);

		free(kills[n].mem);
    kills[n].mem = mem;
  }
  
{ int sz = kills[n].size;
  mem[sz] = 0;	/* the zero must be set first */

  if (ch == '\n' && n == 0)
  { kills[n].size += 1;
    kills[n].kfree -= 1;
    mem[sz-1] = '\r';
    mem[++sz] = 0;
  }
  
  mem[sz-1] = ch;

  return TRUE;
}}}


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
  while (n--)
  { g_header_scan = 1;
    g_inhibit_scan += 1;

  {	int	len = 0;
  	char	*sp = NULL;					/* pointer into string to insert */

#if S_WIN32
    if (ix == 0 && gtusr("NOPASTE") == NULL)
    { 
      sp = ClipPasteStart();
      if (sp != null)
	      len = strlen(sp);
    }
#endif
		if (sp == NULL)
    { sp = kills[ix].mem;
      len = kills[ix].size;
    }
 
    while (--len >= 0)
    { if (*sp == 'M' - '@' && sp[1] == '\n')
        ++sp;
      else
      { if (linsert(1, *sp++) == FALSE)
          return FALSE;
      }
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
