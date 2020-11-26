#ifndef ASLEEP

#define false 0
#define true 1

#define ASLEEP
#define forward

#define when break; case
#define otherwise break; default:

#define fast register

#define not !
#define and &&
#define or ||

#define in_range(a, b, c) ((unsigned)((a)-(b))<=((c)-(b)) and ((c)-(b)) >= 0)
#define maxv(a,b) (a > b ? a : b)
#define minv(a,b) (a < b ? a : b)

#define upper_index(loc) ((int)(sizeof(loc)/sizeof(loc[0])-1))
#define fieldoffs(mode, fld) ((Int)&((mode)0)->fld)

typedef struct
{ char ch;
  union
  { int    xint;
    long   xlong;
    double xdouble;
  } modes;
} * T1sct;
					/* smallest alignment for longs */
#define field_alignment(m) (fieldoffs(T1sct, modes.x##m) & (sizeof(m)-1))

#define one_bit(v) ((((v)^((v)-1)) >> 1)+1)
#define null_sct(s) (*(s *)&nulls[0])

#ifndef null
#define null 0L
#endif
#define HALTED 2
#define NOT_FOUND 1
#define OK 0
#define ENOTTHERE (-1)
#define EMISSING  (-2)
#define EEXISTS   (-3)
#define ESTATE    (-4)  /* incorrect state for operation */
#define ETOOBIG   (-5)
#define ETOOMANY  (-6)
#define ENOSPACE  (-7)
#define EDENIED   (-8)
#define EINVALID  (-9)
#define EOORANGE (-10)
#define EPRECOND (-11)
#define EINVTYPE (-12)
#define EINUSE   (-13)
#define EALARM   (-14)
#define EPROB    (-15) /* A problem occurred */
#define ESYNTAX  (-16)
#define EBIGNAME (-17)
#define ECONCHK  (-18) /* Things happened concurrently which shouldn't have */
#define EERROR   (-19)
#define EKILLED  (-20)

#define EAPPLN   (-50)

#define ECRASH  (-80)
		/* The programmer's logic indicated something impossible
		   but it occured; reasons: (s)he was wrong, 
		   			    Arrays, dereferences written oob.
		 */
#define ELOGIC  (ECRASH-1)

#if SIZEOF_LONG > 4
typedef int Int;
typedef unsigned int Bitset;
typedef unsigned int Set32;
#else
typedef long Int;
typedef unsigned long Bitset;
typedef unsigned long Set32;
#endif

typedef unsigned int Nat2;
typedef unsigned long Nat4;
typedef int Int2;
typedef long Int4;

typedef short Cc;			/* Condition Code */
typedef short Short; 			/* up to 16 bit signed integer */
/*typedef int Vint;			** efficient integer */
typedef unsigned char Byte;
typedef short Bool;			/* Boolean: true, false */
typedef char Char;			/* Character, signed or unsigned */
typedef unsigned short Ushort;		
typedef unsigned short Set16;
typedef unsigned char  Set8;
typedef unsigned long  Quot;		/* 'Quotation', C enum values */
typedef unsigned short Squot;
typedef unsigned short Shortset;
typedef long  Id;
typedef short Sid;
typedef long  Date;
typedef long  Time;
typedef int   Fildes;			/* File descriptor, ANSII: stream */


			/*  CONDITIONAL FEATURES SECTION */

#if CHARCODE == ASCII

#define A_BEL   7
#define A_BS    8
#define A_HT    9
#define A_LF    10
#define A_FF    12
#define A_CR	13
#define	A_SP	32

#endif

#if SIZEOF_LONG == 4
# define MAXINT 0x7fffffff
# define MININT 0x80000000
#else
# define MAXINT 0x7fff
# define MININT 0x8000
#endif
#define NaN MININT


#ifndef __
#undef null
#if __STDC__ || defined(__cplusplus)
#define __(x) x
#define null (void *)0
#else 
#define __(x) ()
#define null 0L
#endif
#endif

#define CANCHOWN  (S_UNIX5)
#define CANCHSZ   ((RELLVL > 2) || S_XENIX)

extern char nulls[];


#endif

