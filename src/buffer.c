/*	BUFFER.C: buffer mgmt. routines
		MicroEMACS 3.10

 * Buffer management.
 * Some of the functions are internal,
 * and some are actually attached to user
 * keys. Like everyone else, they set hints
 * for the display system.
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"
#include	"base.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"logmsg.h"

#if S_WIN32
#include <windows.h>
#endif

char * Pascal repl_bfname(BUFFER * bp, const char * str)

{ return remallocstr(&bp->b_fname, str, 0);
}



int Pascal hidebuffer(int f, int n)

{ curbp->b_luct = 0;
	return OK;
}

#if S_MSDOS
#define RES_TYPE int
#define NEXTBUFFER nextbuffer
#else
#define RES_TYPE BUFFER*
#define NEXTBUFFER nextbuffer_
#endif



RES_TYPE Pascal NEXTBUFFER(int f, int n) /*switch to next buffer in buffer list*/

{	if (f == FALSE)
		n = 1;

{	BUFFER *bp = NULL;	/* current eligible buffer */

							 /* cycle thru buffers until n runs out */
	while (--n >= 0 && (bp = getdefb()) != NULL)
		swbuffer(bp);

	return (RES_TYPE)bp;
}}


#if S_MSDOS == 0

int Pascal nextbuffer(int f, int n)

{ return (int)nextbuffer_(f,n);
}

#endif


#if _DEBUG

void scan_buf_lu()

{ BUFFER *bp;
	for (bp = bheadp; bp != NULL; bp = bp->b_next)
		if ((bp->b_flag & BFINVS) == 0)
		{ int x = bp->b_luct;
		  char bname[100];
		  strcpy(bname, bp->b_bname);
		}
}

#endif



int g_top_luct;

int Pascal lastbuffer(int f, int n)   /* switch to previously used buffers */

{ BUFFER *bp;
	BUFFER *bestbp= NULL;
  BUFFER * maxbp = NULL;
  short tgtlu = curbp->b_luct - 1;
	short bestlu = -1;
	short toplu = -1;

#if _DEBUG
  scan_buf_lu();
#endif

	for (bp = bheadp; bp != NULL; bp = bp->b_next)
		if ((bp->b_flag & BFINVS) == 0)
		{ int lu = bp->b_luct;
			if (lu == 0)
				continue;

      if (lu > toplu)
      { toplu = lu;
        maxbp = bp;
      }
      if (lu <= tgtlu && lu > bestlu)
	  	{	bestbp = bp;
		  	bestlu = lu;
        if (lu == tgtlu)
          break;
			}
    }

  if (bestbp == NULL)
  	bestbp = maxbp;

	if (bestbp == NULL)
		return FALSE;

  if (bestbp != curbp)
  {	bestlu = bestbp->b_luct;
		swbuffer(bestbp);
		bestbp->b_luct = bestlu;
  	--g_top_luct;
  }

	return TRUE;
}

#if 0

int Pascal topluct()	/* calculate top luct */

{ register BUFFER *bp;
		 short maxlu = 0;
	
	for (bp = bheadp; bp != NULL; bp = bp->b_next)
		if ((bp->b_flag & BFINVS) == 0 && bp->b_luct > maxlu)
			maxlu = bp->b_luct;
	
	return maxlu;
}

#endif

void Pascal USE_FAST_CALL swbuffer(BUFFER * bp) /* make buffer BP current */
	
