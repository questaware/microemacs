/*	EVAL.C: Expresion evaluation functions for MicroEMACS

	written 1986 by Daniel Lawrence 			*/

#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"
#include	"build.h"
#include	"edef.h"
#include	"etype.h"
#include	"evar.h"
#include	"elang.h"
#include	"map.h"
#include	"logmsg.h"

#if S_WIN32 && 0
#include <windows.h>
#endif

// extern char *getenv();
#define NILNAMIC	-1
#define MONAMIC 	0
#define DYNAMIC 	1
#define TRINAMIC	2

#define RRET (-1)
#define RINT  0
#define RSTR  1

typedef struct UFUNC
{	signed char f_type;	/* 0 = monamic, 1 = dynamic */
	char  			f_kind;
	char  			f_name[4];  /* name of function */
} UFUNC;

#define	TKVAR	3	/* user variables		*/
#define	TKENV	4	/* environment variables	*/

#define UNDEF 0
															/*	list of recognized user functions	*/
static const UFUNC funcs[] = {
	MONAMIC, RINT, "abs", 	/* absolute value of a number */
	DYNAMIC, RINT, "add",   /* add two numbers together */
	DYNAMIC, RSTR, "and", 	/* logical and */
	MONAMIC, RINT, "asc", 	/* char to integer conversion */
	DYNAMIC, RINT, "ban", 	/* bitwise and */
	MONAMIC, RRET, "bin", 	/* loopup what function name is bound to a key */
	MONAMIC, RINT, "bno", 	/* bitwise not */
	DYNAMIC, RINT, "bor", 	/* bitwise or	 */
	DYNAMIC, RINT, "bxo", 	/* bitwise xor */
	DYNAMIC, RRET, "cat", 	/* concatenate string */
	MONAMIC, RRET, "chr", 	/* integer to char conversion */
	DYNAMIC, RRET, "dir",		/* replace tail of filename with filename */
	NILNAMIC,RRET, "dit",		/* the character in the line above */
	DYNAMIC, RINT, "div", 	/* division */
	MONAMIC, RRET, "env", 	/* retrieve a system environment var */
	DYNAMIC, RSTR, "equ", 	/* logical equality check */
	MONAMIC, RSTR, "exi", 	/* check if a file exists */
	MONAMIC, RRET, "fin", 	/* look for a file on the path... */
	DYNAMIC, RSTR, "gre", 	/* logical greater than */
	NILNAMIC,RRET, "gtc",		/* get 1 emacs command */
	NILNAMIC,RRET, "gtk",		/* get 1 charater */
	MONAMIC, RRET, "ind", 	/* evaluate indirect value */
	DYNAMIC, RRET, "lef", 	/* left string(string, len) */
	MONAMIC, RINT, "len", 	/* string length */
	DYNAMIC, RSTR, "les", 	/* logical less than */
	MONAMIC, RRET, "low", 	/* lower case string */
	TRINAMIC,RRET, "mid",		/* mid string(string, pos, len) */
	DYNAMIC, RINT, "mod", 	/* mod */
	MONAMIC, RINT, "neg", 	/* negate */
	MONAMIC, RSTR, "not", 	/* logical not */
	DYNAMIC, RSTR, "or",		/* logical or */
	DYNAMIC, RRET, "rig", 	/* right string(string, pos) */
	MONAMIC, RINT, "rnd", 	/* get a random number */
	DYNAMIC, RSTR, "seq", 	/* string logical equality check */
	DYNAMIC, RSTR, "sgr", 	/* string logical greater than */
	DYNAMIC, RINT, "sin", 	/* find the index of one string in another */
	DYNAMIC, RSTR, "sle", 	/* string logical less than */
#if DIACRIT
	DYNAMIC, RRET, "slo",		/* set lower to upper char translation */
#endif
	DYNAMIC, RINT, "sub", 	/* subtraction */
#if DIACRIT
	DYNAMIC, RRET, "sup",		/* set upper to lower char translation */
#endif
	DYNAMIC, RINT, "tim", 	/* multiplication */
	MONAMIC, RRET, "tri",		/* trim whitespace off the end of a string */
	MONAMIC, RRET, "upp", 	/* uppercase string */
	TRINAMIC,RRET, "xla",		/* XLATE character string translation */
};

#define NFUNCS	sizeof(funcs) / sizeof(UFUNC)

											/*	and its preprocesor definitions 	*/
#define UFABS			 0
#define UFADD			 1
#define UFAND			 2
#define UFASCII 	 3
#define UFBAND		 4
#define UFBIND		 5
#define UFBNOT		 6
#define UFBOR			 7
#define UFBXOR		 8
#define UFCAT			 9
#define UFCHR		  10
#define UFDIR	    11
#define UFDIT		  12
#define UFDIV		  13
#define UFENV		  14
#define UFEQUAL 	15
#define UFEXIST 	16
#define UFFIND		17
#define UFGREATER	18
#define UFGTCMD 	19
#define UFGTKEY 	20
#define UFIND		  21
#define UFLEFT		22
#define UFLENGTH	23
#define UFLESS		24
#define UFLOWER 	25
#define UFMID			26
#define UFMOD			27
#define UFNEG			28
#define UFNOT			29
#define UFOR			30
#define UFRIGHT 	31
#define UFRND			32
#define UFSEQUAL	33
#define UFSGREAT	34
#define UFSINDEX	35
#define UFSLESS 	36
#if DIACRIT
#define	UFSLOWER	37
#define UFSUB		  38
#define	UFSUPPER	39
#define UFTIMES 	40
#define	UFTRIM		41
#define UFUPPER 	42
#define UFXLATE 	43
#else
#define UFSUB		  37
#define UFTIMES 	38
#define	UFTRIM		39
#define UFUPPER 	40
#define UFXLATE 	41
#endif

