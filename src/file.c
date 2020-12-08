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
#include	"h/msdir.h"
#include	"logmsg.h"

extern int   crlfflag;
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

  register const char *s_cp;
  register const char *cand = &fname[0]; 
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

#if 0

int rinsfile(int wh)

{ static const char act_[] = "Insert\000Read";
  char buf[12];
  
  if (restflag)
    return resterr();

  return readin(gtfilename(strcat(strcpy(buf,act_[wh*7])," file")), wh*2-1);
}

#endif

/* Read a file into the current buffer.
 * Bound to "C-X C-R".
 */
int Pascal fileread(int f, int n)

{
#if 0
   return rinsfile(1);
#else	
   if (restflag)
     return resterr();
/*				"Read file" */
   return readin(gtfilename(TEXT131), TRUE);
#endif
}


/* Insert a file into the current buffer.
 * Bound to "C-X C-I".
 */
int Pascal insfile(int f, int n)

{ if (curbp->b_flag & MDVIEW)
    return rdonly();      
#if 0
  return rinsfile(0);
#else
  if (restflag)
    return resterr();
/*				"Insert file" */
  return readin(gtfilename(TEXT132), -1);
#endif
}

void Pascal customise_buf(BUFFER * bp,const char * fn)

{   const char * pat = NULL;
    --fn;
    while (*++fn != 0)
      if (*fn == '.')
        pat = fn+1;

    if (pat != NULL && g_file_prof != NULL)
    {  
        char * pr = g_file_prof - 1;
        while (*++pr != 0)
        { if (*pr != '.') continue;
        
        { const char * p = pat - 1;

          while (*++p != 0 && *++pr == *p)
            ;

          if (*p != 0 || pr[1] != '=') continue;

          if (pr[2] == '^')
          { ++pr;
            bp->b_flag &= ~MDEXACT;
          }
          bp->b_tabsize = atoi(pr+2);
          break;
        }}
   }
}



BUFFER * Pascal bufflink(char * filename, int create)/* create: 1: create, 16: search, 32: dont stay */

{ char bname[NFILEN];
#if NFILEN < 2 * NBUFN + 30
  error error
#endif
#define text (&bname[NBUFN+1])
  char fname[NFILEN];
  char bestfn[NFILEN];
  int srch = nmlze_fname(&fname[0], filename, bname);
  
{ BUFFER * firstbp = NULL;
  int fno;
  char * fn = fname;
  char * pat = null;

  if 			(create & 32)
    ;
  else if (create & 16)
  { for (fn = fname; *fn != 0; ++fn)
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
  else if (create > 1)
    srch |= MSD_STAY;
    
  create &= ~(16+32);
  
  if (srch && g_newest)
  { unsigned int newdate = 0;
    msd_init(fn, pat, srch|MSD_REPEAT|MSD_HIDFILE|MSD_SYSFILE|MSD_IC);

    while ((fn = msd_nfile(&fno)) != NULL)
    { if (newdate < (unsigned)msd_stat.st_mtime)
      { newdate = (unsigned)msd_stat.st_mtime;
        strpcpy(bestfn,fn,NFILEN);
        srch = 0;
      }
    }
    if (newdate != 0) strcpy(fname,bestfn);
  }
  else if (srch)  
    msd_init(fn, pat, srch|MSD_REPEAT|MSD_HIDFILE|MSD_SYSFILE|MSD_IC);

  while ((fn = ! srch ? fname : msd_nfile(&fno)) != NULL)
  { register BUFFER * bp;
#if S_MSDOS && S_WIN32 == 0
    if (srch & MSD_DIRY)
      fn = LFN_to_8dot3(LFN_from_83, 0, fn, &fname[0]);
#endif
     
    /*loglog2("mfn %s %s", &fn[fno], pat);*/
    /*if (strcmp(&fn[fno], pat) != 0)*/
    /*if (*match_fn_re_ic(&fn[fno], pat, true) != 0)
      { loglog2("late rej %s %s", &fn[fno], pat);
        continue;
      }*/

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
/*						"Cannot create buffer" */
				    return null;
				  }
	  			create = TRUE;
				} 
				else
				{ if      (g_clexec || g_nosharebuffs)		/* => create */
				  { if (bp->b_fname == null || strcmp(bp->b_fname, fn) == 0)
	            break;
				    unqname(bname);
		  		{ extern int bfindmade;
				    if (bfindmade)
		  		    zotbuf(bp);
	        }}
		      else if (create)
			    	break;
				  else				/* old buffer name conflict code */
				  { int s = mlreply(concat(&text[0], TEXT136, bname, "):", null),
								     							 bname, NBUFN);
/*			    						"Buffer (" */
				    if (s == ABORT) 		/* ^G to just quit	*/
				      return firstbp;
				    if (s == FALSE) 		/* CR to clobber it	*/
				    { makename(bname, fn);	/* restore it */
				      unqname(bname);		/* make it unique */
				      create = TRUE;		/* It already exists but this causes */
				    }				/* a quit the next time around	     */
				  }
				}
	    }
	    
	    repl_bfname(bp, fn);
	    bp->b_flag &= ~BFACTIVE;
	      
	    customise_buf(bp,fn);
	  }
	  if (firstbp == NULL)
	    firstbp = bp;

	  if (!srch)
	    break;
  }

  return firstbp;
}}