{	if (bp->b_langprops & BCFOR)
		bp->b_flag &= ~MDEXACT;
			 /* let a user macro get hold of things...if he wants */
	execkey(&exbhook, FALSE, 1);

	bp->b_luct = ++g_top_luct;

 	leavewind(curwp, 0);

	curbp = bp; 			/* Switch. */
{ char * fn = bp->b_fname;
	int flag = bp->b_flag;
  bp->b_flag |= BFACTIVE;			/* code added */

	if (fn != null)
	{
#if S_WIN32
		setconsoletitle(fn);
#endif
		if (!(flag & BFACTIVE))		/* not active yet*/
		{ 
			readin(fn, FILE_LOCK);
			curwp->w_flag |= WFFORCE;
		}
	}

	upwind();
{	WINDOW *wp = curwp;
	wp->w_flag |= WFMODE|WFHARD;			 /* Quite nasty.			 */
	wp->w_bufp	= bp;
	*(WUFFER*)wp = *(WUFFER*)bp;
	setcline();
										/* let a user macro get hold of things...if he wants */
	g_clring = (bp->b_langprops & (BCCOMT+BCPRL+BCFOR+BCSQL+BCPAS+BCML));
	
	if (g_clring & BCCOMT)
		addnewbind(CTRL | 'M', indent);

	execkey(&bufhook, FALSE, 1);
}}}



BUFFER *Pascal getdefb()	/* get the default buffer for a use or kill */

{ BUFFER *bp = curbp->b_next;

			/* Find the next buffer, which will be the default */
	while (bp != curbp)
		if		  (bp == NULL)
			bp = bheadp;
		else if (!(bp->b_flag & BFINVS))
			return bp;
		else
			bp = bp->b_next;
				/* don't get caught in an infinite loop! */
	return NULL;
}


/* Attach a buffer to a window. The
 * values of dot and mark come from the buffer
 * if the use count is 0. Otherwise, they come
 * from some other window.
 */
int Pascal usebuffer(int f, int n)
{
	BUFFER * bp = getcbuf(TEXT24, TRUE);
							/* "Use buffer" */
	if (!bp)
		return ABORT;
				/* make it invisable if there is an argument */
	if (f != FALSE)
		bp->b_flag |= BFINVS;

	swbuffer(bp);
	return TRUE;
}



static void init_buf(BUFFER * bp)

{	LINE * lp = &bp->b_baseline;
	bp->b_dotp		 = lp;
	bp->b_wlinep	 = lp;
	bp->b_baseline.l_fp = (Lineptr)lp;
	bp->b_baseline.l_bp = (Lineptr)lp;
  bp->b_baseline.l_props = L_IS_HD;
  bp->b_doto = 0;
	bp->b_flag &= ~BFCHG; 		/* Not changed		*/
	memset((char *)&bp->mrks, 0, sizeof(MARKS));
//bp->b_remote = NULL;
/*bp->b_fcol = 0;*/
}


/*
 * This routine blows away all of the text in a buffer.
 * If the buffer is marked as changed then we ask if it is ok;
 * The window chain is nearly always wrong if this gets called; 
 * the caller must arrange for the updates that are required. 
 * Return TRUE if everything looks good.
 */
int Pascal bclear(BUFFER * bp_)
	
{ LINE * 	lp;
	LINE * 	nlp;
	BUFFER * bp = bp_;
	int 	 s;

	if ((bp->b_flag & (BFINVS+BFCHG)) == BFCHG /* Not scratch buffer.  */
							 /* Something changed 	 */
	 && (s = mlyesno(TEXT32)) <= 0)
/*					"Discard changes" */
		return s;

	for (nlp = lforw(&bp->b_baseline); 
			((lp = nlp)->l_props & L_IS_HD) == 0; )
	{ nlp = lforw(lp);
		free((char*)lp);
	}

/*loglog1("Rebaseline %x", lp); */

	init_buf(bp);

{ WINDOW * wp;
	for (wp = wheadp; wp != NULL; wp = wp->w_next)
		if (wp->w_bufp == bp)
		{	wp->mrks = bp->mrks;
			wp->w_dotp = &bp->b_baseline;
			wp->w_doto = 0;
		}

	return TRUE;
}}



int Pascal zotbuf(BUFFER * bp)	/* kill the buffer pointed to by bp */

