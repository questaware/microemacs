/*	EDEF:		Global variable definitions for
			MicroEMACS 3.9

                        written by Daniel Lawrence
                        based on code by Dave G. Conroy,
                        	Steve Wilhite and George Jones
*/

#if S_WIN32
#include <windows.h>
#define Filetime FILETIME
#else
#define Filetime time_t
#endif


#define CTRL	 0x0100		/* Control flag, or'ed in		*/
#define META	 0x0200		/* Meta flag, or'ed in			*/
#define CTLX	 0x0400		/* ^X flag, or'ed in			*/
#define SPEC	 0x0800		/* special key (function keys)		*/
#define MOUS	 0x1000		/* alternative input device (mouse)	*/
#define	SHFT	 0x2000		/* shifted (for function keys)		*/
#define	ALTD	 0x4000		/* ALT key...				*/
#define NOTKEY 0x8000

#define	BINDFNC	0		/* key bound to a function		*/
#define	BINDBUF	1		/* key bound to a buffer		*/

typedef int (*Command)(int f, int n);

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

#define ABORT	(-1)	/* ^G, abort, etc.	*/
#define FALSE	 0
#define TRUE	 1

	 /* order is important */
#define	PLAY   (-1)	/*		  playing	*/
#define STOP	  0	/* keyboard macro not in use	*/
#define	RECORD	1	/*		  recording	*/

#define kbd_play(x)   (x < STOP)
#define kbd_record(x) (x > STOP)
				/* Completion types	*/
#define	CMP_BUFFER	 0
#define	CMP_COMMAND	 1
#define	CMP_FILENAME 2
#define	CMP_TOSPACE  3
				/* Directive definitions */
#define DIF		    0
#define DELSE			1
#define DENDIF		2
#define DGOTO			3
#define DRETURN		4
#define DENDM			5
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
#define FIOEOF -1		/* File I/O, end of file.	*/
#define	FIOMEM -2		/* File I/O, out of memory	*/
#define FIOFNF -3		/* File I/O, file not found.	*/
#define FIOERR -4		/* File I/O, error.		*/
#define	FIOFUN -5		/* File I/O, eod of file/bad line*/
#define	FIODEL -6		/* Can't delete/rename file	*/

#define CFKILL  0x0001
#define CFCPCN	0x0002			/* Last command was C-P, C-N	*/


#define	WFTXTU  0x01		/* Text up one line             */
#define WFFORCE 0x02		/* Window needs forced reframe	*/
#define WFMOVE	0x04		/* Movement from line to line	*/
#define WFEDIT	0x08		/* Editing within a line	*/
#define WFHARD	0x10		/* Better to do a full display	*/
#define WFMODE	0x20		/* Update mode line.		*/
#define	WFCOLR	0x40		/* Needs a color change		*/
#define	WFTXTD  0x80		/* Text down one line           */
#define	WFONLY 0x100		/* Not stored */


#define NUMFLAGS  3

#define BFINVS	0x01		/* Internal invisable buffer	*/
#define BFCHG	  0x02		/* Changed since last write	*/
#define	BFNAROW	0x04		/* buffer has been narrowed	*/

				/*	mode flags	*/
#define	NUMMODES  11		/* # of defined modes	       */

#define MDSTT    0x0008
#define	MDVIEW	 0x0008		/* read-only buffer		*/
#define MDWRAP	 0x0010		/* word wrap			*/
#define MDOVER	 0x0020		/* overwrite mode		*/
#define	MDIGCASE 0x0040		/* Exact matching for searches	*/
#define MDMAGIC	 0x0080		/* regular expresions in search */
#define MDSRCHC  0x0100   /* search comments also */
#define	MDMS		 0x0200		/* File to have CRLF		*/
#define	MDCRYPT	 0x0400		/* encrytion mode active	*/
#define	MDASAVE	 0x0800		/* auto-save mode		*/
#define MDDIR    0x1000		/* this file is a directory	*/
#define BFACTIVE 0x2000		/* this buffer is active */
#define BCRYPT2	 0x4000
#define	BFTRUNC	 0x8000		/* buffer was truncated when read */

#define BSOFTTAB 0x01			/* not in use */
//#define BCRYPT2	 0x02

				/* language properties */
#define BCCOMT  0x01		/* c style comments */
#define BCPRL   0x02		/* perl style comments */
#define BCSQL   0x04    /* sql style comments */
#define BCPAS   0x08		/* pascal style comments */
#define BCFOR   0x10		/* fortran style comments */
#define BCML    0x20    /* Markup language */
#define BCSTRNL 0x40    /* NL does not end a string */
#define BCSIMP  0x80		/* simple keyword */

