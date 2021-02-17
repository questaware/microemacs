/*	EVAL.C: Expresion evaluation functions for MicroEMACS

	written 1986 by Daniel Lawrence 			*/

#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"
#include	"build.h"
#include	"edef.h"
#include	"etype.h"
#include	"evar.h"
#include	"elang.h"
#include	"map.h"
#include	"logmsg.h"


// extern char *getenv();

#define MLIX 3
extern char lastline[MLIX+1][NSTRING];
extern int  ll_ix;


#define UNDEF 0

NOSHARE int predefvars[NEVARS] =
{
256,   /* EVACOUNT */		  /* count until next ASAVE */
256,   /* EVASAVE */ 	
1,     /* EVBUFHOOK */		/* actual: sgarbf */
STOP,  /* EVCBFLAGS */		/* actual: kbdmode - curr keyboard macro mode*/
0,     /* EVCBUFNAME */		/* actual: kbdrd */
0,     /* EVCFNAME */		  /* actual: kbdwr */
UNDEF, /* EVCLIPLIFE */		/* actual: cliplife */
0,     /* EVCMDHK */ 		  /* actual: execlevel - execution IF level */
0,     /* EVCMODE */ 	  	/* actual: kbdrep */
4,		 /* EVCMTCOL */		  /* comment colour */
-1,    /* EVCOL1CH */		  /* character selecting colour 1 */
-1,    /* EVCOL2CH */		  /* character selecting colour n */
TRUE,  /* EVCURCHAR */		/* actual: revexist -does reverse video exist?*/
7,     /* EVCURCOL */		  /* */
0,     /* EVCURLINE */		/* */
FALSE, /* EVCWLINE */		  /* actual: mpresf  - Stuff in message line */
0,     /* EVDEBUG */      /* macro debugging flag	*/
0,     /* EVDIAGFLAG */   /* diagonal mouse movements? */
1,     /* EVDISCMD */     /* display command flag */
1,     /* EVDISINP */     /* display input characters */
0,     /* EVEXBHOOK */    /* actual: swb_luct */
0,     /* EVFCOL */		    /* The left hand offset on the screen */
0,     /* EVFILEPROF */	  /* The profiles of file types */
72,    /* EVFILLCOL */		/* not in use */
0,     /* EVGFLAGS */		  /* global control flag */
0,     /* EVGMODE */ 		  /* global editor mode */
7,     /* EVHARDTAB */		/* not in use */
0,     /* EVHIGHLIGHT */	/* not in use */
1,     /* EVHJUMP */		  /* horizontal jump size */
1,     /* EVHSCROLL */		/* horizontal scrolling flag */
CTRL |'U',/* EVINCLD */		/* actual: reptc - current universal rpt char*/
0,     /* EVKEYCT */		  /* consec key ct */
CTRL |'G',/* EVKILL */  	/* actual: abortc- current abort command char*/
CTRL |'M',/* EVLANG */  	/* actual: sterm - search terminating char */
0,     /* EVLASTDIR */	  /* actual: prefix- current pending prefix bits*/
0,     /* EVLASTKEY */    /* last keystoke */
0,     /* EVLASTMESG */   /* actual: prenum    "       "     numeric arg*/
TRUE,  /* EVLINE */       /* actual: predef    "       "    default flag*/
0,     /* EVMATCH */      /* actual: saveflag - Flags, saved with $target var */
1,     /* EVMSFLAG */     /* use the mouse? */
FALSE, /* EVPAGELEN */    /* actual: eexitflag */
TRUE,  /* EVPAGEWIDTH */		/* actual: sgarbf  - screen is garbage	*/
0,     /* EVPALETTE */    /* not in use */
0,     /* EVPENDING */    /* actual: nclicks - clrd on any non-mouse event*/
CTRL |'Q',/* EVPOPUP */    	/* quotec */
FALSE, /* EVREADHK */     /* not in use */
UNDEF, /* EVREGION */	
UNDEF, /* EVREPLACE */       
UNDEF, /* EVSEARCH */     
0,     /* EVSEED */       /* random number seed */
1,     /* EVSSAVE */      /* safe save flag */
1,     /* EVSSCROLL */		/* smooth scrolling enabled flag */
1,     /* EVSTATUS */			/* last command status */
UNDEF, /* EVSTERM */ 	
0,     /* EVUARG */				/* universal argument */
0,		 /* EV_USESOFTAB */
UNDEF, /* EVVERSION */	
UNDEF, /* EVWINTITLE */
UNDEF, /* EVWLINE */ 	
UNDEF, /* EVWRAPHK */	
UNDEF, /* EVWRITEHK */	
0,     /* EVXPOS */			/* current column mouse is positioned to*/
0,     /* EVYPOS */			/* current screen row	     "		*/
};