{	extern BUFFER * g_dobuf;

	if (window_ct(bp) > 0 || bp == g_dobuf)					/* Error if on screen.	*/
	{ mlwrite(TEXT28);
					/* "Buffer displayed" */
		return FALSE;
	}

{	int s = bclear(bp);
	if (s <= FALSE)				/* Blow text away.	*/
		return s;
		
{ BUFFER *bp1 = prevele(bheadp, bp);	/* Find the header. */

	if (bp1 == NULL)			/* Unlink it. */
		bheadp = bp->b_next;
	else
		bp1->b_next = bp->b_next;

	free(bp->b_remote);
	free(bp->b_fname);
	free((char *) bp);			/* Release buffer block */
	
	return TRUE;
}}}


/* Dispose of a buffer, by name.
 * Ask for the name. Look it up (don't get too
 * upset if it isn't there at all!). Get quite upset
 * if the buffer is being displayed. Clear the buffer (ask
 * if the buffer has been changed). Then free the header
 * line and the buffer header. Bound to "C-X K".
 */
int Pascal killbuffer(int f, int n)

{	BUFFER * bp = getcbuf(TEXT26, FALSE);
				             /* "Kill buffer" */
	flush_typah();

	return bp == NULL 													? TRUE :
#if 0
				 bp == curbp && window_ct(bp) > 0 &&
				 bp == (BUFFER*)NEXTBUFFER(0,1) 			? ABORT :
#endif
                                              	zotbuf(bp);
}



#if 0
X 		/* b_bname no longer variable length */
XPascal namebuffer(f,n) /*	Rename the current buffer */
X int f, n;
X{
X register BUFFER *bp;	/* pointer to scan through all buffers */
X char bufn[NBUFN];
X 			/* prompt for and get the new buffer name */
X while (TRUE)
X { if (mlreply(TEXT29, bufn, NBUFN) != TRUE)
X 	/*			"Change buffer name to: " */
X 		return FALSE;
X 	if (bfind(bufn, FALSE, FALSE) == null)	/* check for duplicates */
X 		break;
X }
X 					
X strpcpy(curbp->b_bname, bufn, sizeof(curbp->b_bname));
X curwp->w_flag |= WFMODE;	/* make mode line replot */
X mlerase();
X return TRUE;
X}
X
#endif


static
void Pascal fmt_modecodes(char * t, int mode)
	
{ int c = NUMMODES-2; 																	/* ignore CHGD, INVS */
	static const char modecode[NUMMODES-2] = "WCSEVOMYA"; /* letters for modes */

	t[c] = 0;
	while (--c >= 0)
		t[c] = mode & (1 << c) ? modecode[c] : '.';
}

/*	List all of the active buffers.  First update the special
	buffer that holds the list.  Next make sure at least 1
	window is displaying the buffer list, splitting the screen
	if this is what it takes.  Lastly, repaint all of the
	windows that are displaying the list.  Bound to "C-X C-B". 
	A numeric argument forces it to list invisable buffers as
	well.
*/

int Pascal listbuffers(int iflag, int n)
		 
{ BUFFER * bp;
	Char line[15+NUMMODES-2] = "Global Modes ";
	Int nbytes; 		/* # of bytes in current buffer */
 
	openwindbuf("[List]");

{	Int avail = curwp->w_ntrows - 2;
				/* build line to report global mode settings */
						/* output the mode codes */
//strcpy(&line[0], "Global Modes ");
	fmt_modecodes(&line[13], g_gmode);

	line[13+NUMMODES-2] = '\n';
	line[14+NUMMODES-2] = 0;

	if (linstr(line) == FALSE)
		return FALSE;

	if (linstr("ACT 	Modes 		 Size Buffer			File\n") == FALSE)
		return FALSE;
																					/* output the list of buffers */
	for (bp = bheadp; bp != NULL; bp = bp->b_next) 
	{ if ((bp->b_flag & BFINVS) && iflag == FALSE)
			continue;

		--avail;
		nbytes = 0L;													/* Count bytes in buf.	*/
	{	LINE * lp;
		for (lp = &bp->b_baseline; ((lp=lforw(lp))->l_props & L_IS_HD) == 0; )
 /* for (lp = bp->b_baseline; (lp = lforw(lp)) != bp->b_baseline; ) */
			nbytes += (Int)llength(lp)+1L;

		fmt_modecodes(&line[13], bp->b_flag);
					/* we could restore lastmesg */
		mlwrite("%>%c%c%c %s %7d %15s %s\n",
						bp->b_flag & BFACTIVE ? '@' : ' ',
						bp->b_flag & BFCHG ? '*' : ' ',
						bp->b_flag & BFTRUNC ? '#' : ' ',
						&line[13],
						nbytes,
						bp->b_bname,
						bp->b_fname);
	}}
	curbp->b_flag |= MDVIEW;
	curbp->b_flag &= ~BFCHG;		/* don't flag this as a change */
	if (avail > 0)
		shrinkwind(0, avail);
	return gotobob(0,0);
}}


