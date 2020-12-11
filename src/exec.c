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

typedef struct WHBLOCK {
	LINE *w_begin;		/* ptr to !while statement */
	LINE *w_end;		/* ptr to the !endwhile statement*/
	char  w_type;		/* block type */
	struct WHBLOCK *w_next;	/* next while */
} WHBLOCK;


/* directive name table:
	This holds the names of all the directives....	*/

static const char dname[][8] = {
	"if", "else", "endif",
	"goto", "return", "endm",
	"while", "endw", "break",
	"force"
};

#define NUMDIRS (sizeof(dname) / 8)


char smalleline[80];


static struct BUFFER *bstore = NULL;	/* buffer to store macro text to*/

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

BUFFER * Pascal bmfind(int create, int n)

{ static char bufname[] = "[Macro xx]";
						/* make the buffer name */
  bufname[7] = '0' + (n / 10);
  bufname[8] = '0' + (n % 10);

  return bfind(bufname, create, BFINVS);
}

/* namedcmd:	execute a named command even if it is not bound */

int Pascal namedcmd(int f, int n)
	  /* command arguments [passed through to command executed] */
{
	int (Pascal *kfunc)(int, int);/* ptr to the function to execute */
	int scle = g_clexec;
	int cc;
        char ebuffer[40];

	if (! scle)
			/* prompt the user to type a named command */
			/* and get the function name to execute */
	  kfunc = getname(": ");
	else
	{				/* grab token and advance past */
	  execstr = token(execstr, ebuffer, sizeof(ebuffer));
							     /* evaluate it */
	{ char * fnm = getval(ebuffer, ebuffer);
	  if (fnm == errorm)
	    return FALSE;
							/* and look it up */
	  kfunc = fncmatch(fnm);
	}}

	if (kfunc != NULL)
	{ g_clexec = FALSE;
	  cc = (*kfunc)(f, n);	/* call the function */
	  g_clexec = scle;
	}
	else
	{ mlwrite(TEXT16);
/*			"[No such function]" */
	  cc = FALSE;
	}
	return cc;
}

#if FLUFF || DEBUGM
/*	execcmd:	Execute a command line command to be typed in
			by the user					*/
int Pascal execcmd(int f, int n)

{
	char cmdstr[NSTRING];		/* string holding command to execute */
						      /* get the line wanted */
	int cc = mlreply(": ", cmdstr, NSTRING);
	if (cc != TRUE)
	  return cc;

	g_execlevel = 0;
	return docmd(cmdstr);
}
#endif

/*	docmd:	take a passed string as a command line and translate
		it to be executed as a command. This function will be
		used by execute-command-line and by all source and
		startup files. Lastflag/thisflag is also updated.

	format of the command line is:

		{# arg} <command-name> {<argument string(s)>}

*/

int Pascal docmd(char * cline)
			/* command line to execute */
{
	register int f = g_got_uarg;  /* default argument flag */
	register int n = ! f ? 1 : prenum;   /* numeric repeat value */
	int (Pascal *fnc)(int, int);      	/* function to execute */
	int cc;
	int oldcle = g_clexec;     /* old contents of g_clexec flag */
	char *oldestr = execstr; /* original exec string */
	char ebuffer[40];
#define tkn (&ebuffer[1])
		     /* if we are scanning and not executing..go back here */
  g_got_uarg = FALSE;

	if (g_execlevel)
		return TRUE;

	ebuffer[0] = '[';
	g_clexec = TRUE;	      /* in cline execution */
	execstr = cline;	      /* and set this one as current */

	lastflag = thisflag;
	thisflag = 0;

	cc = macarg(tkn);
	if (cc == TRUE)
		if (gettyp(tkn) != TKCMD)     /* process leadin argument */
		{ f = TRUE;
		  n = atoi(getval(tkn, tkn));
												      /* and now get the command to execute */
		  cc = macarg(tkn);
		}

	if (cc == TRUE)
	{				/* and match the token to see if it exists */
		fnc = fncmatch(tkn);
		if (fnc != NULL)	/* construct the buffer name */
			cc = (*fnc)(f, n);		/* call the function */
		else
																/* find the pointer to that buffer */
		{ BUFFER *bp = bfind(strcat(&ebuffer[0], "]"), FALSE, 0); 
		  if (bp == NULL) 
			{ mlwrite(TEXT16);
/*			      "[No such Function]" */
				cc = FALSE;
			}
			else 				/* execute the buffer */
			{ univct = n;
				cc = dobuf(bp,n);
			}
		}
		cmdstatus = cc;			/* save the status */
		lastfnc = fnc;
	}
	g_clexec = oldcle;			/* restore g_clexec flag */
	execstr = oldestr;
	return cc;
#undef tkn
}