/*	list of recognized environment variables	*/
static
const char * const g_envars[] = {
	"acount",			/* # of chars until next auto-save */
	"asave",			/* # of chars between auto-saves */
	"bufhook",		/* enter buffer switch hook */
	"cbflags",		/* current buffer flags */
	"cblang",			/* current buffer language */
	"cbufname",		/* current buffer name (read only)*/
	"cfname",		  /* current file name */
	"cliplife",		/* life of data in clip board */
	"cmdhook",		/* command loop hook */
	"cmode",			/* mode of current buffer */
	"cmtcolour",	/* Colour used for comments */
	"col1ch",			/* character selecting first colour */
	"col2ch",			/* character selecting second colour */
	"curchar",		/* current character under the cursor */
	"curcol",			/* current column pos of cursor */
	"curline",		/* current line in file */
	"cwline",			/* current screen line in window */
	"debug",			/* macro debugging */
	"discmd",			/* display commands on command line */
	"disinp",			/* display command line input characters */
	"exbhook",		/* exit buffer switch hook */
	"fcol",				/* first displayed column in curent window */
	"fileprof",		/* profiles for file types */
	"fillcol",		/* current fill column */
	"gmode",			/* global modes (buffer flags )*/
	"hardtab",		/* current hard tab size */
	"hjump",			/* horizontal screen jump size */
	"hp1",		/* highlighting string */
	"hp10",		
	"hp11",		
	"hp12",		
	"hp13",		
	"hp14",		
	"hp15",		
	"hp16",		
	"hp17",
	"hp18",
	"hp19",
	"hp2",		
	"hp20",
	"hp21",
	"hp22",
	"hp23",
	"hp24",
	"hp25",
	"hp26",
	"hp27",
	"hp28",
	"hp29",
	"hp3",		
	"hp30",
	"hp4",		
	"hp5",		
	"hp6",		
	"hp7",		
	"hp8",		
	"hp9",
	"incldirs",		/* directories to search */
	"keycount",		/* consecutive times key has been pressed */
	"kill", 			/* kill buffer (read only) */
	"language",		/* language of text messages */
	"lastdir",		/* last direction of search */
	"lastkey",		/* last keyboard char struck */
	"lastmesg",		/* last string mlwrite()ed */
	"line", 			/* text of current line */
	"match",			/* last matched magic pattern */
	"msflag",			/* activate mouse? */
	"noindent",		/* dont copy space from line above */
	"pagelen",		/* number of lines used by editor */
	"pagewidth",	/* current screen width */
	"palette",		/* current palette string */
	"pending",		/* type ahead pending flag */
	"popup",			/* popup message */
	"readhook",		/* read file execution hook */
	"region",			/* current region (read only) */
	"replace",		/* replacement pattern */
	"search",			/* search pattern */
	"seed", 			/* current random number seed */
	"ssave",			/* safe save flag */
	"status",			/* returns the status of the last command */
	"uarg",				/* last universal arg */
	"undos",			/* size of undo cache */
	"version",		/* current version number */
	"winnew",	  	/* window is newly created */
	"wintitle",		/* the title on the window */
	"wline",			/* # of lines in current window */
	"work",				/* # of buffers modified or not yet read */
	"wraphook",		/* wrap word execution hook */
	"writehook",	/* write file hook */
#if MOUSE
	"xpos", 			/* current mouse X position */
	"ypos",	 			/* current mouse Y position */
#endif
	"zcmd"				/* last command */
};

#define NEVARS	(sizeof(g_envars) / sizeof(g_envars[0]))

PD_VAR predefvars[NEVARS+2] =

