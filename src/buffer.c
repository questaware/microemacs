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
#include	"msdir.h"
#include	"logmsg.h"


char * Pascal repl_bfname(BUFFER * bp, const char * str)

{ return bp->b_fname == str ? bp->b_fname : remallocstr(&bp->b_fname, str, 0);
}



int Pascal hidebuffer(int f, int n)

{ curbp->b_luct = 0;
	return OK;
}

#if 0
const char nm[][4] = {"c","cpp", "cxx", "cs",	"h","pc","jav", "prl","pl",
											"for","fre","inc","pre","f", "sql","pas","md"};
const char fm[] 	 = {BCDEF,BCDEF,BCDEF,BCDEF,BCDEF,BCDEF,BCDEF,BCPRL, BCPRL,
                 			BCFOR,BCFOR,BCFOR,BCFOR,BCFOR,BCSQL,BCPAS, BCML};
#endif
const char * suftag  = "cpqafm";

static
void Pascal init_buf(BUFFER * bp)

{		LINE * lp = &bp->b_baseline;
		bp->b_dotp		 = lp;
		bp->b_wlinep	 = lp;
		bp->b_baseline.l_fp = (Lineptr)lp;
		bp->b_baseline.l_bp = (Lineptr)lp;
  	bp->b_baseline.l_dcr = 0;
		bp->b_flag &= ~BFCHG; 		/* Not changed		*/
		memset(&bp->b_doto, 0, sizeof(int)*2 + sizeof(bp->b_mrks));
}


int reload_buffers(void)

{	int ct = -1;
	BUFFER *bp;

//return ct;

	for (bp = bheadp; bp != NULL; bp = bp->b_next)
		if ((bp->b_flag & (BFINVS+MDDIR) == 0) && bp->b_fname)
		{	Filetime dt;
#if S_WIN32
			extern Filetime g_file_time;
			if (!name_mode(bp->b_fname))
				continue;
			dt = g_file_time;
		  if (*(__int64*)&dt == *(__int64*)&(bp->b_utime)
		  &&  *(__int64*)&(bp->b_utime) != 0)
		  	continue;
#else
		  FILE * ffp = ffropen(bp->b_fname);
			if (!ffp)
				continue;
				
		  dt = ffiletime(ffp);
		  if (dt == bp->b_utime && bp->b_utime != 0)
		  	continue;
		  	
		  fclose(ffp);
#endif
			if (++ct == 0)
			{	bp->b_utime = dt;
				if (bp != curbp)
					swbuffer(bp);
			}
		}

	return ct;
}

static
void Pascal customise_buf(BUFFER * bp)

{		int zero = 0;
	  const char * fsuffix = "";
		const char *fn;
		for ( fn = bp->b_bname - 1; *++fn != 0; )
      if (*fn == '.')
      {	fsuffix = fn;
				if (fsuffix[1] == 'e' && fsuffix[2] == '2')
					zero |= BCRYPT2;
//     	if (fsuffix[1] == 's' && fsuffix[2] == 'q')
//     		zero = BCSTRNL;
      }

		init_buf(bp);

{		int tabsize = pd_tabsize;
		if (pd_file_prof != NULL)
    {	
    	char * pr = pd_file_prof - 1;
      while (*++pr != 0)
      { if (*pr != '.') continue;
        
      { const char * p = fsuffix;

        while (*++p != 0 && *++pr == *p)
          ;

        if (*p != 0 || pr[1] != '=') continue;

//			bp->b_flag &= ~ MDIGCASE;
			  if (pr[2] == '^')
        { pr += 1;
				  zero |= MDIGCASE;
				}
      { int six;
        for (six = 6; --six >= 0; )
        	if (((suftag[six] ^ pr[2]) & ~0x20) == 0)
	        { bp->b_langprops = (1 << six);
	          ++pr;
					}
          
        six = atoi(pr+2);
        if (six != 0)
        	tabsize = six;
        break;
      }}}
		}

    if (bp->b_key != NULL)
   		zero |= MDCRYPT;

		bp->b_flag |= zero | g_gflag;
    bp->b_tabsize = tabsize;
		bp->b_color = g_bat_b_color;
}}



int g_bfindmade;						// result from bufflink()

/* Find a buffer, by name.
 * If the buffer is not found and the cflag & 1 is TRUE, create it. 
 */
