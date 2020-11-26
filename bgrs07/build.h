#ifndef BUILDINC
#define BUILDINC

/*	Machine/OS definitions			*/
/*	[Set one of these!!]			*/

#define S_AMIGA	0			/* AmigaDOS			*/
#define S_MSDOS	0			/* MS-DOS			*/
#define S_OS2	0			/* Microsoft or IBM OS/2	*/
#define S_BSD	0			/* UNIX BSD 4.2 and ULTRIX	*/
#define S_UNIX5	1			/* UNIX system V		*/
#define	S_XENIX	0			/* IBM-PC SCO XENIX		*/
#define	S_HPUX	0			/* HPUX HP 9000 minicomputer	*/
#define S_VMS	0			/* VAX/VMS			*/
#define S_FINDER 0			/* Macintosh OS 		*/
#define S_LINUX 0
#if S_LINUX || S_HPUX
 #undef  S_UNIX5
 #define S_UNIX5 1
#endif 
#define S_WIN32 0

#define S_CC_C89AIX 1

/*      [Set any of these ] */
#define S_XTERM   0
#define S_SOLARIS 0

#define NONDIR  0                       /* ndir.h is dirent.h           */
#define USE_UBAR 0

/*	Compiler definitions			*/
/*	[Set one of these!!]			*/
#define UNIX	1	/* a random UNIX compiler */
#define MWC	0	/* Mark Williams C */
#define	ALCYON	0	/* ALCYON Atari ST compiler */ 
#define LATTICE 0	/* Lattice 2.14 through 3.0 compilers */
#define AZTEC	0	/* Aztec C 3.20e */
#define MSC	0	/* MicroSoft C compile version 3 & 4 & 5.1 */
#define XMSC    0       /* Xenix MicroSoft C */
#define TURBO	0	/* Turbo C/MSDOS */
#define DTL	0	/* DataLight C v3.12 */
#define	DGC	0	/* Data General AOS/VS C... */

#if 0
#ifndef __STDC__
#define __STDC__ 1
#endif
#endif

/*      Machine stack growth direction.                     */
/*  [Set this to 1 if your machine stack grows UP!!!]       */
/*      data general mv/eclipse series stack grows up.      */
/*      dec vax series stack grows down... got it???        */

#define STACK_GROWS_UP  0

/*	Debugging options	*/
#define	RAMSIZE	1	/* dynamic RAM memory usage tracking */

/*	Terminal Output definitions		*/
/*	[Set one of these!!]			*/

#define ANSI	0			/* ANSI escape sequences	*/
#define	HP150	0			/* HP150 screen driver		*/
#define	HP110	0			/* HP110 screen driver		*/
#define	VMSVT	0			/* various VMS terminal entries	*/
#define VT52	0			/* VT52 terminal (Zenith).	*/
#define RAINBOW 0			/* Use Rainbow fast video.	*/
#define TERMCAP 1			/* Use TERMCAP			*/
#define	IBMPC	0			/* IBM-PC CGA/MONO/EGA/VGA drvr	*/
#define OS2NPM	0			/* OS/2 non-Presentation Mgr.	*/
#define	DG10	0			/* Data General system/10	*/
#define	TIPC	0			/* TI Profesional PC driver	*/
#define	Z309	0			/* Zenith 100 PC family	driver	*/
#define	MAC	0			/* Macintosh			*/
#define	ATARI	0			/* Atari 520/1040ST screen	*/
#define	DASHER	0			/* DG Dasher 2xx/4xx crts	*/

#define REVERSE_VIDEO 1

/*	Language text options	(pick one)				*/

#define ENGLISH 1
#define	FRENCH	0
#define	SPANISH	0
#define	GERMAN	0
#define	DUTCH	0
#define PLATIN	0

/*	Configuration options	*/

#define CVMVAS	1	/* arguments to page forward/back in pages	*/
#define	CLRMSG	0	/* space clears the message line with no insert	*/
#define	CFENCE	1	/* fench matching in CMODE			*/
#define	TYPEAH	1	/* type ahead causes update to be skipped	*/
#define DEBUGM	1	/* $debug triggers macro debugging		*/
#define	LOGFLG	0	/* send all executed commands to EMACS.LOG	*/
#define	VISMAC	0	/* update display during keyboard macros	*/
#define	CTRLZ	0	/* add a ^Z at end of files under MSDOS only	*/
#define	NBRACE	1	/* new style brace matching command		*/
#define	COMPLET	1	/* new completion code (as of 3.10)		*/
#define	CLEAN	0	/* de-alloc memory on exit			*/
#define	CALLED	0	/* is emacs a called subroutine? or stand alone */
#define ADDCR	0	/* ajout d'un CR en fin de chaque ligne (ST520) */
			/* [= add a CR at the end of each line (ST520)] */

#define REVSTA	1	/* Status line appears in reverse video 	*/
#define	COLOR	1	/* color commands and windows			*/

#define FILOCK	0	/* file locking under unix BSD 4.2		*/
#define	ISRCH	1	/* Incremental searches like ITS EMACS		*/
#define	WORDPRO	1	/* Advanced word processing features		*/
#define	FLABEL	0	/* function key label code [HP150]		*/
#define	APROP	1	/* Add code for Apropos command			*/
#define	CRYPT	1	/* file encryption enabled?			*/
#define MAGIC	1	/* include regular expression matching?		*/
#define	AEDIT	1	/* advanced editing options: en/detabbing	*/
#define	NMDPROC	1	/* named procedures				*/
#define MOUSE	0	/* Include routines for mouse actions		*/
#define	NOISY	1	/* Use a fancy BELL if it exists		*/
#define	DIACRIT	0	/* diacritical marks processed?			*/
#define	BACKCH  1	/* allow debugging file 			*/
#define	MEOPT_SINC 1    /* include search-incls				*/
#define MEOPT_TAGS 1    /* include find-tag                             */

/*	Character set options		*/
/*	[Set one of these!!]		*/
#define ASCII	1	/* always using ASCII char sequences for now	*/
#define EBCDIC	0	/* later IBM mainfraim versions will use EBCDIC	*/

				/* wrap malloced memory */
#define WRAP_MEM  0

#define COLOR 1
#define OWN_MODECOLOUR 1
#define OWN_BOLD 0
#define OWN_BOLD_VAL "\033[42;35m"
#define USE_SCR_BDY 0



#define LONG_ALGNMT 4		/* smallest alignment for longs */
#define SIZEOF_LONG 4           /* size of long */
#define SIZEOF_PTR  4           /* size of int * */

#define CHARCODE ASCII

				/* size for general strings */
#define NSTRING 1026

typedef int   Vint;       /* set this to short int if it is more efficient */

#define REVISION "1.1"

#define USE_MAPSTREAM 0
#define MINIMAP

#define SCR_LINES      25
#define SCR_LINES_STR "25"
#define TELNET 0
#define TELKEYS 2
#define TELNETMAXCHARS 750

#ifndef NFILEN
#define NFILEN 256
#endif

#include "../src/base.h"
#endif /* VERSION */
