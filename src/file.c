/*	FILE.C:   for MicroEMACS

	The routines in this file handle the reading, writing
	and lookup of disk files.
*/
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<sys/types.h>
#include	<sys/stat.h>

#define _POSIX_SOURCE 1

#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"base.h"
#include	"msdir.h"
#include	"logmsg.h"

#if S_WIN32
extern Filetime g_file_time;
#else
#include	<unistd.h>
Filetime g_file_time;
#endif


//#undef USE_DIR
//#define USE_DIR S_WIN32


#if S_BORLAND
#include	<dir.h>
#elif S_WIN32
# include  <direct.h>
# include	<io.h>
#else
# include 	<unistd.h>
# include <termios.h>
#endif

static int g_crlfflag;
#define g_crout g_crlfflag

/*extern int sp_langprops;			** inherited */

//extern char * getcwd(char*, int);
// +ve : MSD_DIRY | MSD_MATCHED
// 0 	 : "..."
// -ve : ordinary

int Pascal nmlze_fname(char * tgt, const char * s, char * tmp)
	
{ int search_type = 0;
  			int tix;
  			char ch;
  
  if (*s == '.' && s[1] == '/')
  { s += 2;
   	while (*s == '/')
   		++s;
   	*tgt++ = '.';
   	*tgt++ = '/';
  }
  tix = -1;

	tgt[NFILEN-2] = 0;

  while (tix < NFILEN-2)
  { ch = *s++;
    tgt[++tix] = ch;

    if (ch == 0)
      break;

    if      (ch == '*')
      search_type = MSD_DIRY | MSD_MATCHED;
    else if (ch == '.' && s[0] == '.' && s[1] == '.')
    {	search_type = -(tix + 1);
    	--tix;
    	s += 2 + (s[2] == '/');
    }
    else if (ch == '/' || ch == '\\')
    {	tgt[tix] = '/';
    { int dif = (tix - 3);
      if (dif >= 0)
      { if (tgt[tix-1] == '.')
      	{ if (tgt[tix-2] == '/')			// "/./"
	        { tix -= 2;
	          continue;
	        }
      		if (dif > 0 && tgt[tix-2] == '.' && tgt[tix-4] != '.' && tgt[tix-3] == '/') // "x/../"
	        { tix -= 4;
	        	for (; --tix >= 0 && tgt[tix] != '/'; )
	            ;
	          continue;
	        }
      	} 
			}
    }}
  }
{ const char * cwd_ = getcwd(tmp, NFILEN);
  if (cwd_ == null)
    cwd_ = "/";
	
{	const Char * cw = &cwd_[strlen(cwd_)];
	int num_dirs = 0;
	int root = 0;
	char * t = tgt;

  while (!root && strcmp_right(t, "../") == 0)
  { t += 3;						  											 			/* target forward */
		++num_dirs;

	  while (!(root = (--cw < cwd_)) && *cw != DIRSEPCHAR) /* cwd backward */
      ;
 	}

  if (num_dirs > 0)
	{	int deduct = -1;
		++cw;
		for (; --num_dirs >= 0; )
		{
#if S_WIN32
			const Char * ncw = strmatch(cw, t);				// MSDOS is not case sensitive
#else
		 	const Char * ncw = strsame(cw, t);
#endif
	    if ((*ncw != 0 && *ncw != DIRSEPCHAR) || t[ncw - cw] != '/')
	    	break;
	    
			deduct += 3;
			++t;
			while ((ch = *cw++) != 0 && ch != DIRSEPCHAR)
				++t;
		}

		if (deduct >= 0)
		{ char * s = tgt + deduct;
			char * tt = tgt - 1 + root;
			char ch;
			tgt[0] = '/';
				
			for (; ((ch = *++s) == '.' || ch == '/'); )
				*++tt = ch;

			if (*t == '/')
				++t;
			strcpy(tt+1, t);
		}
	}

  return search_type;
}}}


#if S_MSDOS == 0

int ffisdiry(FILE * ffp)

{ struct stat fstat_;
  return fstat(fileno(ffp), &fstat_) == OK && (fstat_.st_mode & 040000) != 0;
}


time_t ffiletime(FILE * ffp)

