/*	This file is for functions having to do with key bindings,
		descriptions, and help commands.

	written 11-feb-86 by Daniel Lawrence
*/
#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<sys/stat.h>

#include	"estruct.h"
#if S_MSDOS
#include <windows.h>
#endif
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"map.h"
#include	"msdir.h"
#include	"logmsg.h"

extern char *getenv();

#if 0

static int Pascal cbuf1(int f, int n)  { return execporb(1,n); }
static int Pascal cbuf2(int f, int n)  { return execporb(2,n); }
static int Pascal cbuf3(int f, int n)  { return execporb(3,n); }
static int Pascal cbuf4(int f, int n)  { return execporb(4,n); }
static int Pascal cbuf5(int f, int n)  { return execporb(5,n); }
static int Pascal cbuf6(int f, int n)  { return execporb(6,n); }
static int Pascal cbuf7(int f, int n)  { return execporb(7,n); }
static int Pascal cbuf8(int f, int n)	 { return execporb(8,n); }
static int Pascal cbuf9(int f, int n)  { return execporb(9,n); }
static int Pascal cbuf10(int f, int n) { return execporb(10,n); }
static int Pascal cbuf11(int f, int n) { return execporb(11,n); }
static int Pascal cbuf12(int f, int n) { return execporb(12,n); }
static int Pascal cbuf13(int f, int n) { return execporb(13,n); }
static int Pascal cbuf14(int f, int n) { return execporb(14,n); }
static int Pascal cbuf15(int f, int n) { return execporb(15,n); }
static int Pascal cbuf16(int f, int n) { return execporb(16,n); }
static int Pascal cbuf17(int f, int n) { return execporb(17,n); }
static int Pascal cbuf18(int f, int n) { return execporb(18,n); }
static int Pascal cbuf19(int f, int n) { return execporb(19,n); }
static int Pascal cbuf20(int f, int n) { return execporb(20,n); }
static int Pascal cbuf21(int f, int n) { return execporb(21,n); }
static int Pascal cbuf22(int f, int n) { return execporb(22,n); }
static int Pascal cbuf23(int f, int n) { return execporb(23,n); }
static int Pascal cbuf24(int f, int n) { return execporb(24,n); }
static int Pascal cbuf25(int f, int n) { return execporb(25,n); }
static int Pascal cbuf26(int f, int n) { return execporb(26,n); }
static int Pascal cbuf27(int f, int n) { return execporb(27,n); }
static int Pascal cbuf28(int f, int n) { return execporb(28,n); }
static int Pascal cbuf29(int f, int n) { return execporb(29,n); }
static int Pascal cbuf30(int f, int n) { return execporb(30,n); }
static int Pascal cbuf31(int f, int n) { return execporb(31,n); }
static int Pascal cbuf32(int f, int n) { return execporb(32,n); }
static int Pascal cbuf33(int f, int n) { return execporb(33,n); }
static int Pascal cbuf34(int f, int n) { return execporb(34,n); }
static int Pascal cbuf35(int f, int n) { return execporb(35,n); }
static int Pascal cbuf36(int f, int n) { return execporb(36,n); }
static int Pascal cbuf37(int f, int n) { return execporb(37,n); }
static int Pascal cbuf38(int f, int n) { return execporb(38,n); }
static int Pascal cbuf39(int f, int n) { return execporb(39,n); }
static int Pascal cbuf40(int f, int n) { return execporb(40,n); }

#endif

#include	"efunc.h"	/* function declarations and name table */
										/* LINUX cannot distinguish META and ALTD for characters */
#if S_WIN32
#define MUTA ALTD
#else
#define MUTA META
#endif

/*
 * Command table.
 * This table  is *roughly* in ASCII order, left to right across the
 * characters of the command. This explains the funny location of the
 * control-X commands.
 */
#define PERSONAL_KEYS 1

//--------------------------------------------------------------------------