BUFFER *Pascal bfind(const char * bname, int cflag)
										/* name of buffer to find */
										/* cflag   1: create it 2: make buffer name unique */
{	int cc = -1;
                      			/* find the place in the list to insert this buffer */
	BUFFER * sb = backbyfield(g_bheadp_ref, BUFFER, b_next);

	for (; sb->b_next != NULL; sb = sb->b_next) 
	{ cc = strcmp(sb->b_next->b_bname, bname);
		if (cc < 0)
			continue;

		if (cc > 0)
			break;
		if (!(cflag & 2))
			return sb->b_next;
		cc = 1;					// Case greater than existing, need unique : dont return NULL
	{	
#if 0
		char *sp = strlast(bname+1, '.') - 1;
#else
		char *sp, *sp_ = null;
		for (sp = (char*)bname; *++sp; )			// declared const but never if cflag & 1
		  if (*sp == '.')
				sp_ = sp;
		if (sp_ != NULL)
			sp = sp_;
		--sp;
#endif
		if (in_range(*sp, '0','8') || 
		    in_range((*sp & 0xdf), 'A','Y'))
		  *sp += 1;
		else
			*sp = 'Z';
	}}
	
	g_bfindmade = cc;
	
	if (!(cflag & 1))
		return null;

{	BUFFER *bp = (BUFFER *)mallocz(sizeof(BUFFER)+strlen(bname)+10); //can grow by 2
	if (bp != NULL)
	{	strcpy(bp->b_bname, bname);
//	bp->b_tabsize = 8;				/* Cannot be 0 */
	 	bp->b_fname = mallocz(1);
		bp->b_next = sb->b_next;    /* insert it */
		sb->b_next = bp;
																				/* and set up the other buffer fields */
	/*bp->b_flag |= BFACTIVE; ** very doubtful !!! */
	//bp->b_flag = g_gflag;
	//bp->b_color = g_bat_b_color;

// 	init_buf(bp);
		customise_buf(bp);
// 	bp->b_flag &= ~BFACTIVE;
	}
	return bp;
}}


#if 0

BUFFER * Pascal bufflkup(const char * filename)
{
//const char tagf[] = "/../../../../../tags";
  char fname[NFILEN+5*3+1];

  if (filename[0] == '^')
#if 1
	{	strcpy(strcpy(fname, TAGFNAME)+15, filename+1);
	{	int e = 15+3;
		while ((e -= 3) >= 0 && !fexist(fname+e))
			;
		if (e < 0)
    {	mlwrite(TEXT79);
    	return NULL;
    }

		filename = fname+e;
	}}
#else
	{	int totop = NFILEN+5*3 - 1;
		fname[totop] = 0;
	{ int sl = strlen(strpcpy(fname,filename+1,NFILEN));
		while (--sl >= 0 && 
					 fname[sl] != '/' && fname[sl] != '\\')
			fname[--totop] = fname[sl];

	{	Bool exist;
	  int eix = 5*3 + 1;

	  while (!(exist = fexist(fname)) && (eix -= 3) > 0)
	  {
				strcpy(fname+sl+1,TAGFNAME+eix);
				strcpy(fname+strlen(fname)-TAGFNLEN, fname+totop);
	  }

	  if (!exist)
    {	mlwrite(TEXT79);
    	return NULL;
    }

		filename = (const char *)fname;
	}}}
#endif
  return bufflink(filename, TRUE);
}

#endif


/*	Take a file name, and from it fabricate a buffer name. 
	This routine knows about the syntax of file names on the target system.
	I suppose that this information could be in table form.
	Returns a pointer into fname indicating the end of the file path;
	i.e. 1 character BEYOND the path name.
 */
static
int Pascal makename(char * bname, const char * fname)

{									/* Find the last directory entry name in the file name */
	int  cand = 0;
	int  six;
  char ch;

  for (six = 0; (ch = fname[six++]) != 0; )
#if	S_AMIGA
	  if (ch == ':' || ch == '/')
#elif	S_VMS
	  if (ch == ':' || ch == ']' || ch == '/')
#elif	S_MSDOS | S_FINDER
	  if (ch ==':' || ch == '\\' || ch =='/')
#else
	  if (ch == '/')
#endif
//    cand = s_cp;
	    cand = six;

			/* s_cp is pointing to the first real filename char */

#if	S_VMS
  (void)strpcpypfx(&bname[0], fname+cand, NBUFN, ';');
#else
  (void)strpcpy(&bname[0], fname+cand, NBUFN);
#endif
  return cand;
}


