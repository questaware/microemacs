/* -*- C -*- ****************************************************************
 *
 *  			Copyright 2003 <unknown>.
 *			      All Rights Reserved
 *
 *
 *  System        : Microemacs (jasspa version)
 *  Module        : sinc
 *  Object Name   : sinc.c
 *  Revision      : $Revision$
 *  Date          : 15th Nov 2003
 *  Author        : Peter Smith
 *  Created By    : Peter Smith
 *  Created       : Sat Nov 15 19:41:19 2003
 *  Last Modified : Sat Nov 15
 *
 *  Description	  Code to search back through include files
 *
 *  Notes
 *
 *  History
 *	
 *  $Log$
 *
 ****************************************************************************
 *
 *  Copyright (c) 2003 <unknown>.
 * 
 *  All Rights Reserved.
 * 
 * This  document  may  not, in  whole  or in  part, be  copied,  photocopied,
 * reproduced,  translated,  or  reduced to any  electronic  medium or machine
 * readable form without prior written consent from <unknown>.
 *
 ****************************************************************************/
                                                /* switch this manually */
#define AM_ME 0

extern "C"
{
						/* pick up MEOPT_SINC */
#if AM_ME
# include "emain.h"
#else
# include	<stdio.h>
# include	<stdlib.h>
# include	"estruct.h"
#endif

#if MEOPT_SINC

#if AM_ME
#include "logmsg.h"

#define Pascal
#undef NEAR
#define NEAR

#define when break; case
#define otherwise break; default:

#define false 0
#define true 1

#define null NULL
#define OK 0

typedef int Bool;

#define set_cand_lnno(v) cand_lnno = v

#define linetext text
#define wLine    dotLine
#define wOffset  dotOffset
#define linelength  length

#else

#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"logmsg.h"


#define meABORT ABORT
#define meFALSE FALSE
#define meTRUE  TRUE

#define b_baseLine b_baseline

#define meLineGetNext(lp) lforw(lp)
#define meLineGetPrev(lp) lback(lp)

#define set_cand_lnno(v)

#define linetext l_text
#define wLine    w_dotp
#define wOffset  w_doto
#define linelength  l_used

#define meMalloc(n) malloc(n)
#define meBuffer    BUFFER
#define meLine      LINE
#define backWord(a, b) backword(a, b)

#endif


#define CHQ_ESC 0x1b

int Pascal isword(char ch)
	
{ return isalpha(ch) || ch == '_' || ch == '$' || in_range(ch, '0', '9');
}


#if AM_ME


char * Pascal skipspaces(char * s, char * limstr)
	
	/* char *     limstr;			* the last char */
{          int lim = limstr - s;
  register int ix;
  register char ch;
  
  for (ix = -1; ++ix < lim && ((ch=s[ix]) == 'L'-'@' ||
                                 ch == ' ' || ch == '\t');)
    ;
    
  return &s[ix];
}


typedef struct Paren_s
{ 
  char          ch;
  char          fence;
  int           nest;
  int           nestclamped;
  char          in_mode;
  int           sdir;
  char          prev;
} Paren_t, *Paren;


static Paren_t paren;



int Pascal init_paren(const char * str, int len)

{ paren.nest = 1;
/*paren.nestclamped = 1;*/
  paren.sdir = 1;
  paren.in_mode = 0;
  paren.prev = 0;
  paren.ch = *str;
  
{ register char ch = paren.ch;
					     /* setup proper matching fence */
  switch (ch)
  {
#if 0
    case '(': ch = ')';
							break;
#endif
    case '[':
    case '{': ch += 2;
							break;
    case ']': 
    case '}': ch -= 2;
							break;
#if 0
    case '#': if    (len > 2 && str[1] == 'i' &&  str[2] == 'f')
			    			;
	    	      else if (len > 2 && str[1] == 'e' && (str[2] =='n' || str[2] =='l'))
				        paren.sdir = -1;
    		      else
              { paren.nest = 0;
                ch = 0;
              }
					    break;
    case ')': ch = '(';
					    break;
#endif
    default:  paren.nest = 0;
  }
  paren.fence = ch;
  if (ch < paren.ch)
    paren.sdir = -1;

  return paren.sdir;
}}

#define Q_IN_ESC   1
#define Q_IN_CHAR  2
#define Q_IN_STR   4
#define Q_IN_CMT0  8
#define Q_IN_CMTL 16
#define Q_IN_CMT  32
#define Q_IN_CMT_ 64
#define Q_IN_EOL 128


#if 0
   CMT0		after the first /
   CMT
   CMT_		On the final / of /* xxx */
   EOL		Comment to end of line
#endif


int Pascal scan_paren(char ch)
	
{
  register int mode = paren.in_mode;

  do
  { if ((mode & (Q_IN_STR + Q_IN_CHAR)) != 0)
    { if      (ch == '\n')
        mode = 0;
      else if ((mode & Q_IN_ESC) && paren.sdir > 0)
        mode &= ~Q_IN_ESC;
      else if (ch == '\\' && paren.sdir > 0)
        mode |= Q_IN_ESC;
      else if ((mode & Q_IN_STR) && ch == '"' ||
               (mode & Q_IN_CHAR) && ch == '\'') 
        mode = 0;
    }
    else
    { 
      if      ((mode & ~Q_IN_CMT_) == 0)
      { mode = ch;
        mode = mode == '\'' ? Q_IN_CHAR :
               mode == '"'  ? Q_IN_STR  :
               mode == '/'  ? Q_IN_CMT0 : 
               mode == '\\' && paren.sdir < 0 && (paren.prev == '\'' || paren.prev == '"')
                            ? paren.prev == '"' ? Q_IN_STR : Q_IN_CHAR : 0;
      }
      else if (mode & Q_IN_CMT)
      { if      (mode & Q_IN_EOL)
        { if (ch != '\n')
            break;
          mode = 0;
        }
        else if (ch == '/' && paren.prev == '*')
        { mode = Q_IN_CMT_;
          break;
        }
      }
      else /* if (mode & Q_IN_CMT0) */
      { mode = ch == '*'		    ? Q_IN_CMT	          :
	       ch == '/' && paren.sdir > 0 ? Q_IN_CMT + Q_IN_EOL : 0;
      }	      

      if (mode == 0)
      { 
        if (ch == paren.ch)
        { /*loglog2("INC %d %s", paren.nest, str);*/
          ++paren.nest;
          ++paren.nestclamped;
          if (paren.nestclamped <= 0)
            paren.nestclamped = 1;
        }
        if (ch == paren.fence)
        { /*loglog2("DEC %d %c", paren.nest, ch);*/
          --paren.nest;	/* srchdeffile needs relative and clamped nestings */
          --paren.nestclamped;
        }
      }
    } 
  } while (0);
  
/* if (mode & Q_IN_EOL)
     mode &= ~(Q_IN_CMT+Q_IN_EOL);
*/
  paren.prev = ch;
  paren.in_mode = mode;
  return mode;
}

			/* returns pointing to eol or
			 * the first / of / / */
int Pascal scan_for_sl(meLine * lp)

{ int cplim = lp->linelength;
  int ix;
  
  Paren_t sparen;
  sparen = paren;

  init_paren("{",0);
  for (ix = -1; ++ix < cplim; )
  { 
    if (!(scan_paren(lp->linetext[ix])
            & (Q_IN_STR+Q_IN_CHAR+Q_IN_CMT+Q_IN_CMT_))
        && lp->linetext[ix] =='/' && lp->linetext[ix+1] == '/')
      break;
  }

  paren = sparen;
  return ix;
}

#endif

					/* < 0 : not found */
static
int Pascal strxct_mtch(const char * const * t_, const char * s_, 
											 const char * slim, int enyct)
	
{ int ix = -1;
  for ( ; --enyct >= 0; )
  { const char * pat = t_[++ix];
    const char * src = s_;
    for (; src < slim && *pat == *src; ++pat)
      ++src;

    if (*pat == 0)
    { if (src >= slim || ! isword(*src))
        return ix;
      break;
    }
  }
  
  return -1;
}

static const char * Pascal find_nch(const char * cp, int cplen)
	/* int     cplen;			* the one after the last char */
{ 
  for ( ; --cplen >= 0; ++cp)
    if (! isword(*cp))
      return cp;

  return  "";
}


#define NO_BEST 1000


#define M_NOGO  (int)(~0xfff)	/* ~ -16,000 */

#define M_IN_SCT       1	/* IN STRUCT after LBRACE */ /* must be 1 */
#define M_IN_W         2	/* IN WORD */
#define M_IN_P         4	/* IN ) .. { */
#define M_IN_C	       8	/* in extern "C" */
#define M_AFTER_CC    16	/* after colon colon */
#define M_GOT_NM      32	/* GOT THE NAME */
#define M_AFTER_NM    64	/* immediately after THE name */
#define M_AFTER_NM0  128	/* immediately after 0 words nm  */
#define M_AFTER_RP  0x100	/* immediately after RPAREN */
#define M_PRE_SCT   0x200	/* after class/struct but before LBRACE */

#define M_NEED_B2   0x400	/* after RPAREN in NEED_BODY*/
#define M_NEED_BODY 0x800
#define M_IN_DCL   0x1000	/* in a b .... ; obsolete */
#define M_NO_BODY  0x2000	/* bodies are not legal */
#define M_IN_ENUM  0x4000	/* in enum ... */
#define M_BAN_LP   0x8000	/* left paren must not appear */

/* The state machine includes:

   fn           (             ,          )         int        a    ;

  NEED_LPRPX  ->     NEED_BODY             -> NEED_B2
*/

#define BEFORE_WINDOW_LEN 100

/* Append a line to a buffer not in use */

static int Pascal bufappline(LINE * base, int indent, const char * str)

{          int    sz = strlen(str);
  register LINE * inslp = mk_line(null, indent+sz, indent+sz);
   
  if (inslp == NULL)
    return FALSE;

  memset(&inslp->l_text[0], ' ', indent);
  memcpy(&inslp->l_text[indent], str, sz);
  ibefore(base, inslp);
  return TRUE;
}

}