static KEYTAB keytab[NBINDS+1] = 
{
	{CTRL|'A',	gotobol},
	{CTRL|'B',	backchar},
	{CTRL|'E',	endword},
	{CTRL|'H',	backdel},
//{CTRL|'J',	nop},
	{CTRL|'K',	killtext},
	{CTRL|'L',	refresh},
	{CTRL|'M',	ins_newline},
	{CTRL|'N',	forwline},
#if FLUFF
	{CTRL|'O',	openline},
#endif
	{CTRL|'P',	backline},
	{CTRL|'Q',	quote},
	{CTRL|'S',	forwsearch},
	{CTRL|'U',	uniarg},
//{CTRL|'X',	cex},
	{CTRL|'Z',	backpage},
//{CTRL|'[',	meta},
	{CTLX|CTRL|'B', listbuffers},
	{CTLX|CTRL|'C', quit},
#if AEDIT
	{CTLX|CTRL|'D', detab},
	{CTLX|CTRL|'E', entab},
#endif
	{CTLX|CTRL|'F', filefind},
	{CTLX|CTRL|'I', insfile},
//{CTLX|CTRL|'K', macrotokey},
	{CTLX|CTRL|'L', lowerregion},
	{CTLX|CTRL|'M', delmode},
	{CTLX|CTRL|'N', mvdnwind},
	{CTLX|CTRL|'P', mvupwind},
	{CTLX|CTRL|'R', fileread},
	{CTLX|CTRL|'S', filesave},
#if AEDIT
	{CTLX|CTRL|'T', trim_white},
#endif
	{CTLX|CTRL|'U', upperregion},
	{CTLX|CTRL|'W', filewrite},
#if FLUFF
	{CTLX|CTRL|'X', swapmark},
#endif
	{CTLX|CTRL|'Z', shrinkwind},
	{CTLX|CTRL|'V', viewfile},
	{CTLX|'?',	deskey},
	{CTLX|'!',	spawn},
//{CTLX|'$',	spawn},
	{CTLX|'#',	filter},
	{CTLX|'=',	bufferposition},
	{CTLX|'(',	ctlxlp},
	{CTLX|')',	ctlxrp},
	{CTLX|'<',	narrow},
	{CTLX|'>',	widen},
	{CTLX|'^',	enlargewind},
	{CTLX|' ',	remmark},
	{CTLX|'0',	delwind},
	{CTLX|'1',	onlywind},
	{CTLX|'2',	splitwind},
	{CTLX|'B',	usebuffer},
#if S_BSD
	{CTLX|'D',	bktoshell},
#endif
/*	{CTLX|'F',	abort},*/
#if DEBUGM
	{CTLX|'G',	dispvar},
#endif
	{CTLX|'K',	dropbuffer},
	{CTLX|'M',	togmod},
	{CTLX|'N',	filename},
	{CTLX|'O',	nextwind},
	{CTLX|'P',	prevwind},
#if ISRCH
	{CTLX|'R',	risearch},
	{CTLX|'F',	fisearch},
#endif
	{CTLX|'W',	resize},
	{CTLX|'X',	nextbuffer},
	{CTLX|'Z',	enlargewind},
	{CTLX|'@',	pipecmd},
	{META|CTRL|'R', qreplace},
				/* personal keys */
#if PERSONAL_KEYS == 0
	{CTRL|'D',	forwdel},
	{CTLX|'E',	ctlxe},
	{CTRL|'F',	ctrlg},
	{CTRL|'G',	ctrlg},
#if CFENCE
	{META|CTRL|'F', getfence},
#endif
	{CTRL|'R',	backsearch},
#if FLUFF
	{CTRL|'T',	twiddle},
#endif
	{CTRL|'R',	backsearch},
	{CTRL|'W',	killregion},
	{CTRL|'V',	forwpage},
	{CTRL|'Y',	yank},
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
	{CTRL|'W', ctlxe},
#if CFENCE
	{CTRL|'T', getfence},
#endif
	{CTRL|'R', qreplace},
/*{CTRL|'F', forwsearch}, */
	{META|'S', backsearch},
	{CTLX|'D', killregion},
	{SPEC|CTRL|'D', killregion},
	{CTLX|'C', copyregion},
	{CTRL|'G', kdelete}, /* tbd: freed up ^G to agree with jasspa */
	{CTRL|'F', kdelete},
	{CTRL|'V', yank},
	{CTLX|'|', filter},
	{CTRL|' ', setmark},
#endif

#if WORDPRO
	{META|CTRL|'C', wordcount},
#endif
	{META|CTRL|'G', gotomark},
	{META|CTRL|'H', delbword},
//{META|CTRL|'K', unbindkey},
//{META|CTRL|'L', refresh},
	{META|CTRL|'M', delgmode},
#if 0
	{META|CTRL|'N', namebuffer},  /* no longer supported */
#endif
	{META|CTRL|'S', execfile},
	{META|CTRL|'V', nextdown},
#if WORDPRO && 0
	{META|CTRL|'W', killpara},
#endif
#if FLUFF
	{META|CTRL|'X', execcmd},
#endif
	{META|CTRL|'Z', nextup},
	{META|' ',	setmark},
	{META|'?',	help},
#if FLUFF
	{META|'!',	refresh},
#endif
	{META|'=',	setvar},
	{META|'>',	gotoeob},
	{META|'<',	gotobob},
	{SPEC|CTRL|'<', gotobob},
#if APROP
	{META|'A',	apro},
#endif
//{META|'B',	backword},
	{MUTA|'W',	copyword},
	{META|'D',	delfword},
#if CRYPT
	{META|'E',	setuekey},
#endif
	{META|'G',	gotoline},
	{META|'K',	bindtokey},
	{META|'L',	lowerword},
	{META|'M',	setgmode},
#if WORDPRO
	{META|'N',	gotoeop},
	{META|'P',	gotobop},
	{META|'Q',	fillpara},
#endif
	{META|'R',	sreplace},
#if S_BSD
	{META|'S',	bktoshell},
#endif
	{META|'U',	upperword},
	{META|'V',	backpage},
	{META|'W',	copyregion},
	{META|'X',	namedcmd},
#if S_MSDOS == 0
	{META|'Z',	wordsearch},
#endif
//{META|0x7F, delbword},
#if MOUSE
	{MOUS|'a',	movemd},
	{MOUS|'b',	movemu},
	{MOUS|'e',	mregdown},
	{MOUS|'f',	mregup},
	{MOUS|'1',	resizm},
#endif 
	{MUTA|'S',	forwhunt},
	{MUTA|'R',	backhunt},
	{MUTA|'Z',	wordsearch},
	{META|',',	indentsearch},
	{SPEC|'<',	gotobob},
	{SPEC|'P',	backline},
	{META|SPEC|'<',searchIncls},
	{META|SPEC|'N',nextwind},
	{SPEC|'Z',	backpage},
	{SPEC|'B',	backchar},
	{SPEC|'F',	forwchar},
	{SPEC|'>',	gotoeol},
	{SPEC|'N',	forwline},
	{SPEC|'V',	forwpage},
//{SPEC|'C',	insspace},
	{SPEC|'D',	forwdel},
	{SPEC|CTRL|'B', backword},
	{SPEC|CTRL|'F', nextword},
	{ALTD|SPEC|'P', mvupwind},
	{ALTD|SPEC|'N', mvdnwind},
#if WORDPRO
	{SPEC|CTRL|'Z', gotobop},
	{SPEC|CTRL|'V', gotoeop},
#endif
	{SPEC|SHFT|'1', (Command)1},		// Shift 
	{SPEC|SHFT|'2', (Command)2},
	{SPEC|SHFT|'3', (Command)3},
	{SPEC|SHFT|'4', (Command)4},
	{SPEC|SHFT|'5', (Command)5},
	{SPEC|SHFT|'6', (Command)6},
	{SPEC|SHFT|'7', (Command)7},
	{SPEC|SHFT|'8', (Command)8},
	{SPEC|SHFT|'9', (Command)9},

#if S_VMS
	{0x7F,		backdel},
#else
	{0x7F,		backdel}, // was forw
#endif

	{0, NULL}
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
int  getcmd(int mode)

{ int c = getkey();
	if (mode)
		return c;
{	int pfx = c == (CTRL|'[') ? META :
						c == (CTRL|'X') ? CTLX : 0;
	if (pfx != 0)
		c = getkey();
																									/* Force to upper */
	return pfx | (!in_range(c & 0xff,'a','z') ? c : c & ~0x20);
}}


									/* This function looks a key binding up in the binding table */
KEYTAB * getbind(int c)
				/* key to find what is bound to it */
{	// if (c < 0)
	//	return &hooks[-c];
{		
#if NBINDS
  KEYTAB *ktp;
  short  code;
/*
  if (c == (CTRL | SPEC | 'N'))
    adb(44);
*/
#if 0
	Emacs_cmd * f;
  for (ktp = &keytab[0]-1; (f = (++ktp)->k_ptr.fp) != 0 && f != copyword; )
    ;
#endif

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


static
int Pascal USE_FAST_CALL addnewbind(int c, int (Pascal *func)(int, int))
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
//ktp->k_type   = BINDFNC;
  ktp->k_code   = c;
  ktp->k_ptr.fp = func;
	if (func == ctrlg)					/* if the function is a unique prefix key */
	 	g_abortc = c;								/* reset the appropriate global prefix variable*/

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

      ptr += 1 + (i != 0);
    }
					/* and output the final sequence */
  ptr[0] = c & 255;
  ptr[1] = 0;
	return t;
}



/*	stock() 	String key name TO Command Key

	A key binding consists of one or more prefix functions followed by
	a keystroke.  Allowable prefixes must be in the following order:

	^X	preceeding control-X
	A-	simultaneous ALT key (on PCs mainly)
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

  if(!(c & (MOUS|SPEC|ALTD|SHFT))) 	/* If not a special key */
  {
	  if (in_range(keyname[0], '0','9'))
  	  return atoi(keyname);

  	if (keyname[0] < 32)						/* A literal control char? (Boo, hiss) */
	  { c |= CTRL;
  	  *keyname += '@';
  	}
	}

//if (c & (CTRL|CTLX|META))
  mkul(1, keyname);								/* Make sure it's upper case */

  return c | *keyname & 255;
}