/* token:	chop a token off a string
		return a pointer past the token
*/

char *Pascal token(char * src_, char * tok, int size)
				/* source string, destination token string */
				/* maximum size of token */
{
	register char * src = src_;
		 char quotef = 0;	/* is the current string quoted? */
	register char c;
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
	    else if (quotef == 0 && leading && (c == '"' || c == '\''))
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
	return *src == 0 ? src : src+1;
}


int Pascal macarg(char * tok) /* get a macro line argument */
				/* buffer to place argument */
{       int sclexec = g_clexec;
        g_clexec = TRUE;
   
{	int res = nextarg(null, tok, NSTRING);
        g_clexec = sclexec;
	return res;      
}}

/*	nextarg:	get the next argument	*/

int Pascal nextarg(const char * prompt, char * buffer, int size)
				/* prompt to use if we must be interactive */
				/* buffer to put token into */
				/* size of the buffer */
{					/* if we are interactive, go get it! */
/*	if (g_clexec && execstr == null)
	  adb(55);
*/
	if (g_clexec == FALSE || execstr == null)
	  return getstring(buffer, size, prompt);
					     /* grab token and advance past */
	execstr = token(execstr, buffer, size);

	return getval(buffer, buffer) != getvalnull;/* evaluate it *//* no protection! */
}



int common_return(BUFFER * bp)
	
{	if (bp == NULL)
	{ mlwrite(TEXT113);				/*	"Can not create macro" */
	  return FALSE;
	}
														/* and make sure it is empty */
	bclear(bp);
	bstore = bp;
	return TRUE;
}

/*	storemac:	Set up a macro buffer and flag to store all
			executed command lines there			*/