{ struct stat fstat_;
  if (fstat(fileno(ffp), &fstat_) != OK)
  	return 0;
  return fstat_.st_mtime;
}

#endif


/*##############  Filename utilities ##############*/

#if S_VMS == 0
#if S_LINUX
extern 	int     g_stdin_fileno;
struct termios  g_savetty;
#endif


																			/* Open a file for reading. */ 
FILE* Pascal ffropen(const char * fn)

{	if (fn[0] != ' ')
		return fopen(fn, "rb");
  else
  { int pipefd = dup(0);
//  if (pipefd < 0)
//    return NULL;
  { FILE * res = fdopen(pipefd, "rb");

    fclose(stdin);
 
  { int fd =
#if S_MSDOS
			       open("CON", O_RDONLY+O_BINARY);
    if (fd != 0)
      return 0;
#else
		         open("/dev/tty", O_RDONLY+O_NOCTTY);
#endif
#if S_LINUX
    if (fd >= 0)
	  { struct termios  tty;
	    speed_t     spd;
			g_stdin_fileno = fd;
	  { int rc = tcgetattr(fd, &tty);
	    if (rc < 0)
	      adb(88);
	
	    g_savetty = tty;    /* preserve original settings for restoration */
	
	    spd = B115200;
	    cfsetospeed(&tty, (speed_t)spd);
	    cfsetispeed(&tty, (speed_t)spd);
	
	    cfmakeraw(&tty);
	
	    tty.c_cc[VMIN] = 1;
	    tty.c_cc[VTIME] = 10;
	
	    tty.c_cflag &= ~CSTOPB;
	//  tty.c_cflag &= ~CRTSCTS;    /* no HW flow control? */
	    tty.c_cflag |= CLOCAL | CREAD;
	    rc = tcsetattr(fd, TCSANOW, &tty);
	    if (rc < 0)
	      adb(89);
	  }}
#endif
		return res;
  }}}
}


#if S_MSDOS
#define BINM O_BINARY+O_TRUNC
#else
#define BINM O_TRUNC
#endif


#if S_LINUX

void stdin_close()

{ if (g_savetty.c_iflag != 0)
    tcsetattr(0, TCSANOW, &g_savetty);
}
    
#endif

#if 0

#define NEW_FILE_MODE 0644

/* Open a file for writing. Return 1 if all is well,
                                   0 if exists and must not
                              and -1 on error (cannot create).
 */
static
FILE * ffwopen(int mode, char * fn)
	
#if S_WIN32
{ return 
}
#else
{ int fd = open(fn, O_RDWR+O_CREAT+BINM+(mode==0 ? 0 : O_EXCL), NEW_FILE_MODE);
  if (fd < 0)
    return NULL;

{	FILE * ffp = fdopen(fd, "wb");
  if (ffp == NULL)
  	close(fd);

  return ffp;
}}
#endif
#endif


/* Write a line to the already opened file. The "buf" points to the buffer,
 * and the "nbuf" is its length, less the free newline. Return the status.
 * Check only at the newline.
 */
/* Write a line to the already opened file. The "buf" points to the buffer,
 * and the "nbuf" is its length, less the free newline. Return the status.
 * Check only at the newline.
 */
static
int Pascal USE_FAST_CALL ffputline(int/* bool*/crypt, int nbuf, char buf[], FILE * op)
	
{	int cc;
#if S_WIN32 == 0
	if (crypt == 0)
  	cc = fwrite(&buf[0], 1, nbuf, op);
	else
#endif
	{ int i;
		cc = 0;
  	for (i = -1; ++i < nbuf; )
	  { char c = buf[i];
	  	if (crypt)
				ucrypt(&c, 1);
  
    	cc |= fputc(c, op);
  	}
  }

  cc |= fputs("\r\n" + 1 - g_crout, op);
//cc |= fputs(g_crout ? "\r\n" : "\n", op);

	if (cc < 0)
		mlwrite(TEXT157 "%x", cc);
					/* "Write I/O error" */

	return cc;
}

static int g_flen;
/*
 * Read a line from a file, and store the bytes in the supplied buffer. The
 * "nbuf" is the length of the buffer. Complain about long lines and lines
 * at the end of the file that don't have a newline present. Check for I/O
 * errors too. Return number of characters in the line.
 */
