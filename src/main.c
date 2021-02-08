/*	MicroEMACS 3.10
 *		written by Daniel M. Lawrence
 *		based on code by Dave G. Conroy.
 *
 *	(C)opyright 1988,1989 by Daniel M. Lawrence
 *	MicroEMACS 3.10 can be copied and distributed freely for any
 *	non-commercial purposes. MicroEMACS 3.10 can only be incorporated
 *	into commercial software with the permission of the current author.
 *
 * This file contains the main driving routine, and some keyboard processing
 * code, for the MicroEMACS screen editor.
 *
 */

#include	<stdio.h>
#include	<stdlib.h>

//#ifdef _WINDOWS
//#include <afxwin.h>
//#endif

/* make global definitions not external */
#define maindef

#include	"estruct.h"	/* global structures and defines */
#include	"edef.h"	/* global definitions */
#include	"etype.h"	/* variable prototype definitions */
#include	"elang.h"	/* human language definitions */
#include	"epredef.h"
#include	"build.h"
#include	"map.h"
#include	"msdir.h"
#include	"logmsg.h"


#if	S_BSD | S_UNIX5 | S_HPUX | S_XENIX | S_AMIGA | S_LINUX
# define EMACSRC ".emacsrc"
#else
# define EMACSRC "emacs.rc"
#endif

extern char * getenv();

extern int g_overmode;			/* from line.c */

/* initialized global definitions */
NOSHARE int g_nosharebuffs = FALSE; /* dont allow different files in same buffer*/
NOSHARE int g_clexec = TRUE;	/* command line execution flag	*/

NOSHARE char *execstr = NULL;		/* pointer to string to execute */
NOSHARE int g_execlevel = 0;	/* execution IF level		*/
NOSHARE int g_colours = 7;		/* (backgrnd (black)) * 16 + foreground (white) */
NOSHARE int mpresf = FALSE;	/* TRUE if message in last line */
NOSHARE int ttrow = HUGE; 	/* Row location of HW cursor	*/
NOSHARE int ttcol = HUGE; 	/* Column location of HW cursor */
NOSHARE int lbound = 0;		/* leftmost column of current line
					   being displayed		*/
NOSHARE int abortc = CTRL | 'G';	/* current abort command char	*/
NOSHARE int sterm = CTRL | 'M';	/* search terminating character */

static
NOSHARE int g_prefix = 0;	/* currently pending prefix bits */
NOSHARE int prenum = 0;		/*     "       "     numeric arg */

char highlight[64] = "4hello";

NOSHARE int restflag = FALSE;	    /* restricted use?		*/
NOSHARE int g_newest = 0;         /* choose newest file           */
//#define USE_SVN
#ifdef USE_SVN
 NOSHARE int del_svn = 0;
#else
 #define del_svn 0
#endif
#if WRAP_MEM
 NOSHARE Int envram = 0l;		/* # of bytes current in use by malloc */
#endif

const char g_logm[3][8] = { "FALSE","TRUE", "ERROR" };			/* logic literals	*/

NOSHARE char palstr[49] = "";		/* palette string		*/
NOSHARE char lastmesg[LFSTR] = ""; 	/* last message posted		*/
NOSHARE int(Pascal *lastfnc)(int, int);/* last function executed	*/
NOSHARE int eexitflag = FALSE;	/* EMACS exit flag		*/
NOSHARE int eexitval = 0; 	/* and the exit return value	*/
/*NOSHARE int nclicks = 0;		** cleared on any non-mouse event*/


/* uninitialized global definitions */

NOSHARE int currow;	/* Cursor row			*/
NOSHARE int curcol;	/* Cursor column		*/
NOSHARE int thisflag;	/* Flags, this command		*/
NOSHARE int lastflag;	/* Flags, last command		*/
NOSHARE WINDOW *curwp; 		/* Current window		*/
NOSHARE BUFFER *curbp; 		/* Current buffer		*/
NOSHARE WINDOW *wheadp;		/* Head of list of windows	*/
NOSHARE BUFFER *bheadp;		/* Head of list of buffers 	*/

