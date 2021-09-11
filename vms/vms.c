/*	VMS.C	Operating system specific I/O and spawning functions
		For VAX/VMS operating system
		for MicroEMACS 4.00
		Copyright 1995 by Jeffrey A. Lomicka and Daniel M. Lawrence

	All-new code replaces the previous VMS/SMG implementation which
	prevented using non-SMG terminal drivers (ansi, termcap).  New
	approach to terminal I/O, new (and more) subprocess control
	functions, Kept emacs support, mail/notes interface.

	Some of the above may still be wishlist.

	12-Dec-89	Kevin A. Mitchell
			Start work on RMSIO code.
*/
#include    <stdio.h>
#include    <unistd.h>
#include    <stdlib.h>
#include    <time.h>
#include    "estruct.h"

#if	__VMS
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"logmsg.h"

#define __USE_STD_IOSTREAM

#include ssdef
#include descrip
#include jpidef
#include iodef
#include ttdef
#include tt2def
#include msgdef
#include rms
#include starlet
#include libdef
#include lib$routines
#if 0
#include ctype
#endif
/*
	These are the LIB$SPAWN mode flags.  There's no .h for
	them in VAX C V2.4.
*/
#define CLI$M_NOCONTROL 32
#define CLI$M_NOCLISYM 2
#define CLI$M_NOLOGNAM 4
#define CLI$M_NOKEYPAD 8
#define CLI$M_NOTIFY 16
#define CLI$M_NOWAIT 1
/*
	test macro is used to signal errors from system services
*/
#define test( s) {int st; st = (s); if ((st&1)==0) LIB$SIGNAL( st);}

/*
	This routine returns a pointer to a descriptor of the supplied
	string. The descriptors are static allocated, and up to
	"NUM_DESCRIPTORS" may be used at once.  After that, the old ones
	are re-used. Be careful!

	The primary use of this routine is to allow passing of C strings into
	VMS system facilities and RTL functions.

	There are three forms:

		descrp( s, l)	String descriptor for buffer s, length l
		descptr( s)	String descriptor for asciz buffer s
		DESCPTR( s)	String descriptor for buffer s, using sizeof()
*/
#define NUM_DESCRIPTORS 10
struct  dsc$descriptor_s *descrp(char *s, int l)
{
    static int next_d = 0;
    static struct dsc$descriptor_s dsclist[ NUM_DESCRIPTORS];

    if (next_d >= NUM_DESCRIPTORS) next_d = 0;
    dsclist[ next_d].dsc$w_length = l;
    dsclist[ next_d].dsc$b_dtype =  DSC$K_DTYPE_T;
    dsclist[ next_d].dsc$b_class =  DSC$K_CLASS_S;
    dsclist[ next_d].dsc$a_pointer = s;
    return( &dsclist[ next_d++]);
}

/*
 * Make pointer to descriptor from Asciz string.
 */
struct dsc$descriptor_s *descptr(char *s)
{
    return( descrp( s, strlen( s)));
}

#define DESCPTR(s)	descrp( s, sizeof(s)-1)

/*
	These two structures, along with ttdef.h, are good for manipulating
	terminal characteristics.
*/
typedef struct
{	/* Terminal characteristics buffer */
    unsigned char classs, type;
    unsigned short width;
    unsigned tt1 : 24;
    unsigned char page;
    unsigned long tt2;
} TTCHAR;

typedef struct
{	/* More terminal characteristics (hidden in the status block) */
    short status;
    char txspeed;
    char rxspeed;
    long trash;
} TTCHARIOSB;

typedef struct
{	/* Status block for ordinary terminal reads */
    unsigned short status, len, term, tlen;
} TTIOSB;

typedef struct
{	/* Status block for mailbox reads */
    unsigned short status, len; long sender_pid;
} MBIOSB;

typedef struct
{	/* Messages from the terminal or TW driver */
    short msgtype;	/* Expecting MSG$_TRMHANGUP */
    short unit;		/* Controller unit number */
    char ctrl_len;	/* Length of controller name (should be 3) */
    char ctrl[15];	/* Controller name (should be TWA) */
    short brdcnt;	/* Broadcast message byte count, if MSG$TRMBRDCST */
    char message[514];	/* First two bytes of broadcast message */
} TTMESSAGE;

static const int noterm[] = {0,0};	/* Terminator list of NONE */
static int newbrdcst = FALSE;	/* Flag - is message in Emacs buffer yet.*/

#define	MINREAD	128  /* Smallest read to queue */
#define TYPSIZE 1024 /* Typeahead buffer size, must be several times MINREAD */

static unsigned char tybuf[ TYPSIZE];	/* Typeahead buffer */
static unsigned tyin, tyout, tylen, tymax;/* Inptr, outptr, and length */

static TTIOSB ttiosb;		/* Terminal I/O status block */
static MBIOSB mbiosb;		/* Associated mailbox status block */
static TTMESSAGE mbmsg;		/* Associated mailbox message */
unsigned short vms_iochan;      /* VMS I/O channel open on terminal */
static short mbchan;		/* VMS I/O channel open on associated mbx */
static short waiting;		/* Flag FALSE if read already pending */
static short stalled;		/* Flag TRUE if I/O stalled by full buffer */


/*
	If we come from ME$EDIT, the "suspend-emacs" is not allowed, since
	it will tend to wake itself up and re-hiberneate itself, which is
	a problem.
*/
static short called = 0;		/* TRUE if called from ME$EDIT */
/*
	short_time[ 0] is the negative number of 100ns units of time to
	wait.  -10000 is 1ms, therefore 200ms (2 tenths of a second) is
	-2,000,000.  Hopefully this is long enough for the network delay
	that might be involved between seeing the ESC and seeing the
	characters that follow it.

	This will be initialized from the environment variable
	MICROEMACS$SHORTWAIT.
*/
static long short_time[2] = {-4000000, -1};

static unsigned char tobuf[ TYPSIZE];	/* Output buffer */
static unsigned tolen;			/* Amount used */
NOSHARE TTCHAR orgchar;			/* Original characteristics */
static TTCHARIOSB orgttiosb;		/* Original IOSB characteristics */

static int next_read(int flag);

