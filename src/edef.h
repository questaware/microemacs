/*	EDEF:		Global variable definitions for
			MicroEMACS 3.9

                        written by Daniel Lawrence
                        based on code by Dave G. Conroy,
                        	Steve Wilhite and George Jones
*/

#include "epredef.h"

#define CTRL	0x0100		/* Control flag, or'ed in		*/
#define META	0x0200		/* Meta flag, or'ed in			*/
#define CTLX	0x0400		/* ^X flag, or'ed in			*/
#define SPEC	0x0800		/* special key (function keys)		*/
#define MOUS	0x1000		/* alternative input device (mouse)	*/
#define	SHFT	0x2000		/* shifted (for function keys)		*/
#define	ALTD	0x4000		/* ALT key...				*/
#define NOTKEY  0x8000

#define	BINDNUL	0		/* not bount to anything		*/
#define	BINDFNC	1		/* key bound to a function		*/
#define	BINDBUF	2		/* key bound to a buffer		*/


#if 0
#ifndef null_sct
extern char nulls[];
#define null_sct(T) (*(T*)nulls[0])
#endif

#ifndef fieldoffs
#define fieldoffs(mode, fld) ((Int)&((mode)0)->fld)
#endif
#endif

#define when break; case
#define otherwise break; default:

#ifdef	FALSE
#undef	FALSE
#endif
#ifdef	TRUE
#undef	TRUE
#endif

#define false 0
#define true  1

#define FALSE	 0
#define TRUE	 1
#define ABORT	 2	/* ^G, abort, etc.	*/

	 /* order is important */
#define	PLAY   (-1)	/*		  playing	*/
#define STOP	0	/* keyboard macro not in use	*/
#define	RECORD	1	/*		  recording	*/

#define kbd_play(x)   (x < STOP)
#define kbd_record(x) (x > STOP)
				/* Completion types	*/
#define	CMP_BUFFER	0
#define	CMP_COMMAND	1
#define	CMP_FILENAME	2
				/* Directive definitions */
#define DIF		0
#define DELSE		1
#define DENDIF		2
#define DGOTO		3
#define DRETURN		4
#define DENDM		5
#define DWHILE		6
#define	DENDWHILE	7
#define	DBREAK		8
#define DFORCE		9

/*
 * PTBEG, PTEND, FORWARD, and REVERSE are all toggle-able values for
 * the scan routines.
 */
#define PTBEG	0	/* Leave the point at the beginning on search	*/
#define	PTEND	1	/* Leave the point at the end on search		*/
#define	FORWARD	0			/* forward direction		*/
#define REVERSE	1			/* backwards direction		*/

#define FIOSUC	0		/* File I/O, success.		*/
#define FIOFNF	1		/* File I/O, file not found.	*/
#define FIOEOF	2		/* File I/O, end of file.	*/
#define FIOERR	3		/* File I/O, error.		*/
#define	FIOMEM	4		/* File I/O, out of memory	*/
#define	FIOFUN	5		/* File I/O, eod of file/bad line*/
#define	FIODEL	6		/* Can't delete/rename file	*/

#define CFCPCN	0x0001			/* Last command was C-P, C-N	*/
#define CFKILL  0x0002


#define WFFORCE 0x01		/* Window needs forced reframe	*/
#define WFMOVE	0x02		/* Movement from line to line	*/
#define WFEDIT	0x04		/* Editing within a line	*/
#define WFHARD	0x08		/* Better to do a full display	*/
#define WFMODE	0x10		/* Update mode line.		*/
#define	WFCOLR	0x20		/* Needs a color change		*/
#define	WFTXTD  0x40		/* Text down one line           */
#define	WFTXTU  0x80		/* Text up one line             */


#define NUMFLAGS  4

#define BFINVS	0x01		/* Internal invisable buffer	*/
#define BFCHG	0x02		/* Changed since last write	*/
#define	BFTRUNC	0x04		/* buffer was truncated when read */
#define	BFNAROW	0x08		/* buffer has been narrowed	*/

				/*	mode flags	*/
#define	NUMMODES  10		/* # of defined modes	       */

#define MDSTT   0x0010
#define MDWRAP	0x0010		/* word wrap			*/
#define	MDCMOD	0x0020		/* C indentation and fence match*/
#define	MDMS	0x0040		/* File to have CRLF		*/
#define	MDEXACT	0x0080		/* Exact matching for searches	*/
#define	MDVIEW	0x0100		/* read-only buffer		*/
#define MDOVER	0x0200		/* overwrite mode		*/
#define MDMAGIC	0x0400		/* regular expresions in search */
#define	MDCRYPT	0x0800		/* encrytion mode active	*/
#define	MDASAVE	0x1000		/* auto-save mode		*/
#define MDSRCHC 0x2000          /* search comments also */

#define BSRCH    0x2000         /* currently searching this buffer */
#define MDDIR    0x4000		/* this file is a directory	*/
#define BFACTIVE 0x8000		/* this buffer is active */

				/* language properties */