/*
 * Modes of working
 */
					
#define MD_KEEP_MACROS 1	/* keep macro buffers when failing */
#define MD_KEEP_CR     2	/* keep CR in files */
#define MD_NO_MMI      4	/* no man machine interface */

#define G_STERM (CTRL |'M')

/*
 * Incremental search defines.
 */
#if	ISRCH

#define CMDBUFLEN	100	/* Length of our command buffer */

#define IS_ABORT	 ('G'- '@')	/* Abort the isearch */
#define IS_BACKSP	 ('H'- '@')	/* Delete previous char */
#define	IS_TAB		 ('I'- '@')	/* Tab character (allowed search char) */
#define IS_NEWLINE ('M'- '@')	/* New line from keyboard (Carriage return) */
#define	IS_QUOTE	 ('Q'- '@')	/* Quote next character */
#define IS_REVERSE ('R'- '@')	/* Search backward */
#define	IS_FORWARD ('S'- '@')	/* Search forward */
#define	IS_VMSQUOTE ('V'- '@')/* VMS quote character */
#define	IS_VMSFORW ('X'- '@')	/* Search forward for VMS */
#define	IS_QUIT		 0x1B				/* Esc Exit the search */
#define	IS_RUBOUT	 0x7F	      /* Delete previous character */

/* IS_QUIT is no longer used, the variable metac is used instead */

#endif

/* All text is kept in circularly linked lists of "LINE" structures. These
 * begin at the header line (which is the blank line beyond the end of the
 * buffer). This line is pointed to by the "BUFFER". Each line contains a the
 * number of bytes in the line (the "used" size), the size of the text array,
 * and the text. The end of line is not stored as a byte; it's implied.
 */
typedef struct	LINE {
	struct LINE * l_fp;		/* Link to the next line	*/
	struct LINE * l_bp;		/* Link to the previous line	*/
	int 	        l_dcr; 	/* Used(24) spare(6) incomment(1) header(1) */
	char	        l_text[16]; /* A bunch of characters.	*/
}	LINE;

#define l_is_hd(lp) (!(lp->l_dcr))

#define lforw(lp)	((lp)->l_fp)
#define lback(lp)	((lp)->l_bp)
#define lmove(lp,wh) (((LINE**)lp)[wh])

#define lgetc(lp, n)	((lp)->l_text[n]&0xFF)
#define lgets(lp, n)	&(lp)->l_text[n]
#define lputc(lp, n, c) ((lp)->l_text[n]=(c))

#define Lineptr		struct LINE *  

#define SPARE_SZ 6

#define lused(x) ((x) >> SPARE_SZ+2)
#define SPARE_MASK (((1 << SPARE_SZ) - 1) << 2)
#define llength(lp)	((lp)->l_dcr >> SPARE_SZ+2)

typedef struct
{ struct LINE * curline;
  int		        curoff;
  unsigned int  line_no;
} Lpos_t;

typedef struct MARK
{ struct LINE *markp;	/* Line containing "mark"	*/
  int	         marko;	/* Byte offset for "mark"	*/
} MARK;

typedef struct MARKS
{ struct MARK c[NMARKS];	
} MARKS;


 /* A buffer header exists for every buffer in the system.
  * The buffers are in list bheadp, so commands can search for a buffer by name
  * There is store for the dot and mark in the header, but this is only valid if
  * the buffer is not being displayed (that is, if window_ct(bp) is 1).
  * The field b_linep points to a circularly linked list of lines.
  * When Buffers are "Inactive" their files have not been read in yet.
  */
typedef struct	WINDOW
{	struct	LINE *w_dotp;			/* Line containing "."		*/
	int	          w_doto;
	unsigned int  w_line_no;	/* Lpos_t isomorphism ends      */
  struct  MARKS mrks;
	struct	LINE *w_linep;		/* Top line in the window	*/
	struct	WINDOW *w_next; 	/* Next window			*/
	struct	BUFFER *w_bufp; 	/* Buffer displayed in window	*/
	char	w_toprow;		/* Origin 0 top row of window	*/
	char	w_ntrows;		/* # of rows in window inc MLine*/
	char	w_force;		/* If NZ, forcing row. */
	char	w_flag; 		/* Flags.			*/
#if	0
	int   w_color;		/* current colors		*/
#endif
	int		w_fcol; 		/* first column displayed	*/
}	WINDOW;


typedef char * CRYPTKEY;

