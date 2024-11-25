/*	This file is for functions dealing with execution of
	commands, command lines, buffers, files and startup files

	written 1986 by Daniel Lawrence 			*/

#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"

static int Pascal docmd(char * cline);


/*	The !WHILE directive in the execution language needs to
	stack references to pending whiles. These are stored linked
	to each currently open procedure via a linked list of
	the following structure
*/

typedef struct WHBLOCK
{ struct WHBLOCK *w_next;	/* next while */
	LINE *w_begin;					/* ptr to !while statement */
	LINE *w_end;						/* ptr to the !endwhile statement*/
	char  w_break;					/* block type */
} WHBLOCK;

/* directive name table:
	This holds the names of all the directives....	*/

static const char dname[][8] =
{	"if", "else", "endif",
	"goto", "return", "endm",
	"while", "endw", "break",
	"force"
};

#define NUMDIRS (sizeof(dname) / 8)
#define LIM_WHILE 0


static struct BUFFER * g_bstore = NULL;				/* buffer to store macro text to*/

//int g_exec_level = 0;												/* execution IF level		*/

extern int (Pascal *g_bfunc)(int, int);				/* from bind.c */

#if BACKCH

err err

void logstr(char * str)
		/* append the current command to the log file */
{ FILE * fp = fopen("emacs.log", "a");
  fprintf(fp, str, ((Int*)&str)[1],
					  		   ((Int*)&str)[2],
					  		   ((Int*)&str)[3]);
  fclose(fp);
}

#endif

static
BUFFER * Pascal USE_FAST_CALL bmfind(int create, int n)

{ char ebuffer[NBUFN+1];
	strcpy(ebuffer, "*Macroxx");
  																			/* make the buffer name */
	if (n > 0)					
	{ ebuffer[6] = '0' + (n / 10);
	  ebuffer[7] = '0' + (n % 10);
	}
	else /* n is 0 or -1 */
	{																				/* find buffer user wants to execute */
		int cc = mlreply(TEXT117, &ebuffer[-n], NBUFN);
											/* "Execute buffer: " */
		if (cc <= FALSE)
		  return NULL;
	}
					/* find the pointer to that buffer */
	return bfind(ebuffer, create);
}

static
char * Pascal skipleadsp(char * s, int len)
	
{
  while (--len >= 0 && (*s == ' ' || *s == '\t'))
    ++s;
  return s;
}


#if _DEBUG
int g_rc_lineno;
#endif


#if LIM_WHILE
#define freewhile(x)
#else

static
void Pascal freewhile(WHBLOCK * wp)/* free a list of while block pointers */
				/* head of structure to free */
{
	if (wp != NULL)
	{ freewhile(wp->w_next);
	  free((char*)wp);
	}
}

#endif

static 
char *g_execstr = NULL; 								/* pointer to string to execute */

BUFFER * g_dobuf;