char *g_file_prof = NULL;

char * incldirs;


#if CALLED

Pascal varinit()	/* initialize the user variable list */

{ int i;
  for (i = MAXVARS; --i >= 0;)
    uv[i].u_name[0] = 0;
}

#endif


#if 0

Pascal varclean()	/* initialize the user variable list */

{ int i;

  for (i = MAXVARS - 1; i >= 0; --i)
    if (uv[i].u_name[0] != 0)
      free(uv[i].u_value);
}

#endif


static int absv(int x) /* take the absolute value of an integer */
	
{
  return x < 0 ? -x : x;
}

char *Pascal mkul(int wh, char * str)	/* make a string lower or upper case */
				/* 0 => lower */
				/* string to upper case */
{
	char *sp = str;
	char ch;
	
	for ( ;(ch = *sp) != 0; ++sp)
	  *sp = wh ? toupper(ch) : tolower(ch);
	return str;
}



static char * Pascal plinecpy()
	
{ int here = getccol();
  
  static char line[2];
  line[0] = 0;

{ LINE * pline = curwp->w_dotp;
  
  if (curwp->w_linep != NULL)
  {
    while (pline != curwp->w_linep)
    { pline = lback(pline);
    { int offs = getgoal(pline, here);
      if (offs < llength(pline))
      { line[0] = pline-> l_text[offs];
        break;
      }
    }}
  }
  return line;
}}

static Map_t fnamemap = mk_const_map(T_DOMCHAR0+4, 2, funcs);
/*
typedef struct Map_s
{ Format_t format;
  Char *   srch_key;	// These 2 form a pair.   See Note 1.
  Short    last_ix;

  Short    max_len;	// in bytes
  Short    curr_len;	// in bytes
  Short    curr_mult;   // in entries
  Int	   update_ct;
  Byte     c[1];	// actually Mapeny_t
} Map_t, *Map;
*/

static struct
{ char * result;
	int    top;
	int		 lim;
} g_stk;

static char * push_arg(char * src, int fnum)

{ int sl = strlen(src);
	if (fnum != UFCAT)
		sl = (sl + 2) & -2;

	g_stk.top += sl;
	
	if (g_stk.top >= g_stk.lim)
	{ g_stk.lim += 2 * NSTRING;
	  (void)remallocstr(&g_stk.result, g_stk.result, g_stk.lim + 2 * NSTRING);
	}

{	char * tgt = &g_stk.result[g_stk.top];
	int rc = fnum == 0 ? TRUE : macarg(tgt);
	if ((fnum == UFXLATE || fnum == UFMID) && rc == TRUE)
		rc = macarg(tgt+strlen(tgt)+1);
	g_stk.top -= sl;
	
	return fnum == 0  ? tgt - sl :
				 rc != TRUE ? NULL : tgt;
}}


static const char *Pascal gtfun(char * fname)/* evaluate a function */

