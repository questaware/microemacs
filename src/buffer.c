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



char * Pascal repl_bfname(BUFFER * bp, const char * str)

{ return remallocstr(&bp->b_fname, str, 0);
}



int Pascal hidebuffer(int f, int n)

{ curbp->b_luct = 0;
	return OK;
}



int Pascal nextbuffer(int f, int n)  /* switch to the next buffer in the buffer list */

{	if (f == FALSE)
		n = 1;

{	register BUFFER *bp = NULL;	/* current eligible buffer */

							 /* cycle thru buffers until n runs out */
	while (--n >= 0 && (bp = getdefb()) != NULL)
		(void)swbuffer(bp);

	return (int)bp;
}}

#if _DEBUG

void scan_buf_lu()

{ BUFFER *bp;
	for (bp = bheadp; bp != NULL; bp = bp->b_bufp)
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

	for (bp = bheadp; bp != NULL; bp = bp->b_bufp)
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

  if (bestbp == NULL && maxbp != NULL)
  	bestbp = maxbp;

	if (bestbp == NULL)
		return FALSE;

  bestlu = bestbp->b_luct;
	(void)swbuffer(bestbp);
	bestbp->b_luct = bestlu;
  --g_top_luct;
	return TRUE;
}

#if 0

int Pascal topluct()	/* calculate top luct */

{ register BUFFER *bp;
		 short maxlu = 0;
	
	for (bp = bheadp; bp != NULL; bp = bp->b_bufp)
		if ((bp->b_flag & BFINVS) == 0 && bp->b_luct > maxlu)
			maxlu = bp->b_luct;
	
	return maxlu;
}

#endif

Pascal swbuffer(BUFFER * bp) /* make buffer BP current */
	
{	if (bp->b_langprops & BCFOR)
		bp->b_flag &= ~MDEXACT;
			 /* let a user macro get hold of things...if he wants */
	execkey(&exbhook, FALSE, 1);

#if S_WIN32
	setconsoletitle(bp->b_fname);
#endif
	bp->b_luct = ++g_top_luct;

 	leavewind(curwp);

	curbp = bp; 			/* Switch. */

	if (!(bp->b_flag & BFACTIVE) && bp->b_fname != null)  /* not active yet*/
	{
		bp->b_flag |= g_gmode & MDCRYPT;
		if (bp->b_flag & MDCRYPT)
			resetkey(&bp->b_key);													/* set up for decryption */

		readin(bp->b_fname, FILE_LOCK);
		curwp->w_flag |= WFFORCE;
	}

	if (bp->b_nwnd < 0)
		bp->b_nwnd = 0;
	++bp->b_nwnd; 		/* First use. */

{	WINDOW *wp = curwp;
	wp->w_bufp	= bp;
	wp->w_flag |= WFMODE|WFHARD;			 /* Quite nasty.			 */
#if 0
	for (wp = wheadp; wp != NULL; wp = wp->w_wndp)
		if (wp->w_bufp == bp)
#endif
		{ wp->w_linep 	= bp->b_wlinep;
			wp->w_dotp		= bp->b_dotp;
			wp->w_doto		= bp->b_doto;
			wp->mrks			= bp->mrks;
		/*wp->w_fcol		= bp->b_fcol;*/
			setcline();
		}
			/* let a user macro get hold of things...if he wants */
	g_clring = (bp->b_langprops & (BCCOMT+BCPRL+BCFOR+BCSQL+BCPAS+BCML));
	
	execkey(&bufhook, FALSE, 1);
	return TRUE;
}}



BUFFER *Pascal getdefb()	/* get the default buffer for a use or kill */

{ register BUFFER *bp = curbp->b_bufp;

			/* Find the next buffer, which will be the default */
	while (bp != curbp)
		if		  (bp == NULL)
			bp = bheadp;
		else if (!(bp->b_flag & BFINVS))
			return bp;
		else
			bp = bp->b_bufp;
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
	BUFFER *bp = getdefb();	/* temporary buffer pointer */
                					/* get the buffer name to switch to */
				
	bp = getcbuf(TEXT24, bp ? bp->b_bname : "main", TRUE);
							/* "Use buffer" */
	if (!bp)
		return ABORT;
				/* make it invisable if there is an argument */
	if (f != FALSE)
		bp->b_flag |= BFINVS;

	return swbuffer(bp);
}