static 
int Pascal dobuf(BUFFER * bp, int iter, const char * name)
			 /* iter:   # times to do it */
{
	char smalleline[80];
#if	LOGFLG
	FILE *fp;		/* file handle for log file */
#endif
	int cc = TRUE;
	int lineno = 0;
#if LIM_WHILE
	WHBLOCK whiles[LIM_WHILE];
	int topwh = LIM_WHILE-1;
#endif
	char *s_execstr = g_execstr; 		/* original exec string */

#if _DEBUG
	g_rc_lineno = 0;
#endif

	g_dobuf = bp;
	g_univct = iter;

  while (--iter >= 0 && cc > FALSE)
	{ LINE *lp;						/* pointer to line to execute */
		char *ebuf = smalleline;	/* initial value of eline */
		char *eline;				/* text of line to execute */
		char tkn[NSTRING];	/* buffer to evaluate an expresion in */

  	WHBLOCK *whlist = NULL; /* ptr to !WHILE list */
		WHBLOCK *scan = NULL;		/* ptr during scan */
												    /* scan the buffer, building WHILE header blocks */
		int nest_level = 0;
		int msg = 0;
		int exec_level = 0;			/* clear IF level flags/while ptr */

		for (lp = &bp->b_baseline; !l_is_hd((lp=lforw(lp))); ) 
		{ 					   													/* scan the current line */
																						/* trim leading whitespace */
		  int len = lused(lp->l_dcr);
		  eline = skipleadsp(lp->l_text, len);
		  
		  if (len + (lp->l_text - eline) < 4)
		  	continue;
																/* if is a while directive, make a block... */
			if (eline[0] != '!')
				continue;
	  
		{ char ch = eline[1] - 'w';
			if (ch == 0				  && eline[2] == 'h' ||
					ch == 'b' - 'w' && eline[2] == 'r')
			{ 
#if LIM_WHILE
				WHBLOCK * whtemp = topwh <= 0 ? NULL : &whiles[topwh--];
#else			
				WHBLOCK * whtemp = (WHBLOCK *)mallocz(sizeof(WHBLOCK));
#endif
		 		if (whtemp == NULL ||
	     															/* "%%!BREAK outside of any !WHILE loop" */
	         (scan == NULL && ch != 0))
					goto failexit;

		    whtemp->w_break = ch;
		    whtemp->w_begin = lp;
	  	  whtemp->w_next = scan;
//  	  scan->w_end = 0;
	    	scan = whtemp;
	  	}
												/* if it is an endwhile directive, record the spot.. */
		  if (strcmp_right(&eline[1], &dname[DENDWHILE][0]) == 0)
		  { if (scan == NULL)
	 	    { 			/* %%mismatched !ENDWHILE in %s */
	 	    	goto failexit;
	 			}											/* move top records from the scan list to the
															   whlist until we have moved all BREAK records */
			  do										/* and one WHILE record */
				{ WHBLOCK * whtemp = whlist;
					scan->w_end = lp;
		 			whlist = scan;
		 			scan = scan->w_next;
		 			whlist->w_next = whtemp;
	    	} while (whlist->w_break);
	  	}
		}} /* for */

		if (scan != NULL)
	  { freewhile(scan);
		  eline = "WHILE";		/* %%mismatched !WHILE in '%s' */
failexit:     
	    mlwrite(TEXT121, bp->b_bname, eline);
		  cc = FALSE;
		}
								/* let the first command inherit the flags from the last one..*/
		g_thisflag = g_lastflag;
																	/* starting at the beginning of the buffer */
		lp = &bp->b_baseline; 
		while (1)
		{	if (msg)
			{ mlwrite(msg > 0 ? TEXT124 : TEXT126);
				msg = 0;
	      cc = FALSE;
			}
			if (ebuf != smalleline)
		    free(ebuf);

	    if (g_eexitflag || cc <= FALSE ||
				  l_is_hd((lp = lforw(lp))))
				break;

			++lineno;
#if _DEBUG
			++g_rc_lineno;
#endif
													/* allocate eline and copy macro line to it */
		{ char golabel[20];
			int dirnum = -1;		/* directive index */
			int linlen = lused(lp->l_dcr)+1;
		  if (linlen < sizeof(smalleline))
			  ebuf = smalleline;
	    else 
		  { ebuf = (char *)malloc(linlen);
		    if (ebuf == NULL)
		    { cc = FALSE;
		      break;
		    }
		  }
																						/* trim leading whitespace */
		  eline = skipleadsp(strpcpy(ebuf, lp->l_text, linlen), linlen);
																				   
		  if (*eline == 0 || *eline == ';' ||		/* dump comments and blank lines */
		  		*eline == '*')
		    continue;

#if LOGFLG
		  logstr(eline); logstr("\n");
#endif
																				/* only do this if we are debugging */
#if	DEBUGM
		  if (pd_macbug && g_bstore == null && exec_level == 0)
		    if (debug(bp, eline) == FALSE)
		    { mlwrite("%!"TEXT54); /*	"[Macro aborted]" */
		      cc = FALSE;
		      continue;
		    }
#endif
																				/* Parse directives here.... */
		  if (*eline == '!')								/* Find out which directive this is */
		  {
		    for (dirnum = NUMDIRS; --dirnum >= 0; )
		      if (strcmp_right(eline+1, dname[dirnum]) == 0)
						goto jover__;
																	/* bitch if it's illegal */
		    msg += 1;		    					/* "%%Unknown Directive" */
		    continue;
jover__:;
		  }
																		/* if macro store is on, salt this away */
		  if (g_bstore != null && dirnum != DENDM)
		  { LINE * mp = mk_line(ebuf,linlen-1, linlen-1,0);

																		/* attach the line to the end of the buffer */
        ibefore(&g_bstore->b_baseline, mp);
		    continue;
		  }
																		/* now, execute directives */
		  if (dirnum >= 0)
		  { while (*eline > ' ')						/* skip past the directive */
		      ++eline;

				if (dirnum == DRETURN)
				{	if (exec_level == 0)
						break;													// is smalleline
					continue;
				}
				else
		    switch (dirnum)
		    { case DIF: 				/* IF directive */
						++nest_level;
		      case DWHILE:			/* WHILE directive */
																				/* grab the value of the logical exp */
						if (exec_level == 0)
						{ g_execstr = eline;
							cc = macarg(tkn);
							if (cc <= FALSE || stol(tkn))
								continue;
																							/* expression evaluated false */
						  if (dirnum == DIF)
						  	exec_level |= (1 << nest_level);
						}
																			/* drop down and act just like !BREAK */
		      case DBREAK:			/* BREAK directive */
						if (dirnum == DBREAK && exec_level)
						  continue;
																			/* jump down to the endwhile */
																			/* find the right while loop */
						if (dirnum != DIF)
						{ WHBLOCK* whtemp;
							for (whtemp = whlist; whtemp; whtemp = whtemp->w_next) 
							  if (whtemp->w_begin == lp)
							    goto jover;
        
							msg -= 1;				/* "%%Internal While loop error" */
							continue;
jover:														    /* reset the line pointer back.. */
							lp = whtemp->w_end;
				    }

						continue;

		      when DELSE:				/* ELSE directive */
						exec_level ^= (1 << nest_level);
						continue;

		      when DENDIF:			/* ENDIF directive */
						exec_level &= ~ (1 << nest_level);
						--nest_level;
						continue;

					when DENDM:
		    		if (g_bstore != NULL)
		    		{ g_bstore->b_dotp = lforw(&g_bstore->b_baseline);
				    	g_bstore = NULL;
				    }
		  	    continue;

		  	  default:
						if (exec_level == 0)							/* execute the statement */
																	/* .....only if we are currently executing */
							if      (dirnum == DGOTO)
							{ 
								g_execstr = eline;
								(void)token(golabel, sizeof(golabel));
								 
								for (lp = &bp->b_baseline; 
								     !l_is_hd((lp=lforw(lp))); )
								  if (*lp->l_text == '*' &&
											strcmp_right(&lp->l_text[1],golabel) == 0)
										break;

								if (l_is_hd(lp))
								{	mlwrite(TEXT127, golabel);				/* "%%No such label" */
								 	cc = FALSE;
								}
								continue;
							}
							else if (dirnum == DENDWHILE)
							{ WHBLOCK * whtemp; 
							  for (whtemp = whlist; whtemp; whtemp = whtemp->w_next)
							    if (whtemp->w_break == 0 &&
											whtemp->w_end == lp)
							      goto jover_;
			        
							  msg -= 1;						/* "%%Internal While loop error" */
							  continue;
jover_:							  									/* reset the line pointer back.. */
							  lp = lback(whtemp->w_begin);
								continue;
							}
				}
		  }

			if (exec_level == 0)							/* execute the statement */
			{	
				cc = docmd(eline);
			  if (dirnum == DFORCE)	/* FORCE directive */
			    cc = TRUE;
																							/* check for a command error */
			  if (cc <= FALSE)
			  { cc = -lineno;
#if 0
			    for (wp = wheadp; wp != NULL; wp = wp->w_next)
			      if (wp->w_bufp == bp)	     /* and point it */
			      { wp->w_dotp = lp;
							wp->w_doto = 0;
			        wp->w_flag |= WFHARD;
			      }
#endif
			    bp->b_dotp = lp;									/* in any case set the buffer. */
			    bp->b_doto = 0;
			    if (is_opt('M'))				// MD_KEEP_MACROS
			      bp->b_flag &= ~BFINVS;
			  }
			}
		}} // loop

		freewhile(whlist);
	}

	g_execstr = s_execstr;
	g_dobuf = NULL;
  return cc;
#undef tkn
}


