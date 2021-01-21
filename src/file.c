/*	FILE.C:   for MicroEMACS

	The routines in this file handle the reading, writing
	and lookup of disk files.  All of details about the
	reading and writing of the disk are in "fileio.c".
*/
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif

#include	<stdio.h>
#include	<stdlib.h>
#include	<fcntl.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<io.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"base.h"
#include	"msdir.h"
#include	"logmsg.h"

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

  for (s_cp = cand; (ch = *s_cp++) != 0; )
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
  (void)strpcpypfx(&bname[0], cand, NBUFN, ';');
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

{ if (curbp->b_flag & MDVIEW)
    return rdonly();
/*				"Insert file" */
  return readin(NULL, FILE_LOCK+FILE_REST+FILE_INS);
}

void Pascal customise_buf(BUFFER * bp)

{   const char * pat = NULL;
    const char * fn = bp->b_fname - 1;
    while (*++fn != 0)
      if (*fn == '.')
        pat = fn;

    if (pat != NULL && g_file_prof != NULL)
    {  
        char * pr = g_file_prof - 1;
        while (*++pr != 0)
        { if (*pr != '.') continue;
        
        { const char * p = pat;

          while (*++p != 0 && *++pr == *p)
            ;

          if (*p != 0 || pr[1] != '=') continue;

          if (pr[2] == '^')
          { ++pr;
            bp->b_flag &= ~MDEXACT;
          }
        { int tabsz = atoi(pr+2);
          bp->b_tabsize = tabsz <= 0 ? 8 : tabsz;
          break;
        }}}
   }
}



