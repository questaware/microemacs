/*	This file is for functions having to do with key bindings,
		descriptions, and help commands.

	written 11-feb-86 by Daniel Lawrence
*/
#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<sys/stat.h>

#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"map.h"
#include	"msdir.h"
#include	"logmsg.h"

extern char *getenv();
extern char * g_invokenm;

#if 0

static int Pascal cbuf1(int f, int n)  { return execporb(-1,n); }
static int Pascal cbuf2(int f, int n)  { return execporb(-2,n); }
static int Pascal cbuf3(int f, int n)  { return execporb(-3,n); }
static int Pascal cbuf4(int f, int n)  { return execporb(-4,n); }
static int Pascal cbuf5(int f, int n)  { return execporb(-5,n); }
static int Pascal cbuf6(int f, int n)  { return execporb(-6,n); }
static int Pascal cbuf7(int f, int n)  { return execporb(-7,n); }
static int Pascal cbuf8(int f, int n)	 { return execporb(-8,n); }
static int Pascal cbuf9(int f, int n)  { return execporb(-9,n); }
static int Pascal cbuf10(int f, int n) { return execporb(-10,n); }
static int Pascal cbuf11(int f, int n) { return execporb(-11,n); }
static int Pascal cbuf12(int f, int n) { return execporb(-12,n); }
static int Pascal cbuf13(int f, int n) { return execporb(-13,n); }
static int Pascal cbuf14(int f, int n) { return execporb(-14,n); }
static int Pascal cbuf15(int f, int n) { return execporb(-15,n); }
static int Pascal cbuf16(int f, int n) { return execporb(-16,n); }
static int Pascal cbuf17(int f, int n) { return execporb(-17,n); }
static int Pascal cbuf18(int f, int n) { return execporb(-18,n); }
static int Pascal cbuf19(int f, int n) { return execporb(-19,n); }
static int Pascal cbuf20(int f, int n) { return execporb(-20,n); }
static int Pascal cbuf21(int f, int n) { return execporb(-21,n); }
static int Pascal cbuf22(int f, int n) { return execporb(-22,n); }
static int Pascal cbuf23(int f, int n) { return execporb(-23,n); }
static int Pascal cbuf24(int f, int n) { return execporb(-24,n); }
static int Pascal cbuf25(int f, int n) { return execporb(-25,n); }
static int Pascal cbuf26(int f, int n) { return execporb(-26,n); }
static int Pascal cbuf27(int f, int n) { return execporb(-27,n); }
static int Pascal cbuf28(int f, int n) { return execporb(-28,n); }
static int Pascal cbuf29(int f, int n) { return execporb(-29,n); }
static int Pascal cbuf30(int f, int n) { return execporb(-30,n); }
static int Pascal cbuf31(int f, int n) { return execporb(-31,n); }
static int Pascal cbuf32(int f, int n) { return execporb(-32,n); }
static int Pascal cbuf33(int f, int n) { return execporb(-33,n); }
static int Pascal cbuf34(int f, int n) { return execporb(-34,n); }
static int Pascal cbuf35(int f, int n) { return execporb(-35,n); }
static int Pascal cbuf36(int f, int n) { return execporb(-36,n); }
static int Pascal cbuf37(int f, int n) { return execporb(-37,n); }
static int Pascal cbuf38(int f, int n) { return execporb(-38,n); }
static int Pascal cbuf39(int f, int n) { return execporb(-38,n); }
static int Pascal cbuf40(int f, int n) { return execporb(-38,n); }

#endif

#include	"efunc.h"	/* function declarations and name table */

/*
 * Command table.
 * This table  is *roughly* in ASCII order, left to right across the
 * characters of the command. This explains the funny location of the
 * control-X commands.
 */
#define PERSONAL_KEYS 1

//--------------------------------------------------------------------------

NOSHARE int g_numcmd = NCMDS;	/* number of bindable functions */