static Cc finddo(int f, int n, Command fnc, char * tkn, const char * diag)

#if 0
{	if (fnc == NULL)
		fnc = fncmatch(tkn);					/* match the token to see if it exists */

{	Cc cc;
	KEYTAB kt;
	kt.k_code = 1;
//kt.k_type = BINDFNC;
	kt.k_ptr.fp = fnc;
	
	if (fnc == NULL)								/* find the pointer to that buffer */
	{ kt.k_ptr.fp = bfind(tkn, FALSE); 
//	kt.k_type = BINDBUF;
	}

	return execkey(&kt, f, n);
}}
#else
{	Cc cc;

	if (fnc == NULL)
		fnc = fncmatch(tkn);					/* match the token to see if it exists */

	if (fnc != NULL)
	{ 
		cc = in_range((int)fnc, 1, 40) ? execporb((int)fnc, n)
																	 : (*fnc)(f, n);			/* call the function */
	}
	else													/* find the pointer to that buffer */
	{ BUFFER *bp = bfind(tkn, FALSE); 
	  if (bp == NULL) 
		{ mlwrite(TEXT16, *tkn ? tkn : diag);
						/* "[No such Function]" */
			return FALSE;
		}
		cc = dobuf(bp,n,tkn);
	}

	return cc;
}
#endif

