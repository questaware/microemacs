/*	This file is for functions dealing with execution of
	commands, command lines, buffers, files and startup files

	written 1986 by Daniel Lawrence 			*/

#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"

/*	The !WHILE directive in the execution language needs to
	stack references to pending whiles. These are stored linked
	to each currently open procedure via a linked list of
	the following structure
*/

typedef struct WHBLOCK
{ struct WHBLOCK *w_next;	/* next while */
	LINE *w_begin;					/* ptr to !while statement */
	LINE *w_end;						/* ptr to the !endwhile statement*/
	char  w_break;						/* block type */
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
BUFFER * Pascal bmfind(int create, int n)

{ char ebuffer[NBUFN+1];
	strcpy(ebuffer, "[Macro xx");
  																			/* make the buffer name */
	if (n > 0)					
	{ ebuffer[7] = '0' + (n / 10);
	  ebuffer[8] = '0' + (n % 10);
	}
	else /* n is 0 or -1 */
	{																				/* find buffer user wants to execute */
		int cc = mlreply(n ? TEXT115 : TEXT117, &ebuffer[-n], NBUFN);
											/* "Execute procedure: " */
		if (cc <= FALSE)
		  return NULL;
	}
					/* find the pointer to that buffer */
	return bfind(strcat(ebuffer, "]"), create, n <= 0 ? 0 : BFINVS);
}

/* namedcmd:	execute a named command even if it is not bound */

int Pascal namedcmd(int f, int n)
	  /* command arguments [passed through to command executed] */
{
	int (Pascal *kfunc)(int, int);/* ptr to the function to execute */
	int scle = g_clexec;
	int cc;

	if (! scle)
															/* prompt the user to type a named command */
	  kfunc = getname(" ");			/* and get the function name to execute */
	else
	{	char ebuffer[40];
																					/* grab token and advance past */
	  (void)token(ebuffer, sizeof(ebuffer));
							     /* evaluate it */
	{ char * fnm = getval(ebuffer, ebuffer);
	  if (fnm == g_logm[2])
	    return FALSE;
																								/* and look it up */
	  kfunc = fncmatch(fnm);
	}}

	if (kfunc != NULL)
	{ g_clexec = FALSE;
																												/* call the function */
		cc = in_range((int)kfunc, 1, 40) ? execporb(-(int)kfunc, n)
																		 : (*kfunc)(f, n);	  
	  g_clexec = scle;
	}
	else
	{ mlwrite(TEXT16);
					/* "[No such function]" */
	  cc = FALSE;
	}
	return cc;
}

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
	if (g_execlevel)
		return TRUE;

{	int f = g_got_uarg;
  g_got_uarg = FALSE;							// The 1st command in the buffer gets the arg

{	int n = ! f ? 1 : g_univct;  					/* numeric repeat value */
//int (Pascal *fnc)(int, int);      	  /* function to execute */
	Command fnc;      	  								/* function to execute */
	char ebuffer[132];
#define tkn ebuffer+1
	int cc;											/* TRUE, FALSE, ABORT */
	char *s_execstr = g_execstr; 		/* original exec string */

	g_execstr = cline;	      /* and set this one as current */
	++g_clexec;	      /* in cline execution */

	g_lastflag = g_thisflag;
	g_thisflag = 0;

	cc = macarg(tkn);
	if (cc > FALSE)
		if (*(tkn) < 'A')
		{ f = TRUE;
		  n = atoi(getval(tkn, tkn));
														      /* and now get the command to execute */
		  cc = macarg(tkn);
		}  

	if (cc > FALSE)
	{	fnc = fncmatch(tkn);					/* and match the token to see if it exists */
		if (fnc != NULL)
		{ 
			cc = in_range((int)fnc, 1, 40) ? execporb(-(int)fnc, n)
																		 : (*fnc)(f, n);			/* call the function */
		}
		else													/* find the pointer to that buffer */
		{	ebuffer[0] = '[';														
		{ BUFFER *bp = bfind(strcat(ebuffer, "]"), FALSE, 0); 
		  if (bp == NULL) 
			{ mlwrite(TEXT16);
							/* "[No such Function]" */
				cc = FALSE;
			}
			else 												/* execute the buffer */
				cc = dobuf(bp,n);
		}}
		pd_cmdstatus = cc;								/* save the status */
		lastfnc = fnc;
	}
	--g_clexec;			/* restore g_clexec flag */
	g_execstr = s_execstr;
	return cc;
#undef tkn
}}}


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

	g_execlevel = 0;
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
	const char * src = g_execstr;
	char quotef = 0;	/* is the current string quoted? */
	char c;
	int leading = TRUE;

		      /* first scan past any whitespace in the source string */
	for (; (c = *src) != 0; ++src)
	{			/* process special characters */
	  if (c == '~')
	  { c = *++src;
	    if (c == 0)
	      break;
	    switch (c)
	    { case 'r': c = 'J' - '@';
	      when 'n': c = 'M' - '@'; 
	      when 'l': c = 'L' - '@'; 
	      when 't': c = 'I' - '@';  
	      when 'b': c = 'H' - '@';  
	    }
	  }
	  else			/* check for the end of the token */
	  { if      (c == quotef)
	      break;
	    else if (c == ' ' || c == '\t')
	    { if (leading)
	        continue;
	      if (quotef == 0)
	        break;
	    }
	    else if ((c == '"' || c == '\'') && quotef == 0 && leading)
	    { quotef = c;	/* set quote mode if quote found */
	      leading = 0;
	      continue;
	    }
	  }
	  leading = 0;
	  if (--size > 0)
	    *tok++ = c;
	}

	*tok = 0;
	return g_execstr = (char*)(*src == 0 ? src : src+1);
}