//NOSHARE const int predefinits[NEVARS] =
{
256,   /* EVACOUNT */		  /* count until next ASAVE */
256,   /* EVASAVE */ 	
1,     /* EVBUFHOOK */		/* actual: pd_sgarbf */
STOP,  /* EVCBFLAGS */		/* actual: kbdmode - curr keyboard macro mode*/
0,
0,     /* EVCBUFNAME */		/* actual: kbdrd */
-1,    /* EVCFNAME */		  /* actual: kbdwr */
CLIP_LIFE,/*EVCLIPLIFE */	/* actual: cliplife */
0,     /* EVCMDHK */ 		  /* actual: execlevel - execution IF level */
0,     /* EVCMODE */ 	  	/* actual: kbdrep */
1,		 /* EVCMTCOL */		  /* comment colour */
-1,    /* EVCOL1CH */		  /* character selecting colour 1 */
-1,    /* EVCOL2CH */		  /* character selecting colour n */
TRUE,  /* EVCURCHAR */		/* actual: revexist -does reverse video exist?*/
7,     /* EVCURCOL */		  /* */
0,     /* EVCURLINE */		/* */
FALSE, /* EVCWLINE */		  /* */
0,     /* EVDEBUG */      /* macro debugging flag	*/
1,     /* EVDISCMD */     /* display command flag */
1,     /* EVDISINP */     /* display input characters */
0,     /* EVEXBHOOK */    /* actual: swb_luct */
0,     /* EVFCOL */		    /* The left hand offset on the screen */
0,     /* EVFILEPROF */	  /* The profiles of file types */
72,    /* EVFILLCOL */		/* longest line for wordwrap */
0,     /* EVGMODE */ 		  /* global editor mode */
8,     /* EVHARDTAB */		/* default tab size */
0,     /* EVHIGHLIGHT */	/* */
1,     /* EVHJUMP */		  /* horizontal jump size */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,		 /* EVINCLD */			/* */
0,     /* EVKEYCT */		  /* consec key ct */
CTRL |'G',/* EVKILL */  	/* actual: abortc- current abort command char*/
0,		 /* EVLANG */  			/* actual g_focus_ct */
0,     /* EVLASTDIR */	  /* actual: prefix- current pending prefix bits*/
0,     /* EVLASTKEY */    /* last keystoke */
0,     /* EVLASTMESG */   /* last message */
TRUE,  /* EVLINE */       /* not in use */
0,     /* EVMATCH */      /* actual: saveflag - Flags, saved with $target var */
1,     /* EVMSFLAG */     /* use the mouse? */
0,		 /* EVNOINDENT */		/* dont copy space from line above */
FALSE, /* EVPAGELEN */    /* actual: eexitflag */
TRUE,  /* EVPAGEWIDTH */	/* */
0,     /* EVPALETTE */    /* not in use */
0,     /* EVPENDING */    /* actual: nclicks - clrd on any non-mouse event*/
0,		 /* EVPOPUP */   		/* */
FALSE, /* EVREADHK */     /* not in use */
UNDEF, /* EVREGION */	
UNDEF, /* EVREPLACE */       
UNDEF, /* EVSEARCH */     
0,     /* EVSEED */       /* random number seed */
1,     /* EVSSAVE */      /* safe save flag */
1,     /* EVSTATUS */			/* last command status */
#if 0
0,     /* EVUARG */				/* universal argument */
0,     /* EVUNDOS */
UNDEF, /* EVVERSION */	
UNDEF, /* EVWINTITLE */
UNDEF, /* EVWLINE */ 	
UNDEF, /* EVWRAPHK */	
UNDEF, /* EVWRITEHK */	
0,     /* EVXPOS */			/* current column mouse is positioned to*/
0,     /* EVYPOS */			/* current screen row	*/
#endif
};

//char * g_file_prof = NULL;

void Pascal varinit()	/* initialize the user variable list */

{	int ix;
	for (ix = 31; --ix >= 0; )
		predefvars[ix > 0  ? EVHLIGHT1-1+ix :
							           EVMATCH].p = mallocz(2);
#if S_MSDOS == 0
  predefvars[EVPALETTE].p = strdup("426153789abcdef0");
#endif
#if CALLED
  for (i = MAXVARS; --i >= 0;)
    uv[i].u_name[0] = 0;
#endif
}


#if 0

Pascal varclean()	/* initialize the user variable list */

{ int i;

  for (i = MAXVARS - 1; i >= 0; --i)
    if (uv[i].u_name[0] != 0)
      free(uv[i].u_value);
}

#endif


static int USE_FAST_CALL absv(int x) /* take the absolute value of an integer */
	
{
  return x < 0 ? -x : x;
}


static 
const char * USE_FAST_CALL ltos(int val)	/* numeric logical to string logical */
			/* value to translate */
{
  return g_logm[val == 0 ? 0 : 1];
}


int Pascal USE_FAST_CALL trimstr(int from, char * s)/* trim whitespace off string */
										/* string to trim */
{
	while (--from >= 0 && s[from] <= ' ')
		s[from] = 0;
	return from + 1;
}

char *Pascal mkul(int wh, char * str)	/* make a string lower or upper case */
				/* 0 => lower */
				/* string to upper case */
{	int ix;
	char ch;

	for ( ix = -1; (ch = str[++ix]) != 0; )
	{ 
	  str[ix] = wh ? toupper(ch) : tolower(ch);
	}
	return str;
}



static short Pascal USE_FAST_CALL plinecpy(void)
	
{//int here = getccol();
  
  short res = 0;

	LINE * pline = curwp->w_dotp;
  if (pline != NULL)
    while (pline != curwp->w_linep)
    { pline = lback(pline);
    { int offs = getgoal(getccol(), pline);
      if (offs < llength(pline))
      { res = pline->l_text[offs];
        break;
      }
    }}

  return res;
}

//static
//const char * USE_FAST_CALL fixnull(const char * s)   /* Exclude NULL pointers */
	
//{
//  return s == NULL ? "" : s;
//}


static struct
{ char * result;
	int    top;
	int		 lim;
} g_stk;

static char * USE_FAST_CALL push_arg(int fnum, const char * src)

{	int sl = strlen(src);
  if (fnum != UFCAT)
		sl = (sl + 2) & -2;

	g_stk.top += sl;
	
	if (g_stk.top >= g_stk.lim)
	{ g_stk.lim += 2 * NSTRING;
	  (void)remallocstr(&g_stk.result, g_stk.result, g_stk.lim + 2 * NSTRING);
	}

{	char * tgt = &g_stk.result[g_stk.top];
	int rc = fnum == 0 ? -1 : macarg(tgt);
	if ((fnum == UFMID || fnum == UFXLATE) && rc > 0)
		rc = macarg(tgt+strlen(tgt)+1);
	g_stk.top -= sl;
	return rc < 0 ? tgt - sl :
				 rc 		? tgt			 : NULL;
}}


int USE_FAST_CALL stol(const char * val)					/* convert a string to a numeric logical */
{
  return val[0] == 'T' || atoi(val) != 0;	/* check for logical values */	 
}


#define INTWIDTH (sizeof(int) * 3)

static char result[90];

				/* output integer as radix r */
char * Pascal USE_FAST_CALL int_radix_asc(int i, int radix, char fill)

