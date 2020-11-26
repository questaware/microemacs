/*	UNIX.C:	Operating specific I/O and Spawning functions
		under UNIX V7, BSD4.2/3, System V, SUN OS and SCO XENIX
		for MicroEMACS 3.10
		(C)opyright 1988 by Daniel M. Lawrence
*/

/*#define _POSIX_SOURCE*/
#ifndef _ALL_SOURCE
 #define _ALL_SOURCE
#endif
/*#define _NO_PROTO*/
	
#include	<stdio.h>
#include	<stdlib.h>
#include   	<termio.h>
#include   	<sys/select.h>
#include        <sys/time.h>
#undef CTRL
#include	"estruct.h"
#include	"build.h"
# include	"edef.h"
#include	"etype.h"
# include	"elang.h"
#undef	CTRL


#if	(S_BSD | S_UNIX5 | S_HPUX | S_SUN | S_XENIX) == 0
 MUST DEFINE A UNIX SYSTEM
#endif 


#if	S_UNIX5 | S_HPUX | S_SUN | S_XENIX			/* System V */
/*#  include	<termio.h>*/
#  include	<fcntl.h>
#  include      <sys/types.h>
#  include	<sys/stat.h> 
#endif
#if V7 | S_BSD
/* I hit a system name here... we have to define it back to what
   emacs expacts */
#  include	<sgtty.h>        /* for stty/gtty functions */
#  include	<sys/types.h>
#  include	<sys/ndir.h>
#  include	<sys/stat.h>
 struct  sgttyb  ostate;          /* saved tty state */
 struct  sgttyb  nstate;          /* values for editor mode */
 struct tchars	otchars;	/* Saved terminal special character set */
 struct tchars	ntchars = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
#undef	CTRL
#define CTRL	0x0100		/* Control flag, or'ed in		*/

#if S_BSD
#include <sys/ioctl.h>		/* to get at the typeahead */
extern	int rtfrmshell();	/* return from suspended shell */
#define	TBUFSIZ	128
char tobuf[TBUFSIZ];		/* terminal output buffer */
#endif
#endif  /* V7 or System V */

#include        <signal.h>


#define  STDIPCHAN 0 /* not fileno(stdin), it's closed */

  static int  kbdflgs;			/* saved keyboard fd flags	*/
  static int  kbdqp;			/* how many chars in kbdq	*/
  static char kbdq;			/* char we've already read	*/
  static struct	termio	otermio;	/* original terminal characteristics */
  static struct	termio	ntermio;	/* charactoristics to use inside */



extern  char * getenv();

#if S_XENIX

char * memmove(t_, s_, len_)
      char * t_, *s_;
      int    len_;
{ register int len = len_;
  register char * t = t_;
  register char * s = s_;
  
  if (t <= s)
    while (--len >= 0)
      *t++ = *s++;
  else 
  { t = &t[len];
    s = &s[len];
    
    while (--len >= 0)
      *--t = *--s;
  }
  return t_;
}

#endif

#if 0

sprintf()

{ char ch;
  fputs("printfX", stdout);
  read(STDIPCHAN, &ch, 1);
}


fprintf()

{ sprintf();
}


printf()

{ sprintf();
}

#endif

				/* must be int for signal() */
void tidyup(int n)

{ if (fexist("/tmp/emacssv"))
    quickexit(0,0);
  else
    puts("\f\n file /tmp/emacssv would have caused save\n\r");
  tcapclose();
  exit(1);
}




int Pascal millisleep(unsigned int n)

{ 
#if S_HPUX
    extern nanosleep(long nsecs);
    nanosleep(1000000L * n);
#elif S_LINUX
    extern usleep(int usecs);
    usleep(1000 * n);
#else
#if S_CC_C89AIX
  struct timeval32 tvs;
#else
  struct timeval tvs;
#endif
  tvs.tv_sec = 0;
  tvs.tv_usec = n * 1000;
  select(0, null, null, null, &tvs);
#endif
}



