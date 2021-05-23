/*	Input:	Various input routines for MicroEMACS
		written by Daniel Lawrence
		5/9/86
	Notes:

	MicroEMACS's kernel processes two distinct forms of
	characters.  One of these is a standard unsigned character
	which is used in the edited text.  The other form, called
	an EMACS Extended Character is a 2 byte value which contains
	both an ascii value, and flags for certain prefixes/events.

	Bit	Usage
	---	-----
	0 = 7	Standard 8 bit ascii character
	8	Control key flag
	9	META prefix flag
	10	^X prefix flag
	11	Function key flag
	12	Mouse prefix
	13	Shifted flag (not needed on alpha shifted characters)
	14	Alterate prefix (ALT key on PCs)

	The machine dependent driver is responsible for returning
	a byte stream from the various input devices with various
	prefixes/events embedded as escape codes.  Zero is used as the
	value indicating an escape sequence is next.  The format of
	an escape sequence is as follows:

	0		Escape indicator
	<prefix byte>	upper byte of extended character
	{<col><row>}	col, row position if the prefix byte
			indicated a mouse event
	<event code>	value of event

	Two successive zeroes are used to indicate an actual
	null being input.  These values are then interpreted by
	getkey() to construct the proper extended character
	sequences to pass to the MicroEMACS kernel.
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"base.h"
#include	"msdir.h"
#include	"logmsg.h"

extern int ttgetraw(void);

extern char deltaf[HICHAR];
#define combuf ((char*)deltaf)

extern int   g_cursor_on;


LL g_ll;

static LL g_sll;

static char g_savepat[NPAT+2];

//static int g_slastkey;

static int g_kbdm[NKBDM];		/* Macro */

static LINE * macro_start_line;
static int    macro_start_col;

static int    g_slast_dir;

/* Begin a keyboard macro.
 * Error if not at the top level in keyboard processing.
 * Set up variables and return.
 */
int ctlxlp(int f, int n)

{ if (pd_kbdmode != STOP)
	{ mlwrite(TEXT105);
		return FALSE;
	}

	mlwrite(TEXT106);
				/* "[Start macro]" */

	macro_start_col = getccol();
	macro_start_line = curwp->w_dotp;
	
	g_slast_dir = pd_lastdir;
	memcpy(g_savepat,pat,NPAT+2);

	g_got_search = FALSE;

	pd_kbdwr = 0;
	pd_kbdmode = RECORD;
	return TRUE;
}


/* End keyboard macro. Check for the same limit conditions as the above
 * routine. Set up the variables and return to the caller.
 */
int  ctlxrp(int f, int n)

{ if (pd_kbdmode == STOP)
	{ mlwrite(TEXT107);
					/* "%%Macro not active" */
		return FALSE;
	}

	if (!kbd_record(pd_kbdmode))					// cmd to be ignored 
		return TRUE;
	
	pd_kbdmode = STOP;
	g_execlevel = 0;

	g_kbdm[pd_kbdwr] = 0;								// includes the sequence for this cmd!

	if (g_got_search)
		macro_start_col = -1;

	pd_lastdir = g_slast_dir;

	memcpy(pat,g_savepat,NPAT+2);
//mk_magic(-1);

  mlwrite(TEXT108);
					/* "[End macro]" */

	return TRUE;
}



/* Execute a keyboard macro.
 * The command argument is the number of times to loop. Quit as soon as a
 * command gets an error. Return TRUE if all ok, else FALSE.
 */
int  ctlxe(int f, int n)

{ int col = getccol();
#define MLIX 3
//g_slastkey = lastkey;

	if (col >= macro_start_col)
	{
		if (macro_start_col >= 0)
			curwp->w_doto = getgoal(curwp->w_dotp,macro_start_col);
						
		g_sll = g_ll;
		strpcpy(g_savepat,pat,133);
				 
		if (pd_kbdmode != STOP)
		{ mlwrite(TEXT105);
			return FALSE;
		}

		if (n > 0)
		{ pd_kbdrep = n; 			/* remember how many times to execute */
			pd_kbdmode = PLAY; 	/* start us in play mode */
			pd_kbdrd = 0;				/* at the beginning */
		}
	}

	return TRUE;
}

/* Abort.
 * Beep the beeper. Kill off any keyboard macro, etc., that is in progress.
 * Sometimes called as a routine, to do general aborting of stuff.
 */
int  ctrlg(int f, int n)

{ 	 /* TTbeep();*/
	pd_kbdmode = STOP;
	mlwrite(TEXT8);
				/* "[Aborted]" */
	return ABORT;
}



/*	tgetc:	Get a key from the terminal driver, 
		resolve any keyboard macro action.
*/
int  tgetc()