//typedef struct
//{ CRYPTKEY  		b_key;
//	short   			b_color;		/* current colors		*/
//	short	  			b_flag; 		/* Flags and modes  */
//} BATTRS;

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
{ struct LINE * b_dotp;      /* Link to "." LINE structure  */
	int           b_doto;      /* offset of "."; isomorphism to MARK ends */
	int           b_window_ct; /* valid only after window_ct() */
	struct MARKS  b_mrks;
	struct LINE * b_wlinep ;   /* top LINE in last window */
	struct BUFFER *b_next;     /* next BUFFE R    */   /* isomorphism ends */
#if DO_UNDO
	struct UNDO * b_undo;
#endif
	struct LINE   b_baseline ; /* the header LINE */
	struct LINE * b_narlims[2];/*narrowed top, bottom text */
	signed char   b_tabsize ;  /* size of hard t ab   */
	CRYPTKEY      b_key;
	short         b_color;     /* current colo rs		*/
	short         b_flag;      /* Flags and modes  */
	char          b_langprops; /* type of language of contents */
	short         b_luct;      /* last use cou nt		*/ 
	char *        b_fname;     /* malloc'd and owned by BUFFER */
	char *        b_remote;    /* remote command     */
	Filetime			b_utime;     /* Unix time */
	char          b_bname[6];  /* Buffer name  (or bigger than this) */
}	BUFFER;
				/* for compatibility: */

typedef struct	WUFFER
{	struct	LINE *w_dotp;			/* Line containing "."		*/
	int	          w_doto;
	unsigned int  w_line_no;	/* Lpos_t isomorphism ends      */
  struct  MARKS mrks;
	struct	LINE *w_linep;		/* Top line in the window	*/
//struct	WINDOW *w_next; 	/* Next window			*/
} WUFFER;

/*
 * The starting position of a region, and the size of the region in
 * characters, is kept in a region structure.  Used by the region commands.
 */
typedef struct	{
	struct	LINE *r_linep;		/* Origin LINE address. 	*/
	int		r_offset;		/* Origin LINE offset.		*/
	long	r_size; 		/* Length in characters.	*/
	int		r_lines;		/* number of lines in the buffer*/
	int		r_up;				/* mark is above */
}	REGION;

/* The editor communicates with the display using a high level interface. A
 * "TERM" structure holds useful variables, and indirect pointers to routines
 * that do useful operations. The low level get and put routines are here too.
 * This lets a terminal, in addition to having non standard commands, have
 * funny get and put character code too. The calls might get changed to
 * "termp->t_field" style in the future, to make it possible to run more than
 * one terminal type.
 */
typedef struct	{
	short	t_nrowm1; 		/* current number of rows used	*/
	short	t_ncol; 		/* current Number of columns.	*/
#if MEMMAP == 0
	short	t_margin;		/* min margin for extended lines*/
	short	t_scrsiz;		/* size of scroll region "	*/
#endif
}	TERM;

typedef int Emacs_cmd(int, int);

                        /* Structure for the table of current key bindings */
union EPOINTER {
        Emacs_cmd * fp;      				/* C routine to invoke */
        BUFFER *    buf;            /* buffer to execute */
};
                        /* Structure for the table of predefined variables */
typedef union PD_VAR {
        int    i;      							/* C routine to invoke */
        char * p;                 	/* buffer to execute */
} PD_VAR;
												/* Structure for the key binding table */