static void readast()
{									/* Data arrived from the terminal */
	waiting = 1;
	if ((ttiosb.status == SS$_TIMEOUT) || (ttiosb.status & 1))
	{								/* Read completed okay, process the data */
		if (ttiosb.len)
		{							/* Got some data, adjust input queue parameters */
			tylen += ttiosb.len;
			tyin += ttiosb.len;
		 	test( SYS$WAKE( 0, 0));
			next_read( 1);
		}
		else
		{	/* The user seems to have stopped typing, issue a read
				 that will wake us up when the next character is typed */
	    if (!mbchan) next_read( 0);
		}
  }
	else if (ttiosb.status != SS$_ABORT) 
		LIB$SIGNAL( ttiosb.status);
}

/*
 * flag = TRUE to use timeout of 0.
 */
static int next_read(int flag)
{
	/* No current read outstanding, submit one */
	if ( waiting || stalled)
	{
		unsigned size;
		/*
		 * Wrap the input pointer if out of room.
		 */
		waiting = 0;
		if (sizeof( tybuf) - tyin < MINREAD)
		{
			tymax = tyin;
			tyin = 0;
		}

		size = tymax - tylen;

		if (tyin + size > sizeof( tybuf)) size = sizeof( tybuf) - tyin;

		if (size >= MINREAD)
		{								/* Only read if there is enough room */
			test( SYS$QIO(
										0, vms_iochan,
										flag ?
											IO$_READVBLK | IO$M_NOECHO | IO$M_TRMNOECHO |
											IO$M_NOFILTR | IO$M_TIMED
										:
											IO$_READVBLK | IO$M_NOECHO | IO$M_TRMNOECHO |
											IO$M_NOFILTR,
										&ttiosb, readast, 0, &tybuf[ tyin], flag ? size : 1,
										0, noterm, 0, 0
										));
			stalled = 0;
		}
		else stalled = 1;
	}
	return 0;
}

#if 0
X/***********************************************************
X* FUNCTION - RemoveEscapes - remove ANSI escapes from string
X* (for broadcast messages that contain 'formatting')
X***********************************************************/
Xstatic void RemoveEscapes(char *str)
X{
X	char *in=str, *out=str;
X
X	while (*in)
X	{
X		switch (*in)
X		{
X			case 0x1b:	/* skip escape */
X				in++;
X				if (*in != '[') /* not a CSI */
X				{
X					switch (*in)
X					{
X						/* skip special characters */
X						case ';':
X						case '?':
X						case '0':
X							in++;
X					}
X
X					/* skip any intermediate characters 0x20 to 0x2f */
X					while (*in >= 0x20 && *in <= 0x2f) in++;
X
X					/* skip any final characters 0x30 to 0x7e */
X					if (*in >= 0x30 && *in <= 0x7e) in++;
X					break;
X				}
X				/* fall through to CSI */
X			case 0x9b:	/* skip CSI */
X				in++;
X
X				/* skip any parameters 0x30 to 0x3f */
X				while (*in >= 0x30 && *in <= 0x3f) in++;
X
X				/* skip any intermediates 0x20 to 0x2f */
X				while (*in >= 0x20 && *in <= 0x2f) in++;
X
X				/* skip one final character 0x40 to 0x7e */
X				if (*in >= 0x40 && *in <= 0x7e) in++;
X				break;
X			default:
X				*out++ = *in++;
X		}
X	}
X	*out = 0;
X}
#endif


static void mbreadast()
{
	if (mbiosb.status & 1)
	{								/* Read completed okay, check for hangup message */
		if (mbmsg.msgtype == MSG$_TRMHANGUP)
		{
				/* Got a termination message, process it */
		}
		else if (mbmsg.msgtype == MSG$_TRMUNSOLIC)
		{		/* Got unsolicited input, get it */
			next_read(1);
		}
#if 0
X		else if (mbmsg.msgtype == MSG$_TRMBRDCST)
X		{		/* Got broadcast, get it */
X				/* Hard-coding the mbmsg.brdcnt to 511 is a temp solution.*/
X			mbmsg.brdcnt = 511;
X			memcpy(brdcstbuf, mbmsg.message, 511);
X			brdcstbuf[511] = 0;
X
X			RemoveEscapes(brdcstbuf);
X			pending_msg = newbrdcst = TRUE;
X		}
#endif
		else
		{
		}
		test( SYS$QIO(		/* Post a new read to the associated mailbox */
									0, mbchan, IO$_READVBLK, &mbiosb,
									mbreadast, 0, &mbmsg, sizeof( mbmsg),
									0, 0, 0, 0
									));
	}
	else if (mbiosb.status != SS$_ABORT)
		LIB$SIGNAL( mbiosb.status);
}

