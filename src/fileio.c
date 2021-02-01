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
#include	"msdir.h"

#ifndef S_IFCHR
#define S_IFCHR 0x2000
#endif

/*int confd = 0;*/

char * g_fline = NULL;			/* dynamic return line */
static 
unsigned int g_flen = 0;		/* space available for chars */

static FILE *g_ffp;		/* File pointer, all functions. */

int g_crlfflag;

					/* returns old input */

int Pascal iskboard()

{ struct stat fstat_;
  return fstat(0 , &fstat_) != 0 ? 0 : (fstat_.st_mode & S_IFCHR);
}



#if S_VMS == 0

/*
 * Open a file for reading.
 */
int Pascal ffropen(const char * fn)

{ 		      /* g_flen, fline private to ffropen, ffclose, ffgetline */
  g_flen = 0;		/* force ffgetline to realloc */
  g_crlfflag = FALSE;

  if (fn != NULL && !(fn[0] == '-' && fn[1] == 0))
		g_ffp = fopen(fn, "rb");
  else
  {
#if _WINDOWS
//  g_ffp = fdopen(0, "rb");
		g_ffp = NULL;							// Not needed
	  return FIOSUC;
#else
    int g_pipefd = dup(0);
    if (g_pipefd < 0)
      return 0;
    g_ffp = fdopen(g_pipefd, "rb");
#endif
    fclose(stdin);
 
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
#if S_WIN32
    MySetCoMo();
#endif
  }

  return g_ffp == NULL ? -1 : FIOSUC;
}


#if S_MSDOS
#define BINM O_BINARY+O_TRUNC
#else
#define BINM O_TRUNC
#endif


/* Close a file. Should look at the status in all systems.
 */
int Pascal ffclose()

{ if (g_fline)		/* free this since we do not need it anymore */
  { free(g_fline);
    g_fline = NULL;
  }

{	Cc cc = g_ffp == NULL ? OK : fclose(g_ffp);
#if S_UNIX5 | S_LINUX | S_HPUX | S_SUN | S_XENIX | S_BSD | (S_MSDOS & (LATTICE | MSC | DTL | TURBO)) | S_OS2
  if (cc != OK)
  { mlwrite(TEXT156);
/*			"Error closing file" */
    return FIOERR;
  }
#endif

  return FIOSUC;
}}



/* Open a file for writing. Return 1 if all is well,
                                   0 if exists and must not
                              and -1 on error (cannot create).
 */
int Pascal ffwopen(int mode, char * fn)
	
