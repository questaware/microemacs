/* The functions in this file are a general set of line management utilities.
 * They are the only routines that touch the text. They also touch the buffer
 * and window structures, to make sure that the necessary updating gets done.
 * There are routines in this file that handle the kill buffer too. It isn't
 * here for any good reason.
 *
 * Note that this code only updates the dot and mark values in the window list.
 * Since all the code acts on the current window, the buffer that we are
 * editing must be being displayed, which means that "b_nwnd" is non zero,
 * which means that the dot and mark values in the buffer headers are nonsense.
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


#if 0
X
XLINE * lp_link0;
XLINE * lp_link1;
X
X
XLINE *Pascal lback(lp)
X	LINE * lp;
X{ register LINE * lp_;
X  for (lp_ = lp; (lp_->l_props & L_HAS_BL) == 0; lp_ = lforw(lp))
X    if (lp_ == null)
X    { adb(59);
X      return lp; 
X    }
X
X  lp_link0 = lp_;
X{ register LINE * lp__;
X  lp_=lp_->l_fp[-1];
X  if (lp_ == null)
X  { adb(60);
X    return lp;
X  }
X  
X  lp_link1 = lp_;
X  
X  for (; (lp__ = lp_->l_fp[0]) != lp; lp_ = lp__)
X    if (lp_ == null)
X    { adb(60);
X      return lp; 
X    }
X
X  return lp_; 
X}}
#endif



/* This routine allocates a block of memory large enough to hold a LINE
 * containing "sz" characters. Return a pointer to the new block, or
 * NULL if there isn't any memory left.
 */

LINE *Pascal mk_line(const char * src, int sz, int lsrc)
{
  register LINE *lp = (LINE *)aalloc(sizeof(LINE)+sz);
  if (lp == NULL)
    return NULL;
  lp->l_props = 0;
  lp->l_used  = lsrc;
  lp->l_spare = sz - lsrc;
	if (src != null)
	  memcpy(lgets(lp, 0), src, lsrc);

  return lp;
}



void Pascal ibefore(LINE * tline, LINE * new_line)

{ new_line->l_fp = (Lineptr)tline;
  new_line->l_bp = tline->l_bp;
  lback(new_line)->l_fp = new_line;
  tline->l_bp = (Lineptr)new_line;
}

void Pascal rpl_all(LINE * old, LINE * new_, int wh, int offs, int noffs)
	
{ 
  register WINDOW *wp;

  for (wp = wheadp; ; wp = wp->w_wndp) 
  { if      (wp == NULL) 
    { wp = (WINDOW*)curbp;		/* complete just this loop */
      if (wp->w_linep == old)
        wp->w_linep = new_;
      if (wh < 0)
        break;
    }
    else if ((unsigned)wh <= 1)
      if (wp->w_linep == old)
        wp->w_linep = new_;
    
    if (wp->w_dotp == old || wh < 0 && wp->w_bufp == (BUFFER*)old)
      switch (wh)
      { case -1:wp->w_linep = new_;
        case -2:wp->w_flag |= (WFHARD|WFMODE);
        case 0: wp->w_dotp  = new_;
                wp->w_doto  = noffs;
               
        when 1: wp->w_dotp = new_;
                if (wp == curwp || wp->w_doto >= offs)
                  wp->w_doto += noffs;

        when 2: if (wp->w_doto >= offs)
                { wp->w_dotp = new_;
                  wp->w_doto -= offs;
                }
        otherwise
                if (wp->w_doto >= offs)
                { wp->w_doto -= noffs;
                  if (wp->w_doto < offs)
                    wp->w_doto = offs;
                }
      }
      
      if (wh != -2)
      { register MARK * m;

        for (m = &wp->mrks.c[NMARKS]; --m >= &wp->mrks.c[0]; ) 
          if (m->markp == old || wh < 0)
          { switch (wh)
            { case -1:
              case 0: m->markp = new_;
  			              m->marko = noffs;

		  	      when 1: m->markp = new_;
        	    	      if (m->marko >= offs)
  	      		          m->marko += noffs;
		  	      when 2: if (m->marko >= offs)
                      { m->markp = new_;
                        m->marko -= offs;
                      }
              otherwise
                      if (m->marko >= offs)
                      { m->marko -= noffs;
                        if (m->marko < offs)
                          m->marko = offs;
                      }
            }
          }
    }
    if (wp == (WINDOW*)curbp)
      break;
  }
}