{	static const char hexdigits[] = "0123456789ABCDEF";

	memset(&result, fill, INTWIDTH);
	result[INTWIDTH] = 0;
//result[INTWIDTH+1] = 0;	

{	char *sp = &result[INTWIDTH+1];
	int v = i;		/* sign of resulting number */

	if (v < 0)
	  v = -v;

	do 
	{ *(--sp) = hexdigits[v % radix];	/* and install the new digit */
	  v = v / radix;
	} while (v);

	if (i < 0)
	  *(--sp) = '-';		/* and install the minus sign */

	return sp;
}}



char *Pascal USE_FAST_CALL int_asc(int i)
			/* integer to translate to a string */
{ return int_radix_asc(i,10, ' ');
}


char * float_asc(double x)

{ int sign = 0;
	if (x < 0.0)
	{	x = -x;
		--sign;
	}

{ int shift = 0;
  while (x > 2000000000)
  { ++shift;
    x /= 10;
  }

	if (x > 0.0 && x < 0.00001)
	{		//	shift -= 2;
		while (x < 1.0)
		{
			--shift;
			x *= 10;
		}
	}
  
{	int val = (int)x;
  x -= val;
  result[19] = '-';
{ char * res = int_asc(val);
  strcpy(result+20, res);
{ int after = (int)(x * 10000000);
  char * aft = int_radix_asc(after, 10, '0');
  strcpy(result+40, result+6);
{	char * exp = shift == 0 ? NULL : "e";
	char * shft = int_asc(shift);

  return concat(result+60, result+20+sign, ".", result+40, exp, shft, NULL);
}}}}}}


static 
int Pascal USE_FAST_CALL sindex(char * src, char * pattern) /*find pattern in src*/
	/* char *src;	    * source string to search */
	/* char *pattern;	* string to look for */
{
	char *sp;

	for (sp = src-1; *++sp; )
	{ int ix;
		char ch;
	  for (ix = -1; (ch = pattern[++ix]) != 0 && myeq(ch, sp[ix]); )
	    ;
	    						/* was it a match? */
	  if (ch == 0)
	    return (int)(sp - src) + 1;
	}
						/* no match at all.. */
	return 0;
}

/*	Macro argument token types			*/

#define TOKNUL	0	  /* end-of-string		*/
#define	TOKARG	'@'	/* interactive argument		*/
#define	TOKBUF	'#'	/* buffer argument		*/
#define	TOKVAR	'%'	/* user variables		*/
#define	TOKENV	'$'	/* environment variables	*/
#define	TOKFUN	'&'	/* function....			*/
#define	TOKLBL	'*'	/* line label			*/
#define	TOKSTR	'"'	/* quoted string literal	*/

extern char deltaf[NSTRING];

Map_t g_fnamemap = mk_const_map(T_DOMCHAR0+4, 2, funcs, 0);

Map_t g_evmap = mk_const_map(T_DOMSTR, 0, g_envars, 0);

static 
const char * USE_FAST_CALL gtenvfun(char typ, char * fname)/* evaluate a var/function */

