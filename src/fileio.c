/*  FILEIO.C:	Low level file i/o routines
		MicroEMACS 3.10

 * The routines in this file read and write ASCII files from the disk. All of
 * the knowledge about files are here.
 */

#define _POSIX_SOURCE


#include	<stdio.h>
#include	<stdlib.h>
#include	<io.h>
#include	<fcntl.h>
#include	<errno.h>
#include 	<sys/types.h>
#include	<sys/stat.h>

#include	"estruct.h"

#if S_BORLAND
#include	<dir.h>
#elif S_WIN32
#include    <direct.h>
#else
#include 	<unistd.h>
#endif
#include	"base.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"h/msdir.h"

#ifndef S_IFCHR
#define S_IFCHR 0x2000
#endif

int pipefd = 0;			/* 0 is stdin */
/*int confd = 0;*/


NOSHARE FILE *g_ffp;		/* File pointer, all functions. */
static int eofflag;		/* end-of-file flag */
int crlfflag;


					/* returns old input */

int Pascal iskboard()

{ struct stat fstat_;
  register int cc = fstat(0 , &fstat_);
  return cc == 0 && (fstat_.st_mode & S_IFCHR) ? 1 : 0;
}



#if S_VMS == 0

/*
 * Open a file for reading.
 */
int Pascal ffropen(char * fn)

{ 
  if (fn != NULL && !(fn[0] == '-' && fn[1] == 0))
      g_ffp = fopen(fn, "rb");
  else
  {
#if _WINDOWS
    g_ffp = fdopen(0, "rb");
#else
    pipefd = dup(0);
    if (pipefd < 0)
      return 0;
    g_ffp = fdopen(pipefd, "rb");
#endif
    fclose(stdin);
 
#if S_WIN32
    MySetCoMo();
#else
    if (
#if S_MSDOS
        open("CON", O_RDONLY+O_BINARY)
#else
        open("/dev/tty", O_RDONLY)
#endif
         != 0)
    {
      return 0;
    }
#endif
  }

  if (g_ffp == NULL)
     return -1;
		      /* g_flen, fline private to ffropen, ffclose, ffgetline */
  g_flen = 0;		/* force ffgetline to realloc */
  eofflag = FALSE;
  crlfflag = FALSE;
  return FIOSUC;
}


#if S_MSDOS
#define BINM O_BINARY+O_TRUNC
#else
#define BINM O_TRUNC
#endif


/* Close a file. Should look at the status in all systems.
 */
int Pascal ffclose()

{ if (fline)		/* free this since we do not need it anymore */
  { free(fline);
    fline = NULL;
  }

#if S_UNIX5 | S_LINUX | S_HPUX | S_SUN | S_XENIX | S_BSD | (S_MSDOS & (LATTICE | MSC | DTL | TURBO)) | S_OS2
  if (fclose(g_ffp) != FALSE)
  { mlwrite(TEXT156);
/*			"Error closing file" */
    return FIOERR;
  }
#else
  fclose(g_ffp);
#endif

  return FIOSUC;
}



/* Open a file for writing. Return 1 if all is well,
                                   0 if exists and must not
                              and -1 on error (cannot create).
 */
int Pascal ffwopen(int mode, char * fn)
	
{ if (fn == null)
  { g_ffp = stdout;
    return OK;
  }
{ 
  int fd;
#if S_MSDOS && S_WIN32 == 0
  char afn[266];
  fd = open(LFN_to_8dot3(LFN_to_83, 0, fn, &afn[0]), 
  		O_RDWR+O_CREAT+BINM+(mode == 0 ? 0 : O_EXCL), 0755);
#else
  fd = open(fn, O_RDWR+O_CREAT+BINM+(mode == 0 ? 0 : O_EXCL), 0755);
#endif
  if (fd < 0)
    return errno == EEXIST ? 0 : -1;

  g_ffp = fdopen(fd, "wb");
  if (g_ffp == NULL)
  { close(fd);
    return -1;
  }
  return 1;
}}


/* Write a line to the already opened file. The "buf" points to the buffer,
 * and the "nbuf" is its length, less the free newline. Return the status.
 * Check only at the newline.
 */
int Pascal ffputline(char buf[], int nbuf)
	
{
	register int i = -1;
#if	CRYPT

	if (curbp->b_flag & MDCRYPT)
	{ while (++i < nbuf)
	  { char c[1]; 	/* character to translate */
	    c[0] = buf[i];
	    ucrypt(&c[0], 1);
	    putc(c[0], g_ffp);
	  }
	} 
	else
#endif
#if S_MSDOS
	  while (++i < nbuf)
	    putc(buf[i], g_ffp);
#else
	  fwrite (&buf[0], 1, nbuf, g_ffp);

#endif

	if (crlfflag)
	  putc('\r', g_ffp);

	putc('\n', g_ffp);

	if (ferror(g_ffp))
	{ mlwrite(TEXT157);
/*			"Write I/O error" */
	  return FIOERR;
	}

	return FIOSUC;
}

