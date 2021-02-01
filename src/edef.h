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
#define BFCHG	  0x02		/* Changed since last write	*/
#define	BFTRUNC	0x04		/* buffer was truncated when read */
#define	BFNAROW	0x08		/* buffer has been narrowed	*/

				/*	mode flags	*/
#define	NUMMODES  10		/* # of defined modes	       */

#define MDSTT   0x0010
#define MDWRAP	0x0010		/* word wrap			*/
#define	MDCMOD	0x0020		/* C indentation and fence match*/
#define	MDMS		0x0040		/* File to have CRLF		*/
#define	MDEXACT	0x0080		/* Exact matching for searches	*/
#define	MDVIEW	0x0100		/* read-only buffer		*/
#define MDOVER	0x0200		/* overwrite mode		*/
#define MDMAGIC	0x0400		/* regular expresions in search */
#define	MDCRYPT	0x0800		/* encrytion mode active	*/
#define	MDASAVE	0x1000		/* auto-save mode		*/
#define MDSRCHC 0x2000    /* search comments also */

#define BSRCH    0x2000   /* currently searching this buffer */
#define MDDIR    0x4000		/* this file is a directory	*/
#define BFACTIVE 0x8000		/* this buffer is active */

#define BSOFTTAB 0x01
#define BCRYPT2	 0x02

				/* language properties */
#define BINDENT	0x01		/* Auto indent */
#define BCCOMT  0x02		/* c style comments */
#define BCPRL   0x04		/* perl style comments */
#define BCFOR   0x08		/* fortran style comments */
#define BCSQL   0x10    /* sql style comments */
#define BCPAS   0x20		/* pascal style comments */
#define BCML    0x40    /* Markup language */


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
					
#define MD_KEEP_MACROS 1	/* keep macro buffers when failing */
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

typedef struct
{ struct LINE * curline;
  short	        curoff;
  unsigned int  line_no;
} Lpos_t;

typedef struct MARK
{ struct LINE *markp;	/* Line containing "mark"	*/
  int	       marko;	/* Byte offset for "mark"	*/
} MARK;

typedef struct MARKS
{ struct MARK c[NMARKS];	
}    MARKS;


/*
 * There is a window structure allocated for every active display window. The
 * windows are kept in a big list, in top to bottom screen order, with the
 * listhead at "wheadp". Each window contains its own values of dot and mark.
 * The flag field contains some bits that are set by commands to guide
 * redisplay. Although this is a bit of a compromise in terms of decoupling,
 * the full blown redisplay is just too expensive to run for every input
 * character.
 */
typedef struct	WINDOW
{	struct	WINDOW *w_wndp; 	/* Next window			*/
	struct	BUFFER *w_bufp; 	/* Buffer displayed in window	*/
	struct	LINE *w_linep;		/* Top line in the window	*/
  struct  MARKS mrks;
	struct	LINE *w_dotp;		/* Line containing "."		*/
	short	       w_doto; 		/* offset for "."; isomorphism ends */
	unsigned int w_line_no;		/* Lpos_t isomorphism ends      */
	char	w_toprow;		/* Origin 0 top row of window	*/
	char	w_ntrows;		/* # of rows in window inc MLine*/
	char	w_force;		/* If NZ, forcing row.		*/
	char	w_flag; 		/* Flags.			*/
#if	0
	int   w_color;		/* current colors		*/
#endif
	int		w_fcol; 		/* first column displayed	*/
}	WINDOW;


typedef char * CRYPTKEY;

/* Text is kept in buffers. A buffer header, described below, exists for every
 * buffer in the system. The buffers are kept in a big list, so that commands
 * that search for a buffer by name can find the buffer header. There is a
 * safe store for the dot and mark in the header, but this is only valid if
 * the buffer is not being displayed (that is, if "b_nwnd" is 0). The text for
 * the buffer is kept in a circularly linked list of lines, with a pointer to
 * the header line in "b_linep"	Buffers may be "Inactive" which means the files associated with them
 * have not been read in yet. These get read in at "use buffer" time.
 */