static
Cc Pascal USE_FAST_CALL ffgetline(int flen, char * * line_ref, FILE * ffp)
	
{ char * line = *line_ref;
	int i = -1; 					/* current index into g_fline */
  int c; 							/* current character read */
											
	do
	{	if (i+1 >= flen)					/* if it's longer, get more room */
	  {													/* lines longer than 16Mb get truncated */
	    flen += NSTRING;
		  line = remallocstr(line_ref, *line_ref, flen+2);
			if (line == NULL)
	      return FIOMEM;

			g_flen = flen;
//  	char * tmpline = (char*)malloc(flen+NSTRING+1);
//  	if (tmpline == NULL)
//      return FIOMEM;

//    if (line != null)
//    { memcpy(tmpline, line, flen);
//      free(line);
//    }
//    line = tmpline;
	  }

		c = getc(ffp);
	  if (c == '\r')
	  { g_crlfflag = MDMS;
	    if (!is_opt('X'))
	      continue;
	  } 

	  line[++i] = c;

		if (c < 0)
		{ 
			if (i <= 0)
				return feof(ffp) ? FIOEOF : FIOERR;
			break;
		}

	} while (c != '\n');

//line[i] = 0;

	return i;
}

#endif

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

{												/* "Insert file" */
  return rdonly() ? FALSE
									: readin(NULL, FILE_LOCK+FILE_REST+FILE_INS);
}


int extract_line(char * s, char alt)

{	int ch;
	for (--s; (ch = *++s) != 0; )																	// strip off :n
  	if ((ch == ':' || ch == alt))
  	{	ch = atoi(s+1);
  		if (ch > 0)
  			break;
		}

	*s = 0;
  return ch;
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

{	char * fname = gtfilename(0);
                      	/* "Find file" */
	if	(fname == NULL || fname[0] <= ' ')
	  return FALSE;

{ int	ch = extract_line(fname, '(');

	BUFFER * bp = bufflink(fname,7); // was bufflkup
	if (bp == NULL)
	  return FALSE;

	if (bp->b_flag & BFACTIVE)
	  mlwrite(TEXT135);
					/* "[Old buffer]" */
	swbuffer(bp);
	if (f)
	{ curwp->w_bufp->b_flag |= MDVIEW;
		upmode();
	}
	
	return ch == 0 ? TRUE : gotoline(1,ch);
}}}


int Pascal viewfile(int f, int n)	/* visit a file in VIEW mode */

{	return filefind(1, 1); 	/* bug: > 2 files => some not view */
}

#if CRYPT == 0
# define resetkey(x)
#endif


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
{	char ch;
  char fullcmd[3*NFILEN+1];
	const char * rnm = &cmdbody[strlen(cmdbody)];
	while ((ch = *--rnm) != '/' && ch != ':')	// Go back to slash or :
		if (rnm == cmdbody) 
	    return NULL;

  if (pw != NULL)
  	strcpy(fullcmd, pw);
  else
  {	--g_macargs;
  	++pd_discmd;
		g_disinp -= 7;
  
  { Cc cc_ = mlreply(TEXT101, fullcmd, 40);
		g_disinp += 7;
	  --pd_discmd;
  	++g_macargs;
	  if (cc_ == 0)
  	  return NULL;
  }}

	mlwrite(TEXT139);	/* "Reading file" */

{ char * cmd = fullcmd + strlen(fullcmd) + 1;
	//cmd[-1] = 1;
{	const char * cmdnm = gtusr("scp");
	int clen = strlen(concat(cmd, cmdnm == NULL ? PSCP_CMD : cmdnm, /*pw,*/" ", cmdbody," ",0));
	const char * tmp = gettmpfn();

  Cc cc = ttsystem(strcat(strcat(cmd,tmp),rnm), fullcmd);
  if (cc != OK)
	{	concat(cmd, cmdnm," Fetch failed ",int_asc(cc), 0);
		props = Q_POPUP;
	}
  else
	{//	sprintf(diag_p, "EKEY %x %s", curbp, ekey == NULL ? "()" : ekey);
	 //	mbwrite(diag_p);
	
		if (bp == NULL)
			bp = bufflink(cmd+clen, 0);
		if (bp != NULL)
		{ bp->b_remote = strdup(fullcmd);											  // allow leak
	    bp->b_key = props & Q_INHERIT ? curbp->b_key : NULL;
	  }
	}
  if (props & Q_POPUP)
		mbwrite(cmd);

  memset(fullcmd, 0, sizeof(fullcmd));
	return bp;
}}}