{				/* if we are playing a keyboard macro back, */
  if (kbd_play(pd_kbdmode))
  {    					/* if there is some left... */
    while (true)		/* once only */				
    { if (++pd_kbdrd > pd_kbdwr)
      { pd_kbdrd = 0;	/* reset the macro to the begining for the next rep */
        if (--pd_kbdrep <= 0)
          break;
      }
			lastkey = (int)g_kbdm[pd_kbdrd-1];
      return lastkey;
    }
					/* at the end of last repetition? */
		g_execlevel = 0;					/* weak code ! */
    pd_kbdmode = STOP;
//	lastkey = g_slastkey;
#if VISMAC == 0
    update(FALSE);		/* force a screen update after all is done */
#endif
    strcpy(pat,g_savepat);
		g_ll = g_sll;
  }

  lastkey = ttgetc();	   			 /* fetch a character from the terminal driver */
													  	 /* record it for $lastkey */
  if (kbd_record(pd_kbdmode))
  {//char buf[30];
    g_kbdm[(++pd_kbdwr)-1] = lastkey;
//  sprintf(buf,"Rec%x",lastkey);
//  mbwrite(buf);
    if (pd_kbdwr >= NKBDM)				/* don't overrun the buffer */
    { pd_kbdmode = STOP;
      TTbeep();
    }
  }

  return lastkey;
}

/*	getkey: Get one keystroke. The only prefixs legal here
			are the SPEC and CTRL prefixes.
*/
int  getkey()

{ int c = tgetc();

  if (c == 0)								/* if it exists, process an escape sequence */
  { 
    c = tgetc();				/* get the event type */
#if 0
    if (c & (MOUS >> 8))
    {			      /* mouse events need us to read in the row/col */
      xpos = tgetc();		/* grab the x/y position of the mouse */
      ypos = tgetc();
    }
#endif				
    c = tgetc() | (c << 8);					/* get the event code */
  /*loglog2("AK %x %d", c, c);*/
  }
	/* yank out the control prefix */

  return (c & 255) <= 0x1f ? CTRL | (c+'@') :
		  	 (c & 255) == 0x88 ? CTRL | '\\':
#if S_MSDOS == 0
  	 (c & 255) == 0xa7 ? SPEC | CTRL | 'Y' :	 /* next page */
  	 (c & 255) == 0xdb ? SPEC | CTRL | 'P' :	 /* previous page!! */
  	 (c & 255) == 0xdd ? SPEC | CTRL | 'C' :	 /* copy prev line */
#if 0
>   	 (c & 255) == 0xdb ? SPEC | CTRL | 'O' : /* previous page!! */
>    	 (c & 255) == 0x90 ? SPEC | CTRL | 'B' : /* not used */
>    	 (c & 255) == 0x91 ? SPEC | CTRL | 'P' : /* search reverse */
>    	 (c & 255) == 0x92 ? SPEC | CTRL | 'F' : /* search where ? */
>    	 (c & 255) == 0x93 ? SPEC | CTRL | 'N' : /* search forwards */
>    	 (c & 255) == 0x98 ? SPEC | CTRL | 'U' : /* ten lines up */
>    	 (c & 255) == 0x99 ? SPEC | CTRL | 'V' : /* ? */
>    	 (c & 255) == 0x9a ? SPEC | CTRL | 'W' : /* ten lines down */
>    	 (c & 255) == 0x9b ? SPEC | CTRL | 'X' : /* search for rbrace at bol */
>    	 (c & 255) == 0xAF ? SPEC | CTRL | 'A' : /* search for rbrace at bol */
#endif
#endif
  	  		     c;
}

/*	ectoc:	expanded character to character
		collapse the CTRL and SPEC flags back into an ascii code   */

int  USE_FAST_CALL ectoc(int c)
	
{ if (c & CTRL)
    c &= ~(CTRL | 0x40);
  return c & SPEC ? c & 255 : c;
}

/* Ask a yes or no question in the message line. Return either TRUE, FALSE, or
 * ABORT. The ABORT status is returned if the user bumps out of the question
 * with a ^G. Used any time a confirmation is required.
 */
int  mlyesno(char * prompt)

{ 
  flush_typah();
						/* " [y/n]? " */
  mlwrite("%!%s%s", prompt, TEXT162, null);
														/* get the response */
{ int c = getkey();   			/* was getcmd(); lets us check for anything that might */
													  /* generate a 'y' or 'Y' in case use screws up */
  if (c == abortc)
    return ABORT;

#if	FRENCH
  if (c=='o' || c=='O')
    return TRUE;
#endif

  return (c | 0x20) == 'y';
}}