{	int fnum;
	int iarg1;
	int iarg2;

	const char * errorm = g_logm[2];

	char * arg2 = NULL;											/* to suppress warning */
	char * arg1 = push_arg("", 0);					/* to initialise area */

//*arg1 = 0;
											      /* look the function up in the function table */
	fname[3] = 0;							/* only first 3 chars significant */
  mkul(0, fname);
	fnamemap.srch_key = fname;
	fnum = binary_const(&fnamemap, funcs);
	if (fnum < 0)
	  return errorm;
																		/* if needed, retrieve the first argument */
	if (funcs[fnum].f_type >= MONAMIC)
	{ 
	  if (macarg(arg1) != TRUE)
	    return errorm;
												 /* if needed, retrieve the second and third arguments */
	  if (funcs[fnum].f_type >= DYNAMIC)
	  { arg2 = push_arg(arg1, fnum);		// arglen: length of arg1
			if (arg2 == NULL)
	      return errorm;

      iarg2 = atoi(arg2);
	  }
	  iarg1 = atoi(arg1);
	}
						/* and now evaluate it! */

	if      (funcs[fnum].f_kind < RINT)
	 switch (fnum)
	 {case UFDIT:  return plinecpy();
		when UFCAT:  return arg1;
		when UFLEFT: if (g_stk.top + iarg2 < g_stk.lim)
		               arg1[iarg2] = 0;
		             return arg1;
		when UFRIGHT:iarg1 -= iarg2;
				         return iarg1 <= 0 ? arg1 : strcpy(arg1, &arg1[iarg1]);
		when UFTRIM: return iarg1 = 0, trimstr(arg1, &iarg1);

		when UFDIR:	 return pathcat(arg1, NSTRING-1, arg1, arg2);
		when UFIND:	 return getval(arg1, arg1);

		when UFLOWER:
		case UFUPPER:	return mkul(fnum == UFUPPER, arg1);
		when UFGTKEY:   
		case UFCHR:	  arg1[0] = fnum == UFCHR ? iarg1 : tgetc();
				          arg1[1] = 0;
				          return arg1;
		when UFGTCMD:	return cmdstr(&arg1[0], getcmd());
		when UFENV:
#if	ENVFUNC
				          return fixnull(getenv(arg1));
#else
				          return "";
#endif
		when UFBIND:	return transbind(arg1);
		when UFFIND:
				          return fixnull(flook(0, arg1));
		when UFXLATE:
		case UFMID:	 { char * arg3 = arg2 + strlen(arg2) + 1;
									 if (fnum == UFXLATE)
				           	 return xlat(arg1, arg2, arg3);
									 
								 { int n = atoi(arg2);
								 	 if (n < 0)
								 	 	 n = 0;
								 { int end = n + atoi(arg3);
								 	 if (end >= 0 && end < g_stk.lim)
								 	   arg1[end] = 0;
				           return arg1+n;
			           }}}
#if DIACRIT
		when UFSLOWER:setlower(arg1, arg2);
				          return "";
		when UFSUPPER:setupper(arg1, arg2);
#endif
		default:	    return "";
	 }
	else if (funcs[fnum].f_kind == RINT)
	{switch (fnum)
	 {case UFABS: iarg1 = absv(iarg1);
		when UFADD:	 iarg1 += iarg2;
		when UFSUB:	 iarg1 -= iarg2;
		when UFTIMES:iarg1 *= iarg2;
		when UFDIV:	 iarg1 /= iarg2;
		when UFMOD:	 iarg1 = iarg1 % iarg2;
		when UFNEG:	 iarg1 = -iarg1;
		
		when UFASCII:	iarg1 = (int)arg1[0];
		when UFGTKEY:   
		when UFRND:   iarg1 = (ernd() % absv(iarg1)) + 1;
		when UFSINDEX:iarg1 = sindex(arg1, arg2);
		when UFBAND:	iarg1 &= iarg2;
		when UFBOR:	  iarg1 |= iarg2;
		when UFBXOR:	iarg1 ^= iarg2;
		when UFBNOT:	iarg1 = ~iarg1;
		when UFLENGTH:iarg1  = strlen(arg1);
		otherwise		  return "";
	 }
	 return int_asc(iarg1);
	}
	else
	{switch (fnum)
   {case UFEQUAL:		iarg1 = iarg1 == iarg2;
		when UFLESS:		iarg1 = -iarg1;
		                iarg2 = -iarg2;
		case UFGREATER: iarg1 = iarg1 > iarg2;

		when UFSEQUAL:
		case UFSLESS:
		case UFSGREAT:	iarg1 = strcmp(arg1, arg2);
				/*if (! in_range(iarg1, -1, 1))
				    adb(iarg1);*/
		                if      (fnum == UFSLESS)
		                  iarg1 = (iarg1 & 0x100);
		                else if (fnum == UFSEQUAL)
		                  iarg1 = iarg1 == 0;
		                else if (iarg1 < 0)
		                  iarg1 = 0;
		when UFNOT: 	  iarg1 = stol(arg1);
										iarg1 ^= 1;
		when UFAND:
		case UFOR:		  iarg1 = stol(arg1);
									  iarg2 = stol(arg2);
									  if (fnum == UFOR)
									    iarg1 |= iarg2;
									  else
									    iarg1 &= iarg2;

		when UFTRUTH:	  iarg1 = iarg1 == 42;  /* ???? */
		when UFEXIST:	  iarg1 = fexist(arg1);
	 }
	 return ltos(iarg1);
	}
}


int uv_vnum;