BUFFER * Pascal bufflink(const char * filename, int create)
												/* create: 1: create, 4: look up, MSD_DIRY=16: dont search, 
																	 32:dont stay, 64: swbuffer, 128 no_share */
{ BUFFER * bp_first = NULL;
  char fname_[NFILEN+5*3+4];
#define fname (fname_+2)
  char * fn;
  char fnup[NFILEN+5*3+1];

  if (filename[0] == '^' && (create & 4))
	{	strcpy(strcpy(fnup, TAGFNAME)+15, filename+1+(filename[1] == '/'));
	{	int e = 15+3;
		while ((e -= 3) >= 0 && !fexist(fnup+e))
			;
		if (e < 0)
    {	mlwrite(TEXT79);
    	return NULL;
    }

		filename = fnup+e;
	}}

#if NFILEN < 2 * NBUFN + 30
  error error
#endif
//fname_[0] = 'A';
//fname_[1] = 'B';
	g_bfindmade = FALSE;
	create |= (g_macargs > 0);
{ Fdcr_t fdcr = {NULL};
  char bname[NFILEN];
//#define text (&bname[NBUFN+1])
//int cr = create & ~(16+32+64);
  int srch = nmlze_fname(&fname[0], filename, bname) & ~(create & MSD_DIRY);

  if (srch > 0)
  { Cc cc = msd_init(fname, srch|MSD_REPEAT|MSD_HIDFILE|MSD_SYSFILE|MSD_IC/*|MSD_USEPATH*/);
  	if      (cc < OK)
  	{ mlwrite(TEXT79);
  		return NULL;
  	}
    else if (is_opt('Z'))
    { clr_opt('Z');
    	srch = 0;
    { unsigned int newdate = 0;

      while ((fn = msd_nfile()) != NULL)
      {															// USE_DIR => newest file comes last
#if S_WIN32 == 0
        if (newdate >= (unsigned)msd_stat.st_mtime)
        	continue;
        newdate = (unsigned)msd_stat.st_mtime;
#endif
        strpcpy(fname,fn,NFILEN);
      }
    }}
  }

  while ((fn = srch == 0 ? fname : 
  						 srch < 0  ? searchfile(fname, &fdcr) :
													 msd_nfile()) != NULL)
  { BUFFER * bp;
    for (bp = bheadp; bp != NULL; bp = bp->b_next)
      if ((bp->b_flag & BFINVS)==0 &&
         /*bp->b_fname != null && */ strcmp(fn, bp->b_fname) == 0)
				break;

    if (bp == NULL)
    { makename(bname, fn); 		/* New buffer name.	*/

      while (TRUE)
      { bp = bfind(bname, create);
				if (bp == NULL)
				{ if (create & 1)
				  { mlwrite(TEXT137);
                  /* "Cannot create buffer" */
				    return null;
				  }
	  			create |= 3;
				} 
				else
      	{	create |= 3;
				  if (bp->b_fname[0] == 0 || strcmp(bp->b_fname, fn) == 0)
	          break;
				}
	    }
	    
//    bp->b_flag &= ~BFACTIVE;
	    repl_bfname(bp, fn);
//    customise_buf(bp);
	  }
	  if (bp_first == NULL)
	    bp_first = bp;

	  if (!srch)
	    break;
  } /* while */

	if (create & 64)
		swbuffer(bp_first);

  return bp_first;
}}

#undef fname

static
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

	return NULL;
}

static 
BUFFER * nextbuf(int n) /*switch to next buffer in buffer list*/

{ int cc = lastbuffer(0, -n);
	if (cc > 0)
		return NULL/*curbp*/;

{	BUFFER * bp = getdefb();
	swbuffer(bp);

	return bp;
}}


int Pascal nextbuffer(int f, int n)

{ return nextbuf(n) != NULL;
}


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

#if 0

void check_buffers()

{ BUFFER *bp;
	for (bp = bheadp; bp != NULL; bp = bp->b_next)
	{ int sl = strlen(bp->b_bname);
	  char * stt = &  bp->b_bname[sl];
	  int i = 4;
	  int j = -1;
	  while (--i >= 0)
	  { if (stt[i] != 0)
	  		j = i;
	  }
	  
	  if (j >= 0)
	  { mbwrite("Corruption");
	  	mbwrite(stt+j);
	  }
	}
}