#if 0

static
void Pascal unqname(char * name)			/* make sure a buffer name is unique */
									/* name to check on */
{																			/* check to see if its in the buffer list */
	while (bfind(name, 0, FALSE) != NULL)
	{	char *sp, *sp_;
	  for (sp = name; *sp; ++sp)	/* go to the end of the name */
		  ;
		sp_ = sp;
	 	for (; --sp > name && *sp != '.';)/* go to the last dot */
		  ;
		--sp;
		
		if (sp >= name && (in_range(*sp, '0','8') || 
		                   in_range(*sp, 'a','y') ||
		                   in_range(*sp, 'A','Z')))
		  *sp += 1;
		else
		{ *sp_++ = 'Z';
		  *sp_ = 0;
		}
	}
}

#endif

int g_bfindmade;

/* Find a buffer, by name.
 * If the buffer is not found and the "cflag" is TRUE, create it. 
 * The "bflag" is the settings for the flags in buffer.
 */
BUFFER *Pascal bfind(char * bname,
										 int cflag, int bflag)
				/* name of buffer to find */
				/* cflag   1: create it 2: make buffer name unique */
				/* bit settings for a new buffer */
{	int cc = -1;
                      			/* find the place in the list to insert this buffer */
	BUFFER * sb = backbyfield(&bheadp, BUFFER, b_next);

//if (cflag & 2)
//	unqname(bname);                     /* dont conflict with other buffers */

	for (; sb->b_next != NULL; sb = sb->b_next) 
	{ cc = strcmp(sb->b_next->b_bname, bname);
		if (cc < 0)
			continue;

		if (cc > 0)
			break;
		if ((cflag & 2) == 0)
			break;
		cc = 1;
	{	
#if 0
		char *sp = strlast(bname+1, '.') - 1;
#else
		char *sp, *sp_ = null;
		for (sp = bname; *++sp; )
		  if (*sp == '.')
				sp_ = sp;
		if (sp_ != NULL)
			sp = sp_;
		--sp;
#endif
		if (in_range(*sp, '0','8') || 
		    in_range(*sp, 'a','y') ||
		    in_range(*sp, 'A','Z'))
		  *sp += 1;
		else
		{ *sp++ = 'Z';
		  *sp = 0;
		}
	}}
	
	g_bfindmade = cc;
	
	if (cc == 0)
		return sb->b_next;

	if (!(cflag & 1))
		return null;

{	BUFFER *bp = (BUFFER *)mallocz(sizeof(BUFFER)+strlen(bname)+10); // was 2
	if (bp != NULL)
	{	init_buf(bp);
																				/* and set up the other buffer fields */
	/*bp->b_flag |= BFACTIVE; ** very doubtful !!! */
		bp->b_flag = bflag | g_gmode;
		bp->b_color = g_colours;

		bp->b_next = sb->b_next;    /* insert it */
		sb->b_next = bp;

		strcpy(bp->b_bname, bname);
  	customise_buf(bp);
	}
	return bp;
}}

#if 0

Pascal unmark(int f, int n) /* unmark the current buffers change flag */

{	curbp->b_flag &= ~BFCHG;		/* unmark the buffer */
	upmode(); 		/* unmark all windows as well */
	return TRUE;
}

#endif