static KEYTAB keytab[NBINDS+1] = 
{
	{CTRL|'A',	BINDFNC, gotobol},
	{CTRL|'B',	BINDFNC, backchar},
	{CTRL|'E',	BINDFNC, gotoeol},
	{CTRL|'H',	BINDFNC, backdel},
	{CTRL|'I',	BINDFNC, handletab},
	{CTRL|'J',	BINDFNC, indent},
	{CTRL|'K',	BINDFNC, killtext},
	{CTRL|'L',	BINDFNC, refresh},
	{CTRL|'M',	BINDFNC, ins_newline},
	{CTRL|'N',	BINDFNC, forwline},
#if FLUFF
	{CTRL|'O',	BINDFNC, openline},
#endif
	{CTRL|'P',	BINDFNC, backline},
	{CTRL|'Q',	BINDFNC, quote},
	{CTRL|'S',	BINDFNC, forwsearch},
	{CTRL|'U',	BINDFNC, uniarg},
//{CTRL|'X',	BINDFNC, cex},
	{CTRL|'Z',	BINDFNC, backpage},
//{CTRL|'[',	BINDFNC, meta},
	{CTLX|CTRL|'B', BINDFNC, listbuffers},
	{CTLX|CTRL|'C', BINDFNC, quit},
#if AEDIT
	{CTLX|CTRL|'D', BINDFNC, detab},
	{CTLX|CTRL|'E', BINDFNC, entab},
#endif
	{CTLX|CTRL|'F', BINDFNC, filefind},
	{CTLX|CTRL|'I', BINDFNC, insfile},
//{CTLX|CTRL|'K', BINDFNC, macrotokey},
	{CTLX|CTRL|'L', BINDFNC, lowerregion},
	{CTLX|CTRL|'M', BINDFNC, delmode},
	{CTLX|CTRL|'N', BINDFNC, mvdnwind},
	{CTLX|CTRL|'P', BINDFNC, mvupwind},
	{CTLX|CTRL|'R', BINDFNC, fileread},
	{CTLX|CTRL|'S', BINDFNC, filesave},
#if AEDIT
	{CTLX|CTRL|'T', BINDFNC, trim_white},
#endif
	{CTLX|CTRL|'U', BINDFNC, upperregion},
	{CTLX|CTRL|'W', BINDFNC, filewrite},
#if FLUFF
	{CTLX|CTRL|'X', BINDFNC, swapmark},
#endif
	{CTLX|CTRL|'Z', BINDFNC, shrinkwind},
	{CTLX|CTRL|'V', BINDFNC, viewfile},
	{CTLX|'?',	BINDFNC, deskey},
	{CTLX|'!',	BINDFNC, spawn},
//{CTLX|'$',	BINDFNC, spawn},
	{CTLX|'#',	BINDFNC, filter},
	{CTLX|'=',	BINDFNC, showcpos},
	{CTLX|'(',	BINDFNC, ctlxlp},
	{CTLX|')',	BINDFNC, ctlxrp},
	{CTLX|'<',	BINDFNC, narrow},
	{CTLX|'>',	BINDFNC, widen},
	{CTLX|'^',	BINDFNC, enlargewind},
	{CTLX|' ',	BINDFNC, remmark},
	{CTLX|'0',	BINDFNC, delwind},
	{CTLX|'1',	BINDFNC, onlywind},
	{CTLX|'2',	BINDFNC, splitwind},
	{CTLX|'B',	BINDFNC, usebuffer},
#if S_BSD
	{CTLX|'D',	BINDFNC, bktoshell},
#endif
/*	{CTLX|'F',	BINDFNC, abort},*/
#if DEBUGM
	{CTLX|'G',	BINDFNC, dispvar},
#endif
	{CTLX|'K',	BINDFNC, killbuffer},
	{CTLX|'M',	BINDFNC, togmod},
	{CTLX|'N',	BINDFNC, filename},
	{CTLX|'O',	BINDFNC, nextwind},
	{CTLX|'P',	BINDFNC, prevwind},
#if ISRCH
	{CTLX|'R',	BINDFNC, risearch},
	{CTLX|'F',	BINDFNC, fisearch},
#endif
	{CTLX|'W',	BINDFNC, resize},
	{CTLX|'X',	BINDFNC, nextbuffer},
	{CTLX|'Z',	BINDFNC, enlargewind},
	{CTLX|'@',	BINDFNC, pipecmd},
	{META|CTRL|'R', BINDFNC, qreplace},
				/* personal keys */
#if PERSONAL_KEYS == 0
	{CTRL|'C',	BINDFNC, insspace},
	{CTRL|'D',	BINDFNC, forwdel},
	{CTLX|'E',	BINDFNC, ctlxe},
	{CTRL|'F',	BINDFNC, ctrlg},
	{CTRL|'G',	BINDFNC, ctrlg},
#if CFENCE
	{META|CTRL|'F', BINDFNC, getfence},
#endif
	{CTRL|'R',	BINDFNC, backsearch},
#if FLUFF
	{CTRL|'T',	BINDFNC, twiddle},
#endif
	{CTRL|'R',	BINDFNC, backsearch},
	{CTRL|'W',	BINDFNC, killregion},
	{CTRL|'V',	BINDFNC, forwpage},
	{CTRL|'Y',	BINDFNC, yank},
	{CTLX|'C',	BINDFNC, spawncli},
#else
/*
bind-to-key execute-macro ^W

bind-to-key goto-matching-fence 
bind-to-key search-forward ^S
bind-to-key search-reverse M-S
bind-to-key query-replace-string 
bind-to-key replace-string M-R

bind-to-key kill-region ^XD
bind-to-key kill-region FN^D
bind-to-key copy-region ^XC
bind-to-key kill-kill ^G
bind-to-key kill-kill ^F
bind-to-key yank ^V

bind-to-key shell-command ^X!
bind-to-key i-shell ^X%
;bind-to-key pipe-command ^X<
bind-to-key filter-buffer ^X\
bind-to-key filter-buffer ^X|
*/
	{CTRL|'W', BINDFNC, ctlxe},
#if CFENCE
	{CTRL|'T', BINDFNC, getfence},
#endif
	{CTRL|'R', BINDFNC, qreplace},
/*{CTRL|'F', BINDFNC, forwsearch}, */
	{META|'S', BINDFNC, backsearch},
	{CTLX|'D', BINDFNC, killregion},
	{SPEC|CTRL|'D', BINDFNC, killregion},
	{CTLX|'C', BINDFNC, copyregion},
	{CTRL|'G', BINDFNC, kdelete}, /* tbd: freed up ^G to agree with jasspa */
	{CTRL|'F', BINDFNC, kdelete},
	{CTRL|'V', BINDFNC, yank},
	{CTLX|'%', BINDFNC, spawncli},
	{CTLX|'\\', BINDFNC, filter},
	{CTLX|'|', BINDFNC, filter},
	{CTRL|' ', BINDFNC, setmark},
#endif

#if WORDPRO
	{META|CTRL|'C', BINDFNC, wordcount},
#endif
#if NMDPROC
	{META|CTRL|'E', BINDFNC, execproc},
#endif
	{META|CTRL|'G', BINDFNC, gotomark},
	{META|CTRL|'H', BINDFNC, delbword},
	{META|CTRL|'K', BINDFNC, unbindkey},
	{META|CTRL|'L', BINDFNC, reposition},
	{META|CTRL|'M', BINDFNC, delgmode},
#if 0
	{META|CTRL|'N', BINDFNC, namebuffer},  /* no longer supported */
#endif
	{META|CTRL|'S', BINDFNC, execfile},
	{META|CTRL|'V', BINDFNC, nextdown},
#if WORDPRO && 0
	{META|CTRL|'W', BINDFNC, killpara},
#endif
#if FLUFF
	{META|CTRL|'X', BINDFNC, execcmd},
#endif
	{META|CTRL|'Z', BINDFNC, nextup},
	{META|' ',	BINDFNC, setmark},
	{META|'?',	BINDFNC, help},
#if FLUFF
	{META|'!',	BINDFNC, reposition},
#endif
	{META|'=',	BINDFNC, setvar},
	{META|'>',	BINDFNC, gotoeob},
	{META|'<',	BINDFNC, gotobob},
	{SPEC|CTRL|'<', BINDFNC, gotobob},
	{META|'~',	BINDFNC, unmark},
#if APROP
	{META|'A',	BINDFNC, apro},
#endif
	{META|'B',	BINDFNC, backword},
	{ALTD|'W',	BINDFNC, copyword},
	{META|'D',	BINDFNC, delfword},
#if CRYPT
	{META|'E',	BINDFNC, setuekey},
#endif
	{META|'G',	BINDFNC, gotoline},
	{META|'K',	BINDFNC, bindtokey},
	{META|'L',	BINDFNC, lowerword},
	{META|'M',	BINDFNC, setgmode},
#if WORDPRO
	{META|'N',	BINDFNC, gotoeop},
	{META|'P',	BINDFNC, gotobop},
	{META|'Q',	BINDFNC, fillpara},
#endif
	{META|'R',	BINDFNC, sreplace},
#if S_BSD
	{META|'S',	BINDFNC, bktoshell},
#endif
	{META|'U',	BINDFNC, upperword},
	{META|'V',	BINDFNC, backpage},
	{META|'W',	BINDFNC, copyregion},
	{META|'X',	BINDFNC, namedcmd},
#if S_MSDOS == 0
	{META|'Z',	BINDFNC, quickexit},
#endif
//{META|0x7F, BINDFNC, delbword},
#if MOUSE
	{MOUS|'a',	BINDFNC, movemd},
	{MOUS|'b',	BINDFNC, movemu},
	{MOUS|'e',	BINDFNC, mregdown},
	{MOUS|'f',	BINDFNC, mregup},
	{MOUS|'1',	BINDFNC, resizm},
#endif 
	{ALTD|'S',	BINDFNC, forwhunt},
	{ALTD|'R',	BINDFNC, backhunt},
	{ALTD|'Z',	BINDFNC, wordsearch},
	{META|',',	BINDFNC, indentsearch},
	{SPEC|'<',	BINDFNC, gotobob},
	{SPEC|'P',	BINDFNC, backline},
	{META|SPEC|'P',BINDFNC, searchIncls},
	{META|SPEC|'N',BINDFNC, nextwind},
	{SPEC|'Z',	BINDFNC, backpage},
	{SPEC|'B',	BINDFNC, backchar},
	{SPEC|'F',	BINDFNC, forwchar},
	{SPEC|'>',	BINDFNC, gotoeol},
	{SPEC|'N',	BINDFNC, forwline},
	{SPEC|'V',	BINDFNC, forwpage},
//{SPEC|'C',	BINDFNC, insspace},
	{SPEC|'D',	BINDFNC, forwdel},
	{SPEC|CTRL|'B', BINDFNC, backword},
	{SPEC|CTRL|'F', BINDFNC, nextword},
	{ALTD|SPEC|'P', BINDFNC, mvupwind},
	{ALTD|SPEC|'N', BINDFNC, mvdnwind},
#if WORDPRO
	{SPEC|CTRL|'Z', BINDFNC, gotobop},
	{SPEC|CTRL|'V', BINDFNC, gotoeop},
#endif
	{SPEC|SHFT|'1', BINDFNC, (Command)1},		// Shift 
	{SPEC|SHFT|'2', BINDFNC, (Command)2},
	{SPEC|SHFT|'3', BINDFNC, (Command)3},
	{SPEC|SHFT|'4', BINDFNC, (Command)4},
	{SPEC|SHFT|'5', BINDFNC, (Command)5},
	{SPEC|SHFT|'6', BINDFNC, (Command)6},
	{SPEC|SHFT|'7', BINDFNC, (Command)7},
	{SPEC|SHFT|'8', BINDFNC, (Command)8},
	{SPEC|SHFT|'9', BINDFNC, (Command)9},

#if S_VMS
	{0x7F,		BINDFNC, backdel},
#else
	{0x7F,		BINDFNC, backdel}, // was forw
#endif

	{0, 	0, NULL}
};