int Pascal macarg(char * tok) /* get a macro line argument */
				/* buffer to place argument */
{ ++g_clexec;
{	int res = nextarg(null, tok, NSTRING);
  --g_clexec;
	return res;      
}}

/*	nextarg:	get the next argument	*/

int Pascal nextarg(const char * prompt, char * buffer, int size)
				/* prompt to use if we must be interactive */
				/* buffer to put token into */
				/* size of the buffer */
{					/* if we are interactive, go get it! */
/*if (g_clexec && g_execstr == null)
	  adb(55);
*/
	if (g_clexec <= 0 || g_execstr == null)
	  return getstring(buffer, size, prompt);
																			     /* grab token and advance past */
	(void)token(buffer, size);

	return getval(buffer, buffer) != getvalnull;/* evaluate it *//* no protection! */
}



static int USE_FAST_CALL common_return(BUFFER * bp)
	
{	if (bp == NULL)
	{ mlwrite(TEXT113);				/*	"Can not create macro" */
	  return FALSE;
	}
														/* and make sure it is empty */
	g_bstore = bp;
	bclear(bp);
	return TRUE;
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

#if	NMDPROC
/*	storeproc:	Set up a procedure buffer and flag to store all
			executed command lines there			*/

int Pascal storeproc(int f, int n)
	/* int n;		** macro number to use */
{
	char bname[NBUFN];		/* name of buffer to use */
			/* a numeric argument means its a numbered macro */
	if (f != FALSE)
	  return storemac(f, n);

	bname[0] = '[';
																							/* get the name of the procedure */
{	int cc = mlreply(TEXT114, &bname[1], NBUFN-2);
								/* "Procedure name: " */
	return cc <= FALSE ? cc
																							/* set up the new macro buffer */
				 						 : common_return(bfind(strcat(bname, "]"), TRUE, BFINVS));
}}

#endif


int Pascal execporb(int isp, int n)
										 /* isp; ** must be -ve, 0, 1 */
{	BUFFER * bp = bmfind(FALSE, -isp);
	if (bp == NULL) 
	{ mlwrite(isp < 0 ? TEXT130 : TEXT116);
										/* "Macro not defined" */
										/* "No such procedure" */
		return FALSE;
	}
					/* and now execute it as asked */
	return dobuf(bp,n);
}

/*	execbuf:	Execute the contents of a buffer of commands	*/

int Pascal execbuf(int f, int n)
	/* int f, n;	** default flag and numeric arg */
{ 
  return execporb(0,n);
}


/*	execproc:	Execute a procedure				*/

int Pascal execproc(int f, int n)
	/* int f, n;	** default flag and numeric arg */
{ return execporb(1,n);
}


static
char * Pascal skipleadsp(char * s, int len)
	
{
  while (--len >= 0 && (*s == ' ' || *s == '\t'))
    ++s;
  return s;
}


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

BUFFER * g_dobuf;

int Pascal dobuf(BUFFER * bp, int iter)
			 /* iter:   # times to do it */
{
	char smalleline[80];
#if	LOGFLG
	FILE *fp;		/* file handle for log file */
#endif
	int cc = TRUE;
#if LIM_WHILE
	WHBLOCK whiles[LIM_WHILE];
	int topwh = LIM_WHILE-1;
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
		g_execlevel = 0;		/* clear IF level flags/while ptr */

		for (lp = &bp->b_baseline; ((lp=lforw(lp))->l_props & L_IS_HD) == 0; ) 
		{ 					   													/* scan the current line */
																						/* trim leading whitespace */
		  eline = skipleadsp(lp->l_text, lp->l_used);
		  
		  if (lp->l_used + (lp->l_text - eline) < 4)
		  	continue;
																/* if is a while directive, make a block... */
			if (eline[0] != '!')
				continue;
	  
			if (eline[1] == 'w' && eline[2] == 'h' ||
					eline[1] == 'b' && eline[2] == 'r')
			{
#if LIM_WHILE
				WHBLOCK * whtemp = topwh <= 0 ? NULL : &whiles[topwh--];
#else			
				WHBLOCK * whtemp = (WHBLOCK *)mallocz(sizeof(WHBLOCK));
#endif
		 		if (whtemp == NULL ||
	     															/* "%%!BREAK outside of any !WHILE loop" */
	         (eline[1] == 'b' && scan == NULL))
					goto failexit;

		    whtemp->w_break = eline[1];
		    whtemp->w_begin = lp;
	  	  whtemp->w_next = scan;
//  	  whtemp->w_end = 0;
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
	    	} while (whlist->w_break == 'b');
	  	}
		} /* for */

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

	    if (eexitflag || cc <= FALSE ||
				  ((lp = lforw(lp))->l_props & L_IS_HD) != 0)
				break;
													/* allocate eline and copy macro line to it */
		{	int dirnum;					/* directive index */
			int linlen = lp->l_used+1;
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
		  if (macbug && g_bstore == null && g_execlevel == 0)
		    if (debug(bp, eline) == FALSE)
		    { mlwrite("%!"TEXT54); /*	"[Macro aborted]" */
		      cc = FALSE;
		      continue;
		    }
#endif
																				/* Parse directives here.... */
		  dirnum = -1;
		  if (*eline == '!')								/* Find out which directive this is */
		  {
		    for (dirnum = NUMDIRS; --dirnum >= 0; )
		      if (strcmp_right(eline+1, dname[dirnum]) == 0)
						break;
																				/* and bitch if it's illegal */
		    if (dirnum < 0)
		    { msg += 1;		    					/* "%%Unknown Directive" */
		      continue;
		    }
		  }
																		/* if macro store is on, salt this away */
		  if (g_bstore != null && dirnum != DENDM)
		  { LINE * mp = mk_line(ebuf,linlen-1,linlen-1);
		    if (mp == NULL)
		    { cc = FALSE;
		      continue;
		    }
																		/* attach the line to the end of the buffer */
        ibefore(&g_bstore->b_baseline, mp);
		    continue;
		  }
																		/* now, execute directives */
		  if (dirnum >= 0)
		  {	char *s_execstr = g_execstr; 		/* original exec string */

		    while (*eline > ' ')						/* skip past the directive */
		      ++eline;

				if (dirnum == DRETURN)
				{	if (g_execlevel == 0)
						break;													// is smalleline
					continue;
				}
				else
		    switch (dirnum)
		    { case DIF: 				/* IF directive */
						++nest_level;
		      case DWHILE:			/* WHILE directive */
																				/* grab the value of the logical exp */
						if (g_execlevel == 0)
						{ g_execstr = eline;
							cc = macarg(tkn);
							g_execstr = s_execstr;
							if (cc <= FALSE || stol(tkn))
								continue;
																							/* expression evaluated false */
						  if (dirnum == DIF)
						  	g_execlevel |= (1 << nest_level);
						}
																			/* drop down and act just like !BREAK */
		      case DBREAK:			/* BREAK directive */
						if (dirnum == DBREAK && g_execlevel)
						  continue;
																			/* jump down to the endwhile */
																			/* find the right while loop */
						if (dirnum == DWHILE)
						{ WHBLOCK* whtemp;
							for (whtemp = whlist; whtemp; whtemp = whtemp->w_next) 
							  if (whtemp->w_begin == lp)
							    break;
        
							if (whtemp == NULL) 
							{ msg -= 1;				/* "%%Internal While loop error" */
							  continue;
							}
																					    /* reset the line pointer back.. */
							lp = whtemp->w_end;
				    }

						continue;

		      when DELSE:				/* ELSE directive */
						g_execlevel ^= (1 << nest_level);
						continue;

		      when DENDIF:			/* ENDIF directive */
						g_execlevel &= ~ (1 << nest_level);
						--nest_level;
						continue;

		      when DGOTO:	/* GOTO directive */
																	/* .....only if we are currently executing */
						if (g_execlevel == 0) 
						{ char golabel[20];
							g_execstr = eline;
							(void)token(golabel, sizeof(golabel));
						//eline = g_execstr
							g_execstr = s_execstr;
						  
						  for (lp = &bp->b_baseline; 
						      ((lp=lforw(lp))->l_props & L_IS_HD) == 0; )
						    if (*lp->l_text == '*' &&
										strcmp_right(&lp->l_text[1],golabel) == 0)
									break;

							if (lp->l_props & L_IS_HD)
						  {	mlwrite(TEXT127, golabel);				/* "%%No such label" */
						  	cc = FALSE;
							}
						}
						continue;
	        
		      when DENDWHILE:	/* ENDWHILE directive */
						if (g_execlevel == 0)
																					/* find the right while loop */
						{ WHBLOCK * whtemp; 
						  for (whtemp = whlist; whtemp; whtemp = whtemp->w_next)
						    if (whtemp->w_break == 'w' &&
										whtemp->w_end == lp)
						      break;
		        
						  if (whtemp == NULL)
						  { msg -= 1;						/* "%%Internal While loop error" */
						    continue;
						  }
																				  /* reset the line pointer back.. */
						  lp = lback(whtemp->w_begin);
						}
						continue;
					when DENDM:
		    		if (g_bstore != NULL)
		    		{ g_bstore->b_dotp = lforw(&g_bstore->b_baseline);
				    	g_bstore = NULL;
				    }
		  	    continue;
		    } // switch
		  }

			if (g_execlevel == 0)							/* execute the statement */
			{	
				cc = docmd(eline);
			  if (dirnum == DFORCE)	/* FORCE directive */
			    cc = TRUE;
																							/* check for a command error */
			  if (cc <= FALSE)
			  {	rpl_all(-1, 0, (LINE*)bp, lp, 0);		/* point window(s) at line */
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

	g_dobuf = NULL;
  return cc;
#undef tkn
}


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
	  macbug = FALSE;
	else if (c == abortc) 
	  return FALSE;
	else 
	{ int oldcmd = pd_discmd;		        
	  int oldinp = g_disinp;
	  int oldstatus = pd_cmdstatus;
	  
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
		        
	    default: TTbeep();				/* illegal command */
      		     goto dbuild;
	  }
	}
	return TRUE;
}
#endif




BUFFER * g_dofilebuff;

/*	dofile: yank a file into a buffer and execute it
		if there are no errors, delete the buffer on exit */

static int Pascal dofile(const char * fname)
				/* file name to execute */
{ char bname[NBUFN];		/* name of buffer */

  makename(bname, fname); 		/* derive the name of the buffer */

{ BUFFER *dfb = bfind(bname, 3, 0);
  if (dfb == NULL) 			   		/* get the needed buffer */
    return FALSE;

{ BUFFER *scb = curbp;	   		
  curbp = dfb;			      		/* make this one current */
  curbp->b_flag = MDVIEW;			/* mark the buffer as read only */
				                  		/* and try to read in the file to execute */
{ Cc cc = readin(fname, 0);
  curbp = scb;								/* restore the current buffer */
  if (cc <= FALSE)
    return cc;
							/* go execute it! */
  cc = dobuf(dfb,1);
  if (cc > FALSE &&
      window_ct(dfb) == 0)
            		    /* not displayed, remove the now unneeded buffer and exit */
  { zotbuf(dfb);
    dfb = NULL;
  }
  g_dofilebuff = dfb;
  return cc;
}}}}

					                        /* execute the startup file */
int Pascal startup(const char * sfname)
	/*  sfname   ** name of startup file ("" if default) */
{
																					   /* look up the path for the file */
	const char *fspec = flook(0, sfname);
																			      /* if it isn't around */
	if (fspec == NULL)
	{				  																/* complain if we are interactive */
	  if (g_clexec <= 0)
	    mlwrite(TEXT214, sfname);
						/* "%%No such file as %s" */
	   return 13;
	}
																						/* otherwise, execute it */
	return dofile(fspec);
}



int Pascal execfile(int f, int n)	/* execute a series of commands in a file */
	/* int f, n;	** default flag and numeric arg to pass on to file */
{ 
        char ebuffer[65];
	register int cc = mlreply(TEXT129, ebuffer, sizeof(ebuffer)-1);
/*			      "File to execute: " */

					   /* look up the path for the file */
	return !cc ? cc : startup(ebuffer);
}