class Sinc
{
	private:
		static int sparen_nest;
		static int sparen_nc;		/* nest clamped */

		static int prev_block;		/* index into ops */

		static const char * const ops[];

		/*static Short srchdpth = 0;*/
	public:
		static char * namelist;

		static meBuffer * g_outbuffer;		/* buffer accumulating includes */
		static short good_class;		/* nesting of valid T C::p() */
		static short from_class;		/* searching from class member */
		static short from_fld;			/* searching from field */
		static short ccontext; 			/* measures paren nesting p1(){ */

		static short ask_type;
		static short srch_exact;	/* <= 0 => true */
		static short min_paren_nest;

		static short best_nest;			/* the most unnested match */
		static meBuffer * best_bp;
                                            /* 0 or (+ve) type found */
    static int doit_forward(meLine * lp, char * cp_, meLine * basel = NULL);

		static int srchdeffile(const char * fname, char * str, int depth);
};

int Sinc::sparen_nest;
int Sinc::sparen_nc;

int Sinc::prev_block;

char * Sinc::namelist;
meBuffer * Sinc::g_outbuffer;
short Sinc::good_class;
short Sinc::from_class;
short Sinc::from_fld;
short Sinc::ccontext;

short Sinc::ask_type;
short Sinc::srch_exact;
short Sinc::min_paren_nest;

short Sinc::best_nest;
meBuffer * Sinc::best_bp;


const char * const Sinc::ops[] = 
		 	{ "class","struct","union","enum", "\"C\"", /* 5 */
		  	"static","public","private","protected", /* 9 */
		   	"and","or","not",
		   	"return","then","else" };

