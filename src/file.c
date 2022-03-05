/*	FILE.C:   for MicroEMACS

	The routines in this file handle the reading, writing
	and lookup of disk files.  All of details about the
	reading and writing of the disk are in "fileio.c".
*/
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<fcntl.h>
#include	<sys/types.h>
#include	<sys/stat.h>
//#include	<io.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"base.h"
#include	"msdir.h"
#include	"logmsg.h"

#if S_WIN32 == 0
#include	<unistd.h>
#endif

extern int    g_crlfflag;
extern char * g_fline;					/* from fileio.c */

/*extern int sp_langprops;			** inherited */

/*##############  Filename utilities ##############*/

/*	Take a file name, and from it fabricate a buffer name. 
	This routine knows about the syntax of file names on the target system.
	I suppose that this information could be in table form.
	Returns a pointer into fname indicating the end of the file path;
	i.e. 1 character BEYOND the path name.
 */
int Pascal makename(char * bname, const char * fname)

{									/* Find the last directory entry name in the file name */
	const char *s_cp;
  const char *cand = &fname[0]; 
  char  ch;

  for (s_cp = cand; (ch = *s_cp++) != 0 && *s_cp != 0; )
#if	S_AMIGA
	  if (ch == ':' || ch == '/')
#elif	S_VMS
	  if (ch == ':' || ch == ']' || ch == '/')
#elif	S_MSDOS | S_FINDER
	  if (ch ==':' || ch == '\\' || ch =='/')
#else
	  if (ch == '/')
#endif
	    cand = s_cp;

			/* s_cp is pointing to the first real filename char */
#if	S_VMS
  (void)strpcpypfx(&bname[0], cand, NBUFN, ';');
#else
  (void)strpcpy(&bname[0], cand, NBUFN);
#endif
  return cand - fname;
}

/* Read a file into the current buffer.
 * Bound to "C-X C-R".
 */
int Pascal fileread(int f, int n)

{                       /* "Read file" */
  return readin(NULL, FILE_LOCK+FILE_REST);
}


/* Insert a file into the current buffer.
 * Bound to "C-X C-I".
 */
int Pascal insfile(int f, int n)

{ return rdonly() ? FALSE : 	
												/* "Insert file" */
  									readin(NULL, FILE_LOCK+FILE_REST+FILE_INS);
}

#if 0
const char nm[][4] = {"c","cpp", "cxx", "cs",	"h","pc","jav", "prl","pl",
											"for","fre","inc","pre","f", "sql","pas","md"};
const char fm[] 	 = {BCDEF,BCDEF,BCDEF,BCDEF,BCDEF,BCDEF,BCDEF,BPRL, BPRL,
                 			BCFOR,BCFOR,BCFOR,BCFOR,BCFOR,BCSQL,BCPAS, BCML};
#endif
const char * suftag  = "cpfqPm";

void Pascal customise_buf(BUFFER * bp)

{		int tabsize = pd_tabsize;
#if 0
		const char * pat = strlast(bp->b_bname,'.');
#else
	  const char * pat = NULL;
		const char * fn = bp->b_bname - 1;
    while (*++fn != 0)
      if (*fn == '.')
        pat = fn;
#endif
    if (pat != NULL)
    {  if (strcmp(".e2", pat) == 0)
      	bp->b_mode |= BCRYPT2;

			if (pd_file_prof != NULL)
    	{	
    		char * pr = pd_file_prof - 1;
        while (*++pr != 0)
        { if (*pr != '.') continue;
        
        { const char * p = pat;

          while (*++p != 0 && *++pr == *p)
            ;

          if (*p != 0 || pr[1] != '=') continue;

				{	int diff = pr[2] == '^';
					bp->b_flag &= ~ MDIGCASE;
					bp->b_flag |= diff * MDIGCASE;
          pr += diff;
        { int six;
          for (six = 6; --six >= 0 && suftag[six] != pr[2]; )
          	;
          if (six >= 0)
          { bp->b_langprops = (1 << six);
            ++pr;
				  }
          
          tabsize = atoi(pr+2);
          break;
        }}}}
      }
		}
    bp->b_tabsize = tabsize;
}