static
int Pascal getechockey(int mode)

{ char outseq[20];
	char tok[NSTRING];
								/* check to see if we are executing a command line */
	int c;
	if (g_macargs <= 0)
		c = getcmd(mode);
	else
	{ macarg(tok);	/* get the next token */
	  c = stock(tok);
	}
																	/* change it to something printable */
  if (!mode)
  	mlwrite("\001 %s",cmdstr(&outseq[1], c));	// can be overwritten!

  return c;
}



			/* bindtokey: add a new key to the key binding table */
int Pascal bindtokey(int f, int n)
									/* int f, n;	** command arguments [IGNORED] */
{
	int (Pascal *kfunc)(int, int) = getname(2);
																				/* ": bind-to-key " */
	if (kfunc == NULL)
	{ mlwrite(TEXT16);
				  /* "[No such function]" */
	  return FALSE;
	}

{	int c = getechockey(FALSE);

	return addnewbind(c, kfunc); /* search the table to see if it exists */
}}

			/* unbindkey: delete a key from the key binding table */

Pascal unbindkey(int f, int n)
							/* int f, n;	** command arguments [IGNORED] */
{
  if (g_macargs <= 0)		/* prompt the user to type in a key to unbind */
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
  KEYTAB * ktp = getbind(0);			/* get pointer to end of table */
			           										/* copy the last entry to the current one */
  *sktp = ktp[-1];
  ktp[-1].k_code = 0;										/* null out the last one */

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
	if (cc <= FALSE)
	  return cc;
																     /* build the response string for later */
	strcat(outseq, &bufn[1]);
	bufn[0] = '[';
{	BUFFER *kmacro = bfind(strcat(bufn, "]"), FALSE);
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
      mlwrite("%>%25s%s\n", name, cmdstr(&outseq[1], ktp->k_code));
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

{
  return buildlist(1);
}

#if	APROP

int Pascal apro(int f, int n)	/*Apropos (List functions that match a substring)*/

{   return buildlist(-1);
}

#endif
																    	 /* build a binding list(limited or full)*/
int Pascal buildlist(int wh)
											/* 0 => linstr, -1 => filter, 1 => dont filter */
{ openwindbuf(TEXT21);

	if (wh == 0)
  { 
#define bltbl "^B	kill_buffer contents\n" \
              "^F	Complete a filename\n" \
				      "^G	Abort\n"\
				      "^K	Chars to eol\n"\
				      "^N	Filename of buffer\n"\
				      "^S,Alt-S Search string\n"\
				      "^W	Chars to eow\n"\
				      "Arrows	Previous\n"\
				      ".*^$[\\	Magic\n"
  	(void)linstr(bltbl);
  }
  else
 	{ char mstring[NSTRING];	/* string to match cmd names to */
	  mstring[0] = 0;
    if (wh < 0)
		{ int cc =  mlreply(TEXT20, mstring, NSTRING - 1);
/*			 "Apropos string: " */
			if (cc < 0)
				return cc;
  	}
			  
  {	const NBIND *nptr;	/* pointer into the name binding table */
  	for (nptr = &names[0]-1; (++nptr)->n_func != NULL; )
		{
			if (append_keys(nptr->n_name, nptr->n_func, mstring) < 0)
				break;
		}
  }}

  return mkdes();
}

static char * g_fspec;
//static int   g_fspec_len;


#if S_MSDOS == 0
#define HOMEPATH "HOME"
static
 char * g_invokenm;
#else
#define HOMEPATH "HOMEPATH"
#endif


#if S_MSDOS == 0

void Pascal flook_init(char * cmd)

{ 
	g_invokenm = strdup(cmd);
}

#endif


/*!********************************************************
 bool name_mode(char * fname)
 
 Return Value
 ~~~~~~~~~~~~
 st_mode

 Parameters
 ~~~~~~~~~~
 char * fname		the name of the file from the current directory or absolute
**********************************************************/

#if S_WIN32 == 0

int Pascal name_mode(const char * s)

{	struct stat stat_;
 	if (stat(s, &stat_) != OK)
 	  return 0;
	return stat_.st_mode;
}

#endif
 
#if S_BSD | S_UNIX5 | S_XENIX | S_SUN | S_HPUX

int Pascal fexist(const char * fname)	/* does <fname> exist on disk? */
					/* file to check for existance */
{
  char tfn[NFILEN+2];
  /* nmlze_fname(&tfn[0], fname); */
  return access(fname, 0) == 0;
}

#endif

#if 0
static 
int USE_FAST_CALL find_sl(const char * s, int tix)

{ while (--tix >= 0 && s[tix] != '/' && s[tix] != '\\')	/* strip back to / */
    ;
    
  return tix;
}
#endif


/* replace the last entry in dir by file */
/* t can equal dir */

char * Pascal pathcat(char * t, int bufsz, const char * dir, const char * file)
	
{ if      (dir == NULL || file[0] == '/' || file[0] == '\\')
    dir = file;
	else if (*strmatch("./../", dir) == 0)
    dir += 2;

{ int tix = -1;

	while (TRUE)
	{	int six = 0;
		for (; ++tix < bufsz - 1 && dir[six] != 0; ++six)
	    t[tix] = dir[six];

		if (dir == file)
			break;

		while (1)
    {
	   	while (--tix >= 0 && t[tix] != '/' && t[tix] != '\\')	/* strip back to / */
	      ;

	    if (tix > 0)
	    { if (t[tix-1] != '.' && *strmatch("../", file) == 0)
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



static
const char * fex_file(int drop_sl, const char ** ref_dir, const char * file)
												// next_dir == NULL => do not append file	
{ char ch;
	const char * dir = *ref_dir;
  if (dir != NULL)
	{	if (// dir[0] == '-' && dir[1] == 'I' ||
		 		   dir[0] == '.' && dir[1] == '/')
			dir += 2;
  
	{	int ix = -1;

// 	mlwrite("Fex_file %s %s%p\n", file, dir);

		while ((ch = dir[++ix]) != 0 && ch != PATHCHR)
			;
			
		*ref_dir = dir + ix - (ch == 0);
	{	int sl = strlen(dir) + strlen(file) + 127;
		char * fs = remallocstr(&g_fspec, NULL, sl);
		char * diry = strcat(strpcpy(fs, dir, ix + 1),DIRSEPSTR+drop_sl);
// 	mbwrite(diry);
  {	char * pc = pathcat(g_fspec, sl-1, diry, file);
// 	mlwrite("%pAfter %s", pc);
    if (fexist(pc))
    	return (const char*)pc;
  }}}}
  
  return NULL;
}

/*char * inclmod;			** initial mod to incldirs */

/*	wh == -1 =>
	Look up the existence of a file in the directory of the buffer || 
	along the include path in the INCLDIRS environment variable. 
		wh ==  1 => 
	Look up the existence of a file along the normal $PATH
		else 
	Look first in the $HOME directory
	then in the directory containing the executable.
*/
const char * Pascal flook(char wh, const char * fname)

{	const char * ppath = getenv("PATH");
	const char * path = wh > 0 ? ppath					:			// Q_LOOKP
											wh < 0 ? curbp->b_fname :			// Q_LOOKI
				 											 getenv(HOMEPATH);		// Q_LOOKH
	int drop_sl = (wh & 2);	// Q_LOOKI
	int clamp = 2;

	while (--clamp >= 0 && path != NULL)
  { for (--path; *++path != 0;)
  	{ const char * res = fex_file(drop_sl, &path, fname);
	  	if (res)
	   		return res;
	  }

		drop_sl = 0;

	  if      (wh < 0)	// Q_LOOKI
			path = pd_incldirs;
		else if (wh == Q_LOOKH)
			path = ppath;
		else
			break;
	}

	if (fexist(fname))
		return fname;

#if 1
	return NULL;
#else
#if S_WIN32
	(void)GetModuleFileName(0, invokenm, 512);
	path = invokenm;
#else
	path = flook(Q_LOOKP, g_invokenm);
#endif
  return fex_file(1, &path, path);
#endif
}

#if 0

char * Pascal flookdown(char * dir, char * fname)

{ Char * fn;
  Char buf[NFILEN+1];
  
  msd_init(strcat(strcat(strcpy(buf,dir), "/"), fname);
  				 MSD_DIRY | MSD_REPEAT | MSD_HIDFILE | MSD_SYSFILE | USE_PATH);

  while ((fn = msd_nfile()) != NULL)
  { mlreply(fn, buf, 10);
  }
  
  return fn;
}



char *Pascal flooknear(knfname, name)
	char *knfname;		/* known file name */
	char *name;				/* file to look for */
{ 
	char * t = remallocstr(&g_fspec, knfname, 0);
//strpcpy(&g_fspec[0], knfname, g_fspec_len);
	int tix;
	int lix = 0;
	char ch;
	for (tix = -1; (ch = t[++ix]) != 0; )
		if (ch == DIRSEPCHR || ch == '/')
		  lix = tix;

  strpcpy(&t[lix], name, NFILEN - lix);

  return fexist(t) ? t : NULL;
}

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
    if (1 /*key->k_type == BINDFNC*/)
		{ const NBIND *nptr;										/* pointer into name binding table */
    	for (nptr = &names[0]-1; (++nptr)->n_func != NULL; ) 
        if (nptr->n_func == func)
          return nptr->n_name;
    }
    else																		/* skim the buffer list for a match */
		{ BUFFER *bp;
      for (bp = bheadp; bp != NULL; bp = bp->b_next)
        if (bp == (BUFFER*)func)
          return bp->b_bname;
		}

  return "";
}

Map_t g_namemap = mk_const_map(T_DOMSTR, 0, names, 1);	//-1: last entry not wanted

						/* fncmatch:	match fname to a function in the names table and return
													any match or NULL if none
						*/
int (Pascal *Pascal USE_FAST_CALL fncmatch(const char * fname))(int, int)
	/* char *fname;	** name to attempt to match */
		
{ int last_fnum = binary_const(-1, fname);

	return last_fnum < 0 ? NULL : names[last_fnum].n_func;
}


#if 0
			/* set a KEYTAB to the given name of the given type */
void Pascal setktkey(int type, char * name, KEYTAB * key)
				/* type of binding */
				/* name of function or buffer */
{																		// Only called on table hooks
  key->k_code = 1;
//key->k_type = type;
	if      (type == BINDFNC)
	  key->k_ptr.fp = fncmatch(name);
	else if (type == BINDBUF)
	  /* not quite yet... */;
}

#endif

int Pascal help(int f, int n)	/* give me some help!!!!
		   bring up a fake buffer and read the help file
		   into it with view mode			*/
{
  static const char emacshlp[] = "microemacs.md";
	       char *fname = (char*)flook(Q_LOOKH, emacshlp);

	BUFFER *bp;
	if (fname == NULL || (bp = bufflink(fname, 64)) == NULL)
	{ mlwrite(TEXT12);
					/* "[Help file missing]" */
	  return FALSE;
	}
		    /* make this window in VIEW mode, update all mode lines */
	bp->b_flag |= MDVIEW;
	bp->b_fname = strdup("HELP");  // allow the leakage
//upmode();
	return TRUE;
}


int Pascal deskey(int f, int n)	/* describe the command for a certain key */

{												     /* prompt the user to type us a key to describe */
	mlwrite(TEXT13);
			  /* ": describe-key " */
#define DESC_TO_FILE 0
#if DESC_TO_FILE
{	char outseq[NSTRING];
	BUFFER *bp = bufflink("pjsout", 64);
	if (swbuffer(bp) > 0)
	{	
		gotoeob(0,0);
		while (pd_lastkey > 32)
		{ mlerase();
			update(FALSE);

		{	int	c = getechockey(2);
			const char * ptr = getfname(c);				/* find the right ->function */
			mlwrite("\001 %d %s %s", pd_lastkey, cmdstr(&outseq[0], c),
																			 *ptr != 0 ? ptr  TEXT14);
			linstr(strcat(lastmesg,"\n"));
		}}
	}
}
#else
{	int	c = getechockey(0);

	const char * ptr = getfname(c);				/* find the right ->function */
																				/* change it to something printable */
	mlwrite("\001 %d %s", pd_lastkey, // cmdstr(&outseq[0], c),
																		*ptr != 0 ? ptr : TEXT14);
																										/*"Not Bound" */
}
#endif
	return TRUE;
}