int Pascal serialopen()
{
    TTCHAR newchar;		/* Adjusted characteristics */
    int status;
    char *waitstr;
    int szmbmsg = sizeof(mbmsg);

/*  strcpy(os, "VMS"); */
	tyin = 0;
	tyout = 0;
	tylen = 0;
	tymax = sizeof( tybuf);
	status = LIB$ASN_WTH_MBX( /* Create a new PY/TW pair */
	descptr( "SYS$OUTPUT:"),
						&szmbmsg,
						&szmbmsg,
						&vms_iochan,
						&mbchan);
	if ((status & 1) == 0)
	{ 				/* The assign channel failed, was it because of the mailbox? */
		if (status == SS$_DEVACTIVE)
		{ 			/* We've been called from NOTES, so we can't use the mailbox */
			test( SYS$ASSIGN( descptr( "SYS$OUTPUT:"), &vms_iochan, 0, 0));
			mbchan = 0;
		}
		else 
			LIB$SIGNAL( status);
	}
	waiting = 0;		/* Block unsolicited input from issuing read */
	stalled = 0;		/* Don't start stalled */
	if (mbchan) test( SYS$QIO(		/* Post a read to the associated mailbox */
														0, mbchan, IO$_READVBLK, &mbiosb,
														mbreadast, 0, &mbmsg, sizeof( mbmsg),
														0, 0, 0, 0
													 ));
/*
	Fetch the characteristics and adjust ourself for proper operation.
*/
	test( SYS$QIOW(
								0, vms_iochan, IO$_SENSEMODE, &orgttiosb,
								0, 0, &orgchar, sizeof( orgchar), 0, 0, 0, 0));
	newchar = orgchar;
	newchar.tt2 |= TT2$M_PASTHRU; /* Gives us back ^U, ^X, ^C, and ^Y. */
	newchar.tt2 |= TT2$M_BRDCSTMBX; /* Get broadcast messages */
	newchar.tt1 &= ~TT$M_MBXDSABL;	/* Make sure mailbox is on */
	newchar.tt1 |= TT$M_NOBRDCST; /* Don't trash the screen with these */

/*
	Hostsync allows super-fast typing (workstation paste, PC
	send-file) without loss of data, as long as terminal supports
	XON/XOFF.  VWS and DECWindows terminal emulators require HOSTSYNC
	for PASTE operations to work, even though there is no wire involved.
*/
	newchar.tt1 |= TT$M_HOSTSYNC;
/*
	If you MUST, and if you know you don't need XON/XOFF
	synchronization, you can get ^S and ^Q back as data by defining
	XONDATA in ESTRUCT.H.  This is guarnteed to fail on VT125, VT100's
	over 3600 baud, any serial line terminal with smooth scroll
	enabled, VT200's over 4800 baud.	This is guarnteed to WORK if you
	are using a VT330/340 with SSU enabled, a VWS or DECWindows
	terminal emulator.	Note that if XONDATA is not set, I trust the
	settings the user has, so you just $ SET TERM /[NO]TTSYNC as you wish.
*/
#if XONDATA
	newchar.tt1 &= ~TT$M_TTSYNC;
#endif
/*
	I checked in DISPLAY.C, and verified that the mrow and mcol
	numbers aren't used until after ttopen() is called.  I override
	the terminal-supplied numbers with large numbers of my own, so
	that workstation terminal resizes will work to reasonable limits.

	I don't just use the current sizes as the maximum, becuase it's
	possible to resize the terminal emulator after Emacs is started,
	or even to disconnect and reconnect with a new terminal size, so
	the maximums must not change over multiple calls to ttopen().

	Also note that I do the changes to newchar, so that the actual
	terminal window will be reduced to the maximum values Microemacs
	will support.
*/
#if 0
X 	term.t_mrow = 72; 	/* 72 is European full page */
X 	term.t_mcol = 256;		/* 256 is Wider than any termnal I've tried */
X 	if (newchar.page > term.t_mrow) newchar.page = term.t_mrow;
X 	term.t_nrow = newchar.page-1;
X 	if (newchar.width > term.t_mcol) newchar.width = term.t_mcol;
X 	term.t_ncol = newchar.width;
#endif
/*
	Set these new characteristics
*/
	test( SYS$QIOW(
								0, vms_iochan, IO$_SETMODE, 0,
								0, 0, &newchar, sizeof( newchar), 0, 0, 0, 0));
/*
	For some unknown reason, if I don't post this read (which will
	likely return right away) then I don't get started properly.
	It has something to do with priming the unsolicited input system.
*/
	test( SYS$QIO(
								0, vms_iochan,
								IO$_READVBLK | IO$M_NOECHO | IO$M_TRMNOECHO |
															 IO$M_NOFILTR | IO$M_TIMED,
								&ttiosb, readast, 0, tybuf, sizeof( tybuf),
								0, noterm, 0, 0
							 ));
/*	Initialize the short_time value for esc-reads.	Larger values may
	be needed on network links.  I'm still experimeinting to get the
	best numbers.
*/
	waitstr = getenv( "MICROEMACS$SHORTWAIT");
	if (waitstr) short_time[ 0] = -atoi( waitstr);
	loginfo1("vms_iochan open %d", vms_iochan);
	return 0;
}

int Pascal serialclose()
{
	loginfo1("vms_iochan close %d", vms_iochan);
	if (vms_iochan == 0)
		return 0;

	ttflush();
#if 0
X if (tolen > 0)
X { /* Buffer not empty, flush out last stuff */
X 	test( SYS$QIOW( 0, vms_iochan, IO$_WRITEVBLK	| IO$M_NOFORMAT,
X 									0, 0, 0, tobuf, tolen, 0, 0, 0, 0));
X 	tolen = 0;
X }
#endif
	test( SYS$CANCEL( vms_iochan)); /* Cancel any pending read */
	test( SYS$QIOW(
								0, vms_iochan,IO$_SETMODE, 0,
								0, 0, &orgchar, sizeof( orgchar), 0, 0, 0, 0));
	if (mbchan) test( SYS$DASSGN( mbchan));
	test( SYS$DASSGN( vms_iochan));
	vms_iochan = 0;
	return 0;
}

int Pascal ttputc(char c)
{
#if 0
X static FILE * ppop;
X
X if (ppop == 0)
X 	ppop = fopen("vpop", "w");
X if (ppop != 0)
X 	putc(c, ppop);
X fflush(ppop);
#endif
	tobuf[ tolen++] = c;
	if (tolen >= sizeof( tobuf))
	{ /* Buffer is full, send it out */
		test( SYS$QIOW( 0, vms_iochan, IO$_WRITEVBLK | IO$M_NOFORMAT,
										0, 0, 0, tobuf, tolen, 0, 0, 0, 0));
		tolen = 0;
	}
	return 0;
}

int Pascal ttflush()
{
/*
	I choose to ignore any flush requests if there is typeahead
	pending.	Speeds DECNet network operation by leaps and bounds
	(literally).
*/
	if (tylen == 0 && tolen != 0 && vms_iochan != 0)
	{ /* No typeahead, send it out */
		test( SYS$QIOW( 0, vms_iochan, IO$_WRITEVBLK	| IO$M_NOFORMAT,
										0, 0, 0, tobuf, tolen, 0, 0, 0, 0));
		tolen = 0;
	}
	return 0;
}
/*
	grabnowait is a routine that tries to read another
	character, and if one doesn't come in as fast as we expect
	function keys sequences to arrive, we return -1.	This is called
	after receving ESC to check for next character.  It's okay to wait
	too long, but the length of the delay controls how log the user
	waits after hitting ESC before results are seen.

	Note that we also wake from hibernation if a character arrives, so
	this never causes an undue delay if the user it actually typing.
*/
int Pascal grabnowait()
{
	if (tylen == 0)
	{ /* Nothing immediately available, hibernate for a short time */
		test( SYS$SCHDWK( 0, 0, short_time, 0));
		test( SYS$HIBER());
	}
	return (tylen == 0)? -1: ttgetc();
}

