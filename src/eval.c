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

static
char *Pascal xlat(char * srctgt, char * lookup, char * trans);

#if USE_FLOAT
#define FA1 farg1
#define FA2 farg2

#define X_ASC(x) float_asc(x)

#if _MSC_VER < 1600
#define ATOLDBL(t,s) t = atof(s)
#else
#define ATOLDBL(t,s) t = strtold(s, NULL)
#endif

#else
#define FA1 iarg1
#define FA2 iarg2

#define X_ASC(x) int_asc(x)
#endif

// extern char *getenv();
#define NILNAMIC	-1
#define MONAMIC 	0
#define DYNAMIC 	1
#define TRINAMIC	2

#define RRET 255
#define RINT  0
#define RSTR  1

typedef struct UFUNC
{	short 			f_type_kind;	/* 0 = monamic, 1 = dynamic */
	char  			f_name[4];  /* name of function */
} UFUNC;

#define	TKVAR	3	/* user variables		*/
#define	TKENV	4	/* environment variables	*/

#define UNDEF 0
															/*	list of recognized user functions	*/
static const UFUNC funcs[] = {
	MONAMIC*256+RINT, "abs", 	/* absolute value of a number */
	DYNAMIC*256+RINT, "add",   /* add two numbers together */
	DYNAMIC*256+RSTR, "and", 	/* logical and */
	MONAMIC*256+RINT, "asc", 	/* char to integer conversion */
	DYNAMIC*256+RINT, "ban", 	/* bitwise and */
	MONAMIC*256+RRET, "bin", 	/* loopup what function name is bound to a key */
	MONAMIC*256+RINT, "bno", 	/* bitwise not */
	DYNAMIC*256+RINT, "bor", 	/* bitwise or	 */
	DYNAMIC*256+RINT, "bxo", 	/* bitwise xor */
	DYNAMIC*256+RRET, "cat", 	/* concatenate string */
	MONAMIC*256+RRET, "chr", 	/* integer to char conversion */
	DYNAMIC*256+RRET, "dir",	/* replace tail of filename with filename */
	NILNAMIC*256+RRET,"dit",	/* the character in the line above */
	DYNAMIC*256+RINT, "div", 	/* division */
	MONAMIC*256+RRET, "env", 	/* retrieve a system environment var */
	DYNAMIC*256+RSTR, "equ", 	/* logical equality check */
	MONAMIC*256+RSTR, "exi", 	/* check if a file exists */
	MONAMIC*256+RRET, "fin", 	/* look for a file on the path... */
	DYNAMIC*256+RSTR, "gre", 	/* logical greater than */
	NILNAMIC*256+RRET,"gtc",	/* get 1 emacs command */
	NILNAMIC*256+RRET,"gtk",	/* get 1 charater */
	MONAMIC*256+RRET, "ind", 	/* evaluate indirect value */
	DYNAMIC*256+RRET, "lef", 	/* left len chars */
	MONAMIC*256+RINT, "len", 	/* string length */
	DYNAMIC*256+RSTR, "les", 	/* logical less than */
	MONAMIC*256+RRET, "low", 	/* lower case string */
	TRINAMIC*256+RRET,"mid",	/* mid string(string, pos, len) */
	DYNAMIC*256+RINT, "mod", 	/* mod */
	MONAMIC*256+RINT, "neg", 	/* negate */
	MONAMIC*256+RSTR, "not", 	/* logical not */
	DYNAMIC*256+RSTR, "or",		/* logical or */
	DYNAMIC*256+RRET, "rig", 	/* chars from pos */
	DYNAMIC*256+RSTR, "seq", 	/* string logical equality check */
	DYNAMIC*256+RSTR, "sgr", 	/* string logical greater than */
	DYNAMIC*256+RINT, "sin", 	/* find the index of one string in another */
	DYNAMIC*256+RSTR, "sle", 	/* string logical less than */
#if DIACRIT
	DYNAMIC*256+RRET, "slo",  /* set lower to upper char translation */
#endif
	DYNAMIC*256+RINT, "sub", 	/* subtraction */
#if DIACRIT
	DYNAMIC*256+RRET, "sup",		/* set upper to lower char translation */
#endif
	DYNAMIC*256+RINT, "tim", 	/* multiplication */
	MONAMIC*256+RRET, "tri",	/* trim whitespace off the end of a string */
	MONAMIC*256+RRET, "upp", 	/* uppercase string */
	TRINAMIC*256+RRET,"xla",		/* XLATE character string translation */
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
#define UFSEQUAL	32
#define UFSGREAT	33
#define UFSINDEX	34
#define UFSLESS 	35
#if DIACRIT
#define	UFSLOWER	36
#define UFSUB		  37
#define	UFSUPPER	38
#define UFTIMES 	39
#define	UFTRIM		40
#define UFUPPER 	41
#define UFXLATE 	42
#else
#define UFSUB		  36
#define UFTIMES 	37
#define	UFTRIM		38
#define UFUPPER 	39
#define UFXLATE 	40
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
#if 0
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
#endif
	"incldirs",		/* directories to search */
	"keycount",		/* consecutive times key has been pressed */
	"kill", 			/* kill buffer (read only) */
	"lastdir",		/* last direction of search */
	"lastkey",		/* last keyboard char struck */
	"lastmesg",		/* last string mlwrite()ed */
	"line", 			/* text of current line */
	"match",			/* last matched magic pattern */
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

#define NEVARS	(sizeof(g_envars) / sizeof(g_envars[0])+30)

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
0,		 /* EVINCLD */			/* */
0,     /* EVKEYCT */		  /* consec key ct */
CTRL |'G',/* EVKILL */  	/* actual: abortc- current abort command char*/
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


#ifdef _DEBUG
char * dbg_val(const char * banner, const char * val)

{ const char * dbg = gtusr("dbg");
	if (dbg && dbg[0] > '0')
	{ char buf[60];
		strcat(strcpy(buf, banner), val);
		mbwrite(buf);
	}
	return dbg;
}
#else
#define dbg_val(a,b) NULL
#endif


static
int USE_FAST_CALL need_realloc(int vnum)

{	if (vnum >= EVHLIGHT1)
		return TRUE;		

	switch (vnum)
	{ 
	  case EVINCLD:	 
	  case EVPALETTE:
	  case EVFILEPROF:
		case EVMATCH:
				return TRUE;
	}
	
	return FALSE;
}


void Pascal varinit()	/* initialize the user variable list */

{	int ix;
	for (ix = EVHLIGHT1+30 ; --ix >= 0; )
		if (need_realloc(ix))
			predefvars[ix] .p = mallocz(2);

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

static int USE_FAST_CALL absv(int x) /* take the absolute value of an integer */
	
{
  return x < 0 ? -x : x;
}

#endif

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
  while (1)
  { pline = lback(pline);
  	if (l_is_hd(pline))
  		break;
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
				 rc > 0	? tgt			 : NULL;
}}


int USE_FAST_CALL stol(const char * val)					/* convert a string to a numeric logical */
{
  return val[0] == 'T' || atoi(val) != 0;	/* check for logical values */	 
}


#define INTWIDTH (sizeof(int) * 3)

static char g_result[INTWIDTH+40];

				/* output integer as radix r */
char * Pascal USE_FAST_CALL int_radix_asc(int i, int radix, char fill)

{	static const char hexdigits[] = "0123456789ABCDEF";

	memset(g_result+1, fill, INTWIDTH);
	*(short*)&g_result[INTWIDTH+2] = 0;
//g_result[INTWIDTH+3] = 0;

{	char * sp = &g_result[INTWIDTH+2];		/* 13 places from g_result[1] */
	int v = i;

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


char * float_asc(MYFLOAT x)

{ char b_shift[6];
	char b_mid[41];
	int sign = 1;
	if (x < 0.0)
	{	x = -x;
		sign -= 2;
	}

#if _DEBUG
  if (dbg_val("SINIT ", int_asc((int)x)))
  {	sprintf(b_mid, "FLOAT_ASC %10.10lf", x);
  	mbwrite(b_mid);
  }
#endif

{ int shift = 0;
  while (x > 2000000000)
  { ++shift;
    x /= 10;
  }

	if (x >= 0.00000001 && x < 0.00001)
	{		//	shift -= 2;
		while (x < 1.0)
		{
			--shift;
			x *= 10;
		}
	}

	b_mid[0] = 0;
	b_shift[0] = !!shift * 'e';
{	char * shft = int_asc(shift);
	strcpy(b_shift+1, shft);
	
{	int val = (int)x;
  x -= val;
{ char * mid = int_asc(val*sign);
  strcpy(b_mid+1, mid);
{	int after = (int)(x * 1000000001.1F);
  char * aft = int_radix_asc(after, 10, '0');
	int nines = 0;
	int dix = 0;
	g_result[4] = '.';

{	int ix = strlen(strcat(b_mid+1, g_result+4));
//char last = b_mid[ix+1-1];
#if 1
	while (--ix >= 0)
	{	if (b_mid[ix+1] == '.')
	  {
		  dix = ix+1;
			if (b_mid[ix+2] != 0)
				continue;
			b_mid[ix+1] = 0;
		}
		if (b_mid[ix+1] == '0')
		{	if (nines + dix > 0)
				break;
			b_mid[ix+1] = 0;
		}
		else 
		{	++nines;
			if (b_mid[ix+1] != '9')
				break;
		}
	}

	if (nines > 6)			// five nines
	{	if (ix < 0)
			b_mid[0] = '1';

		b_mid[ix+1] += 1;
		
		if      (dix > 0)
		{ strcpy(b_mid+ix+1+1, "0000000000000");
		  ix = dix - 2;
		}	
		if (ix >= 0)
			b_mid[ix+1+1] = 0;
	}
}
#endif
{ char * res = concat(g_result, b_mid+!b_mid[0], b_shift, NULL);
  dbg_val("FA ", res);
	return res;
}}}}}}}


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


/*	structure to hold user variables and their definitions	*/

typedef struct UVAR
{	char u_name[NVSIZE + 1];	       /* name of user variable */
	char *u_value;				/* value (string) */
} UVAR;

/*	current user variables (This structure will probably change)	*/

static
UVAR NOSHARE g_uv[MAXVARS + 1];	/* user variables */

static int g_uv_vnum;


static
int Pascal var_index(int tag, char var[NVSIZE+1])
												/* name of variable to fetch */
{ int vix;

fvar:	vix = -1;
{	int wh = var[0] == '$';
	switch (tag)
	{ case '&': 
		    if (var[1] == 'i' && var[2] == 'n' && var[3] == 'd')// && g_macargs > 0)
		    {			  /* grab token, and eval it */
		      (void)token(var, NVSIZE+1);
		      getval(&var[0], var);
		      goto fvar;
		    }
		    vix = 0;
		otherwise
				if (var[wh] == 'h' && var[wh+1] == 'p')
				{ tag = atoi(var+wh+2)-1;
					if (in_range(tag, 0, 29))
						return (EVHLIGHT1 + tag) | (TKENV << 11);
				}
				vix = (TKENV << 11);

	  when '%':		  /* check for existing legal user variable */
	  	  (void)gtusr(&var[1]);
		  	vix = g_uv_vnum;
			  if (vix >= 0)
			  {	strcpy(g_uv[vix].u_name, &var[1]);
					return vix | (TKVAR << 11);
		    }				/* indirect operator? */
	}

	if (vix >= 0)
 		vix = binary_const(vix, &var[wh]) | vix;

	if (vix < 0)
	{ mlwrite(TEXT52, var);
					/* "%%No such variable as '%s'" */
	  return -1;
	}

	return vix;
}}


static 
const char * USE_FAST_CALL gtenvfun(char typ, char * fname)/* evaluate a var/function */

{ BUFFER * bp = curbp;
	int hookix = 0;

	signed char tk;
	int iarg1;
	int vnum = var_index(typ, fname);
	if (vnum < 0)
		return g_logm[2];

	if (typ == TOKFUN)
	{	int iarg2 = -1;
#if USE_FLOAT
		MYFLOAT farg1, farg2;
#endif
		int sl1;
		char * arg2 = NULL;
		char * arg1 = push_arg(0,"");					/* to initialise area */
		int type_kind = funcs[vnum].f_type_kind;
		dbg_val("TK ", int_asc(type_kind));

		if (type_kind >= MONAMIC)	/* retrieve the first argument */
		{ 
		  if (macarg(arg1) <= 0)
		    return null;

			sl1 = strlen(arg1);
													 /* if needed, retrieve the second and third arguments */
		  if (type_kind >= DYNAMIC*256)
		  { arg2 = push_arg(vnum, arg1);// arglen: length of arg1
				if (arg2 == NULL)
		      return null;

	      iarg2 = atoi(arg2);
#if USE_FLOAT	      
	      ATOLDBL(farg2, arg2);
#endif
		  }
		  iarg1 = atoi(arg1);
#if USE_FLOAT	      
		  ATOLDBL(farg1, arg1);
#endif
		}

//	if (vnum < 0)
//		return NULL;
		tk = type_kind;
																				/* and now evaluate it! */
		if      (tk < RINT)
		 switch (vnum)
		 {case UFDIT:		*(short*)arg1 = plinecpy();
		 								return arg1;
		  when UFRIGHT: if ((unsigned)iarg2 > (unsigned)sl1)
		  								iarg2 = sl1; 
		  							return strcpy(arg1, &arg1[iarg2]);
			when UFDIR:		return pathcat(arg1, NSTRING-1, arg1, arg2);
			when UFIND:		return getval(arg1, arg1);

			when UFLOWER:
			case UFUPPER:	return mkul(vnum - UFLOWER, arg1);
			when UFLEFT:	if (g_stk.lim - g_stk.top > iarg2)
			            	  arg1[iarg2] = 0;
										if (0)
			            	{
			when UFTRIM:    (void)trimstr(sl1,arg1);
											if (0)
											{ 
			when UFGTKEY:
			default: // UFCHR
												arg1[0] = vnum == UFGTKEY ? tgetc() : iarg1;
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
		else if (tk == RINT)
		{switch (vnum)
		 {case UFADD:	  	dbg_val("+A1 ", arg1);
											dbg_val("+A2 ", arg2);
		 									return X_ASC(FA1 + FA2);
			when UFSUB:	  	
											dbg_val("-A1 ", arg1);
										{	char * t = dbg_val("-A2 ", arg2);
#if _DEBUG
											MYFLOAT arg = FA1 - FA2;
											if (t)
											{	char buff[200];
											  sprintf(buff, "S_A %10.10lf  %10.10lf  %10.10lf", arg, FA1, FA2);
										  	mbwrite(buff);
										  }
#endif
											return X_ASC(FA1 - FA2);
										}
			when UFTIMES: 	return X_ASC(FA1 * FA2);
			when UFDIV:	  	return X_ASC(FA1 / FA2);
			when UFNEG:	  	return X_ASC(-FA1);
			when UFMOD:	  	iarg1 = iarg1 % iarg2;
			
		  when UFABS:			if (arg1 < 0)
		  									iarg1 = -iarg1;
			when UFBAND:		iarg1 &= iarg2;
			when UFBOR:	  	iarg1 |= iarg2;
			when UFBNOT:
			case UFBXOR:		iarg1 ^= iarg2;
			when UFASCII:		iarg1 = (int)arg1[0];
			when UFSINDEX:	iarg1 = sindex(arg1, arg2);
			when UFLENGTH:	iarg1  = sl1;
//		otherwise		  	return "";
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
	{	vnum &= 0xff;
		if (need_realloc(vnum))
			return predefvars[vnum].p;

#define result (&deltaf[NSTRING / 2])   /* leave beginning for extra safety */
	  iarg1 = predefvars[vnum].i;

		switch (vnum)
		{ case EVPAGELEN:  iarg1 = term.t_nrowm1 + 1;
		  when EVPAGEWIDTH:iarg1 = term.t_ncol;
		  when EVCURCOL:   iarg1 = getccol();
		  when EVCURLINE:	 iarg1 = setcline();
		  when EVHARDTAB:  iarg1 = bp->b_tabsize;
	//	when EVUSESOFTTAB:iarg1 = bp->b_mode & BSOFTTAB ? 1 : 0;
		  when EVCBFLAGS:  iarg1 = bp->b_flag;
		  when EVCBLANG:	 iarg1 = bp->b_langprops;;
		  when EVCBUFNAME: return bp->b_bname;
		  when EVCFNAME:   return bp->b_fname;
		  when EVCMODE:    iarg1 = iarg1 >> NUMFLAGS;
	//  when EVSRES:	   return sres;
	//  when EVPALETTE:  return palstr;
	//  when EVFILEPROF: return g_file_prof;
		  when EVCURCHAR:  iarg1 = llength(curwp->w_dotp) == curwp->w_doto 
	                    								? '\n' : lgetc(curwp->w_dotp, curwp->w_doto);
		  when EVWLINE:    iarg1 = curwp->w_ntrows;
		  when EVCWLINE:   iarg1 = getwpos();
		  when EVSEARCH:   return pat;
	//  when EVHIGHLIGHT:return highlight;
		  when EVREPLACE:  return rpat;
	//  when EVMATCH:    return patmatch;
		  when EVKILL:	   return getkill();
		  when EVREGION:   return getreg(&result[0]);
		  
		  when EVLINE:	   return getctext(&result[0]);
		  when EVLASTMESG: return strpcpy(result,lastmesg,NPAT);
	//  when EVFCOL:	   iarg1 = pd_fcol;

		  when EVBUFHOOK:
		  case EVEXBHOOK:  --hookix;
		  case EVWRITEHK:  --hookix;
		  case EVCMDHK:    --hookix;
		  case EVWRAPHK:   --hookix;
		  case EVREADHK:   --hookix;
							   		   return getfname(hookix);
		  when EVVERSION:  return VERSION;
#if 0		  
		  when EVLANG:	   return LANGUAGE;
#endif
	    when EVZCMD:     return g_ll.lastline[g_ll.ll_ix & MLIX];
#if S_WIN32
		  when EVWINTITLE: return null;	// getconsoletitle();
#endif
			when EVWORK:		 iarg1 = lastbuffer(0,0) & 0xff;
		  when EVPENDING:
#if	GOTTYPAH
										   iarg1 = typahead();
#endif
		  case EVDEBUG:   
		  case EVSTATUS:  
		  case EVDISCMD:  
		  case EVDISINP:  
		  case EVSSAVE:    return ltos(iarg1);

		  default:	       loglog2("Var %d = %x", vnum, iarg1);
		}
		return int_asc(iarg1);
#undef result
	}

	return int_asc(iarg1);
}


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


/*static*/
int Pascal svar(int var, const char * value)	/* set a variable */

{	if (value == NULL)
		return FALSE;

{ int val = atoi(value);
  int hookix = 0;		  /* set an environment variable */

{	BUFFER * bp = curbp;
	Cc cc = TRUE;
  int vnum = var & 0x7ff;
  char ** varref = &predefvars[vnum].p;

  if (var - vnum == (TKVAR << 11))
	{ varref = &g_uv[vnum].u_value;
  	goto remalloc;
  }

	if (vnum >= EVHLIGHT1)
		goto remalloc;

  switch (vnum) 
  {
#if S_WIN32
//  when EVWINTITLE: setconsoletitle( value );
#endif
//	when EVUSESOFTTAB:bp->b_mode &= ~BSOFTTAB;
//									 if (val)
//										 bp->b_mode |= BSOFTTAB;
	  when EVPAGELEN:	 newdims(0, val);
	  when EVPAGEWIDTH:newdims(val, 0);
	  when EVCURCOL:	 (void)setccol(val);
	  when EVCURLINE:	 cc = gotoline(TRUE, val);
	  when EVCBFLAGS:	 lchange(WFMODE);
                   /*if (val & BFCHG)
                       mbwrite("EVAL");*/
	  case EVCMODE:
	      						 hookix = bp->b_flag;
	       						 if (vnum == EVCBFLAGS)
	   	  						   hookix = (hookix & ~(BFCHG|BFINVS))
										          | (val & (BFCHG|BFINVS));
		    						 else
		    						 { hookix &= MDSTT - 1;
			  						   hookix |= val << NUMFLAGS;
			  						   curwp->w_flag |= WFMODE;
		 	  						 }
			  						 bp->b_flag = hookix;
//	when EVSRES:	   cc = TTrez(value);

	  when EVCURCHAR:	 ldelchrs(1, FALSE, FALSE);		/* delete 1 char */
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
	  when EVCFNAME:	 	 repl_bfname(bp, value);
			               curwp->w_flag |= WFHARD | WFMODE;
	  when EVBUFHOOK: ++hookix;
	  case EVEXBHOOK: ++hookix;
	  case EVWRITEHK: ++hookix;
	  case EVCMDHK:   ++hookix;
	  case EVWRAPHK:  ++hookix;
    case EVREADHK:  hooks[hookix].k_code = 1;
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
	  case EVSSAVE:		val = stol(value);
										goto storeint;

		when EVHARDTAB:	if (val != 0)
                 	    bp->b_tabsize = val;

	                  upwind(TRUE);
//									goto storeint;
										if (0)
    when EVHJUMP:   if (val > term.t_ncol - 1)
				              val = term.t_ncol - 1;
    default:
storeint:
		predefvars[vnum].i = val;
  }

  return cc;

remalloc:
	return remallocstr(varref, value, 0) != null;
}}}



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
{	int vix = var_index(var[0],var);
	if (vix < 0)
		return vix;

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
const char *Pascal getval(char * tgt, const char * src)
														/* token: token to evaluate */
{	int blen = NSTRING-1;
//const char * src = (const char *)s;
	char * srcp1 = src + 1;

	switch (*src)
	{ case TOKARG:															/* interactive argument */
						if (src[1] == TOKARG)
						{	++src;
							blen = 0;
						}
						getval(&src[0], &src[1]);
			      ++pd_discmd;							/* echo it always! */
					{	Cc cc = getstring(&tgt[0], blen, src);
						--pd_discmd;
							return cc < 0 ? getvalnull : tgt;
			    }
	  case TOKBUF:															/* buffer contents fetch */
					{ BUFFER * bp = bfind(getval(tgt, srcp1), FALSE);
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
	  when TOKVAR:  src = gtusr(srcp1);
									if (src == NULL)
									  src = g_logm[2];	// "ERROR"

	  when TOKFUN: srcp1[3] = 0;				    /* look the function up in the function table */
								 (void)mkul(0, srcp1);		/* only first 3 chars significant */
	  	
	  case TOKENV: src = gtenvfun(*src, srcp1);
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

#if 0

int Pascal ernd()	/* returns a random integer */

{	pd_seed = absv(pd_seed * 1721 + 10007);
	return pd_seed;
}

#endif

static
void fmt_desv(char pfx, const char * v, const char * val)
	
{					/* and add it as a line to the buffer */
#if NVSIZE > 20
error error
#endif
  (void)mlwrite("%!%>%c%12s %s\n", pfx, v, val);
}



int Pascal mkdes()

{ curbp->b_flag |= MDVIEW;
  curbp->b_flag &= ~BFCHG;										/* don't flag this as a change */
  curwp->w_dotp = lforw(&curbp->b_baseline);     /* back to the beginning */
  curwp->w_doto = 0;
  curwp->w_line_no = 1;
#if S_MSDOS == 0
	mlerase();					/* clear the mode line */
	lastmesg[0] = 0;
#endif

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
	char buf[6];
	BUFFER * bp = curbp;
	buf[0] = 'h';
	buf[1] = 'p';
        
  openwindbuf(TEXT56);
//curbp->b_tabsize = 0;
	curbp->b_flag |= BFCHG;										/* Suppress the beep */
  
//--pd_discmd;
				      												/* build the environment variable list */
  for (uindex = -1; ++uindex < NEVARS; )
  {	BUFFER * sbp = curbp;
  	curbp = bp;
		strcpy(buf+2, int_asc(uindex - EVHLIGHT1 + 1));
	{ const char * nm = uindex <= EVZCMD ? g_envars[uindex] : buf;
  	const char * val = gtenvfun(TOKENV, nm);
    curbp = sbp;
    fmt_desv('$', nm, val);
  }}

  lnewline(1);
					    /* build the user variable list */
	for (uindex = MAXVARS; --uindex >= 0 && g_uv[uindex].u_name[0] != 0; )
    fmt_desv('%', g_uv[uindex].u_name, g_uv[uindex].u_value);

//++pd_discmd;

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

/*	Filter a string through a translation table	*/
static
char *Pascal xlat(char * srctgt, char * lookup, char * trans)
	/* char *srctgt;	* string to filter */
	/* char *lookup;	* characters to translate */
	/* char *trans;		* resulting translated characters */
{
	int six, tix = -1;

	for (six = -1; ; )
	{
retry:
	{	char lch;
	  char ch = srctgt[++six];
		int lix;
	
		for (lix = -1; (lch = lookup[++lix]) != 0; )
			if (ch == lch)
	  	{ if (trans[0] == 0)
	   			goto retry;
	   		
		 		ch = trans[lix];
		 		break;
		 	}

	  srctgt[++tix] = ch;
	  if (ch == 0)
	  	break;
	}}

	return srctgt;
}

