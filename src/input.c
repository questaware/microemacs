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

//extern int   g_cursor_on;
extern unsigned int g_Dmatchlen;

LL g_ll;

static LL g_sll;

static int g_kbdm[NKBDM];		/* Macro */

//static LINE * macro_start_line;
int   macro_start_col;
int 	g_last_cmd;

static int    g_slast_dir;
static int		g_kbdwr = 0;								// -ve => play


static void USE_FAST_CALL save_state(int wh)

{	static char g_savepat[NPAT+2];

	if (wh)
		strcpy(pat,g_savepat);
	else
	{	g_sll = g_ll;
		g_slast_dir = pd_lastdir;
		strcpy(g_savepat,pat);
	}
}


/* Begin a keyboard macro.
 * Error if not at the top level in keyboard processing.
 * Set up variables and return.
 */
int ctlxlp(int f, int n)

{ if (g_kbdwr > 0)
	{ mlwrite(TEXT105);	// Already active
		return FALSE;
	}

	g_kbdwr = 1;

//macro_start_line = curwp->w_dotp;
	
	macro_start_col = getccol();
	save_state(0);

	mlwrite(TEXT106);
				/* "[Start macro]" */

	return TRUE;
}


//extern int g_exec_level;

/* End keyboard macro. Check for the same limit conditions as the above
 * routine. Set up the variables and return to the caller.
 */
int  ctlxrp(int f, int n)

{ if (g_kbdwr == 0)
	{ mlwrite(TEXT107);
					/* "%%Macro not active" */
		return FALSE;
	}

  if (g_kbdwr < 0)						// cmd to be ignored 
		return TRUE;

  g_kbdwr = - g_kbdwr;
//g_kbdm[g_macro_last_pos] = 0;					// exclude the sequence for this cmd!
//g_macro_last_pos = NKBDM-1;
	pd_lastdir = g_slast_dir;
	save_state(1);

//#if _DEBUG
//if (g_exec_level)
//	mbwrite("g_exec_level not 0");
//#endif
//g_exec_level = 0;

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
//g_slastkey = lastkey;

	if (col >= macro_start_col && lused(curwp->w_dotp->l_dcr) > 0)
	{
		if (macro_start_col >= 0)
			curwp->w_doto = getgoal(macro_start_col, curwp->w_dotp);
						
		save_state(0);
				 
		if (g_kbdwr >= 0)
		{ mlwrite(TEXT105);	// Already active
			return FALSE;
		}

		mlwrite("%!%s", lastmesg);

		pd_kbdrep = n; 			/* remember how many times to execute */
		pd_kbdrd = 1;				/* at the beginning */
	}

	return TRUE;
}

/* Abort.
 * Beep the beeper. Kill off any keyboard macro, etc., that is in progress.
 * Sometimes called as a routine, to do general aborting of stuff.
 */
int  ctrlg(int f, int n)

{ /*TTbeep();*/
	mlwrite(TEXT8);
				/* "[Aborted]" */
	return ABORT;
}



/*	tgetc:	Get a key from the terminal driver, 
		resolve any keyboard macro action.
*/
int  tgetc()

{ int reskey;

	int kbdrd = pd_kbdrd;					/* if we are playing a keyboard macro back, */

  if (kbdrd > 0)
  {    								/* if there is some left... */
    while (true) /* once only */				
    { if (++kbdrd > -g_kbdwr)
      { kbdrd = 1;	/* reset the macro to the begining for the next rep */
        if (--pd_kbdrep <= 0)
          break;
      }
      pd_kbdrd = kbdrd;
			reskey = (int)g_kbdm[kbdrd-2];
			goto retlbl;
    }
							/* at the end of last repetition? */
    pd_kbdrd = 0;							/* mode STOP; */
//#if _DEBUG
//	if (g_exec_level)
//		mbwrite("g_exec_level not 0");
//#endif
		g_ll = g_sll;
    save_state(1);
#if VISMAC == 0
    update(FALSE);		/* force a screen update after all is done */
#endif
  }

	reskey = ttgetc();	   			/* fetch a char from the terminal driver */
	loglog2("TTGETC %x %c", reskey, reskey);
											  	 
	if (g_kbdwr > 0)								/* record it for $lastkey */
	{//char buf[30];
  	if (g_kbdwr > NKBDM)					/* don't overrun the buffer */
    	tcapbeep();
		else
	  	g_kbdm[++g_kbdwr-2] = reskey;
#if _DEBUG && 0
	{ char buf[40];
  	sprintf(buf,"Rec at %d %x",g_kbdwr-2, reskey);
 		mbwrite(buf);
	}
#endif
  }

retlbl:
  return pd_lastkey = reskey;
}

/*	getkey: Get one keystroke. The only prefixs legal here
			are the SPEC and CTRL prefixes.
*/
int  getkey()