typedef struct
{ short k_code;	        /* Key code, 0 => end of table */
//short k_type;	        /* binding type (C function or buffer) */
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

#define MLIX 3

typedef struct LL
{	int  ll_ix;						// = MLIX;
	char lastline[MLIX+1][NSTRING];
//char pat[NPAT+2];
} LL;

typedef struct Paren_s
{ 
  char				  in_mode;
  signed char   complex;
  char          prev;
  signed char   nest;
  signed char   nestclamped;
  char          ch;
  signed char		lang;
  short					sdir;
  short         fence;
} Paren_t, *Paren;

extern Paren_t g_paren;

typedef struct Fdcr_s
{ void * ip;
  char   name[NFILEN+1];
} Fdcr_t, *Fdcr;


typedef struct UNDO 
{
	struct UNDO * u_bp;		/* Link to the previous UNDO	*/
	struct LINE * u_lp;		/* Link to LINE */
	struct LINE * u_llost;/* Link to lost lines */
	int						u_size;	/* size of text */
	int						u_offs;
	int 	        u_dcr; 	/* Used(24) spare(6) incomment(1) header(1) */
	char	        u_text[16]; /* A bunch of characters.	*/
}	UNDO;

#include "epredef.h"

					   /*  Internal defined functions */
#define nextab(a)	((a - (a % tabsize)) + tabsize)
#define nextabp(a,sz)	((a - (a % sz)) + sz)
#define chcaseunsafe(ch)   (ch ^ 0x20)

/* from MAIN.C */

#define OPT_M    1
#define OPT_P    8
#define OPT_R   16
#define OPT_S   32
#define OPT_V  256
#define OPT_X 1024
#define OPT_Z 4096

#define Q_INHERIT 0x8
#define Q_POPUP   1

extern int g_opts;

#define val_opt(ch) ((g_opts >> ('Z' - ch)) & 1)
#define is_opt(ch)  (g_opts & (1 << ('Z' - ch)))
#define clr_opt(ch) g_opts ^= (g_opts & (1 << ('Z' - ch)))

extern int kinsert_n;
extern int g_got_uarg;
extern int g_got_search;


/* initialized global external declarations */
//NOSHARE extern BATTRS g_bat;		/* backgrnd (black*16) + foreground (white) */
NOSHARE extern char * g_bat_b_key;
NOSHARE extern short  g_bat_b_color;

NOSHARE extern int g_numcmd;			/* number of bindable functions */
NOSHARE extern int g_nosharebuffs;/* disallow different files in same buffer*/

NOSHARE extern int   g_macargs;		/* take values from command arguments */
//NOSHARE extern CRYPTKEY g_ekey;		/* global encryption key	*/
NOSHARE extern char golabel[];		/* current line to go to	*/
extern const char mdname[NUMMODES][8];		/* text names of modes		*/
extern const NBIND names[];				/* name to function table	*/

#ifdef _MSC_VER

#include <windows.h>

#else
typedef struct 
{   short X;
    short Y;
} COORD;

#endif

extern COORD  g_coords;						/* location of HW cursor */
#define ttrow g_coords.Y		  		
#define ttcol g_coords.X
#if MOUSE
NOSHARE extern int g_lbound;		/* leftmost col of current line being displayed*/
#endif
NOSHARE extern int g_abortc;			/* current abort command char	*/

//NOSHARE extern char highlight[64];	/* the highlight string */

NOSHARE extern int cryptflag;		/* currently encrypting?	*/
//NOSHARE extern int g_newest;  /* choose newest file           */
NOSHARE extern Int envram;			/* # of bytes current in use by malloc */

const extern char g_logm[3][8];

//NOSHARE extern char palstr[49];		/* palette string		*/
NOSHARE extern char lastmesg[NCOL+140];	/* last message posted		*/
//NOSHARE extern int (Pascal *g_lastfnc)(int, int);/* last function executed */
NOSHARE extern int  g_eexitflag;		/* EMACS exit flag */

/* uninitialized global external declarations */

NOSHARE extern int   g_thisflag;	/* Flags, this command		*/
NOSHARE extern int   g_lastflag;	/* Flags, last command		*/
NOSHARE extern Short g_clring;
NOSHARE extern Bool  g_inhibit_undo;


NOSHARE extern WINDOW *curwp; 		/* Current window		*/
NOSHARE extern BUFFER *curbp; 		/* Current buffer		*/
NOSHARE extern void * g_heads[2];

#define wheadp ((WINDOW *)g_heads[0])		/* Head of list of windows	*/
#define bheadp ((BUFFER*)g_heads[1])		/* Head of list of buffers	*/

#define g_wheadp_ref &g_heads[0]
#define g_bheadp_ref &g_heads[1]

NOSHARE extern BUFFER *blistp;		/* Buffer for C-X C-B		*/
				extern LL	g_ll;

#if	DIACRIT
NOSHARE	extern char lowcase[HICHAR];	/* lower casing map		*/
NOSHARE	extern char upcase[HICHAR];	/* upper casing map		*/
#endif

	      extern char pat[NPAT+2];	/* Search pattern		*/
NOSHARE extern char rpat[NPAT+2];	/* replacement pattern		*/

//			extern char *g_file_prof;	/* profiles of files */

/*	Various "Hook" execution variables	*/
NOSHARE extern KEYTAB hooks[6];		/* executed on all file reads */

//NOSHARE extern char *patmatch;

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

#define Q_LOG_CHAR 3

#define Q_IN_CMT0  1
#define Q_IN_CMT   2
#define Q_IN_ESC   4
#define Q_IN_CHAR  8
#define Q_IN_STR  16
#define Q_IN_CMTL 32
#define Q_IN_CMT_ 64
#define Q_IN_EOL 128
#define Q_IS_NEG 256

#define QUOTEC ('Q' - '@')

#define Q_LOOKP 1
#define Q_LOOKI -1
#define Q_LOOKH 0

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

