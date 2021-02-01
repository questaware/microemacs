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

#if	S_VMS
/*#define	const	readonly*/
#define	VOID	void
#define NOSHARE
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

#ifdef __STDC__	/* if ANSI C compatible */
#define __(x) x
#define	ETYPE union
#else
#define __(x) ()
#define	ETYPE union
#endif

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
/*# define	unlink(a)	delete(a)*/
#endif

/* some options for AOS/VS */
#if     AOSVS
# define ORMDNI  1
#endif

#if XMSC
#define rename(a,b) rename(b,a)
#endif
#if S_MSDOS || S_VMS
#define unix_rename(a,b) rename(a,b)
#endif

#include <string.h>
#include <stdlib.h>

				/* define some ability flags */

#if	S_MSDOS | S_OS2 | S_UNIX5 | S_HPUX | S_BSD | S_XENIX | WMCS | S_VMS
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
#define	KBLOCK	250			/* sizeof kill buffer chunks	*/
#define	NBLOCK	16			/* line block chunk size	*/
#define	NVSIZE	10			/* max #chars in a var name	*/
#define NMARKS	 2			/* number of marks must be po2  */


#if	S_UNIX5 | S_HPUX | S_BSD | S_XENIX
# define PATHCHR ':'
#elif	WMCS
# define PATHCHR ','
#else
# define PATHCHR ';'
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

/*	TEMPORARY macros for terminal I/O  (to be placed in a machine
					    dependant place later) */

#define TTopen(x)	ttopen(x)
#define	TTclose(x)      tcapclose(x)
#define	TTgetc		(*term.t_getchar)
#define	TTputc		ttputc
#define	TTflush()	ttflush()
#define	TTeeol		tcapeeol
#define	TTeeop		tcapepage
#define	TTbeep		tcapbeep
#define	TTrev(x)	tcaprev(x)
//#define TTrez		tcapcres


typedef int (Pascal *Pfunc)();	/* function name is bound to */
			/* Structure for the name binding table */

/* HICHAR - 1 is the largest character we will deal with.
 * HIBYTE represents the number of bytes in the bitmap.
 */
#define HICHAR	256