BUFFER * Pascal bufflink(const char * filename, int create)
												/* create: 1: create, MSD_DIRY=16: dont search, 
																	 32:dont stay, 64: swbuffer, 128 no_share */
{ BUFFER * bp_first = NULL;
  char fname[NFILEN];
  char * fn;
#if 0
  if 			(create & 32)
    ;
  else if (create & 16)
  { for (; *fn != 0; ++fn)
      if (*fn == '/')
      { *fn = 0;
        pat = fn+1;
      }
    fn = fname;
    if (pat == null)
    { pat = fn;
      fn = "./";
    }
  }
#endif

#if NFILEN < 2 * NBUFN + 30
  error error
#endif
{ int fno;
	FILE * ip = 0;
	char pipefn[NFILEN] = "";
  char bname[NFILEN];
#define text (&bname[NBUFN+1])
	int cr = create & ~(16+32+64);
  int srch = nmlze_fname(&fname[0], filename, bname) & ~(create & MSD_DIRY);

  if 		  (srch > 0)
  { msd_init(fname, NULL, srch|MSD_REPEAT|MSD_HIDFILE|MSD_SYSFILE|MSD_IC);
    if (is_opt('Z'))
    { unsigned int newdate = 0;

      while ((fn = msd_nfile(&fno)) != NULL)
      { if (newdate < (unsigned)msd_stat.st_mtime)
        { newdate = (unsigned)msd_stat.st_mtime;
          strpcpy(fname,fn,NFILEN);
          srch = 0;
        }
      }
    }
  }

  while ((fn = srch == 0 ? fname : 
  						 srch < 0  ? searchfile(fname, pipefn, &ip) :
													 msd_nfile(&fno)) != NULL)
  { BUFFER * bp;
    for (bp = bheadp; bp != NULL; bp = bp->b_next)
      if ((bp->b_flag & BFINVS)==0 &&
          bp->b_fname != null && strcmp(fn, bp->b_fname) == 0)
				break;

    if (bp == NULL)
    { makename(bname, fn); 		/* New buffer name.	*/

      while (TRUE)
      { bp = bfind(bname, cr, 0);
      	cr |= 2;
				if (bp == NULL)
				{ if (cr & 1)
				  { mlwrite(TEXT137);
                  /* "Cannot create buffer" */
				    return null;
				  }
	  			cr |= 1;
	  			continue;
				} 
				else
				{ // if (cr)
				  { if (bp->b_fname == null || strcmp(bp->b_fname, fn) == 0)
	            break;
	        }
#if 0
				  else				/* old buffer name conflict code */
				  { int cc = mlreply(concat(&text[0], TEXT136, bname, "):", null),
								     							  bname, NBUFN);
                                            /* "Buffer (" */
				    if (cc < 0) 		  /* ^G to just quit	*/
				      return bp_first;
				    if (cc != FALSE) 		  /* CR to clobber it	*/
				      continue;

				    makename(bname, fn);	/* restore it */
				    cr |= 1;		    			/* It already exists but this causes */
				  }         				      /* a quit the next time around	     */
#endif
				}
	    }
	    
	    bp->b_flag &= ~BFACTIVE;
	    repl_bfname(bp, fn);
	    customise_buf(bp);
	  }
	  if (bp_first == NULL)
	    bp_first = bp;

	  if (!srch)
	    break;
  }

	if (create & 64)
		swbuffer(bp_first);

  return bp_first;
}}



static
BUFFER * Pascal bufflkup(const char * filename, int create)
                              /* create: 1:create, 16:search, 32: dont stay */
{ 
//const char tagf[] = "/../../../../../tags";
  char fname[NFILEN+5*3];

  if (filename[0] == '^')
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

  return bufflink(filename,create);
}

/* Select a file for editing.
 * Look around to see if you can find it another buffer; 
 * if you can find it just switch to the buffer; 
 * if you cannot do so, create a new buffer, read in the text
 * and switch to the new buffer.
 * Bound to C-X C-F.
 */
int Pascal filefind(int f, int n)