#if	S_UNIX5 | S_HPUX | S_SUN | S_XENIX

int Pascal setttyflags(int flags)
	
{ int iter;
  if (flags != kbdflgs)
  { for (iter = 20; --iter >= 0; )
    {
#ifdef F_SETW
      if (fcntl(0, F_SETFLW, flags) >= OK)
#else
      if (fcntl(0, F_SETFL, flags) >= OK)
#endif
        break;
      millisleep(20);
    }
    if (iter >= 0)
      kbdflgs = flags;
  }
#if 0
  if ((fcntl(0, F_GETFL, flags) ^ fkbflags) & O_NDELAY)
  { mlwrite("KBD polling error%w");
    adb(40);
  }
#endif
  return kbdflgs;
}

#endif

void serialopen()

{
#if	S_UNIX5 | S_HPUX | S_SUN | S_XENIX
	ioctl(0, TCGETA, &otermio);	/* save old settings */
#if 0
	ntermio.c_iflag = 0;		/* setup new settings */
	ntermio.c_oflag = 0;
	ntermio.c_lflag = 0;
	ntermio.c_cc[VTIME] = 0;
#endif
	ntermio.c_cflag = otermio.c_cflag;
	ntermio.c_line = otermio.c_line;
	ntermio.c_cc[VMIN] = 1;
	ioctl(0, TCSETAW, &ntermio);	/* and activate them */

#elif   S_BSD
        gtty(0, &ostate);                       /* save old state */
        gtty(0, &nstate);                       /* get base of new state */
        nstate.sg_flags |= RAW;
        nstate.sg_flags &= ~(ECHO|CRMOD);       /* no echo for now... */
        stty(0, &nstate);                       /* set mode */
	ioctl(0, TIOCGETC, &otchars);		/* Save old characters */
	ioctl(0, TIOCSETC, &ntchars);		/* Place new character into K */

			/* provide a smaller terminal output buffer so that
	   	   	   the type ahead detection works better (more often) */
	setbuffer(stdout, &tobuf[0], TBUFSIZ);
	signal(SIGTSTP,SIG_DFL);	/* set signals so that we can */
	signal(SIGCONT,rtfrmshell);	/* suspend & restart emacs */
#endif
	signal(SIGTERM,tidyup);
		/* we are not sure of the initial position of the cursor */
/*	ttrow = 2;     ** must be in range **
	ttcol = 2;*/
}

			/* This function gets called just before we go
			 * back home to the command interpreter.	 */
void serialclose()

{
#if	S_UNIX5 | S_HPUX | S_SUN | S_XENIX
	ioctl(0, TCSETA, &otermio);	/* restore terminal settings */
	setttyflags(kbdflgs & ~O_NDELAY);
#endif

#if     V7 | S_BSD
        stty(0, &ostate);
	ioctl(0, TIOCSETC, &otchars);	/* Place old character into K */
#endif
}

/*
 * Write a character to the display. 
 */
ttputc(char c)

{
  static short sent;
#if 0
  static FILE * ppop;

  if (ppop == 0)
    ppop = fopen("/tmp/ppop", "w");
  if (ppop != 0)
    putc(c, ppop);
  fflush(ppop);
#endif
#if TELNETMAXCHARS 
  if (--sent < 0)
  { sent = TELNETMAXCHARS;
    fflush(stdout);
    millisleep(100);
  }
#endif
  /* write(1, &c, 1);   ** dont use this, it's not buffered */
  write(1, &c, 1);
/*putc(c, stdout);*/
#if 0
    fflush(stdout);
#endif
}

/*
 * Flush terminal buffer.
 */
ttflush()
{
        fflush(stdout);
}

static int  eaten_char = -1;		 /* Re-eaten char */


void Pascal reeat(int c)

{
    eaten_char = c;			/* save the char for later */
}
	    	     /* TTGETC:	Read a character from the terminal, performing no
			editing and doing no echo at all.
	      */