/*	ctoec:	character to extended character
		pull out the CTRL and SPEC prefixes (if possible)	*/

int  ctoec(int c)

{
   return (unsigned)c > 0x1F ? c : CTRL | (c+'@');
}

#if S_MSDOS == 0

void  homeusr(char buf[])

{ char uname[30];
  strpcpy(&uname[0], &buf[1], 20);
{ FILE * pw = fopen("/etc/passwd", "r");
  while (true)
  { char * s = &uname[0];
    char * ln;
    if (pw == NULL || (ln = fgets(&buf[0], NSTRING-1, pw)) == NULL)
    { strcpy(&buf[0], "??");
      break;
    }
    while (*ln == *s++ && *ln != 0)
      ++ln;
    if (*ln == ':')				// entry found
    { int iter = 4;
      while (--iter >= 0)
      { ++ln;
        while (*ln != ':' && *ln != 0)
          ++ln;
      }

      if (*ln != 0)
        ++ln;
    
    { char * t = &buf[0];
      
      while ((*t = *ln++) != ':' && *t != 0)
				++t;
      *t = 0;
      break;
    }}
  }
  if (pw != NULL)
    fclose(pw);
}}

#endif

#if COMPLET

#if S_MSDOS == 0
#define lwr(x) x
#else
#define lwr(x) tolower(x)
#endif

									    	/*	comp_command:	Attempt completion on a command name */
static int comp_name(char * name, int cpos, int wh)
													/* command containing the current name to complete */
													/* ptr to position of next character to insert */
{
	int trash;
	int i;															/* index into strings */
	int comflag = FALSE;   							/* was there a completion at all? */

            /* start attempting completions, one character at a time */
  for ( ; cpos < NSTRING; ++cpos)
  { int match = 1;
    BUFFER *bp = bheadp;                		/* trial buffer to complete */
    int curbind = g_numcmd;

    if (wh == CMP_FILENAME)
    {   
//    strcpy(&name[cpos], "*");
			name[cpos] = '*';
			name[cpos+1] = 0;
   /* mbwrite2("MSD_INIT:",name); */
      msd_init(name, NULL, 
                  MSD_DIRY| MSD_REPEAT| MSD_STAY| MSD_HIDFILE| MSD_SYSFILE);
    }
                                                  /* Then the iterators */
    while (curbind > 0 && bp != NULL)
    { const Char * eny;
      if      (wh == CMP_COMMAND)
      { 
        eny = names[--curbind].n_name; /* trial command to complete */
      }
      else if (wh == CMP_BUFFER)
      {        
        eny = bp->b_bname;
        bp = bp->b_next;
      }
      else /* if (wh == CMP_FILENAME) */
      { 
        eny = msd_nfile(&trash);
        if (eny == NULL)
          break;
      }
   /* mbwrite(eny); */

      for (i = -1; ++i < cpos; )
			{	if (name[i] == '\\' && eny[i] == '/')
					continue;	
        if (lwr(name[i]) != lwr(eny[i]))
          break;
			}
                                                        /* if it is a match */
      if (i == cpos)
      {               				/* if this is the first match, simply record it */
        if      (match > 0)
        { --match;
          name[i] = lwr(eny[i]);
          if (wh == CMP_FILENAME) name[i+1] = 0;
        }
                                        /* if there's a difference, stop here */
        else if (lwr(name[i]) != lwr(eny[i]))
          return i;
      }
    } /* while over entries */
                                               /* with no match, we are done */
    if (match > 0)
    { if (comflag == FALSE)      /* beep if we never matched */
    	  TTbeep();
      break;
    }
                              /* if we have completed all the way... go back */
    if (name[cpos] == 0)
      break;
                          /* remember we matched, and complete one character */
    mlout(name[cpos]);
    TTflush();
    comflag = TRUE;
  } /* for (cpos) */
  return cpos;
}

#endif

static int  redrawln(char buf[], int clamp)
      
{ int len = clamp;
  char * s;
  char c;

  for (s = &buf[-1]; (c = *++s) != 0; )
  { 
    if (c == '\r')
    { if ((len -= 4) >= 0 && g_disinp > 0)
        mlputs("<CR>");		/* put out <CR> for <ret> */
    }  
    else
    { if (c < ' ')
      { c ^= 0x40;
        if (--len >= 0)
          mlout('^');
      }

      if (--len >= 0)
        mlout(c);
    }
  }
/*if (ttcol < term.t_ncol-1)
  { mlout(' ');
    ttcol -= 1;
  }*/
  return clamp - len;
}

int g_txt_woffs;
int g_chars_since_ctrl;