const char *Pascal gtusr(char * vname)	/* look up a user var's value */
																				/* name of user variable to fetch */
{
	int vnum;	/* ordinal number of user var */

			/* scan the list looking for the user var name */
	for (vnum = MAXVARS; --vnum >= 0 && uv[vnum].u_name[0] != 0; )
	  if (strcmp(vname, uv[vnum].u_name) == 0)
	  { char * vptr = uv[vnum].u_value;
	    if (vptr == null)
	      break;

	    uv_vnum = vnum;
	    return vptr;
	  }
	
	uv_vnum = vnum;
	return g_logm[2];						// "ERROR"
}

#if 0

char *Pascal funval(i)
	int i;
{ return funcs[i].f_name;
}

char *Pascal envval(i)
	int i;
{ return envars[i];
}


Pascal binary(key, tval, tlength)
	char *key;		/* key string to look for */
	char *(Pascal *tval)(); /* ptr to function to fetch table value */
	int tlength;		/* length of table to search */
{
	int l = 0;	  /* set current search limit as entire list */
	int u = tlength - 1;
							/* get the midpoint! */
	while (u >= l)
	{ int i = (l + u) >> 1;

	  int cresult = strcmp(key, (*tval)(i));
	  if (cresult == 0)
	    return i;
	  if (cresult < 0)
	    u = i - 1;
	  else
	    l = i + 1;
	}
	return -1;
}

#endif


static Map_t evmap = mk_const_map(T_DOMSTR, 0, envars);


const char *Pascal gtenv(const char * vname)
 			/* name of environment variable to retrieve */
{
  int ix = 0;
	int vnum;		/* ordinal number of var refrenced */
#define res vnum
 extern char deltaf[NSTRING];
#define result (&deltaf[NSTRING / 2])   /* leave beginning for extra safety */
 /* s tatic char result[NSTRING + 1];	** string result */

			  /* scan the list, looking for the referenced name */
	evmap.srch_key = vname;
	vnum = binary_const(&evmap, envars);
	
	if (vnum < 0) 												/* return errorm on a bad reference */
		return g_logm[2];

	switch (vnum)
	{ case EVPAGELEN: res = term.t_nrowm1 + 1;
	  when EVPAGEWIDTH:res = term.t_ncol;
	  when EVCURCOL:  res = getccol();
	  when EVCURLINE: res = setcline();
	  when EVHARDTAB: res = curbp->b_tabsize;
		when EVUSESOFTTAB: res = curbp->b_mode & BSOFTTAB ? 1 : 0;
	  when EVCBFLAGS: res = curbp->b_flag;
	  case EVCMODE:   if (vnum == EVCMODE) res = res >> NUMFLAGS;
	  when EVCBUFNAME:return curbp->b_bname;
	  when EVCFNAME:  return fixnull(curbp->b_fname);
//  when EVSRES:	  return sres;
	  when EVPALETTE: return palstr;
	  when EVFILEPROF:return g_file_prof;
	  when EVCURCHAR: res = llength(curwp->w_dotp) == curwp->w_doto 
                    								? '\n' : lgetc(curwp->w_dotp, curwp->w_doto);
	  when EVWLINE:   res = curwp->w_ntrows;
	  when EVCWLINE:  res = getwpos();
	  when EVSEARCH:  return pat;
	  when EVHIGHLIGHT: return highlight;
	  when EVLASTDIR: res = lastdir;
	  when EVINCLD:	  return incldirs;
	  when EVREPLACE: return rpat;
	  when EVMATCH:   return fixnull(patmatch);
	  when EVKILL:	  return getkill();
	  when EVCLIPLIFE:res = g_cliplife;
	  when EVREGION:  return getreg(&result[0]);
	  
	  when EVPENDING:
#if	GOTTYPAH
									  return ltos(typahead());
#else
									  return g_logm[0];
#endif
	  when EVLINE:	  return getctext(&result[0]);
	  when EVSTERM:   return cmdstr(&result[0], sterm);
	  when EVLASTMESG:return lastmesg;
	  when EVFCOL:	  res = curwp->w_fcol;

	  when EVBUFHOOK:
	  case EVEXBHOOK: ++ix;
	  case EVWRITEHK: ++ix;
	  case EVCMDHK:   ++ix;
	  case EVWRAPHK:  ++ix;
	  case EVREADHK:  ++ix;
						   		  return getfname(-ix);
	  when EVVERSION: return VERSION;
	  when EVLANG:	  return LANGUAGE;
    when EVZCMD:    return lastline[ll_ix & MLIX];
#if S_WIN32
	  when EVWINTITLE:return "";	// getconsoletitle();
#endif
	  when EVDEBUG:   
	  case EVSTATUS:  
	  case EVDISCMD:  
	  case EVDISINP:  
	  case EVSSCROLL: 
	  case EVSSAVE:   
	  case EVHSCROLL: 
	  case EVDIAGFLAG:
	  case EVMSFLAG:  return ltos(predefvars[vnum]);

	  otherwise       res = predefvars[vnum];
										loglog2("Var %d = %x", vnum, res);
	}
	return int_asc(res);
#undef result
}

