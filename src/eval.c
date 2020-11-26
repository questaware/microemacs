/*	EVAL.C: Expresion evaluation functions for
		MicroEMACS

	written 1986 by Daniel Lawrence 			*/

#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"
#include	"build.h"
#include	"edef.h"
#include	"etype.h"
#include	"evar.h"
#include	"elang.h"
#include	"h/map.h"
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
0,     /* EVCMDHK */ 		  /* actual: execlevel - execution IF level */
0,     /* EVCMODE */ 	  	/* actual: kbdrep */
0x71,  /* EVCMTCOL */		  /* comment colour *//* 0x80:blink,  0x8:intense */
-1,    /* EVCOL1CH */		  /* character selecting colour 1 */
-1,    /* EVCOL2CH */		  /* character selecting colour 2 */
FALSE, /* EVCURCHAR */		/* actual: revexist -does reverse video exist?*/
7,     /* EVCURCOL */		  /* */
0,     /* EVCURLINE */		/* */
TRUE,  /* EVCURWIDTH */		/* actual: sgarbf  - screen is garbage	*/
FALSE, /* EVCWLINE */		  /* actual: mpresf  - Stuff in message line */
0,     /* EVDEBUG */         	/* macro debugging flag	*/
0,     /* EVDIAGFLAG */      	/* diagonal mouse movements? */
1,     /* EVDISCMD */        	/* display command flag */
1,     /* EVDISINP */        	/* display input characters */
0,     /* EVEXBHOOK */       	/* actual: swb_luct */
0,     /* EVFCOL */		        /* The left hand offset on the screen */
0,     /* EVFILEPROF */	      /* The profiles of file types */
72,    /* EVFILLCOL */		/* not in use */
1,     /* EVFLICKER */		/* do flicker supression? */
0,     /* EVGFLAGS */		  /* global control flag */
0,     /* EVGMODE */ 		  /* global editor mode */
7,     /* EVHARDTAB */		/* not in use */
0,     /* EVHIGHLIGHT */		/* not in use */
1,     /* EVHJUMP */		    /* horizontal jump size */
1,     /* EVHSCROLL */		  /* horizontal scrolling flag */
CTRL |'U', /* EVINCLD */	  /* actual: reptc - current universal rpt char*/
0,     /* EVKEYCT */		    /* consec key ct */
CTRL |'G', /* EVKILL */       /* actual: abortc- current abort command char*/
CTRL |'M', /* EVLANG */       /* actual: sterm - search terminating char */
0,     /* EVLASTDIR */	      /* actual: prefix- current pending prefix bits*/
0,     /* EVLASTKEY */       	/* last keystoke */
0,     /* EVLASTMESG */      	/* actual: prenum    "       "     numeric arg*/
TRUE,  /* EVLINE */          	/* actual: predef    "       "    default flag*/
0x11,  /* EVLWIDTH */		      /* actual: quotec quote char during mlreply() */
0,     /* EVMATCH */         	/* actual: saveflag - Flags, saved with $target var */
1,     /* EVMODEFLAG */      	/* display mode lines flag */
1,     /* EVMSFLAG */        	/* use the mouse? */
FALSE, /* EVPAGELEN */       	/* actual: eexitflag */
0,     /* EVPALETTE */       	/* not in use */
0,     /* EVPENDING */       	/* actual: nclicks - clrd on any non-mouse event*/
0,     /* EVPOPYOP */       	/* */
CTOEC('\r'),/* EVRAM */       /* actual: gs_term - param to getstring() */
FALSE, /* EVREADHK */        	/* not in use */
UNDEF, /* EVREGION */	
UNDEF, /* EVREPLACE */       
0,     /* EVRVAL */          	/* return value of a subprocess (niu) */
UNDEF, /* EVSEARCH */        
0,     /* EVSEED */          	/* random number seed */
0,     /* EVSOFTTAB */	 /* current soft tab size (0: use hard tabs) */
UNDEF, /* EVSRES */          
1,     /* EVSSAVE */         	/* safe save flag */
1,     /* EVSSCROLL */	 /* smooth scrolling enabled flag */
1,     /* EVSTATUS */		 /* last command status */
UNDEF, /* EVSTERM */ 	
UNDEF, /* EVTARGET */	
UNDEF, /* EVTERM */
UNDEF, /* EVTIME */		
0,     /* EVUARG */			 /* universal argument */
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