/*
 * Read a line from a file, and store the bytes in the supplied buffer. The
 * "nbuf" is the length of the buffer. Complain about long lines and lines
 * at the end of the file that don't have a newline present. Check for I/O
 * errors too. Return status.
 */
int Pascal ffgetline(int * len_ref)
	
{	register int c; 	/* current character read */
	register int i = -1; 	/* current index into fline */

	if (eofflag)			/* if we are at the end...return it */
	  return FIOEOF;
					/* dump fline if it ended up too big */
        if (g_flen > NSTRING)
	  g_flen = 0;
							/* read the line in */
	*len_ref = 0;
	do
	{ c = getc(g_ffp);		    /* if it's longer, get more room */
	  if (c <= 'Z'- '@')
	  { if (c == '\r')
	    { crlfflag = TRUE;
	      if ((gflags & MD_KEEP_CR) == 0)
	         continue;
	    } 
	    if (c == 'Z'- '@')
	      continue;
	  }
	  if ((unsigned)++i >= g_flen)
	  {
#if S_MSDOS
	    if (g_flen >= 0xfe00)
	      return FIOMEM;
#endif
	  { char * tmpline = (char*)malloc(g_flen+NSTRING+1);
	    if (tmpline == NULL)
	      return FIOMEM;
	    if (fline != null)
	    { memcpy(tmpline, fline, g_flen);
	      free(fline);
	    }
	    fline = tmpline;
	    g_flen += NSTRING;
	  }}
	  fline[i] = c;
	} while (c != EOF && c != '\n');

	*len_ref = i;
	fline[i] = 0;
				/* test for any errors that may have occured */
	if (c == EOF)
	{ if (ferror(g_ffp))
	  { mlwrite(TEXT158);
/*				"File read error" */
	    return FIOERR;
	  }

	  if (i == 0)
	    return FIOEOF;
	  eofflag = TRUE;
	}
					/* terminate and decrypt the string */
#if	CRYPT
	if (curbp->b_flag & MDCRYPT)
	  ucrypt(fline, strlen(fline));
#endif
	return FIOSUC;
}

#endif


//extern char * getcwd(char*, int);


int Pascal nmlze_fname(char * tgt, char * src, char * tmp)
	
{ register char * t = tgt;
  register char * s = src;
  register char ch;
       int got_star = 0;

  while (t < &tgt[NFILEN])
  { ch = *s++;
    *t++ = ch;
    *t = 0;

    if (ch == 0)
      break;
    if (ch == '\\')
      ch = '/';

    if      (ch == '*')
      got_star = MSD_DIRY;
    else if (ch == '/')
    { t -= 4;
      if (t >= tgt)
      { if (strcmp(t+1, "/./") == 0)		/* /./ -> / */
        { t += 2;
          continue;
        }
        if (strcmp(t, "/../") == 0 && t[-1] != '.')	/* take out dir/.. */
        { for (; --t >= tgt && *t != '/'; )
            ;
          t += 1;
          continue;
        }
      }
      t += 4;
    }
  }
  
  #define cwd_ s
#if S_WIN32 && S_BORLAND == 0 && S_CYGWIN == 0
  cwd_ = _getcwd(tmp, NFILEN);
#else
  cwd_ = getcwd(tmp, NFILEN);
#endif
  if (cwd_ == null)
    cwd_ = "/";
{ Char * cwdend = &cwd_[strlen(cwd_)];
  Char * cw = cwdend;
  t = tgt;

  while (*strmatch("../", t) == 0 && cw >= cwd_)
  { tgt += 3;						  /* target forward */
    while (--cw >= cwd_ && *cw != '/' && *cw != '\\') /* cwd backward */
      ;
  }

  if (cw < cwdend)
  { const Char * ncw = strmatch(cw+1, t);
    if ((*ncw == 0 || *ncw == '\\' || *ncw == '/') && t[ncw - cw - 1] == '/')
    { s = t;					/* both go down the same */
      
      for ( ; ++cw <= ncw && t > tgt; )	/* you have to think it out!*/
      { ++s;
        if (*cw == 0 || *cw == '\\' || *cw == '/')
          t -= 3;
      }
      strcpy(t, s);
    }
  }

  return got_star;
}}




#if S_MSDOS == 0

int Pascal ffisdiry()

{ struct stat fstat_;
  return fstat(fileno(g_ffp), &fstat_) == OK && (fstat_.st_mode & 040000) != 0;
}

#endif