//--------------------------------------------------------------------------

#if NBINDS == 0
static KEYTAB * oflowkeytab;
static int oflowtabsize = -1;
static int oflowcursize = -1;

static const KEYTAB null_keytab = {0,0};


static KEYTAB * aux_getbind;		/* only valid if result != null */
#endif

/*	GETCMD: Get a command from the keyboard. Process all applicable
		prefix keys
*/
int  getcmd()

{ int c = getkey();
	int pfx = c == (CTRL|'[') ? META :
						c == (CTRL|'X') ? CTLX : 0;
	if (pfx != 0)
		c = getkey();
																									/* Force to upper */
	return pfx | (!in_range(c & 0xff,'a','z') ? c : c & ~0x20);
}


									/* This function looks a key binding up in the binding table */
KEYTAB * Pascal getbind(int c)
				/* key to find what is bound to it */
{	if (c < 0)
		return &hooks[-c];
{		
#if NBINDS
  KEYTAB *ktp;
  short  code;
/*
  if (c == (CTRL | SPEC | 'N'))
    adb(44);
*/
  for (ktp = &keytab[0]-1; (code = (++ktp)->k_code) != 0 && code != c; )
    ;

  return ktp;

#else
  KEYTAB * vkeytab = oflowkeytab;
  int ix = oflowcursize;
  int ch = c;

  while (true)
  {
    while (--ix >= 0)
      if (ch == (vkeytab[ix].k_code & (NOTKEY-1)))
      { aux_getbind = &vkeytab[ix];
        if (aux_getbind->k_ptr.fp == NULL)
          return &null_keytab;
        return aux_getbind;
      }

    if (vkeytab == keytab)
      break;

    ix = upper_index(keytab)+1;
    vkeytab = keytab;
  }

  return &null_keytab;
#endif
}}