int gs_keyct;

static int getstr(char * buf, int nbuf, int promptlen, int gs_type)
	  
{ int llix = -1;
  int llcol = -1;
  int fulllen = 0;    /* maximum buffer position */
  int cpos = 0;
  char * autostr = "";
#if S_MSDOS == 0
	int twid = FALSE;
#endif
	int redo = 0;

  buf[0] = 0;

  g_chars_since_ctrl = 1000;
  gs_keyct = 0;
  g_txt_woffs = curwp->w_doto;

  for (;;)
  {	int c;             													/* current input character */
  	if (redo & 1)
    { c = buf[cpos];			/* reseek */
      buf[cpos] = 0;
      tcapmove(term.t_nrowm1, promptlen + redrawln(buf, 0));
      buf[cpos] = c;
      redo &= ~1;
    }
  { int ch = *autostr;
    if (ch != 0)
    { ++autostr;
      gs_keyct += 1;
      goto getliteral;
    }

    if (redo)
    { tcapeeol();				/* redraw */
      c = ttcol;
      (void)redrawln(&buf[cpos], term.t_ncol-c);
      tcapmove(term.t_nrowm1, c);
      redo = 0;
    }
    TTflush();
    ch = getkey();
    if (ch != (SPEC | 'P') && ch != (SPEC | 'N'))
      llcol = -1;
                              /* if they hit the line terminate, wrap it up */
    if (ch == sterm)
    { /* mlerase();                  ** clear the message line */
      if (buf[0] != 0)
        break;
                          /* if we default the buffer, return FALSE */
      return FALSE;
    }

    if (ch == abortc)
    	return ctrlg(FALSE, 0);    /* Abort any kb macro */

    gs_keyct += 1;
                     /* change from command form back to character form */
    c = ectoc(ch);
#if S_MSDOS == 0
    if (c == '/' && twid && gs_type == CMP_FILENAME)
    { char * homedir = getenv("HOME");
      buf[cpos] = 0;
      if (cpos == 1 && homedir != NULL)
      { strpcpy(&combuf[0], homedir, HICHAR-2);
      }
      else
      { homeusr(strpcpy(&combuf[0], buf, HICHAR-2));
      }
      buf[0] = 0;
      cpos = 0;
      autostr = strcat(combuf,"/");
      redo = 3;
      twid = FALSE;
      continue;
    }               
#endif

    if      (c==0x7F || c==0x08 || c==0x15)         /* rubout/erase */
    { if (cpos > 0)
      { int tpos;
      	--cpos;
        for (tpos = cpos; ++tpos <= fulllen; )   /* include the 0 */
          buf[tpos-1] = buf[tpos];      
        --fulllen;
				redo = 3;
      }
    }
    else if (c == 'A'-'@' && gs_type >= 0)
    { return buildlist(null);
//    return ABORT;
    }
    else if (c == 'F'-'@')
    { int tpos,ix;
      char mybuf[256];
      buf[cpos+1] = 0;
      for (tpos = cpos; --tpos >= 0 && !is_space(buf[tpos]); )
        ;
      ((char*)memcpy(mybuf,buf+tpos+1,cpos-tpos))[cpos-tpos] = 0;

      ix = comp_name(mybuf, cpos - tpos - 1, CMP_FILENAME);
      if (ix > cpos - tpos)
      { cpos = tpos+1+ix;
        fulllen = cpos;
        buf[cpos] = 0;
        memcpy(buf+tpos+1,mybuf,ix);
      }
    }
    else if ((c == ' '|| c == '\t') && gs_type >= 0 && cpos > 0)
    { 
    	int tpos = comp_name(buf, cpos, gs_type);    /* attempt a completion */
      if (tpos > 0 && buf[tpos - 1] == 0)
        break;
      cpos = tpos;
      buf[cpos] = 0;
      fulllen = cpos;
    }
    else if (ch & SPEC)
    { int tpos = cpos;
      switch (ch)
      { case (SPEC | 'F'): tpos += 1;
        when (SPEC | 'B'): tpos -= 1;
        when (SPEC | '<'): 
								         { if 			(tpos != 0 || promptlen == 0)
								        		 tpos = 0;
								        	 else if (gs_type < 0)
								        	 { strpcpy(buf, lastmesg, nbuf);
														 fulllen += promptlen;
								        		 promptlen = 0;
								        		 cpos = 0;
												     tcapmove(term.t_nrowm1, 0);
								        		 continue;
								        	 }
								         }
        	
				when (SPEC | '>'): tpos = fulllen;
        when (SPEC | 'P'):
        case (SPEC | 'N'):	/* up or down arrows */
								         { if (llcol >= 0)		/* remove the old */
								           { cpos = llcol;
								             fulllen = cpos;
								             buf[cpos] = 0;
								           }
								           llcol = cpos;
								           llix += ch - (SPEC | 'O');
								           autostr = &g_ll.lastline[(g_ll.ll_ix-llix) & MLIX][0];
								           redo = 3;
								           continue;
								         }
      }
      
      if (tpos < 0 || tpos > fulllen)
        /* tcapbeep() */ ;
      else
      { cpos = tpos;
        redo = 1;
      }
    }
    else if (c <= 'Z'-'@' && ch != quotec || c == (ALTD | 'S'))
    { autostr = fixnull(getwtxt(c, &combuf[0], HICHAR-3-cpos));
    }
    else 
    { int tpos;
    	char mybuf[3];

      if (ch == quotec)
      	ch = getkey();     /* get a character from the user */
      
getliteral:
      c = ectoc(ch);

      if (fulllen >= nbuf-1)
      { autostr = "";
        continue;
      }
#if S_MSDOS == 0
      if (cpos == 0 && c == '~')
        twid = 1;
#endif
#if 0
      if (cpos < 0 || (unsigned)fulllen > 80)
      { tcapbeep();
      }
#endif
      ch = c;
      for (tpos = ++fulllen; --tpos >= cpos; )
        buf[tpos+1] = buf[tpos];
      buf[cpos++] = ch;
      if (fulllen > cpos)
      	redo = 2;
      
      if (g_disinp <= 0)
				ch = '*';
#if S_MSDOS
      mybuf[0] = ch;
      mybuf[1] = 0;
      (void)redrawln(mybuf, term.t_ncol-ttcol);
#else
			mlout(ch);
#endif
    }
  }}
  g_ll.ll_ix += 1;
  strpcpy(&g_ll.lastline[g_ll.ll_ix & MLIX][0], buf, NSTRING-1);
  return TRUE;
}