static
void io_message(const char * txt, int nline)

{ mlwrite("[%s%d%s%s", txt, nline, TEXT143, &"s]"[!nline]);
#if S_MSDOS
{ int row = ttrow;			// unfortunately the window can be scrolled down by 1
  int col = ttcol;
  tcapmove(0,0);
	Sleep(4);
  tcapmove(row,col);
}
#endif
}


// Results: -1: file not found, OK: OK, 1: R/O File, 2: File changed, 8 Multiple Links

Cc do_ftime(BUFFER * bp,
#if S_WIN32 == 0
						FILE * ffp,
#endif
						Bool update)
{
#if S_MSDOS
#define datetime g_file_time
	Cc cc = name_mode(bp->b_fname);
	if (cc < 0)
		return 0;
#else
	Cc cc = OK;
	Filetime datetime = ffp == NULL ? g_file_time : ffiletime(ffp);
#endif
	if (*(Filetime*)&datetime != *(Filetime*)&(bp->b_utime)
	 && *(Filetime*)&bp->b_utime != 0)
		cc = 2;

	if (update)
		bp->b_utime = datetime;

	return cc & ~4;
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
{	static char * g_line = NULL;
	int ins = props & FILE_INS;

  if (props & FILE_REST)
  { if (resterr())
  		return FALSE;
		fname = (char const*)gtfilename(ins);
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
  int got_at = 0;

  const char * s = fname-1;
  while (*++s != 0)
  { if (*s == '@')
      got_at = 2;
  }
  if (s[-1] == '/')
   --got_at;

{ Cc cc = FIOSUC;
	BUFFER * bp = curbp;
  if (got_at > 0)
  { BUFFER * tbp = get_remote(Q_POPUP, bp, NULL, fname);
    if (tbp == NULL)
      --cc;
    else
    { unsigned char ch;
    	char * ss = tbp->b_remote;
      if (ss == NULL || *ss == 0)
        --cc;

      while ((ch = *++ss) > 1)
      	;
      while ((ch = *++ss) != 0)
        if (ch == ' ')
          fname = ss + 1;
//    fname = strdup(fname);     					// allow leak
//    cryptremote(tbp->b_remote);
    }
  }

{ FILE * ffp = cc != FIOSUC ? NULL : ffropen(fname);

#if S_MSDOS == 0
#define FILE_ATTRIBUTE_DIRECTORY S_IFDIR
#endif

#if S_MSDOS
	if (ffp == NULL)			/* File not found. */
#endif
	{ 
#if S_MSDOS
    diry = name_mode(fname) & FILE_ATTRIBUTE_DIRECTORY;
#else
    diry = S_ISDIR(name_mode(fname));
#endif
		if (ffp == NULL)			/* File not found. */
		  if (!diry)
				cc = 1;
			else
				if (got_at == 0)                 /* add trailing slash */
					fname = strcat(strcpy(fnbuff, fname), "/");
	}

  if (cc < 0)
    return FALSE;

	if (!(props & FILE_NMSG))
														/* read the file in */
		mlwrite(cc > 0 ? TEXT138 :
    	    	ins    ? TEXT153 : TEXT139);
/*		        "[No such file]" */
/*			      "[New file]" */
/*            "[Inserting file]" */
/*            "[Reading file]" */

  if (!ins)
  { Cc rc = bclear(bp);
  	if (rc <= FALSE)			/* Changes not discarded */
  	  return rc;

		run_trim(bp, 0);
// 	bp->b_flag &= ~(BFINVS|BFCHG);
		if (!(props & FILE_NMSG))
  	{	if (repl_bfname(bp, fname) == NULL)
	 	  	return ABORT;
	 	}
  			/* let a user macro get hold of things...if he wants */
  	execwrap(0);  // readhook
  }
  
{	int  nline = 0;
  int tabw = 0;
	Cc tcc = do_ftime(bp,		// Set the file date
#if S_WIN32 == 0
									  ffp,
#endif
									  TRUE);
	if (diry)
	{ bp->b_flag |= MDDIR;
		msd_init(fname, MSD_REPEAT | MSD_STAY | MSD_HIDFILE | MSD_SYSFILE);
	}

  if (cc != 0)
    goto out;

  init_paren("(", 0);
	
  g_crlfflag = 0;
//g_flen = 0;

{ LINE * nextline = &bp->b_baseline;
#if	CRYPT
	Bool crypt = !CRYPT ? 0 : bp->b_flag & MDCRYPT;
	if (crypt)
	{
	 	resetkey(&bp->b_key);													/* set up for decryption */
	}
#endif

	if (ins)
	{ 				                          /* back up a line and save the mark here */
	/*if ((curwp->w_dotp->l_props & L_IS_HD) == 0) */
	  curwp->w_line_no -= 1;
 		nextline = curwp->w_dotp;
	  curwp->w_dotp    = lback(curwp->w_dotp);
	  curwp->w_doto    = 0;
    g_paren.in_mode |= curwp->w_dotp->l_dcr & Q_IN_CMT;
	/*curwp->mrks.c[0].markp = lback(curwp->w_dotp);
	  curwp->mrks.c[0].marko = 0;*/
  }

//sp_langprops = bp->b_langprops & BCCOMT;
	while (1)
  { char * ln;
  	LINE *lp1;
	  if (diry)
		{ 
			cc = FIOEOF;
	    ln = msd_nfile();
	    if (ln == NULL)
	      break;
	     
	    /*mlreply(fline, spareline+200, 60);*/

			if (ln[0] == '.' && ln[1] == '/')
				ln += 2;
	    cc = strlen(ln);
	  }
	  else 
		{ ++nline;
			if (g_flen > NSTRING)		/* dump g_fline if it ended up too big */
	  		g_flen = 0;

	  	cc = ffgetline(g_flen, &g_line, ffp);
	  	if (cc < 0)
	  		break;

	  	ln = g_line;
	  }
	  
	  lp1 = mk_line(ln,cc,cc,g_paren.in_mode & Q_IN_CMT);
//  if (lp1 == NULL)
//  {	cc = FIOMEM;
//    break;
//  }

	{ int len = llength(lp1);
#if	CRYPT
		if (crypt)
		{	ucrypt(lp1->l_text, len);
//		if (bp->b_flag & BCRYPT2)
			if (bp->b_flag < 0)
				double_crypt(lp1->l_text, len);
		}
#endif
    scan_par_line(lp1);
		if (nline < 5 &&
				len > 6 && ln[2] == 't' && ln[3] == 'a' && ln[4] == 'b' 
								&&(ln[0] == ln[1]
									 || ln[1] == '*') &&
								  (ln[0] == '/' || ln[0] == '-'))
                   
	  	tabw = atoi(ln+5);

	  ibefore(nextline, lp1);
	}}
//curwp->w_dotp = topline;
#if 0
	if (!line)
	{ free(line);
  	line = NULL;
	}
#endif

{	// extern FILE *g_ffp;		/* File pointer, all functions. */
	if (ffp)
	  (void)fclose(ffp);

	if ((props & FILE_NMSG) == 0)
	{ int txt_o = TEXT140_O + ins * (TEXT154_O - TEXT140_O);
																	/* "Read/Inserted 999 line" */
		if (cc < FIOEOF)
		{ txt_o = cc == FIOEOF ? TEXT141_O : 0;
																	/* "I/O Error, OUT OF MEM */
			bp->b_flag |= BFTRUNC;
	  	(void)repl_bfname(bp, "SHORT");
	  }
	  io_message(TEXTS_+txt_o, nline);
	}
out:
	bp->b_flag |= g_crlfflag & MDMS;

//readin_lines = nline;
	if (ins)
	{    /* advance to the next line and mark the window for changes */
	/*curwp->mrks.c[0].markp = lforw(curwp->mrks.c[0].markp);
	  bp->mrks = curwp->mrks;*/
/*	curwp->w_dotp = lforw(curwp->w_dotp); */
		bp->b_doto = 0;
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
		if (tabw != 0)
	  	bp->b_tabsize = tabw;

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
}}

	return cc < 0;	/* FIOEOF */ /* False if error.	*/
}}}}}



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

	return writeout(fname);
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

  if (bp->b_fname[0] == 0)		/* Must have a name.	*/
	{	mlwrite(TEXT145);
					/* "No file name" */
		return FALSE;
	}
{ char * fn = bp->b_fname;
	char * cmd = bp->b_remote;
  if (cmd != NULL)
	{	fn = cmd;
	{	int sl = strlen(fn);
    if (sl == 0)
	  { mlwrite(TEXT102);
	  	return FALSE;
	  }
//	cryptremote(fn);									// read it

#if _DEBUG
    mbwrite(fn);
#endif
		while (fn[--sl] > ' ')
			;
			
	  fn += sl + 1;
//  for (fn = &fn[sl]; *--fn != ' '; )
//    ;
//  ++fn;
  }}

{	int rc = writeout(fn);
	if (rc > FALSE)
	{ 
	  if (cmd != NULL)
		{ char * ss = fn - 1;												// file precedes it
			*ss = 0;																	// terminate 1st name
		  while (--ss > cmd && *ss > ' ')
		    ;
		  *ss = 0;																	// terminate cmd
		    
			g_disinp -= 7; 							/* turn command input echo off */
//		char * pw = cmd;
		{	int blank = -1;
			while (((unsigned char *)cmd)[++blank] > 1)
				;
			cmd[blank] = 0;
//		--cmd;
//    while (*++cmd != 0 && *cmd != 1)
//    	;
//    *cmd++ = 0;
		    	
//		mbwrite(cmd+3*NFILEN);                  					// Security concerns
				
		{ char cmd_[3*NFILEN+1];
			Cc cc = ttsystem(concat(cmd_,cmd+blank+1, " ",fn," ",ss+1,NULL),cmd);
		  if (cc != 0)
		  { mlwrite(TEXT155" %d", cc);
		  	rc = FALSE;
		  }
		  memset(cmd, 0, blank);
			g_disinp += 7; 							/* turn command input echo on */
//	  pd_sgarbf = TRUE;
		}}}
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
  LINE *lp;
  int nline;	  /* number of lines written */
	char tname[NSTRING+100];	/* temporary file name */
	BUFFER * bp = curbp;

	if (bp->b_flag < 0) // BCRYPT2
	{ mlwrite(TEXT223);
		return FALSE;
	}
				        /* let a user macro get hold of things */
	execwrap(3);  // writehook
          				 
{ FILE * op;                 	/* determine if we will use the save method */
#if S_VMS
  int caution = 0;
#elif S_MSDOS
	int w;
	Cc caution = do_ftime(bp, TRUE); // & (1+8);
  if (caution & (1+2+8))
	{ int yn = mlyesno(caution & 2 ? TEXT29 : caution & 8 ? TEXT218 : TEXT221);
	  if (yn < 0)
  	  return yn;
  	if ((caution & 8) && !yn)
  		caution = -1;
  }
  
//if (*(__int64*)&g_file_time != *(__int64*)&bp->b_utime)
//{ int yn = mlyesno(TEXT29);
//  if (yn <= 0)
// 	  return yn;
//}
#else
  struct stat stat_;
  int caution = stat(fn , &stat_);
  int w = (stat_.st_mode & (getuid() == stat_.st_uid ? 0200 :
                            getgid() == stat_.st_gid ? 020  : 02
                           ));
  if 			(caution < 0)
  	w = MYUMASK;
  else if (stat_.st_nlink > 1 || !w)
  { int yn =  mlyesno(w ? TEXT218 : TEXT221);
    if (yn < 0)
      return yn;
                                   
    if (stat_.st_nlink > 1 && !yn)
      caution = -1;
  }
  
  if (stat_.st_mtime != bp->b_utime)
	{ int yn = mlyesno(TEXT29);
	  if (yn <= 0)
  	  return yn;
	}
#endif
  if (pd_ssave && caution >= 0)
    caution = 1;

	g_crout = (bp->b_flag & MDMS) >> 9;
	if (bp->b_flag & MDCRYPT)
		resetkey(&bp->b_key);
    						   /* Perform Safe Save..... */
              		 /* duplicate original file name, and find where to trunc it */
#if S_MSDOS == 0
	for (nline = 4; --nline >= 0; )/* mk unique name using random numbers */
#endif
	{	char * s = caution <= 0 ? strpcpy(tname, fn, NSTRING-30)
														: mkTempCommName('o', tname);
		op = fopen(tname, S_WIN32 ? "wb" : "w"); // why does wb not work?
		if (op != NULL)
			goto good;
	}

	mlwrite(TEXT155);		/*	"Cannot open file for writing" */
	tcapkopen();						/// reopen the keyboard (Unix only)
	return FALSE;
good:		
	mlwrite(TEXT103,fn);
							/* "[Saving %s]\n" */

												/* write the current buffer's lines to the disk file */
	cc = FIOSUC;
	nline = -1;					/* track the Number of lines		*/
	for (lp = &bp->b_baseline;
			!l_is_hd((lp = lforw(lp))); )
	{ 
	  cc = ffputline(CRYPT == 0 ? 0 : bp->b_flag & MDCRYPT,
	  							 llength(lp),
	  							 &lp->l_text[0], 
									 op);
		if (cc < FIOSUC)
			break;
		++nline;
	}

#if S_WIN32
	fclose(op);
#endif

	bp->b_flag &= ~BFCHG;
	(void)do_ftime(bp,
#if S_WIN32 == 0
								 op,
#endif
								 TRUE);
#if S_WIN32 == 0
	fclose(op);
#endif
	upmode();		/* Update mode lines.	*/
	
{	extern char deltaf[];
	#define mesg deltaf 
//mesg[0] = 0;		/* message buffer */

	if (caution > 0)					/* erase original file */
	{ (void)unlink(fn);
	{	int rc = rename(tname, fn);
		if (rc == 0)
		{ // tname[0] = 0;

			if (! S_MSDOS && w)
				chmod(fn, w);
		}
	}}
//repl_bfname(bp, fn);		// destroys fn
																					 /* report on status of file write */
	io_message(strcpy(tname, TEXT149), nline);
															/* "[Wrote 999 line" */

	tcapkopen();						/// reopen the keyboard (Unix only)
	return cc + 1;
}}}


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
//if (rc == FALSE)
//	fname[0] = 0;

	curbp->b_flag &= ~MDVIEW; 		 /* no longer read only mode */

	repl_bfname(curbp, fname);

	upmode();       /* Update mode lines. */
	return TRUE;
}}



int Pascal fetchfile(int f, int n)

{ LINE * lp = curwp->w_dotp;

	int len = llength(lp) > 2*NFILEN ? 2*NFILEN : llength(lp);
	char cmdline[2*NFILEN+1];
	((char*)memcpy(cmdline, (char*)lp->l_text, len))[len] = 0;
	if (curbp->b_flag < 0) // BCRYPT2
		double_crypt(cmdline, len);

//mbwrite(cmd);
	
{	char * s = cmdline;
	while (*s != 0 && *s != '\t')					  // Go to first tab
		++s;
		
	*s = 0;
		
	while (*++s == '\t')										// skip extra tabs
		;
		
	if (*s == 0)
	{ mbwrite(TEXT222);       // "No Password";
		return FALSE;
	}

//*--s = 0;

{	int encrypt = Q_INHERIT;
	char * pw = s;

	while (*++s != 0)
		if (*s == '\t')					// skip to next tab
		{ *s = 0;
			if (s[1] != 0)
  	  	encrypt = 0;
		}

{ BUFFER * bp = get_remote(encrypt | f, NULL, pw, cmdline);

	memset(cmdline, 0, len);
                          // If the fetch failed continue with any file from last time
{ int rc = swbuffer(bp);
	if (rc < 0)
		return rc;

//sprintf(diag_p, "EKEY %x %s", tbp, tbp->b_key == NULL ? "()" : tbp->b_key);
//mbwrite(diag_p);

	return gotobob(0,0);
}}}}}