NOSHARE char rpat[NPAT+10];	/* replacement pattern		*/

/*	Various "Hook" execution variables	*/

NOSHARE KEYTAB hooks[6];

/*	The variable matchlen holds the length of the matched string -
	used by the replace functions. The variable patmatch holds the
	string that satisfies the search command. The variables matchline
	and matchoff hold the line and offset position of the *start* of
	match.
*/

NOSHARE char *patmatch = NULL;
NOSHARE int lastdir = 0;

#if	DEBUGM
				/* vars needed for macro debugging output*/
 NOSHARE char outline[NSTRING];	/* global string to hold debug line text */
#endif

NOSHARE char *g_ekey = NULL;		/* global encryption key	*/

/*	make VMS happy...	*/

#if	S_VMS
#include	<ssdef.h>
#define GOOD	(SS$_NORMAL)
#else
#define GOOD	0
#endif

char * g_invokenm;
char * g_homedir;


#define MLIX 3

extern char lastline[MLIX+1][NSTRING];
extern int  ll_ix;

extern int g_timeout_secs;
extern int macro_last_pos;


void Pascal editloop(int c_); /* forward */


//void Pascal load_pat(const char * src);



/*	Process a command line.   May be called any time.	*/

static
void Pascal dcline(int argc, char * argv[])