/* Delete line "lp". Fix all of the links that might point at it (they are
 * moved to offset 0 of the next line. Unlink the line from whatever buffer it
 * might be in. Release the memory. The buffers are updated too; the magic
 * conditions described in the above comments don't hold here.
 */
LINE * Pascal lfree(register LINE * lp, int noffs)

{ LINE * nlp = lforw(lp);
  rpl_all(lp, nlp, 0, noffs, noffs);
  lback(lp)->l_fp = (Lineptr)nlp;
  nlp->l_bp = lp->l_bp;
#if 0
X  if (lp->l_props & L_HAS_BL)
X  { LINE * prev = lp_link0;
X    (void)lback(lforw(lp));
X    lp_link1->l_fp[-1] = lp_link0;
X    lp = (LINE*)&lp->l_fp[-1];
X  }
#endif
  free((char *) lp);
  return nlp;
}

int g_inhibit_scan = 0;		/* also used by replace */
static
int header_scan = 0;
/*
 * This routine gets called when a character is changed in place in the current
 * buffer. It updates all of the required flags in the buffer and window
 * system. The flag used is passed as an argument (WFEDIT, or WFHARD); 
 * if the buffer is displayed in more than 1 window we change EDIT to HARD.
 * Set MODE if the mode line needs to be updated (the "*" has to be set).
 */
void Pascal lchange(int flag)

{	WINDOW *wp;

  if (curbp->b_nwnd != 1) 		/* Ensure hard. 	*/
    flag = WFHARD;
  if ((curbp->b_flag & BFCHG) == 0)	/* First change, so	*/
	{	curbp->b_flag |= BFCHG;
  /*mbwrite(curbp->b_fname);*/
    TTbeep();
    flag = WFHARD;
  }
			   /* make sure all the needed windows get this flag */ 
  (void)orwindmode(flag, 0);

  wp = curwp;

  if (g_inhibit_scan == 0)
  {	LINE * lp = wp->w_dotp;
		int ct = header_scan + 6;     								/* just 6 more */
		while ((lp->l_props & L_IS_HD) == 0 && --ct >= 0)
		  lp = lback(lp);

		header_scan = ct + 6;
		ct = 12;

		init_paren("",0);
		paren.in_mode = (lp->l_props & Q_IN_CMT);

    while (true)
		{ scan_par_line(lp);
		  lp = lforw(lp);
	  	if (lp->l_props & L_IS_HD)
	    	break;
		  if      ((lp->l_props ^ paren.in_mode) & Q_IN_CMT)
		  {// logstr("CMT %s %d\n", lgets(lp, 0), lp->l_props);
	  	  lp->l_props ^= Q_IN_CMT;
		    wp->w_flag |= WFEDIT;
		  }
		  else if (--ct <= 0)
	  	  break;
		  updall(wp, 0);
		}
  }
}

#define EXPANSION_SZ 8

/* Insert a newline into the buffer at the current location of dot in the
 * current window. The funny ass-backwards way it does things is not a botch;
 * it just makes the last line in the file not a special case. Return TRUE if
 * everything works out and FALSE on error (memory allocation failure). The
 * update of dot and mark is a bit easier then in the above case, because the
 * split forces more updating.
 */
int Pascal lnewline()