{ if (fn == null)
  { g_ffp = stdout;
    return OK;
  }
{ int fd;
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
	
{	int i = -1;

#if	CRYPT
	if (curbp->b_flag & MDCRYPT)
	{ while (++i < nbuf)
	  { char c = buf[i];
	    ucrypt(&c, 1);
	    putc(c, g_ffp);
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

	if (g_crlfflag)
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
	
{	int c; 							/* current character read */
	int i = -1; 				/* current index into g_fline */
											
  if (g_flen > NSTRING)		/* dump g_fline if it ended up too big */
	  g_flen = 0;
											/* read the line in */
	*len_ref = 0;
	do
	{ c = getc(g_ffp);		    /* if it's longer, get more room */
//  if (c <= 'Z'- '@')
//  { if (c == 'Z'- '@')
//		  continue;
	    if (c == '\r')
	    { g_crlfflag = TRUE;
	      if ((gflags & MD_KEEP_CR) == 0)
	        continue;
	    } 
//  }
	  if ((unsigned)++i >= g_flen)
	  {
	    if (g_flen >= 0xfffe - NSTRING)
	      return FIOMEM;

	  { char * tmpline = (char*)malloc(g_flen+NSTRING+1);
	    if (tmpline == NULL)
	      return FIOMEM;
	    if (g_fline != null)
	    { memcpy(tmpline, g_fline, g_flen);
	      free(g_fline);
	    }
	    g_fline = tmpline;
	    g_flen += NSTRING;
	  }}
	  g_fline[i] = c;
	} while (c != EOF && c != '\n');

	*len_ref = i;
	g_fline[i] = 0;
				/* test for any errors that may have occured */
	if (c == EOF)
	{ if (ferror(g_ffp))
	  { mlwrite(TEXT158);
/*				"File read error" */
	    return FIOERR;
	  }
	}
					/* terminate and decrypt the string */
#if	CRYPT
	if (curbp->b_flag & MDCRYPT)
	{	ucrypt(g_fline, i);
		if (curbp->b_mode & BCRYPT2)
			double_crypt(g_fline, i);
	}
#endif
	return c == EOF ? FIOEOF : FIOSUC;
}

#endif


//extern char * getcwd(char*, int);


int Pascal nmlze_fname(char * tgt, const char * src, char * tmp)
	
{ int got_star = 0;
  const char * s = src;
  			char * t;
  			char ch;
  
  if (*s == '.' && s[1] == '/')
  { s += 2;
   	while (*s == '/')
   		++s;
   	*tgt++ = '.';
   	*tgt++ = '/';
  }

  t = tgt;

  while (t < &tgt[NFILEN-2])
  { ch = *s++;
    *t++ = ch;
    *t = 0;

    if (ch == 0)
      break;

    if      (ch == '*')
      got_star = MSD_DIRY;
    else if (ch == '/' || ch == '\\')
    {	t[-1] = '/';
    { int dif = (t - tgt) + 4;
      if (dif >= 0)
      { if (t[-2] == '.')
      	{ if (t[-3] == '/')			// "/./"
	        { t -= 2;
	          continue;
	        }
      		if (dif > 0 && t[-5] != '.' && t[-4] == '/' && t[-3] == '.') // "x/../"
	        { for (; --t >= tgt && *t != '/'; )
	            ;
	          t += 1;
	          continue;
	        }
      	} 
			}
    }}
  }
{
#if S_WIN32 && S_BORLAND == 0 && S_CYGWIN == 0
  const char * cwd_ = _getcwd(tmp, NFILEN);
#else
  const char * cwd_ = getcwd(tmp, NFILEN);
#endif
  if (cwd_ == null)
    cwd_ = "/";
	
{ const Char * cwdend = &cwd_[strlen(cwd_)];
  const Char * cw = cwdend;
  int num_dirs = 0;
  t = tgt;

  while (strcmp_right(t, "../") == 0 && cw >= cwd_)
  { t += 3;						  											 /* target forward */
    while (--cw >= cwd_ && *cw != DIRSEPCHAR)	 /* cwd backward */
      ;

		++num_dirs;
	}

{ int root = cw < cwd_;

	if (num_dirs > 0)
	{	int deduct = -1;
		for (; --num_dirs >= 0; )
		{	const Char * ncw;
#if S_WIN32
			ncw = strmatch(cw+1, t);				// MSDOS is not case sensitive
#else
		 	ncw = strsame(cw+1, t);
#endif
	    if ((*ncw != 0 && *ncw != DIRSEPCHAR) || t[ncw - cw - 1] != '/')
	    	break;
	    
			deduct += 3;
			t = &t[ncw - cw];
			while (*++cw != 0 && *cw != DIRSEPCHAR)
				;
		}

		if (deduct >= 0)
		{ char * tt = tgt - 1;
			char * s;
			if (root)
				*++tt = '/';
				
			for (s = tgt + deduct; *++s != 0 && (*s == '.' || *s == '/'); )
				*++tt = *s;

			if (*t == '/')
				++t;
			strcpy(tt+1, t);
		}
	}
		
  return got_star;
}}}}



#if S_MSDOS == 0

int Pascal ffisdiry()

{ struct stat fstat_;
  return fstat(fileno(g_ffp), &fstat_) == OK && (fstat_.st_mode & 040000) != 0;
}

#endif
