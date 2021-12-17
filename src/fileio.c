/*  FILEIO.C:	Low level file i/o routines
		MicroEMACS 3.10

 * The routines in this file read and write ASCII files from the disk. All of
 * the knowledge about files are here.
 */

#define _POSIX_SOURCE


#include	<stdio.h>
#include	<stdlib.h>
#include	<fcntl.h>
#include	<errno.h>
#include 	<sys/types.h>
#include	<sys/stat.h>

#include	"estruct.h"

#if S_BORLAND
#include	<dir.h>
#elif S_WIN32
# include  <direct.h>
# include	<io.h>
#else
# include 	<unistd.h>
# include <termios.h>
#endif
#include	"base.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"msdir.h"

#ifndef S_IFCHR
#define S_IFCHR 0x2000
#endif

#if S_WIN32
#define getcwd _getcwd
#endif

/*int confd = 0;*/

char * g_fline = NULL;			/* dynamic return line */
static 
unsigned int g_flen = 0;		/* space available for chars */

FILE *g_ffp;		/* File pointer, all functions. */

int g_crlfflag;

					/* returns old input */

int Pascal iskboard()

{ struct stat stat_;
  return fstat(0 , &stat_) != 0 ? 0 : (stat_.st_mode & S_IFCHR);
}



#if S_VMS == 0

#if S_LINUX
struct termios  g_savetty;
#endif

																			/* Open a file for reading. */
int Pascal ffropen(const char * fn)

{ 		      		/* g_flen, fline private to ffropen, ffclose, ffgetline */
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
 
  { int fd =
#if S_MSDOS
        open("CON", O_RDONLY+O_BINARY);
#else
        open("/dev/tty", O_RDONLY+O_NOCTTY);
#endif
    if (fd != 0)
    {
      return 0;
    }
#if S_LINUX
  { struct termios  tty;
    speed_t     spd;
    unsigned char   buf[80];
    int     reqlen = 79;
    int     rdlen;
    int     pau = 0;
    int rc = tcgetattr(fd, &tty);
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
  }
#endif
  }}

  return g_ffp == NULL ? -1 : FIOSUC;
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

#define NEW_FILE_MODE 0644

/* Open a file for writing. Return 1 if all is well,
                                   0 if exists and must not
                              and -1 on error (cannot create).
 */
FILE * ffwopen(int mode, char * fn)
	
{ if (fn == null)
  { g_ffp = stdout;
    return OK;
  }
{
#if S_MSDOS && S_WIN32 == 0
  char afn[266];
  fn = LFN_to_8dot3(LFN_to_83, 0, fn, &afn[0]);
#endif
{ int fd = open(fn, O_RDWR+O_CREAT+BINM+(mode == 0 ? 0 : O_EXCL), NEW_FILE_MODE);
  if (fd < 0)
    return NULL;

{	FILE * ffp = fdopen(fd, "wb");
  if (ffp == NULL)
  	close(fd);

  return ffp;
}}}}


/* Write a line to the already opened file. The "buf" points to the buffer,
 * and the "nbuf" is its length, less the free newline. Return the status.
 * Check only at the newline.
 */
int Pascal ffputline(FILE * op, char buf[], int nbuf)
	
{	int i = -1;
	int cc = 0;

#if S_MSDOS
  while (++i < nbuf)
  { char c = buf[i];
#if	CRYPT
		if (curbp->b_flag & MDCRYPT)
		  ucrypt(&c, 1);
#endif
  
    cc |= fputc(c, op);
  }
#else
  fwrite (&buf[0], 1, nbuf, op);
#endif

	cc |= fputs(g_crlfflag ? "\r\n" : "\n", op);

	if (cc < 0)
	{ mlwrite(TEXT157);
					/* "Write I/O error" */
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
	      if (!is_opt('X'))
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

  t = tgt-1;

  while (t < &tgt[NFILEN-2])
  { ch = *s++;
    *++t = ch;

    if (ch == 0)
      break;

    if      (ch == '*')
      got_star = MSD_DIRY;
    else if (ch == '/' || ch == '\\')
    {	t[0] = '/';
    { int dif = (t - tgt) - 3;
      if (dif >= 0)
      { if (t[-1] == '.')
      	{ if (t[-2] == '/')			// "/./"
	        { t -= 2;
	          continue;
	        }
      		if (dif > 0 && t[-2] == '.' && t[-3] == '/' && t[-4] != '.' ) // "x/../"
	        { t -= 4;
	        	for (; --t >= tgt && *t != '/'; )
	            ;
	          continue;
	        }
      	} 
			}
    }}
  }

  t[1] = 0;
{
  const char * cwd_ = getcwd(tmp, NFILEN);
  if (cwd_ == null)
    cwd_ = "/";
	
{ const Char * cw = &cwd_[strlen(cwd_)];
  int num_dirs = 0;
  int root = 0;
  t = tgt;

  while (strcmp_right(t, "../") == 0)
  { t += 3;						  											 /* target forward */
		++num_dirs;

    while (!(root = (--cw < cwd_)) && *cw != DIRSEPCHAR)	 /* cwd backward */
      ;
      
		if (root)
			break;
	}

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
			++t;
			while (*++cw != 0 && *cw != DIRSEPCHAR)
				++t;
		}

		if (deduct >= 0)
		{ char * tt = tgt - 1;
			char * s;
			char ch;
			if (root)
				*++tt = '/';
				
			for (s = tgt + deduct; ((ch = *++s) == '.' || ch == '/'); )
				*++tt = ch;

			if (*t == '/')
				++t;
			strcpy(tt+1, t);
		}
	}
		
  return got_star;
}}}


#if S_MSDOS == 0

int Pascal ffisdiry()

{ struct stat fstat_;
  return fstat(fileno(g_ffp), &fstat_) == OK && (fstat_.st_mode & 040000) != 0;
}

#endif