{	if (resterr())		/* don't allow this command if restricted */
		return FALSE;

{	char * s;
  char * fname = gtfilename(TEXT133);
                      	/* "Find file" */
	if	(fname == NULL || fname[0] <= ' ')
	  return FALSE;

	for (s = fname-1; *++s != 0																	// strip off :n
  					     && !((*s == ':' || *s == '(') && isdigit(s[1]))
                  ; )
		;
          
	if (*s == 0)
		s = NULL;
	else
		*s = 0;

{	BUFFER * bp = bufflkup(fname, (g_macargs > 0));
	if (bp == NULL)
	  return FALSE;

	if (bp->b_flag & BFACTIVE)
	  mlwrite(TEXT135);
					/* "[Old buffer]" */
	swbuffer(bp);
	
	return s == NULL ? TRUE : gotoline(1,atoi(s+1));
}}}


int Pascal viewfile(int f, int n)	/* visit a file in VIEW mode */

{	int s = filefind(f, n); 	/* bug: > 2 files => some not view */
	if (s)
	{ curwp->w_bufp->b_flag |= MDVIEW;
	  upmode();
	}

	return s;
}

#if CRYPT == 0
# define resetkey(x)
#endif


BUFFER * Pascal gotfile(const char * fname)
					/* file name to find */
{ BUFFER *bp;
	for (bp = bheadp; bp != NULL; bp = bp->b_next)
	{ if (bp->b_flag & BFINVS)
			continue;

		if      (fname == NULL)
		{	if (bp->b_flag & BFCHG)
				return bp;
		}
		else if (bp->b_fname != null)
	  { 
#if S_MSDOS | S_OS2
																							/* msdos isn't case sensitive */
			const char * t = strmatch(fname, bp->b_fname);
	  	if (*t == 0 && bad_strmatch == 0)
#else
			if (strcmp(fname, bp->b_fname) == 0)
#endif
		  {//mlwrite(TEXT135);
            /* "[Old buffer]" */
				return bp;
			}
	  }
	}
	  
	return 0;
}


const char * gettmpfn()

{	const  char * td = (char *)getenv("TEMP");
	if (td == NULL)
#if (defined _DOS) || (defined _WIN32)
						/* the C drive : better than ./ as ./ could be on a CD-Rom etc */
		td = "c:" ;
#else
		td = "." ;
#endif
	return td;
}


static 
BUFFER * get_remote(int props, BUFFER * bp, const char * pw, const char * cmdbody)
								//Bool inherit_props, Bool popup
{ Cc cc;
	char ch;
  char fullcmd[3*NFILEN+1];

	const char * cmdnm = gtusr("scp");
  if (cmdnm == NULL)
    cmdnm = PSCP_CMD;			// "c:\\bbin\\pscp.exe ";

{	const char * rnm = &cmdbody[strlen(cmdbody)];
	while ((ch = *--rnm) != '/' && ch != ':')	// Go back to slash or :
		if (rnm == cmdbody) 
	    return NULL;

  --g_macargs;
  ++pd_discmd;
{ int o_disinp = g_disinp;

  g_disinp = FALSE;

  cc = 1;
  if (pw != NULL)
  	strcpy(fullcmd, pw);
  else
  	cc = mltreply(TEXT101, fullcmd, 40);

  if (cc != 0)
    mlwrite(TEXT139);	/* "Reading file" */

{ char * cmd = fullcmd + strlen(fullcmd) + 1;

  --pd_discmd;
  ++g_macargs;
  g_disinp = o_disinp;
  if (cc == 0)
    return NULL;

{	int clen = strlen(concat(cmd, cmdnm, /*pw,*/" ", cmdbody," ",0));
	const char * tmp = gettmpfn();

  cc = ttsystem(strcat(strcat(strcat(cmd,tmp),"/"),rnm+1), fullcmd);
  if (cc != OK)
	{	props |= Q_POPUP;
		concat(cmd, cmdnm," Fetch failed ",int_asc(cc), 0);
	}
  else
	{//	sprintf(diag_p, "EKEY %x %s", curbp, ekey == NULL ? "()" : ekey);
	 //	mbwrite(diag_p);
	
		if (bp == NULL)
			bp = bufflink(cmd+clen, (g_macargs > 0));
		if (bp != NULL)
		{ cmd[-1] = 1;
		  bp->b_remote = strdup(fullcmd);											  // allow leak
	    bp->b_key = props & Q_INHERIT ? curbp->b_key : NULL;
	    if (bp->b_key != NULL)
    		bp->b_flag |= MDCRYPT;
	  }

//	memset(&fullcmd[strlen(cmdnm)],'*', pwlen);
	}
  if (props & Q_POPUP)
		mbwrite(cmd);

  memset(fullcmd, 0, sizeof(fullcmd));
	return bp;
}}}}}