/*	A more generalized prompt/reply function allowing the caller
	to specify the proper terminator. If the sterm is not
	a return('\r'), return will echo as "<CR>"
*/
int  getstring(char * buf, int nbuf, const char * prompt)

{				  /* prompt the user for the input string */
  --g_cursor_on;
{ int res = getstr(buf, nbuf, mlwrite(prompt), -1);
  ++g_cursor_on;
  return res;
}}


char gs_buf[NSTRING+2];		/* buffer to hold tentative name */

char * complete(char * prompt, char * defval, int type, int maxlen)
					/* prompt to user on command line */
					/* default value to display to user */
				/* type of what we are completing */
				/* maximum length of input field */
{ int cc;
		 /* if executing a command line get the next arg and match it */
  if (g_clexec)
    return macarg(gs_buf) <= FALSE ? NULL : gs_buf;

#if COMPLET == 0
  strcpy(gs_buf, prompt);
  if (type != CMP_COMMAND && defval)
    concat(&gs_buf[0], "[", defval, "]", null);

  cc = mlreply(strcat(&gs_buf[0], ": "), gs_buf, maxlen);
#else
{ int plen = ! prompt ? 0
											: mlwrite(type==CMP_COMMAND ? "%s"	 :
															  defval	    			? "%s[%s]: " :
																			              "%s: ",   prompt, defval);
  --g_cursor_on;
  cc = getstr(gs_buf, NSTRING-2, plen, type);
  ++g_cursor_on;
}
#endif
  return cc < 0				          	? NULL   : 
         defval && gs_buf[0] == 0 ? strpcpy(gs_buf,defval,NSTRING) : gs_buf;
}

	/* get a command name from the command line. Command completion means
	   that pressing a <SPACE> attempts to complete an unfinished command
	   name if it is unique.
	*/
//int ( * getname(char * prompt))(int, int)
Command getname(char * prompt)
				/* string to prompt with */
{
  char *sp = complete(prompt, NULL, CMP_COMMAND, NSTRING);

  return sp == NULL ? NULL : fncmatch(sp);
}


/*	     getcbuf:	get a completion from the user for a buffer name.

			I was goaded into this by lots of other people's
			completion code.
*/
BUFFER * getcbuf(char *prompt, char *defval, int createflag)
				/* prompt to user on command line */
				/* default value to display to user */
				/* should this create a new buffer? */
{
  char *sp = complete(prompt, defval, CMP_BUFFER, NBUFN);

  return sp == NULL ? NULL : bfind(sp, createflag, 0);
}


char * USE_FAST_CALL gtfilename(char * prompt)
				/* prompt to user on command line */
{
  return complete(prompt, NULL, CMP_FILENAME, NFILEN);
}