{ BUFFER * bp = curbp;
	int ix = 0;

	int vnum = binary_const(typ == TOKENV, fname);

	if (typ == TOKFUN)
	{	int iarg1;
		int iarg2;
		char * arg2 = NULL;
		char * arg1 = push_arg(0,"");					/* to initialise area */

		if (funcs[vnum].f_type >= MONAMIC)	/* retrieve the first argument */
		{ 
		  if (macarg(arg1) <= 0)
		    return null;
													 /* if needed, retrieve the second and third arguments */
		  if (funcs[vnum].f_type >= DYNAMIC)
		  { arg2 = push_arg(vnum, arg1);// arglen: length of arg1
				if (arg2 == NULL)
		      return null;

	      iarg2 = atoi(arg2);
		  }
		  iarg1 = atoi(arg1);
		}

		if (vnum < 0)
			return NULL;
											/* and now evaluate it! */

		if      (funcs[vnum].f_kind < RINT)
		 switch (vnum)
		 {case UFDIT:		*(short*)arg1 = plinecpy();
		 								return arg1;
		  when UFRIGHT: iarg2 -= strlen(arg1);
					        	return strcpy(arg1, &arg1[-iarg2 < 0 ? 0 : -iarg2]);
			when UFDIR:		return pathcat(arg1, NSTRING-1, arg1, arg2);
			when UFIND:		return getval(arg1, arg1);

			when UFLOWER:
			case UFUPPER:	return mkul(vnum - UFLOWER, arg1);
			when UFLEFT:	if (g_stk.lim - g_stk.top > iarg2)
			            	  arg1[iarg2] = 0;
										if (0)
			            	{
			when UFTRIM:    (void)trimstr(strlen(arg1),arg1);
											if (0)
											{ 
			when UFGTKEY:
			default: // UFCHR
												arg1[0] = vnum == UFGTKEY ? iarg1 : tgetc();
							          arg1[1] = 0;
											}
					          }
			case UFCAT:   return arg1;

			when UFGTCMD:	return cmdstr(&arg1[0], getcmd(0));
			when UFBIND:  return getfname(stock(arg1));
			when UFFIND:	arg1 = (char*)flook(Q_LOOKH, arg1);
										if (0)
			case UFENV:			arg1 = getenv(arg1);
										return arg1 == NULL ? "" : arg1;
			when UFXLATE:
			case UFMID:	 { char * arg3 = arg2 + strlen(arg2) + 1;
										 if (vnum == UFXLATE)
					           	 return xlat(arg1, arg2, arg3);
										 
									 	 if (iarg2 < 0)
									 	 	 iarg2 = 0;
									 { int end = iarg2 + atoi(arg3);
									 	 if (end >= 0 && end < g_stk.lim)
									 	   arg1[end] = 0;
					           return arg1+iarg2;
				           }}
#if DIACRIT
			when UFSLOWER:setlower(arg1, arg2);
					          if (0)
			when UFSUPPER:  setupper(arg1, arg2);
										return ""
#endif
		 }
		else if (funcs[vnum].f_kind == RINT)
		{switch (vnum)
		 {case UFADD:	  	iarg1 += iarg2;
			when UFSUB:	  	iarg1 -= iarg2;
			when UFTIMES: 	iarg1 *= iarg2;
			when UFDIV:	  	iarg1 /= iarg2;
			when UFMOD:	  	iarg1 = iarg1 % iarg2;
			when UFNEG:	  	iarg1 = -iarg1;
			
			when UFASCII:		iarg1 = (int)arg1[0];
			when UFGTKEY: 	  
		  when UFABS:			iarg1 = absv(iarg1);
			when UFRND:   	iarg1 = (ernd() % absv(iarg1)) + 1;
			when UFSINDEX:	iarg1 = sindex(arg1, arg2);
			when UFBAND:		iarg1 &= iarg2;
			when UFBOR:	  	iarg1 |= iarg2;
			when UFBNOT:		iarg2 = -1;
			case UFBXOR:		iarg1 ^= iarg2;
			when UFLENGTH:	iarg1  = strlen(arg1);
			otherwise		  	return "";
		 }
		 return int_asc(iarg1);
		}
		else
		{switch (vnum)
	   {case UFEQUAL:		iarg1 = iarg1 == iarg2;
			when UFLESS:		iarg1 = iarg1 < iarg2;
											if (0)
			case UFGREATER:  iarg1 = iarg1 > iarg2;

			when UFSEQUAL:
			case UFSGREAT:	
			case UFSLESS:		iarg1 = strcmp(arg1, arg2);
											/*if (! in_range(iarg1, -1, 1))
					    						adb(iarg1);*/
			                if      (vnum == UFSLESS)
			                  iarg1 &= 2;
			                else if (vnum == UFSEQUAL)
			                  iarg1 = iarg1 == 0;
			                else if (iarg1 < 0)
			                  iarg1 = 0;
			when UFNOT: 	  
			case UFAND:
			case UFOR:		  iarg1 = stol(arg1);
											if (vnum == UFNOT)
												iarg1 ^= 1;
											else
										  {	iarg2 = stol(arg2);
												if (vnum == UFOR)
										    	iarg1 |= iarg2;
												else
											  	iarg1 &= iarg2;
											}

			when UFEXIST:	  iarg1 = fexist(arg1);
		 }
		 return ltos(iarg1);
		}
	}
	else
	{ if (vnum < 0)
			return g_logm[2];
 
		if (in_range(vnum, EVHLIGHT1, EVHLIGHT9))
			goto ret_str;

		switch (vnum)
		{ 
		  case EVINCLD:	 
		  case EVPALETTE:
		  case EVFILEPROF:
			case EVMATCH:
				goto ret_str;
		}

#define result (&deltaf[NSTRING / 2])   /* leave beginning for extra safety */
	{	int res = predefvars[vnum].i;

		switch (vnum)
		{ case EVPAGELEN:  res = term.t_nrowm1 + 1;
		  when EVPAGEWIDTH:res = term.t_ncol;
		  when EVCURCOL:   res = getccol();
		  when EVCURLINE:	 res = setcline();
		  when EVHARDTAB:  res = bp->b_tabsize;
	//	when EVUSESOFTTAB:res = bp->b_mode & BSOFTTAB ? 1 : 0;
		  when EVCBFLAGS:  res = bp->b_flag;
		  when EVCBLANG:	 res = bp->b_langprops;;
		  when EVCBUFNAME: return bp->b_bname;
		  when EVCFNAME:   return bp->b_fname;
		  when EVCMODE:    res = res >> NUMFLAGS;
	//  when EVSRES:	   return sres;
	//  when EVPALETTE:  return palstr;
	//  when EVFILEPROF: return g_file_prof;
		  when EVCURCHAR:  res = llength(curwp->w_dotp) == curwp->w_doto 
	                    								? '\n' : lgetc(curwp->w_dotp, curwp->w_doto);
		  when EVWLINE:    res = curwp->w_ntrows;
		  when EVCWLINE:   res = getwpos();
		  when EVSEARCH:   return pat;
	//  when EVHIGHLIGHT:return highlight;
		  when EVREPLACE:  return rpat;
	//  when EVMATCH:    return patmatch;
		  when EVKILL:	   return getkill();
		  when EVREGION:   return getreg(&result[0]);
		  
		  when EVLINE:	   return getctext(&result[0]);
		  when EVLASTMESG: return strpcpy(result,lastmesg,NPAT);
	//  when EVFCOL:	   res = pd_fcol;

		  when EVBUFHOOK:
		  case EVEXBHOOK:  --ix;
		  case EVWRITEHK:  --ix;
		  case EVCMDHK:    --ix;
		  case EVWRAPHK:   --ix;
		  case EVREADHK:   --ix;
							   		   return getfname(ix);
		  when EVVERSION:  return VERSION;
#if 0		  
		  when EVLANG:	   return LANGUAGE;
#endif
	    when EVZCMD:     return g_ll.lastline[g_ll.ll_ix & MLIX];
#if S_WIN32
		  when EVWINTITLE: return null;	// getconsoletitle();
#endif
			when EVWORK:		 res = lastbuffer(0,0) & 0xff;
		  when EVPENDING:
#if	GOTTYPAH
										   res = typahead();
#endif
		  case EVDEBUG:   
		  case EVSTATUS:  
		  case EVDISCMD:  
		  case EVDISINP:  
		  case EVSSAVE:   
		  case EVMSFLAG:   return ltos(res);

		  default:	       loglog2("Var %d = %x", vnum, res);
		}
		return int_asc(res);
#undef result

	ret_str:
		return predefvars[vnum].p;
		}
	}
}


