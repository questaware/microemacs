/*	ESTRUCT:	Structure and preprocesser defined for
			MicroEMACS 3.10

			written by Daniel Lawrence
			based on code by Dave G. Conroy,
				Steve Wilhite and George Jones
*/
#define IN_ESTRUCT 1

#ifdef	LATTICE
#undef	LATTICE 	 /* don't use their definitions...use ours	 */
#endif
#ifdef	S_MSDOS
#undef	S_MSDOS
#endif
#ifdef	S_AMIGA
#undef	S_AMIGA
#endif
#ifdef	EGA
#undef	EGA
#endif
#ifdef	CTRLZ
#undef	CTRLZ
#endif

/*	Program Identification.....

	PROGNAME should always be MicroEMACS for a distribution
	unmodified version. People using MicroEMACS as a shell
	for other products should change this to reflect their
	product. Macros can query this via the $progname variable
*/

#define PROGNAME	"Edm"
#define	VERSION		"2.7"


#include "build.h"
/*
#if S_UNIX5
#include <unistd.h>
#endif
*/
#if	S_VMS
#define	const	readonly
#define	VOID	void
#define NOSHARE noshare
#else
#ifdef	__STDC__
#define	VOID	void
#define	NOSHARE
#else
#define	VOID
#define NOSHARE
#endif
#endif

/*	System dependant library redefinitions, structures and includes */

//#ifdef __STDC__	/* if ANSI C compatible */
//#define null (void *)0
//#else
//#define null 0L
//#endif

#if S_MSDOS & (TURBO | MSC)
#define	NEAR
#define	DNEAR
#define	Pascal pascal
#define	Cdecl cdecl
#else
#define NEAR
#define	DNEAR
#define	Pascal
#define	Cdecl
#endif

#if	TURBO
#include      <dos.h>
#include      <mem.h>
#undef peek
#undef poke
#define       peek(a,b,c,d)   movedata(a,b,FP_SEG(c),FP_OFF(c),d)
#define       poke(a,b,c,d)   movedata(FP_SEG(c),FP_OFF(c),a,b,d)
#endif

#if	S_MSDOS && LATTICE
/* you may have to remove this one definition with LATTICE version
   3.2 and above						  */
#define	unsigned
#endif

#if	AZTEC
#undef	putc
#undef	getc
#if	S_MSDOS
#define	getc	a1getc
#define	int86	sysint
#define	intdos(a, b)	sysint(33, a, b)
#define	inp	inportb
#define	outp	outportb
#else
#define getc	agetc
#endif
#define putc	aputc

struct XREG {
	unsigned ax,bx,cx,dx,si,di,ds,es;
};

struct HREG {
	char al,ah,bl,bh,cl,ch,dl,dh,d1,d2,e1,e2;
};

union REGS {
	struct XREG x;
	struct HREG h;
};

struct SREGS {
	unsigned cs, ss, ds, es;
};
#endif

#if	S_MSDOS & DTL
# include	<dos.h>
#endif

#if	S_MSDOS & MWC
# include	<dos.h>
# define	int86(a, b, c)	intcall(b, c, a)
# define	intdos(a, b)	intcall(a, b, DOSINT)
# define	inp(a)		in(a)
# define	outp(a, b)	out(a, b)

struct XREG {
	unsigned int ax,bx,cx,dx,si,di,ds,es,flags;
};

struct HREG {
	char al,ah,bl,bh,cl,ch,dl,dh;
	unsigned int ds,es,flags;
};

union REGS {
	struct XREG x;
	struct HREG h;
};
#endif

#if	S_MSDOS & MSC
# include	<dos.h>
# include	<memory.h>
# define	peek(a,b,c,d)	movedata(a,b,FP_SEG(c),FP_OFF(c),d)
# define	poke(a,b,c,d)	movedata(FP_SEG(c),FP_OFF(c),a,b,d)
#if USE_UBAR
#define REGS _REGS
#define SREGS _SREGS
#endif
#endif

#if	S_MSDOS & LATTICE
# undef	CPM
# undef	LATTICE
# include	<dos.h>
# undef	CPM
#endif

/* this keeps VMS happy */
#if	S_VMS
# define	getname xgetname
# define	unlink(a)	delete(a)
#endif