{ BUFFER *bp;
	char * startfile = EMACSRC;	/* startup file */
	BUFFER * firstbp = NULL;	/* ptr to first buffer in cmd line */
	int gline = 0; 	 				/* line to goto at start or 0 */
	int genflag = 0;
	const char * def_bname = "main";
	int nopipe = 1;
	int	carg;
						/* Parse a command line */
#if S_MSDOS && 0
	for (carg = argc; --carg > 0; )
		clean_arg(argv[carg]);
#endif
	if (g_invokenm == NULL)
		g_invokenm = argv[0];

	for (carg = argc; --carg > 0; )
	{ char * filev = (++argv)[0];
		if			(filev[0] == '-')
			switch (toupper(filev[1]))
			{ 			/* Process Startup macros */
				case 'E': genflag |= 4; /* -e process error file */
				when 'M': gflags |= MD_KEEP_MACROS;
				when 'X': gflags |= MD_KEEP_CR;
		 /* when 'Z': gflags |= MD_NO_MMI; */

		 /* when 'G': gline = atoi(&filev[2]); ** -g for initial goto */
				
				when 'I': /* -i<var> [<value>] set an initial */
					/* value for a variable */
					if (--carg > 0)
						set_var(&filev[2], (++argv)[0]);
#if CRYPT 				
				when 'K': /* -k<key> for code key */
					if (filev[2] != 0)
						g_ekey = strdup(&filev[2]);			// visible from command line args
					g_gmode |= MDCRYPT;
#endif						
				when 'R': /* -r restrictive use */
					restflag = TRUE;
				when 'S': /* -/pat for initial search string */
					genflag |= 1;
				case '/': 
					if (filev[2] == 0 && carg > 1)
					{ filev = (++argv)[0];
						filev -= 2;
						--carg;
					}
					if (filev[2] == '"')
					{ filev += 1;
						filev[strlen(filev)-1] = 0;
					}
					
					strpcpy(&lastline[0][0],
									strpcpy(&pat[0],filev+2, sizeof(pat)), sizeof(pat));
					ll_ix = 0;

				when 'B': def_bname = filev+2;
					genflag |= BFACTIVE;
				when 'P': /* -p for search $PATH */
					genflag |= 2;
//			when 'T': /* -T for search directory structure */
//				genflag |= 16;
				when 'V': /* -v for View File */
					genflag |= MDVIEW; /* 0x100 */
				when 'Z': g_newest = 1;
				when 'W': g_timeout_secs = atoi(&filev[2]);
									if (g_timeout_secs == 0)
										g_timeout_secs = 900;			/* 15 minutes */
			}
		else if (filev[0]== '@')
			startfile = &filev[1];
		else			/* Process an input file */
		{
#ifdef USE_SVN
      if (strcmp_right(filev,"https://svn.") == 0)
				del_svn = 6;
#endif
		  if (filev[1] != 0)
		  { int ignore = 1;
		  
  			for ( filev = (filev+2); *filev != 0; ++filev)
  			{
#ifdef USE_SVN
  				if (*filev == '@')
  					del_svn = filev - argv[0] + 1000;
#endif
  				if (*filev == '@')
  				  ignore = 2;

  			  if (*filev == ':' && --ignore <= 0 && filev - argv[0] > del_svn)
  			  { *filev = 0;
  					gline = atoi(filev+1);
  					break;
  			  }
  			}
			}

		  filev = argv[0];
#ifdef USE_SVN
		  if (del_svn > 0)
		  { char * spareline = &lastline[0][0];
				int sl = strlen(filev);
				while (filev[sl] != '/')
					--sl;
				if (del_svn < 1000)
				{ concat(spareline,"svncone ",filev," tmpdir", null);
				  ttsystem(spareline, NULL);
				  concat(spareline,"tmpdir/",&filev[sl+1], null);
				  filev = spareline;
				  concat(lastline[1],"copy ",filev," .", null);
				  ttsystem(lastline[1], NULL);
				  del_svn = 1 - g_newest;
				}
				else
				{ del_svn = del_svn - 1000;
				  concat(pat,"scp ",filev," tmpdir/", null);
						
				  pipefilter('e');
				}
		  }
#endif
		  if (genflag & 2)									// look along path
		  { filev = (char *)flook(0, filev);
				if (filev == null)
					filev = argv[0];
		  }
		  bp = bufflink(strdup(filev),TRUE/*|(genflag & 16)*/); /* setup buffer for file */
		  if (bp != NULL)
		  { bp->b_flag |= (genflag & MDVIEW);

				if (firstbp == NULL)
				{ if (genflag & BFACTIVE)
				  { zotbuf(bp);
						bp = bufflink(def_bname, 1 | MSD_DIRY);
						if (bp != NULL)													// safe against out of memory 
						{ bp->b_flag |= (genflag & MDVIEW);
					  	repl_bfname(bp, filev);
						}
				  }
				  firstbp = bp;
				}
		  }
		  lastline[0][0] = 0;
		}
	} // loop

	if (firstbp == null)
	{
		firstbp = bfind(def_bname, TRUE, 0);
		nopipe = iskboard();
		if (nopipe == 0)
		{ 
			firstbp->b_fname = strdup("-");
			firstbp->b_flag |= g_gmode;
		}
	}

	curwp->w_ntrows = term.t_nrowm1-1; /* "-1" for mode line. */
	curbp = firstbp;
	openwind(curwp, curbp);
					/* if we are C error parsing... run it! */
	carg = startup(startfile);
#if 0
	if (gflags & MD_NO_MMI)
	{ writeout(null);
		return 1;
	}
#endif
	if (carg != TRUE)
	{ firstbp = dofilebuff;
		if (firstbp == NULL)
		{	carg = 13;
			startfile = "No .rc file";
		}
		else
		{// firstbp->b_flag |= BFACTIVE;
			startfile = "Error in .rc file%w%w";
		}
		mbwrite(startfile);
	}
	else
	{ if (genflag & 4)
			carg = startup("error.rc");
	}
	if (carg == 13)
		exit(1);
																								/* we now have the .rc file */
	for (bp = bheadp; bp != NULL; bp = bp->b_bufp)
	{ bp->b_flag |= g_gmode;
		bp->b_color = g_colours;
	  if (bp->b_fname != NULL)
	  	customise_buf(bp);
	}

	swbuffer(firstbp);

#ifndef _WINDOWS
  tcapepage();
#endif
  ttopen();
  tcapkopen();    /* open the keyboard */
  tcaprev(FALSE);
  sgarbf = TRUE;

	if (g_clring & BCCOMT)
		addnewbind(CTRL | 'M', indent);

#if 0
	putpad("\033[m\033[2J\007");TTflush();
	sleep(3);
	putpad("\033[27m\033[2J\007");TTflush();
	sleep(3);
	putpad("\033[34;47m\033[2J\007\007");TTflush();
	sleep(3);
	putpad("\033[m\033[2J\007\007\007");TTflush();
	sleep(3);/*
	putpad("\033[2;2H\007");
	sleep(3);*/
#endif		
#if S_MSDOS == 0
	{ char buf[35];
		mlwrite(strpcpy(&buf[0], lastmesg, 30));
	}
#endif
	
	if (nopipe == 0)
		firstbp->b_fname = null;

	(void)gotoline(TRUE, gline);
 
	if (genflag & 1)
		(void)forwhunt(FALSE, 0);

  g_gmode &= ~MDCRYPT;
	g_clexec = FALSE;
}


