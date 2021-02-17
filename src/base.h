#ifndef ASLEEP

#define false 0
#define true 1
#define OK 0

#define ASLEEP
#define forward

#define when break; case
#define otherwise break; default:

#define null 0

#define in_range(a,b,c) ((unsigned)((a)-(b)) <= (unsigned)((c)-(b)) && ((signed)c)-(b) >= 0)
#define fieldoffs(mode, fld) ((Int)&((mode)0)->fld)
#define backbyfield(p, mode, fld) (mode*)((char*)p - fieldoffs(mode*, fld))


#define upper_index(arr) (sizeof(arr)/sizeof(arr[0]) - 1)

#define is_space(ch) (ch <= ' ')

#if SIZEOF_LONG > 4
typedef int Int;
typedef unsigned int Set;
typedef unsigned int Set32;
#else
typedef long Int;
typedef unsigned long Set;
typedef unsigned long Set32;
#endif

typedef long Id;
/*
#if SIZEOF_LONG > 4
#define Int int
typedef unsigned int Set;
typedef unsigned int Set32;
#define Id long
#else
#define Int long
typedef unsigned long Set;
typedef unsigned long Set32;
typedef long Id;
#endif*/


typedef short Cc;			/* Condition Code */
typedef short Short;			/* up to 16 bit signed integer */
typedef unsigned char Byte;
typedef short Bool;			/* Boolean: true, false */
typedef char  Char;			/* Character, signed or unsigned */
typedef unsigned short Ushort;		
typedef unsigned short Shortset;
typedef unsigned short Set16;
typedef unsigned long  Quot;		/* 'Quotation', C enum values */
typedef unsigned short Squot;

#ifndef FILENAMESZ
#define FILENAMESZ NFILEN
#endif


#define EDENIED   (-8)

extern char bad_strmatch;
#endif