extern
void io_message(const char * txt, int cc, int nline)

{ if (cc > FIOEOF)
  { txt = cc == FIOMEM ? TEXT99 : TEXT141;
			/* "OUT OF MEMORY, " */
			/* "I/O ERROR, " */
    curbp->b_flag |= BFTRUNC;
  }

  mlwrite("[%s%d%s%s", txt, nline,  TEXT143,nline == 1 ? "]" : "s]");
}


/*	Read file "fname" into the curbp, blowing away any text
  	found there.  Called by both the read and find commands.
  	Return the final status of the read.  Also called by the mainline,
  	to read in a file specified on the command line as an argument. 
  	The command in $readhook is called after the buffer is set up
  	and before it is read. 
*/
/*Int readin_lines;			** aux result */

int Pascal readin(char const * fname, int props)
										/* name of file to read */
									 	/* check for file locks?, -ve => insert */
{	int ins = props & FILE_INS;

  if ((props & FILE_REST) && resterr())
  	return FALSE;

	if (fname == NULL)
	{ fname = (char const*)gtfilename(ins ? TEXT132 : TEXT131);
    if (fname == NULL)
  		return FALSE;
	}

#if	FILOCK
	if ((props & FILE_LOCK) &&  lockchk(fname) < 0)
	  return ABORT;
#endif

#if S_MSDOS && S_WIN32 == 0
{	char spareli[257];
	fname = LFN_to_8dot3(LFN_to_83, 0, fname, &spareli[0]);
}
#endif
{ char fnbuff[NFILEN+1];
  int diry = FALSE;
	Cc cc;

	const char * s = fname-1;
  const char * rname = NULL;
  int got_at = FALSE;
  while (*++s != 0)
  { if (*s == '@')
      got_at = TRUE;
    if (*s == '/')
      rname = s;
  }

{ BUFFER * bp = curbp;
  cc = FIOSUC;
  if (!got_at)
  { if (s[-1] == '/')                           /* remove trailing slash */
	   fname = strpcpy(fnbuff, fname, s - fname);
	}
	else
  { BUFFER * tbp = get_remote(Q_POPUP, bp, NULL, fname);
    cc = tbp == NULL ? -1 : FIOSUC;
    if (cc == FIOSUC) 
    { char ch;
    	char * s = tbp->b_remote;
      if (s == NULL || *s == 0)
        cc = -1;

      while ((ch = *++s) != 0 && ch != 1)
      	;
      while ((ch = *++s) != 0)
        if (ch == ' ')
          fname = s + 1;
      fname = strdup(fname);     					// allow leak
//    cryptremote(tbp->b_remote);
    }
  }

  if (cc == FIOSUC)
    cc = ffropen(fname);

	if (cc != FIOSUC)			/* File not found. */
	{ 
#if S_MSDOS
	  diry = name_mode(fname) & 040000;
	  if (diry != 0)
	    cc = OK;
	  else
	    cc = 1;
#endif
	}

	if ((props & FILE_NMSG) == 0)
														/* read the file in */
		mlwrite(cc < 0 ? TEXT152 :
		        cc > 0 ? TEXT138 :
    	    	ins    ? TEXT153 : TEXT139);
/*		        "[No such file]" */
/*			      "[New file]" */
/*            "[Inserting file]" */
/*            "[Reading file]" */
  if (cc < 0)
    return FALSE;

  if (!ins)
	{ Cc rc = bclear(bp);
  	if (rc <= FALSE)			/* Changes not discarded */
  	  return rc;

  	bp->b_flag &= ~(BFINVS|BFCHG);
  	fname = repl_bfname(bp, fname);
  	if (fname == null)
  	  return ABORT;

  			/* let a user macro get hold of things...if he wants */
  	execkey(&readhook, FALSE, 1);
  }
  
  if (cc != 0)
    goto out;

  init_paren("(",0);
	
	if (ins)
	{ 				                          /* back up a line and save the mark here */
	/*if ((curwp->w_dotp->l_props & L_IS_HD) == 0) */
	  { curwp->w_dotp    = lback(curwp->w_dotp);
	    curwp->w_line_no -= 1;
	  }
	  curwp->w_doto    = 0;
    g_paren.in_mode |= curwp->w_dotp->l_props & Q_IN_CMT;
	/*curwp->mrks.c[0].markp = lback(curwp->w_dotp);
	  curwp->mrks.c[0].marko = 0;*/
  }
{	int   len;
	int   fno;
 	int   nline = 0;
 	char * sfline = NULL;
  LINE * nextline = ins == 0 ? &bp->b_baseline : lforw(curwp->w_dotp);
#if S_MSDOS == 0
	diry = ffisdiry();
#elif S_WIN32 == 0
  char spareline[257];
#endif
	if (diry)
	{ msd_init(fname, "", MSD_DIRY | MSD_REPEAT | MSD_STAY | MSD_HIDFILE | MSD_SYSFILE);
	  bp->b_flag |= MDDIR;
	}

	cc = FIOSUC;
//sp_langprops = bp->b_langprops & BCCOMT;
	while (cc == FIOSUC)
  { char * ln;
  	LINE *lp1;
	  if (diry)
	  { ln = msd_nfile(&fno);
	    if (ln == NULL)
	      break;
	     
	    /*mlreply(fline, spareline+200, 60);*/
#if S_MSDOS && S_WIN32 == 0
	    ln  = LFN_to_8dot3(LFN_from_83, 1, ln, &spareline[0]);
#endif
			if (ln[0] == '.' && ln[1] == '/')
				ln += 2;
	    len = strlen(ln);
	  }
	  else 
	  { cc = ffgetline(&len);
	  	if (cc != FIOSUC && len == 0)
	  		break;
	  	ln = g_fline;
#if 0
      if ((g_paren.in_mode & 0x3f) && len > 2)
        ln[0] = 'A' + (g_paren.in_mode & 0x3f);
#endif
	  }
	  
	  lp1 = mk_line(ln,len,len);
	  if (lp1 == NULL
#if 0
	       || (nline & 0x3ff) == 0 && typahead() && (getkey() & CTRL)
#endif
	     )
	  { cc = FIOMEM;
	    (void)repl_bfname(bp, "SHORT");
	    break;
	  }

    lp1->l_props = g_paren.in_mode & Q_IN_CMT;
	  ibefore(nextline, lp1);
	  ++nline;

    scan_par_line(lp1);
	}
//curwp->w_dotp = topline;
	if (g_crlfflag)
	  bp->b_flag |= MDMS;
#if 0
	if (!g_fline)
	{ free(g_fline);
  	g_fline = NULL;
	}
#endif

{	extern FILE *g_ffp;		/* File pointer, all functions. */
	if (g_ffp)
	  (void)fclose(g_ffp);

	if ((props & FILE_NMSG) == 0)
		io_message(ins >= 0 ? TEXT140 : TEXT154, cc, nline);
																	/* "Read 999 line" */
}}
out:
//readin_lines = nline;
	bp->b_doto = 0;

	if (ins)
	{    /* advance to the next line and mark the window for changes */
	/*curwp->mrks.c[0].markp = lforw(curwp->mrks.c[0].markp);
	  bp->mrks = curwp->mrks;*/
/*	curwp->w_dotp = lforw(curwp->w_dotp); */
	  bp->b_flag |= BFCHG;			/* we have changed	*/
	  bp->b_flag &= ~BFINVS;		/* and are not temporary*/
	  bp->b_wlinep = curwp->w_linep;
	  bp->b_dotp = curwp->w_dotp;
	/*bp->b_fcol = curwp->w_fcol;*/
	  curwp->w_flag |= WFHARD | WFMODE;
	}
	else
	{ WINDOW * wp;
	  tcapkopen();								/* open the keyboard again (Unix only) */
//  swb_luct = topluct() + 1;
//  bp->b_luct = swb_luct;
	  bp->b_dotp = lforw(&bp->b_baseline);
	  bp->b_wlinep = bp->b_dotp;

	  for (wp = wheadp; wp != NULL; wp=wp->w_next)
	    if (wp->w_bufp == bp)
	      openwind(wp);
#if S_MSDOS == 0
		if (diry)
		{ filter(-1,0);
			bp->b_flag &= ~BFCHG;
		}
#endif
  }

{	LINE * lp;
 	int clamp = 3;

#if	CRYPT
	if (bp->b_flag & MDCRYPT)
	{
#if S_WIN32
		MySetCoMo();
#endif
	 	resetkey(&bp->b_key);													/* set up for decryption */
		clamp = 0;
	}
#endif

	for (lp = &bp->b_baseline;
			 ((lp=lforw(lp))->l_props & L_IS_HD) == 0 && --clamp != 0; )
	{
#if	CRYPT
		if (clamp < 0)
		{	int len = lp->l_used;
			ucrypt(lp->l_text, len);
			if (bp->b_mode & BCRYPT2)
				double_crypt(lp->l_text, len);
		}
#endif
	
		if (lp->l_used > 6 &&(lp->l_text[0] == '/' && lp->l_text[1] == '*' ||
                          lp->l_text[0] == '/' && lp->l_text[1] == '/' ||
                          lp->l_text[0] == '-' && lp->l_text[1] == '-')
									     && lp->l_text[2] == 't' && lp->l_text[3] == 'a'
									     && lp->l_text[4] == 'b' && lp->l_text[5] == ' ')
	  { int tabw = atoi(lp->l_text+6);
	    if (tabw != 0)
	      bp->b_tabsize = tabw;
	  }
	}

	return cc <= FIOEOF;	/* False if error.	*/
}}}}