/*	docmd:	take a passed string as a command line and translate
		it to be executed as a command. This function will be
		used by execute-command-line and by all source and
		startup files. Lastflag/thisflag is also updated.

	format of the command line is:

		{# arg} <command-name> {<argument string(s)>}

*/
static
int Pascal docmd(char * cline)
			/* command line to execute */
{	  	  	  	  		     /* if we are scanning and not executing..go back here */
	char *s_execstr = g_execstr; 		/* original exec string */

	g_execstr = cline;	      						/* and set this one as current */

{	int f = g_got_uarg;
  g_got_uarg = FALSE;								// The 1st command in the buffer gets the arg

{	int n = ! f ? 1 : g_univct;  					/* numeric repeat value */
//int (Pascal *fnc)(int, int);      	  /* function to execute */
//Command fnc;      	  								/* function to execute */
	char ebuffer[132];
#define tkn ebuffer+1
	++g_macargs;	      									/* in cline execution */

	g_lastflag = g_thisflag;
	g_thisflag = 0;

{ int	cc = macarg(tkn);									/* TRUE, FALSE, ABORT */
	if (cc > FALSE)
		if (*(tkn) < 'A')
		{ f = TRUE;
		  n = atoi(getval(tkn, tkn));
														      /* and now get the command to execute */
		  cc = macarg(tkn);
		}  

	if (cc > FALSE)
	{	
#if 1
		cc = finddo(f,n,NULL,tkn, cline);
#else
		if (fnc != NULL)
		{ 
			cc = in_range((int)fnc, 1, 40) ? execporb((int)fnc, n)
																		 : (*fnc)(f, n);			/* call the function */
		}
		else													/* find the pointer to that buffer */
		{ BUFFER *bp = bfind(tkn, FALSE); 
		  if (bp == NULL) 
			{ mlwrite(TEXT16, "???");
							/* "[No such Function]" */
				cc = FALSE;
			}
			else 												/* execute the buffer */
				cc = dobuf(bp,n, tkn);
		}
#endif
		pd_cmdstatus = cc;						/* save the status */
//	g_lastfnc = fnc;
	}
	--g_macargs;										/* restore g_macargs flag */
	g_execstr = s_execstr;
	return cc;
#undef tkn
}}}}


/* namedcmd:	execute a named command even if it is not bound */