const char *Pascal fixnull(const char * s)/* Don't return NULL pointers! */
	
{
  return s == NULL ? "" : s;
}


char * Pascal trimstr(char * s, int * from)/* trim whitespace off string */
			/* string to trim */
{
	char *sp = &s[*from];
        
	while (--sp >= s && *sp <= ' ')
	  *sp = 0;
	  
	*from = sp - s + 1;

	return s;
}



static int Pascal findvar(char * var)      /* find a variables type and name */
	
{	int vtype;	/* type to return */

fvar:	vtype = -1;
	switch (var[0])
	{ case '$':			/* check for legal enviromnent var */
		    evmap.srch_key = &var[1];
	      vtype = binary_const(&evmap, envars)|(TKENV << 11);

	  when '%':		  /* check for existing legal user variable */
	  	  if (gtusr(&var[1]) != g_logm[2])
		      vtype = (TKVAR << 11) | uv_vnum;
		    else
		    { vtype = uv_vnum;
		      if (vtype >= 0)
		      { strcpy(uv[vtype].u_name, &var[1]);
						uv[vtype].u_value = NULL;
						vtype |= TKVAR << 11;
		      }
		    }				/* indirect operator? */
	  when '&': 
		    if (var[1] == 'i' && var[2] == 'n' && var[3] == 'd' && g_clexec)
		    {			  /* grab token, and eval it */
		      execstr = token(execstr, var, NVSIZE+1);
		      getval(&var[0], var);
		      goto fvar;
		    }
	}
	return vtype;
}



int Pascal set_var(char var[NVSIZE+1], char * value)	/* set a variable */
					/* name of variable to fetch */
					/* value to set variable to */
{
	int  vd = findvar(var);		/* variable num/type */
        
	if (vd < 0)
	{ mlwrite(TEXT52, var);
/*			"%%No such variable as '%s'" */
	  return FALSE;
	}

	if (value == NULL)
	  value = "";

#if DEBUGM == 0
	return svar(vd, value);	/* and set the appropriate value */
#else
{	int cc = svar(vd, value);	/* and set the appropriate value */
		/* if $debug == TRUE, every assignment will echo a statment to
							that effect here. */
	if (macbug && (strcmp(var, "%track") != 0))
	{ ++g_discmd;
	  mlwrite("(%s <- %s)", var, value);
    --g_discmd;
	  update(TRUE);
	        
	  if (getkey() == abortc) /* and get the keystroke to hold the output */
	  { mlforce(TEXT54);
/*				"[Macro aborted]" */
	    cc = FALSE;
	  }
	}
	return cc;
}
#endif
}


int Pascal setvar(int f, int n)	/* set a variable */
	/* int n;	** numeric arg (can overide prompted value) */
{
	char ch;
  int cc;
	char var[2*NSTRING+1];	/* name of variable to fetch */
	     var[0] = 0;
					/* first get the variable to set.. */
	if (g_clexec == FALSE)
	{ cc = getstring(&var[0], sizeof(var), TEXT51);
/*				 "Variable to set: " */
	  if (cc != TRUE)
	    return ABORT;
	} 
	else				/* grab token and skip it */
	{ execstr = token(execstr, var, NVSIZE + 1);
	}

  for (cc = -1; (ch = var[++cc]) != 0 &&
							 (g_clexec || ch != ' ' && ch != '='); )
    ;
	var[cc+(var[cc] == 0)] = 0;
														/* get the value for that variable */
	if 	    (f != FALSE)
	  strcpy(&var[cc+1], int_asc(n));
	else if (var[cc+1] == 0)
	{ if (mlreply(TEXT53, &var[cc+1], NSTRING-cc+1) != TRUE)
/*				 "Value: " */
	    return ABORT;
	}

	return
#if S_MSDOS == 0
	        var[0] == '#' ? use_named_str(&var[1], &var[cc+1]) :
#endif
	                        set_var(var, &var[cc+1]);
}

