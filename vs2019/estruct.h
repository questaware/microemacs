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

/*	Character set enums  */
#define ASCII	1	/* always using ASCII char sequences for now	*/
#define EBCDIC	0	/* later IBM mainfraim versions will use EBCDIC	*/

#include "build.h"
#include <time.h>

				/* handle constant and voids properly */
#define NOSHARE
#if	S_VMS
#define	const	readonly
#define	VOID	void
#undef  NOSHARE
#define NOSHARE noshare

#elif	defined(__STDC__)
#define	VOID	void

#elif S_WIN32 == 0 || S_BORLAND
#define	VOID
#endif

/*	System dependant library redefinitions, structures and includes */

#undef null

#ifdef __STDC__	/* if ANSI C compatible */
#define __(x) x
#define null (void *)0
#else
#define __(x) ()
#define null 0L
#endif

#if S_WIN32 && S_BORLAND == 0

#pragma warning(disable : 4996)
				/* NEAR: dodgy but convenient */
//#define NEAR
//#define DNEAR
#define Pascal
/*#define PASCAL __stdcall */
#define	Cdecl

#elif S_MSDOS & (TURBO | MSC)
#define	NEAR
#define	DNEAR
#if S_WIN32
#define	Pascal
#else
#define	Pascal pascal
#endif
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

#if	LATTICE & S_MSDOS
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

#if XMSC
#define rename(a,b) rename(b,a)
#endif
#if S_MSDOS
#define unix_rename(a,b) rename(a,b)
#endif

#include <string.h>

				/* define some ability flags */
#if	S_MSDOS | S_OS2 | S_UNIX5 | S_HPUX | S_BSD | S_XENIX | WMCS
# define ENVFUNC 1
#else
# define ENVFUNC 0
#endif

#if	S_MSDOS || S_OS2
# define DIRSEPSTR	"\\"
# define DIRSEPCHAR	'\\'
# define PATHCHR	';'
#else
# define DIRSEPSTR	"/"
# define DIRSEPCHAR	'/'
# define PATHCHR	':'
#endif

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

#elif	(UNIX == 0) && (MSC == 0)
#if	CHARCODE == ASCII
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
# define CHCASE(c)  chcase(c)	      /* Toggle extended letter case.*/
#else
# define CHCASE(c)  ((c) ^ DIFCASE)   /* Toggle the case of a letter.*/
#endif

/*	Dynamic RAM tracking and reporting redefinitions	*/

#if WRAP_MEM
#define malloc	allocate
#define strdup  duplicate
#define free	release
#else
//#define aalloc(x) calloc(((x)+SIZEOF_PTR-1)/SIZEOF_PTR, SIZEOF_PTR)
#endif

#if MSC
/*extern char * memcpy();*/
/*#define memcpy(t,s,l) native_cp(t,s,l);*/
/*#define memmove(t,s,l) memcpy(t,s,l); */
#endif

/*	TEMPORARY macros for terminal I/O  (to be placed in a machine
					    dependant place later) */

/*#define TTopen	ttopen*/
#define	TTclose		tcapclose
#define tcapkopen()
#define	TTgetc		(*term.t_getchar)
#define	TTputc		ttputc
#define	TTflush()		
#define	TTsetcol(col)
#define	TTeeol		tcapeeol
#define	TTeeop		tcapepage
#define	tcaprev(x)	/* not used in MSDOS */
//#define TTrez		tcapcres


/* HICHAR - 1 is the largest character we will deal with.
 * HIBYTE represents the number of bytes in the bitmap.
 */
#define HICHAR	256

#if S_WIN32
#define itoa _itoa
#define unlink _unlink
#define fdopen _fdopen
#define dup _dup
#define open _open
#define getcwd _getcwd
#define write _write
#define chdir _chdir
#define chmod _chmod

#endif