int Sinc::doit_forward(meLine * lp, char * cp_, meLine * basel)

{ int  word_ct = M_NOGO;
 register
  int ch;
  prev_block = -1;

#define ix ch

  for (ix = BEFORE_WINDOW_LEN+1; --ix > 0; )
  {
#if AM_ME
    if (lp == basel)
#else
    if (lp->l_props & L_IS_HD)
#endif
    { ix = 0;
      word_ct = ix;
      break;
    } 
    lp = meLineGetPrev(lp);
  }
{ int  rp_line = -1;		/* line containing a () */
  char last_ch = ix;
  int  in_esc = ix;		/* line after \ */
  int  paren_dpth = ix;
  int  brace_dpth = ix;		/* relevant only to structures */
  int  paren_nm = ix;
  int  brace_nm;
  int  tok_ct = ix;
  int  last_paren = ix;
  int  lines_left;
  int  obrace_dpth = paren.nestclamped;
  init_paren("[",0);
  paren.nest = ix;
{ int save_a_cc = 0;
  register
   int state = ix;
#undef ix

  for (lines_left = 2 * BEFORE_WINDOW_LEN; --lines_left >= 0; )
  { lp = meLineGetNext(lp);

#if AM_ME
    if (lp == basel)
#else
    if (lp->l_props & L_IS_HD)
#endif
      break;
      
  { int following = false;
    int directive = false;

    char * cpstt = &lp->linetext[0];
    char * cplim = cpstt+lp->linelength;	/* exclusive */
    const char * cp;
    loglog3("%d %d %s", paren_dpth, lp->linelength, cpstt);
    if (in_esc)
    { if (cplim[-1] != '\\')
        in_esc = false;
      continue;
    }

    for (cp = cpstt-1; ++cp <= cplim; )
    { ch = cp == cplim ? '\n' : *cp;
      
      if (! following)
      { if (isspace(ch)) 
          continue;
        following = true;
        if (ch == '#')
        { directive = true;
          if (cplim[-1] == '\\')
            in_esc = true;
          /* word_ct = 0; state = 0; */
        }
      }

      if      (scan_paren((char)ch))
      { state &= ~M_IN_W;
        if (ch == '"' && cp - 2 >= cpstt && cp[-1] == 'C' || cp[-2] == '"')
          ch = 'A';
        else 
        { if (ch != '"' && ch != '\'')
          { ch = ' ';
            state &= ~M_IN_W;
            continue;
          }
        }
      }
      else if (paren.nest == 0 && ! directive)
      { if (paren_dpth < 2 && isword((char)ch))
        {
          if ((state & M_IN_W) == 0)
          { 
            ch = strxct_mtch(&ops[0], cp, cplim, upper_index(ops)+1);
            if (ch >= 0)
				    { if      (ch > 9)
				        word_ct = M_NOGO;
				      else if (ch > 4)		/* public or private */
				      { cp += strlen(ops[ch])-1;/* ignore following ch */
				        /*word_ct = 0;*/
				        continue;
	    			  }
	      			else			/*class, union, struct, enum, or "C"*/
				      { prev_block = ch;
	    		    /*if ((state & M_IN_SCT) == 0)*/
	        		  state |= M_PRE_SCT;
	        			if (ch == 3)
				          state |= M_IN_ENUM;
				        if (ch == 4)
	    			      state |= M_IN_C;
	      			}
	    			}
            state |= M_IN_W;
            if ((state & M_AFTER_NM))
				      if (paren_dpth==0 && brace_dpth <= brace_nm)
	    			  { loglog3("R00 %d %d %d", paren_dpth, brace_dpth, brace_nm);
                return 0;		/* a tgt   b; ignored */ 
								/* a tgt ()b; allowed */
			        }
      			if (word_ct > 0)
            	state |= M_IN_DCL;
    				if ((state & M_AFTER_RP) && (state & M_GOT_NM))
				      state |= M_BAN_LP;
       	    word_ct += 1;
         	  if      (cp == cp_)
           	{ state |= M_GOT_NM | M_AFTER_NM;
             	brace_nm = brace_dpth;
             	paren_nm = paren_dpth;
      	    /*char bufff[60];
      	      sprintf(bufff, "w_ct %d spn %d state %x rpl %d ll %d", 
      	    	word_ct, sparen_nest, state, rp_line, lines_left);
      	      mbwrite(bufff);*/
						
      	      loglog4("state %x rp_line %d lines_left %d wc %d", state, rp_line, lines_left, word_ct);
             
      				if (sparen_nest == -1 && (state & M_IN_P)	||
          				word_ct < 0		 /* set back */ ||
	          		  rp_line == lines_left  /* a (x)..tgt; ignored */)
      	      { loglog("Arft RP");
								return 0;
      				}
	      			if (paren_dpth > 0 && (from_class || from_fld || good_class)) /* p (^here) */
	      			{ loglog1("pdrej %20.20s", cp);
			        	return 0;
    			  	}
        
            	if ((state & M_PRE_SCT) == 0 &&
                   word_ct < 0) /* or (word_ct & 0x80) */
      				{ /*paren_dpth = 0;*/
              	state |= M_AFTER_NM0;
            	}

      			  continue;
            }
          }
        	state &= ~(M_AFTER_NM0+M_AFTER_RP);
        }
        else					/* not a letter */
        { const char * t;
          state &= ~M_IN_W;
          if (isspace(ch))
            continue;
          tok_ct += 1;
          if (ch == ']' || ch == '*' || ch == '&')
            continue;
          else if (ch == '<' && *(t = find_nch(cp+1, cplim-cp-1)) == '>')
          { cp = t;
            state |= M_IN_W;
          }
          else if (ch == '(')
				  { save_a_cc |= state;
	    			if (state & M_BAN_LP)
	    			{ loglog("BAN");
				      return 0;
	    			}
	    			paren_dpth += 1;
            loglog2("PINC %d %s", paren_dpth, cpstt);
            if (paren_dpth < 2 &&
               (state & M_AFTER_NM))
            { 
              if (word_ct > 1 && paren_nm == 0)
              { if (!(state & M_AFTER_CC) || (from_fld+from_class+good_class))
       	        {/*char bufff[50];
       	          sprintf(bufff, "word_ct %d", word_ct);
       	          mbwrite(bufff);*/
                  return 6;
                }
              }
				      if ((state & M_IN_SCT)==0 && paren_dpth==1)
                state |= M_NEED_BODY;
            }
            if (word_ct > 0)
              word_ct = 0;
          }
          else if (ch == ',' || ch == ')' /* rparen */) 
				  { if (ch == ')')
	      			paren_dpth -= 1;

				    if (state & M_AFTER_NM0)
	    			{ loglog("ANM0");
				      return 0;
	    			}
	    		  if (paren_dpth!=0  && obrace_dpth < 0 &&
	        			sparen_nc != 0 &&
	       			 (state & (M_GOT_NM+M_IN_DCL+M_NO_BODY)) == (M_GOT_NM+M_IN_DCL))
	      			state |= M_NEED_BODY; 
				    word_ct = 0;
	    			if (ch == ',')
				    { if ((state & (M_GOT_NM+M_NEED_BODY+M_PRE_SCT))==M_GOT_NM &&
	          		  (paren_dpth == 0) && (paren_nm == 0))
								return 11;
				      if ((state & M_IN_ENUM) && (state & M_AFTER_NM))
	        			return 12;

	      			if      ((state & M_IN_DCL) && paren_dpth == 0 || 
	            			   (state & M_IN_ENUM))
								word_ct = 1; 
				      else if (paren_dpth != 1 || word_ct <= -127)
								word_ct = M_NOGO;
	    			}
				    else
	    			{ state |= (save_a_cc & M_AFTER_CC);
              loglog2("DINC %d %s", paren_dpth, cpstt);
				      if (paren_dpth == 0)
	    			  { save_a_cc = 0;
				        state &= ~(M_IN_W+M_AFTER_NM+M_AFTER_NM0);
								state |= M_IN_P + M_AFTER_RP;
	        			if (word_ct < 2)
	        			{ loglog("unimpeed");
	          			state &= ~M_IN_P;
	        			}
								if (state & M_NEED_BODY)
								{ rp_line = lines_left;
								  state |= M_NEED_B2;
	          			last_paren = tok_ct + 1;
								}
								continue;
	      			}
	      			if (paren_dpth < 0)
	        			paren_dpth = 0;
	    			}
	  			}
	  			else if (ch == ';' || ch == '}'/* rbrace */)
	  			{ if (state & M_GOT_NM)
	    			{/*char buf[120];
	      			sprintf(buf, "sparen_nc %d state %x paren_nm %d", sparen_nc, state, paren_nm);
	      			mbwrite(buf);*/
	      			if (sparen_nc < 0 || !(state & (M_NEED_BODY+M_PRE_SCT))&&!paren_nm)
	      			{ int res = word_ct > 1 ? 7 : 0;
	        			loglog2("Res %d : %20.20s", res, cp);
	        			return res;
	      			}
				      if ((state & M_AFTER_RP) || ch == ';')
	    			  { loglog("ARP");
								return 0;
				      }
	    			}
				    paren_dpth = 0; 
	    			state &= ~(M_IN_DCL+M_IN_ENUM);
				    if	    (ch == '}')
	    			{ word_ct = (state & M_IN_SCT);
				      brace_dpth -= 1;
				      state &= ~(M_NO_BODY+M_PRE_SCT);
	    			  if (brace_dpth <= 0)
				      { state &= ~(M_IN_P+M_IN_SCT);
								brace_dpth = 0;
								prev_block = -1;
				      }
	    			}
				    else 
	    			{ if (word_ct == 0)
								state &= ~M_PRE_SCT;
				      if (word_ct < 2)
								state &= ~M_IN_P;
				      word_ct = 0;
	    			}
	  			}
				  else if (ch == '{')
				  { if (state & M_GOT_NM)
				    { loglog4("S %x ff %d fc %d gc %d", state, from_fld, from_class, good_class);
				      return (state & (M_NEED_BODY+M_PRE_SCT)) && paren_nm <= 1 && 
				             (!(state & M_AFTER_CC) ||
				               (from_fld+from_class)|| good_class ||(state & M_NEED_BODY))
				                        ? 5 : 0;
				    }
				    if (last_paren == tok_ct)
				      return 13;
				    if (state & M_PRE_SCT)
				      state |= M_IN_SCT;
				    word_ct = 0;
				    paren_dpth = 0; 
				    brace_dpth += 1;
				    state &= ~(M_IN_P+M_PRE_SCT+M_NEED_BODY);
				  }
				  else if (paren_dpth >= 2)
				    ;
				  else if (ch == cp[1] && cplim - cp > 1)	/* repeating char */
				  { ++cp;
				    if (ch == ':')
				    { word_ct -= 1;
				      state |= M_AFTER_CC;
				      continue;
				    }
				    word_ct = M_NOGO;
				    state |= M_NO_BODY;
				    state &= ~M_NEED_BODY;
				  }
				  else if (ch == ':')
				  { if (state & M_PRE_SCT)
				    { if (state & M_AFTER_NM && paren_nm == 0)
				        return 4;
				    }
				    word_ct = M_NOGO;
			            if (state & M_IN_SCT)
			              word_ct = 0;
				    continue;
				  }
				  else if (ch == '=')
				  { if (state & M_AFTER_NM)
				    { if (state & M_IN_ENUM)
				        return 10;
				      if (word_ct > 1)
				      { if (paren_dpth == 0)
				          if (sparen_nc < 0 || !(state & (M_NEED_BODY+M_PRE_SCT))&&!paren_nm)
				            return 7;
			          if ((from_fld+from_class+good_class) || !(state & M_AFTER_CC))
				        { if (state & M_IN_SCT)
				            return 8;
				          if ((state & M_NEED_BODY) == 0 &&
						          (paren_dpth == 0 || sparen_nc < 0))
								    return 9;
					      }
				      }
				      loglog("C =");
					    return 0;
					  }
				    word_ct = -127;
					}
					else
					{ word_ct = M_NOGO;
					  if (state & M_AFTER_NM)
					  { loglog("ANM");
				      return 0;
				   	}
				  }			/* transient states :*/
          state &= ~(M_AFTER_NM+M_AFTER_NM0+M_AFTER_RP+M_AFTER_CC/*+M_PRE_SCT*/);
        }
      }
    } /* loop */
    following = 10000;
    while (--following >= 0 && lp->linetext[lp->linelength-1] == CHQ_ESC)
      lp = meLineGetNext(lp);
  }}

  loglog("Ret 0");
  return 0;
}}} /* doit_forward */