/*	This is the primary entry point that is used by command line
	invocation, and by applications that link with microemacs in
	such a way that each invocation of Emacs is a fresh environment.

	There is another entry point in VMS.C that is used when
	microemacs is "linked" (In quotes, because it is a run-time link
	rather than a link-time link.) with applications that wish Emacs
	to preserve it's context across invocations.	(For example,
	EMACS.RC is only executed once per invocation of the
	application, instead of once per invocation of Emacs.)

	Note that re-entering an Emacs that is saved in a kept
	subprocess would require a similar entrypoint.
*/

#if S_WIN32
int main_(int argc, char * argv[])
#elif CALLED
int emacs(int argc, char * argv[])
#else
int main(int argc, char * argv[])
#endif
{
#if defined(_DEBUG) || LOGGING_ON
	log_init("emacs.log", 300000, 0);
	loglog("***************Started***************");
#endif
//char ch  = ttgetc();
	init_fncmatch();
	vtinit();
#if CALLED
	varinit();		/* user variables */
#endif
#if DIACRIT
	initchars();		/* character set definitions */
#endif
	tcapopen(); 	/* open the screen AGAIN ! */

{	KEYTAB * hpp;
	for (hpp = &hooks[6]; --hpp >= &hooks[0]; )
	  hpp->k_ptr.fp = nullproc;

	wheadp =
	curwp = (WINDOW *) aalloc(sizeof(WINDOW)); /* First window	*/

	set_var("$incldirs", getenv("INCLUDE"));
	g_homedir = getenv("HOME");
	if (g_homedir == NULL)
		g_homedir = "";

	(void)dcline(argc, argv);
	do
	{ lastflag = 0; 								/* Fake last flags.*/

																	/* execute the "command" macro, normally null*/
		execkey(&cmdhook, FALSE, 1);	/* used to push/pop lastflag */
				
		update(FALSE);		/* Fix up the screen	*/

	{ int	c = getkey(); 	/* get the next command from the keyboard */

		editloop(c);
	}}
	while (!eexitflag);

#if S_WIN32
	ClipSet(NULL);
//tcapbeeol(-1,0);
#else
	tcapclose(0);
#endif

#ifdef USE_SVN
	if (del_svn)
	{ 
		ttsystem("rmdir /s/q tmpsvndir", NULL);
	}
#endif
#if CLEAN
	clean();
#endif
	return eexitval;
}}

#if CLEAN
/*
	On some primitive operation systems, and when emacs is used as
	a subprogram to a larger project, emacs needs to de-alloc its
	own used memory, otherwise we just exit.
*/
Pascal clean()

