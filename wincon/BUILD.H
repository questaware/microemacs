#ifndef BUILDINC
#define BUILDINC
			/*  ENVIRONMENT SELECTION SECTION */

/*	Program Identification.....
	Macros can query this via the $progname variable
*/

#define PROGNAME	"Edm"
#define	VERSION		"2.8"


/*	Machine/OS definitions			*/
/*	[Set one of these!!]			*/

#define S_AMIGA	0			/* AmigaDOS			*/
#define S_MSDOS	1			/* MS-DOS			*/
#define S_OS2	0			/* Microsoft or IBM OS/2	*/
#define S_UNIX5	0			/* UNIX system V		*/
#define	S_SUN	0			/* SUN v4.0			*/
#define	S_HPUX	0			/* HPUX HP 9000 minicomputer	*/
#define S_VMS	0			/* VAX/VMS			*/
#define S_FINDER 0			/* Macintosh OS 		*/
#define S_WIN32 1
#define S_BORLAND 0

#define NONDIR  	0                     /* ndir.h is dirent.h           */
#define USE_UBAR  0
#define GOTTYPAH  1
#define HIDECRYPT 1

/*	Compiler definitions			*/
/*	[Set one of these!!]			*/
#define UNIX	0	/* a random UNIX compiler */
#define MSC	1	/* MicroSoft C compile version 3 & 4 & 5.1 */

/*#ifndef __STDC__
#define __STDC__ 1
#endif*/

/*      Machine stack growth direction.                     */
/*  [Set this to 1 if your machine stack grows UP!!!]       */
/*      data general mv/eclipse series stack grows up.      */
/*      dec vax series stack grows down... got it???        */

#define STACK_GROWS_UP  0

/*	Debugging options	*/
#define	RAMSIZE	0	/* dynamic RAM memory usage tracking */

/*	Terminal Output definitions		*/
/*	[Set one of these!!]			*/

#define ANSI	0			/* ANSI escape sequences	*/
#define VT52	0			/* VT52 terminal (Zenith).	*/
#define TERMCAP 0			/* Use TERMCAP			*/
#define	IBMPC	1			/* IBM-PC CGA/MONO/EGA/VGA drvr	*/

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
#define DEBUGM	0	/* $debug triggers macro debugging		*/
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
#define	BACKCH  0	/* allow debugging file				*/
#define	MEOPT_SINC 1    /* include search-incls				*/
#define	MEOPT_TAGS 1    /* include find-tag				*/
//#define	MEOPT_IPIPES 1


#define NONDIR 0                        /* use ndir.h not dirent.h      */

		/* wrap malloced memory */
#define WRAP_MEM  0

#define LOGGING_ON 0

#define OWN_MODECOLOUR 0
#define OWN_BOLD 0
#define OWN_BOLD_VAL "\033[42;35m"

#define LONG_ALGNMT 2		/* smallest alignment for longs, ptrs */
#define SIZEOF_LONG 4           /* size of long */
#define SIZEOF_PTR  4           /* size of int * */

#define LITTLEENDIAN 1

#define CHARCODE ASCII

				/* size for general strings */
#define NSTRING 1026
#define FILENAMESZ 256

typedef int   Vint;       /* set this to short int if it is more efficient */

#define USE_MAPSTREAM 0
#define MINIMAP

#include "../src/base.h"
#endif /* BUILDINC */