int Pascal addnewbind(int c, int (Pascal *func)(int, int))
					/* key to find what is bound to it */
{ /*if (c == (CTRL | SPEC | 'N'))
      adb(4);*/

{ KEYTAB * ktp = getbind(c);

#if NBINDS
  if (ktp == &keytab[NBINDS])
  { mlwrite(TEXT94);
					/* "[TABLE OVERFLOW]" */
    return FALSE;
  }

#else
  if (ktp == &null_keytab ||
      in_range(ktp - keytab, 0, upper_index(keytab))
     )
  { if (oflowcursize >= oflowtabsize)
    {
      if (oflowcursize < 0)
        oflowcursize = 0;
        
      oflowtabsize = oflowcursize + 50;
    { KEYTAB * tab = (KEYTAB*)remallocstr((char**)&oflowkeytab, 
				         													(const char *)oflowkeytab, 
																				  sizeof(KEYTAB)*oflowtabsize);
      if (tab == NULL)
      { mlwrite(TEXT94);
							/* "[TABLE OVERFLOW]" */
        return FALSE;
      }
    }}
    ktp = &oflowkeytab[oflowcursize++];
  }
#endif
  ktp->k_code   = c;
  ktp->k_type   = BINDFNC;
  ktp->k_ptr.fp = func;
	if (func == ctrlg)						/* if the function is a unique prefix key */
	 	abortc = c;								  /* reset the appropriate global prefix variable*/

  return TRUE;
}}


typedef struct
{ short  code;
	char   key[2];
} Vis_t;

static Vis_t viskeys[6+MOUSE] = { {CTRL, "^"},
																	{SPEC, "FN"},
																	{META, "M-"},
#if MOUSE
																	{MOUS, "MS"},
#endif
																	{SHFT, "S-"},
																	{ALTD, "A-"},
																	{CTLX, "^X"} };

//static const short viscod[7] = {CTLX, ALTD, SHFT, MOUS, META, SPEC, CTRL};
//static const char  viskey[14] = "^XA-S-MSM-FN^";		// assume aligned

												/* change a key command to a string we can print out */