int Pascal storemac(int f, int n)
	/* int n;		** macro number to use */
{
	if (f == FALSE)
	{ mlwrite(TEXT111);/* must have a numeric argument to this function */
/*			"No macro specified" */
	  return FALSE;
	}
					/* range check the macro number */
	if (! in_range(n, 1,40))
	{ mlwrite(TEXT112);
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
	if (f == TRUE)
	  return storemac(f, n);
					/* get the name of the procedure */
{	int cc = mlreply(TEXT114, &bname[1], NBUFN-2);
	if (cc != TRUE)
/*			      "Procedure name: " */
	  return cc;

	bname[0] = '['; 	/* construct the macro buffer name */
					/* set up the new macro buffer */
	return common_return(bfind(strcat(&bname[0], "]"), TRUE, BFINVS));
}}

#endif


int Pascal execporb(int isp, int n)
	/* isp;		** must be bool */
{
	register BUFFER *bp;		/* ptr to buffer to execute */
        char ebuffer[NBUFN+1];

			/* find out what buffer the user wants to execute */
	int cc = mlreply(isp ? TEXT115 : TEXT117, &ebuffer[isp], NBUFN);
	if (cc != TRUE)
/*			      "Execute procedure: " */
	  return cc;

	if (isp)
	{ ebuffer[0] = '[';			/* construct the buffer name */
	  strcat(ebuffer, "]");
	}
					/* find the pointer to that buffer */
	bp = bfind(ebuffer, FALSE, 0);
	if (bp == NULL) 
	{ mlwrite(TEXT116);
/*			"No such procedure" */
	  return FALSE;
	}
					/* and now execute it as asked */
	cc = dobuf(bp,n);
	return cc;
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


char * Pascal skipleadsp(char * s, int len)
	
{
  while (--len >= 0 && (*s == ' ' || *s == '\t'))
    ++s;
  return s;
}



void Pascal freewhile(WHBLOCK * wp)/* free a list of while block pointers */
				/* head of structure to free */
{
	if (wp != NULL)
	{ freewhile(wp->w_next);
	  free((char*)wp);
	}
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

int Pascal dobuf(BUFFER * bp, int iter)
			 /* iter:   # times to do it */
{	int cc = TRUE;

  while (--iter >= 0 && cc == TRUE)
	{ register LINE *lp;	/* pointer to line to execute */
		int dirnum;					/* directive index */
		int linlen;					/* length of line to execute */
		char *einit;				/* initial value of eline */
		register char *eline;/* text of line to execute */
		char tkn[NSTRING];	/* buffer to evaluate an expresion in */
#if	LOGFLG
		FILE *fp;		/* file handle for log file */
#endif
  	WHBLOCK *whlist = NULL; /* ptr to !WHILE list */
		WHBLOCK *scanner = NULL;/* ptr during scan */
												    /* scan the buffer, building WHILE header blocks */

		int nest_level = 0;
		g_execlevel = 0;		/* clear IF level flags/while ptr */

		for (lp = bp->b_baseline; ((lp=lforw(lp))->l_props & L_IS_HD) == 0; ) 
		{ 					   													/* scan the current line */
																						/* trim leading whitespace */
		  eline = skipleadsp(lp->l_text, lp->l_used);
																/* if is a while directive, make a block... */
			if (eline[0] != '!')
				continue;
	  
			if (eline[1] == 'w' && eline[2] == 'h' ||
					eline[1] == 'b' && eline[2] == 'r')
			{ WHBLOCK * whtemp = (WHBLOCK *)aalloc(sizeof(WHBLOCK));
		 		if (whtemp == NULL ||
	     															/* "%%!BREAK outside of any !WHILE loop" */
	         (eline[1] == 'b' && scanner == NULL))
					goto failexit;

		    whtemp->w_begin = lp;
		    whtemp->w_type = eline[1];
	  	  whtemp->w_next = scanner;
	    	scanner = whtemp;
	  	}
												/* if it is an endwhile directive, record the spot.. */
		  if (strncmp(&eline[1], &dname[DENDWHILE][0], 4) == 0)
		  { if (scanner == NULL)
	 	    { 			/* %%mismatched !ENDWHILE in %s */
	 	    	goto failexit;
	 			}
															/* move top records from the scanner list to the
															   whlist until we have moved all BREAK records */
			  do										/* and one WHILE record */
				{ WHBLOCK * whtemp = whlist;
					scanner->w_end = lp;
		 			whlist = scanner;
		 			scanner = scanner->w_next;
		 			whlist->w_next = whtemp;
	    	} while (whlist->w_type == 'b');
	  	}
		} /* for */

		if (scanner != NULL)
		{ eline = "WHILE";		/* %%mismatched !WHILE in '%s' */
failexit:     
	    mlwrite(TEXT121, bp->b_bname, eline);
	    freewhile(scanner);
		  cc = FALSE;
		  goto eexec;
		}
						/* let the first command inherit the flags from the last one..*/
		thisflag = lastflag;
																	/* starting at the beginning of the buffer */
		for (lp = bp->b_baseline; 
	     ((lp = lforw(lp))->l_props & L_IS_HD) == 0 && eexitflag == FALSE 
	     					        													&& cc == TRUE;)
		{															/* allocate eline and copy macro line to it */
		  linlen = lp->l_used+1;
		  if (linlen < sizeof(smalleline))
		    einit = smalleline;
	    else 
		  { einit = (char *)malloc(linlen);
		    if (einit == NULL)
		    { cc = FALSE;
		      break;
		    }
		  }
		  strpcpy(einit, lp->l_text, linlen);
																						/* trim leading whitespace */
		  eline = skipleadsp(einit, linlen-1);
																				   /* dump comments and blank lines */
		  if (*eline == ';' || *eline == 0 ||
		  		*eline == '*')
		    goto onward;

#if LOGFLG
		  logstr(eline); logstr("\n");
#endif
																				/* only do this if we are debugging */
#if	DEBUGM
		  if (macbug && bstore == null && g_execlevel == 0)
		    if (debug(bp, eline) == FALSE)
		    { mlforce(TEXT54);						/*	"[Macro aborted]" */
		      cc = FALSE;
		      break;
		    }
#endif
																				/* Parse directives here.... */
		  dirnum = -1;
		  if (*eline == '!')								/* Find out which directive this is */
		  {
		    for (dirnum = NUMDIRS; --dirnum >= 0; )
		      if (strncmp(eline+1, dname[dirnum], strlen(dname[dirnum])) == 0)
						break;
																				/* and bitch if it's illegal */
		    if (dirnum < 0)
		    { mlwrite(TEXT124);			/*	"%%Unknown Directive" */
		      cc = FALSE;
		      break;
		    }
						/* service only the !ENDM macro here */
		    if (dirnum == DENDM)
		    { bstore = NULL;
		      goto onward;
		    }
		  }
																		/* if macro store is on, salt this away */
		  if (bstore != null)
		  { int sz = strlen(eline);
				LINE * mp = mk_line(eline,sz,sz);
		    if (mp == NULL)
		    { mlwrite(TEXT125);					/* "Out of memory while storing macro" */
		      return FALSE;
		    }
																		/* attach the line to the end of the buffer */
        ibefore(bstore->b_baseline, mp);
		    goto onward;
		  }
																		/* now, execute directives */
		  if (dirnum != -1)
		  {															/* skip past the directive */
		    while (*eline > ' ')
		      ++eline;

		    switch (dirnum)
		    { case DIF: 				/* IF directive */
						++nest_level;
		      case DWHILE:			/* WHILE directive */
																				/* grab the value of the logical exp */
						if (g_execlevel == 0)
						{ char *saveexecstr = execstr; /* original exec string */
							execstr = eline;
							linlen = macarg(tkn);
							execstr = saveexecstr;
							if (linlen != TRUE)
								goto eexec;
							if (stol(tkn) == TRUE)
								goto onward;
																							/* expression evaluated false */
						  if (dirnum == DIF)
						  	g_execlevel |= (1 << nest_level);
							else
						  	goto onward;
						}
																			/* drop down and act just like !BREAK */
		      case DBREAK:			/* BREAK directive */
						if (dirnum == DBREAK && g_execlevel)
						  goto onward;
																			/* jump down to the endwhile */
																			/* find the right while loop */
						if (dirnum == DWHILE)
						{ WHBLOCK* whtemp;
							for (whtemp = whlist; whtemp; whtemp = whtemp->w_next) 
							  if (whtemp->w_begin == lp)
							    break;
        
							if (whtemp == NULL) 
							{ mlwrite(TEXT126);						/* "%%Internal While loop error" */
							  cc = FALSE;
							  goto eexec;
							}
																					    /* reset the line pointer back.. */
							lp = whtemp->w_end;
				    }

						goto onward;

		      when DELSE:				/* ELSE directive */
						g_execlevel ^= (1 << nest_level);
						goto onward;

		      when DENDIF:			/* ENDIF directive */
						g_execlevel &= ~ (1 << nest_level);
						--nest_level;
						goto onward;

		      when DGOTO:	/* GOTO directive */
																	/* .....only if we are currently executing */
						if (g_execlevel == 0) 
						{ LINE * glp;
							static char golabel[20] = "";
						  eline = token(eline, golabel, sizeof(golabel));
						  
						  for (glp = bp->b_baseline; 
						      ((glp=lforw(glp))->l_props & L_IS_HD) == 0; )
						    if (*glp->l_text == '*' &&
										strncmp(&glp->l_text[1], 
							         golabel, strlen(golabel))==0)
						    { lp = glp;
						      goto onward;
						    }
						  mlwrite(TEXT127, golabel);				/* "%%No such label" */
						  cc = FALSE;
						  goto eexec;
						}
						goto onward;
	        
		      when DRETURN:	/* RETURN directive */
						if (g_execlevel == 0)
						  goto eexec;
						goto onward;

		      when DENDWHILE:	/* ENDWHILE directive */
						if (g_execlevel == 0)
																					/* find the right while loop */
						{ WHBLOCK * whtemp; 
						  for (whtemp = whlist; whtemp; whtemp = whtemp->w_next)
						    if (whtemp->w_type == 'w' &&
										whtemp->w_end == lp)
						      break;
		        
						  if (whtemp == NULL)
						  { mlwrite(TEXT126);						/* "%%Internal While loop error" */
						    cc = FALSE;
						    goto eexec;
						  }
																				   /* reset the line pointer back.. */
						  lp = lback(whtemp->w_begin);
						}
						goto onward;
		    }
		  }

			if (g_execlevel == 0)							/* execute the statement */
			{	
				cc = docmd(eline);
			  if (curwp->w_linep == null && ! eexitflag)
			    adb(91);
			  if (dirnum == DFORCE)	/* FORCE directive */
			    cc = TRUE;
																							/* check for a command error */
			  if (cc != TRUE)
			  {					/* look if buffer is showing */
		      rpl_all((LINE*)bp, lp, -2, 0, 0);
#if 0
			    for (wp = wheadp; wp != NULL; wp = wp->w_wndp)
			      if (wp->w_bufp == bp)	     /* and point it */
			      { wp->w_dotp = lp;
							wp->w_doto = 0;
			        wp->w_flag |= WFHARD;
			      }
#endif
			    bp->b_dotp = lp;									/* in any case set the buffer. */
			    bp->b_doto = 0;
			    if (gflags & MD_KEEP_MACROS)
			      bp->b_flag &= ~BFINVS;
			  }
			}
onward: 					    /* on to the next line */
			if (einit != smalleline)
		    free(einit);
		}
eexec:						/* exit the current function */
		freewhile(whlist);
	}
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
	register int c; 		/* temp character */
	register KEYTAB *key;		/* ptr to a key entry */
	static char track[NSTRING] = "";/* expression to track value of */
	       char temp[NSTRING];
	
dbuild: /* Build the information line to be presented to the user */

	strcpy(outline, "<<<");
					/* display the tracked expression */
	if (track[0] != 0)
	{ int oldstatus = cmdstatus;
	  docmd(track);
	  cmdstatus = oldstatus;
	  concat(&outline[0], "[=", gtusr("track"), "]", null);
	}
							/* debug macro name */
	concat(&outline[0], bp->b_bname, ":", 
						     /* and lastly the line */
			    eline, ">>>", null);
						/* write out the debug line */
dinput: outline[term.t_ncol - 1] = 0;
	ostring(outline);
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
	{ int oldcmd = discmd;		        
	  int oldinp = disinp;
	  int oldstatus = cmdstatus;
	  
	  switch (c)
	  { case '?': strcpy(outline, TEXT128);     /* list commands */
		        
/*"(e)val exp, (c/x)ommand, (t)rack exp, (^G)abort, <SP>exec, <META> stop debug"*/
		      goto dinput;

	    case 'c': discmd = TRUE;			/* execute statement */
		      disinp = TRUE;
		      execcmd(FALSE, 1);
		      discmd = oldcmd;
		      disinp = oldinp;
		      goto dbuild;

	    case 'x': discmd = TRUE;		/* execute extended command */
		      disinp = TRUE;
		      namedcmd(FALSE, 1);
		      cmdstatus = oldstatus;
		      discmd = oldcmd;
		      disinp = oldinp;
		      goto dbuild;

	    case 'e': strcpy(temp, "set %track ");   /* evaluate expresion */
		      discmd = TRUE;
		      disinp = TRUE;
					getstring(&temp[11], NSTRING, "Exp: ");
		      discmd = oldcmd;
		      disinp = oldinp;
		      docmd(temp);
		      cmdstatus = oldstatus;
		      concat(&temp[0], " = [", gtusr("track"), "]", null);
		      mlforce(temp);
		      c = getkey();
		      goto dinput;

	    case 't': discmd = TRUE;			/* track expresion */
		      disinp = TRUE;
		      getstring(&temp[0], NSTRING, "Exp: ");
		      discmd = oldcmd;
		      disinp = oldinp;
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




BUFFER * dofilebuff;

/*	dofile: yank a file into a buffer and execute it
		if there are no errors, delete the buffer on exit */

static int Pascal dofile(const char * fname)
				/* file name to execute */
{
	   BUFFER *cb = curbp;	   /* temp to hold current buf while we read */

  register int cc;			/* results of various calls */
	   char bname[NBUFN];		/* name of buffer */

  makename(bname, fname); 		/* derive the name of the buffer */
  unqname(bname); 			/* make sure we don't stomp things */
{ BUFFER *dfb = bfind(bname, TRUE, 0);
  if (dfb == NULL) 			   /* get the needed buffer */
    return FALSE;

  curbp = dfb;			/* make this one current */
  curbp->b_flag = MDVIEW;	/* mark the buffer as read only */
				/* and try to read in the file to execute */
  cc = readin(fname, FALSE);
  curbp = cb;			/* restore the current buffer */
  if (cc != TRUE)
    return cc;
							/* go execute it! */
  cc = dobuf(dfb,1);
  if (cc == TRUE &&
      dfb->b_nwnd == 0)
		    /* not displayed, remove the now unneeded buffer and exit */
  { zotbuf(dfb);
    dfb = NULL;
  }
  dofilebuff = dfb;
  return cc;
}}

					/* execute the startup file */
int Pascal startup(char * sfname, int n)
	/*  sfname   ** name of startup file ("" if default) */
{
	register int cc;
					   /* look up the path for the file */
	const char *fspec = flook(0, sfname);
						      /* if it isn't around */
	if (fspec == NULL)
	{				  /* complain if we are interactive */
	  if (g_clexec == FALSE)
	    mlwrite(TEXT214, sfname);
/*				"%%No such file as %s" */
	   return 13;
	}
						/* otherwise, execute it */
	while (n-- > 0 && (cc = dofile(fspec)) == TRUE)
	  ;	        

	return cc;
}



int Pascal execfile(int f, int n)	/* execute a series of commands in a file */
	/* int f, n;	** default flag and numeric arg to pass on to file */
{ 
        char ebuffer[65];
	register int cc = mlreply(TEXT129, ebuffer, sizeof(ebuffer)-1);
	if (!cc)
/*			      "File to execute: " */
	  return cc;
					   /* look up the path for the file */
	return startup(ebuffer, n);
}