/* Ask for a file name, and write the
 * contents of the current buffer to that file.
 * Update the remembered file name and clear the
 * buffer changed flag. This handling of file names
 * is different from the earlier versions, and
 * is more compatable with Gosling EMACS than
 * with ITS EMACS. Bound to "C-X C-W".
 */
int Pascal filewrite(int f, int n)

{	int cc;
	char fname[NFILEN];

	if (resterr())		/* don't allow this command if restricted */
		return FALSE;

	if ((cc = mlreply(TEXT144, fname, NFILEN)) <= FALSE)
/*		       "Write file: " */
		return cc;

	cc = writeout(fname);
	if (cc > FALSE)
	{	repl_bfname(curbp, fname);
		curbp->b_flag &= ~BFCHG;
		upmode();			/* Update mode lines.	*/
	}
	return cc;
}


/* Save the contents of the current buffer in its associated file. 
 * Do nothing if nothing has changed.
 * Error if there is no remembered file name for the buffer. 
 * Bound to "C-X C-S". May get called by "C-Z".
 */
int Pascal filesave(int f, int n)

{	// if (rdonly())
//		return FALSE;

{	BUFFER * bp = curbp;
	if ((bp->b_flag & BFCHG) == 0)	/* Return, no changes.	*/
		return TRUE;

    					/* complain about truncated files */
  if (bp->b_flag & (BFTRUNC+BFNAROW))
  { char * txt = (bp->b_flag & BFTRUNC) ? TEXT146 :
																			/* "Truncated file..write it out" */
	    																	  TEXT147;
																			/* "Narrowed Buffer..write it out" */
  	if (mlyesno(txt) <= 0)
			return ctrlg(-1,-1);			/* "[Aborted]" */
  }

  if (bp->b_fname == null)		/* Must have a name.	*/
	{	mlwrite(TEXT145);
					/* "No file name" */
		return FALSE;
	}
{	char * fn = bp->b_fname;
	char * cmd = bp->b_remote;
  if (cmd != NULL)
	{	fn = cmd;
	{	int sl = strlen(fn);
    if (sl == 0)
	  { mlwrite(TEXT102);
	  	return FALSE;
	  }
//		cryptremote(fn);									// read it

#if _DEBUG
    mbwrite(fn);
#endif
    for (fn = &fn[sl]; *--fn != ' '; )
      ;
    ++fn;
  }}

{	int rc = writeout(fn);
	if (rc > FALSE)
	{ bp->b_flag &= ~BFCHG;
	  upmode();		/* Update mode lines.	*/

	  if (cmd != NULL)
		{	fn[-1] = 0;																				// terminate 1st name
  
		{	char * ss = fn;																	// file precedes it
		  while (--ss > cmd && *ss != ' ')
		    ;
		  *ss = 0;																					// terminate cmd
		    
		{	char * pw = cmd;
			--cmd;
	    while (*++cmd != 0 && *cmd != 1)
	    	;
	    *cmd++ = 0;
		    	
//		mbwrite(cmd+3*NFILEN);                  					// Security concerns
			g_disinp -= 7; 							/* turn command input echo off */
				
		{ char cmd_[3*NFILEN+1];
			Cc cc = ttsystem(concat(cmd_,cmd, " ",fn," ",ss+1,NULL),pw);
		  if (cc != 0)
		  { mlwrite(TEXT155" %d", cc);
		  	rc = FALSE;
		  }
			g_disinp += 7; 							/* turn command input echo off */
		  memset(bp->b_remote, 0, cmd - pw);
//	  pd_sgarbf = TRUE;
		}}}}
	}
//  cryptremote(bp);							// reencrypt it
	return rc;
}}}}