char * Pascal cmdstr(char * t, int c) 
				/* destination string for sequence */
				/* sequence to translate */
{
	char * ptr = t;
	int i;
	for (i = sizeof(viskeys)/sizeof(viskeys[0]); --i >= 0; )
    if (c & viskeys[i].code)
    { ((short*)ptr)[0] = *(short *)&viskeys[i].key;

      ptr += 2;
      if (i == 0)
        ptr -= 1;
    }
					/* and output the final sequence */
	ptr[0] = c & 255;	/* strip the prefixes */
	ptr[1] = 0;
	return t;
}



/*	stock() 	String key name TO Command Key

	A key binding consists of one or more prefix functions followed by
	a keystroke.  Allowable prefixes must be in the following order:

	^X	preceeding control-X
	A-	simeltaneous ALT key (on PCs mainly)
	S-	shifted function key
	MS	mouse generated keystroke
	M-	Preceding META key
	FN	function key
	^	control key

	Meta and ^X prefix of lower case letters are converted to upper
	case.  Real control characters are automatically converted to
	the ^A form.
*/

unsigned int Pascal stock(char * keyname)
				/* name of key to translate to Command key form */
{
  unsigned int c = 0;
  int i;

  if (in_range(keyname[0], '0','9'))
    return keyname[0]-'0';
  
	for (i = sizeof(viskeys)/sizeof(viskeys[0]); --i > 0; )
    if (keyname[0] == viskeys[i].key[0] && keyname[1] == viskeys[i].key[1])
    { c |= viskeys[i].code;
      keyname += 2;
    }
				    /* a control char?	(Always upper case) */
  if (keyname[0] == '^' && keyname[1] != 0)
  { c |= CTRL;
    ++keyname;
  }
				
  if(!(c & (MOUS|SPEC|ALTD|SHFT))) /* If not a special key */
  {	if (*keyname < 32)						 /* A literal control character? (Boo, hiss) */
	  { c |= CTRL;
  	  *keyname += '@';
  	}
	}

  if (c & (CTRL|CTLX|META))
    mkul(1, keyname);							/* Make sure it's upper case */

  return c | *keyname & 255;
}


static
int Pascal getechockey(int mode)

{ char outseq[20];
	char tok[NSTRING];
								/* check to see if we are executing a command line */
	int c;
	if (g_clexec <= 0)
		c = mode & 1 ? getkey() : getcmd();
	else
	{ macarg(tok);	/* get the next token */
	  c = stock(tok);
	}
																	/* change it to something printable */
  if ((mode & 2) == 0)
  	mlwrite("\001 %s",cmdstr(&outseq[0], c));	// can be overwritten!

  return c;
}



			/* bindtokey: add a new key to the key binding table */
int Pascal bindtokey(int f, int n)
									/* int f, n;	** command arguments [IGNORED] */
{
	int (Pascal *kfunc)(int, int) = getname(TEXT15);
																				/* ": bind-to-key " */
	if (kfunc == NULL)
	{ mlwrite(TEXT16);
				  /* "[No such function]" */
	  return FALSE;
	}

{	int c = getechockey((kfunc == ctrlg));

	return addnewbind(c, kfunc); /* search the table to see if it exists */
}}

			/* unbindkey: delete a key from the key binding table */

Pascal unbindkey(int f, int n)
							/* int f, n;	** command arguments [IGNORED] */
{
  if (g_clexec <= 0)		/* prompt the user to type in a key to unbind */
    mlwrite(TEXT18);
				 /* ": unbind-key " */

{ int c = getechockey(FALSE);					/* get the command sequence to unbind */

  KEYTAB *sktp = getbind(c); 					/* search table to see if it exists */

  if (sktp->k_code == 0)
  { mlwrite(&TEXT19[1]);
					/* "[Key not bound]" */
    return FALSE;
  }
{
#if NBINDS
  KEYTAB * ktp = getbind(0) - 1;		/* get pointer to end of table */
			           										/* copy the last entry to the current one */
  *sktp = *ktp;
  *ktp = ktp[1];										/* null out the last one */

#else
  if (in_range(sktp - keytab, 0, upper_index(keytab)))
    addnewbind(c, NULL);
  else
    sktp->k_ptr.fp = NULL;
#endif
  return TRUE;
}}}

#if 0
  What does this do (NOTKEY ?)?

	      /* macrotokey: Bind a key to a macro in the key binding table */
int Pascal macrotokey(int f, int n)
							 /* int f, n;	 ** command arguments [IGNORED] */
{
	char bufn[NBUFN+2];								/* buffer to hold macro name */
	char outseq[NBUFN+20]; 
																				/* ": macro-to-key " */
{	int cc = mlreply(strcpy(outseq, TEXT215), &bufn[1], NBUFN-2);
	if (cc != TRUE)
	  return cc;
																     /* build the response string for later */
	strcat(outseq, &bufn[1]);
	bufn[0] = '[';
{	BUFFER *kmacro = bfind(strcat(bufn, "]"), FALSE, 0);
	if (kmacro == NULL)
	{ mlwrite(TEXT130);
					/* "Macro not defined" */
	  return FALSE;
	}

	cc = getechockey(FALSE); /* get command sequence to bind */

	return addnewbind(cc | NOTKEY, (int (Pascal *)(int, int))kmacro);
}}}