/* Dispose of a buffer, by name.
 * Ask for the name. Look it up (don't get too
 * upset if it isn't there at all!). Get quite upset
 * if the buffer is being displayed. Clear the buffer (ask
 * if the buffer has been changed). Then free the header
 * line and the buffer header. Bound to "C-X K".
 */
int Pascal killbuffer(int f, int n)

{ BUFFER * nxt_bp = getdefb();
					                    			/* get the buffer name to kill */
	BUFFER * bp = getcbuf(TEXT26, nxt_bp ? nxt_bp->b_bname : "main", FALSE);
				             /* "Kill buffer" */
	flush_typah();

	return bp == NULL 																			? TRUE :
				 bp->b_nwnd > 0 && bp == curbp 
												&& bp == (BUFFER*)nextbuffer(0,1) ? ABORT
                                                          : zotbuf(bp);
}



static void init_buf(BUFFER * bp, LINE * lp)

{	lp->l_fp = (Lineptr)lp;
	lp->l_bp = (Lineptr)lp;
  lp->l_props 	 = L_IS_HD;
	bp->b_dotp		 = lp;
	bp->b_baseline = lp;
	bp->b_wlinep	 = lp;
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
	 && (s = mlyesno(TEXT32)) != TRUE)
/*					"Discard changes" */
		return s;

	for (nlp = lforw(bp->b_baseline); 
			((lp = nlp)->l_props & L_IS_HD) == 0; )
	{ nlp = lforw(lp);
		free((char*)lp);
	}

/*loglog1("Rebaseline %x", lp); */

	init_buf(bp, lp);

{ WINDOW * wp;
	for (wp = wheadp; wp != NULL; wp = wp->w_wndp)
		if (wp->w_bufp == bp)
		{	wp->mrks = bp->mrks;
			wp->w_dotp = bp->b_baseline;
			wp->w_doto = 0;
		}

	return TRUE;
}}



int Pascal zotbuf(BUFFER * bp)	/* kill the buffer pointed to by bp */

{	extern BUFFER * g_dobuf;

	if (bp->b_nwnd > 0 || bp == curbp || bp == g_dobuf)	/* Error if on screen.	*/
	{ mlwrite(TEXT28);
					/* "Buffer displayed" */
		return FALSE;
	}

{	int s = bclear(bp);
	if (s != TRUE)				/* Blow text away.	*/
		return s;
		
{ BUFFER *bp1 = prevele(bheadp, bp);	/* Find the header. */

	if (bp1 == NULL)			/* Unlink it. */
		bheadp = bp->b_bufp;
	else
		bp1->b_bufp = bp->b_bufp;

	free((char *) bp->b_baseline);		/* Release header line. */
	if (bp->b_remote != NULL)
		free(bp->b_remote);
	if (bp->b_fname != null)
		free(bp->b_fname);
	free((char *) bp);			/* Release buffer block */
	
	return TRUE;
}}}


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

Pascal listbuffers(int iflag, int n)
		 
{ BUFFER * bp;
	Char line[15+NUMMODES-2] = "Global Modes ";
	Int nbytes; 		/* # of bytes in current buffer */
 
	openwindbuf("[List]");
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
	for (bp = bheadp; bp != NULL; bp = bp->b_bufp) 
	{ if ((bp->b_flag & BFINVS) && iflag == FALSE)
			continue;

		nbytes = 0L;													/* Count bytes in buf.	*/
	{	LINE * lp;
		for (lp = bp->b_baseline; ((lp=lforw(lp))->l_props & L_IS_HD) == 0; )
 /* for (lp = bp->b_baseline; (lp = lforw(lp)) != bp->b_baseline; ) */
			nbytes += (Int)llength(lp)+1L;

		fmt_modecodes(&line[13], bp->b_flag);
					/* we could restore lastmesg */
		mlwrite("%>%c%c%c %s %7D %15s %s\n",
						bp->b_flag & BFACTIVE ? '@' : ' ',
						bp->b_flag & BFCHG ? '*' : ' ',
						bp->b_flag & BFTRUNC ? '#' : ' ',
						&line[13],
						nbytes,
						bp->b_bname,
						fixnull(bp->b_fname));
	}}
	curbp->b_flag |= MDVIEW;
	curbp->b_flag &= ~BFCHG;		/* don't flag this as a change */
	return gotobob(0,0);
}