int Pascal namedcmd(int f, int n)
	  /* command arguments [passed through to command executed] */
{
	int (Pascal *kfunc)(int, int);/* ptr to the function to execute */
	int scle = g_macargs;
	int cc;

	if (! scle)
																	/* prompt the user to type a named command */
	  kfunc = getname(0);						/* and get the function name to execute */
	else
	{	char ebuffer[40];
																						/* grab token and advance past */
	  (void)token(ebuffer, sizeof(ebuffer));
							     /* evaluate it */
	{ const char * fnm = getval(ebuffer, ebuffer);
	  if (fnm == g_logm[2])
	    return FALSE;
																								/* and look it up */
	  kfunc = NULL;
	}}

#if 1
	cc = finddo(f,n,kfunc,"", "?");
#else
	if (kfunc != NULL)
	{ // g_macargs = FALSE;
																												/* call the function */
		cc = in_range((int)kfunc, 1, 40) ? execporb((int)kfunc, n)
																		 : (*kfunc)(f, n);
	  // g_macargs = scle;
	}
	else
	{ mlwrite(TEXT16, "???");
					/* "[No such function]" */
	  cc = FALSE;
	}
#endif
	return cc;
}

#if FLUFF || DEBUGM

/*	execcmd:	Execute a command line command to be typed in
			by the user					*/
int Pascal execcmd(int f, int n)

{
	char cmdnm[NSTRING];		/* string holding command to execute */
						      /* get the line wanted */
	int cc = mlreply(": ", cmdnm, NSTRING);
	if (cc > FALSE)
	  return cc;

//g_exec_level = 0;
	return docmd(cmdnm);
}
#endif


/* token:	chop a token off a string
		return a pointer past the token
*/

char *Pascal token(char * tok, int size)
				/* source string, destination token string */
				/* maximum size of token */
{
#if 1
	const char * src = g_execstr;
	char quotef = 0;	/* is the current string quoted? */
	char c;
	int leading = 1;

	for (; (c = *src) != 0; ++src)
	{														/* process special characters */
		if 			(leading < 0)		
	    switch (c)
	    { case 'r': c = 'J' - '@';
	      when 'n': c = 'M' - '@'; 
	      when 'l': c = 'L' - '@'; 
	      when 't': c = 'I' - '@';  
	      when 'b': c = 'H' - '@';  
	    }
	  else if (c == '~')
	  	leading = -1;
	  else if (c == quotef)									/* check for the end of the token */
	  	break;
	  else if (c == ' ' || c == '\t')
	  { if (leading)
	      continue;
	    if (quotef == 0)										/* terminates unless in quotes */
	      break;
	  }
	  else if ((c == '"' || c == '\'') && leading > 0)
	  { quotef = c;													/* set quote mode if quote found */
	    leading = 0;
	    continue;
	  }
	  if (--size <= 0)
	  	break;
	  *tok++ = c;
	  leading = 0;
	}

	*tok = 0;
	return g_execstr = (char*)(*src == 0 ? src : src+1);
#else
	const char * src = g_execstr - 1;
	char quotef = 0;	/* is the current string quoted? */
	char c;
	int leading = -1;

	for (; (c = *++src) != 0;)
	{	if (c == quotef)										/* check for the end of the token */
	  	break;
		++leading;
		if (c == '~')													/* process special characters */
	    switch (*++src)
	    { case 'r': c = 'J' - '@';
	      when 'n': c = 'M' - '@'; 
	      when 'l': c = 'L' - '@'; 
	      when 't': c = 'I' - '@';  
	      when 'b': c = 'H' - '@';
	      otherwise --src;
	    }
	  else if (c <= ' ')
		{	if (leading == 0)
				continue;
		  if (quotef == 0)										/* terminates unless in quotes */
	      break;
	  }
	  else if ((c == '"' || c == '\'') && quotef + leading == 0)
	  { quotef = c;													/* set quote mode if quote found */
	    continue;
	  }
	  if (--size <= 0)
	  	break;
	  *tok++ = c;
	}

	*tok = 0;
	return g_execstr = (char*)src+!!c;
#endif
}