/*	structure to hold user variables and their definitions	*/

typedef struct UVAR
{	char u_name[NVSIZE + 1];	       /* name of user variable */
	char *u_value;				/* value (string) */
} UVAR;

/*	current user variables (This structure will probably change)	*/

static
UVAR NOSHARE g_uv[MAXVARS + 1];	/* user variables */

static int g_uv_vnum;

const char *Pascal gtusr(char * vname)			/* look up a user var's value */
																						/* name of user variable to fetch */
{	char * vptr = NULL;
	int vnum;
																/* scan the list looking for the user var name */
	for (vnum = MAXVARS; --vnum >= 0 && g_uv[vnum].u_name[0] != 0; )
	  if (strcmp(vname, g_uv[vnum].u_name) == 0)
	  { vptr = g_uv[vnum].u_value;
	    break;
	  }
	
	g_uv_vnum = vnum;
	return vptr;
}

#if 0

char *Pascal funval(i)
	int i;
{ return funcs[i].f_name;
}

static
char *Pascal envval(i)
	int i;
{ return g_envars[i];
}


Pascal binary(key, tval, tlength)
	char *key;		/* key string to look for */
	char *(Pascal *tval)(); /* ptr to function to fetch table value */
	int tlength;		/* length of table to search */
{
	int l = 0;	  /* set current search limit as entire list */
	int u = tlength - 1;
							/* get the midpoint! */
	while (u >= l)
	{ int i = (l + u) >> 1;

	  int cresult = strcmp(key, (*tval)(i));
	  if (cresult == 0)
	    return i;
	  if (cresult < 0)
	    u = i - 1;
	  else
	    l = i + 1;
	}
	return -1;
}

#endif



int Pascal setvar(int f, int n)	/* set a variable */
	/* int n;	** numeric arg (can overide prompted value) */
{
	char ch;
  int cc;
	char var[2*NSTRING+1];									/* name of variable to fetch */
	int clex = g_macargs;
	if (clex <= 0)
	{ cc = getstring(&var[0], NVSIZE+2, TEXT51);
/*				 "Variable to set: " */
	  if (cc <= 0)
	    return ABORT;
	} 
	else				/* grab token and skip it */
		(void)token(var, NVSIZE + 1);

  for (cc = -1; (ch = var[++cc]) != 0 &&
							 (clex >= 0 || ch != ' ' && ch != '='); )
    ;
	var[cc+(var[cc] == 0)] = 0;
														/* get the value for that variable */
	if 	    (f != FALSE)
	  strcpy(&var[cc+1], int_asc(n));
	else if (var[cc+1] == 0)
	{ if (mlreply(TEXT53, &var[cc+1], NSTRING-cc+1) <= 0)
/*				 "Value: " */
	    return ABORT;
	}

	return
#if S_MSDOS == 0
	        var[0] == '#' ? use_named_str(&var[1], &var[cc+1]) :
#endif
	                        set_var(var, &var[cc+1]);
}

/*
 * Set current column.
 */
static
 int Pascal setccol(int pos)
												/* position to set cursor */
{
	int llen = llength(curwp->w_dotp);
	int offs;
	for (offs = -1; ++offs < llen; )
	{ curwp->w_doto = offs;
		if (getccol() >= pos)
			break;
	}	

	return TRUE;
}


static
int Pascal svar(int var, const char * value)	/* set a variable */

