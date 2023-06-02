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

// FILE *g_ffp;		/* File pointer, all functions. */

int g_crlfflag;

#if S_VMS == 0

#if S_LINUX
extern 	int     g_stdin_fileno;
struct termios  g_savetty;
#endif

																			/* Open a file for reading. */
FILE* Pascal ffropen(const char * fn)

{ 		      					/* g_fline private to ffropen, ffclose, ffgetline */
  g_crlfflag = 0;

  if (!(fn[0] == '-' && fn[1] == 0))
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
int Pascal ffputline(FILE * op, char buf[], int nbuf)
	
{	int cc;
	int crypt = CRYPT == 0 ? 0 : curbp->b_flag & MDCRYPT;
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

	cc |= fputs(g_crlfflag ? "\r\n" : "\n", op);

	if (cc < 0)
	{ mlwrite(TEXT157 "%x", cc);
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
int Pascal ffgetline(FILE * ffp)
	
{ static 
   int g_flen;
  int flen = g_flen;
  if (flen > NSTRING)		/* dump g_fline if it ended up too big */
	  flen = 0;

{	int i = -1; 				/* current index into g_fline */
	char * line = g_fline;
  int c; 							/* current character read */
											
	do
	{ c = getc(ffp);		    /* if it's longer, get more room */
											/* test for any errors that may have occured */
		if (c < 0)
		{ if (!feof(ffp))
		  { mlwrite(TEXT158);
							/* "File read error" */
		    return FIOERR;
		  }
		  return FIOEOF;
		}
	  if (c == '\r')
	  { g_crlfflag = MDMS;
	    if (!is_opt('X'))
	      continue;
	  } 
	  if (++i >= flen)
	  {														/* lines longer than 16Mb get truncated */
	    flen += NSTRING;
			/*line = */remallocstr(&line, line, flen);
			if (line == NULL)
	      return FIOMEM;

			g_fline = line;
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
	  line[i] = c;
	} while (c != '\n');

//line[i] = 0;

	return i;
}}

#endif

//extern char * getcwd(char*, int);
// +ve : MSD_DIRY | MSD_MATCHED
// 0 	 : "..."
// -ve : ordinary

int Pascal nmlze_fname(char * tgt, const char * src, char * tmp)
	
{ int search_type = 0;
  const char * s = src;
  			int tix;
  			char ch;
  
  if (*s == '.' && s[1] == '/')
  { s += 2;
   	while (*s == '/')
   		++s;
   	*tgt++ = '.';
   	*tgt++ = '/';
  }
#if 0
  t = tgt-1;

	tgt[NFILEN-2] = 0;

  while (t < &tgt[NFILEN-2])
  { ch = *s++;
    *++t = ch;

    if (ch == 0)
      break;

    if      (ch == '*')
      search_type = MSD_DIRY | MSD_MATCHED;
    else if (ch == '.' && s[0] == '.' && s[1] == '.')
    {	search_type = (tgt - t) - 1;
    	--t;
    	s += 2 + (s[2] == '/');
    }
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
#else
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
#endif
{ const char * cwd_ = getcwd(tmp, NFILEN);
  if (cwd_ == null)
    cwd_ = "/";
	
{ const Char * cw = &cwd_[strlen(cwd_)];
	int num_dirs = 0;
  int root = 0;
	char * t = tgt;

  while (strcmp_right(t, "../") == 0)
  { t += 3;						  											 			/* target forward */
		++num_dirs;

	  while (!(root = (--cw < cwd_)) && *cw != DIRSEPCHAR) /* cwd backward */
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
			while ((ch = *++cw) != 0 && ch != DIRSEPCHAR)
				++t;
		}

		if (deduct >= 0)
		{ char * s = tgt + deduct;
			char * tt = tgt - 1;
			char ch;
			if (root)
				*++tt = '/';
				
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

int Pascal ffisdiry(FILE * ffp)

{ struct stat fstat_;
  return fstat(fileno(ffp), &fstat_) == OK && (fstat_.st_mode & 040000) != 0;
}

#endif