/* some options for AOS/VS */
#if     AOSVS
# define ORMDNI  1
#endif

#include <string.h>

				/* define some ability flags */

#if	S_MSDOS | S_OS2 | S_UNIX5 | S_HPUX | S_BSD | S_XENIX | WMCS
# define ENVFUNC 1
#else
# define ENVFUNC 0
#endif

#if	ATARI || S_MSDOS || S_OS2
# define DIRSEPSTR	"\\"
# define DIRSEPCHAR	'\\'
#else
# define DIRSEPSTR	"/"
# define DIRSEPCHAR	'/'
#endif

			/*  Emacs global flag bit definitions (for gflags) */
#define GFREAD	1
					/*	internal constants	*/
#define NBINDS	210			/* max # of bound keys		*/
#if AOSVS
# define NFILEN 80			/* # of bytes, file name	*/
#else
# define NFILEN 256
#endif
#define NBUFN	32			/* # of bytes, buffer name	*/
#define NLINE	160			/* # of bytes, input line	*/
#define	NSTRING	1026			/* # of bytes, string buffers	*/
#define NKBDM	256			/* # of strokes, keyboard macro */
#define NPAT	128			/* # of bytes, pattern		*/
#define HUGE	1000			/* Huge number			*/
#define	NLOCKS	100			/* max # of file locks active	*/
#define	NCOLORS	16			/* number of supported colors	*/
#define	KBLOCK	250			/* sizeof kill buffer chunks	*/
#define	NBLOCK	16			/* line block chunk size	*/
#define	NVSIZE	10			/* max #chars in a var name	*/
#define NMARKS	 2			/* number of marks must be po2  */
#define MAXVARS 64

#if	S_UNIX5 | S_HPUX | S_BSD | S_XENIX
# define PATHCHR ':'
#else
#if	WMCS
# define PATHCHR ','
#else
# define PATHCHR ';'
#endif
#endif


#if UNIX || MSC
# include  <ctype.h>
# define isletter(c) isalpha(c)
#endif

#if  DIACRIT
#ifdef isletter
# undef isletter
#endif
#ifdef	islower
# undef	islower
#endif
#ifdef	isupper
# undef	isupper
#endif
#endif

#if	DIACRIT == 0 && (UNIX == 0) && (MSC == 0)
#if	ASCII
# define isletter(c)	(('a' <= c && 'z' >= c) || ('A' <= c && 'Z' >= c))
# define islower(c)	(('a' <= c && 'z' >= c))
# define isupper(c)	(('A' <= c && 'Z' >= c))
#endif

#if	EBCDIC
# define isletter(c)	(('a' <= c && 'i' >= c) || ('j' <= c && 'r' >= c) || ('s' <= c && 'z' >= c) || ('A' <= c && 'I' >= c) || ('J' <= c && 'R' >= c) || ('S' <= c && 'Z' >= c))
# define islower(c)	(('a' <= c && 'i' >= c) || ('j' <= c && 'r' >= c) || ('s' <= c && 'z' >= c))
# define isupper(c)	(('A' <= c && 'I' >= c) || ('J' <= c && 'R' >= c) || ('S' <= c && 'Z' >= c))
#endif

#define toupper(c) upperc(c)
#define tolower(c) lowerc(c)
#endif

#if	DIACRIT
# define	CHCASE(c)  chcase(c)	     /* Toggle extended letter case.*/
#else
# define CHCASE(c)  ((c) ^ DIFCASE)   /* Toggle the case of a letter.*/
#endif

/*	Dynamic RAM tracking and reporting redefinitions	*/

#if WRAP_MEM
#define calloc	callocate
#define malloc	allocate
#define strdup  duplicate
#define free	release
#define aalloc(x) callocate(x)
#else
#define aalloc(x) calloc(((x)+SIZEOF_PTR-1)/SIZEOF_PTR, SIZEOF_PTR)
#endif


#if MSC
/*extern char * memcpy();*/
/*#define memcpy(t,s,l) native_cp(t,s,l);*/
/*#define memmove(t,s,l) memcpy(t,s,l); */
#endif