/* Look through the list of
 * buffers. Return TRUE if there
 * are any changed buffers. Buffers
 * that hold magic internal stuff are
 * not considered; who cares if the
 * list of buffer names is hacked.
 * Return FALSE if no buffers
 * have been changed.
 */
Pascal anycb()
{
	register BUFFER *bp;

	for (bp = bheadp; bp != NULL; bp = bp->b_bufp) 
		if ((bp->b_flag & (BFINVS+BFCHG)) == BFCHG)
			return TRUE;

	return FALSE;
}


#define BCDEF (BINDENT+BCCOMT)
#define BPRL	(BINDENT+BCPRL)

int g_bfindmade;

/* Find a buffer, by name.
 * If the buffer is not found and the "cflag" is TRUE, create it. 
 * The "bflag" is the settings for the flags in buffer.
 */
BUFFER *Pascal bfind(const char * bname,
										 int cflag, int bflag)
				/* name of buffer to find */
				/* create it if not found? */
				/* bit settings for a new buffer */
{
#if 0
	const char nm[][4] = {"c","cpp", "cxx", "cs",	"h","pc","jav", "prl","pl",
												"for","fre","inc","pre","f", "sql","pas","md"};
	const char fm[] 	 = {BCDEF,BCDEF,BCDEF,BCDEF,BCDEF,BCDEF,BCDEF,BPRL, BPRL,
                  			BCFOR,BCFOR,BCFOR,BCFOR,BCFOR,BCSQL,BCPAS, BCML};
#endif
	int cc = -1;
	BUFFER * db = bheadp;
                      			/* find the place in the list to insert this buffer */
	BUFFER * sb = backbyfield(&bheadp, BUFFER, b_bufp);

	for (; sb->b_bufp != NULL; sb = sb->b_bufp) 
	{ cc = strcmp(sb->b_bufp->b_bname, bname);
#if 0
  { FILE *track = fopen("emacs.log", "a");
		fprintf(track, "Cmp %d %s %s\n", cc, sb->b_bufp->b_bname, bname);
		fclose(track);
  }
#endif	
		if (cc >= 0)
			break;
	}
	
	g_bfindmade = cc;
	
	if (cc == 0)
		return sb->b_bufp;

	if (cflag == FALSE)
		return null;

{ LINE *lp;
	BUFFER *bp = (BUFFER *)aalloc(sizeof(BUFFER)+strlen(bname)+2);
	if (bp == NULL)
		return NULL;

	lp = mk_line(null,0,0);
	if (lp == NULL)
	{ free((char *) bp);
		return NULL;
	}
									/* and insert it */
	loglog1("Baseline %x", lp);

	init_buf(bp, lp);
						/* and set up the other buffer fields */
		 /*bp->b_flag |= BFACTIVE; ** very doubtful !!! */
	bp->b_flag = bflag | g_gmode;
	bp->b_color = g_colours;
	bp->b_tabsize = tabsize;

	bp->b_bufp = sb->b_bufp;    /* insert it */
	sb->b_bufp = bp;

	for (cc = strlen(strcpy(bp->b_bname, bname)); --cc > 0 && bname[cc] != '.'; )
		;

  if (cc > 0)
  {	bname += cc + 1;

	  if (toupper(bname[0]) == 'E' &&
	  		bname[1] == '2' &&
	  		bname[2] == 0)
			bp->b_mode |= BCRYPT2;
#if 0  
  	for (cc = upper_index(nm)+1; --cc >= 0; )
	  	if (strcmp_right(nm[cc], bname) == 0)
		  {
			  bp->b_langprops = fm[cc];
  			break;
	  	}
#endif
	}

	return bp;
}}



Pascal unmark(int f, int n) /* unmark the current buffers change flag */

{	curbp->b_flag &= ~BFCHG;		/* unmark the buffer */
	upmode(); 		/* unmark all windows as well */
	return TRUE;
}