{ register int i;
  for (i = MAXVARS; --i >= 0;)
    uv[i].u_name[0] = 0;
}

#endif


#if 0

Pascal varclean()	/* initialize the user variable list */

{ register int i;

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
	register char *sp = str;
	register char ch;
	
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

static char result[2 * NSTRING];
       int  stktop = 0;

static const char *Pascal gtfun(char * fname)/* evaluate a function */
	
{
		 int fnum;
	register int iarg1;
		 int  iarg2;
		 int  arglen;
		 char * arg1 = &result[stktop];
		 char * arg2;

//*arg1 = 0;
			      /* look the function up in the function table */
	fname[3] = 0;			/* only first 3 chars significant */
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

				/* if needed, retrieve the second argument */
	  if (funcs[fnum].f_type >= DYNAMIC)
	  { arglen = strlen(arg1);
	    if (fnum != UFCAT)
	      arglen = (arglen + 2) & -2;
	    stktop += arglen;
	    arg2 = &result[stktop];
	    if (macarg(arg2) != TRUE)
	    { stktop -= arglen;
	      return errorm;
	    }
	    stktop -= arglen;
      iarg2 = atoi(arg2);
	  }
	  iarg1 = atoi(arg1);
	}
						/* and now evaluate it! */
	if (funcs[fnum].f_kind == RINT)
	{ switch (fnum)
	  {case UFABS: iarg1 = absv(iarg1);
		when UFADD:	 iarg1 += iarg2;
		when UFSUB:	 iarg1 -= iarg2;
		when UFTIMES:iarg1 *= iarg2;
		when UFDIV:	 iarg1 /= iarg2;
		when UFMOD:	 iarg1 = iarg1 % iarg2;
		when UFNEG:	 iarg1 = -iarg1;
		
		when UFDIT:  return plinecpy();
		when UFCAT:  return arg1;
		when UFLEFT: if (stktop + iarg2 < 2 * NSTRING - 2)
		               arg1[iarg2+1] = 0;
		             return arg1;
		when UFRIGHT:
		case UFMID:	
		case UFLENGTH:
		case UFTRIM: iarg1  = strlen(arg1);
				         if (fnum == UFLENGTH)
				         	 break;

				         if (fnum == UFRIGHT)
								 { 
									 iarg1 -= iarg2;
				           if (iarg1 < 0)
				             iarg1 = 0;
				           return strcpy(arg1, &arg1[iarg1]);
								 }
				         
				         return fnum == UFTRIM 				 					 ? arglen = iarg1, trimstr(arg1, &arglen) :
								        (unsigned)iarg2 >= (unsigned)arglen ? ""    :
							   																strpcpy(arg1,&arg1[iarg2-1], atoi(arg2)+1);

		when UFDIR:	 return pathcat(arg1, NSTRING-1, arg1, arg2);
		when UFIND:	 return getval(arg1, arg1);

		when UFLOWER:
		case UFUPPER:	return mkul(fnum == UFUPPER, arg1);
		when UFASCII:	iarg1 = (int)arg1[0];
		when UFGTKEY:   
		case UFCHR:	  arg1[0] = fnum == UFCHR ? iarg1 : tgetc();
				          arg1[1] = 0;
				          return arg1;
		when UFGTCMD:	return cmdstr(&arg1[0], getcmd());
		when UFRND:   iarg1 = (ernd() % absv(iarg1)) + 1;
		when UFSINDEX:iarg1 = sindex(arg1, arg2);
		when UFENV:
#if	ENVFUNC
				          return fixnull(getenv(arg1));
#else
				          return "";
#endif
		when UFBIND:	return transbind(arg1);
		when UFFIND:
				          return fixnull(flook(0, arg1));
		when UFBAND:	iarg1 &= iarg2;
		when UFBOR:	  iarg1 |= iarg2;
		when UFBXOR:	iarg1 ^= iarg2;
		when UFBNOT:	iarg1 = ~iarg1;
		when UFXLATE:{ arglen += (strlen(arg2) + 2) & -2;
		               stktop += arglen;
		             { char * arg3 = &result[stktop];
		               if (macarg(arg3) != TRUE) 
		               { stktop -= arglen;
				             return errorm;
				           }
		               stktop -= arglen;
		               return xlat(arg1, arg2, arg3);
			           }}
#if DIACRIT
		when UFSLOWER:setlower(arg1, arg2);
				          return "";
		when UFSUPPER:setupper(arg1, arg2);
#endif
		default:	    return "";
	  }
	  return int_asc(iarg1);
	}
	else
	{ /*printf(" FN %d ", fnum);*/
	  switch (fnum)
    {case UFEQUAL:	iarg1 = iarg1 == iarg2;
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
		otherwise	return "";
    }
    return ltos(iarg1);
  }
#undef arg
}


