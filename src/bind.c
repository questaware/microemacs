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


#if S_WIN32 == 0
#define INCDIR "/usr/include/"
#else
#define INCDIR "C:/Program Files/Microsoft Visual Studio/VC98/include/"
#endif


#define is_bindfunc(ktp) ((Set32)((ktp)->k_ptr.fp) < (Set32)&invokenm)
/*		(((ktp)->k_code & NOTKEY) == 0) */

extern char *getenv();
extern char * g_invokenm;
extern char * homedir;

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
	{CTRL|'X',	BINDFNC, cex},
	{CTRL|'U',	BINDFNC, uniarg},
	{CTRL|'Z',	BINDFNC, backpage},
	{CTRL|'[',	BINDFNC, meta},
	{CTLX|CTRL|'B', BINDFNC, listbuffers},
	{CTLX|CTRL|'C', BINDFNC, quit},
#if AEDIT
	{CTLX|CTRL|'D', BINDFNC, detab},
	{CTLX|CTRL|'E', BINDFNC, entab},
#endif
	{CTLX|CTRL|'F', BINDFNC, filefind},
	{CTLX|CTRL|'I', BINDFNC, insfile},
	{CTLX|CTRL|'K',	BINDFNC, macrotokey},
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
/*	{CTRL|'S',	BINDFNC, setfillcol},*/
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
/*      {CTRL|'F', BINDFNC, forwsearch}, */
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
	{META|'.',	BINDFNC, setmark},
	{META|'=',	BINDFNC, setvar},
	{META|'>',	BINDFNC, gotoeob},
	{META|'<',	BINDFNC, gotobob},
	{SPEC|CTRL|'<',	BINDFNC, gotobob},
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
	{META|'F',	BINDFNC, nextword},
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
	{META|0x7F,	BINDFNC, delbword},
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
	{SPEC|'C',	BINDFNC, insspace},
	{SPEC|'D',	BINDFNC, forwdel},
	{SPEC|CTRL|'B', BINDFNC, backword},
	{SPEC|CTRL|'F', BINDFNC, nextword},
	{ALTD|SPEC|'P', BINDFNC, mvupwind},
	{ALTD|SPEC|'N', BINDFNC, mvdnwind},
#if WORDPRO
	{SPEC|CTRL|'Z', BINDFNC, gotobop},
	{SPEC|CTRL|'V', BINDFNC, gotoeop},
#endif
	{SPEC|SHFT|'1', BINDFNC, cbuf1},
	{SPEC|SHFT|'2', BINDFNC, cbuf2},
	{SPEC|SHFT|'3', BINDFNC, cbuf3},
	{SPEC|SHFT|'4', BINDFNC, cbuf4},
	{SPEC|SHFT|'5', BINDFNC, cbuf5},
	{SPEC|SHFT|'6', BINDFNC, cbuf6},
	{SPEC|SHFT|'7', BINDFNC, cbuf7},
	{SPEC|SHFT|'8', BINDFNC, cbuf8},
	{SPEC|SHFT|'9', BINDFNC, cbuf9},

#if S_VMS
	{0x7F,		BINDFNC, backdel},
#else
	{0x7F,		BINDFNC, backdel}, // was forwdel
#endif

	{0,		0, NULL}
};

//--------------------------------------------------------------------------

#if NBINDS == 0
static KEYTAB * oflowkeytab;
static int oflowtabsize = -1;
static int oflowcursize = -1;

static const KEYTAB null_keytab = {0,0};


static KEYTAB * aux_getbind;		/* only valid if result != null */
#endif

									/* This function looks a key binding up in the binding table */
KEYTAB * Pascal getbind(int c)
				/* key to find what is bound to it */
{	if (c < 0)
		return &hooks[-c];
{		
#if NBINDS
  register KEYTAB *ktp;
  register short  code;
/*
  if (c == (CTRL | SPEC | 'N'))
    adb(44);
*/
  for (ktp = &keytab[0]-1; (code = (++ktp)->k_code) != 0 && code != c; )
    ;

  return ktp;

#else
  KEYTAB * vkeytab = oflowkeytab;
  register int ix = oflowcursize;
  register int ch = c;

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

	if (func == meta)							/* if we have rebound the meta key, */
	  sterm = c;									/* make the search terminator follow it */

  return TRUE;
}}



int Pascal getechockey(int yn)