BUFFER * Pascal bufflkup(char * filename, int create)/* create: 1: create, 16: search, 32: dont stay */

{ BUFFER * res;
  int ups = 0;
  int relative = 0;
  char fname[NFILEN];
  strpcpy(fname,filename+1,NFILEN);

  if (fname[0] != '/' && fname[0] != '\\' && fname[1] != ':')
    relative = 1;

  if (filename[0] != '^')
    return bufflink(filename,create);

/*mbwrite2("Lk1:",fname);*/

  while ((res = bufflink(fname,create | 32)) == NULL)
  { if (relative) 
    { int i = ++ups;
      if (ups > 5) break;
      
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

static char * ffmsg = TEXT133;

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
  char *fname = gtfilename(ffmsg);
/*				"Find file" */
	if	(fname == NULL || fname[0] == 0)
	  return FALSE;

	for (s = fname-1; *++s != 0																	// strip off :n
  					     && (*s != ':' && *s != '(' || !isdigit(s[1]) )
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
	
	if (s != null)
	{ gotobob(1,0);
		return forwline(1, atoi(s+1)-1);
	}
	return TRUE;
}}}


int Pascal viewfile(int f, int n)	/* visit a file in VIEW mode */

{	char * sffmsg = ffmsg;
	ffmsg = TEXT134;
{	int s = filefind(f, n); 	/* bug: > 2 files => some not view */
	if (s)
	{ curwp->w_bufp->b_flag |= MDVIEW;
	  upmode();
	}
	ffmsg = sffmsg;
	return s;
}}

#if CRYPT == 0
# define resetkey()
#else

static void Pascal resetkey()	/* reset the encryption key if needed */

{ if (curbp->b_flag & MDCRYPT)
  {
    if (curbp->b_key == NULL)
      curbp->b_key = ekey;
    if (setekey(&curbp->b_key) == TRUE)
    { char * key = curbp->b_key;   
      int sl = strlen(key);		/* and set up the key to be used! */
		
      initcrypt(key, sl);	      /* de-encrypt it */
      initcrypt(key, sl);	      /* re-encrypt it...seeding it to start */
    }
  }	  
}
#endif

BUFFER * Pascal gotfile(char * fname)
					/* file name to find */
{
	register BUFFER *bp;

	if (fname[0] == 0)
		return curbp;

#if	S_MSDOS | S_OS2
	mkul(0, fname); 	       /* msdos isn't case sensitive */
#endif

	for (bp = bheadp; bp != NULL; bp = bp->b_bufp)
	  if ((bp->b_flag & BFINVS)==0 &&
	      bp->b_fname != null && strcmp(bp->b_fname, fname) == 0)
	  {
			mlwrite(TEXT135);
/*			       "[Old buffer]" */
			return bp;
	  }
	  
	return 0;
}


/*	Read file "fname" into the curbp, blowing away any text
	found there.  Called by both the read and find commands.
	Return the final status of the read.  Also called by the mainline,
	to read in a file specified on the command line as an argument. 
	The command in $readhook is called after the buffer is set up
	and before it is read. 
*/
/*Int readin_lines;			** aux result */

int Pascal readin(char * fname, int lockfl)
				/* name of file to read */
			 	/* check for file locks?, -ve => insert */
{ register LINE *lp1;
		       int len;
	register int s;
	register int nline = 0;
		    int crlf = 0;
        int diry = FALSE;

	if (fname == NULL)
		return FALSE;

#if	FILOCK
	if (lockfl && lockchk(fname) == ABORT)
	  return ABORT;
#endif
	resetkey();				/* set up for decryption */

	g_clring = (curbp->b_langprops & (BCCOMT + + BCPRL + BCFOR + BCSQL + BCPAS));

  if (lockfl >= 0)
	{ s = bclear(curbp);
	  if (s != TRUE)			/* Might be old.	*/
	    return s;

	  curbp->b_flag &= ~(BFINVS|BFCHG);
	  fname = repl_bfname(curbp, fname);
	  if (fname == null)
	    return ABORT;
	  
			/* let a user macro get hold of things...if he wants */
	  execkey(&readhook, FALSE, 1);
  }
#if S_MSDOS && S_WIN32 == 0
{	char spareli[257];
	fname = LFN_to_8dot3(LFN_to_83, 0, fname, &spareli[0]);
}
#endif
{ int trail_len = strlen(fname)-1;
	char trail_ch = fname[trail_len];
	if (trail_len > 0 && trail_ch == '/')
	  fname[trail_len] = 0;

{  int is_rc = trail_ch == 'c' && trail_len > 3
            && fname[trail_len-1] == 'r' ? 1 : 0;

	s = ffropen(fname);
	if (s != FIOSUC)			/* File not found. */
	{ 
#if S_MSDOS
	  diry = name_mode(fname) & 040000;
	  if (diry == 0)
#endif
	  { mlwrite(lockfl >= 0 ? TEXT138 : TEXT152);
/*			"[New file]" */
/*		        "[No such file]" */
      if (lockfl < 0)
        return FALSE;
	    goto out;
	  }
	}
						/* read the file in */
	mlwrite(lockfl >= 0 ? TEXT139 : TEXT153);
/*		"[Reading file]" */
/*		"[Inserting file]" */
        init_paren("",0);
	s = FIOEOF;

	if (lockfl < 0)
	{ 				/* back up a line and save the mark here */
	/*  if ((curwp->w_dotp->l_props & L_IS_HD) == 0) */
	  { curwp->w_dotp    = lback(curwp->w_dotp);
	    curwp->w_line_no -= 1;
	  }
	  curwp->w_doto    = 0;
    paren.in_mode |= curwp->w_dotp->l_props & Q_IN_CMT;
	/*curwp->mrks.c[0].markp = lback(curwp->w_dotp);
	  curwp->mrks.c[0].marko = 0;*/
  }

{	char * sfline = NULL;
	int    fno;
	LINE * topline = curwp->w_dotp;
#if S_MSDOS == 0
	diry = ffisdiry();
#elif S_WIN32 == 0
  char spareline[257];
#endif
	if (diry)
	{ sfline = fline;
    msd_init(fname, "", MSD_DIRY | MSD_REPEAT | MSD_STAY | MSD_HIDFILE | MSD_SYSFILE);
	  curbp->b_flag |= MDDIR;
	}

	paren.sdir = 1;
/*	sp_langprops = curbp->b_langprops & BCCOMT;*/
	while (TRUE)
	{ if (diry)
	  { fline = msd_nfile(&fno);
	    if (fline == NULL)
	      break;
	     
	    /*mlreply(fline, spareline+200, 60);*/
#if S_MSDOS && S_WIN32 == 0
	    fline = LFN_to_8dot3(LFN_from_83, 1, fline, &spareline[0]);
#endif
			if (fline[0] == '.' && fline[1] == '/')
				fline = fline + 2;
	    len = strlen(fline);
	  }
	  else 
	  { s = ffgetline(&len);
	    if (s != FIOSUC)
	      break;
#if 0
      if ((paren.in_mode & 0x3f) && len > 2)
        fline[0] = 'A' + (paren.in_mode & 0x3f);
#endif
	  }
	  
	  lp1 = mk_line(fline,len,len);
	  if (lp1 == NULL
#if 0
	       || (nline & 0x3ff) == 0 && typahead() && (getkey() & CTRL)
#endif
	     )
	  { s = FIOMEM;
	    (void)repl_bfname(curbp, "SHORT");
	    break;
	  }

    lp1->l_props = paren.in_mode & Q_IN_CMT;
	  ibefore(lockfl >= 0 ? curbp->b_baseline : lforw(curwp->w_dotp), lp1);
	  
	  ++nline;
	  if (lockfl < 0)
	    curwp->w_dotp = lp1; /* and advance and write out the current line */
    if (! is_rc)
      scan_par_line(lp1);
	}
	curwp->w_dotp = topline;
	if (crlfflag)
	  curbp->b_flag |= MDMS;

	if (diry)
	  fline = sfline;
	else
	  ffclose();				/* Ignore errors. */
}
/*		     "Read 999 line" */
	io_message(lockfl >= 0 ? TEXT140 : TEXT154, s, nline);
out:
  fname[trail_len] = trail_ch;
     /* readin_lines = nline; */
	
	curbp->b_doto = 0;
	if (lockfl < 0)
	{    /* advance to the next line and mark the window for changes */
	/*curwp->mrks.c[0].markp = lforw(curwp->mrks.c[0].markp);
	  curbp->mrks = curwp->mrks;*/
/*	  curwp->w_dotp = lforw(curwp->w_dotp); */
	  curwp->w_flag |= WFHARD | WFMODE;
	  curbp->b_flag |= BFCHG;		/* we have changed	*/
	  curbp->b_flag &= ~BFINVS;		/* and are not temporary*/
	  curbp->b_wlinep = curwp->w_linep;
	  curbp->b_dotp = curwp->w_dotp;
	/*curbp->b_fcol = curwp->w_fcol;*/
    mbwrite("READIN");
	}
	else
	{
#if S_MSDOS == 0
	  tcapkopen();	/* open the keyboard again */
#endif
	  curbp->b_flag |= BFACTIVE;	/* code added */
	  swb_luct = topluct() + 1;
	  curbp->b_luct = swb_luct;
	  curbp->b_dotp = lforw(curbp->b_baseline);
	  curbp->b_wlinep = curbp->b_dotp;

	  for (lp1=(LINE*)wheadp; 
         lp1 != NULL; 
         lp1=(LINE*)((WINDOW*)lp1)->w_wndp)
	    if (((WINDOW*)lp1)->w_bufp == curbp)
	    { openwind((WINDOW*)lp1, curbp);
	      ((WINDOW*)lp1)->w_flag |= WFMODE|WFHARD;
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
	
	return s != FIOERR && s != FIOFNF;	/* False if error.	*/
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
  {	int s = writeout(curbp->b_fname);
		if (s == TRUE)
		{ curbp->b_flag &= ~BFCHG;
		  upmode();		/* Update mode lines.	*/
		{	int sl = strlen(curbp->b_fname);
		  if (curbp->b_remote != NULL)
		  { char cmd[3*NFILEN+1];
		    char cmd_[3*NFILEN+1];
				strcpy(cmd, curbp->b_remote);

			{	char * s = &cmd[strlen(cmd)];
			  while (--s > cmd && *s != ' ')
			    ;
			  *s = 0;
  
			{	char * ss = s;
			  while (--ss > cmd && *ss != ' ')
			    ;
			  *ss = 0;
  
		    concat(cmd_,cmd, " ", s+1, " ", ss+1, null);
				mbwrite(cmd_);

			{ int cc = system(cmd_);
			  if (cc != 0)
			  { mlwrite(TEXT155);
			  	s = FALSE;
			  }
		  }}}}
		}}
		return s;
  }}

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
{ register Cc cc;
    int caution = false;
    struct stat fstat_;
 register LINE *lp;
        int nline;	/* number of lines written */
        int sp;		/* offset into tname */
	char tname[NSTRING];	/* temporary file name */
				      /* let a user macro get hold of things */
	execkey(&writehook, FALSE, 1);
				 /* determine if we will use the save method */
	if (fn != null)
	{
#if S_VMS
    caution = 0;
#else
#if S_MSDOS
  	cc = name_mode(fn);
		if (cc != 0 && (cc | 0200) == 0 && ssave)
	  	caution = 1;
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
		resetkey();
	     /* turn off ALL keyboard translation in case we get a dos error */
		tcapkclose();
	}						   /* Perform Safe Save..... */

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
	if (caution)
		caution = cc > 0;

	if (cc <= 0)									/* if the open failed.. clean up and abort */
	{ mlwrite(TEXT155);		/*			"Cannot open file for writing" */

#if S_MSDOS
		tcapkopen();
#endif
		return FALSE;
	}

	mlwrite(TEXT148); /* tell us that we're writing */
/*		"[Writing...]" */

	crlfflag = curbp->b_flag & MDMS;
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
#if S_MSDOS
	tcapkopen();			/* reopen the keyboard, and return our status */
#endif
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
	repl_bfname(curbp, s == FALSE ? "" : fname);
						/* Update mode lines. */
	upmode();
	curbp->b_flag &= ~MDVIEW; 		 /* no longer read only mode */
	return TRUE;
}}



int Pascal fetchfile(int f, int n)

{ LINE * lp = curwp->w_dotp;
	if (lp->l_text == NULL)
	  lp = lforw(lp);
	if (lp->l_text == NULL)
	{	mbwrite("Nothing there");
	 	return FALSE;
	}

{	int len = lp->l_used;
	char cmdline[2*NFILEN+1];
	memcpy(cmdline, (char*)lp->l_text, len < 2*NFILEN ? len :	2*NFILEN);
	cmdline[lp->l_used] = 0;
	
{ char cmd[3*NFILEN+1];
	const char * cmdnm = gtusr("scp");

	strcpy(cmd,cmdnm == NULL || strcmp(cmdnm,"ERROR") == 0 ? "c:\\bin\\pscp" : cmdnm);
	strcat(cmd," ");
	
//mbwrite(cmd);
	
{	char rname[NFILEN] = "remotefile";
	int rix = -1;
	char * ss;
	char * s = cmdline;
	while (*s != 0 && *s != '\t')					// Go to first tab
		++s;
		
	ss = s;
	while (--ss > cmdline && *ss != '/')	// Go back to slash
		;
	
	if (*ss == '/')												// Get file name
	{	while (*++ss != '\t')
		  rname[++rix] = *ss;
		rname[++rix] = 0;
	}

	while (*s == '\t')										// skip extra tabs
		++s;
		
	--s;

	if (*s == 0)
	{ mbwrite("No Password");
		return FALSE;
	}
	*s = 0;

{ char fullcmd[3*NFILEN+1];
	concat(fullcmd,cmd,"-pw ",s+1," ", cmdline," ",0);
{ int clen = strlen(fullcmd);
	strcat(strcat(strcat(fullcmd,(char *)getenv("TEMP")),"\\"),rname);

//mbwrite(fullcmd);

{ int cc = system(fullcmd);
	if (cc != 0)
	{ concat(cmd,"Fetch failed ",int_asc(cc), 0);
		mbwrite(cmd);
	}

{	// char diag_p[80];
	BUFFER * tbp = curbp;
  char * ekey = curbp->b_key;
  int tabsz = curbp->b_tabsize;
  int smode = g_gmode;
  if (ekey != NULL)
    g_gmode |= MDCRYPT;

//sprintf(diag_p, "EKEY %x %s", curbp, ekey == NULL ? "()" : ekey);
//mbwrite(diag_p);
	
{	BUFFER * bp = bufflkup(fullcmd+clen, g_clexec);
	if (bp == NULL)
		return FALSE;
		
	bp->b_key = ekey;
	bp->b_tabsize = tabsz;
	(void)swbuffer(bp);
	bp->b_remote = strdup(fullcmd);		// allow leak

  g_gmode = smode;	

//sprintf(diag_p, "EKEY %x %s", tbp, tbp->b_key == NULL ? "()" : tbp->b_key);
//mbwrite(diag_p);

	if (s != null)
	{ gotobob(1,0);
		return forwline(1, atoi(s+1)-1);
	}
	return TRUE;
}}}}}}}}}