int uv_vnum;

const char *Pascal gtusr(char * vname)	/* look up a user var's value */
																				/* name of user variable to fetch */
{
	register int vnum;	/* ordinal number of user var */

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
	return errorm;
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
	register int l = 0;	  /* set current search limit as entire list */
	register int u = tlength - 1;
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


#if S_HPUX == 0 || 1
 const
#endif
       char *Pascal gtenv(const char * vname)
 			/* name of environment variable to retrieve */
{
	         int ix = 0;
	register int vnum;		/* ordinal number of var refrenced */
#define res vnum
 extern char deltaf[NSTRING];
#define result (&deltaf[NSTRING / 2])   /* leave beginning for extra safety */
 /* s tatic char result[NSTRING + 1];	** string result */

			  /* scan the list, looking for the referenced name */
	evmap.srch_key = vname;
	vnum = binary_const(&evmap, envars);
	
	if (vnum < 0) 		/* return errorm on a bad reference */
		return errorm;
	switch (vnum)
	{ case EVPAGELEN: res = term.t_nrowm1 + 1;
	  when EVCURCOL:  res = getccol();
	  when EVCURLINE: res = setcline();
	  when EVHARDTAB: res = curbp->b_tabsize;
	  when EVRAM:	    res = (int)(envram >> 10);
	  when EVCURWIDTH:res = term.t_ncol;
	  when EVCBFLAGS: res = curbp->b_flag;
	  case EVCMODE:   if (vnum == EVCMODE) res = res >> NUMFLAGS;
	  when EVCBUFNAME:return curbp->b_bname;
	  when EVCFNAME:  return fixnull(curbp->b_fname);
//  when EVSRES:	  return sres;
	  when EVPALETTE: return palstr;
	  when EVFILEPROF:return g_file_prof;
	  when EVLWIDTH:  
	  case EVCURCHAR: ix = llength(curwp->w_dotp);
                    res = vnum == EVLWIDTH    ? ix   :
                          ix == curwp->w_doto ? '\n' : lgetc(curwp->w_dotp, curwp->w_doto);
	  when EVWLINE:   res = curwp->w_ntrows;
	  when EVCWLINE:  res = getwpos();
	  when EVTARGET:  saveflag = lastflag;
			  					  res = curgoal;
	  when EVSEARCH:  return pat;
	  when EVHIGHLIGHT: return highlight;
	  when EVLASTDIR: res = lastdir;
	  when EVINCLD:	  return incldirs;
	  when EVTIME:	  return ""; /* timeset(); */
	  when EVREPLACE: return rpat;
	  when EVMATCH:   return fixnull(patmatch);
	  when EVKILL:	  return getkill();
	  when EVREGION:  return getreg(&result[0]);
	  
	  when EVPENDING:
#if	GOTTYPAH
			  return ltos(typahead());
#else
			  return falsem;
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
	  when EVWINTITLE:return curbp->b_fname;		// getconsoletitle();
#endif

	  when EVFLICKER: 
	  case EVDEBUG:   
	  case EVSTATUS:  
	  case EVDISCMD:  
	  case EVDISINP:  
	  case EVMODEFLAG:
	  case EVSSCROLL: 
	  case EVSSAVE:   
	  case EVHSCROLL: 
	  case EVDIAGFLAG:
	  case EVMSFLAG:  return ltos(predefvars[vnum]);

	  otherwise       ix = vnum;
										res = predefvars[vnum];
										loglog2("Var %d = %x", ix, res);
	}
	return int_asc(res);
#undef result
}

char *Pascal fixnull(char * s)/* Don't return NULL pointers! */
	
{
  return s == NULL ? "" : s;
}


char * Pascal trimstr(char * s, int * from)/* trim whitespace off string */
			/* string to trim */
{
	register char *sp = &s[*from];
        
	while (--sp >= s && *sp <= ' ')
	  *sp = 0;
	  
	*from = sp - s + 1;

	return s;
}



int Pascal findvar(char * var)/* find a variables type and name */
	
{
	register int vtype;	/* type to return */

fvar:	vtype = -1;
	switch (var[0])
	{ case '$':			/* check for legal enviromnent var */
		    evmap.srch_key = &var[1];
	      vtype = binary_const(&evmap, envars)|(TKENV << 11);

	  when '%':		  /* check for existing legal user variable */
	  	  if (gtusr(&var[1]) != errorm)
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
	      var[4] = 0;
		    if (strcmp(&var[1], "ind") == 0)
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
	{ ++discmd;
	  mlwrite("(%s <- %s)", var, value);
    --discmd;
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
  register int cc;
	char var[2*NSTRING+1];	/* name of variable to fetch */
	     var[0] = 0;
					/* first get the variable to set.. */
	if (g_clexec == FALSE)
	{ cc = getstring(&var[0], sizeof(var), TEXT51);
/*				 "Variable to set: " */
	  if (cc != TRUE)
	    return ABORT;
	  for (cc = -1; var[++cc] != 0 && var[cc] != ' ' && var[cc] != '='; )
	    ;
	} 
	else				/* grab token and skip it */
	{ execstr = token(execstr, var, NVSIZE + 1);
	  cc = strlen(var);
	}
	if (var[cc] == 0)
	  var[cc+1] = 0;
	else
	  var[cc] = 0;
					/* get the value for that variable */
	if 	(f == TRUE)
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



int Pascal svar(int var, char * value)	/* set a variable */

{
  register int cc = TRUE;
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
	    when EVHARDTAB:	 curbp->b_tabsize = val;
	                     upwind();
	    when EVPAGELEN:	 cc = newdims(term.t_ncol, val);
	    when EVCURWIDTH: cc = newdims(val, term.t_nrowm1+1);
	    when EVCURCOL:	 cc = setccol(val);
	    when EVCURLINE:	 cc = gotoline(TRUE, val);
	    when EVRAM:
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
	    when EVCBUFNAME: curwp->w_flag |= WFMODE;
				return FALSE; /* strpcpy(curbp->b_bname, value, NBUFN); */
	    when EVCFNAME:	 repl_bfname(curbp, value);
				               curwp->w_flag |= WFMODE;
//	  when EVSRES:	   cc = TTrez(value);
	    when EVFILEPROF: hookix = strlen(value)+1;
	    	               free(g_file_prof);
				               g_file_prof = malloc(hookix);
				               strcpy(g_file_prof,value);
				
	    when EVCURCHAR:	 ldelchrs(1L, FALSE);	/* delete 1 char */
			                 linsert(1, (char)val);
				               backchar(FALSE, 1);
											 updline(FALSE);
	    when EVWLINE:	   cc = resize(FALSE, val);
	    when EVCWLINE:	 cc = forwline(FALSE, val - getwpos());
	    when EVTARGET:	 curgoal = val;
				               thisflag = saveflag;
	    when EVSEARCH:	 mcclear();
	    			           strpcpy(pat,value,NPAT);
	    when EVREPLACE:	 strpcpy(rpat,value,NPAT);
	    when EVHIGHLIGHT:strpcpy(highlight,value,sizeof(highlight));
	    when EVLASTMESG: strpcpy(lastmesg,value,NSTRING);
	    when EVPALETTE:	 strpcpy(palstr,value,sizeof(palstr));
	    when EVPOPUP:    mbwrite(value);
				               upwind();
	    when EVKILL:
	    when EVLINE:	   putctext(value);
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
	    case EVFLICKER:	
	    case EVDEBUG:	
	    case EVSTATUS:	
	    case EVDISCMD:	
	    case EVDISINP:	
	    case EVSSCROLL:	
	    case EVSSAVE:	
	    case EVDIAGFLAG:	
	    case EVMSFLAG:  predefvars[vnum] = stol(value);

	    when EVMODEFLAG:predefvars[vnum] = stol(value);
	                    upwind();

	    when EVHJUMP:   if (val <= 0)
									      val = 1;
										  if (val > term.t_ncol - 1)
					              val = term.t_ncol - 1;
	    default:	      predefvars[vnum] = val;
    }
  }
  return cc;
}



int Pascal stol(char * val)	/* convert a string to a numeric logical */
{
        
  return val[0] == 'T' || atoi(val) != 0;	/* check for logical values */
	 
}


#define INTWIDTH (sizeof(int) * 3)


char *Pascal int_asc(int i)
			/* integer to translate to a string */
{
#if S_MSDOS && S_CYGWIN == 99
  static char result[INTWIDTH*2+2];
	memset(&result, ' ', INTWIDTH); 


 return itoa(i, &result[INTWIDTH], 10);
#else
  static char result[INTWIDTH+2];
	memset(&result, ' ', INTWIDTH); 

{	register char *sp = &result[INTWIDTH+1];
	register int v = i;		/* sign of resulting number */

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
#endif
}}

int Pascal gettyp(char * token)	/* find the type of a passed token */

{
	register char c = *token;	/* first char in token */
        
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
char *Pascal getval(char * tgt, char * token) 
				/* token: token to evaluate */
{
	int blen = NSTRING;
#define cc blen
 register BUFFER *bp;			/* temp buffer pointer */
          int typ = gettyp(token);

	switch (typ)
	{ case TKNUL:   return "";
	  case TKARG:			/* interactive argument */
						getval(&token[0], &token[1]);
		      { int sdc = discmd;	/* echo it always! */
						discmd = TRUE;
						cc = getstring(&tgt[0], NSTRING, token);
						discmd = sdc;
						if (cc == ABORT)
						  return getvalnull;
						return tgt;
		      }
	  case TKBUF:			/* buffer contents fetch */
										/* grab the right buffer */
						bp = bfind(getval(tgt, &token[1]), FALSE, 0);
						if (bp == NULL)
						  return getvalnull;
										/* if the buffer is displayed, get the window
							      	 vars instead of the buffer vars */
						if (bp->b_nwnd > 0)
						{ curbp->b_dotp = curwp->w_dotp;
						  curbp->b_doto = curwp->w_doto;
						}
					{ LINE * ln = bp->b_dotp;
							   		/* make sure we are not at the end */
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
	  when TKVAR:	bp = (BUFFER*)gtusr(token+1);
	  when TKENV:	bp = (BUFFER*)gtenv(token+1);
	  when TKFUN:	bp = (BUFFER*)gtfun(token+1);
	  when TKLIT:
	  case TKCMD:	bp = (BUFFER*)token;
	  when TKSTR: bp = (BUFFER*)(token + 1);
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
  return val ? truem : falsem;
}


int Pascal ernd()	/* returns a random integer */

{	seed = absv(seed * 1721 + 10007);
	return seed;
}

int Pascal sindex(char * source, char * pattern) /* find pattern within source */
	/* char *source;	* source string to search */
	/* char *pattern;	* string to look for */
{
	register char *sp;

	for (sp = source; *sp; ++sp)
	{ register int ix;
	  
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
	register char *sp;	/* pointer into source table */
	register char *lp;	/* pointer into lookup table */

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
  upmode();
  mlerase();					/* clear the mode line */
}

/*	describe-variables	Bring up a fake buffer and list the contents
				of all the environment variables
*/

Pascal desvars(int f, int n)

{ register int uindex = curbp->b_tabsize;   /* index into uvar table */
        
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
	register int cc;
	char var[NVSIZE+1];	/* name of variable to fetch */
	char val[NSTRING];

				/* first get the variable to display.. */
	if (g_clexec == FALSE)
  { cc = getstring(&var[0], NVSIZE+1, TEXT55);
/*				 "Variable to display: " */
	  if (cc != TRUE)
	    return cc;
	} 
	else		/* macro line argument */
	{		/* grab token and skip it */
	  execstr = token(execstr, var, NVSIZE + 1);
	}
					/* check the legality, find the var */
	
	if (findvar(var) < 0)
	{ mlwrite(TEXT52, var);
/*			"%%No such variable as '%s'" */
	  return FALSE;
	}
				        
	mlwrite("%s = %s", var, getval(val, var));
	update(TRUE);
	return TRUE;
}

#endif


/*	describe-functions	Bring up a fake buffer and list the
				names of all the functions
*/

Pascal desfunc(int f, int n)

{	register int uindex;	/* index into funcs table */

	openwindbuf(TEXT211);
						  /* build the function list */
	for (uindex = -1; ++uindex < NFUNCS; )
	{			     /* add in the environment variable name */
	  fmt_desv('&', funcs[uindex].f_name, "");
	}

  mkdes();
	return TRUE;
}