int ttgetraw()

{	char c;
	if (eaten_char != -1)
        { int c = eaten_char;
          eaten_char = -1;
          return c;
        }
  
#if     V7 | S_BSD
        read(STDIPCHAN, &c, 1);
	return 255 & (int)c;

#elif	S_UNIX5 | S_HPUX | S_SUN | S_XENIX
	if (kbdqp > 0)
	    kbdqp = 0;
	else
	{		/* we desperatly seek a character so we turn off
		   	   the NO_DELAY flag and simply wait for the bastard*/
	    setttyflags(kbdflgs & ~O_NDELAY);
	    if ((kbdflgs & O_NDELAY) || read(STDIPCHAN, &kbdq, 1) < 0)
	      return 0;
	}
#if 0
{ static FILE * ppop;

  if (ppop == 0)
    ppop = fopen("/tmp/ppop", "w");
  /* write(1, &c, 1);   ** dont use this, it's not buffered */
  if (ppop != 0)
    fprintf(ppop, "C %x ", kbdq);
}    
#endif  
	return kbdq & 255;
#endif
}


#if	TYPEAH
/* typahead:	Check to see if any characters are already in the
		keyboard buffer
*/

typahead()

{
#if 0
	return FALSE;
#elif	S_BSD || S_LINUX
	int x;	/* holds # of pending chars */

	return ioctl(0,FIONREAD,&x) < 0 ? 0 : x;
#elif	S_UNIX5 | S_HPUX | S_SUN | S_XENIX

	if (kbdqp <= 0)			/* set O_NDELAY */
	   
	{ setttyflags(kbdflgs | O_NDELAY);
	  kbdqp = read(STDIPCHAN, &kbdq, 1);
	}
	return kbdqp;
#else
	return FALSE;
#endif
}
#endif

#if	S_BSD

bktoshell()		/* suspend MicroEMACS and wait to wake up */

{ int pid = getpid();
  tcapclose();
  kill(pid,SIGTSTP);
}

rtfrmshell()

{ ttopen();
  curwp->w_flag = WFHARD;
  sgarbf = TRUE;
}
#endif

static void usehost(line, end)
	char *   line;
	char	 end;
{ TTflush();
  tcapclose();
  if (!g_clexec)
    ttputc('\n');                /* Already have '\r'    */

  system(line);
  if (end)
  {
    putpad(TEXT188);
       /*     "[End]" */
    TTflush();
  }

  ttopen();
  if (end)
#if 1
    tgetc();
#else
  { char s;
    while ((s = tgetc()) != '\r' && s != ' ')
     ;
  }
#endif
  sgarbf = TRUE;
}


int gen_spawn(f, n, prompt, line)
	char *   prompt;
	char *   line;
{ register int  s;

  if (restflag)
    return resterr();
  
  if (prompt == NULL)
    tcapmove(term.t_nrowm1, 0);             /* Seek to last line.   */
  else
  { s=mlreply(prompt, line, NLINE);
    if (s != TRUE)
      return s;
  }

  usehost(line, prompt != NULL and !g_clexec);

  ttcol = 0;  		   /* otherwise we shall not see the message */
			   /* if we are interactive, pause here */
  return TRUE;
}

/* Create a subjob with a copy of the command intrepreter in it. When the
 * command interpreter exits, mark the screen as garbage so that you do a full
 * repaint. Bound to "^X C". The message at the start in VMS puts out a newline.
 * Under some (unknown) condition, you don't get one free when DCL starts up.
 */
spawncli(f, n)

{ register char *cp = getenv("SHELL");
  if (cp == NULL or *cp == 0)
#if	S_BSD
    cp = "exec /bin/csh";
#else
    cp = "exec /bin/sh";
#endif
  return gen_spawn(f, n, NULL, cp);
}


/* Run a one-liner in a subjob. When the command returns, wait for a single
 * character to be typed, then mark the screen as garbage so a full repaint is
 * done. Bound to "C-X !".
 */