#define BINDENT	0x01		/* Auto indent */
#define BCCOMT  0x02		/* c style comments */
#define BCPRL   0x04		/* perl style comments */
#define BCFOR   0x08		/* fortran style comments */
#define BCSQL   0x10            /* sql style comments */
#define BCPAS   0x20		/* pascal style comments */


/*	Macro argument token types			*/

#define TKNUL	0	/* end-of-string		*/
#define	TKARG	1	/* interactive argument		*/
#define	TKBUF	2	/* buffer argument		*/
#define	TKVAR	3	/* user variables		*/
#define	TKENV	4	/* environment variables	*/
#define	TKFUN	5	/* function....			*/
#define	TKDIR	6	/* directive			*/
#define	TKLBL	7	/* line label			*/
#define	TKLIT	8	/* numeric literal		*/
#define	TKSTR	9	/* quoted string literal	*/
#define	TKCMD  10	/* command name			*/

/*
 * Modes of working
 */
					
#define MD_KEEP_MACROS 1	/* keep macro befers when failing */
#define MD_KEEP_CR     2	/* keep CR in files */
#define MD_NO_MMI      4	/* no man machine interface */


/*
 * Incremental search defines.
 */
#if	ISRCH

#define CMDBUFLEN	100	/* Length of our command buffer */

#define IS_ABORT	0x07	/* Abort the isearch */
#define IS_BACKSP	0x08	/* Delete previous char */
#define	IS_TAB		0x09	/* Tab character (allowed search char) */
#define IS_NEWLINE	0x0D	/* New line from keyboard (Carriage return) */
#define	IS_QUOTE	0x11	/* Quote next character */
#define IS_REVERSE	0x12	/* Search backward */
#define	IS_FORWARD	0x13	/* Search forward */
#define	IS_VMSQUOTE	0x16	/* VMS quote character */
#define	IS_VMSFORW	0x18	/* Search forward for VMS */
#define	IS_QUIT		0x1B	/* Exit the search */
#define	IS_RUBOUT	0x7F	/* Delete previous character */

/* IS_QUIT is no longer used, the variable metac is used instead */

#endif

                        /* Structure for the table of current key bindings */
union EPOINTER {
        int (Pascal *fp)(int,int);      /* C routine to invoke */
        BUFFER *buf;                    /* buffer to execute */
};
			/* Structure for the key binding table */
typedef struct
{ short k_code;	        /* Key code, 0 => end of table */
  short k_type;	        /* binding type (C function or buffer) */
  union EPOINTER k_ptr; /* ptr to thing to execute */
} KEYTAB;

			/* Structure for the name binding table */
typedef struct 
{	const char *n_name;			/* name of function key */
	int (*n_func)(int, int);	/* function name is bound to */
}	NBIND;

/*	The editor holds deleted text chunks in the KILL buffer. The
	kill buffer is logically a stream of ascii characters, however
	due to its unpredicatable size, it gets implemented as a linked
	list of chunks. (The d_ prefix is for "deleted" text, as k_
	was taken up by the keycode structure)
*/
typedef struct KILL {
	struct KILL *d_next;   /* link to next chunk, NULL if last */
	char d_chunk[KBLOCK];	/* deleted text */
} KILL;

typedef struct Paren_s
{ 
  char          ch;
  char          fence;
  int           nest;
  int           nestclamped;
  short         in_mode;
  short         complex;
  int           sdir;
  char          prev;
  char          olcmt;
} Paren_t, *Paren;

extern Paren_t paren;

					   /*  Internal defined functions */
#define nextab(a)	((a - (a % tabsize)) + tabsize)
#define nextabp(a,sz)	((a - (a % sz)) + sz)
#define chcaseunsafe(ch)   (ch ^ 0x20)

#define LFSTR 132


/* from MAIN.C */

extern int kinsert_n;
extern int g_got_uarg;
extern int g_got_search;

/* initialized global external declarations */
NOSHARE extern int g_clexec;	/* command line execution flag	*/
NOSHARE extern int g_numcmd;	/* number of bindable functions */
NOSHARE extern int g_nosharebuffs;  /* never allow different files in the same buffer */

NOSHARE extern char *ekey;		/* global encryption key	*/
NOSHARE extern char *execstr;		/* pointer to string to execute */
NOSHARE extern char golabel[];		/* current line to go to	*/
NOSHARE extern int g_execlevel;		/* execution IF level		*/
extern const char mdname[NUMMODES][8];		/* text names of modes		*/
NOSHARE extern int  g_numcmd;		/* number of bindable functions */
NOSHARE extern const NBIND names[];	/* name to function table	*/
NOSHARE extern int gfcolor;		/* global forgrnd color (white) */
NOSHARE extern int gbcolor;		/* global backgrnd color (black)*/
NOSHARE extern int mpresf;		/* Stuff in message line	*/
NOSHARE extern int vtrow;		/* Row location of SW cursor	*/
NOSHARE extern int vtcol;		/* Column location of SW cursor */
NOSHARE extern int ttinit;		/* => ttrow is wrong */
NOSHARE extern int ttrow;		/* Row location of HW cursor	*/
NOSHARE extern int ttcol;		/* Column location of HW cursor */
NOSHARE extern int lbound;		/* leftmost column of current line
					   being displayed		*/