{ if (curbp->b_flag & MDVIEW)     						/* don't allow this command if */
    return rdonly();              						/* we are in read only mode    */

{ LINE * lp1 = curwp->w_dotp;   /* Get the address and  */
  int    doto = curwp->w_doto;   /* offset of "."        */

	int  sz = lp1->l_used - doto;
  LINE * inslp = mk_line(&lp1->l_text[doto], sz + EXPANSION_SZ, sz);
  if (inslp == NULL)				                      /* New first half line  */
    return FALSE;

  lp1->l_used = doto;				/* REALLO */

{	LINE * s = doto == 0 ? lp1 : lforw(lp1);

  inslp->l_props = s->l_props & ~L_IS_HD;

  if (lp1->l_props & L_IS_HD)
    ibefore(lp1, inslp);
	else
  { ibefore(lforw(lp1), inslp);
    curwp->w_line_no += 1;
  }

  rpl_all(lp1, inslp, 2, doto, 0);
  lchange(WFHARD);
  return TRUE;
}}}

int newlinect = 0;

int g_overmode;

/* Insert n copies of the character "c" at the current location of dot. 
 * In the easy case all that happens is that the text is stored in the line.  
 * In the hard case, the line has to be reallocated.  When the window list is 
 * updated take special care;  You always update dot in the current window.  
 * You update mark, and a dot in another window, if it is greater than the 
 * place where you did the insert.  Return TRUE if all is well.
 */
int Pascal linsert(int n, char c)

{ int ins = n;
  if (curbp->b_flag & MDVIEW)	/* don't allow this command if	*/
    return rdonly();		/* we are in read only mode	*/
/*if (! (curbp->b_flag & BFCHG)	&& g_discmd)
    TTbeep();		
*/
  if (c == '\n')
  { ++newlinect;  
    return lnewline();
  }
  else
  { LINE * lp = curwp->w_dotp;		/* Current line */

   if (g_overmode &&
       curwp->w_doto < lp->l_used  &&
			(lgetc(lp, curwp->w_doto) != '\t' ||
			(unsigned short)curwp->w_doto % curbp->b_tabsize == (curbp->b_tabsize - 1)))
    ins = 0;

 	{ register int  doto;
	  LINE * newlp;

    if (ins <= lp->l_spare)
    { newlp = lp;
      newlp->l_spare -= ins;
      newlp->l_used += ins;
    }
    else
    { ins += lp->l_used;
      newlp = mk_line(&lp->l_text[0],BSIZE(ins+EXPANSION_SZ),lp->l_used);
      if (newlp == NULL)
        return FALSE;
			newlp->l_fp = lp->l_fp;
      newlp->l_bp = lp->l_bp;
      newlp->l_used = ins;
      newlp->l_props= lp->l_props & ~L_IS_HD;
    }

    doto = curwp->w_doto;
    if (ins != 0)
    {	if ((Int)lp->l_used - doto > 0)
    		memmove(&newlp->l_text[doto+n],&lp->l_text[doto],(Int)lp->l_used-doto);
    }
    memset(&newlp->l_text[doto],c,n);

    if      (lp->l_props & L_IS_HD) 	      /* At the end: special */
    {
#if _DEBUG
			if (doto != 0 || lp == newlp)
			{ mlwrite(TEXT170);							/* "bug: linsert" */
				return FALSE;
			}
#endif
			ibefore(lp, newlp);				/* Link in */
    }
    else if (lp != newlp)
    { lback(lp)->l_fp = (Lineptr)newlp;
      lforw(lp)->l_bp = (Lineptr)newlp;
      free((char *) lp);
    }

    rpl_all(lp, newlp, 1, doto, n);
    lchange(WFEDIT);
    return TRUE;
	}}
}

int Pascal insspace(int f, int n)/* insert spaces forward into text */

{	linsert(n, ' ');
	return backchar(f, n);
}



/*
 * linstr -- Insert a string at the current point
 */
int Pascal linstr(const char * instr_)
	
{	int status = TRUE;
	const char * instr = instr_;

	if (instr != NULL && *instr != 0)
	{ g_inhibit_scan += 1;
	  newlinect = 1;
	  
	  while (*instr)
	  { status = linsert(1, *instr);
							/* Insertion error? */
/*	    if (! status)
	    { mlwrite(TEXT99);
 *					"%%Can not insert string" *
	      break;
	  } */
	    instr++;
	  }
	  header_scan = newlinect;
	  g_inhibit_scan -= 1;
	}
	return status;
}