/*
 * Set current column.
 */
static
 int Pascal setccol(int pos)
												/* position to set cursor */
{
	int llen = llength(curwp->w_dotp);
	int offs;
	for (offs = -1; ++offs < llen; )
	{ curwp->w_doto = offs;
		if (getccol() >= pos)
			break;
	}	

	return TRUE;
}


int Pascal svar(int var, char * value)	/* set a variable */

{	int cc = TRUE;
  int vnum = var & 0x7ff;

  if (var == (TKENV << 11) + EVINCLD)
    cc = FALSE;

  if (var - vnum == (TKVAR << 11) || !cc)
  {     
    return remallocstr(cc ? &uv[vnum].u_value : &incldirs, value, 0) != null;
  }
  else
  { int val = atoi(value);
    int hookix = 0;		  /* set an environment variable */

    switch (vnum) 
    {
#if S_WIN32
//    when EVWINTITLE: setconsoletitle( value );
#endif
	    when EVHARDTAB:	 if (val > 0)
                       { tabsize = val;
                  	     curbp->b_tabsize = val;
                       }
	                     upwind();
			when EVUSESOFTTAB: curbp->b_mode &= ~BSOFTTAB;
											 if (val)
													curbp->b_mode |= BSOFTTAB;
	    when EVPAGELEN:	 cc = newdims(term.t_ncol, val);
	    when EVPAGEWIDTH:cc = newdims(val, term.t_nrowm1+1);
	    when EVCURCOL:	 cc = setccol(val);
	    when EVCURLINE:	 cc = gotoline(TRUE, val);
	    when EVCBFLAGS:	 lchange(WFMODE);
                     /*if (val & BFCHG)
                         mbwrite("EVAL");*/
	    case EVCMODE:
	        						 hookix = curbp->b_flag;
	        						 if (vnum == EVCBFLAGS)
	    	  						   hookix = (hookix & ~(BFCHG|BFINVS))
											          | (val & (BFCHG|BFINVS));
			    						 else
			    						 { hookix &= MDSTT - 1;
				  						   hookix |= val << NUMFLAGS;
				  						   curwp->w_flag |= WFMODE;
			 	  						 }
				  						 curbp->b_flag = hookix;
	    when EVCBUFNAME: return FALSE;						// read only
	    when EVCFNAME:	 curwp->w_flag |= WFMODE;
	    								 repl_bfname(curbp, value);
//	  when EVSRES:	   cc = TTrez(value);
	    when EVFILEPROF: if (g_file_prof != NULL)
	    	               	 free(g_file_prof);
				               g_file_prof = strdup(value);
				
	    when EVCURCHAR:	 ldelchrs(1L, FALSE);	/* delete 1 char */
			                 linsert(1, (char)val);
				               backchar(FALSE, 1);
											 updline(FALSE);
	    when EVWLINE:	   cc = resize(FALSE, val);
	    when EVCWLINE:	 cc = forwline(FALSE, val - getwpos());
	    when EVSEARCH:	 //mcclear();
											 strpcpy(pat,value,NPAT);
	    when EVREPLACE:	 strpcpy(rpat,value,NPAT);
	    when EVHIGHLIGHT:strpcpy(highlight,value,sizeof(highlight));
	    when EVLASTMESG: strpcpy(lastmesg,value,NSTRING);
	    when EVPALETTE:	 strpcpy(palstr,value,sizeof(palstr));
	    when EVPOPUP:    mbwrite(value);
				           //  upwind();
	    when EVKILL:
		  when EVCLIPLIFE: g_cliplife = val;
	    when EVLINE:	   return FALSE;						// read only
	    when EVSTERM:	   sterm = stock(value);
	    when EVFCOL:	   if (val < 0)
	                       val = curwp->w_doto;
	                     minfcol = val;
	                     curwp->w_fcol = val;
				               curwp->w_flag |= WFHARD | WFMODE;
	    when EVBUFHOOK:
	    case EVEXBHOOK: ++hookix;
	    case EVWRITEHK: ++hookix;
	    case EVCMDHK:   ++hookix;
	    case EVWRAPHK:  ++hookix;
	    case EVREADHK:  ++hookix;
	  				  		    setktkey(&hooks[hookix], BINDFNC, value);

	    when EVHSCROLL: lbound = 0;
#if S_WIN32
		  when EVWINTITLE:setconsoletitle(value);
#endif
	    when EVDEBUG:	
	    case EVSTATUS:	
	    case EVDISCMD:	
	    case EVDISINP:	
	    case EVSSCROLL:	
	    case EVSSAVE:	
	    case EVDIAGFLAG:	
	    case EVMSFLAG:  predefvars[vnum] = stol(value);

	    when EVHJUMP:   if (val <= 0)
									      val = 1;
										  if (val > term.t_ncol - 1)
					              val = term.t_ncol - 1;
	    default:	      predefvars[vnum] = val;
    }
  }
  return cc;
}