int Pascal grabwait()
{
	return ttgetc();
}


static int	eaten_char = -1;		 /* Re-eaten char */


void Pascal reeat(int c)

{
	eaten_char = c; 		/* save the char for later */
}


int Pascal ttgetraw() /* was ttgetc */
{
	extern short alarm_went_off;
				register unsigned ret;

	if (eaten_char != -1)
	{ int c = eaten_char;
		eaten_char = -1;
		return c;
	}

	loginfo1("setast %d", alarm_went_off);
	if (alarm_went_off)
		return 0;

	while (tylen == 0)
	{ /* Nothing to send, wait for something interesting */
		ttflush();
		test(SYS$HIBER());
	}
		/*
		 * Got something, return it.
		 */
	SYS$SETAST( 0);
	ret = tybuf[ tyout++];

	if (tyout >= tymax)
	{	tyout = 0;
		tymax = sizeof( tybuf);
	}

	tylen--;		/* Should be ADD_INTERLOCKED */

	if (stalled && (tylen < 2 * MINREAD)) 
	{
		test( SYS$DCLAST( next_read, 1, 0));
	}

	SYS$SETAST( 1);
	loginfo2("ttgetraw %d %c", ret, ret);
	return ret;
}

/*
 * Typahead - any characters pending?
 */
int Pascal typahead()
{
		return (tylen != 0);
}

/*
 * Shell out to DCL.
 */