const char * stoi_msg[] = {TEXT68, TEXT69};

int Pascal istring(int f, int n)	/* ask for and insert a string into the current
																	   buffer at the current point */
{ char tstring[NPAT+1];	/* string to add */

	int status = mltreply(stoi_msg[g_overmode], tstring, NPAT);
											/* "String to insert<META>: " */
											/* "String to overwrite<META>: " */
	if (status == TRUE)
	{	if (f == FALSE)
		  n = 1;

		if (n < 0)
		  n = - n;

		while (--n >= 0 && (status = linstr(tstring)))
		  ;
	}
	return status;
}


int Pascal ovstring(int f, int n_) /* ask for and overwite a string into the current
			       buffer at the current point */
				/* ignored arguments */
{ g_overmode = true;
{ int cc = istring(f, n_);
  g_overmode = false;
  return cc;
}}

int kinsert_n;		/* parameter to kinsert, used in region.c */


/* This function deletes "n" bytes, starting at dot. It understands how to deal
 * with end of lines, etc. It returns TRUE if all of the characters were
 * deleted, and FALSE if they were not (because dot ran into the end of the
 * buffer. The "kflag" is TRUE if the text should be put in the kill buffer.
 */
int Pascal ldelchrs(Int n, int tokill)
								 /* Int n; 		  * # of chars to delete */
								 /* int tokill;	* put killed text in kill buffer flag */
{
  int res = TRUE;

  if (curbp->b_flag & MDVIEW)     /* don't allow this command if  */
    return rdonly();              /* we are in read only mode     */

  g_inhibit_scan += 1;
/* if (! (curbp->b_flag & BFCHG) && g_discmd)
     TTbeep();             
*/
  while (n > 0)
  { int  doto = curwp->w_doto;
    LINE * dotp = curwp->w_dotp;
    if (dotp->l_props & L_IS_HD)   /* Hit end of buffer.   */
    { res = FALSE;
      break;
    }

  { int chunk = dotp->l_used - doto;  /* Size of chunk.       */
    if (chunk > n)
      chunk = n;
    if (chunk <= 0)               /* End of line, merge.  */
    { if (ldelnewline() == FALSE
       || tokill && kinsert('\n') == FALSE)
      { res = FALSE;
        break;
      }
      --n;
      continue;
    }

    n -= chunk;
    if (n <= 0)
      rpl_all(dotp, dotp, 3, doto, chunk);

    if (tokill)
  	{ char * cp1 = &dotp->l_text[doto];
			int delct;
    	for (delct = chunk; --delct >= 0; )
        if (kinsert(*cp1++) == FALSE)
          res = ABORT;
    }

		if (dotp->l_used-(doto+chunk) <= 0)
	  { if (dotp->l_used-(doto+chunk) < 0)
				adb(44);
	  }
		else
    	memmove(&dotp->l_text[doto], &dotp->l_text[doto+chunk],
    				  dotp->l_used-(doto+chunk));

    dotp->l_used -= chunk;

    chunk += dotp->l_spare;
    if (chunk > 255)
      chunk = 255;
    dotp->l_spare = chunk;
  }}

#if S_WIN32
  if (tokill && kinsert_n <= 0)
    ClipSet(getkill());
#endif
  lchange(WFEDIT);
  g_inhibit_scan -= 1;
  return res;
}

/* getctext:	grab and return a string with the text of
		the current line
*/
char *Pascal getctext(char * t)