int Pascal macarg(char * tok) /* get a macro line argument */
				/* buffer to place argument */
{ ++g_macargs;
{	int res = nextarg(null, tok, NSTRING);
  --g_macargs;
	return res;      
}}

/*	nextarg:	get the next argument	*/

int Pascal nextarg(const char * prompt, char * buffer, int size)
				/* prompt to use if we must be interactive */
				/* buffer to put token into */
				/* size of the buffer */
{	extern int g_gs_keyct;
	g_gs_keyct = 0;

	if (g_macargs <= 0 || g_execstr == NULL)		/* we are interactive */
	  return getstring(buffer, size, prompt);
																			      /* grab token and advance past */
	(void)token(buffer, size);

	return getval(buffer, buffer) != getvalnull;/* evaluate it ** no protection!*/
}



static int USE_FAST_CALL common_return(BUFFER * bp)
	
{	if (bp == NULL)
	{ mlwrite(TEXT99);				/*	out of memory */
	  return FALSE;
	}
														/* and make sure it is empty */
	bp->b_flag |= BFINVS;
	g_bstore = bp;
	return bclear(bp);
}

/*	storemac:	Set up a macro buffer and flag to store all
			executed command lines there			*/

int Pascal storemac(int f, int n)
	/* int n;		** macro number to use */
{
	if (f == FALSE || ! in_range(n, 1,40))
	{ mlwrite(f == FALSE ? TEXT111		/* must have a numeric argument */
											 : TEXT112);
/*			"No macro specified" */
/*			"Macro number out of range" */
	  return FALSE;
	}

	return common_return(bmfind(TRUE, n));	/*set up the new macro buffer */
}


int Pascal execporb(int isp, int n)
										 /* isp; ** must be 0, or 1 to 40 */
{	BUFFER * bp = bmfind(FALSE, isp);
	if (bp == NULL) 
	{ mlwrite(TEXT130);
										/* "Macro not defined" */
		return FALSE;
	}
					/* and now execute it as asked */
	return dobuf(bp,n,"");
}

/*	execbuf:	Execute the contents of a buffer of commands	*/

int Pascal execbuf(int f, int n)
	/* int f, n;	** default flag and numeric arg */
{ 
  return execporb(0,n);
}

/*	dobuf:	execute the contents of the buffer pointed to
		by the passed BP

	Directives start with a "!" and include:

	!endm		End a macro
	!if (cond)	conditional execution
	!else
	!endif
	!return 	Return (terminating current macro)
	!goto <label>	Jump to a label in the current macro
	!force		Force macro to continue...even if command fails
	!while (cond)	Execute a loop if the condition is true
	!endwhile
        
	Line Labels begin with a "*" as the first nonblank char, like:

	*LBL01
*/


#if	DEBUGM
/*		Interactive debugger

		if $debug == TRUE, The interactive debugger is invoked
		commands are listed out with the ? key			*/