#define MYUMASK 0644


#ifdef __S_IWRITE
#if __S_IWRITE != 0200
  error here
#endif
#endif

/* This function performs the details of file writing. It uses
 * the file management routines in the "fileio.c" package. The
 * number of lines written is displayed. Several errors are
 * posible, and cause writeout to return a FALSE result. When
 * $ssave is TRUE,  the buffer is written out to a temporary
 * file, and then the old file is unlinked and the temporary
 * renamed to the original name.  Before the file is written,
 * a user specifyable routine (in $writehook) can be run.
 */

int Pascal writeout(const char * fn)
				/* name of file to write current buffer to */
{ Cc cc;
  struct stat stat_;
  int caution;
  LINE *lp;
  int nline;	  /* number of lines written */
  int sp;		    /* offset into tname */
	char tname[NSTRING];	/* temporary file name */
	BUFFER * bp = curbp;

	if (bp->b_mode & BCRYPT2)
	{ mlwrite(TEXT223);
		return FALSE;
	}
				        /* let a user macro get hold of things */
	execkey(&writehook, FALSE, 1);
          				 
{ FILE * op;                 	/* determine if we will use the save method */
#if S_VMS
  caution = 0;
#elif S_MSDOS
  caution = ssave == 0 ? 0 : name_mode(fn);
	if (caution & 0200)
	 	caution = 0;
#else
	cc = stat(fn , &stat_);
{ int w = cc < 0 ? TRUE :
	             (stat_.st_mode & (getuid() == stat_.st_uid ? 0200 :
	                               getgid() == stat_.st_gid ? 020  : 02
	                               ));
	caution = ssave && cc >= 0 || !w;
	if (caution && stat_.st_nlink > 1 || !w)
	{ caution = mlyesno(w ? TEXT218 : TEXT221);
	  if (caution < 0)
  	  return caution;
	}
}
#endif
	if (bp->b_flag & MDCRYPT)
		resetkey(&bp->b_key);
    						   /* Perform Safe Save..... */
              		 /* duplicate original file name, and find where to trunc it */
	sp = makename(tname, fn);
	strpcpy(tname, fn, NSTRING-30);		/* overwrite the makename */
	for (nline = 4; --nline >= 0; )/* mk unique name using random numbers */
	{ if (caution)
			strcpy(&tname[sp+1], int_asc(ernd()));
		op = ffwopen(caution, tname);
		if (op != NULL)
			break;
	}

	if (op == NULL)									/* if the open failed.. clean up and abort */
		mlwrite(TEXT155);		/*	"Cannot open file for writing" */
	else
	{	mlwrite(TEXT148);
					/* "[Writing...]" */

		g_crlfflag = bp->b_flag & MDMS;
												/* write the current buffer's lines to the disk file */
		cc = OK;
		nline = 0;					/* track the Number of lines		*/
		for (lp = &bp->b_baseline;
				((lp = lforw(lp))->l_props & L_IS_HD) == 0; )
		{ if ((cc = ffputline(op, &lp->l_text[0], llength(lp))) != FIOSUC)
				break;
			++nline;
		}
			
#if S_MSDOS & CTRLZ
		putc('Z'-'@', ffp);		/* add a ^Z at the end of the file */
#endif

		fclose(op);
		
	{ extern char deltaf[];
		#define mesg deltaf 
		mesg[0] = 0;		/* message buffer */
 
#if S_VMS == 0
		if (caution)					/* erase original file */
		{ int cc = unlink(fn);
			if (cc == 0 && rename(tname, fn) == 0)
			{ if (! S_MSDOS && stat_.st_mode != MYUMASK)
					chmod(fn, stat_.st_mode & 07777);
			}
			else
			{ concat(&mesg[0], TEXT150, tname, null);
												/* ", saved as " */
				cc = FIODEL;		/* failed */
			}
		}
#endif
																						 /* report on status of file write */
		io_message(strcat(&mesg[0], TEXT149), FIOSUC, nline);
															/* "[Wrote 999 line" */
	}}
	tcapkopen();						/// reopen the keyboard (Unix only)
	return op != NULL;
}}


		/*	This command modifies the file name associated with the
			current buffer. It is like the "f" command in UNIX "ed".
			You can type a blank line at the prompt if you wish.
		*/