spawn(f, n)

{ char line[NLINE];

  return gen_spawn(f, n, "!", line);
}

/* Run an external program with arguments. When it returns, wait for a single
 * character to be typed, mark the screen as garbage so a full repaint is done.
 * Bound to "C-X $".
 */
execprg(f, n)

{ return spawn(f, n);
}


/* Pipe a one line command into a window
 * Bound to ^X @
 */
pipecmd(f, n)

{	register WINDOW *wp;	/* pointer to new window */
	register BUFFER *bp;	/* pointer to buffer to zot */
        char	line[NLINE];	/* command line send to shell */

 static char bname[] = "command";
 static char filnam[] = "command";

	if (restflag)
	  return resterr();

{       int s = mlreply("@", line, NLINE);     /* get the command to pipe in */
	if (s != TRUE)
          return s;
			/* get rid of the command output buffer if it exists */
        bp = bfind(bname, FALSE, 0);
	if (bp != FALSE) 
	{		/* try to make sure we are off screen */
	  for (wp = wheadp; wp != NULL; wp = wp->w_wndp)
	    if (wp->w_bufp == bp)
	    { onlywind(FALSE, 1);
	      break;
	    }
	  if (zotbuf(bp) != TRUE)
	    return FALSE;
	}

	usehost(strcat(strcat(line,">"), filnam), FALSE);

	     /* split the current window to make room for the command output */
	if (splitwind(FALSE, 1) == FALSE)
	  return FALSE;
					    /* and read the stuff in */
        bp = bufflink(filnam, g_clexec);
	if (bp == NULL)
	  return FALSE;
	  
	swbuffer(bp);
		     /* make this window in VIEW mode, update all mode lines */
	curwp->w_bufp->b_flag |= MDVIEW;
	orwindmode(WFMODE, 0);

	unlink(filnam);			/* get rid of the temporary file */
	return TRUE;
}}

unix_rename(old, new)	/* change the name of a file */
	char *new;	/* new file name */
	char *old;	/* original file name */
{ int cc = link(old, new);
  return cc != 0 ? cc : unlink(old);
}

#define FILNAM1 "/tmp/fltinp"
#define FILNAM2 "/tmp/fltout"

			/* filter a buffer through an external program
			 * Bound to ^X #
			 */
filter(f, n)

{ register BUFFER *bp = curbp;
  char line[NLINE];			/* command line send to shell */
  char * tmpnam = bp->b_fname;		/* place to store real file name */

 static char filnam1[] = FILNAM1;
 static char filnam2[] = FILNAM2;

  if (restflag)
    return resterr();

  if (bp->b_flag & MDVIEW)
    return rdonly();
					/* get the filter name and its args */
{ int s = mlreply("#", line, NLINE);
  if (s != TRUE)
    return s;

  bp->b_fname = null;			/* prevent freeing */
					/* write it out, checking for errors */
  s = writeout(filnam1);
  if (s != TRUE)
    s = 1000;
  else
  { usehost(strcat(line," </tmp/fltinp >/tmp/fltout"), FALSE);
					  /* on failure, escape gracefully */
    s = readin(filnam2,FALSE);
    unlink(filnam1);			  /* and get rid of the temporary file */
    unlink(filnam2);
  }
    
  if (bp->b_fname != null)
    free(bp->b_fname);
  bp->b_fname = tmpnam;

  if (s != TRUE)
  { mlwrite(s == 1000 ? TEXT2 : TEXT3);
/*                      "[Cannot write filter file]" */
/*                      "[Execution failed]" */
    sleep(1);
    return s;
  }

  bp->b_flag |= BFCHG;			/* flag it as changed */
  return TRUE;
}}

		   /* return a system dependant string with the current time */
char *Pascal timeset()

{ extern char * ctime();

  char buf[16];

  time((void*)buf);
{ register char *sp = ctime((const time_t *)buf);
  sp[strlen(sp)-1] = 0;
  return sp;
}}

