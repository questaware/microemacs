/*	CHAR.C: Character handling functions for
		MicroEMACS 3.10
		(C)opyright 1988 by Daniel Lawrence

		ALL THE CODE HERE IS FOR VARIOUS FORMS OF ASCII AND
		WILL HAVE TO BE MODIFIED FOR EBCDIC
*/

#include	<stdio.h>
#include	<stdarg.h>
#include	"estruct.h"
#define IN_CHAR_C
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"


#define DIFCASE  ('a'-'A')


#define get_arg(type,ap) va_arg(ap, type)


char * concat(char * tgt, ...)
{
  va_list ap;
  va_start(ap, tgt);

{ char * s;
  tgt[0] = 0;

  while ((s = get_arg(char *, ap)) != null)
    strcat(tgt, s);

  return tgt;
}}
					/* at most n chars including the 0 */

char * Pascal strpcpypfx(char * t_, const char * s_, int n_, char term)

{ short n;
  char ch;
  const char * s = s_;
  char * t = t_;

  for (n = -1; ++n < n_ && (ch = s[n]) != 0 && ch != term;
      )
    t[n] = ch;
  
  t[n] = 0;

  return t_;
}

					/* at most n chars including the 0 */
char * Pascal strpcpy(char * t_, const char * s_, int n_)
#if 0
{ register short n = n_;
  register char * s = s_;
  register char * t = t_;
 
  if (t != null)
  { while (--n > 0 && (*t++ = *s++) != 0)
      ;
  
    if (n == 0)
      *t = 0;
  }
  return t_;
}
#else
{ return strpcpypfx(t_, s_, n_-1, 0);
}
#endif


/* ################# string match routines ##################### */

char bad_strmatch;

const char * Pascal com_match(const char * t_, const char * s_, int mask)

{ register int tix = 0;
  register const char * s = s_;
  register char ch;

#if CHARCODE != ASCII
  if (mask == 0x20)
    for (; *s != 0 && (ch = t_[tix]) != 0 && toupper(ch) == toupper(*s); ++s)
      ++tix;
  else 
#endif
    for (; *s != 0 && (ch = t_[tix]) != 0 && (ch | mask) == (*s | mask); ++s)
      ++tix;
  bad_strmatch = *s;
  return &t_[tix];
}


const char * Pascal strmatch(const char * t_, const char * s_)

{ return com_match(t_, s_, 0x20);
}


#if	DIACRIT

NOSHARE	char lowcase[HICHAR];	/* lower casing map		*/
NOSHARE	char upcase[HICHAR];	/* upper casing map		*/

/*	isletter()
		Is the character a letter?  We presume a letter must
		be either in the upper or lower case tables (even if it gets
		translated to itself).
*/
int Pascal isletter(ch)
	register unsigned int ch;
{
  return upcase[ch] || lowcase[ch];
}

int Pascal islower(ch)
	register unsigned int	ch;
{
  return lowcase[ch] != 0;
}

int Pascal isupper(ch)
	register unsigned int	ch;
{
  return upcase[ch] != 0;
}

unsigned int Pascal chcase(ch)
	register unsigned int	ch;
{
  return islower(ch) ? lowcase[ch] :
				 isupper(ch) ? upcase[ch]  : ch;
}

char Pascal toupper(ch)
	char ch;
{
  return ! islower(ch) ? ch : lowcase[ch];
}


char Pascal tolower(ch)
	char ch;
{
  return ! isupper(ch) ? ch : upcase[ch];
}


Pascal initchars() /* initialize the character upper/lower case tables */

{	register int index;     
#if 0
					/* all of both tables to zero */
	for (index = HICHAR - 1; index >= 0; --index)
	{ lowcase[index] = 0;
	  upcase[index] = 0;
	}
#endif
						/* lower to upper */
	for (index = 'a'; index <= 'z'; index++)
	  lowcase[index] = index - DIFCASE;
						/* upper to lower */
	for (index = 'A'; index <= 'Z'; index++)
	  upcase[index] = index + DIFCASE;

#if	S_MSDOS
				/* setup various extended IBM-PC characters */
{ static const char specials[] = {
	0x9a,	/* 81: U with an umlat */
	0x90,	/* 82: E with an acute accent */
	0x83,	/* 83: A with two dots */
	0x8e,	/* 84: A with an umlat */
	0x85,	/* 85: A with a grave accent */
	0x8f,	/* 86: A with a circle */
	0x80,	/* 87: C with a cedilla */
	0x88,	/* 88: E with a ^ */
	0x89,	/* 89: E with two dots */
	0x8a,	/* 8a: E with a grave accent */
	0x8b,	/* 8b: I with two dots */
	0x8c,	/* 8c: I with a ^ */
	0x8d,	/* 8d: I with a grave accent */
	0,		/* 8e: */
	0,		/* 8f: */
	0,		/* 90: */
	0x92,	/* 91: AE combination */
	0,		/* 92: */
	0x93,	/* 93: O with a ^ */
	0x99,	/* 94: O with an umlat */
	0x95,	/* 95: O with an acute accent */
	0x96,	/* 96: U with a ^ */
	0x97,	/* 97: U with an grave accent */
	0x98,	/* 98: Y with two dots */
			   };
	
	upcase[0x80]  = 0x87;	/* C with a cedilla */
	upcase[0x8e]  = 0x84;	/* A with an umlat */
	upcase[0x8f]  = 0x86;	/* A with a circle */
	upcase[0x90]  = 0x82;	/* E with an acute accent */
	upcase[0x92]  = 0x91;	/* AE combination */
	upcase[0x99]  = 0x94;	/* O with an umlat */
	upcase[0x9a]  = 0x81;	/* U with an umlat */
	upcase[0xa5]  = 0xa4;	/* N with a ...... */

	memcpy(&lowcase[0x81], specials, sizeof(specials));

	lowcase[0xa0] = 0xa0;	/* A with an acute accent */
	lowcase[0xa1] = 0xa1;	/* I with an acute accent */
	lowcase[0xa2] = 0xa2;	/* O with an acute accent */
	lowcase[0xa3] = 0xa3;	/* U with an acute accent */
	lowcase[0xa4] = 0xa5;	/* N with a ...... */
	lowcase[0xa6] = 0xa6;	/* A underlined */
	lowcase[0xa7] = 0xa7;	/* O underlined */
}
#endif
}

/*	Set a character in the lowercase map */

int Pascal setlower(ch, val)
	char *ch;	/* ptr to character to set */
	char *val;	/* value to set it to */
{ return lowcase[*ch & 255] = *val & 255;
}

/*	Set a character in the uppercase map */

int Pascal setupper(ch, val)
	char *ch;	/* ptr to character to set */
	char *val;	/* value to set it to */
{
  return upcase[*ch & 255] = *val & 255;
}
#else
				/* not DIACRIT */
				/* change *cp to an upper case character */
//void uppercase(char * cp)
			/* ptr to character to uppercase */
//{ if (islower(*cp))
//    *cp -= DIFCASE;
//}


#if UNIX == 0 && MSC == 0

char Pascal toupper(ch)	/* return the upper case equivalant of a character */
	char ch;	/* character to get uppercase euivalant of */
{  return ! islower(ch) ? ch : ch - DIFCASE;
}

char Pascal tolower(ch)	/* return the lower case equivalant of a character */
	char ch;	/* character to get lowercase equivalant of */
{  return ! isupper(ch) ? ch : ch + DIFCASE;
}

#endif

#endif