typedef struct	BUFFER
{	struct	BUFFER *b_bufp; 	/* Link to next BUFFER		*/
	struct	LINE *b_baseline;	/* Link to the header LINE	*/
	struct	LINE *b_wlinep;		/* Link top LINE in last window */
  struct  MARKS mrks;
	struct	LINE *b_dotp;		  /* Link to "." LINE structure	*/
	short	  b_doto; 		      /* offset of "."; isomorphism ends */
	short   b_luct;			      /* last use count		*/ 
	struct	LINE *b_narlims[2];/* Link to narrowed top, bottom text */
	char    b_langprops;		  /* type of language of contents */
  signed char	b_nwnd; 		  /* Count of windows on buffer	*/
	short	  b_flag; 		      /* Flags and modes  */
	unsigned char b_mode;	    /* Flags and modes (extra) */
	unsigned char b_tabsize;	/* size of hard tab		*/
#if	COLOR
	int   	b_color;					/* current colors		*/
#endif
#if	CRYPT
	CRYPTKEY b_key;
#endif
	char *  b_fname;					/* malloc'd and owned by BUFFER */
	char *	b_remote;					/* remote command 		*/
	char	  b_bname[1]; 			/* Buffer name			*/
}	BUFFER;
				/* for compatibility: */
#define b_linep b_baseline

/*
 * The starting position of a region, and the size of the region in
 * characters, is kept in a region structure.  Used by the region commands.
 */
typedef struct	{
	struct	LINE *r_linep;		/* Origin LINE address. 	*/
	short	r_offset;		/* Origin LINE offset.		*/
	long	r_size; 		/* Length in characters.	*/
	int	r_lines;		/* number of lines in the buffer*/
}	REGION;

/*
 * All text is kept in circularly linked lists of "LINE" structures. These
 * begin at the header line (which is the blank line beyond the end of the
 * buffer). This line is pointed to by the "BUFFER". Each line contains a the
 * number of bytes in the line (the "used" size), the size of the text array,
 * and the text. The end of line is not stored as a byte; it's implied.
 */
typedef struct	LINE {
	struct LINE * l_fp;		/* Link to the next line	*/
	struct LINE * l_bp;		/* Link to the previous line	*/
	short	l_used; 		    /* Used size			*/
  unsigned char l_spare;/* spare space */
	char  l_props;
	char	l_text[1];		/* A bunch of characters.	*/
}	LINE;

#define L_IS_HD 1

#define lforw(lp)	((lp)->l_fp)
#define lback(lp)	((lp)->l_bp)
#define Lineptr		struct LINE *          

#define lgetc(lp, n)	((lp)->l_text[n]&0xFF)
#define lgets(lp, n)	&(lp)->l_text[n]
#define lputc(lp, n, c) ((lp)->l_text[n]=(c))
#define llength(lp)	((lp)->l_used)

/*
 * The editor communicates with the display using a high level interface. A
 * "TERM" structure holds useful variables, and indirect pointers to routines
 * that do useful operations. The low level get and put routines are here too.
 * This lets a terminal, in addition to having non standard commands, have
 * funny get and put character code too. The calls might get changed to
 * "termp->t_field" style in the future, to make it possible to run more than
 * one terminal type.
 */
typedef struct	{
	short	t_mrowm1; 		 /* max number of rows allowable */
	short	t_nrowm1; 		/* current number of rows used	*/
	short	t_mcol; 		/* max Number of columns.	*/
	short	t_ncol; 		/* current Number of columns.	*/
	short	t_margin;		/* min margin for extended lines*/
	short	t_scrsiz;		/* size of scroll region "	*/
}	TERM;

typedef int Pascal Emacs_cmd(int, int);

                        /* Structure for the table of current key bindings */
union EPOINTER {
        Emacs_cmd * fp;      							/* C routine to invoke */
        BUFFER *    buf;                  /* buffer to execute */
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
NOSHARE extern int g_numcmd;	/* number of bindable functions */
NOSHARE extern int g_clexec;	/* command line execution flag	*/
NOSHARE extern int g_nosharebuffs;  /* dont allow different files in same buffer */

NOSHARE extern CRYPTKEY g_ekey;		/* global encryption key	*/
NOSHARE extern char *execstr;		/* pointer to string to execute */
NOSHARE extern char golabel[];		/* current line to go to	*/
NOSHARE extern int g_execlevel;		/* execution IF level		*/
extern const char mdname[NUMMODES][8];		/* text names of modes		*/
extern const NBIND names[];	/* name to function table	*/
NOSHARE extern int g_colours;		/* backgrnd (black*256) + foreground (white) */
NOSHARE extern int mpresf;		/* Stuff in message line	*/
NOSHARE extern int vtrow;		/* Row location of SW cursor	*/
NOSHARE extern int ttinit;		/* => ttrow is wrong */
NOSHARE extern int ttrow;		/* Row location of HW cursor	*/
NOSHARE extern int ttcol;		/* Column location of HW cursor */
NOSHARE extern int lbound;		/* leftmost col of current line being displayed*/
NOSHARE extern int abortc;		/* current abort command char	*/
NOSHARE extern int sterm;		/* search terminating character */

NOSHARE extern int prenum;		/*     "       "     numeric arg */

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
extern int g_cliplife;


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