BUFFER * Pascal bufflink(const char * filename, int create)
															/* create: 1:create, 16:search, 32:dont stay */
{ char bname[NFILEN];
#if NFILEN < 2 * NBUFN + 30
  error error
#endif
#define text (&bname[NBUFN+1])
  char fname[NFILEN];
  int srch = nmlze_fname(&fname[0], filename, bname);
  
{ BUFFER * firstbp = NULL;
  int fno;
  char * fn = fname;
  const char * pat = null;

  if 			(create & 32)
    ;
#if 0
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

  create &= ~(16+32);
  
  if (srch)
  { msd_init(fn, pat, srch|MSD_REPEAT|MSD_HIDFILE|MSD_SYSFILE|MSD_IC);
    if (g_newest)
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

  while ((fn = ! srch ? fname : msd_nfile(&fno)) != NULL)
  { BUFFER * bp;
#if S_MSDOS && S_WIN32 == 0
    if (srch & MSD_DIRY)
      fn = LFN_to_8dot3(LFN_from_83, 0, fn, &fname[0]);
#endif
    for (bp = bheadp; bp != NULL; bp = bp->b_bufp)
      if ((bp->b_flag & BFINVS)==0 &&
          bp->b_fname != null && strcmp(fn, bp->b_fname) == 0)
				break;

    if (bp == NULL)
    { makename(bname, fn); 		/* New buffer name.	*/

      while (TRUE)
      { bp = bfind(bname, create, 0);
				if (bp == NULL)
				{ if (create)
				  { mlwrite(TEXT137);
                  /* "Cannot create buffer" */
				    return null;
				  }
	  			create = TRUE;
	  			continue;
				} 
				else
				{ if      (create || g_nosharebuffs)
				  { if (bp->b_fname == null || strcmp(bp->b_fname, fn) == 0)
	            break;
	        }
				  else				/* old buffer name conflict code */
				  { int cc = mlreply(concat(&text[0], TEXT136, bname, "):", null),
								     							  bname, NBUFN);
                                            /* "Buffer (" */
				    if (cc == ABORT) 		  /* ^G to just quit	*/
				      return firstbp;
				    if (cc != FALSE) 		  /* CR to clobber it	*/
				      continue;

				    makename(bname, fn);	/* restore it */
				    create = TRUE;		    /* It already exists but this causes */
				  }         				      /* a quit the next time around	     */
				}
				unqname(bname);                     /* The file name differs */
	    }
	    
	    bp->b_flag &= ~BFACTIVE;
	    repl_bfname(bp, fn);
	    customise_buf(bp);
	  }
	  if (firstbp == NULL)
	    firstbp = bp;

	  if (!srch)
	    break;
  }

  return firstbp;
}}



static
 BUFFER * Pascal bufflkup(const char * filename, int create)
                              /* create: 1:create, 16:search, 32: dont stay */
{ BUFFER * res;
  int ups = 0;
  int relative = 0;
  char fname[NFILEN];

  if (filename[0] != '^')
    return bufflink(filename,create);

  strpcpy(fname,filename+1,NFILEN);

  if (fname[0] != '/' && fname[0] != '\\' && fname[1] != ':')
    relative = 1;

/*mbwrite2("Lk1:",fname);*/

  while ((res = bufflink(fname,create | 32)) == NULL)
  { if (relative) 
    { int i = ++ups;
      if (i > 5) break;
      
      fname[0] = 0;
      while (--i >= 0)
        strcat(fname,"../");
      strpcpy(fname+ups*3,filename+1,NFILEN-ups*3);
    }
    else
    { int i = strlen(fname);
      while (--i >= 0 && fname[i] != '/')
        ;
        
      if (i < 0) break;
    { int src = i;
      while (--i >= 0 && fname[i] != '/' && fname[i] != ':')
        ;
      if (i < 0) break;
      strcpy(fname+i+1,fname+src+1);
    }}
  /*mbwrite2("Lk2:",fname);*/
  }

  if (res == NULL)
    mlwrite(TEXT79);

  return res;
}

static char * g_ffmsg = TEXT133;

/* Select a file for editing.
 * Look around to see if you can find it another buffer; 
 * if you can find it just switch to the buffer; 
 * if you cannot do so, create a new buffer, read in the text
 * and switch to the new buffer.
 * Bound to C-X C-F.
 */
Pascal filefind(int f, int n)

{	if (restflag)		/* don't allow this command if restricted */
		return resterr();

{	char * s;
  char *fname = gtfilename(g_ffmsg);
                      /* "Find file" */
	if	(fname == NULL || fname[0] == 0)
	  return FALSE;

	for (s = fname-1; *++s != 0																	// strip off :n
  					     && !((*s == ':' || *s == '(') && isdigit(s[1]))
                  ; )
		;
          
	if (*s == 0)
		s = NULL;
	else
		*s = 0;

{	BUFFER * bp = bufflkup(fname, g_clexec);
	if (bp == NULL)
	  return FALSE;
	if (bp->b_flag & BFACTIVE)
	  mlwrite(TEXT135);
/*			       "[Old buffer]" */
	(void)swbuffer(bp);
	
	return s == NULL ? TRUE : gotoline(1,atoi(s+1));
}}}


int Pascal viewfile(int f, int n)	/* visit a file in VIEW mode */

{	char * sffmsg = g_ffmsg;
	g_ffmsg = TEXT134;
{	int s = filefind(f, n); 	/* bug: > 2 files => some not view */
	if (s)
	{ curwp->w_bufp->b_flag |= MDVIEW;
	  upmode();
	}
	g_ffmsg = sffmsg;
	return s;
}}

#if CRYPT == 0
# define resetkey(x)
#endif


BUFFER * Pascal gotfile(const char * fname)
					/* file name to find */
{ const int mode = S_MSDOS | S_OS2 ? 0x20 : -1;
												/* msdos isn't case sensitive */
	BUFFER *bp;

	for (bp = bheadp; bp != NULL; bp = bp->b_bufp)
	  if ((bp->b_flag & BFINVS) == 0 && bp->b_fname != null)
	  { const char * t = com_match(fname, bp->b_fname, mode);
	  	if (*t == 0 && bad_strmatch == 0)
		  {
//  		mlwrite(TEXT135);
            /* "[Old buffer]" */
				return bp;
			}
	  }
	  
	return 0;
}

static 
BUFFER * get_remote(BUFFER * bp_, const char * pw, const char * cmdbody,
                     Bool inherit_props, Bool popup)
{ int o_disinp = g_disinp;
  int o_discmd = g_discmd;
  int o_clexec = g_clexec;

  Cc cc;
  char fullcmd[3*NFILEN+1];

{ const char * cmdnm = gtusr("scp");

	const char * rnm = &cmdbody[strlen(cmdbody)];
	while (--rnm > cmdbody && *rnm != '/' && *rnm != ':')	// Go back to slash or :
		;

  if (rnm <= cmdbody)
    rnm = "Xremotefile";

  if (cmdnm == NULL || strcmp(cmdnm,"ERROR")==0)
    cmdnm = "c:\\bin\\pscp.exe ";

  g_clexec = FALSE;
  g_disinp = FALSE;
  g_discmd = TRUE;

  cc = 1;
  if (pw == NULL)
  { cc = mltreply(TEXT101, &fullcmd[3*NFILEN-40], 40);
    pw = &fullcmd[3*NFILEN-40];
  }

  if (cc != 0)
    mlwrite(TEXT139);

  g_disinp = o_disinp;
  g_discmd = o_discmd;
  g_clexec = o_clexec;
  if (cc == 0)
    return NULL;

{ int clen = strlen(concat(fullcmd, cmdnm, /*pw,*/" ", cmdbody," ",0));
	char * tmp = (char *)getenv("TEMP");
	if (tmp == NULL)
		tmp = "./";

  cc = ttsystem(strcat(strcat(strcat(fullcmd,tmp),"/"),rnm+1), pw);
  if (cc != OK)
	{ mbwrite((const char*)concat(fullcmd, cmdnm," Fetch failed ",int_asc(cc), 0));
    return NULL;
	}
    
//sprintf(diag_p, "EKEY %x %s", curbp, ekey == NULL ? "()" : ekey);
//mbwrite(diag_p);
	
{	BUFFER * bp = bp_ != NULL ? bp_ : bufflink(fullcmd+clen, g_clexec);
	if (bp != NULL)
  { 
  	bp->b_remote = strdup(fullcmd);								// allow leak
    bp->b_key = inherit_props ? curbp->b_key : NULL;
    if (bp->b_key != NULL)
    	bp->b_flag |= MDCRYPT;
  }

//memset(&fullcmd[strlen(cmdnm)],'*', pwlen);
  if (popup)
    mbwrite(fullcmd);

  memset(fullcmd, 0, sizeof(fullcmd));
	return bp;
}}}}



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
{ int ins = props & ~(FILE_REST + FILE_LOCK);

  if (restflag && (props & FILE_REST))
    return resterr();

	if (fname == NULL)
	{ fname = (char const*)gtfilename(ins ? TEXT132 : TEXT131);
    if (fname == NULL)
  		return FALSE;
	}

#if	FILOCK
	if ((props & FILE_LOCK) && lockchk(fname) == ABORT)
	  return ABORT;
#endif

#if S_MSDOS && S_WIN32 == 0
{	char spareli[257];
	fname = LFN_to_8dot3(LFN_to_83, 0, fname, &spareli[0]);
}
#endif
  if (!ins)
	{ Cc cc = bclear(curbp);
  	if (cc != TRUE)			/* Changes not discarded */
  	  return cc;

  	curbp->b_flag &= ~(BFINVS|BFCHG);
  	fname = repl_bfname(curbp, fname);
  	if (fname == null)
  	  return ABORT;

  			/* let a user macro get hold of things...if he wants */
  	execkey(&readhook, FALSE, 1);
  }
  
{ char fnbuff[NFILEN+1];
 	int nline = 0;
	int crlf = 0;
  int diry = FALSE;
	Cc cc;

{ const char * s = fname-1;
  const char * rname = NULL;
  int got_at = FALSE;
  while (*++s != 0)
  { if (*s == '@')
      got_at = TRUE;
    if (*s == '/')
      rname = s;
  }

  cc = FIOSUC;
  if (!got_at)
  { if (s[-1] == '/')                           /* remove trailing slash */
	   fname = strpcpy(fnbuff, fname, s - fname);
	}
	else
  { BUFFER * tbp = get_remote(curbp, NULL, fname, FALSE, TRUE);
    cc = tbp == NULL ? -1 : FIOSUC;
    if (cc == FIOSUC) 
    { char * s = tbp->b_remote;
      if (*s == 0)
        cc = -1;
      while (*++s != 0)
        if (*s == ' ')
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
  if (cc != 0)
    goto out;

  init_paren("",0);
	cc = FIOEOF;
	
	if (ins)
	{ 				                          /* back up a line and save the mark here */
	/*if ((curwp->w_dotp->l_props & L_IS_HD) == 0) */
	  { curwp->w_dotp    = lback(curwp->w_dotp);
	    curwp->w_line_no -= 1;
	  }
	  curwp->w_doto    = 0;
    paren.in_mode |= curwp->w_dotp->l_props & Q_IN_CMT;
	/*curwp->mrks.c[0].markp = lback(curwp->w_dotp);
	  curwp->mrks.c[0].marko = 0;*/
  }
{	int   len;
	int   fno;
 	char * sfline = NULL;
  LINE * nextline = ins == 0 ? curbp->b_baseline : lforw(curwp->w_dotp);
#if S_MSDOS == 0
	diry = ffisdiry();
#elif S_WIN32 == 0
  char spareline[257];
#endif
	if (diry)
	{ sfline = g_fline;
    msd_init(fname, "", MSD_DIRY | MSD_REPEAT | MSD_STAY | MSD_HIDFILE | MSD_SYSFILE);
	  curbp->b_flag |= MDDIR;
	}

	paren.sdir = 1;
//sp_langprops = curbp->b_langprops & BCCOMT;*/
	while (TRUE)
  { LINE *lp1;
	  if (diry)
	  { g_fline = msd_nfile(&fno);
	    if (g_fline == NULL)
	      break;
	     
	    /*mlreply(fline, spareline+200, 60);*/
#if S_MSDOS && S_WIN32 == 0
	    g_fline = LFN_to_8dot3(LFN_from_83, 1, g_fline, &spareline[0]);
#endif
			if (g_fline[0] == '.' && g_fline[1] == '/')
				g_fline = g_fline + 2;
	    len = strlen(g_fline);
	  }
	  else 
	  { cc = ffgetline(&len);
	    if (cc != FIOSUC)
	      break;
#if 0
      if ((paren.in_mode & 0x3f) && len > 2)
        g_fline[0] = 'A' + (paren.in_mode & 0x3f);
#endif
	  }
	  
	  lp1 = mk_line(g_fline,len,len);
	  if (lp1 == NULL
#if 0
	       || (nline & 0x3ff) == 0 && typahead() && (getkey() & CTRL)
#endif
	     )
	  { cc = FIOMEM;
	    (void)repl_bfname(curbp, "SHORT");
	    break;
	  }

    lp1->l_props = paren.in_mode & Q_IN_CMT;
	  ibefore(nextline, lp1);
	  ++nline;

    scan_par_line(lp1);
	}
//curwp->w_dotp = topline;
	if (g_crlfflag)
	  curbp->b_flag |= MDMS;

	if (diry)
	  g_fline = sfline;
	else
	  ffclose();				/* Ignore errors. */
}
/*		     "Read 999 line" */
	io_message(ins ==0 ? TEXT140 : TEXT154, cc, nline);
out:
//readin_lines = nline;
	curbp->b_doto = 0;

	if (ins)
	{    /* advance to the next line and mark the window for changes */
	/*curwp->mrks.c[0].markp = lforw(curwp->mrks.c[0].markp);
	  curbp->mrks = curwp->mrks;*/
/*	curwp->w_dotp = lforw(curwp->w_dotp); */
	  curbp->b_flag |= BFCHG;		/* we have changed	*/
	  curbp->b_flag &= ~BFINVS;		/* and are not temporary*/
	  curbp->b_wlinep = curwp->w_linep;
	  curbp->b_dotp = curwp->w_dotp;
	/*curbp->b_fcol = curwp->w_fcol;*/
	  curwp->w_flag |= WFHARD | WFMODE;
	}
	else
	{ WINDOW * wp;
	  tcapkopen();								/* open the keyboard again (Unix only) */
	  curbp->b_flag |= BFACTIVE;	/* code added */
//  swb_luct = topluct() + 1;
//  curbp->b_luct = swb_luct;
	  curbp->b_dotp = lforw(curbp->b_baseline);
	  curbp->b_wlinep = curbp->b_dotp;

	  for (wp = wheadp; wp != NULL; wp=wp->w_wndp)
	    if (wp->w_bufp == curbp)
	    { openwind(wp, curbp);
	      wp->w_flag |= WFMODE|WFHARD;
	    /*wp->w_fcol = curbp->b_fcol;*/
	    }
  }

{ LINE * lp = curbp->b_dotp;
	int clamp = 2;
	while (--clamp >= 0)
	{ if (lp->l_used > 6 &&(lp->l_text[0] == '/' && lp->l_text[1] == '*' ||
                          lp->l_text[0] == '/' && lp->l_text[1] == '/' ||
                          lp->l_text[0] == '-' && lp->l_text[1] == '-')
									     && lp->l_text[2] == 't' && lp->l_text[3] == 'a'
									     && lp->l_text[4] == 'b' && lp->l_text[5] == ' ')
	  { int tabw = atoi(lp->l_text+6);
	    if (in_range(tabw, 2, 16))
	      curbp->b_tabsize = tabw;
	  }
	  lp = lforw(lp);
	}

	return cc != FIOERR && cc != FIOFNF;	/* False if error.	*/
}}}}



int Pascal unqname(char * name)			/* make sure a buffer name is unique */
				/* name to check on */
{
	register char *sp, *sp_;
																/* check to see if it is in the buffer list */
	while (bfind(name, 0, FALSE) != NULL)
	{	for (sp = name; *sp; ++sp)	/* go to the end of the name */
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
		{ *sp_++ = '0';
		  *sp_ = 0;
		}
	}
	return OK;
}


/* Ask for a file name, and write the
 * contents of the current buffer to that file.
 * Update the remembered file name and clear the
 * buffer changed flag. This handling of file names
 * is different from the earlier versions, and
 * is more compatable with Gosling EMACS than
 * with ITS EMACS. Bound to "C-X C-W".
 */
int Pascal filewrite(int f, int n)

{	register int s;
	char fname[NFILEN];

	if (restflag)		/* don't allow this command if restricted */
		return resterr();

	if ((s=mlreply(TEXT144, fname, NFILEN)) != TRUE)
/*		       "Write file: " */
		return s;

	s = writeout(fname);
	if (s == TRUE)
	{	repl_bfname(curbp, fname);
		curbp->b_flag &= ~BFCHG;
		upmode();			/* Update mode lines.	*/
	}
	return s;
}


/* Save the contents of the current buffer in its associated file. 
 * Do nothing if nothing has changed.
 * Error if there is no remembered file name for the buffer. 
 * Bound to "C-X C-S". May get called by "C-Z".
 */
int Pascal filesave(int f, int n)

{   
#if S_WIN32
#if GOTTYPAH
  extern int flush_typah();
    flush_typah();
#endif
#endif

  if (curbp->b_flag & MDVIEW)
		return rdonly();
	if ((curbp->b_flag & BFCHG) == 0)	/* Return, no changes.	*/
		return TRUE;
    					/* complain about truncated files */
  if      ((curbp->b_flag & BFTRUNC) && mlyesno(TEXT146) == FALSE)
/*			    "Narrowed Buffer..write it out" */
    ;
  else if ((curbp->b_flag & BFNAROW) && mlyesno(TEXT147) == FALSE)
/*			    "Truncated file..write it out" */
    ;
  else
	{	if (curbp->b_fname == null)		/* Must have a name.	*/
		{	mlwrite(TEXT145);
	/*			"No file name" */
			return FALSE;
	  }
  { char * fn = curbp->b_fname;
    if (curbp->b_remote != NULL)
		{	fn = curbp->b_remote;
      if (*fn == 0)
		  { mlwrite(TEXT102);
		  	return FALSE;
		  }
//		cryptremote(fn);									// read it

#if _DEBUG
      mbwrite(curbp->b_remote);
#endif
      for (fn = &fn[strlen(fn)]; *--fn != ' '; )
        ;
      ++fn;
    }

  {	int rc = writeout(fn);
		if (rc == TRUE)
		{ curbp->b_flag &= ~BFCHG;
		  upmode();		/* Update mode lines.	*/

		  if (curbp->b_remote != NULL)
		  { char cmd[3*NFILEN+1];

				strcpy(cmd, curbp->b_remote);

			{	char * s = &cmd[strlen(cmd)];				// remote file
			  while (--s > cmd && *s != ' ')
			    ;
			  *s++ = 0;
  
			{	char * ss = s;											// file here
			  while (--ss > cmd && *ss != ' ')
			    ;
			  *ss++ = 0;
		    
//			mbwrite(cmd_);                  Security concerns

		  { char cmd_[3*NFILEN+1];
				int cc = ttsystem(concat(cmd_,cmd, " ", s, " ", ss, NULL), NULL);
			  if (cc != 0)
			  { mlwrite(TEXT155);
			  	rc = FALSE;
			  }
			  memset(cmd, 0, sizeof(cmd));
			  memset(cmd_, 0, sizeof(cmd_));
  			memset(curbp->b_remote, 0, strlen(curbp->b_remote));
			  sgarbf = TRUE;
		  }}}}
		}
//  cryptremote(curbp);							// reencrypt it
		return rc;
  }}}

  mlwrite(TEXT8);
/*				"[Aborted]" */
  return FALSE;
}

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
  int caution;
  struct stat fstat_;
  LINE *lp;
  int nline;	  /* number of lines written */
  int sp;		    /* offset into tname */
	char tname[NSTRING];	/* temporary file name */

	if (curbp->b_mode & BCRYPT2)
	{ mlwrite(TEXT223);
		return FALSE;
	}
				        /* let a user macro get hold of things */
	execkey(&writehook, FALSE, 1);
          				 
                  	/* determine if we will use the save method */
#if S_VMS
  caution = 0;
#else
#if S_MSDOS
  caution = ssave == 0 ? 0 : name_mode(fn);
	if (caution & 0200)
	 	caution = 0;
#else
	cc = stat(fn , &fstat_);
{ int w = cc < 0 ? TRUE :
	             (fstat_.st_mode & (getuid() == fstat_.st_uid ? 0200 :
	                                getgid() == fstat_.st_gid ? 020  : 02
	                               ));
	caution = ssave && cc >= 0 || !w;
	if (caution && fstat_.st_nlink > 1 || !w)
	{ cc = mlyesno(w ? TEXT218 : TEXT221);
	  if (cc == ABORT)
  	  return cc;
  	caution = cc != FALSE;
	}
}
#endif
#endif
	if (curbp->b_flag & MDCRYPT)
		resetkey(&curbp->b_key);
	     /* turn off ALL keyboard translation in case we get a dos error */
	tcapkclose();
    						   /* Perform Safe Save..... */
              		 /* duplicate original file name, and find where to trunc it */
	sp = makename(tname, fn);
	strpcpy(tname, fn, NSTRING-30);		/* overwrite the makename */
	for (nline = 4; --nline >= 0; )/* mk unique name using random numbers */
	{ if (caution)
			strcpy(&tname[sp+1], int_asc(ernd()));
		cc = ffwopen(caution, tname);
		if (cc != 0)
			break;
	}

	if (cc <= 0)									/* if the open failed.. clean up and abort */
	{ mlwrite(TEXT155);		/*	"Cannot open file for writing" */
		tcapkopen();								// Unix only
		return FALSE;
	}

	mlwrite(TEXT148); /* tell us that we're writing */
/*		"[Writing...]" */

	g_crlfflag = curbp->b_flag & MDMS;
			/* write the current buffer's lines to the open disk file */
	lp = curbp->b_baseline; 		/* start at the first line. 		*/
	cc = OK;
	nline = 0;					/* track the Number of lines		*/
	while (((lp = lforw(lp))->l_props & L_IS_HD) == 0)
	{ if ((cc = ffputline(&lp->l_text[0], llength(lp))) != FIOSUC)
			break;
		++nline;
	}
		
#if S_MSDOS & CTRLZ
	putc(26, ffp);		/* add a ^Z at the end of the file */
#endif

	cc |= ffclose();
	if (cc == FIOSUC)
	{ extern char deltaf[];
		#define mesg deltaf 
		mesg[0] = 0;		/* message buffer */
 
#if S_VMS == 0
		if (caution)					/* erase original file */
		{ int cc = unlink(fn);
			if (cc == 0 && rename(tname, fn) == 0)
			{ if (! S_MSDOS && fstat_.st_mode != MYUMASK)
					chmod(fn, fstat_.st_mode & 07777);
			}
			else
			{ concat(&mesg[0], TEXT150, tname, null);
/*							", saved as " */
				cc = FIODEL;		/* failed */
			}
		}
#endif
							/* report on status of file write */
		strcat(&mesg[0], TEXT149);
		io_message(mesg, FIOSUC, nline);
				/*	"[Wrote 999 line" */
	}
	tcapkopen();						/// reopen the keyboard (Unix only)
	return cc == FIOSUC;
}


		/*	This command modifies the file name associated with the
			current buffer. It is like the "f" command in UNIX "ed".
			You can type a blank line at the prompt if you wish.
		*/
int Pascal filename(int f, int n)

{ char fname[NFILEN];

	if (restflag) 	/* don't allow this command if restricted */
		return resterr();
		
{ int s = mlreply(TEXT151, fname, NFILEN);				/* "Name: " */
	if (s == ABORT)
		return s;

	curbp->b_flag &= ~MDVIEW; 		 /* no longer read only mode */

	repl_bfname(curbp, s == FALSE ? "" : fname);

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
	
{	int encrypt = TRUE;
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
    encrypt = (s[1] == 0);
  }

{ BUFFER * bp = get_remote(NULL, pw, cmdline, encrypt, f);

	memset(cmdline, 0, len);
  if (bp == NULL)
    return -1;
                          // If the fetch failed continue with any file from last time
	(void)swbuffer(bp);

//sprintf(diag_p, "EKEY %x %s", tbp, tbp->b_key == NULL ? "()" : tbp->b_key);
//mbwrite(diag_p);

	return gotobob(1,0);
}}}}}