{	Cc cc = TRUE;
  int vnum = var & 0x7ff;
  char ** varref;

  if (var - vnum == (TKVAR << 11))
	{ varref = &g_uv[vnum].u_value;
  	goto remalloc;
  }

	if (value == null)
		return TRUE;

{ int val = atoi(value);
  int hookix = 0;		  /* set an environment variable */

	varref = &predefvars[vnum].p;
	if (in_range(vnum, EVHLIGHT1, EVHLIGHT9))
		goto remalloc;

  switch (vnum) 
  {
#if S_WIN32
//  when EVWINTITLE: setconsoletitle( value );
#endif
//	when EVUSESOFTTAB:curbp->b_mode &= ~BSOFTTAB;
//									 if (val)
//										 curbp->b_mode |= BSOFTTAB;
	  when EVPAGELEN:	 cc = newdims(0, val);
	  when EVPAGEWIDTH:cc = newdims(val, 0);
	  when EVCURCOL:	 cc = setccol(val);
	  when EVCURLINE:	 cc = gotoline(TRUE, val);
	  when EVCBFLAGS:	 lchange(WFMODE);
                   /*if (val & BFCHG)
                       mbwrite("EVAL");*/
	  case EVCMODE:
	      						 hookix = curbp->b_flag;
	       						 if (vnum == EVCBFLAGS)
	   	  						   hookix = (hookix & ~(BFCHG|BFINVS))
										          | (val & (BFCHG|BFINVS));
		    						 else
		    						 { hookix &= MDSTT - 1;
			  						   hookix |= val << NUMFLAGS;
			  						   curwp->w_flag |= WFMODE;
		 	  						 }
			  						 curbp->b_flag = hookix;
//	when EVSRES:	   cc = TTrez(value);

	  when EVCURCHAR:	 ldelchrs(1, FALSE);		/* delete 1 char */
		                 linsert(1, (char)val);
			               backbychar(1);
										 update(FALSE);
	  when EVWLINE:	   cc = resize(0,val);
	  when EVCWLINE:	 val -= getwpos();
										 cc = forwline(val,val);
	  when EVLASTMESG: strpcpy(lastmesg,value,NSTRING);

	  when EVSEARCH:
	  case EVREPLACE:	 strpcpy(vnum == EVSEARCH ? pat : rpat,value,NPAT);

	  when EVPALETTE:
	  case EVFILEPROF:
	  case EVINCLD:		 goto remalloc;

	  when EVPOPUP:    mbwrite(value);
				           //  upwind();
	  when EVKILL:
		case EVCBUFNAME: 
	  case EVLINE:	   return FALSE;						// read only
	  when EVFCOL:	   if (val < 0)
	                     val = curwp->w_doto;
	                   pd_fcol = val;
	                   curwp->w_fcol = val;
	                   if (0)
	  when EVCFNAME:	 	 repl_bfname(curbp, value);
			               curwp->w_flag |= WFHARD | WFMODE;
	  when EVBUFHOOK:
	  case EVEXBHOOK: ++hookix;
	  case EVWRITEHK: ++hookix;
	  case EVCMDHK:   ++hookix;
	  case EVWRAPHK:  ++hookix;
    case EVREADHK:  ++hookix;
										hooks[hookix].k_code = 1;
								//	hooks[hookix].k_type = BINDFNC;
										hooks[hookix].k_ptr.fp = fncmatch(value);
//									setktkey(BINDFNC, value, &hooks[hookix]);
#if S_WIN32
		when EVWINTITLE:setconsoletitle((char*)value);	// Is this safe?
#endif
	  when EVDEBUG:	
	  case EVSTATUS:	
	  case EVDISCMD:	
	  case EVDISINP:	
	  case EVSSAVE:	
    case EVMSFLAG:  val = stol(value);
										goto storeint;

		when EVHARDTAB:	if (val != 0)
                 	    curbp->b_tabsize = val;

	                  upwind(TRUE);
//									goto storeint;
										if (0)
    when EVHJUMP:   if (val > term.t_ncol - 1)
				              val = term.t_ncol - 1;
    default:	      goto storeint;
  }

  return cc;

storeint:
	predefvars[vnum].i = val;

	return TRUE;

remalloc:
	return remallocstr(varref, value, 0) != null;
}}



#if DEBUGM

static int Pascal findvar(char * var)      /* find a variables type and name */
	
{	int vix;	/* type to return */

fvar:	vix = -1;
	switch (var[0])
	{ case '$':			/* check for legal enviromnent var */
	      vix = binary_const(1, &var[1])|(TKENV << 11);

	  when '%':		  /* check for existing legal user variable */
	  	  (void)gtusr(&var[1]);
	  	  vix = g_uv_vnum;
		    if (vix >= 0)
		    {	strcpy(g_uv[vix].u_name, &var[1]);

					vix |= TKVAR << 11;
		    }				/* indirect operator? */
	  when '&': 
		    if (var[1] == 'i' && var[2] == 'n' && var[3] == 'd')// && g_macargs > 0)
		    {			  /* grab token, and eval it */
		      (void)token(var, NVSIZE+1);
		      getval(&var[0], var);
		      goto fvar;
		    }
	}
	return vix;
}

#endif


int Pascal set_var(char var[NVSIZE+1], const char * value)	/* set a variable */
					/* name of variable to fetch */
					/* value to set variable to */
{	int vix;	/* type to return */

fvar:	vix = -1;
	switch (var[0])
	{ default:
		case '$':			/* check for legal enviromnent var */
	      vix = binary_const(1, &var[var[0] == '$'])|(TKENV << 11);

	  when '%':		  /* check for existing legal user variable */
				if (strlen(var) < NVSIZE)
	  	  {	(void)gtusr(&var[1]);
		  	  vix = g_uv_vnum;
			    if (vix >= 0)
			    {	strcpy(g_uv[vix].u_name, &var[1]);
						vix |= TKVAR << 11;
		    	}				/* indirect operator? */
		    }
	  when '&': 
		    if (var[1] == 'i' && var[2] == 'n' && var[3] == 'd')// && g_macargs > 0)
		    {			  /* grab token, and eval it */
		      (void)token(var, NVSIZE+1);
		      getval(&var[0], var);
		      goto fvar;
		    }
	}

	if (vix < 0)
	{ mlwrite(TEXT52, var);
					/* "%%No such variable as '%s'" */
	  return FALSE;
	}

#if DEBUGM == 0
	return svar(vix, value);	/* and set the appropriate value */
#else
{	int cc = svar(vix, value);	/* and set the appropriate value */
		/* if $debug == TRUE, every assignment will echo a statment to
							that effect here. */
	if (pd_macbug && (strcmp(var, "%track") != 0))
	{ mlwrite("%!(%s <- %s)", var, value);
	  update(TRUE);
	        
	  if (getkey() == g_abortc) /* and get the keystroke to hold the output */
	  { mlwrite("%!"TEXT54);
						/* "[Macro aborted]" */
	    cc = FALSE;
	  }
	}
	return cc;
}
#endif
}


const char getvalnull[] = "";

				/* the oob checks are faulty */
					/* find the value of a token */
