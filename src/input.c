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


#define MLIX 3

char lastline[MLIX+1][NSTRING];
int  ll_ix = MLIX;

char slastline[MLIX+1][NSTRING];
int  sll_ix;
char savepat[133];

int g_slastkey;

int kbdm[NKBDM];		/* Macro */

static const char text_105[] = TEXT105;

/*			"%%Macro already active" */

int    macro_last_pos;
int    macro_start_col;
LINE * macro_start_line;

char * pat_sv;
int slast_dir;

/* Begin a keyboard macro.
 * Error if not at the top level in keyboard processing.
 * Set up variables and return.
 */
int ctlxlp(int f, int n)

{ if (kbdmode != STOP)
	{ mlwrite(text_105);
		return FALSE;
	}

	mlwrite(TEXT106);
/*		"[Start macro]" */
	g_got_search = FALSE;

	kbdwr = 0;
	kbdmode = RECORD;
	macro_last_pos = 0;
	macro_start_col = curwp->w_doto;
	macro_start_line = curwp->w_dotp->l_bp;
	
	if (pat_sv == NULL) pat_sv = malloc(NPAT+10);
	memcpy(pat_sv,pat,NPAT+10);
	slast_dir = lastdir;
	return TRUE;
}


/* End keyboard macro. Check for the same limit conditions as the above
 * routine. Set up the variables and return to the caller.
 */
int  ctlxrp(int f, int n)

{ if (kbdmode == STOP)
	{ mlwrite(TEXT107);
/*			"%%Macro not active" */
		return FALSE;
	}

	if (kbd_record(kbdmode))
	{ /*
		int ix = kbdwr;
	  char buf[133];
		char c1[10], c2[10], c3[10];
		strcpy(c1, int_asc(kbdm[0]));
		strcpy(c2, int_asc(kbdm[1]));
		strcpy(c3, int_asc(kbdm[2]));

		while (--ix > 0 && in_range(kbdm[ix], 1,255))
		  ;

		concat(buf,c1,",",c2,",",c3,",", int_asc(ix), 0);
		mbwrite(buf);
			
	  concat(buf, TEXT108, ":", kbdm, "(", int_asc(kbdwr), ")",0);
		mbwrite(buf); */
		
	  if (macro_last_pos > 0)
			kbdm[macro_last_pos - 1] = 0;
		kbdwr = macro_last_pos - 1;
		kbdrd = 0; 
		kbdmode = STOP;
		g_execlevel = 0;

	  mlwrite(TEXT108);
/*			"[End macro]" */
		n = macro_start_col - curwp->w_doto;

		if (n != 0 && !g_got_search &&	macro_start_line == curwp->w_dotp->l_bp)
		{ int key = 2118;		// forward-character
			curwp->w_doto = macro_start_col;
			if (n < 0)
			{ n = -n;
				key = 2114;			// backward-character
			}
			while (--n >= 0)
		    if (kbdwr < NKBDM)
		      kbdm[(++kbdwr)-1] = key;
		}
	}

	memcpy(pat,pat_sv,NPAT+10);
	mcstr(-1);
	lastdir = slast_dir;

	return TRUE;
}



/* Execute a keyboard macro.
 * The command argument is the number of times to loop. Quit as soon as a
 * command gets an error. Return TRUE if all ok, else FALSE.
 */
int  ctlxe(int f, int n)

