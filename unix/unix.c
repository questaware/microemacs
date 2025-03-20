/*	UNIX.C:	Operating specific I/O and Spawning functions
		under UNIX V7, BSD4.2/3, System V, SUN OS and SCO XENIX
		for MicroEMACS 3.10
		(C)opyright 1988 by Daniel M. Lawrence
*/

/*#define _POSIX_SOURCE*/
/*#define _ALL_SOURCE*/
/*#define _NO_PROTO*/
	
#include	<stdio.h>
#include	<stdlib.h>
#include  <termio.h>
#include  <time.h>
#include  <unistd.h>
#include  <pthread.h>
#include  <sys/select.h>
#include  <sys/time.h>
#include  <sys/poll.h>
#include	<errno.h>
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

extern int g_stdin_fileno;

#define  STDIPCHAN g_stdin_fileno    /* can have been reopened */

  static int  kbdflgs;			/* saved keyboard fd flags	*/
  static int  kbdqp;			/* how many chars in kbdq	*/
  static char kbdq;			/* char we've already read	*/
  static struct	termio	otermio;	/* original terminal characteristics */
  static struct	termio	ntermio;	/* charactoristics to use inside */

static const char g_emacsdir[] = "mkdir -p ~/.emacs.d/";

extern char * getenv();


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

void sprintf()

{ char ch;
  fputs("printfX", stdout);
  read(STDIPCHAN, &ch, 1);
}


void fprintf()

{ sprintf();
}


void printf()

{ sprintf();
}

#endif


long unsigned int thread_id(void)

{ return pthread_self();
}

				/* must be int for signal() */
void tidyup(int n)

{ if (fexist("/tmp/emacssv"))
    quickexit(0,0);
  else
    puts("\f\n file /tmp/emacssv would have caused save\n\r");
  tcapclose(0);
  exit(1);
}




int Pascal millisleep(unsigned int n)

{ 
#if S_HPUX
    extern nanosleep(long nsecs);
    nanosleep(1000000L * n);
#elif S_LINUX
    usleep(1000 * n);
#else
  struct timeval tvs;
  tvs.tv_sec = 0;
  tvs.tv_usec = n * 1000;
  select(0, null, null, null, &tvs);
#endif
}

#if 0

struct subprocess {
	pid_t pid;
	int stdin;
	int stdout;
	int stderr;
};

static
void mk_pipe(int fds[2]) 
{
  if (pipe(fds) == -1) { perror("Could not create pipe"); exit(1); }
}

static
void mv_fd(int fd1, int fd2) 
{
  if (dup2(fd1,  fd2) == -1) { perror("Could not duplicate pipe end"); exit(1); }
  close(fd1);
}
														// Start program at argv[0] with arguments argv.
														// Set up new stdin, stdout and stderr.
														// Puts references to new process and pipes into `p`.
void subproc_call(int argc, char* argv[], struct subprocess * p) 

{ int ch_in[2], ch_out[2], ch_err[2];
  pipe(ch_in); pipe(ch_out); pipe(ch_err);

  pid_t pid = fork();
  if (pid == 0)									/* am child */
  { // printf("In child for %s\n", argv[1]);
  	close(0); close(1); close(2);                              // close parent pipes
    close(ch_in[1]);    close(ch_out[0]);    close(ch_err[0]); // child pipe ends
    mv_fd(ch_in[0], 0); mv_fd(ch_out[1], 1); mv_fd(ch_err[1], 2);
  { char* envp[] = { NULL };
		argv[argc] = NULL;
    execve(argv[1], argv+1, envp);
    mlwrite("Exec failed %d", errno);
    exit(-1);
  }} 
  else 
	{ close(ch_in[0]); close(ch_out[1]); close(ch_err[1]); // unused child pipe ends
    p->pid = pid;
    p->stdin = ch_in[1];   // parent wants to write to subprocess ch_in
    p->stdout = ch_out[0]; // parent wants to read from subprocess ch_out
    p->stderr = ch_err[0]; // parent wants to read from subprocess ch_err
  }
}

#endif

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
void ttputc(unsigned char c)

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
int ttflush(void)
{
  fflush(stdout);
}

// static int  g_eaten_char = -1;		 /* Re-eaten char */

#if 0

void Pascal reeat(int c)

{
	g_eaten_char = c;			/* save the char for later */
}

#endif
	    	     /* TTGETC:	Read a character from the terminal, performing no
								editing and doing no echo at all.
	      			*/
int ttgetraw()