const char *Pascal getval(char * tgt, char * tok)
														/* token: token to evaluate */
{	int blen = NSTRING-1;
	const char * src = (const char *)tok;
	char * tokp1 = tok + 1;
//int typ = gettyp(tok);

	switch (*tok)
	{ case TOKARG:															/* interactive argument */
						if (tok[1] == TOKARG)
						{	++tok;
							blen = 0;
						}
						getval(&tok[0], &tok[1]);
			      ++pd_discmd;							/* echo it always! */
					{	Cc cc = getstring(&tgt[0], blen, tok);
						--pd_discmd;
							return cc < 0 ? getvalnull : tgt;
			    }
	  case TOKBUF:															/* buffer contents fetch */
					{ BUFFER * bp = bfind(getval(tgt, tokp1), FALSE);
						if (bp == NULL)
						  return getvalnull;
						  
					{	WINDOW * wp = window_ct(bp);  // Any window on the buffer
					  LINE * lp = wp->w_dotp;
						src = (const char *)&lp->l_text[0];
						if (!l_is_hd(lp))
						{									  		/* step the buffer's line ptr ahead a line */
							wp->w_doto = 0;
							wp->w_dotp = lforw(lp);
						}
						if (lp->l_dcr < ((NSTRING-1)<< (SPARE_SZ+2)))
							blen = lused(lp->l_dcr);
							              /* grab the line as an argument */
		      }}
	  when TOKVAR:  src = gtusr(tokp1);
									if (src == NULL)
									  src = g_logm[2];	// "ERROR"

	  when TOKFUN: tokp1[3] = 0;				    /* look the function up in the function table */
								 (void)mkul(0, tokp1);		/* only first 3 chars significant */
	  	
	  case TOKENV: src = gtenvfun(*tok, tokp1);
//  when TOKLIT:
//  case TOKCMD:                             // do nothing
	  when TOKSTR: ++src;
							{ char ch;
							  for (blen = -1; (ch = src[++blen]) != 0 && ch != '"'; )
							    ;
	  					}
//	when TOKNUL:
	  when TOKLBL:	src = NULL;
	}

	return strpcpy(tgt, src == NULL ? "" : src, blen+1);
}



int Pascal ernd()	/* returns a random integer */

{	pd_seed = absv(pd_seed * 1721 + 10007);
	return pd_seed;
}


/*	Filter a string through a translation table	*/

char *Pascal xlat(char * srctgt, char * lookup, char * trans)
	/* char *srctgt;	* string to filter */
	/* char *lookup;	* characters to translate */
	/* char *trans;		* resulting translated characters */
{
	char *sp;	/* pointer into source table */
	char *lp;	/* pointer into lookup table */

	for (sp = srctgt; *sp; ++sp)
	{ for (lp = lookup; *lp; ++lp)
	    if (*sp == *lp)
	    { *sp = trans[lp - lookup];
	      break;
	    }
	}

	return srctgt;
}


static
void fmt_desv(char pfx, const char * v, const char * val)
	
{					/* and add it as a line to the buffer */
#if NVSIZE > 12
error error
#endif
  (void)mlwrite("%!%>%c%12s %s\n", pfx, v, val);
}



int Pascal mkdes()

{ curbp->b_flag |= MDVIEW;
  curbp->b_flag &= ~BFCHG;										/* don't flag this as a change */
  curwp->w_dotp = lforw(&curbp->b_baseline);     /* back to the beginning */
  curwp->w_doto = 0;
//upmode();
//mlerase();					/* clear the mode line */

#if DO_UNDO
	g_inhibit_undo = 0;
#endif
  return TRUE;
}

/*	list-variables	Bring up a fake buffer and list the contents
				of all the environment variables
*/

int Pascal listvars(int f, int n)

{ int uindex; // = curbp->b_tabsize;   /* index into uvar table */
	BUFFER * bp = curbp;
        
  openwindbuf(TEXT56);
//curbp->b_tabsize = 0;
	curbp->b_flag |= BFCHG;										/* Suppress the beep */
  
  --pd_discmd;
				      												/* build the environment variable list */
  for (uindex = -1; ++uindex < NEVARS; )
  {	BUFFER * sbp = curbp;
  	curbp = bp;
  {	char * val = gtenvfun(TOKENV, g_envars[uindex]);
    curbp = sbp;
    fmt_desv('$', g_envars[uindex], val);
  }}

  lnewline(1);
					    /* build the user variable list */
	for (uindex = MAXVARS; --uindex >= 0 && g_uv[uindex].u_name[0] != 0; )
    fmt_desv('%', g_uv[uindex].u_name, g_uv[uindex].u_value);

  ++pd_discmd;

  return mkdes();
}

									/*	describe-functions	Bring up a fake buffer and list the
											names of all the functions */

int Pascal desfunc(int f, int n)

{	int uindex;

	openwindbuf(TEXT211);
						  							/* build the function list */
	for (uindex = -1; ++uindex < NFUNCS; )
											     /* add in the environment variable name */
	  fmt_desv('&', funcs[uindex].f_name, NULL);

  return mkdes();
}


/*------------------------------------------------------------*/

#if DEBUGM

int Pascal dispvar(int f, int n)	/* display a variable's value */
	/* int n;		** numeric arg (can overide prompted value) */
{
	char var[NVSIZE+1];	/* name of variable to fetch */
	char val[NSTRING];

												/* first get the variable to display.. */
	if (g_macargs > 0)
	  (void)token(var, NVSIZE + 1);			/* grab token and skip it */
	else
  { int cc = getstring(&var[0], NVSIZE+1, TEXT55);
																/* "Variable to display: " */
	  if (cc != TRUE)
	    return cc;
	} 

	if (findvar(var) < 0)								/* check the legality, find the var */
	{ mlwrite(TEXT52, var);
					/*	"%%No such variable as '%s'" */
	  return FALSE;
	}
				        
	mlwrite("%s = %s", var, getval(val, var));
	return update(TRUE);
}

#endif