{
#define MLIX 3
	extern char lastline[MLIX+1][NSTRING];
	extern int	ll_ix;

	extern char slastline[MLIX+1][NSTRING];
	extern int	sll_ix;
	extern char savepat[133];

	g_slastkey = lastkey;

	if (g_got_search || curwp->w_dotp->l_used > macro_start_col)
	{
//	curwp->w_doto = macro_start_col;
						
		sll_ix = ll_ix;
		memcpy(slastline,lastline,sizeof(slastline));
		strpcpy(savepat,pat,133);
				 
		if (kbdmode != STOP)
		{ mlwrite(text_105);
			return FALSE;
		}

		if (n > 0)
		{ kbdrep = n; 	/* remember how many times to execute */
			kbdmode = PLAY; 	/* start us in play mode */
			kbdrd = 0;		/*		at the beginning */
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
	kbdmode = STOP;
	mlwrite(TEXT8);
/*		"[Aborted]" */
	return ABORT;
}



/*	tgetc:	Get a key from the terminal driver, 
		resolve any keyboard macro action.
*/
int  tgetc()

{				/* if we are playing a keyboard macro back, */
  if (kbd_play(kbdmode))
  {    					/* if there is some left... */
    while (true)		/* once only */				
    { if (++kbdrd > kbdwr)
      { kbdrd = 0;	/* reset the macro to the begining for the next rep */
        if (--kbdrep <= 0)
          break;
      }
			lastkey = (int)kbdm[kbdrd-1];
      return lastkey;
    }
					/* at the end of last repetition? */
		g_execlevel = 0;					/* weak code ! */
    kbdmode = STOP;
		lastkey = g_slastkey;
#if VISMAC == 0
    update(FALSE);		/* force a screen update after all is done */
#endif
    strcpy(pat,savepat);
    ll_ix = sll_ix;
    memcpy(lastline,slastline,sizeof(slastline));
  }

  lastkey = ttgetc();	       /* fetch a character from the terminal driver */
						   /* record it for $lastkey */
  if (kbd_record(kbdmode))
  {//char buf[30];
    kbdm[(++kbdwr)-1] = lastkey;
//  sprintf(buf,"Rec%x",lastkey);
//  mbwrite(buf);
    if (kbdwr >= NKBDM)				/* don't overrun the buffer */
    { kbdmode = STOP;
      TTbeep();
    }
  }

  return lastkey;
}

/*	getkey: Get one keystroke. The only prefixs legal here
			are the SPEC and CTRL prefixes.
*/
int  getkey()

{ register int c = tgetc();

#if 1				 /* if it exists, process an escape sequence */
  if (c == 0)
  { 
#define upper c
    upper = tgetc();	/* get the event type */
#if 0
    if (upper & (MOUS >> 8))
    {			      /* mouse events need us to read in the row/col */
      xpos = tgetc();		/* grab the x/y position of the mouse */
      ypos = tgetc();
    }
#endif				/* get the event code */
    c = tgetc() | (upper << 8);
  /*loglog2("AK %x %d", c, c);*/
  }
	/* yank out the control prefix */
#endif
/* was	return (c & 255) < 0x00 || (c & 255) > 0x1F ? c : CTRL | (c+'@'); */
  return (c & 255) <= 0x1f ? CTRL | (c+'@') :
		  	 (c & 255) == 0x88 ? CTRL | '\\':
#if S_MSDOS == 0
  	 (c & 255) == 0xa7 ? SPEC | CTRL | 'Y' :
  	 (c & 255) == 0xdb ? SPEC | CTRL | 'P' :
  	 (c & 255) == 0xdd ? SPEC | CTRL | 'C' :
#if 0
>   	 (c & 255) == 0xa7 ? SPEC | CTRL | 'Y' : /* next page */
>   	 (c & 255) == 0xdb ? SPEC | CTRL | 'O' : /* previous page!! */
>   	 (c & 255) == 0xdd ? SPEC | CTRL | 'C' : /* copy prev line */
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

/*	GETCMD: Get a command from the keyboard. Process all applicable
		prefix keys
*/
int  getcmd()

{ register int c = getkey();
	  KEYTAB * key = getbind(c);
					/* resolve META and CTLX prefixes */
  if (key->k_code != 0)
  do
  { 
    if	    (key->k_ptr.fp == meta)
      c = META;
    else if (key->k_ptr.fp == cex)
      c = CTLX;
    else
      break;
    c = getkey() | c;
#if 1
    if (in_range(c & 0xff,'a','z'))
      c &= ~0x20;			/* Force to upper */
#endif
  } while (0);
  return c;
}

/*
 * Ask a yes or no question in the message line. Return either TRUE, FALSE, or
 * ABORT. The ABORT status is returned if the user bumps out of the question
 * with a ^G. Used any time a confirmation is required.
 */
int  mlyesno(char * prompt)

{ int  c = g_discmd;		/* input character */
  
  g_discmd = TRUE;
						/* " [y/n]? " */
  mlwrite("%s%s", prompt, TEXT162, null);
  g_discmd = c;
							/* get the response */
  c = getcmd();   /* getcmd() lets us check for anything that might */
			  /* generate a 'y' or 'Y' in case use screws up */
  if (c == ectoc(abortc))		/* Bail out! */
    return ABORT;

#if	FRENCH
  if (c=='o' || c=='O')
    return TRUE;
#endif

  return c == 'y' || c == 'Y';
}

/*	ectoc:	expanded character to character
		collapse the CTRL and SPEC flags back into an ascii code   */

int  ectoc(int c)
	
{ if (c & CTRL)
    c &= ~(CTRL | 0x40);
  return c & SPEC ? c & 255 : c;
}

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
    register char * ln;
    if (pw == NULL || (ln = fgets(&buf[0], NSTRING-1, pw)) == NULL)
    { strcpy(&buf[0], "??");
      break;
    }
    while (*ln == *s++ && *ln != 0)
      ++ln;
    if (*ln == ':')
    { register int iter = 4;
      while (--iter >= 0)
      { ++ln;
        while (*ln != ':' && *ln != 0)
          ++ln;
      }
      iter = NSTRING - 1;
      if (*ln != 0)
        ++ln;
    
    { char * t = &buf[0];
      
      while (--iter > 0 && (*t = *ln) != ':' && *ln++ != 0)
	++t;
      *t++ = '/';
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

	    /*	comp_command:	Attempt a completion on a command name	*/
static int comp_name(char * name, int cpos, int wh)
			/* command containing the current name to complete */
			/* ptr to position of next character to insert */
{
	int trash;
	int i;															/* index into strings */
	int comflag = FALSE;   							/* was there a completion at all? */

            /* start attempting completions, one character at a time */
  for ( ; cpos < NSTRING; ++cpos)
  { Bool match = false;
    BUFFER *bp = bheadp;                		/* trial buffer to complete */
    int curbind = g_numcmd;

    if (wh == CMP_FILENAME)
    {   
      strcpy(&name[cpos], "*");
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
        bp = bp->b_bufp;
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
      {               /* if this is the first match, simply record it */
        if (! match)
        { match = true;
          name[i] = lwr(eny[i]);
          if (wh == CMP_FILENAME) name[i+1] = 0;
        }
                                        /* if there's a difference, stop here */
        if (lwr(name[i]) != lwr(eny[i]))
          return i;
      }
    } /* while over entries */
                                               /* with no match, we are done */
    if (! match)
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
      
{ register int len = clamp;
           char * s;
  register char c;

  for (s = &buf[-1]; (c = *++s) != 0; )
  { 
    if (c == '\r')
    { if ((len -= 4) >= 0)
        outstring("<CR>");		/* put out <CR> for <ret> */
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
int g_chars_since_shift;


int gs_keyct;

static int getstr(char * buf, int nbuf, int promptlen, int gs_type)
	  
{ int llix = -1;
  int llcol = -1;
  int c;             /* current input character */
  int fulllen = 0;    /* maximum buffer position */
  int cpos = 0;
  char * autostr = "";
#if S_MSDOS == 0
	int twid = FALSE;
#endif
	int redo = 0;

  buf[0] = 0;

  g_chars_since_shift = 10000;
  gs_keyct = 0;
  g_txt_woffs = curwp->w_doto;

  for (;;)
  { if (redo & 1)
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
      c = ectoc(ch);
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
    gs_keyct += 1;
                     /* change from command form back to character form */
    c = ectoc(ch);
#if S_MSDOS == 0
    if (c == '/' && twid && gs_type == CMP_FILENAME)
    { extern char * homedir;
      buf[cpos] = 0;
      if (cpos == 1)
      { strcat(strpcpy(&combuf[0], homedir, NSTRING-1), "/");
      }
      else
      { strpcpy(&combuf[0], buf, NSTRING-2);
        homeusr(&combuf[0]);
      }
      buf[0] = 0;
      cpos = 0;
      autostr = &combuf[0];
      redo = 3;
      twid = FALSE;
      continue;
    }               
#endif
    if (c == ectoc(abortc))
    { ctrlg(FALSE, 0);    /* Abort the input? */
      return ABORT;
    }
#define tpos c
    if (c==0x7F || c==0x08 || c==0x15)         /* rubout/erase */
    { if (cpos > 0)
      { --cpos;
        for (tpos = cpos; ++tpos <= fulllen; )   /* include the 0 */
          buf[tpos-1] = buf[tpos];      
        --fulllen;
				redo = 3;
      }
    } 
    else if (ch == (CTRL | 'A') && gs_type >= 0)
    { buildlist(null);
      return ABORT;
    }               
    else if (ch == (CTRL | 'F'))
    { int ix;
      char mybuf[256];
      buf[cpos+1] = 0;
      for (tpos = cpos; --tpos >= 0 && !isspace(buf[tpos]); )
        ;
      memcpy(mybuf,buf+tpos+1,cpos-tpos);
      mybuf[cpos-tpos] = 0;
      ix = comp_name(mybuf, cpos - tpos - 1, CMP_FILENAME);
      if (ix > cpos - tpos)
      { memcpy(buf+tpos+1,mybuf,ix);
        buf[tpos+1+ix] = 0;
        cpos = tpos+1+ix;
        fulllen = cpos;
      }
    }
    else if (gs_type >= 0 && (c == ' '|| c == ectoc(sterm) || c == '\t'))
    { tpos = comp_name(buf, cpos, gs_type);    /* attempt a completion */
      if (buf[tpos - 1] == 0)
        break;
      cpos = tpos;
      buf[cpos] = 0;
      fulllen = cpos;
    }
    else if (ch & SPEC)
    { tpos = cpos;
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
            autostr = &lastline[(ll_ix-llix) & MLIX][0];
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
    else if (c <= 'Z'-'@' && c != quotec || c == (ALTD | 'S'))
    { autostr = getwtxt(c, &combuf[0], NSTRING-3-cpos);
    }
    else 
    { char mybuf[3];

      if (c == quotec)
      { ch = getkey();     /* get a character from the user */
        c = ectoc(ch);
      }
getliteral:
      
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
      for (tpos = fulllen+1; --tpos >= cpos; )
        buf[tpos+1] = buf[tpos];
      buf[cpos] = ch;
      
#if S_MSDOS
      if (g_disinp <= 0)
				ch = '*';
      mybuf[0] = ch;
      mybuf[1] = 0;
      (void)redrawln(mybuf, term.t_ncol-ttcol);
#else
			mlout(ch);
#endif
      cpos++;
      fulllen++;
      if (fulllen > cpos)
      { redo = 2;
      }
    }
  }}
  ll_ix += 1;
  strpcpy(&lastline[ll_ix & MLIX][0], buf, NSTRING-1);
  return TRUE;
}

/*	A more generalized prompt/reply function allowing the caller
	to specify the proper terminator. If the sterm is not
	a return('\r'), return will echo as "<CR>"
*/
int  getstring(char * buf, int nbuf, const char * prompt)

{				  /* prompt the user for the input string */
  Bool scoo = cursor_on_off(false);
  int res = getstr(buf, nbuf, mlwrite(prompt), -1);
  (void)cursor_on_off(scoo);
  return res;
}


char gs_buf[NSTRING+2];		/* buffer to hold tentative name */

char * complete(char * prompt, char * defval, int type, int maxlen)
					/* prompt to user on command line */
					/* default value to display to user */
				/* type of what we are completing */
				/* maximum length of input field */
{
		 /* if executing a command line get the next arg and match it */
  if (g_clexec)
    return macarg(gs_buf) != TRUE ? NULL : gs_buf;

#if COMPLET == 0
  strcpy(gs_buf, prompt);
  if (type != CMP_COMMAND && defval)
    concat(&gs_buf[0], "[", defval, "]", null);

{ int cc = mlreply(strcat(&gs_buf[0], ": "), gs_buf, maxlen);
  return cc == ABORT	          	? NULL   : 
         defval && gs_buf[0] == 0 ? defval : gs_buf;
}
#else
{ int plen = ! prompt ? 0
											: mlwrite(type==CMP_COMMAND ? "%s"	 :
															  defval	    			? "%s[%s]: " :
																			              "%s: ",   prompt, defval);
  Bool scoo = cursor_on_off(false);
  int cc = getstr(gs_buf, NSTRING-2, plen, type);
  (void)cursor_on_off(scoo);
  return cc == FALSE && defval ? defval :
         cc == ABORT	         ? null   : gs_buf;
}
#endif
}

	/* get a command name from the command line. Command completion means
	   that pressing a <SPACE> attempts to complete an unfinished command
	   name if it is unique.
	*/
int ( * getname(char * prompt))(int, int)
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


char * gtfilename(char * prompt)
				/* prompt to user on command line */
{
  return complete(prompt, NULL, CMP_FILENAME, NFILEN);
}

void  outstring(const char * s) /* output a string of input characters */

{ if (g_disinp > 0)
    mlputs(s);
}


void  ostring(const char * s)	/* output a string of output characters */

{
  if (g_discmd)
    mlputs(s);
}