typedef struct
{ struct LINE	* curline;
  short           curoff;
  unsigned int    line_no;
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
typedef struct	WINDOW {
	struct	WINDOW *w_wndp; 	/* Next window			*/
	struct	BUFFER *w_bufp; 	/* Buffer displayed in window	*/
	struct	LINE *w_linep;		/* Top line in the window	*/
        struct  MARKS mrks;
	struct	LINE *w_dotp;		/* Line containing "."		*/
	short	w_doto; 		/* offset for "."; isomorphism ends */
	unsigned int w_line_no;		/* Lpos_t isomorphism ends      */
	char	w_toprow;		/* Origin 0 top row of window	*/
	char	w_ntrows;		/* # of rows in window inc MLine*/
	char	w_force;		/* If NZ, forcing row.		*/
	char	w_flag; 		/* Flags.			*/
#if	COLOR
	int     w_color;		/* current colors		*/
#endif
	int	w_fcol; 		/* first column displayed	*/
}	WINDOW;


/* Text is kept in buffers. A buffer header, described below, exists for every
 * buffer in the system. The buffers are kept in a big list, so that commands
 * that search for a buffer by name can find the buffer header. There is a
 * safe store for the dot and mark in the header, but this is only valid if
 * the buffer is not being displayed (that is, if "b_nwnd" is 0). The text for
 * the buffer is kept in a circularly linked list of lines, with a pointer to
 * the header line in "b_linep"	Buffers may be "Inactive" which means the files associated with them
 * have not been read in yet. These get read in at "use buffer" time.
 */
typedef struct	BUFFER {
	struct	BUFFER *b_bufp; 	/* Link to next BUFFER		*/
	struct	LINE *b_baseline;	/* Link to the header LINE	*/
	struct	LINE *b_wlinep;		/* Link top LINE in last window */
        struct  MARKS mrks;
	struct	LINE *b_dotp;		/* Link to "." LINE structure	*/
	short	b_doto; 		/* offset of "."; isomorphism ends */
	short   b_luct;			/* last use count		*/ 
	struct	LINE *b_narlims[2];	/* Link to narrowed top, bottom text */
	char    b_langprops;		/* type of language of contents */
 signed char	b_nwnd; 		/* Count of windows on buffer	*/
	short	b_flag; 		/* Flags and modes 		*/
	short   b_tabsize;		/* size of hard tab		*/
#if	CRYPT
	char *  b_key;	                /* current encrypted key        */
#endif
	char *  b_fname;		/* malloc'd and owned by BUFFER */
	char *	b_remote;		/* remote command 		*/
	char	b_bname[1]; 		/* Buffer name			*/
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
	Int	r_size; 		/* Length in characters.	*/
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
#if SIZEOF_PTR <= 4 || 1
	struct LINE * l_fp;		/* Link to the next line	*/
	struct LINE * l_bp;		/* Link to the previous line	*/
#else
	Int     l_fp;
	Int     l_bp;
#endif
	short	l_used; 		/* Used size			*/
  unsigned char l_spare; 		/* spare space */
	char    l_props;
	char	l_text[1];		/* A bunch of characters.	*/
}	LINE;

#define L_IS_HD 1

#if SIZEOF_PTR <= 4 || 1
#define lforw(lp)	((lp)->l_fp)
#define lback(lp)	((lp)->l_bp)
#define Lineptr		struct LINE *          
#else
#define lforw(lp)	((struct LINE *)((lp)->l_fp+0x100000000L))
#define lback(lp)	((struct LINE *)((lp)->l_bp+0x100000000L))
#define Lineptr		Int          
#endif

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
	int (Pascal *t_getchar)(); /* Get character from keyboard. */
}	TERM;

/*	TEMPORARY macros for terminal I/O  (to be placed in a machine
					    dependant place later) */

#define TTopen		ttopen
#define	TTclose		tcapclose
#define	tcapkclose()
#define	TTgetc		(*term.t_getchar)
#define	TTputc		ttputc
#define	TTflush()	ttflush()
#define	TTeeol		tcapeeol
#define	TTeeop		tcapeeop
#define	TTbeep		tcapbeep
#define	TTrev(x)	tcaprev(x)
//#define TTrez		tcapcres

/* HICHAR - 1 is the largest character we will deal with.
 * HIBYTE represents the number of bytes in the bitmap.
 */
#define HICHAR	256