{ register BUFFER *bp;	/* buffer list pointer */
	register WINDOW *wp;	/* window list pointer */

	wp = wheadp;			/* first clean up the windows */
	while (wp)
	{ WINDOW * tp = wp->w_wndp;
		free(wp);
		wp = tp;
	}
	wheadp = NULL;
					/* then the buffers */
	while ((bp = bheadp))
	{ bp->b_nwnd = 0;
		bp->b_flag = 0; /* don't say anything about a changed buffer! */
		zotbuf(bp);
	}

	kdelete(0,0); 		/* and the kill buffer */

#if MAGIC
	mcclear();			/* clear some search variables */
	rmcclear();
#endif
	if (patmatch != NULL)
	{ free(patmatch);
		patmatch = NULL;
	}

	varclean(); 		/* dealloc the user variables */
	vtfree(); 		/* and the video buffers */
}
#endif

#if S_MSDOS && 0

void clean_arg(char * str)

{ register int ix;
	for (ix = strlen(str); --ix >= 0 && str[ix]	< ' '; )
		;
	str[ix+1] = 0;
}
#endif


KEYTAB * lastbind;
KEYTAB * prevbind;

/* This is the general command execution routine. It handles the fake binding
 * of all the keys to "self-insert". It also clears out the "thisflag" word,
 * and arranges to move it to the "lastflag", so that the next command can
 * look at it. Return the status of command.
 */
static int Pascal execute(int c, int f, int n)

{ register int status;
	KEYTAB *key = getbind(c); /* key entry to execute */
		 /* loglog3("L %x T %x ct %d", lastbind == NULL ? 0 : lastbind->k_code, key->k_code, keyct); */

	if (key != lastbind && key->k_ptr.fp != cex
											&& key->k_ptr.fp != meta)
	{ prevbind = lastbind;
		lastbind = key;
		keyct = 0;
	}

	keyct += 1;
					 
	if		 (key->k_code != 0) /* if keystroke is bound to a function..do it*/
	{ thisflag = 0;
	
		status = execkey(key, f, n);			// f is 0 or any other value
	}
	else if (!in_range(c, ' ', 0xFF)) /* Self inserting.	*/
	{  /*  TTbeep();*/
		mlwrite(TEXT19);		/* complain 	*/
	/*							 "[Key not bound]" */
		thisflag = 0; 			/* Fake last flags. */
		status = FALSE;
	}
	else
	{ /* If a space was typed, fill column is defined, the argument is non-
		 * negative, wrap mode is enabled, and we are now past fill column,
		 * and we are not read-only, perform word wrap.
		 */
		if (c == ' ' && (curwp->w_bufp->b_flag & (MDVIEW | MDWRAP)) == MDWRAP
								 && fillcol > 0 && getccol() > fillcol
								 && n >= 0)
			execkey(&wraphook, FALSE, 1);

		if (n <= 0) 		/* Fenceposts.	*/
		{ lastflag = 0;
			return n == 0;
		}

		g_overmode = curwp->w_bufp->b_flag & MDOVER;
		status = linsert(n, (char)c); 	/* do the insertion */
		g_overmode = FALSE;
#if 0
						 /* check for CMODE fence matching */
		if ((c == '}' || c == ')' || c == ']') &&
				(curbp->b_flag & MDCMOD))
			fmatch(c);
#endif
		thisflag = 0; 			/* For the future. */
								 /* check auto-save mode */
		if (curbp->b_flag & MDASAVE)
			if (--gacount == 0)
			{ gacount = gasave;
			/*update(TRUE);*/ /* why ?*/
				filesave(FALSE, 0);
			}
	}
	lastflag = thisflag;
	return status;
}


int g_got_uarg = FALSE;
int g_got_search = FALSE;

/*
	This is called to let the user edit something.	Note that if you
	arrange to be able to call this from a macro, you will have
	invented the "recursive-edit" function.
*/
void Pascal editloop(int c_)