{ int c = tgetc();

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
int  USE_FAST_CALL mlyesno(const char * prompt)

{ // flush_typah();
						/* " [y/n]? " */
  mlwrite("%!%s%s", prompt, TEXT162, null);
  flush_typah();
														/* get the response */
{ int c = getkey();   			/* was getcmd(); lets us check for anything that might */
													  /* generate a 'y' or 'Y' in case use screws up */
  if (c == g_abortc)
    return ABORT;

#if	FRENCH
  if (c=='o' || c=='O')
    return TRUE;
#endif

  return (c | 0x20) == 'y';
}}

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

#if S_MSDOS == 0
#define lwr(x) x
#else
#define lwr(x) tolower(x)
#endif

								    	  /* comp_command:	Attempt completion on a command name*/
static int USE_FAST_CALL comp_name(int cpos, int wh, char * name)
													/* command containing the current name to complete */
													/* ptr to position of next character to insert */
{
  if (wh == CMP_FILENAME)
	{ strcpy(name+cpos, "*");
    if (msd_init(name,
        		     MSD_DIRY|MSD_REPEAT|MSD_STAY|MSD_HIDFILE|MSD_SYSFILE|MSD_USEPATH|MSD_MATCHED)
	         != OK)
			return cpos;
	}

{	int best = -1;
	const Char * eny;
  int curbind = 0;
  BUFFER *bp = bheadp;                		/* trial buffer to complete */
                                                  /* Then the iterators */
  while (bp != NULL)
	{	int i;																	/* index into strings */
		char ch;

    if (wh == CMP_BUFFER)
    {        
        eny = bp->b_bname;
        bp = bp->b_next;
    }
    else 
    { if (wh == CMP_COMMAND)
       
        eny = names[++curbind - 1].n_name; /* trial command to complete */
    
      else /* if (wh == CMP_FILENAME) */
    
        eny = msd_nfile();

      if (eny == NULL)
        break;
    }
 /* mbwrite(eny); */
                     /* start attempting completions one character at a time */
    for (i = -1; (ch = eny[++i]); )
    {
    	if (name[i] == 0 || name[i] == '*')
    	{ name[i] = ch;
    		name[i+1] = 0;
    	}
#if S_MSDOS == 0
      if (name[i] != ch)
#else
      if (name[i] == '/' && ch == '\\')
        continue; 
      if (((name[i] ^ ch) & ~0x20) != 0)
#endif
        break;
    }

    if (i >= cpos && i < (best & 0xff))
    	best = i;
  } // loop

	if (best > 0)
	{ name[best] = 0;
		mlputs(0, name+cpos);
		TTflush();
		return best;
	}

  return cpos;
}}

static int USE_FAST_CALL redrawln(int clamp, char buf[])

{ if (clamp)
		clamp = term.t_ncol-ttcol;
{ int len = clamp;
  char * s;
  char c;

  for (s = &buf[-1]; (c = *++s) != 0; )
  {
    if (c == '\r')
    { len -= 4;
    //if (g_disinp > 0)
        mlputs(0, "<CR>");		/* put out <CR> for <ret> */
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
}}

static
int USE_FAST_CALL getwtxt(int lim, int from, int wh, char * buf)
	  														/* wh+('W'-'@') : B, F, G, K, N, S, W else whole line */
{ int offs = wh == 0 || wh == 'K'-'W' ? from : 0;
	LINE * lp = curwp->w_dotp;
	  
	int lct = llength(lp) - offs;
	if (lct > lim)
	  lct = lim;

{ char * t = buf;
  int	ct = 0;
	while (--lct >= 0)
	{ char c = lgetc(lp, offs);
	  if (! isword(c) && ct > 0 && wh == 0)
	  	break;
	  *t++ = c;
	  ++offs;
	  ++ct;
	}

  *t = 0;

  return wh == 0 ? offs : from;
}}

int g_chars_since_ctrl;

int g_gs_keyct;

static int getstr(char * buf, int nbuf, int promptlen, int gs_type)

{ char mybuf[NSTRING+2];
  char * autostr = "";
  int woffs = curwp->w_doto;
	int cc = TRUE;
	int llix = -1;
	int key_ct = 0;
  int fulllen = 0;    /* maximum buffer position */
  int cpos = 0;
	int redo = 0;
#if S_MSDOS == 0
	int twid = FALSE;
#endif
//int plain = gs_type - CMP_COMMAND;
  
  buf[0] = 0;

//++g_cursor_on;
  g_chars_since_ctrl = 1000;
  
  if (nbuf == 0)
  { buf[0] = getkey();
  	buf[1] = 0;
  }
  else
  for (;;)
  {	int c;             													/* current input character */
  	if (redo & 1)
    { redo -= 1;
      c = buf[cpos];			/* reseek */
      buf[cpos] = 0;
      tcapmove(255, promptlen + redrawln(0, buf));	// just calculate length
      buf[cpos] = c;
    }
  { int ch = *autostr;
    if (ch != 0)
    { ++autostr;
      goto getliteral;
    }

    if (redo)
    { redo = 0;
      tcapeeol();				/* redraw */
	    c = ttcol;
      (void)redrawln(1, &buf[cpos]);
      tcapmove(255, c);
    }
    TTflush();
    ch = getkey();
#if 0
    if (!plain && (ch == '-' || in_range(ch,'0','9'))
    { int val = 0;
    	int sign = 1;
    	if (ch == '-')
    	{ ch = getkey();
    		sign = -1;
    	}
    	++plain;
    	while (in_range(ch,'0','9'))
    		val = val * 10 + ch - '0';
    	if (sign < 0 && val == 0)
    		val = 1
    }
#endif
                              /* if they hit the line terminate, wrap it up */
    if (ch == G_STERM)
    { mlerase();                  /* clear the message line */
      cc = buf[0] != 0;						/* if we default the buffer, return FALSE */
      break;
    }

    if (ch == g_abortc)
    {	cc = ctrlg(FALSE, 0);    /* ABORT, Abort any kb macro */
    	buf[0] = 0;
    	break;
    }

    key_ct += 1;
	{	int chix = ch - (ALTD | '1');
		if ((unsigned)chix <= 2)
    { strcpy(mybuf, g_ll.lastline[g_last_cmd]);
#if 1
		{ int beg = 0;
			int cix = -1;
			while (1)
			{ ch = mybuf[++cix];
				if (ch <= ' ')
				{ mybuf[cix] = 0;
          if (ch == 0)
            break;
          if (--chix == 0)
						beg = cix+1;
				}
			}
			autostr = mybuf + (chix > 0 ? cix : beg);
		}
#else      
		{ int cix_ = strlen(autostr);
			int cix = cix_;
      mybuf[cix+1] = 0;
      mybuf[cix+2] = 0;
      for (; --cix > 0;)
      	if (mybuf[cix] <= ' ')
       	  mybuf[cix] = 0;
      while (--ch >= 0)
       	autostr += strlen(autostr)+1;
    }
#endif
    	redo = 3;
			continue;
		}
	}

	{	int tpos = cpos;
    if (ch & SPEC)
    {	ch -= SPEC;
      switch (ch)
      { case ('F'): tpos += 1;
        when ('B'): tpos -= 1;
        when ('<'): 													// beginning of line
							    { if 			(tpos != 0 || promptlen == 0)
							   		  tpos = 0;
							   	  else if (gs_type < 0)			// include the prompt
								    { fulllen += promptlen;
								    	promptlen = 0;
//							   		cpos = 0;
//										tcapmove(255, 0);
											strpcpy(buf, lastmesg, nbuf);
//							      continue;
								    }
								  }
        	
        when ('P'):
        case ('N'):														// up or down arrows
								   	llix += ch - 'O';
								    autostr = &g_ll.lastline[(g_ll.ll_ix-llix) & MLIX][0];

								    cpos = 0;
								    fulllen = 0;
								    buf[0] = 0;
								    redo = 3;
								    continue;
				when ('>'):	tpos = fulllen;
      }
      
	    if ((unsigned int)tpos > (unsigned)fulllen)
//    if (tpos < 0 || tpos > fulllen)
        /* tcapbeep() */ ;
      else
      { cpos = tpos;
        redo = 1;
      }
      continue;
    }
  }
                     /* change from command form back to character form */
    c = ectoc(ch);
#if S_MSDOS == 0
    if (c == '/' && twid && gs_type == CMP_FILENAME)
    { char * homedir = getenv("HOME");
      buf[cpos] = 0;
      if (cpos == 1 && homedir != NULL)
      { strpcpy(&mybuf[0], homedir, NSTRING-2);
      }
      else
      { homeusr(strpcpy(&mybuf[0], buf, NSTRING-2));
      }
      buf[0] = 0;
      cpos = 0;
      autostr = strcat(mybuf,"/");
      redo = 3;
      twid = FALSE;
      continue;
    }               
#endif
    if (c==0x7F || c==0x08)         /* rubout/erase */
    { if (cpos > 0)
      { int tpos;
				redo = 3;
        --fulllen;
        for (tpos = --cpos; ++tpos <= fulllen+1; )   /* include the 0 */
          buf[tpos-1] = buf[tpos];
      }
    }
    else if (c == 'A'-'@')
    { (void)buildlist(0);			// cc = TRUE
    	break;
    }
#if 0
    else if (c == 'F'-'@')
    { int tpos,ix;
      buf[cpos+1] = 0;
      for (tpos = cpos; --tpos >= 0 && !is_space(buf[tpos]); )
        ;
      ((char*)memcpy(mybuf,buf+tpos+1,cpos-tpos))[cpos-tpos] = 0;

      ix = comp_name(cpos - tpos - 1, CMP_FILENAME,mybuf);
      if (ix > cpos - tpos)
      { cpos = tpos+1+ix;
        fulllen = cpos;
        buf[cpos] = 0;
        memcpy(buf+tpos+1,mybuf,ix);
      }
    }
#endif
    else if ((c == ' '|| c == '\t' || c == 'F'-'@') && gs_type >= 0 && cpos > 0)
    {// if (gs_type == CMP_TOSPACE)
     //   break;

    { int typ = c == 'F'-'@' ? CMP_FILENAME : gs_type;
    	int tpos = comp_name(cpos, typ, buf);    /* attempt a completion */
      if (tpos > 0 && buf[tpos - 1] == 0)
        break;
      cpos = tpos;
      fulllen = cpos;
      buf[cpos] = 0;
    }}
    else if ((c <= 'Z'-'@' && c != 'I'-'@' || c == (ALTD | 'S')) && c != QUOTEC)
    {	switch (c)
      {	case 'B'-'@':
    			autostr = getkill();
				when 'N'-'@':
					if (1 || curbp->b_fname)
						autostr = curbp->b_fname;
				when 'S'-'@':
				case ALTD | 'S':
					if (pd_patmatch != NULL)
						autostr = pd_patmatch;

				when 0:										// All illegal CTRL.s mapped to /
					autostr = "/";
				otherwise
					autostr = mybuf;
		      woffs = getwtxt(NSTRING-3-cpos, woffs, c - ('W'-'@'), &mybuf[0]);
			}
    }
    else 
    { int tpos;
      if (c == QUOTEC)
      	ch = getkey();     /* get a character from the user */
      
getliteral:
      c = ectoc(ch);

#if S_MSDOS == 0
      if (cpos == 0 && c == '~')
        twid = 1;
#endif
      if (cpos >= nbuf - 2)
      { cpos = nbuf - 2;
      	autostr = "";
        tcapbeep();
      }

      for (tpos = ++fulllen; --tpos >= cpos; )
        buf[tpos+1] = buf[tpos];
      buf[cpos++] = c;
      if (fulllen > cpos)
      	redo = 2;
      
      if (g_disinp <= 0)
				c = '*';

#if S_MSDOS
    { short bb = c & 255;
      (void)redrawln(1, (char*)&bb);
    }
#else
			mlout(c);
#endif
    }
  }}

  g_gs_keyct = key_ct;
//++g_cursor_on;

	if (cc > FALSE)
  {	g_ll.ll_ix += 1;
  	strpcpy(&g_ll.lastline[g_ll.ll_ix & MLIX][0], buf, NSTRING-1);
  }

  return cc;
}

/*	A more generalized prompt/reply function allowing the caller
	to specify the proper terminator. If the sterm is not
	a return('\r'), return will echo as "<CR>"
*/
int  getstring(char * buf, int nbuf, const char * prompt)

{				  /* prompt the user for the input string */
  return getstr(buf, nbuf, mlwrite(prompt), -1);
}


static
char gs_buf[NFILEN+2];		/* buffer to hold tentative name */

static
char * complete(const char * prompt, char * defval, int type)
					/* prompt to user on command line */
					/* default value to display to user */
				/* type of what we are completing */
				/* maximum length of input field */

						 /* if executing a command line get the next arg and match it */
{ char * buf = gs_buf;
	if (g_macargs > 0)
    return macarg(buf) <= FALSE ? NULL : buf;

{ int plen = ! prompt ? 0
											: mlwrite(defval ? "%s[%s]: " :
																			   "%s: ",   prompt, defval);
	flush_typah();
{	int cc = getstr(buf, NFILEN, plen, type);

  return cc < 0				         ? NULL   : 
         defval && buf[0] == 0 ? strcpy(buf,defval) : buf;
}}}

	/* get a command name from the command line. Command completion means
	   that pressing a <SPACE> attempts to complete an unfinished command
	   name if it is unique.
	*/
Command getname(int wh)
				/* string to prompt with */
{ char *sp = complete(TEXT15+wh, NULL, CMP_COMMAND);

  return sp == NULL ? NULL : fncmatch(sp);
}


char * gtfilename(int wh)
				/* prompt to user on command line */
{
  return complete(wh ? TEXT132 : TEXT131, NULL, CMP_FILENAME);
}


/*	     getcbuf:	get a completion from the user for a buffer name.

						I was goaded into this by lots of other people's completion code.
*/
BUFFER * getcbuf(int createflag, BUFFER * bp, const char *prompt)
												/* prompt to user on command line */
{				
  char *sp = complete(prompt, bp ? bp->b_bname : "main", CMP_BUFFER);

  return sp == NULL ? NULL : bfind(sp, createflag);
}