#endif


int g_top_luct;

int Pascal lastbuffer(int f, int n)   /* switch to previously used buffers */

{	short sign = n <= 0 ? -1 : 1;
  short thislu = curbp->b_luct * sign;
	int count = 0;

#if _DEBUG
  scan_buf_lu();
#endif

	while (1)
	{	short toplu = 0;
  	BUFFER * bestbp = NULL;
  	BUFFER *bp;
		for (bp = bheadp; bp != NULL; bp = bp->b_next)
			if ((bp->b_flag & BFINVS) == 0)
			{	if (f >= 0)
					count += 1 + ((bp->b_flag & BFCHG) << 14);
				else
				{	if (bp->b_flag & BFCHG)
					{	bestbp = bp;
						goto sw;
					}
				}
	
			{ int lu = bp->b_luct * sign;
				if (lu == 0)
					continue;
	
	      if (lu < thislu && lu > toplu)
	      {	toplu = lu;
	       	bestbp = bp;
	      }
	    }}
	
		if (n == 0)
			return count;
	
		if (sign > 0 && toplu == 0 && count > 1 && thislu != 32000)
		{	thislu = 32000;
			continue;
		}
	
		pd_winnew = 0;
	
		if (toplu == 0)
		  return 0;

sw:
	{	int lu = bestbp->b_luct;
		swbuffer(bestbp);
	 	bestbp->b_luct = lu;
	
//  if (n > 0 && remain == 0)
//		mbwrite(TEXT86);

		return 1;
	}}
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


int Pascal USE_FAST_CALL swbuffer(BUFFER * bp) /* make buffer BP current */
	
{	if (bp == NULL)
		return ABORT;
		 /* let a user macro get hold of things...if he wants */
	execwrap(4);	 // exbhook

	curbp = bp; 			/* Switch. */
	bp->b_luct = ++g_top_luct;

{	int active = bp->b_flag & BFACTIVE;
	if (!active)
		customise_buf(bp);	/* recustomise after emacs.rc */

	g_clring = (bp->b_langprops & (BCCOMT+BCPRL+BCFOR+BCSQL+BCPAS+BCML));		

	if (!active)
	{	bp->b_flag |= BFACTIVE;
//  bp->b_color = g_bat_b_color;

	{ char * fn = bp->b_fname;
		if (fn[0])
		{
			readin(fn, 0);
#if S_WIN32 && 0
			setconsoletitle(fn);
#endif
		}
	}}

{ WINDOW * cwp = leavewind(0, NULL);
	cwp->w_bufp	= bp;
	*(WUFFER*)cwp = *(WUFFER*)bp;

	setcline();							
	execwrap(5); // bufhook      let a user macro get hold of things
	
	return upwind(TRUE);
}}}


/* Attach a buffer to a window. 
 */
int Pascal usebuffer(int f, int n)

{	BUFFER * bp = getcbuf(TRUE, getdefb(), TEXT24);
							/* "Use buffer" */

	return swbuffer(bp);
}


/*
 * This routine blows away all of the text in a buffer.
 * If the buffer is marked as changed then we ask if it is ok;
 * The window chain is nearly always wrong if this gets called; 
 * the caller must arrange for the updates that are required. 
 * Return TRUE if everything looks good.
 */
int Pascal bclear(BUFFER * bp)
	
{ LINE * 	lp;
	LINE * 	nlp;
	int 	 s;

	if ((bp->b_flag & (BFINVS+BFCHG)) == BFCHG /* Not scratch buffer.  */
							 /* Something changed 	 */
	 && (s = mlyesno(TEXT32)) <= 0)
/*					"Discard changes" */
		return s;

	for (nlp = lforw(&bp->b_baseline); 
			!l_is_hd((lp = nlp)); )
	{ nlp = lforw(lp);
		free((char*)lp);
	}

/*loglog1("Rebaseline %x", lp); */

  init_buf(bp);

{ WINDOW * wp;
	for (wp = wheadp; wp != NULL; wp = wp->w_next)
		if (wp->w_bufp == bp)
		{	wp->w_dotp = &bp->b_baseline;
			memset(&wp->w_doto, 0, sizeof(int)*2 + sizeof(wp->mrks));	
		}

	return TRUE;
}}



int Pascal zotbuf(BUFFER * bp)	/* kill the buffer pointed to by bp */

{	extern BUFFER * g_dobuf;

	if (window_ct(bp) != NULL || bp == g_dobuf)					/* Error if on screen.	*/
	{ mlwrite(TEXT28);
					/* "Buffer displayed" */
		return FALSE;
	}

{	int s = bclear(bp);
	if (s <= FALSE)				/* Blow text away.	*/
		return s;
		
{ BUFFER *bp1 = prevele(1, bp);	/* Find the header. */

	if (bp1 == NULL)			/* Unlink it. */
		*g_bheadp_ref = bp->b_next;
	else
		bp1->b_next = bp->b_next;

#if DO_UNDO
	if (bp->b_undo)
		run_trim(bp, 0);
#endif

	free(bp->b_remote);
	free(bp->b_fname);
	free((char *) bp);			/* Release buffer block */
	
	return TRUE;
}}}


/* Dispose of a buffer, by name.
 * Ask for the name. Look it up. Be silent on not found.
 * If the buffer is being displayed in two windows drop this window.
 * Otherwise move this window on to the last buffer or next buffer
 * to move of this buffer.
 * If this buffer is no longer in a window then
 * if the buffer has been changed ask for confirmation and remove it.
 * Bound to "C-X K".
 */
int Pascal dropbuffer(int f, int n)

{ if (n == 0)
	{	BUFFER *bp;
		for (bp = bheadp; bp != NULL; bp = bp->b_next)
			if ((bp->b_flag & (BFINVS+BFCHG)) == 0 && bp != curbp)
					zotbuf(bp);
		return OK;
	}

{	BUFFER * bp = getcbuf(FALSE, curbp, TEXT26);
				             /* "Kill buffer" */
	if (bp == NULL)
		return ABORT;

  window_ct(bp);
{	int ct = bp->b_window_ct;
	if (ct > 1)
	{	(void)delwind(0,0);
		return FALSE;
	}
	
	(void)lastbuffer(0,1);
{ BUFFER * nb = curbp;
	if (nb == bp)
		nb = nextbuf(1);
	
	if (nb == NULL /*|| nb == bp */)			/* fake deletion of last buffer */
		return TRUE;

	swbuffer(nb);
	return zotbuf(bp);
}}}}



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
	
{ static const char modecode[9] = "VWOa*/MCA"; /* letters for modes */
	int c = 9;
	int mask = MDASAVE << 1;

	while (--c >= 0)
		t[c] = mode & (mask >>= 1) ? modecode[c] : '.';
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
		 
{	openwindbuf("*List");

{	Char line[15+NUMMODES-2+80] = "Global Modes:  .........\n"
														 "Actual	Modes   Size  Buffer       File\n";
												/* build line to report global mode settings */
	fmt_modecodes(&line[15], g_gflag);

	if (linstr(line) == FALSE)
		return FALSE;

{ BUFFER * bp;
	Int avail = curwp->w_ntrows - 2;
																					/* output the list of buffers */
	for (bp = bheadp; bp != NULL; bp = bp->b_next) 
	{ if ((bp->b_flag & BFINVS) && (iflag == 0))
			continue;

		fmt_modecodes(line, bp->b_flag);

		--avail;
	{ Int	nbytes = 0L;													/* Count bytes in buf.	*/
		LINE * lp;
		for (lp = &bp->b_baseline; !l_is_hd((lp=lforw(lp))); )
			nbytes += (Int)llength(lp)+1L;
																					/* we could restore lastmesg */
		line[9] = 0;
		mlwrite("%>%c%c%c %s %7d %15s %s\n",
						bp->b_flag & BFACTIVE ? '@' : ' ',
						bp->b_flag & BFCHG ? '*' : ' ',
						bp->b_flag & BFTRUNC ? '#' : ' ',
						line,
						nbytes,
						bp->b_bname,
						bp->b_fname);
	}}

//if (avail > 0)
		shrinkwind(0, avail);

	return mkdes();
}}}


#if 0

Pascal unmark(int f, int n) /* unmark the current buffers change flag */

{	curbp->b_flag &= ~BFCHG;		/* unmark the buffer */
	upmode(); 		/* unmark all windows as well */
	return TRUE;
}

#endif