{ int f; 	/* default flag */
	int n; 	/* numeric repeat count */
	int c = c_;

					/* if there is something on the command line, clear it */
	if (mpresf != FALSE)
	{ mlerase();
		update(FALSE);
	}
					/* override the arguments if prefixed */
	if (g_prefix)
	{ if (isletter(c & 0xff))
			c &= ~0x20;
		c |= g_prefix;
		f = predef; 		/* pass it on to the next cmd */
		n = prenum;
		g_prefix = 0;
	} 
	else
	{ macro_last_pos = kbdwr;
	  n = 1;
		f = g_got_uarg;
		if (f)
		{ n = univct;
			g_got_uarg = 0;
		}
	}
					/* do META-# processing if needed */
	if ((c & META) && ((unsigned)((c & 0xff) - '0') <= 9 || (c & 0xff) == '-')
								 && getbind(c)->k_code == 0)
	{ n = 0;		/* start with a zero default */
		f = 1; 		/* there is a # arg */
							/* current minus flag */
#define mflag f
		c &= ~META; 	/* strip the META */
		while (true)
		{ c -= '0';
			if			(c == '-'-'0')
			{ if (mflag < 0 || n != 0) /* already hit a minus or digit? */
					break;
				mflag = -1;
			}
			else if ((unsigned)c > 9)
				break;
			else
				n = n * 10 + c;
			
			mlwrite(c == '-'-'0' ? "Arg: -" : "Arg: %d", n * mflag);
			c = getkey(); /* get the next key */
		}
		n *= mflag;
		c += '0';
#undef mflag
	}
				/* and execute the command */
	execute(c, f, n);
}


#if S_MSDOS == 0

/*	Fancy quit command. If the any buffer has changed do a write on 
	that buffer and exit emacs, otherwise simply exit.
*/
Pascal quickexit(int f, int n)

{
	BUFFER *bp;		/* scanning pointer to buffers */
	BUFFER *oldcb = curbp; /* original current buffer */
	int status;

	for (bp = bheadp; bp != NULL; bp = bp->b_bufp) 
	{
		if ((bp->b_flag & (BFCHG+BFINVS)) == BFCHG &&
				bp->b_fname != null)
		{ curbp = bp; 	/* make that buffer cur */
			mlwrite(TEXT103,bp->b_fname);
/*				"[Saving %s]\n" */
			status = filesave(f, n);
			if (status != TRUE)
			{ curbp = oldcb;	/* restore curbp */
				return status;
			}
		}
	}
	return quit(f, n);				/* conditionally quit */
}

#endif


/* Quit command. If an argument, always quit. Otherwise confirm if a buffer
 * has been changed and not written out. Normally bound to "C-X C-C".
 */
Pascal quit(int f, int n)

{ int status = TRUE;
						/* Argument forces it.	*/
	if (! f && anycb())
				/* All buffers clean or user says it's OK. */
		status = mlyesno(TEXT104);
								/* "Modified buffers exist. Leave anyway" */
	if (status)
	{ tcapmove(term.t_nrowm1, 0);
//	g_discmd = true;

#if FILOCK
		if (lockrel() != TRUE)
		{ f = 1;
			n = 1;
		}
#endif
#if S_WIN32 == 0
		tcapclose(0);
#endif
		eexitval = f ? n : GOOD;
		eexitflag = TRUE; /* flag a program exit */
	}
	return status;
}


int rdonly()

{ 	 /* TTbeep(); */
	mlwrite(TEXT109);
/*		"VIEW mode" */
	return FALSE;
}


int resterr()

{ 	 /* TTbeep();*/
	mlwrite(TEXT110);
/*		"[That command is RESTRICTED]" */
	return FALSE;
}

int Pascal nullproc(int f, int n) /* user function that does NOTHING */

{ return OK;
}


int Pascal meta(int f, int n) /* set META prefixing pending */

{ g_prefix |= META;
	prenum = n;
	predef = f;
	return TRUE;
}

int Pascal cex(int f, int n)	/* set ^X prefixing pending */

{ g_prefix |= CTLX;
	prenum = n;
	predef = f;
	return TRUE;
}


int Pascal uniarg(int f, int n) /* set META prefixing pending */

{ char buff[NSTRING+2];

	if (g_clexec)
	{ if (mlreply("", &buff[0], NSTRING) != TRUE)
			return ABORT;
						
		n = atoi(buff);
	}

	univct = n;
	g_got_uarg = TRUE;
	return TRUE;
}