{
	char *tgt = t;	/* string pointer into returned line */

		     /* find the contents of the current line and its length */
	LINE * lp = curwp->w_dotp;
	char * sp = lp->l_text;
	int size = lp->l_used;
	if (size >= NSTRING)
	  size = NSTRING - 1;
				/* copy it across */
	while (--size >= 0)
	  *tgt++ = *sp++;
	*tgt = 0;
	return t;
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
/* static */ int Pascal ldelnewline()

{	LINE * lp1 = curwp->w_dotp;
	LINE * lp2 = lforw(lp1);
	int used1 = lp1->l_used; 
	if      (lp2->l_props & L_IS_HD)	/* At the buffer end.	*/
	{ if (used1 == 0) 			/* Blank line.		*/
	    lfree(lp1,0);
	}
#if 0
X 																								/* not worth it */
X	if (lp2->l_used <= lp1->l_spare - lp1->l_used)
X	{ register WINDOW *wp;
X
X	  cp1 = &lp1->l_text[lp1->l_used];
X	  cp2 = &lp2->l_text[0];
X	  while (cp2 != &lp2->l_text[lp2->l_used])
X	    *cp1++ = *cp2++;
X	  for (wp = wheadp; wp != NULL; wp = wp->w_wndp)
X	  { MARK * m;
X
X	    if (wp->w_linep == lp2)
X	      wp->w_linep = lp1;
X	    if (wp->w_dotp == lp2)
X	    { wp->w_dotp  = lp1;
X	      wp->w_doto += lp1->l_used;
X	    }
X	    for (m = &wp->mrks.c[NMARKS - 1]; m >= &wp->mrks.c[0]; --m) 
X	      if (m->markp == lp2)
X	      { m->markp = lp1;
X		m->marko += lp1->l_used;
X	      }
X	  }
X	  lp1->l_used += lp2->l_used;
X	  lp1->l_fp = lp2->l_fp;
X	  lforw(lp2)->l_bp = lp1;
X	  free((char *) lp2);
X	  return TRUE;
X	}
#endif
	else
	{ if (used1 > 0)
		{ int nsz = used1+lp2->l_used;
		  LINE * lp3 = mk_line(&lp1->l_text[0], nsz, used1);
	    if (lp3 == NULL)
	      return FALSE;
	/*  TTbeep(); */
	    lp3->l_used = nsz;
			lp3->l_spare = 0;
			lp3->l_props = lp1->l_props;
	    memcpy(&lp3->l_text[used1], &lp2->l_text[0], lp2->l_used);
	    ibefore(lp2, lp3);

	    lfree(lp2, used1);
	  }
	  lchange(WFHARD);
	  lfree(lp1, used1);
	}
	return TRUE;
}

/* The kill buffer abstraction */

#define NOOKILL 2

static struct
{ char * mem;
  int    size;
  int    kfree;		/* # of bytes used in kill buffer*/
} kills[NOOKILL];


int Pascal chk_k_range(int n)

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
  { ClipSet(NULL);
  }
#endif
  if (kills[n].mem != null)
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

		if (kills[n].mem != NULL)
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


/* return contents of the kill buffer 
 */
char *Pascal getkill()
	
{	
  return kills[kinsert_n].mem == NULL ? "" : kills[kinsert_n].mem;
}


char last_was_yank;

/* When (N = -n) > 0 then the repeat count else the kill buffer to use.
 * Yank text back from the kill buffer. Bound to "C-Y".
 */
int Pascal yank(int f, int n)

{
  register int	len;
  register char	*sp;					/* pointer into string to insert */
  int ix = 0;

  if (curbp->b_flag & MDVIEW)	/* don't allow this command if	*/
    return rdonly();					/* we are in read only mode	*/

  if (n <= 0)
    n = -n;
  else
  { ix = chk_k_range(n);
    if (ix < 0)
      return FALSE;
    n = 1;
  }
  
  last_was_yank = true;
				/* make sure there is something to yank */
  while (n--)
  { 
    newlinect = 1;
    g_inhibit_scan += 1;

#if S_WIN32
    if (ix == 0)
    { 
      sp = ClipPasteStart();
      if (sp == null)
        break;
      len = strlen(sp);
    }
    else
#endif
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

    header_scan = newlinect;
    g_inhibit_scan -= 1;
    lchange(WFEDIT);
#if S_WIN32
    if (ix == 0)
    { ClipPasteEnd();
      break;
    }
#endif
  }
  return TRUE;
}