#endif

static 
int Pascal append_keys(const char * name, Emacs_cmd * addr, const char * filt)

{ char outseq[80];
  KEYTAB * ktp;
  int c;
#if	APROP
	const char *sp;

	for (sp = name-1; *++sp != 0 && *strmatch(filt, sp) != 0; )
	   ;
	if (*sp == 0)
		return 1;
#endif
																			/* search down any keys bound to this */
  for (c = 0; ++c < NOTKEY; )
  { ktp = getbind(c);
    if (ktp->k_ptr.fp == addr)
    {																		/* pad out some spaces */
      mlwrite("%>%25s%s\n", name, cmdstr(&outseq[0], ktp->k_code));
      name = "";											 	/* and clear the line */
    }
  }

  if (name[0] != 0)	   			/* if no key was bound, we need to dump it anyway */
  	mlwrite("%>%s\n", name);

  return OK;
}

	/* Describe bindings:
	   bring up a fake buffer and list the key bindings
	   into it with view mode
	*/
int Pascal desbind(int f, int n)

#if	APROP
{
  return buildlist("");
}


int Pascal apro(int f, int n)	/*Apropos (List functions that match a substring)*/

{ char mstring[NSTRING];	/* string to match cmd names to */

   mstring[0] = 0;
{  int cc = mlreply(TEXT20, mstring, NSTRING - 1);
/*			 "Apropos string: " */
   return cc == ABORT ? ABORT : buildlist(mstring);
}}
#else
{ return OK;
}
#endif
																    	 /* build a binding list(limited or full)*/
int Pascal buildlist(const char * mstring)  
											/* 1 => full list */
										    							/* match string if a partial list */
{ const NBIND *nptr;	/* pointer into the name binding table */
  int cc = TRUE;

#define bltbl "^B	kill Buffer contents\n" \
              "^F	Complete filename\n" \
				      "^G	Abort\n"\
				      "^K	Chars to eol\n"\
				      "^N	Filename of buffer\n"\
				      "^S,Alt-S Search string\n"\
				      "^W	Chars to eow\n"\
				      "Arrows	Previous\n"\
				      ".*^$[\\	Magic\n"
  openwindbuf(TEXT21);
			  
  if (mstring == NULL)
  { if (linstr(bltbl) != TRUE)
      cc = FALSE;
  }
  else    
  {	for (nptr = &names[0]-1; (++nptr)->n_func != NULL; )
		{
			if (append_keys(nptr->n_name, nptr->n_func, mstring) < 0)
				break;
		}
#if 0
	{ BUFFER * BP; 								/* add blank line between key and macro lists */
		lnewline();
									 							/* scan buffers for macros and their bindings*/
		for (bp = bheadp; bp != NULL; bp = bp->b_bufp)
		{																					/* add in the command name */
		  if (bp->b_bname[0] != '[')							/* is this buffer a macro? */
		    continue;

		{	NBIND t = {bp->b_bname, ((*)(int, int))bp};
			if (append_keys(&t, mstring) < 0)
				break;
		}}
	}
#endif
  }

  (void)mkdes();
  return cc;
}

void Pascal flook_init(char * cmd)

{ g_invokenm = strdup(cmd);

  if (g_invokenm[0] == '"')
  { ++g_invokenm;
    g_invokenm[strlen(g_invokenm)-1] = 0;
  }
{ char * sp;
  for (sp = g_invokenm; *sp != 0 && *sp != ' '; ++sp)
    if (*sp == '\\')
      *sp = '/';

  if (*sp != 0)
    *sp = 0;
}}


/*!********************************************************
 bool fexist(char * fname)
 
 Remarks
 ~~~~~~~
 Indicates whether or not the file exists and is readable
 
 Return Value
 ~~~~~~~~~~~~
 As above

 Parameters
 ~~~~~~~~~~
 char * fname		the name of the file from the current directory or absolute
**********************************************************/

int Pascal name_mode(const char * s)

{	struct stat stat_;
	if (stat(s, &stat_) != OK)
	  return 0;
	return stat_.st_mode;
}


#if S_BSD | S_UNIX5 | S_XENIX | S_SUN | S_HPUX

int Pascal fexist(const char * fname)	/* does <fname> exist on disk? */
					/* file to check for existance */
{
  char tfn[NFILEN+2];
  /* nmlze_fname(&tfn[0], fname); */
  return access(fname, 0) == 0;
}

#endif


/* replace the last entry in dir by file */
/* t can equal dir */

char * Pascal pathcat(char * t, int bufsz, const char * dir, const char * file)
	