NOSHARE extern int reptc;		/* current universal repeat char*/
NOSHARE extern int abortc;		/* current abort command char	*/
NOSHARE extern int sterm;		/* search terminating character */

NOSHARE extern int prenum;		/*     "       "     numeric arg */

extern const char cname[][9];		/* names of colors		*/
NOSHARE extern char highlight[64];	/* the highlight string */

NOSHARE extern int cryptflag;		/* currently encrypting?	*/
NOSHARE extern int restflag;		/* restricted use?		*/
NOSHARE extern int g_newest;            /* choose newest file           */
NOSHARE extern Int envram;		/* # of bytes current in use by malloc */
const extern char errorm[];		/* error literal		*/
const extern char truem[];		/* true literal 		*/
const extern char falsem[];		/* false litereal		*/
NOSHARE extern char palstr[49];		/* palette string		*/
NOSHARE extern char lastmesg[LFSTR];	/* last message posted		*/
NOSHARE extern int (Pascal *lastfnc)(int, int);/* last function executed */
NOSHARE extern char *fline; 		/* dynamic return line */
NOSHARE extern unsigned int g_flen;	/* max len(fline) */
NOSHARE extern int eexitflag;		/* EMACS exit flag */

/* uninitialized global external declarations */

NOSHARE extern int currow;	/* Cursor row			*/
NOSHARE extern int curcol;	/* Cursor column		*/
NOSHARE extern int thisflag;	/* Flags, this command		*/
NOSHARE extern int lastflag;	/* Flags, last command		*/
NOSHARE extern int curgoal;	/* Goal for C-P, C-N		*/
NOSHARE extern Short g_clring;

NOSHARE extern WINDOW *curwp; 		/* Current window		*/
NOSHARE extern BUFFER *curbp; 		/* Current buffer		*/
NOSHARE extern WINDOW *wheadp;		/* Head of list of windows	*/
NOSHARE extern BUFFER *bheadp;		/* Head of list of buffers	*/
NOSHARE extern BUFFER *blistp;		/* Buffer for C-X C-B		*/

#if	DIACRIT
NOSHARE	extern char lowcase[HICHAR];	/* lower casing map		*/
NOSHARE	extern char upcase[HICHAR];	/* upper casing map		*/
#endif

        extern char pat[NPAT+10];	/* Search pattern		*/
NOSHARE extern char tap[NPAT+10];	/* Reversed pattern array.	*/
NOSHARE extern char rpat[NPAT+10];	/* replacement pattern		*/

	extern char *g_file_prof;	/* profiles of files */

/*	Various "Hook" execution variables	*/
NOSHARE extern KEYTAB hooks[6];		/* executed on all file reads */

NOSHARE extern char *patmatch;
NOSHARE extern int lastdir;

#if	DEBUGM
/*	vars needed for macro debugging output	*/
NOSHARE extern char outline[];	/* global string to hold debug line text */
#endif



NOSHARE extern TERM	term;		/* Terminal information.	*/


#define readhook hooks[0]	/* executed on all file reads */
#define wraphook hooks[1]	/* executed when wrapping text */
#define cmdhook hooks[2]	/* executed before looking for a command */
#define writehook hooks[3]	/* executed on all file writes */
#define exbhook hooks[4]	/* executed when exiting a buffer */
#define bufhook hooks[5]	/* executed when entering a buffer */

#define C_BOLD    (8+1)
#define C_USCORE  (8+4)
#define C_BLINK   (8+5)
#define C_REV	  (8+7)

#define C_BLACK   0
#define C_RED     1
#define C_GREEN   2
#define C_YELLOW  3
#define C_BLUE	  4
#define C_MAGENTA 5
#define C_CYAN	  6
#define C_WHITE	  7

#define BG(x) ((x)<<12)
#define FG(x) ((x)<<8)


#define Q_IN_ESC   1
#define Q_IN_CHAR  2
#define Q_IN_STR   4
#define Q_IN_CMT0  8
#define Q_IN_CMTL 16
#define Q_IN_CMT  32
#define Q_IN_CMT_ 64
#define Q_IN_EOL 128

/*
	This is the message which should be added to any "About MicroEMACS"
	boxes on any of the machines with window managers.


	------------------------------------------
	|					 |
	|	 MicroEMACS v3.xx		 |
	|		for the ............	 |
	|					 |
	|    Text Editor and Corrector		 |
	|					 |
	|    written by Daniel M. Lawrence	 |
	|    [based on code by Dave Conroy]	 |
	|					 |
	|    Send inquiries and donations to:	 |
	|    617 New York St			 |
	|    Lafayette, IN 47901		 |
	|					 |
	------------------------------------------
*/