#define SLEN 133
#if SLEN < NFILEN + 35
# undef SLEN
# define SLEN NFILEN+35
#endif

#define Q_NEED_RP 80
#define Q_GOT_LP  40

				 /* result: 0 => not found, -1 => aborted, else
					    type of occurrence */
int Sinc::srchdeffile(const char * fname, char * str, int depth)
  	
{
 static const char * dinc[] = { "define", "include", };
 static char incldr[] = "$incldirs";

  Paren_t sparen = paren;
  char lbuf[SLEN+2];
  char mybuf[SLEN];
  int rcc;
  char * cp;
  Bool made_buf = false;
  meBuffer * bp;
#if AM_ME
  meWindow * curwp = frameCur->windowCur;
  meBuffer * sbp = frameCur->bufferCur;
#else
  meBuffer * sbp = curbp;
#endif
#if MEOPT_TYPEAH
  if (TTahead())
    return 0;
#endif
#if GOTTYPAH
  if (typahead())
    return 0;
#endif
  
  for (cp = namelist; cp != null; cp = *(char**)cp)
  { if (strcmp(fname, &cp[sizeof(char*)]) == 0)
      return 0;
  }    

#if AM_ME
  bp = fname[0] == 0 ? sbp : bfind(fname, 0);

  while (bp == NULL)		      /* yuk, newfile writes curbp ! */
  { char * ffile;
    cp = getval(incldr);
    if (cp == NULL || *cp == 0)
    { cp = meGetenv("INCLUDE");
      if (cp == NULL)
        cp = "";
        
      setVar(incldr, cp, NULL);
    }

    ffile = fLookup(&lbuf[0], fname, NULL, 
			    					meFL_CHECKDOT | meFL_EXEC,
			    					sbp->fileName, cp);
    if (ffile != NULL)
    { loglog1("fLookup %s", cp);
      bp = bfind(cp, BFND_CREAT);
      bp->fileName = meStrdup(ffile) ;
      bp->intFlag |= BIFFILE ;
    }

    if (bp != NULL)
      made_buf = true;
    else
    { if (cp == NULL)
			  cp = "";
      rcc = strlen(cp);
      if (rcc >= SLEN-6 || ask_type != 0)
        return 0;
      strcpy(lbuf,fname);
      strcat(lbuf," ? extra INCLS path || CR:");
      if (meGetString(lbuf,MLSEARCH,0,&lbuf[rcc+1],SLEN-3-rcc) != meTRUE ||
          lbuf[rcc+1] == 0
         )
				return 0;
      strcpy(&lbuf[0], cp)[rcc] = PATHCHR;
      loglog1("Set InclDirs %s", lbuf);
      setVar(incldr, lbuf, NULL);
    }
  }

  swbuffer(curwp, bp);
  
  if (bp->intFlag & BIFSRCH)
    return 0;
#else
  bp = fname[0] == 0 ? curbp : gotfile(fname);

  loglog3("WHILE SBP %x BP %x CBP %x", sbp, bp, curbp);

  while (bp == NULL)		      /* yuk, newfile writes curbp ! */
  { made_buf = true;
  {	const char * fcp = (char*)flook('I', fname);
  
    if (fcp == NULL)
      loglog1("flook failed %s", fname);
    else
    { bp = bufflink(fcp, TRUE);
      loglog3("SBP %x BP %x CBP %s", sbp, bp, curbp);
    }

    if (bp == NULL)
    { const char * ids = gtenv(incldr+1);
      rcc = strlen(ids);
      if (rcc >= SLEN-6 || ask_type != 0 ||
					mlreply(concat(&lbuf[0], fname," ? extra INCLS path || CR:",null),
									&lbuf[rcc+1], SLEN-3-rcc) != TRUE)
				return 0;
      strcpy(&lbuf[0], ids)[rcc] = PATHCHR;
      set_var(incldr, lbuf);
    }
  }}

  swbuffer(bp);
  paren = sparen;
  loglog3("sbp %x bp %x cbp %x", sbp, bp, curbp);
 
  if (bp->b_flag & BSRCH)
    return 0;
#endif

  if (ask_type == '1')
  { /*setcline();*/
    return 1;
  }

{ int cand_doto;
  meLine * cand_lp = NULL;
  meLine * prev;
  char ch;
  int good_fld = false;
  char gs_buf [NSTRING];

#if AM_ME  
  int cand_lnno;  
  meUShort save_o = curwp->dotOffset;
  meLine * save_l = curwp->dotLine;
        
  meLine * lp = meLineGetPrev(bp->baseLine);
  int lnno = bp->lineCount;
  int cplen = lp->length;
  if (fname[0] == 0)
  { lp = save_l;
    cplen = save_o;
    lnno = bp->dotLineNo;
  }
    
  bp->intFlag |= BIFSRCH;

  for (rcc = 0; best_nest > 0 && lp != bp->baseLine;
                lp = prev, cplen = prev->length)
#else
  Lpos_t save = *(Lpos_t*)&curwp->w_dotp;
  LINE * lp = lback(curbp->b_baseline);
  int cplen = lp->l_used;

  loglog3("cwp %x lp %x ? %x", curwp, save.curline, 0);
  if (fname[0] == 0)
  { lp = save.curline;
    cplen = save.curoff;
  }
    
  bp->b_flag |= BSRCH;

  for (rcc = 0; best_nest > 0 && (lp->l_props & L_IS_HD) == 0;
                lp = prev, cplen = prev->l_used)
#endif
  { prev = meLineGetPrev(lp);
      
#if AM_ME  
    --lnno;
#endif
    if (/*prev->length > 0 && */ prev->linetext[prev->linelength-1] == CHQ_ESC)
      continue;

  { int is_directive = false;
    char * cpstt = &lp->linetext[0];
    char * cplim = &cpstt[cplen - 1];
    cp = skipspaces(&cpstt[0], cplim);
	  
    if (cplen == 0)
      continue;

    if (cplen > SLEN)
      cplen = SLEN;

    if (cp[0] == '#')
    { cp = skipspaces(cp+1, cplim);

      if (strxct_mtch(dinc, cp, cplim + 1, 2) >= 0 &&
          (paren.in_mode & (Q_IN_STR+Q_IN_CHAR+Q_IN_CMT+Q_IN_EOL)) == 0)
      { ch = *cp;
        cp = skipspaces(&cp[7], cplim);
    
        if (ch == 'd')
        { if (strxct_mtch(&str, cp, cplim + 1, 1) >= 0 && g_outbuffer == NULL)
				  { cand_doto = cp - &lp->linetext[0];
				    cand_lp = lp;
				    set_cand_lnno(lnno);  
				    rcc = 100;
				    best_nest = -1;
				  }
        }
        else if (rcc == 0)
        { char * fn = (char*)meMalloc(strlen(fname)+sizeof(char *)+2);
				  if (fn != null)
				  { strcpy(&fn[sizeof(char *)], fname);
				    *(char**)fn = namelist;
				    namelist = fn;
				  }

/*#define fnb ((char*)gs_buf)*/
#define fnb mybuf
				  cp = strpcpy(&fnb[0], cp+1, 128);
				  
				  while ((ch = *++cp) != '"' && ch != '>' && ch != 0)
				    ;
				  
				  ch = 0;
				  *cp = ch;
				  if (ask_type == 0)
				  {
/*			    strcpy(&b[0], "Search at (N)");
				    b[11] = '1' + depth;
				    if (paren.in_mode != 0)
				      strcat(&b[0], "NP!");
*/
#if AM_ME  
#define b ((char*)&gs_buf[0])
            sprintf(&b[0], "%s SP,CR,N,1,^G", &fnb[0]);
            ch = meGetString(b,MLSEARCH,0,b,80);
#else
#define b ((char*)&gs_buf[128])
				    ch = getstring(b, 80, concat(&b[0], &fnb[0], " SP,CR,N,1,^G", null));
#endif              
				    if (ch == meTRUE)
				    { if (b[0] == ' ' || b[0] == '1')
				      { ch = meFALSE;
								ask_type = b[0];
	    			  }
	    			  if (b[0] == 0)
	    			    ch = meFALSE;
				    }
				  }

				  if	    (ch == meABORT)
				  { rcc = -1;
				    best_nest = -1;
				  }
				  else if (ch == FALSE)
				  {/* char buff[80];
				    char spaces[] = "                    ";
				    concat(&buff[0], &spaces[20-srchdpth], "SI ", &fnb[0], null);
				    logstr(buff); */
#if AM_ME
				    curwp->dotLine = save_l;
				    curwp->dotOffset = save_o;
#else
				    rest_l_offs(&save);
#endif
				  /*++srchdpth;*/
			      loglog2("srchdeffile %d %s", paren.nest, fnb);
				    rcc = srchdeffile(&fnb[0], str, depth+1);
            loglog3("SRCHDEFFILE %d %d %s", paren.nest, rcc, fnb);
            if (g_outbuffer != NULL)
				      (void)bufappline(g_outbuffer->b_baseline->l_fp, depth * 3, fnb);

			          /*--srchdpth;*/
				    if (rcc > 0)
				      cand_lp = NULL;
				  }
        }
      }
      is_directive = true;
    }
				      /* deal with double slash this ' . */
    for (cp = &cpstt[scan_for_sl(lp)]; --cp >= cpstt && best_nest > 0; )
    { ch = *cp;
      if (paren.in_mode || is_directive)
scan:   
        scan_paren(ch);
      else        
      { switch (ch)
        { case ' ':
          case '\t':
	  			case '=':
				  case ',':
          when '}': 
          case '/': case '\\': 
          case '"': case '\'': goto scan;

				  case ';': ccontext = 0;
						  		/*if (best_nest != NO_BEST)
          					  loglog1("CBRACE %d", paren.nest);*/
          when ')': if (ccontext >= Q_NEED_RP)
						          ccontext += 1;
      
          when '(': if (ccontext >= Q_NEED_RP)
											ccontext -= 1;
				  when ':': if (ccontext == Q_NEED_RP)
						  	    { if (cp > cpstt && cp[-1]==':')
	  	    					  { 
						  	        if      (good_class > 0 ?  good_class < paren.nest
	  	    														    			: -good_class < paren.nest)
	  	          					good_class = paren.nest;
						  	        else if (good_class = paren.nest)
						  	          good_class = -paren.nest;
	  	    					  //mbwrite("[from class]");
	  	      					}
	  	    					}
          when '{':/*if (best_nest != NO_BEST) 
                      loglog1("OBRACE %d", paren.nest);*/
            				scan_paren(ch);
      		  			/*if (paren.nest < 0)
                      paren.nest = 0;*/
               				      /* structure definitions */
							    	if (paren.nest < min_paren_nest)
			    					{ min_paren_nest = paren.nest;
								      ccontext = Q_NEED_RP;
			    					}
																	    					/* is the candidate good? */
								    if (paren.nest < sparen_nest &&
								      /*paren.nest >= 0          && */best_nest != NO_BEST) 
		    						{ int ct;										/* look back for class or struct */
								      curwp->wLine = lp;
								      curwp->wOffset = cp - cpstt;
							 	      loglog3("TRYDEC %d %d %d", best_nest, paren.nest, rcc);

								      for (ct = 40; --ct > 0 && backWord(0, 1); )
								      { int wh;
								        char * wd = (char *)&curwp->wLine->linetext[curwp->wOffset];
								      /*if      (wd[0] == '(')
												  rcc = -1;*/
											      /*else if (wd[0] == ':')
												  rcc = 2;
												else */ if (wd[0] == ';' || wd[0] == '}')
											  { ct = 0;
												  break;
												}
								        wh = strxct_mtch(ops, wd,
													 							 &wd[curwp->wLine->linelength - curwp->wOffset],
										 										 upper_index(ops)+1);
								        if (!in_range(wh, 0, 4) )
												  continue;

												ct = 0;			/* end of looping */
								        if      (wh == 4)	/* "C" */
												  ;
												else if (wh == 3)	/* enum */
												{ if (from_fld)
												    break;
												} 
												else if (from_fld+from_class+good_class)
												{ //mbwrite("CSU");
												  good_fld = true;
												}
											      /*else if (wh == 2)
												  break;*/
												else
												/* if (!from_class) */
												    break;
												best_nest -= 1;		/* "C", enum, class unnest */
												loglog4("dec bn to %d, %d %d %s", best_nest, wh, rcc, wd);
												ct = 1;
												break;
							  	    }
							  	    if (ct == 0)
								      { loglog1("lost best %s", cp);
						//		      mbwrite("Lost Best");
												best_nest = NO_BEST;
												/*best_bp = NULL;*/
												cand_lp = NULL;
												rcc = 0;
								      }
								      if (rcc < 0)
												rcc = 0;
								    }

				  otherwise if (ch == *str &&
                        cp == cpstt || ! isword(cp[-1]))
      	            { if      (strxct_mtch(&str, cp, cplim + 1, 1) < 0)
				                 ; 		    /* unique code point for match */
						           /* word matched */
								      else if (g_outbuffer != NULL)
		  					       ;
						   	      else if (srch_exact > 0 /* && fname[0] != 0 */)
		      						{
#if AM_ME
								        strncpy(&lbuf[0], cpstt, cplen+2);
								        ch = meGetString(lbuf,MLSEARCH,0,lbuf,80);
#else
				                ch = getstring(&lbuf[0], 80, strpcpy(&lbuf[0], cpstt, cplen+1));
#endif
								        if      (ch == meABORT)
												  rcc = -1;
											  else if (ch == meFALSE || lbuf[0] == 0)
											    continue;
											  else
												{ cand_doto = cp - cpstt; 
												  cand_lp = lp;
											    set_cand_lnno(lnno);
												  rcc = 100;
												  best_nest = 0;
												}
											}
											else if (/*paren.nestclamped<=2 &&*/ best_nest==NO_BEST)
											{ sparen_nc = paren.nestclamped;
											  sparen_nest = paren.nest;
											  loglog2("TRY %d %s", sparen_nest, cpstt);
											  loglog2("Try %d %s", sparen_nc, cp);
//											sprintf(buf, "Try %d %d %50.50s", sparen_nest, sparen_nc, cp);
//											mbwrite(buf);
											  ch = doit_forward(lp, cp,bp->b_baseLine);
								        if (ch != 0)
		        						{
#if 0
											    sparen_nc >= 0 && ((state & M_IN_P)|| paren_dpth>0)||
										    /*sparen_nc == 0 && (state & M_AFTER_RP)        or*/
											    sparen_nc > 0  && ! (state & M_IN_SCT)      ||
#endif
											    cand_doto = cp - cpstt; 
												  cand_lp = lp;
										      set_cand_lnno(lnno);  
											    rcc = ch;
			   									best_nest = sparen_nc;
			   
													if (best_nest == 0 && from_fld+from_class)
												  {//mbwrite("Equal nest ff");
												    ++best_nest;
			   									}
											  	loglog3("Found %d bn %d %s", rcc, best_nest, cpstt);
												{ char * tp; 
//			   								char buff[50]; 
												  for (tp = cp; --tp > cpstt+1 && isspace(*tp); )
												    ;
//			   								sprintf(buff, "BN %d %20.20s", best_nest, tp); mbwrite(buff);
											  	if (tp[0] == ':' && tp[-1] == ':')
												    good_fld = true;
		        						}}
		        						init_paren("}",0);
	      								paren.nest = sparen_nest;
												paren.nestclamped = sparen_nc;
								      }
		    						}
				            goto scan;
				}
      }  
    } /* for (over the line) */
    scan_paren('\n');
    if (cand_lp != NULL && from_fld+from_class && !(good_fld+good_class) && best_nest != 1 && rcc != 100)
    { cand_lp = NULL;
      rcc = 0;
      best_nest = NO_BEST;
    }
  }} /* while "lines" */

#if AM_ME
  bp->intFlag &= ~BIFSRCH;
 
  if (cand_lp != NULL && best_bp == NULL /* && best_nest <= 0)*/)
  { loglog2("SETTING doto %d bn %d", cand_doto, best_nest);
    best_bp = bp;
    curwp->dotLine = cand_lp;
    curwp->dotOffset = cand_doto;
    curwp->dotLineNo = cand_lnno ;

    curwp->updateFlags |= WFMOVEC | WFMOVEL;
  }
  else
  { curwp->dotLine = save_l;
    curwp->dotOffset = save_o;
  }
  
  if (bp != sbp)
  { swbuffer(curwp, sbp);
    if (made_buf && best_bp != bp)
      zotbuf(bp, 1);
  }
#else
  bp->b_flag &= ~BSRCH;
 
  if (cand_lp != NULL && best_bp == NULL /* && best_nest <= 0)*/)
  { loglog2("SETTING doto %d bn %d", cand_doto, best_nest);
    best_bp = bp;
    curwp->w_dotp = cand_lp;
    curwp->w_doto = cand_doto;
    curwp->w_flag |= WFMOVE;
    setcline();
  }
  else
  { rest_l_offs(&save);
    loglog3("Cwp %x lp %x ? %x", curwp, curwp->w_dotp, 0);
  }
  
  loglog3("Sbp %x bp %x cbp %x", sbp, bp, curbp);

  if (bp != sbp)
  { loglog("SWB");
    swbuffer(sbp);
    if (made_buf && best_bp != bp)
    { loglog("ZB");
      zotbuf(bp);
    }
  }
#endif
  return rcc;
}}