int Pascal stol(char * val)					/* convert a string to a numeric logical */
{
  return val[0] == 'T' || atoi(val) != 0;	/* check for logical values */	 
}


#define INTWIDTH (sizeof(int) * 3)


char *Pascal int_asc(int i)
			/* integer to translate to a string */
{
  static char result[INTWIDTH+2];
	memset(&result, ' ', INTWIDTH); 

{	char *sp = &result[INTWIDTH+1];
	int v = i;		/* sign of resulting number */

	if (v < 0)
	  v = -v;

	*sp = 0;
	do 
	{ *(--sp) = '0' + v % 10;	/* and install the new digit */
	  v = v / 10;
	} while (v);

	if (i < 0)
	  *(--sp) = '-';		/* and install the minus sign */

	return sp;
}}

int Pascal gettyp(char * tok)	/* find the type of a passed token */

{
	char c = *tok;	/* first char in token */
        
	if (c >= '0' && c <= '9')	/* a numeric literal? */
	  return TKLIT;

	switch (c)
	{	case 0:		return TKNUL;
		case '"':	return TKSTR;

		case '@':	return TKARG;
		case '#':	return TKBUF;
		case '$':	return TKENV;
		case '%':	return TKVAR;
		case '&':	return TKFUN;
		case '*':	return TKLBL;

		default:	return TKCMD;
	}
}

char getvalnull[] = "";

				/* the oob checks are faulty */
					/* find the value of a token */
char *Pascal getval(char * tgt, char * tok) 
														/* token: token to evaluate */
{	int blen = NSTRING;
	BUFFER *bp;			/* temp buffer pointer */
	int typ = gettyp(tok);

	switch (typ)
	{ case TKNUL:   return "";
	  case TKARG:															/* interactive argument */
						getval(&tok[0], &tok[1]);
		      { int sdc = g_discmd;							/* echo it always! */
						g_discmd = TRUE;
					{	Cc cc = getstring(&tgt[0], NSTRING, tok);
						g_discmd = sdc;
						return cc == ABORT ? getvalnull : tgt;
		      }}
	  case TKBUF:			/* buffer contents fetch */
										/* grab the right buffer */
						bp = bfind(getval(tgt, &tok[1]), FALSE, 0);
						if (bp == NULL)
						  return getvalnull;
															/* if the buffer is displayed, get the window
												      	 vars instead of the buffer vars */
						if (bp->b_nwnd > 0)
						{ curbp->b_dotp = curwp->w_dotp;
						  curbp->b_doto = curwp->w_doto;
						}
					{ LINE * ln = bp->b_dotp;
						if (ln->l_props & L_IS_HD)
						  break;
							  		/* step the buffer's line ptr ahead a line */
						bp->b_dotp = lforw(ln);
						bp->b_doto = 0;

									/* if displayed buffer, reset window ptr vars*/
						if (bp->b_nwnd > 0)
						{ curwp->w_dotp = curbp->b_dotp;
						  curwp->w_doto = 0;
						  curwp->w_flag |= WFMOVE;
						}
						if (ln->l_used + 1 < NSTRING)
						  blen = ln->l_used + 1;
							              /* grab the line as an argument */
						bp = (BUFFER*)&ln->l_text[0];
		      }
	  when TKVAR:	bp = (BUFFER*)gtusr(tok+1);
	  when TKENV:	bp = (BUFFER*)gtenv(tok+1);
	  when TKFUN:	bp = (BUFFER*)gtfun(tok+1);
	  when TKLIT:
	  case TKCMD:	bp = (BUFFER*)tok;
	  when TKSTR: bp = (BUFFER*)(tok + 1);
	  otherwise   tgt[0] = 0;
	              return tgt;
	}

	strpcpy(tgt, (char*)bp, blen);
	if (typ == TKSTR)
	{ char * t;
	  for (t = tgt-1; *++t != 0 && *t != '"'; )
	    ;
	  *t = 0;
	}
	return tgt;
}