{ char outseq[20];
  int c = getckey(yn);
					/* change it to something printable */
  if (g_discmd)
  { mlwrite("\001 ");
    ostring(cmdstr(&outseq[0], c));
  }
  return c;
}



		/* bindtokey: add a new key to the key binding table */
Pascal bindtokey(int f, int n)
	/* int f, n;	** command arguments [IGNORED] */
{
	register unsigned int c;/* command key to bind */
/*	register KEYTAB *ktp;	** pointer into the command table */
				/* prompt the user to type in a key to bind */
				/* get the function name to bind it to */

	register int (Pascal *kfunc)(int, int) = getname(TEXT15);
				/*	": bind-to-key " */
	if (kfunc == NULL)
	{ mlwrite(TEXT16);
									/* "[No such function]" */
	  return FALSE;
	}

  c = getechockey((kfunc == meta) || (kfunc == ctrlg));

	return addnewbind(c, kfunc); /* search the table to see if it exists */
}

	      /* macrotokey: Bind a key to a macro in the key binding table */
Pascal macrotokey(int f, int n)
	/* int f, n;	** command arguments [IGNORED] */
{
	char bufn[NBUFN+2];	/* buffer to hold macro name */
	char outseq[NBUFN+20]; 
				/*	": macro-to-key " */
{	register int cc = mlreply(strcpy(outseq, TEXT215), &bufn[1], NBUFN-2);
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

			/* unbindkey: delete a key from the key binding table */

Pascal unbindkey(int f, int n)
	/* int f, n;	** command arguments [IGNORED] */
{
  if (!g_clexec)		/* prompt the user to type in a key to unbind */
    mlwrite(TEXT18);
								/* ": unbind-key " */

{ int c = getechockey(FALSE);					/* get the command sequence to unbind */

  register KEYTAB *sktp = getbind(c); /* search table to see if it exists */

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
  ktp->k_code   = 0;		/* null out the last one */

#else
  if (in_range(sktp - keytab, 0, upper_index(keytab)))
    addnewbind(c, NULL);
  else
    sktp->k_ptr.fp = NULL;
#endif
  return TRUE;
}}}

int Pascal append_keys(const char * name, const BUFFER * addr, const char * filt)

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
    if (ktp->k_ptr.buf == addr)
    {																		/* pad out some spaces */
      if (mlwrite("%>%25s%s\n", name, cmdstr(&outseq[0], ktp->k_code)) == 0)
        return -1;
      name = ""; 	/* and clear the line */
    }
  }

  if (name[0] != 0)	   /* if no key was bound, we need to dump it anyway */
  { if (mlwrite("%>%s\n", name) == 0)
      return -1;
  }
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
{ register const NBIND *nptr;	/* pointer into the name binding table */
  register BUFFER *bp;
  register int cc = TRUE;

#define bltbl "^B	kill Buffer contents\n" \
              "^F	Complete filename\n" \
				      "^G	Abort\n"\
				      "^K	Chars to eol\n"\
				      "^N	File Name of buffer\n"\
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
			if (append_keys(nptr->n_name, (const BUFFER*)nptr->n_func, mstring) < 0)
				break;
		}
									 /* add a blank line between the key and macro lists */
		lnewline();
									 /* scan all buffers looking for macros and their bindings*/
        
		for (bp = bheadp; bp != NULL; bp = bp->b_bufp)
		{																					/* add in the command name */
		  if (bp->b_bname[0] != '[')							/* is this buffer a macro? */
		    continue;

			if (append_keys(bp->b_bname, bp, mstring) < 0)
				break;
		}
  }

  mkdes();
  return cc;
}


/* get a command key sequence from the keyboard */

unsigned int Pascal getckey(int mflag)
			/* going for a meta sequence? */
{
	char tok[NSTRING];		/* command incoming */

			/* check to see if we are executing a command line */
	if (g_clexec)
	{ macarg(tok);	/* get the next token */
	  return stock(tok);
	}
						/* or the normal way */
	return mflag ? getkey() : getcmd();
}

#if S_WIN32

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

#endif


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
 char * fname		the name of the file from the current directory
			or absolute
**********************************************************/

int Pascal name_mode(const char * s)