{	char c;
#if 0
	if (g_eaten_char != -1)
  { int c = g_eaten_char;
    g_eaten_char = -1;
    return c;
  }
#endif
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

void flush_typah()

{ struct pollfd fds;
  fds.fd = 0; /* this is STDIN */
  fds.events = POLLIN;

//g_eaten_char = -1;

  while (TRUE)
	{	int ret = poll(&fds, 1, 0);
  	if (ret != 1)
  		break;
  	
    (void)ttgetraw();
  }
}

#if	TYPEAH
/* typahead:	Check to see if any characters are already in the
		keyboard buffer
*/

int typahead()

{ extern int in_next,in_last;
  if (in_next != in_last)
  	return TRUE;

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

int bktoshell()		/* suspend MicroEMACS and wait to wake up */

{ int pid = getpid();
  tcapclose(1);
  kill(pid,SIGTSTP);
}

int rtfrmshell()

{ ttopen();
  upwind(TRUE);
  return 0;     /* dont know the value */
}
#endif

static void usehost(line, end)
	char *   line;
	char	 end;
{ extern int g_nopipe;
	TTflush();
  tcapclose(1);
  if (g_macargs <= 0)
  { ttputc('\n');                /* Already have '\r'    */

	  if (g_nopipe == 0)
  	  system("reset -Q");
  }
  
  system(line);
  if (end)
  {
    putpad(TEXT6);
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
  upwind(TRUE);
}


int gen_spawn(f, n, prompt, line)
  int      f;
  int      n;
	char *   prompt;
	char *   line;
{ register int  s;

  if (resterr())
    return FALSE;
  
  if (prompt == NULL)
    tcapmove(term.t_nrowm1, 0);             /* Seek to last line.   */
  else
  { s=mlreply(prompt, line, NLINE);
    if (s != TRUE)
      return s;
  }

  usehost(line, prompt != NULL && g_macargs <= 0);

  ttcol = 0;  		   /* otherwise we shall not see the message */
			   /* if we are interactive, pause here */
  return TRUE;
}

/* Create a subjob with a copy of the command intrepreter in it. When the
 * command interpreter exits, mark the screen as garbage so that you do a full
  * repaint. Bound to "^X C". The message at the start in VMS puts out a newline.
   * Under some (unknown) condition, you don't get one free when DCL starts up.
    */
int spawncli(int f, int n)
    
{ register char *cp = getenv("SHELL");
  if (cp == NULL || *cp == 0)
#if S_BSD
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

int spawn(int f, int n)

{ char line[NLINE];

  return gen_spawn(f, n, "!", line);
}



char * mkTempCommName(char suffix, /*out*/char *filename)
{
#define DIRY_CHAR '/'
  char * td = getenv("HOME");
         char c2[2];
  c2[0] = c2[1] = 0;

  if (td[strlen(td)-1] != DIRY_CHAR)
    c2[0] = DIRY_CHAR;

{ int iter;

  for (iter = 25; --iter >= 0; )
	{	char s[2];
		*(short*)&s = suffix;		// little endian

	{ char *ss = concat(filename,td,c2,g_emacsdir+10, "me",int_asc(getpid()),s,NULL);
    if (!fexist(ss))
      break;

    suffix = 'A' + 24 - iter;       // File should not exist anyway
  }}
  return filename;
}}

Cc write_diag(int cc)

{ mlwrite(cc == 1000 ? TEXT2 : TEXT3);
/*                      "[Cannot write filter file]" */
/*                      "[Execution failed]" */
  sleep(1);
  return cc;
}


/* Pipe a one line command into a window
 * Bound to ^X @
 */

int pipefilter(wh)
   char    wh;        /* # ! < @ */
{
 static int bix;
        char   bname [10];
        char   pipeInFile[NFILEN];
        char   pipeOutFile[NFILEN];

        char line[NSTRING+2*NFILEN+100];       /* command line send to shell */

  if (resterr())
    return FALSE;

{ Cc cc;
  char prompt[2];
  int s_discmd = pd_discmd;
  prompt[0] = wh;
  wh -= '<';
  if (wh == '#'-'<' && rdonly())
    return FALSE;

#ifdef USE_SVN
  if (wh == 'e'-'<')
    strpcpy(line, g_ll.lastline[0], sizeof(line)-2*NFILEN);
  else
#endif
	{ extern int g_last_cmd;
    prompt[1] = 0;
  	strcpy(line,"sort");
  	if (wh == ' '-'<')
  		pd_discmd = 0;
  	else 
    {	if (mlreply(prompt, line, NLINE) <= FALSE)
      	return FALSE;

			g_last_cmd = g_ll.ll_ix & MLIX;
		}

    if (line[0] == '%' || line[0] == '\'')
    { char sch;
      int ix;
      for (ix = 0; isalpha((sch = line[++ix])); )
        ;

      line[ix] = 0;

    { const char * val = gtusr(line+1);
      line[ix] = sch;

      if (val != NULL)
        strcpy(line,strcat(strcpy(pipeInFile,val),line+ix));
    }}
  }

{ char * fnam1 = NULL;

  if (wh <= '#'-'<' || wh == '<'-'<')       /* setup the proper file names */
  {
    fnam1 = mkTempCommName('i', pipeInFile);

    if (writeout(fnam1) <= FALSE)   /* write it out, checking for errors */
			return write_diag(1000);

    mlwrite(TEXT159);         /* "\001Wait ..." */
  }

	pd_discmd = s_discmd;
//tcapmove(term.t_nrowm1, 0);

{ char * fnam2 = strcat(mkTempCommName('o', pipeOutFile), int_asc(++bix));

//char rcmd[256];
//system(g_emacsdir);
//strcat(strcat(strcat(strcpy(rcmd,"rm -f ")," "),g_emacsdir+9),fnam2);
//system(rcmd);  
	if (fnam1 != NULL)
	  strcat(strcat(line,"<"), fnam1);

  strcat(line,">");

  usehost(strcat(line,fnam2), FALSE);

  if (wh >= '<'-'<')   /* <  @ */
  { BUFFER * bp = bfind(line /*strcat(strcpy(bname,"_cmd"),int_asc(bix))*/, TRUE);
    if (bp == NULL)
      return FALSE;
/*
    if (splitwind(FALSE, 1) == FALSE)
      return FALSE;
*/
    swbuffer(bp);
  /*linstr(tmpnam); */
                /* make this window in VIEW mode, update all mode lines */
    curwp->w_bufp->b_flag |= MDVIEW;
    upmode();
                                  /* and get rid of the temporary file */
  }
{	Cc rc = FALSE;
  if (wh == '!'-'<')
  { FILE * ip = fopen(fnam2, "rb");
    if (ip != NULL)
    { char * ln;
      while ((ln = fgets(&line[0], NSTRING+NFILEN*2-1, ip))!=NULL)
        fputs(ln, stdout);

      fclose(ip);
      puts(TEXT6);
      ttgetc();
    /*homes();*/
      rc = TRUE;
    }
  }
  else                          /* on failure, escape gracefully */
  { BUFFER * bp = curbp;
    char * sfn = bp->b_fname;
    bp->b_fname = null;                 /* otherwise it will be freed */
    rc = readin(fnam2, FILE_NMSG);
    bp->b_fname = sfn;                  /* restore name */
    bp->b_flag |= BFCHG;                /* flag it as changed */
    if (wh == '#'-'<')
      flush_typah();
  }
                                  /* get rid of the temporary files */
  if (fnam1 != NULL)
    unlink(fnam1);
  unlink(fnam2);
//if (wh == 'E' - '<')
//  unlink(fnam3);
  return rc;
}}}}}


  /* Pipe a one line command into a window
   * Bound to ^X @
   */
int Pascal pipecmd(int f, int n)

{ return pipefilter(n >= 0 ? '@' : '<');
}

  /*
   * filter a buffer through an external DOS program
   * Bound to ^X #
   */
int Pascal filter(int f, int n)

{ return pipefilter(f < 0 ? ' ' : '#');
}



char * searchfile(char * result, Fdcr fdcr)

{ FILE * ip = (FILE*)fdcr->ip;
	if (ip == NULL)
	{ char buf[NFILEN+20];
//	char * basename = result+strlen(result)+1;	
		char * cmd = concat(buf, "ffg -/ ", /* basename, " ", */ result, NULL);
		fdcr->ip = popen(cmd, "r");
		if (fdcr->ip == NULL)
			return NULL;
	}

{ char * fname = fgets(result, NFILEN, ip);
	if (fname == NULL)
		pclose(ip);
	else
	{ int sl = strlen(fname)-1;
		if (sl <= 0)
			fname = NULL;
		else														// strip off CR,LF
			fname[sl] = 0;
	}

	return fname;
}}


		   /* return a system dependant string with the current time */
char *Pascal timeset()

{ extern char * ctime();

  char buf[16];

  time((void*)buf);
{ char *sp = ctime((const time_t *)buf);
  sp[strlen(sp)-1] = 0;
  return sp;
}}