int Pascal debug(BUFFER* bp, char * eline)
	/* bp;	        ** buffer to execute */
	/* eline;	** text of line to debug */
{
	int c; 									/* temp character */
	KEYTAB *key;						/* ptr to a key entry */
	static char track[NSTRING] = "";/* expression to track value of */
	       char temp[NSTRING];
	int oldcmd = pd_discmd;
	int oldinp = g_disinp;
	
dbuild: /* Build the information line to be presented to the user */
	pd_discmd = oldcmd;
	g_disinp = oldinp;

	strcpy(outline, "<<<");
					/* display the tracked expression */
	if (track[0] != 0)
	{ int oldstatus = pd_cmdstatus;
	  docmd(track);
	  pd_cmdstatus = oldstatus;
	  concat(&outline[0], "[=", gtusr("track"), "]", null);
	}
							/* debug macro name */
	concat(&outline[0], bp->b_bname, ":", 
						     /* and lastly the line */
			    eline, ">>>", null);
						/* write out the debug line */
dinput: 
  outline[term.t_ncol - 1] = 0;
  if (pd_discmd > 0)
    mlputs(term.t_ncol, outline);

	update(TRUE);
						   /* and get the keystroke */
	c = getkey();
					    /* META key turns off debugging */
	key = getbind(c);
	if	(key->k_ptr.fp == meta)
	  pd_macbug = FALSE;
	else 
	{ int oldcmd = pd_discmd;		        
	  int oldinp = g_disinp;
	  int oldstatus = pd_cmdstatus;
	  
		if (c == g_abortc) 
		  return FALSE;

	  switch (c)
	  { case '?': strcpy(outline, TEXT128);     /* list commands */
		        
/*"(e)val exp, (c/x)ommand, (t)rack exp, (^G)abort, <SP>exec, <META> stop debug"*/
      		      goto dinput;

	    case 'c': pd_discmd = TRUE;			/* execute statement */
      		      g_disinp = TRUE;
      		      execcmd(FALSE, 1);
      		      goto dbuild;

	    case 'x': pd_discmd = TRUE;		/* execute extended command */
      		      g_disinp = TRUE;
      		      namedcmd(FALSE, 1);
      		      pd_cmdstatus = oldstatus;
      		      goto dbuild;

	    case 'e': strcpy(temp, "set %track ");   /* evaluate expresion */
      		      pd_discmd = TRUE;
      		      g_disinp = TRUE;
      					getstring(&temp[11], NSTRING, "Exp: ");
      		      pd_discmd = oldcmd;
      		      g_disinp = oldinp;
      		      docmd(temp);
      		      pd_cmdstatus = oldstatus;
      		      concat(&temp[0], "%! = [", gtusr("track"), "]", null);
      		      mlwrite(temp);
      		      c = getkey();
      		      goto dinput;

	    case 't': pd_discmd = TRUE;			/* track expresion */
      		      g_disinp = TRUE;
      		      getstring(&temp[0], NSTRING, "Exp: ");
      		      pd_discmd = oldcmd;
      		      g_disinp = oldinp;
      		      concat(&track[0], "set %track ", temp, null);
      		      goto dbuild;

	    case ' ': break;			      /* execute a statement */
		        
	    default: tcapbeep();				/* illegal command */
      		     goto dbuild;
	  }
	}
	return TRUE;
}
#endif

					                        /* execute the startup file */
Cc Pascal startup(const char * sfname)
			/*  sfname   ** name of startup file ("" if default) */
{ Cc cc = -32000;
	const char *fspec = flook(Q_LOOKH, sfname);			/* look up the path for the file */
	
	if (fspec == NULL)
	{				  																/* complain if we are interactive */
	  mlwrite(TEXT214, sfname);
						/* "%%No such file as %s" */
	}
	else
	{
#if _DEBUG
		mbwrite(fspec);
#endif
																						/* otherwise, execute it */
	{	BUFFER * dfb = bfind("\377", 3);
  	if (dfb != NULL) 			   		/* get the needed buffer */
		{ BUFFER *scb = curbp;	   		
  	  dfb->b_flag = MDVIEW;
			curbp = dfb;			      		/* make this one current */
//	  curbp->b_flag = MDVIEW;			/* mark the buffer as read only */
						                  		/* and try to read in the file to execute */
		  cc = readin(fspec, 0);
									/* go execute it! */
		  if (cc > FALSE)
		  	cc = dobuf(dfb,1, "Startup");

		  if (cc > FALSE &&
			    window_ct(dfb) == NULL)
			{         		  	/* not displayed, remove the unneeded buffer and exit */
		  	zotbuf(dfb);

				curbp = scb;								/* restore the current buffer */
			}
		}
	}}

  return cc;
}



int Pascal execfile(int f, int n)	/* execute a series of commands in a file */
	/* int f, n;	** default flag and numeric arg to pass on to file */
{ 

#if 1
  char ebuffer[65];
	Cc cc = mlreply(TEXT129, ebuffer, sizeof(ebuffer)-1);
/*			      "File to execute: " */

					   /* look up the path for the file */
	if (cc > FALSE)
		cc = startup(ebuffer);
	return cc;
#endif
}