{ 
  if (dir[0]=='.' && dir[1]=='/' && dir[2]=='.' && dir[3]=='.' && dir[4]=='/')
    dir += 2;

  if (file[0] == '/' || file[0] == '\\')
    dir = file;

{ int tix = -1;

	while (TRUE)
	{	int six = 0;
		for (; ++tix < bufsz - 1 && dir[six] != 0; ++six)
	    t[tix] = dir[six];

		if (dir == file)
			break;

		while (1)
    { while (--tix >= 0 && t[tix] != '/' && t[tix] != '\\')	/* strip back to / */
	      ;

	    if (tix > 0)
	    { if (t[tix-1] != '.' &&
	          file[0] == '.'  && file[1] == '.' && file[2] == '/')
	      { for (file = &file[2]; *++file == '/';)		/* strip forward to non / */
	          ;
	        continue;
	      }
	    }

	    break;
	  }
	  
	  dir = file;
  }

  t[tix] = 0;

  return t;
}}



static char fspec[256+2];	/* full path spec to search */

#if 0
					/* the text in dir following the last / is ignored*/
char * fex_up_dirs(const char * dir, const char * file)

{ if (dir != NULL)
  { int clamp;
//	strpcpy(fspec, dir, sizeof(fspec)-2);
 		
    for (clamp = 8; --clamp >= 0; )
    { char * pc = pathcat(fspec, sizeof(fspec), dir, file);
      if (fexist(pc))
        return fspec;
      dir = pathcat(fspec, sizeof(fspec), fspec, "../a");
    }
  }
  
  return NULL;  
}

#endif

static
int fex_path(const char * dir, const char * file)
	
{ 
  if (dir != NULL)
  { char * pc = pathcat(&fspec[0], sizeof(fspec), dir, file);
    if (fexist(pc))
      return 1;
  }
  
  return 0;  
}

extern char * g_incldirs;				// from eval.c

/*char * inclmod;			** initial mod to incldirs */

/*	wh == I => 
	Look up the existence of a file in the current working directory || 
	along the include path in the INCLDIRS environment variable. 
	wh == E => 
	Look up the existence of a file in the current working directory || 
	along the normal $PATH
	else 
	  if wh is in uppercase then look first in the $HOME directory
	  always look up a file along the normal $PATH.
*/
const char * Pascal flook(char wh, const char * fname)

{ char uwh = toupper(wh);
  char *path;	/* environmental PATH variable */
//char buf[100];

	if (fexist(fname))
	  return fname;
			                  /* if we have an absolute path check only there! */
	if (*fname == '\\' || *fname == '/' ||
	    *fname != 0 && fname[1] == ':')
	  return NULL;

	//loglog1("not so far %s", fname);

	if (uwh == 'I')
  { //loglog2("from file %s inc %s", curbp->b_fname, fname);
    if (fex_path(curbp->b_fname, fname))
      return fspec;

#if ENVFUNC
	  if (wh < 'a')
	    if (fex_path(getenv("HOME"), fname))
	      return fspec;
#endif

	  if (g_incldirs != NULL)								// gtenv("incldirs");
	  { char * incls;
	  	for (incls = g_incldirs; *incls != 0; )
	  	{ char * ln = incls;
	  		char ch;
	  		while ((ch = *++ln) != 0 && ch != PATHCHR)
	  			;
			{	int len = ln - incls + 1;
				if (len < sizeof(fspec))
	  		{	if (fex_path(strpcpy(fspec, incls, len), fname))
						return fspec;
	  		}
	  		incls = ln + (ch != 0);
	  	}}
	  }

	  return NULL;
	}
#if S_VMS
  if (g_invokenm != NULL && strlen(g_invokenm)+strlen(fname) < sizeof(fspec))
  { strcpy(fspec, g_invokenm);
    for (path = fspec-1; *++path != 0 && *path != ']'; )
      ;
    if (*path != 0)
    { strcpy(path+1, fname);
      if (fexist(fspec))
        return fspec;
    }
  }
#else
	if (fname != g_invokenm)
  { if (fex_path(flook('E', g_invokenm), fname))
	    return fspec;
	}
#endif

#if ENVFUNC
	path = getenv("PATH");

	if (path != NULL)
	  for (; *path != 0; ++path)
	  { int ix;

	    if (path[0] == '-' && path[1] == 'I'
	     || path[0] == '.' && path[1] == '/')
	      path += 2;

/*	    fspec[0] = 0;
            if (inclmod != NULL && path[0] != '/')
              strcpy(&fspec[0], inclmod);
*/
	    for (ix = -1; ix < (int)sizeof(fspec)-2 &&
	            	   *path != 0
#if S_MSDOS == 0
           		   && *path != ' '
#endif
          		   && *path != PATHCHR
		              ; )
	      fspec[++ix] = *path++;
	    
	    if (path[-1] != DIRSEPCHAR)
	      fspec[++ix] = '/';
	        
	    fspec[ix+1] = 0;	        
/*	{ char buf[200];
  	  strcat(strcat((buf, "PATHE "),fspec);
  	  MessageBox(NULL, buf, "pe",MB_YESNO|MB_ICONQUESTION);
    } */
	    if (fex_path(fspec, fname))
	      return fspec;

	    if (*path == 0)
	      break;
	  }
#endif

	return NULL;	/* no such luck */
}

