/*	EVAR.H: Environment and user variable definitions
		for MicroEMACS

		written 1986 by Daniel Lawrence
*/

/*	list of recognized environment variables	*/

NOSHARE const char * g_envars[] = {
	"acount",			/* # of chars until next auto-save */
	"asave",			/* # of chars between auto-saves */
	"bufhook",		/* enter buffer switch hook */
	"cbflags",		/* current buffer flags */
	"cbufname",		/* current buffer name */
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
	"diagflag",		/* diagonal mouse movements enabled? */
	"discmd",			/* display commands on command line */
	"disinp",			/* display command line input characters */
	"exbhook",		/* exit buffer switch hook */
	"fcol",				/* first displayed column in curent window */
	"fileprof",		/* profiles for file types */
	"fillcol",		/* current fill column */
	"gflags",			/* global internal emacs flags */
	"gmode",			/* global modes */
	"hardtab",		/* current hard tab size */
	"highlight",	/* highlighting string */
	"hjump",			/* horizontal screen jump size */
	"hscroll",		/* horizontal scrolling flag */
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
	"sterm",			/* search terminator character */
	"uarg",				/* last universal arg */
	"version",		/* current version number */
	"wintitle",		/* the title on the window */
	"wline",			/* # of lines in current window */
	"wraphook",		/* wrap word execution hook */
	"writehook",	/* write file hook */
	"xpos", 			/* current mouse X position */
	"ypos",	 			/* current mouse Y position */
	"zcmd"				/* last command */
};

#define NEVARS	(sizeof(g_envars) / sizeof(g_envars[0]))


/*	list of recognized user functions	*/

typedef struct UFUNC
{	char  f_type;	/* 1 = monamic, 2 = dynamic */
	char  f_kind;
	char  f_name[4];  /* name of function */
} UFUNC;

#define NILNAMIC	0
#define MONAMIC 	1
#define DYNAMIC 	2
#define TRINAMIC	3

#define RRET (-1)
#define RINT  0
#define RSTR  1

NOSHARE const UFUNC funcs[] = {
	MONAMIC, RINT, "abs", 	/* absolute value of a number */
	DYNAMIC, RINT, "add",   /* add two numbers together */
	DYNAMIC, RSTR, "and", 	/* logical and */
	MONAMIC, RINT, "asc", 	/* char to integer conversion */
	DYNAMIC, RINT, "ban", 	/* bitwise and	 9-10-87  jwm */
	MONAMIC, RRET, "bin", 	/* loopup what function name is bound to a key */
	MONAMIC, RINT, "bno", 	/* bitwise not */
	DYNAMIC, RINT, "bor", 	/* bitwise or	 9-10-87  jwm */
	DYNAMIC, RINT, "bxo", 	/* bitwise xor	 9-10-87  jwm */
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
	DYNAMIC, RRET, "slo",		/* set lower to upper char translation */
	DYNAMIC, RINT, "sub", 	/* subtraction */
	DYNAMIC, RRET, "sup",		/* set upper to lower char translation */
	DYNAMIC, RINT, "tim", 	/* multiplication */
	MONAMIC, RRET, "tri",		/* trim whitespace off the end of a string */
	MONAMIC, RRET, "upp", 	/* uppercase string */
	TRINAMIC,RRET, "xla",	/* XLATE character string translation */
};

#define NFUNCS	sizeof(funcs) / sizeof(UFUNC)

/*	and its preprocesor definitions 	*/

#define UFABS		0
#define UFADD		1
#define UFAND		2
#define UFASCII 3
#define UFBAND	4
#define UFBIND	5
#define UFBNOT	6
#define UFBOR		7
#define UFBXOR	8
#define UFCAT		9
#define UFCHR		10
#define UFDIR	  11
#define UFDIT		12
#define UFDIV		13
#define UFENV		14
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
#define UFMID		26
#define UFMOD		27
#define UFNEG		28
#define UFNOT		29
#define UFOR		30
#define UFRIGHT 31
#define UFRND		32
#define UFSEQUAL	33
#define UFSGREAT	34
#define UFSINDEX	35
#define UFSLESS 	36
#define	UFSLOWER	37
#define UFSUB		  38
#define	UFSUPPER	39
#define UFTIMES 	40
#define	UFTRIM		41
#define UFUPPER 	42
#define UFXLATE 	43