const char *Pascal ltos(int val)	/* numeric logical to string logical */
			/* value to translate */
{
  return g_logm[val == 0 ? 0 : 1];
}


int Pascal ernd()	/* returns a random integer */

{	seed = absv(seed * 1721 + 10007);
	return seed;
}

int Pascal sindex(char * source, char * pattern) /* find pattern within source */
	/* char *source;	* source string to search */
	/* char *pattern;	* string to look for */
{
	char *sp;

	for (sp = source; *sp; ++sp)
	{ int ix;  
	  for (ix = -1; pattern[++ix] != 0 && myeq(pattern[ix], sp[ix]); )
	    ;
	    						/* was it a match? */
	  if (pattern[ix] == 0)
	    return (int)(sp - source) + 1;
	}
						/* no match at all.. */
	return 0;
}

/*	Filter a string through a translation table	*/

char *Pascal xlat(char * srctgt, char * lookup, char * trans)
	/* char *srctgt;	* string to filter */
	/* char *lookup;	* characters to translate */
	/* char *trans;		* resulting translated characters */
{
	char *sp;	/* pointer into source table */
	char *lp;	/* pointer into lookup table */

	for (sp = srctgt; *sp; ++sp)
	{ for (lp = lookup; *lp; ++lp)
	    if (*sp == *lp)
	    { *sp = trans[lp - lookup];
	      break;
	    }
	}

	return srctgt;
}


void fmt_desv(char pfx, const char * v, const char * val)
	
{					/* and add it as a line to the buffer */
#if 0
  (void)mlwrite("%>%c%14s %s\n", pfx, v, val);
#else
  char buf[2];
  buf[0] = pfx;
  buf[1] = 0;
  linstr(buf);
  linstr(v);
  linstr(&"              "[strlen(v)]);
  linstr(val);
  lnewline();
#endif
}



void Pascal mkdes()

{ lnewline();

  curbp->b_flag |= MDVIEW;
  curbp->b_flag &= ~BFCHG;			/* don't flag this as a change */
  curwp->w_dotp = lforw(curbp->b_baseline);     /* back to the beginning */
  curwp->w_doto = 0;
//upmode();
  mlerase();					/* clear the mode line */
}

/*	describe-variables	Bring up a fake buffer and list the contents
				of all the environment variables
*/

Pascal desvars(int f, int n)

{ int uindex = curbp->b_tabsize;   /* index into uvar table */
        
  openwindbuf(TEXT56);
  curbp->b_tabsize = uindex;
				      /* build the environment variable list */
  for (uindex = -1; ++uindex < NEVARS; )
    fmt_desv('$', envars[uindex], gtenv(envars[uindex]));

  lnewline();
					    /* build the user variable list */
  for (uindex = MAXVARS; --uindex >= 0 && uv[uindex].u_name[0] != 0; )
    fmt_desv('%', uv[uindex].u_name, uv[uindex].u_value);

  mkdes();
  return TRUE;
}

/*------------------------------------------------------------*/

#if	DEBUGM

int Pascal dispvar(int f, int n)	/* display a variable's value */
	/* int n;		** numeric arg (can overide prompted value) */
{
	char var[NVSIZE+1];	/* name of variable to fetch */
	char val[NSTRING];

												/* first get the variable to display.. */
	if (g_clexec == FALSE)
  { int cc = getstring(&var[0], NVSIZE+1, TEXT55);
																/* "Variable to display: " */
	  if (cc != TRUE)
	    return cc;
	} 
	else																/* macro line argument */
	{																		/* grab token and skip it */
	  execstr = token(execstr, var, NVSIZE + 1);
	}
														/* check the legality, find the var */
	if (findvar(var) < 0)
	{ mlwrite(TEXT52, var);
					/*	"%%No such variable as '%s'" */
	  return FALSE;
	}
				        
	mlwrite("%s = %s", var, getval(val, var));
	return update(TRUE);
}

#endif


/*	describe-functions	Bring up a fake buffer and list the
				names of all the functions
*/

Pascal desfunc(int f, int n)

{	int uindex;	/* index into funcs table */

	openwindbuf(TEXT211);
						  /* build the function list */
	for (uindex = -1; ++uindex < NFUNCS; )
	{			     /* add in the environment variable name */
	  fmt_desv('&', funcs[uindex].f_name, "");
	}

  mkdes();
	return TRUE;
}