int Pascal spawncli(int f, int n)
{
	register char *cp;

		/*
		 * Don't allow this command if restricted
		 */
	if (resterr()) 
		return FALSE;

	tcapmove(term.t_nrowm1, 0);
	/*movecursor(term.t_nrow, 0); 			** Seek to last line. 	*/
	TTclose(0); 					/* stty to old settings */
	test( LIB$SPAWN( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
	TTopen(0);
	pd_sgarbf = TRUE;
	return TRUE;
}


static void usehost(char * line, char end)

{ TTflush();
	tcapclose(0);
	if (g_clexec <= 0)
		ttputc('\n'); 							 /* Already have '\r' 	 */

	system(line);
	if (end)
	{	putpad(/*TEXT188*/"[End1]");
			 /* 		"[End]" */
		TTflush();
	}

	ttopen(0);
	if (end)
#if 1
		tgetc();
#else
	{ char s;
		while ((s = tgetc()) != '\r' && s != ' ')
		 ;
	}
#endif
	pd_sgarbf = TRUE;
}

/*
 * Spawn a command.
 */
int Pascal spawn(int f, int n)
{
	register int		s;
	char			line[NLINE];
		/*
		 * Don't allow this command if restricted.
		 */
	if (resterr())
		return FALSE;

	if ((s=mlreply("!", line, NLINE)) != TRUE)
		return s;

	TTputc('\n'); 	/* Already have '\r' */
	TTflush();
	TTclose(0); 		/* stty to old modes */
	system(line);
	TTopen(0);
	TTflush();

		/* if we are interactive, pause here */
	if (g_clexec <= 0) 
	{
#if S_VMS
		write(1, "\r\n\n[End2]", 9);
#else
		mlputs(/*TEXT6*/"\r\n\n[End2]");
#endif
/*					 "\r\n\n[End]" */
		tgetc();
#if S_VMS
		tcapepage();
#endif
	}
	pd_sgarbf = TRUE;
	return TRUE;
}


/*
 * Run an external program with arguments. When it returns, wait for a single
 * character to be typed, then mark the screen as garbage so a full repaint is
 * done. Bound to "C-X $".
 */
int Pascal execprg(int f, int n)
{
	register int	s;
	char					line[NLINE];

	/* Don't allow this command if restricted. */
	if (resterr())
		return FALSE;

	if ((s=mlreply("!", line, NLINE)) != TRUE)
		return s;

	TTputc('\n'); 		/* Already have '\r' */
	TTflush();
	TTclose(0); 		/* stty to old modes */
	system(line);
	TTopen(0);
	mlputs(/*TEXT188*/"[End3]");		/* Pause. */
/*						 "[End]" */
	TTflush();
	while ((s = tgetc()) != '\r' && s != ' ')
		;
	pd_sgarbf = TRUE;
	return TRUE;
}

#if 0
X
Xint Pascal pipecmd(int f, int n)
X{
X register int 	 s; 		/* return status from CLI */
X register WINDOW *wp; 	 /* pointer to new window */
X register BUFFER *bp; 	 /* pointer to buffer to zot */
X char 	 line[NLINE]; 	 /* command line send to shell */
X static char bname[] = "command.log";
X
X static char filnam[NFILEN] = "command.log";
X
X 			/* don't allow this command if restricted */
X if (resterr()) return FALSE;
X
X 	 /* get the command to pipe in */
X if ((s=mlreply("@", line, NLINE)) != TRUE) return(s);
X
X 	 /* get rid of the command output buffer if it exists */
X if ((bp=bfind(bname, FALSE, 0)) != FALSE) 
X	{						/* try to make sure we are off screen */
X 	wp = wheadp;
X		while (wp != NULL) {
X 		if (wp->w_bufp == bp) {
X 			onlywind(FALSE, 1);
X 			break;
X 		}
X 		wp = wp->w_next;
X 	}
X 	if (zotbuf(bp) != TRUE)
X		 	return FALSE;
X }
X
X TTputc('\n');		/* Already have '\r'	 */
X TTflush();
X TTclose(0);		/* stty to old modes		*/
X
X test( LIB$SPAWN( descptr( line), DESCPTR( "NL:"), descptr( filnam),
X 								0, 0, 0, 0, 0, 0, 0, 0));
X TTopen(0);
X TTflush();
X pd_sgarbf = TRUE;
X s = TRUE;
X
X if (s != TRUE)
X 	return s;
X
X 	 /* split the current window to make room for the command output */
X if (splitwind(FALSE, 1) == FALSE)
X 	return FALSE;
X
X 	 /* and read the stuff in */
X if (getfile(filnam, FALSE) == FALSE)
X 	return FALSE;
X
X 	 /* make this window in VIEW mode, update all mode lines */
X curwp->w_bufp->b_mode |= MDVIEW;
X wp = wheadp;
X while (wp != NULL) 
X {
X 	wp->w_flag |= WFMODE;
X 	wp = wp->w_next;
X }
XX 	 /* and get rid of the temporary file */
X	delete(filnam);
X return TRUE;
X}
X
Xint Pascal filter(int f, int n)
X{
X 			 register int 	 s; /* return status from CLI */
X register BUFFER *bp;	/* pointer to buffer to zot */
X 			 char line[NLINE];	/* command line send to shell */
X char tmpnam[NFILEN];	/* place to store real file name */
X static char bname1[] = "fltinp.com";
X
X static char filnam1[] = "fltinp.com";
X static char filnam2[] = "fltout.log";
X
X /* don't allow this command if restricted */
X if (resterr())
X 	return FALSE;
X
X if (rdonly())
X 	return FALSE;
X
X /* get the filter name and its args */
X 			 if ((s=mlreply("#", line, NLINE)) != TRUE)
X 							 return s;
X
X /* setup the proper file names */
X bp = curbp;
X strcpy(tmpnam, bp->b_fname);	/* save the original name */
X strcpy(bp->b_fname, bname1);	/* set it to our new one */
X
X /* write it out, checking for errors */
X if (writeout(filnam1, "w") != TRUE)
X {	mlwrite(TEXT2);
X/* 										 "[Cannot write filter file]" */
X 	strcpy(bp->b_fname, tmpnam);
X 	return FALSE;
X }
X
X TTputc('\n');			/* Already have '\r'		*/
X TTflush();
X TTclose(0);			/* stty to old modes		*/
X s = 1;
X
X test( LIB$SPAWN( descptr( line), descptr( filnam1), descptr( filnam2),
X 								0, 0, 0, &s, 0, 0, 0, 0));
X TTopen(0);
X TTflush();
X pd_sgarbf = TRUE;
X s &= 1;
X
X /* on failure, escape gracefully */
X if (!s || (readin(filnam2,0) == FALSE)) 
X {
X 	mlwrite(TEXT3);
X/* 										 "[Execution failed]" */
X 	strcpy(bp->b_fname, tmpnam);
X 	delete(filnam1);
X 	delete(filnam2);
X 	return s;
X }
X
X /* reset file name */
X strcpy(bp->b_fname, tmpnam);	/* restore name */
X bp->b_flag |= BFCHG;		/* flag it as changed */
X
X /* and get rid of the temporary file */
X delete(filnam1);
X delete(filnam2);
X return TRUE;
X}
X
#else

/* Pipe a one line command into a window
 * Bound to ^X @
 */
int pipecmd(int f, int n)

{ register WINDOW *wp;	/* pointer to new window */
	register BUFFER *bp;	/* pointer to buffer to zot */
				char	line[NLINE*2];	/* command line send to shell */

 static char bname[] = "command";
 static char filnam[] = "command";

	strcpy(line, "pipe ");

	if (resterr())
		return FALSE;

{ int s = mlreply("@", &line[5], NLINE);		 /* get the command to pipe in */
	if (s != TRUE)
		return s;
			/* get rid of the command output buffer if it exists */
	bp = bfind(bname, FALSE, 0);
	if (bp != FALSE) 
	{ 	/* try to make sure we are off screen */
		for (wp = wheadp; wp != NULL; wp = wp->w_next)
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
	bp = bufflink(filnam, (g_clexec > 0) + 64);
	if (bp == NULL)
		return FALSE;
		
				 /* make this window in VIEW mode, update all mode lines */
	curwp->w_bufp->b_flag |= MDVIEW;
	orwindmode(WFMODE);

	unlink(filnam); 		/* get rid of the temporary file */
	return TRUE;
}}

#if 0
X
Xunix_rename(char * old, char * new)	/* change the name of a file */
X
X{ int cc = link(old, new);
X  return cc != 0 ? cc : unlink(old);
X}
X
#endif

#define FILNAM1 "SYS$LOGIN:fltinp"
#define FILNAM2 "SYS$LOGIN:fltout"

			/* filter a buffer through an external program
			 * Bound to ^X #
			 */
int filter(int f, int n)

{ register BUFFER *bp = curbp;
	char line[NLINE*2]; 		/* command line send to shell */
	char * tmpnam = bp->b_fname;		/* place to store real file name */

 static char filnam1[] = FILNAM1;
 static char filnam2[] = FILNAM2;
 
	strcpy(line, "pipe ");

	if (resterr())
		return FALSE;

	if (rdonly())
		return FALSE;

					/* get the filter name and its args */
{ int s = mlreply("#", &line[5], NLINE);
	if (s != TRUE)
		return s;

	bp->b_fname = null; 		/* prevent freeing */
					/* write it out, checking for errors */
	s = writeout(filnam1);
	if (s != TRUE)
		s = 1000;
	else
	{
		usehost(strcat(&line[0]," <"FILNAM1" >"FILNAM2), FALSE);
						/* on failure, escape gracefully */
		s = readin(filnam2,0);
		unlink(filnam1);				/* and get rid of the temporary file */
		unlink(filnam2);
	}
		
	if (bp->b_fname != null)
		free(bp->b_fname);
	bp->b_fname = tmpnam;

	if (s != TRUE)
	{ mlwrite(s == 1000 ? TEXT2 : TEXT3);
/*											"[Cannot write filter file]" */
/*											"[Execution failed]" */
		sleep(1);
		return s;
	}

	bp->b_flag |= BFCHG;			/* flag it as changed */
	return TRUE;
}}

#endif

/*
	The rename() function is built into the VMS C RTL, and need not be
	duplicated here.
*/

char *Pascal timeset()
{
		register char *sp;		/* temp string pointer */
		unsigned long buf[16];		/* time data buffer */

		time(buf);
		sp = ctime(buf);
		sp[strlen(sp)-1] = 0;
		return(sp);
}


#if 0
X
X/* FILE Directory routines 	*/
X
Xstatic char fname[NFILEN]; 	/* path of file to find */
Xstatic char path[NFILEN];		/* path of file to find */
Xstatic char rbuf[NFILEN];		/* return file buffer */
Xstatic char *ctxtp = NULL; 	/* context pointer */
Xstatic struct dsc$descriptor pat_desc; /* descriptor for pattern */
Xstatic struct dsc$descriptor rbuf_desc;	/* descriptor for returned file name */
X
X/*
X * Do a wild card directory search (for file name completion)
X * fspec is the pattern to match.
X */
Xchar *Pascal getffile(char *fspec)
X
X{
X register int index; 	/* index into various strings */
X register int point; 	/* index into other strings */
X register int extflag; 	/* does the file have an extention? */
X register int verflag; 	/* does the file have a version? */
X register char *cp, c;
X
X /* first parse the file path off the file spec */
X strcpy(path, fspec);
X index = strlen(path) - 1;
X while (index >= 0 && (path[index] != ']' && path[index] != ':'))
X 	--index;
X
X path[index+1] = 0;
X
X /* check for a version number */
X point = strlen(fspec) - 1;
X verflag = FALSE;
X while (point >= 0) {
X 	if ((c=fspec[point]) == ';') {
X 		verflag = TRUE;
X 		break;
X 	} else if (c == '.' || c == ']' || c == ':')
X 		break;
X 	point--;
X }
X
X /* check for an extension */
X point = strlen(fspec) - 1;
X extflag = FALSE;
X while (point >= 0) {
X 	if ((c=fspec[point]) == '.') {
X 		extflag = TRUE;
X 		break;
X 	} else if (c == ']' || c == ':')
X 		break;
X 	point--;
X }
X
X /* construct the composite wild card spec */
X strcpy(fname, path);
X strcat(fname, &fspec[index+1]);
X strcat(fname, "*");
X if (!extflag)
X 	strcat(fname, ".*");
X if (!verflag)
X 	strcat(fname, ";*");
X
X pat_desc.dsc$a_pointer = fname;
X pat_desc.dsc$w_length = strlen(fname);
X pat_desc.dsc$b_dtype = DSC$K_DTYPE_T;
X pat_desc.dsc$b_class = DSC$K_CLASS_S;
X
X for (cp=rbuf; cp!=rbuf+NFILEN; *cp++=' ') ;
X rbuf_desc.dsc$a_pointer = rbuf;
X rbuf_desc.dsc$w_length = NFILEN;
X rbuf_desc.dsc$b_dtype = DSC$K_DTYPE_T;
X rbuf_desc.dsc$b_class = DSC$K_CLASS_S;
X
X LIB$FIND_FILE_END(&ctxtp);
X ctxtp = NULL;
X if (LIB$FIND_FILE(&pat_desc, &rbuf_desc, &ctxtp) != RMS$_SUC)
X 	return(NULL);
X
X /* return the first file name!
X  * isolate the filename and extension
X  * and append that on to the original path
X  */
X for (cp=rbuf; *cp!=' ' && cp!=rbuf+NFILEN-1; cp++) ;
X *cp = 0;
X for (cp--; *cp!=';' && cp!=rbuf-1; cp--) ;
X *cp = 0;
X for (cp--; *cp!=']' && cp!=rbuf-1; cp--) ;
X strcat(path,++cp);
X mklower(path);
X return(path);
X}
X
Xchar *Pascal getnfile()
X{
X register int index; 	/* index into various strings */
X register int point; 	/* index into other strings */
X register int extflag; 	/* does the file have an extention? */
X char fname[NFILEN]; 	/* file/path for DOS call */
X register char *cp;
X
X /* and call for the next file */
X for (cp=rbuf; cp!=rbuf+NFILEN; *cp++=' ') ;
X if (LIB$FIND_FILE(&pat_desc, &rbuf_desc, &ctxtp) != RMS$_SUC)
X 	return(NULL);
X
X /* return the next file name!
X  * isolate the original path,
X  * isolate the filename and extension,
X  * and append filename/extension on to the original path
X  */
X for (cp=path+strlen(path)-1; *cp!=']' && cp!=path-1; cp--)
X 	;
X
X *++cp = 0;
X for (cp=rbuf; *cp!=' ' && cp!=rbuf+NFILEN-1; cp++)
X 	;
X *cp = 0;
X for (cp--; *cp!=';' && cp!=rbuf-1; cp--)
X 	;
X *cp = 0;
X for (cp--; *cp!=']' && cp!=rbuf-1; cp--)
X 	;
X strcat(path,++cp);
X mklower(path);
X return(path);
X}
X
X/*
X The following ME$EDIT entry point is used when MicroEmacs is
X called up from MAIL or NOTES.  Note that it may be called more than
X once, and that "main()" is never called.
X
X Mail/Notes entry point.  Should be declared UNIVERSAL in ME.OPT.
X*/
XME$EDIT(struct dsc$descriptor *infile, struct dsc$descriptor *outfile)
X{
X static int first_time = 1;
X char *instr, *outstr;
X register int status;
X register BUFFER *bp; /* buffer list pointer */
X char bname[NBUFN]; 	/* buffer name of file to read */
X
X called = 1;
X if (first_time)
X {
X 	first_time = 0;
X 	vtinit();
X 	edinit(mainbuf);	/* Buffers, windows */
X 	varinit();		/* user variables */
X 	initchars();		/* character set definitions */
X 	dcline( 0, NULL, TRUE);
X }
X else TTopen(0);
X
X outstr = strncpy( calloc( 1, 1+outfile->dsc$w_length),
X										outfile->dsc$a_pointer, outfile->dsc$w_length);
X
X if (infile->dsc$w_length <= 0)
X 	instr = outstr;
X else 
X		instr = strncpy( calloc( 1, 1+infile->dsc$w_length),
X 									infile->dsc$a_pointer, infile->dsc$w_length);
X
X makename( bname, outstr);
X unqname(bname);
X bp = bfind(bname, TRUE, 0);
X strcpy(bp->b_fname, instr);
X bp->b_active = FALSE;
X swbuffer( bp);
X strcpy(bp->b_fname, outstr);
X bp->b_flag |= BFCHG; 					 /* flag it as changed */
X free( instr);
X free( outstr);
X pd_sgarbf = TRUE;
X status = edit loop();
X
Xabortrun:
X TTclose(0);
X return status;
X}
X
X
XPascal bktoshell(int f, int n)
X{
X/*
X Pause this process and wait for it to be woken up
X*/
X unsigned pid;
X char *env, *dir;
X int argc;
X char *argv[ 16];
X
X if (called)
X {
X 	mlwrite( "Called MicroEMACS can't be suspended.");
X 	return FALSE;
X }
X
X env = getenv("MICROEMACS$PARENT");
X
X if (env == NULL)
X {
X 	mlwrite( "No parent process.");
X 	return FALSE;
X }
X tcapmove(term.t_nrowm1, 0);
X /*movecursor(term.t_nrow, 0);*/
X TTclose(0);
X
X test( LIB$DELETE_LOGICAL(
X 												DESCPTR( "MICROEMACS$PARENT"),
X 												DESCPTR( "LNM$JOB")));
X test( LIB$GETJPI( &JPI$_PID, 0, 0, &pid, 0, 0));
X test( LIB$SET_LOGICAL(
X 											DESCPTR( "MICROEMACS$PROCESS"),
X 											descptr( int_asc( pid)),
X 											DESCPTR( "LNM$JOB")));
X pid = atoi( env);
X test( SYS$WAKE( &pid, 0));
X
X for(;;)
X {	/* Hibernate until MICROEMACS$COMMAND is defined */
X 	test( SYS$HIBER());
X 	env = getenv( "MICROEMACS$COMMAND");	/* Command line arguments */
X 	if (env != NULL) break; /* Winter is over */
X }
X
X test( LIB$DELETE_LOGICAL(
X 												DESCPTR( "MICROEMACS$COMMAND"),
X 												DESCPTR( "LNM$JOB")));
X TTopen(0);
X
X argv[ 0] = env;
X argc = 1;
X for( ; ;)
X {	/* Define each argument */
X 	if (*env == 0x80)
X 	{ /* Seperator */
X 		argv[argc++] = env+1;
X 		if (argc > 15) break;
X 		*env++ = 0;
X 	}
X 	else if (*env++ == 0) break;
X }
X/*
X First parameter is default device
X*/
X test( LIB$SET_LOGICAL(
X 											DESCPTR( "SYS$DISK"),
X 											descptr( argv[ 0]),0));
X/*
X Second argument is default directory
X*/
X test( SYS$SETDDIR( descptr( argv[ 1]), 0, 0));
X/*
X Remaining came from command line
X*/
X pd_sgarbf = TRUE;
X dcline( argc-2, &argv[ 2], FALSE);
X return TRUE;
X}
X
#endif

#if 1

char * utovmsfn(char * t_, char * s_)

{ char * t = t_;
	char * s = s_;
	char * lt;
	
	while (s[0] != 0 && *s != '/')
		++s;

	if (s[0] == 0)
	{ strcpy(t_,s_);
		return t_;
	}
	else
	{ s = s_;
		*t++ = '[';
		
		while (s[0] == '.' && (s[1] == '.' && s[2] == '/'|| s[1] == '/'))
		{ if (s[1] != '.') 
				s += 2;
			else
			{ *t++ = '-';
				s += 3;
			}
		}
		
		lt = t;
		*t++ = '.';

		while (s[0] != 0 && (isalpha(s[0]) || isdigit(s[0])))
		{ while (*s != 0 && *s != '/')
				*t++ = *s++;
			if (*s == 0) break;
			++s;
			lt = t;
			*t++ = '.';
		}
		
		*lt = ']';
		*t++ = 0; 
		
		return t_;		
	}
}


/*
 * Here are the much faster I/O routines.  Skip the C stuff, use
 * the VMS I/O calls.  Puts the files in standard VMS format, too.
 */
#define successful(s) ((s) & 1)
#define unsuccessful(s) (!((s) & 1))

static struct FAB fab;		/* a file access block */
static struct RAB rab;		/* a record access block */

					/* Open a file for reading. */
int Pascal ffropen(char *fn)
{
	unsigned long status;
	char vmsfn[1000];
				/* initialize structures */
	fab=cc$rms_fab;
	rab=cc$rms_rab;

	fab.fab$l_fna = utovmsfn(vmsfn,fn);
	fab.fab$b_fns = strlen(fab.fab$l_fna);
	fab.fab$b_fac = FAB$M_GET;
	fab.fab$b_shr = FAB$M_SHRGET;
	fab.fab$l_fop = FAB$M_SQO;

	rab.rab$l_fab = &fab;
	rab.rab$l_rop = RAB$M_RAH;	/* read-ahead for multibuffering */

	status=SYS$OPEN(&fab);
	if (status==RMS$_FLK)
	{  /* File locking problem:
			* Add the SHRPUT option, allowing shareability with other writers.
			* This lets us read batch logs and stuff like that. Don't turn it on
			* automatically since adding this sharing eliminates the read-ahead
			*/
		fab.fab$b_shr |= FAB$M_SHRPUT;
		status=SYS$OPEN(&fab);
	}

	if (successful(status))
	{ if (unsuccessful(SYS$CONNECT(&rab)))
		{
			SYS$CLOSE(&fab);
			return FIOFNF;
		}
	}
	else return FIOFNF;

	return FIOSUC;
}

/*
 * Pascal ffwopen(char *fn, char *mode)
 *
 * fn = file name, mode = 1 must not exist
 */
int Pascal ffwopen(int mode, char *fn)
{
	unsigned long status;
	char vmsfn[1000];

	/* initialize structures */
	fab=cc$rms_fab;
	rab=cc$rms_rab;

	fab.fab$l_fna = utovmsfn(vmsfn,fn);
	fab.fab$b_fns = strlen(fab.fab$l_fna);
	fab.fab$b_fac = FAB$M_PUT;	/* writing this file */
	fab.fab$b_shr = FAB$M_NIL;	/* no other writers */
	fab.fab$l_fop = FAB$M_SQO;	/* sequential ops only */
	fab.fab$b_rat = FAB$M_CR; /* carriage returns on ends */
	fab.fab$b_rfm = FAB$C_VAR;	/* variable length file */

	rab.rab$l_fab = &fab;
	rab.rab$l_rop = RAB$M_WBH;	/* write behind - multibuffer */

	if (0)
	{
		/* append mode */
		rab.rab$l_rop = RAB$M_EOF;
		status=SYS$OPEN(&fab);
		if (status == RMS$_FNF)
				status=SYS$CREATE(&fab);
	}
	else	/* *mode == 'w' */
	{
		/* write mode */
		fab.fab$l_fop |= FAB$M_MXV; /* always make a new version */
		status=SYS$CREATE(&fab);
	}

	if (successful(status))
	{
		status=SYS$CONNECT(&rab);
	}

	if (unsuccessful(status))
		return -1;
	return 1;
}

/*
 * Close a file. Should look at the status in all systems.
 */
int Pascal ffclose()
{
	unsigned long status;
							/* free this since we do not need it anymore */
	if (fline)
	{	free(fline);
		fline = NULL;
	}

	status = SYS$DISCONNECT(&rab);
	if (successful(status)) 
		status = SYS$CLOSE(&fab);
	else
		SYS$CLOSE(&fab);

	if (unsuccessful(status))
	{	mlwrite(TEXT156);
/*											"Error closing file" */
		return FIOERR;
	}
	return FIOSUC;
}

extern void Pascal ucrypt(char *, int);

/*
 * Write a line to the already opened file. The "buf" points to the buffer,
 * and the "nbuf" is its length, less the free newline. Return the status.
 * Check only at the newline.
 */
int Pascal ffputline(char buf[], int nbuf)
{
				register char *obuf=buf;

#if CRYPT
	if (cryptflag)
	{
		/* get a reasonable buffer */
		if (fline && g_flen < nbuf)
		{
			free(fline);
			fline = NULL;
		}

		if (fline == NULL)
		{
			if ((fline=malloc(g_flen = nbuf+NSTRING))==NULL)
			{
				return FIOMEM;
			}
		}

		/* copy data */
		memcpy(fline,buf,nbuf);

		/* encrypt it */
		ucrypt(fline,nbuf);

		/* repoint output buffer */
		obuf=fline;
	}
#endif

				/* set output buffer */
	rab.rab$l_rbf = obuf;
	rab.rab$w_rsz = nbuf;

	if (unsuccessful(SYS$PUT(&rab))) 
	{
		mlwrite(TEXT157);
/*											"Write I/O error" */
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
int Pascal ffgetline(int * nbytes)

{
	unsigned long status;

	/* if we don't have an fline, allocate one */
	if (fline == NULL)
	{ g_flen = fab.fab$w_mrs ? fab.fab$w_mrs+1 : 32768;
		fline = malloc(g_flen);
			
		if (fline == NULL)
			return FIOMEM;
	}

	/* read the line in */
	rab.rab$l_ubf = fline;
	rab.rab$w_usz = g_flen;

	status=SYS$GET(&rab);
	*nbytes = rab.rab$w_rsz;
	if (status == RMS$_EOF) return FIOEOF;
	if (unsuccessful(status)) 
	{
		mlwrite(TEXT158);
/*											"File read error" */
		return FIOERR;
	}

	/* terminate and decrypt the string */
	fline[rab.rab$w_rsz] = 0;
				
#if CRYPT
	if (cryptflag)
		ucrypt(fline, *nbytes);
#endif
	return FIOSUC;
}

#endif

#if 0

X
X
X/***********************************************************
X* FUNCTION - addspec - utility function for expandargs
X***********************************************************/
X#define ADDSPEC_INCREMENT 10
Xstatic void Pascal addspec(struct dsc$descriptor dsc, int *pargc,
X 			char ***pargv, int *pargcapacity)
X{
X	 char *s;
X
X 	 /* reallocate the argument array if necessary */
X	 if (*pargc == *pargcapacity)
X			 *pargv = realloc(*pargv,sizeof(**pargv) * (*pargcapacity += ADDSPEC_INCREMENT));
X
X 	 /* allocate new argument */
X	 s=strncpy(malloc(dsc.dsc$w_length+1),dsc.dsc$a_pointer,dsc.dsc$w_length);
X	 s[dsc.dsc$w_length]=0;
X
X 	 /* put into array */
X	 (*pargv)[(*pargc)++] = s;
X}
X
X/***********************************************************
X* FUNCTION - expandargs - massage argc and argv to expand
X* wildcards by calling VMS.
X***********************************************************/
Xvoid Pascal expandargs(int *pargc, char ***pargv)
X{
X	int argc = *pargc;
X	char **argv = *pargv;
X
X	int nargc=0;
X	char **nargv=NULL;
X	int nargcapacity=0;
X
X	struct dsc$descriptor result_filespec={0,DSC$K_DTYPE_T,DSC$K_CLASS_D,NULL};
X
X 	 /* loop over all arguments */
X	while (argc--)
X	{
X		struct dsc$descriptor filespec={strlen(*argv),DSC$K_DTYPE_T,DSC$K_CLASS_S,*argv};
X 	unsigned long context=0;
X
X /* should check for wildcards: %, *, and "..." */
X 	if (**argv != '-' && (strchr(*argv,'%') || strchr(*argv,'*') ||
X 			strstr(*argv,"...")))
X 	{
X 		/* search for all matching filenames */
X 		while ((LIB$FIND_FILE(&filespec,&result_filespec,&context)) & 1)
X 		{
X 			int i;
X
X 			/* LIB$FIND_FILE returns uppercase. Lowercase it */
X 			for (i=0;i<result_filespec.dsc$w_length;i++)
X 				if (is_upper(result_filespec.dsc$a_pointer[i]))
X 					result_filespec.dsc$a_pointer[i] = tolower(result_filespec.dsc$a_pointer[i]);
X
X 				addspec(result_filespec,&nargc,&nargv,&nargcapacity);
X 		}
X 	}
X 	else
X 		addspec(filespec,&nargc,&nargv,&nargcapacity);
X
X 	LIB$FIND_FILE_END(&context);
X
X 	argv++;
X }
X
X	 STR$FREE1_DX(&result_filespec);
X
X	 *pargc=nargc;
X	 *pargv=nargv;
X}
X
#endif

int Pascal millisleep(unsigned int n)

{ 
#if S_HPUX
		extern nanosleep(long nsecs);
		nanosleep(1000000L * n);
#elif S_LINUX
		extern usleep(int usecs);
		usleep(1000 * n);
#else
	struct timeval tvs;
	tvs.tv_sec = 0;
	tvs.tv_usec = n * 10;
	select(0, null, null, null, &tvs);
	loginfo1("millisleep %d", n);
#endif
	return 0;
}

#else
Pascal vms_hello()
{
}
#endif