extern "C"
{
/*
 * searchIncls -- Search back including the the include files for 
 *	a C definition of the word under the cursor.
 *	If found, reset the "." to be one the definition.
 *	An argument causes the search to stop at all matching
 *	words found in include files.
 */
int Pascal searchIncls(int f, int n)

{
  static char nfnd[] = "[Not Found] %s";

  char  tgt[92];
#if AM_ME
  meWindow * curwp = frameCur->windowCur;
  meLine * lp = curwp->dotLine;
  int g_nosharebuffs;
#else
  LINE * lp = curwp->w_dotp;
#endif
  int slpu = lp->linelength;
  register int ix = curwp->wOffset;

  if (Sinc::g_outbuffer != NULL)
  { tgt[0] = '?';
    tgt[1] = 0;
  }
  else
  { strpcpy(&tgt[0], &lp->linetext[ix], sizeof(tgt)-1);

    for (; ++ix <= slpu && isword(lp->linetext[ix-1]);)
      ;

    ix -= curwp->wOffset;
    ix -= 1;
    if (ix <= 0)
      return OK;

    if (ix < sizeof(tgt)-1)
      tgt[ix] = 0;

    ix = curwp->wOffset;
    while (--ix > 0 && isspace(lp->linetext[ix]))
      ;

    Sinc::good_class = 0;
    Sinc::
     from_fld = ix > 0 && lp->linetext[ix-1] == '-' && lp->linetext[ix] == '>' ||
		  	        ix >=0 && lp->linetext[ix] == '.';
		Sinc::
     from_class = 
               ix > 0 && lp->linetext[ix-1] == ':' && lp->linetext[ix] == ':';
  }
  Sinc::srch_exact = n - 1;
  init_paren("}",0);
  ix = 0;
  paren.nest = ix;
  paren.nestclamped = ix;
  Sinc::ask_type = ix;
  Sinc::ccontext = ix;
  Sinc::min_paren_nest = ix;
  Sinc::best_nest = NO_BEST;
  Sinc::best_bp = NULL;

  /* namelist = null;*/
  /* srchdpth = 0;*/
  lp->linelength = curwp->wOffset;	/*we dont need an Me 0 terminator here*/
  g_nosharebuffs = true;

  int cc = Sinc::srchdeffile("", &tgt[0], 0);

  if (Sinc::g_outbuffer == NULL)

  { if (cc > 0)
 #if AM_ME
    { mlwrite(MWABORT, "Found type %d", cc);
      if (Sinc::best_bp != NULL)
        swbuffer(frameCur->windowCur, Sinc::best_bp);
    }
    else
    { /*mbwrite("[Not Found]");*/
      mlwrite(MWABORT, 
  #if MEOPT_TYPEAH
      	      TTahead() ? "Interrupted" : 
  #endif
    				          nfnd, tgt);
    }
 #else
    { mlwrite("Found typ %d", cc);
      if (Sinc::best_bp != NULL)
        swbuffer(Sinc::best_bp);
    }
    else
    {
 #if S_WIN32
      nfnd[11] = 0;
      mbwrite(nfnd);
      nfnd[11] = ' ';
 #endif
      mlwrite(
 #if GOTTYPAH
              typahead() ? TEXT220 : 
 #endif
				     nfnd, tgt);
      curwp->w_flag &= ~(WFFORCE|WFMODE|WFHARD);	     /* stay put */
    }
    g_nosharebuffs = false;
 #endif

    lp->linelength = slpu;
  }

  for ( lp = (meLine*)Sinc::namelist; lp != null; lp = (meLine*)Sinc::namelist)
  { Sinc::namelist = *(char**)lp;
    free(reinterpret_cast<char*>(lp));
  }

  return cc != 0;
}



int Pascal getIncls(int f, int n)

{ static const char * fistr = "file incls";
         int at_end = curwp->w_dotp - curbp->b_baseline;
	if (at_end == 0)
		curwp->w_dotp = curwp->w_dotp->l_bp;
	
  Sinc::g_outbuffer = bfind(fistr, TRUE, 0);
  if (Sinc::g_outbuffer == NULL)
    return 0;

  if (Sinc::g_outbuffer->b_fname == NULL)
    Sinc::g_outbuffer->b_fname = strdup(fistr+5) ;

{ int cc = bclear(Sinc::g_outbuffer);
  if (cc != TRUE)
	  return cc;

  cc = searchIncls(f, n);
  if (Sinc::g_outbuffer->b_baseline->l_fp != Sinc::g_outbuffer->b_baseline)
  { Sinc::g_outbuffer->b_flag |= BFACTIVE;
    swbuffer(Sinc::g_outbuffer);
  }
  else
  { mlwrite("No includes fnd");
		zotbuf(Sinc::g_outbuffer);      
  }
  Sinc::g_outbuffer = NULL;
  if (at_end == 0)
		curwp->w_dotp = curwp->w_dotp->l_fp;
  return cc;
}}

}

#endif