#if 0

char * Pascal flookdown(char * dir, char * fname)

{ int trash
  Char * fn;
  Char buf[11];
  msd_init(dir, fname, MSD_DIRY | MSD_REPEAT | MSD_HIDFILE | MSD_SYSFILE);

  while ((fn = msd_nfile(&trash)) != NULL)
  { mlreply(fn, buf, 10);
  }
  
  return fn;
}



char *Pascal flooknear(knfname, name)
	char *knfname;		/* known file name */
	char *name;				/* file to look for */
{ 
	strpcpy(&fspec[0], knfname, sizeof(fspec));
{ Char * t = &fspec[strlen(fspec)]
  while (t > fspec && * t != DIRSEPCHR && *t != '/')
    --t;
  strpcpy(&t[0], name, NFILEN - (t - fspec));
  
  return fexist(fspec) ? fspec : NULL;
}}

#endif

				/* getfname:	This function takes a ptr to KEYTAB entry and gets the name
										  associated with it
				*/
const char *Pascal getfname(int keycode)
											/* key binding to return a name of */
{	KEYTAB * key = getbind(keycode);

  Command func = key->k_ptr.fp; 				/* ptr to the requested function*/

  if (func != 0)
									      								/* skim the binding table for a match */
    if (key->k_type == BINDFNC)
		{ const NBIND *nptr;										/* pointer into name binding table */
    	for (nptr = &names[0]-1; (++nptr)->n_func != NULL; ) 
        if (nptr->n_func == func)
          return nptr->n_name;
    }
    else																		/* skim the buffer list for a match */
		{ BUFFER *bp;
      for (bp = bheadp; bp != NULL; bp = bp->b_bufp)
        if (bp == (BUFFER*)func)
          return bp->b_bname;
		}

  return "";
}

Map_t namemap = mk_const_map(T_DOMSTR, 0, names, 1);	//-1: last entry not wanted
int g_funcnum = 0;


						/* fncmatch:	match fname to a function in the names table and return
													any match or NULL if none
						*/
int (Pascal *Pascal fncmatch(char * fname))(int, int)
	/* char *fname;	** name to attempt to match */
{		
	namemap.srch_key = fname;
	
{ int last_fnum = binary_const(&namemap, names);

	return last_fnum < 0 ? NULL : names[last_fnum].n_func;
}}



const char *Pascal transbind(char * skey)/* string key name to binding name..*/
					/* name of key to get binding for */
{ const char *bindname = getfname(stock(skey));
  return *bindname == 0 ? g_logm[2] : bindname;
}


					/* execute a function bound to a key */
int Pascal execkey(KEYTAB * key, int f, int n)	

{	return key->k_code == 0 			? TRUE :
  			 key->k_type != BINDFNC ? dobuf(key->k_ptr.buf,n) :
		  	 in_range((int)(key->k_ptr.fp), 1,40) 
				  											? execporb(-(int)(key->k_ptr.fp),n)
		  													: (*(key->k_ptr.fp))(f, n);
}

			/* set a KEYTAB to the given name of the given type */
void Pascal setktkey(KEYTAB * key, int type, char * name)
				/* type of binding */
				/* name of function or buffer */
{																		// Only called on table hooks
  key->k_type = type;
	if      (type == BINDFNC)
	  key->k_ptr.fp = fncmatch(name);
	else if (type == BINDBUF)
	  /* not quite yet... */;
}

int Pascal help(int f, int n)	/* give me some help!!!!
		   bring up a fake buffer and read the help file
		   into it with view mode			*/
{
  static const char emacshlp[] = "emacs.md";
	       char *fname = (char*)flook(0, emacshlp);

	BUFFER *bp;
	if (fname == NULL || (bp = bufflink(fname, (g_clexec > 0) | 64)) == NULL)
	{ mlwrite(TEXT12);
					/* "[Help file missing]" */
	  return FALSE;
	}
#if 0
								/* split the current window to make room for the help stuff */
	if (splitwind(FALSE, 1) == FALSE)
	  return FALSE;
#endif
		    /* make this window in VIEW mode, update all mode lines */
	curbp->b_flag |= MDVIEW;
	curbp->b_fname = strdup("HELP");  // allow the leakage
	upmode();
	return TRUE;
}

int Pascal deskey(int f, int n)	/* describe the command for a certain key */

{	register int c; 	/* key to describe */
	char outseq[NSTRING];
			     /* prompt the user to type us a key to describe */
	mlwrite(TEXT13);
			  /* ": describe-key " */
	c = getechockey(2);

{	const char * ptr = getfname(c);				/* find the right ->function */
																				/* change it to something printable */
	mlwrite("\001 %d %s %s", lastkey, cmdstr(&outseq[0], c),
																		*ptr != 0 ? ptr : TEXT14);  
																										/*"Not Bound" */
	return TRUE;
}}