{	struct stat fstat_;
	if (stat(s, &fstat_) != OK)
	  return 0;
	return fstat_.st_mode;
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
	
{ register int tix;

  if (dir[0] == '.' && dir[1] == '/' && dir[2] == '.' && dir[3] == '.' && dir[4] == '/')
    dir += 2;

  if (file[0] == '/' || file[0] == '\\')
    strpcpy(t, file, bufsz);
  else
  { for (tix = -1; ++tix < bufsz - 1 && dir[tix] != 0; )
      t[tix] = dir[tix];

    t[tix] = 0;
   
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
  
  { int six = 0;
    for (; ++tix < bufsz - 1 && file[six] != 0; ++six)
      t[tix] = file[six];

    t[tix] = 0;
  }}
  return t;
}


static char fspec[256+2];	/* full path spec to search */

					/* the text in dir following the last / is ignored*/
char * fex_up_dirs(const char * dir, char * file)

{ if (dir != NULL)
  { register int clamp;

    for (clamp = -1; ++clamp < sizeof(fspec)-2 && dir[clamp] != 0; )
      fspec[clamp] = dir[clamp];

    fspec[clamp] = 0;

    for (clamp = 8; --clamp >= 0; )
    { char * pc = pathcat(&fspec[0], sizeof(fspec), fspec, file);
      if (fexist(pc))
        return fspec;
      (void)pathcat(fspec, sizeof(fspec), fspec, "../a");
    }
  }
  
  return NULL;  
}


int fex_path(const char * dir, const char * file)
	
{ 
  if (dir != NULL)
  { char * pc = pathcat(&fspec[0], sizeof(fspec), dir, file);
    if (fexist(pc))
      return 1;
  }
  
  return 0;  
}

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

{          char uwh = toupper(wh);
  register char *path;	/* environmental PATH variable */
//         char buf[100];

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
	  if (wh < 'a' && homedir[0] != 0)
	    if (fex_path(homedir, fname))
	      return fspec;
#endif
	  if (fex_path(INCDIR, fname))
	    return fspec;
#if 1
	  path = getenv("INCLLIST");
	  if (path != NULL)
	  { FILE * ip = fopen(path, "r");
	    if (ip != NULL)
	    { char * line;
	      /*mlreply("Looking", fspec, 10);*/
	      while (line = fgets(&fspec[1], sizeof(fspec)-2, ip))
	      { 
	        line += strlen(line)-1;
	        line[0] = 0;
	        while (--line > &fspec[1] && *line != '/')
	          ;
#if	S_MSDOS | S_OS2
		 	       																		/* msdos isn't case sensitive */
	        if (*strmatch(fname, line+1) == 0 && bad_strmatch == 0)
#else
	        if (strcmp(fname, line+1) == 0)
#endif       
	        { line = &fspec[1];
	          break;
	        }
	      }
	      fclose(ip);
	      return line;
	    }
	  }
#endif
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
	char *knfname;	/* known file name */
	char *name;	/* file to look for */
{ strpcpy(&fspec[0], knfname, sizeof(fspec));
  Char * t = &fspec[strlen(fspec)]
  while (t > fspec && * t != DIRSEPCHR && *t != '/')
    --t;
  strpcpy(&t[0], name, NFILEN - (t - fspec));
  
  return fexist(fspec) ? fspec : NULL;
}

#endif

static const short viscod[7] = {CTLX, ALTD, SHFT, MOUS, META, SPEC, CTRL};
static const char viskey[14] = "^XA-S-MSM-FN^";



/* change a key command to a string we can print out */
char * Pascal cmdstr(char * t, int c) 
				/* destination string for sequence */
				/* sequence to translate */
{
	register char * ptr = t;
	register int i;
	for (i = -1; ++i < 7; )
    if (c & viscod[i])
    { ((char*)ptr)[0] = *(char *)&viskey[i*2];
      ((char*)ptr)[1] = *(char *)&viskey[i*2+1];
	     
      if (i == 6)			/* must be last */
        ptr += 1;
      else
        ptr += 2;
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
  register unsigned int c = 0;
  register int i;

  if (in_range(keyname[0], '0','9'))
    return atoi(keyname);
  
  for (i = -1; ++i < 6; )
    if (keyname[0] == viskey[i*2] && keyname[1] == viskey[i*2+1] &&
        (i != 0 || keyname[2] != 0))   /* Key is not bare ^X */
    { c |= viscod[i];
      keyname += 2;
    }
				    /* a control char?	(Always upper case) */
  if (keyname[0] == '^' && keyname[1] != 0)
  { c |= CTRL;
    ++keyname;
    mkul(1, keyname);
  }
				/* A literal control character? (Boo, hiss) */
  if(!(c & (MOUS|SPEC|ALTD|SHFT)) && *keyname < 32)
  { c |= CTRL;
    *keyname += '@';
  }

		/* make sure we are not lower case if used with ^X or M- */
  if(!(c & (MOUS|SPEC|ALTD|SHFT)))	/* If not a special key */
    if( c & (CTLX|META))		/* If is a prefix */
      mkul(1, keyname);		/* Then make sure it's upper case */

  return c | *keyname & 255;
}


/* getfname:	This function takes a ptr to KEYTAB entry and gets the name
		associated with it
*/
const char *Pascal getfname(int keycode)
					/* key binding to return a name of */
{	KEYTAB * key = getbind(keycode);

  int (Pascal *func)(int,int) = key->k_ptr.fp; /* ptr to the requested function*/
  register const NBIND *nptr;	/* pointer into the name binding table */
  register BUFFER *bp;				/* ptr to buffer to test */

  if (key->k_code != 0)				/* if this isn't a valid key, it has no name */
									      			/* skim the binding table, looking for a match */
    if (key->k_type == BINDFNC)
      for (nptr = &names[0]-1; (++nptr)->n_func != NULL; ) 
        if (nptr->n_func == func)
          return nptr->n_name;
    else
				/* skim the buffer list looking for a match */
      for (bp = bheadp; bp != NULL; bp = bp->b_bufp)
        if (bp == (BUFFER*)func)
          return bp->b_bname;

  return "";
}

/* fncmatch:	match fname to a function in the names table and return
		any match or NULL if none
*/

extern Map_t namemap; /* from main.c */

int last_fnum;

int (Pascal *Pascal fncmatch(char * fname))(int, int)
	/* char *fname;	** name to attempt to match */
{
	namemap.srch_key = fname;
	last_fnum = binary_const(&namemap, names);

	return last_fnum < 0 ? NULL : names[last_fnum].n_func;
}



const char *Pascal transbind(char * skey)/* string key name to binding name..*/
					/* name of key to get binding for */
{ const char *bindname = getfname(stock(skey));
  return *bindname == 0 ? errorm : bindname;
}


					/* execute a function bound to a key */
int Pascal execkey(KEYTAB * key, int f, int n)	

{	if (key->k_code != 0)
  {
  	if (key->k_type == BINDFNC)
     /* if (is_bindfunc(key)) */
		  return (*(key->k_ptr.fp))(f, n);
		  
		univct = n;
  {	int cc = dobuf(key->k_ptr.buf,n);
		if (cc != TRUE)
		  return cc;
  }}
  return TRUE;
}

			/* set a KEYTAB to the given name of the given type */
void Pascal setktkey(KEYTAB * key, int type, char * name)
				/* type of binding */
				/* name of function or buffer */
{
  	key->k_type = type;
	if      (type == BINDFNC)
	  key->k_ptr.fp = fncmatch(name);
	else if (type == BINDBUF)
	  /* not quite yet... */;
}

Pascal help(int f, int n)	/* give me some help!!!!
		   bring up a fake buffer and read the help file
		   into it with view mode			*/
{
  static const char emacshlp[] = "emacs.md";
	       char *fname = (char*)flook(0, emacshlp);

	register BUFFER *bp;
	if (fname == NULL || (bp = bufflink(fname, g_clexec)) == NULL)
	{ mlwrite(TEXT12);
/*				"[Help file is not online]" */
	  return FALSE;
	}
								/* split the current window to make room for the help stuff */
	if (splitwind(FALSE, 1) == FALSE)
	  return FALSE;

	(void)swbuffer(bp);
		    /* make this window in VIEW mode, update all mode lines */
	curbp->b_flag |= MDVIEW;
	curbp->b_fname = strdup("HELP");  // allow the leakage
	upmode();
	return TRUE;
}

Pascal deskey(int f, int n)	/* describe the command for a certain key */

{	register int c; 	/* key to describe */
	char outseq[NSTRING];
			     /* prompt the user to type us a key to describe */
	mlwrite(TEXT13);
	  /*  ": describe-key " */

	c = getckey(FALSE);
					/* change it to something printable */
	mlwrite("\001 %d %s ", lastkey, cmdstr(&outseq[0], c));  
						/* find the right ->function */
{	const char * ptr = getfname(c);
					 	/* put the command sequence */
	ostring(*ptr != 0 ? ptr : "Not Bound");
	return TRUE;
}}