int Pascal filename(int f, int n)

{ char fname[NFILEN];

	if (resterr()) 	/* don't allow this command if restricted */
		return FALSE;
		
{ int rc = mlreply(TEXT151, fname, NFILEN);				/* "Name: " */
	if (rc < 0)
		return rc;

	curbp->b_flag &= ~MDVIEW; 		 /* no longer read only mode */

	repl_bfname(curbp, rc == FALSE ? "" : fname);

	upmode();       /* Update mode lines. */
	return TRUE;
}}



int Pascal fetchfile(int f, int n)

{ LINE * lp = curwp->w_dotp;
	if (lp->l_text == NULL)
	  lp = lforw(lp);

	if (lp->l_text == NULL)
	{	mbwrite(TEXT203);       // "Nothing there"
	 	return FALSE;
	}

{	int len = lp->l_used > 2*NFILEN ? 2*NFILEN : lp->l_used;
	char cmdline[2*NFILEN+1];
	((char*)memcpy(cmdline, (char*)lp->l_text, len))[len] = 0;
	if (curbp->b_mode & BCRYPT2)
		double_crypt(cmdline, len);

//mbwrite(cmd);
	
{	int encrypt = Q_INHERIT;
	char * s = cmdline;
	while (*s != 0 && *s != '\t')					  // Go to first tab
		++s;
		
	*s = 0;
		
	while (*++s == '\t')										// skip extra tabs
		;
		
	if (*s == 0)
	{ mbwrite(TEXT222);       // "No Password";
		return FALSE;
	}

	*--s = 0;
	
{ char * pw = s+1;

	while (*++s != 0 && *s != '\t')					// skip to next tab
		;
  
  if (*s != 0)
  { *s = 0;
		if (s[1] != 0)
    	encrypt = 0;
  }

{ BUFFER * bp = get_remote(encrypt | f, NULL, pw, cmdline);

	memset(cmdline, 0, len);
  if (bp == NULL)
    return -1;
                          // If the fetch failed continue with any file from last time
  swbuffer(bp);

//sprintf(diag_p, "EKEY %x %s", tbp, tbp->b_key == NULL ? "()" : tbp->b_key);
//mbwrite(diag_p);

	return gotobob(0,0);
}}}}}
