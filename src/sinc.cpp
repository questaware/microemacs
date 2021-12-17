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

#define EXTRA_ZOT_ARG ,1
#define EXTRA_SW_ARG(x) x,

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
#define EXTRA_ZOT_ARG
#define EXTRA_SW_ARG(x)

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

#if AM_ME

char * Pascal skipspaces(char * s, char * limstr)
	
	/* char *     limstr;			* the last char */
{ int lim = limstr - s;
  int ix;
  char ch;
  
  for (ix = -1; ++ix < lim && ((ch=s[ix]) == 'L'-'@' ||
                                 ch == ' ' || ch == '\t');)
    ;
    
  return &s[ix];
}


typedef struct Paren_s
{ 
  char          ch;
  char          fence;
  char          prev;
  char          in_mode;
  int           nest;
  int           nestclamped;
  int           sdir;
} Paren_t, *Paren;


static Paren_t paren;



int Pascal init_paren(const char * str, int len)

/*paren.nestclamped = 1;*/
{ paren.sdir = 1;
  paren.nest = 0;
  paren.in_mode = 0;
  paren.prev = 0;
  paren.ch = *str;
  
{ char ch = paren.ch;
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
  int mode = paren.in_mode;

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
int Pascal USE_FAST_CALL scan_for_sl(meLine * lp)

{ int cplim = lp->linelength;
  int ix;
  
  Paren_t sparen = paren;

  init_paren("{",0);
  for (ix = -1; ++ix < cplim; )
  { 
    if (!(scan_paren(lp->linetext[ix])
            & (Q_IN_STR+Q_IN_CHAR+Q_IN_CMT+Q_IN_CMT_))
        && lp->linetext[ix] =='/' && lp->linetext[ix+1] == '/')
      break;
  }

  paren = g_sparen;
  return ix;
}
#endif

static const char * USE_FAST_CALL find_nch(int cplen, const char * cp)
	/* int     cplen;			* the one after the last char */
{ 
  for ( ; --cplen >= 0; )
    if (! isword(*++cp))
      return cp;

  return  "";
}

#define CHQ_ESC 0x1b

#define NO_BEST 1000


#define M_NOGO  (-16384)

#define M_IN_SCT        1	  /* IN STRUCT after LBRACE */ /* must be 1 */
#define M_IN_W          2	  /* IN WORD */
#define M_IN_P          4	  /* IN ) .. { */
#define M_AFTER_KW	    8	  /* immediately after keyword */
#define M_AFTER_CC   0x010	/* after colon colon */
#define M_GOT_NM     0x020	/* GOT THE NAME (NEVER LOST) */
#define M_AFTER_NM   0x040	/* immediately after THE name */
#define M_AFTER_NM0  0x080	/* immediately after 0 words nm  */
#define M_AFTER_RP   0x100	/* immediately after RPAREN */
#define M_PRE_SCT    0x200	/* after class/struct but before LBRACE */

#define M_NEED_B2    0x400	/* after RPAREN in NEED_BODY*/
#define M_NEED_BODY  0x800	/*               => M_AFTER_NM */
#define M_IN_DCL    0x1000	/* in a b .... ; obsolete */
#define M_NO_BODY   0x2000	/* bodies are not legal */
#define M_IN_ENUM   0x4000	/* in enum ... */
#define M_BAN_LP    0x8000	/* left paren must not appear (NEVER LOST) */

/* The state machine includes:

   fn           (             ,          )         int        a    ;

  NEED_LPRPX  ->     NEED_BODY             -> NEED_B2
*/

#define BEFORE_WINDOW_LEN 100

}

class Sinc
{
	private:
		enum { F_NF, F_DCL, F_FLD, F_DCLINIT, F_ENUM, F_METH, F_OMETH, 
					 F_SCT, F_CLASS, F_PARAM, F_DERIVED };
	
		static int sparen_nest;
		static int sparen_nc;		/* nest clamped */
		
		static Lpos_t ext_c;

//	static int prev_block;		/* not in use */

		static const char * const ops[];

#if AM_ME
		static meLine * basel;			/* extra parameter */
# define SETBL(bl) Sinc::basel = bl
#else
# define SETBL(bl)
#endif
		static int strxct_mtch(int stt, const char * s_, 
																		const char * slim);
		/*static Short srchdpth = 0;*/
	public:
		static char * namelist;
		static meBuffer * g_outbuffer;	/* buffer accumulating includes */
		static const char * target;

		static short from_wh;						/* searching from 1 field, 2 class */
		static short good_class;				/* nesting of valid T C::p() */
		static short ccontext; 					/* measures paren nesting p1(){ */
		static short ccont_need_eq;

		static short ask_type;
		static short srch_exact;	/* <= 0 => true */
		static short min_paren_nest;

		static short best_nest;			/* the most unnested match */
		static meBuffer * best_bp;
                                            /* 0 or (+ve) type found */
    static int doit_forward(meLine * lp, char * cp_);
		static int USE_FAST_CALL srchdeffile(int depth, const char * fname);
		static void USE_FAST_CALL bufappline(int indent, const char * str);
};

int Sinc::sparen_nest;
int Sinc::sparen_nc;
Lpos_t Sinc::ext_c;

// int Sinc::prev_block;

char * Sinc::namelist;
meBuffer * Sinc::g_outbuffer;
short Sinc::good_class;
short Sinc::from_wh;		/* INHERITED ONLY */
short Sinc::ccontext;
short Sinc::ccont_need_eq;

short Sinc::ask_type;
short Sinc::srch_exact;
short Sinc::min_paren_nest;

short Sinc::best_nest;
meBuffer * Sinc::best_bp;

const char * const Sinc::ops[] =
		 	{ "class","struct","union","enum",  /* 4: */
//	  	"public","private","protected", /* 7: */
		   	"and","or","not",
		   	"return","if", "while", "then","else", "switch", // end of ops
				"define", "include", };

const char * Sinc::target;

int Pascal Sinc::strxct_mtch(int stt, const char * s_, 
											 								const char * slim)
{ const char * * t_;
	int ix = 1;
	if      (stt < 0)
	{ t_ = &Sinc::target;
	}
	else if (stt > 0)
	{ t_ = (const char **)&Sinc::ops[upper_index(Sinc::ops) - 1];
		++ix;
	}
	else
	{ t_ = (const char **)&Sinc::ops;
		ix = upper_index(Sinc::ops) - 1;
	}
  for ( ; --ix >= 0; )
  { const char * pat = t_[ix];
    const char * src = s_;
    for (; src < slim && *pat == *src; ++pat)
      ++src;

    if (*pat == 0)
    { if (src >= slim || ! isword(*src))
        break;
    }
  }
										/* < 0 : not found */
  return ix;
}

/* Append a line to a buffer */

void USE_FAST_CALL Sinc::bufappline(int indent, const char * str)

{ int    sz = strlen(str);
	if (Sinc::g_outbuffer)
	{	LINE * inslp = mk_line(null, indent+sz, indent+sz);
	   
		if (inslp != NULL)
		{ memset(&inslp->l_text[0], ' ', indent);
			strcpy(&inslp->l_text[indent], str);
			ibefore(Sinc::g_outbuffer->b_baseline.l_fp, inslp);
		}
	}
}


int Sinc::doit_forward(meLine * lp, char * cp_)

{ int  word_ct = M_NOGO;

  for (int ix = BEFORE_WINDOW_LEN+1; --ix > 0; )
  {
#if AM_ME
    if (lp == Sinc::basel)
#else
    if (lp->l_props & L_IS_HD)
#endif
    { word_ct = 0;
      break;
    } 
    lp = meLineGetPrev(lp);
  }
//Sinc::prev_block = -1;
{ int  obrace_dpth = paren.nestclamped;
  init_paren("[",-1);
{ bool in_esc = false;		/* line after \ */
  int  paren_dpth = 0;
  int  brace_dpth = 0;		/* relevant only to structures */
  int  paren_nm = 0;
//int  brace_nm;
//int  tok_ct = 0;

{ int save_a_cc = 0;
  int state = 0;
  int  rp_line = -1;		/* line containing a () */
  int  lines_left;

  for (lines_left = 2 * BEFORE_WINDOW_LEN; --lines_left >= 0; )
  { lp = meLineGetNext(lp);

#if AM_ME
    if (lp == Sinc::basel)
#else
    if (lp->l_props & L_IS_HD)
#endif
      break;
      
    int directive = 0;

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
    { int ch = cp == cplim ? '\n' : *cp;
      
      if (! directive )
      { if (isspace(ch)) 
          continue;
        --directive; // -1
        if (ch == '#')
        { directive = 1;
          if (cplim[-1] == '\\')
            in_esc = true;
          /* word_ct = 0; state = 0; */
        }
      }

      if      (scan_paren((char)ch))
      { state &= ~M_IN_W;
//      if   (ch == '"')
//			{ if (cp - 2 >= cpstt && cp[-1] == 'C' || cp[-2] == '"')
//         	ch = 'A';
//     	  else if (ch != '\'')
//         	continue;
//			}
      }
      else if (paren.nest == 0 && directive <= 0)
      { if (paren_dpth < 2 && isword((char)ch))
        {
          if ((state & M_IN_W) == 0)
          {
            ch = strxct_mtch(0, cp, cplim);
            if (ch >= 0)
				    { if (ch > 3)
				    	{ state |= M_AFTER_KW;
				        word_ct = M_NOGO;
				      }
//			      else if (ch >= 5)								/* protected, public or private */
//			      { cp += strlen(ops[ch])-1;			/* ignore following ch */
//			        /*word_ct = 0;*/
//			        continue;
//    			  }
	      			else			/*class, union, struct, or enum */
				      { // prev_block = ch;
	    		    /*if ((state & M_IN_SCT) == 0)*/
	        		  state |= M_PRE_SCT;
	        			if (ch == 3)
				          state |= M_IN_ENUM;
	      			}
	    			}
            state |= M_IN_W;
#if 0
            if ((state & M_AFTER_NM))
				      if (paren_dpth==0 && brace_dpth <= brace_nm)
	    			  { loglog3("R00 %d %d %d", paren_dpth, brace_dpth, brace_nm);
                return F_NF;		/* a tgt   b; ignored */ 
								/* a tgt ()b; allowed */
			        }
#endif
      			if (state & M_GOT_NM)
      			{ if ((state & (M_AFTER_RP+M_NEED_BODY)) == 0)
      					return F_NF;
				      state |= M_BAN_LP;
      			}
      			else
      			{	if (word_ct > 0)
            		state |= M_IN_DCL;
	       	    word_ct += 1;
         	  	if      (cp == cp_)
	           	{ state |= M_GOT_NM | M_AFTER_NM;
//             	brace_nm = brace_dpth;
	             	paren_nm = paren_dpth;		// is >= 0
							
	      	      loglog4("Z %x rp_l %d l_left %d wc %d", state,rp_line,lines_left,word_ct);
	             
		      			if (paren_dpth > 0 && (from_wh || good_class)) /* p (^here) */
		      			{ loglog1("pdrej %20.20s", cp);
				        	return F_NF;
	    			  	}

	      				if ((state & M_IN_P) && sparen_nest == -1 ||
	          				word_ct < 0		 /* set back */ 				||
		          		  rp_line == lines_left  /* a (x)..tgt; ignored */)
	      	      { loglog("Arft RP");
									return F_NF;
	      				}

								if (sparen_nc > 1)
				        	return F_NF;

	            	if ((state & M_PRE_SCT) == 0 && word_ct < 0) 
	      				{ /*paren_dpth = 0;*/
	              	state |= M_AFTER_NM0;
		      			  continue;
	            	}
	            }
						}
          }
        	state &= ~(M_AFTER_NM0+M_AFTER_RP);
        }
        else					/* not a letter */
        { const char * t;
          state &= ~M_IN_W;
          if (isspace(ch) || ch == ']')						// [xxx] is invisible
            continue;
//        tok_ct += 1;
          if      (ch == '*' || ch == '&')
          {	if (state & M_AFTER_NM)
          		return F_NF;
          }
          else if (ch == ':' && cp < cplim && cp[1] == ':')
          { --word_ct;
            ++cp;
          }
          else if (ch == '<' && *(t = find_nch(cplim-cp-1, cp)) == '>')
          { cp = t;
            state |= M_IN_W;
          }
          else if (ch == '(' && !(state & M_AFTER_KW))
				  { save_a_cc |= state;
	    			if (state & M_BAN_LP)
	    			{ loglog("BAN");
				      return F_NF;
	    			}
	    			paren_dpth += 1;
            loglog2("PINC %d %s", paren_dpth, cpstt);
            if (paren_dpth < 2 &&
               (state & M_AFTER_NM))
            { 
              if (word_ct - 1 > paren_nm)
              { if (!(state & M_AFTER_CC) || (from_wh+good_class))
       	        {/*char bufff[50];
       	          sprintf(bufff, "word_ct %d", word_ct);
       	          mbwrite(bufff);*/
                  return F_CLASS;
                }
              }
				      if ((state & M_IN_SCT)==0 && paren_dpth==1)
                state |= M_NEED_BODY;
            }
            if (word_ct > 0)
              word_ct = 0;
          }
          else if (ch == ',' || ch == ')' /* rparen */) 
				  { if (state & M_AFTER_NM0)
	    			{ loglog("ANM0");
				      return F_NF;
	    			}
	    		  if (paren_dpth!=0  && obrace_dpth < 0 &&
	        			sparen_nc != 0 &&
	       			 (state & (M_GOT_NM+M_IN_DCL+M_NO_BODY)) == (M_GOT_NM+M_IN_DCL))
	      		{	state |= M_NEED_BODY;
	      			state &= ~M_NEED_B2;
	      		}	
						word_ct = 0;
	    			if (ch == ',')
				    { if ((state & M_IN_ENUM) && (state & M_AFTER_NM))
	        			return -F_ENUM;

				    	if ((state & (M_GOT_NM+M_NEED_BODY+M_PRE_SCT))==M_GOT_NM)	
				    	{	if (paren_dpth == 0 && paren_nm == 0 &&
	          		  	sparen_nc <= 0)
	          		  return F_DCL;
	          		if ((state & M_NEED_BODY) == 0)
	          			return F_NF;
	          	}

	      			if      ((state & M_IN_DCL) && paren_dpth == 0 || 
	            			   (state & M_IN_ENUM))
								word_ct = 1; 
				      else if (paren_dpth != 1)  // || word_ct <= -127)
								word_ct = M_NOGO;
	    			}
				    else
	    			{ state |= (save_a_cc & M_AFTER_CC);
              loglog2("DINC %d %s", paren_dpth, cpstt);
				      if (--paren_dpth == 0)
	    			  { save_a_cc = 0;
				        state &= ~(M_IN_W+M_AFTER_NM+M_AFTER_NM0+M_IN_P);
								state |= M_AFTER_RP;
								if (state & M_NEED_BODY)
								{ rp_line = lines_left;
								  state |= M_NEED_B2;
								}
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
	      			{ int res = word_ct > 1 ? F_SCT : F_NF;
	        			loglog2("Res %d : %20.20s", res, cp);
	        			return res;
	      			}
				      if ((state & M_AFTER_RP) || ch == ';')
	    			  { loglog("ARP");
								return F_NF;
				      }
	    			}
				    paren_dpth = 0; 
	    			state &= ~(M_IN_DCL+M_IN_ENUM+M_NEED_BODY);
				    if	    (ch == '}')
	    			{ word_ct = (state & M_IN_SCT);  // == 1
				      brace_dpth -= 1;
				      state &= ~(M_NO_BODY+M_PRE_SCT);
	    			  if (brace_dpth <= 0)
				      { state &= ~(M_IN_P+M_IN_SCT);
								brace_dpth = 0;
								// prev_block = -1;
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
				    { loglog3("S %x fw %d gc %d", state, from_wh, good_class);
				      return paren_nm <= 1 && 
				      			(state & (M_NEED_BODY+M_PRE_SCT)) && 
				             ((state & M_NEED_BODY)|| !(state & M_AFTER_CC) ||
				               (from_wh+good_class))
				                        ? -F_METH : F_NF;
				    }
				    if (state & M_PRE_SCT)
				      state |= M_IN_SCT;
				    state &= ~(M_IN_P+M_PRE_SCT+M_NEED_BODY);
				    word_ct = 0;
				    paren_dpth = 0; 
				    brace_dpth += 1;
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
				  else if (ch == ':')											/* single colon */
				  { if (state & M_PRE_SCT)
				    { if (state & M_AFTER_NM && paren_nm == 0)
				        return F_DERIVED;
				    }
				    word_ct = M_NOGO;
			      if (state & M_IN_SCT)
			        word_ct = 0;
				  }
				  else if (ch == '=')
				  { if (state & M_AFTER_NM)
				    { if (state & M_IN_ENUM)
				        return -F_ENUM;
				      if (word_ct > 1)
				      { if (paren_dpth == 0)
				          if (sparen_nc < 0 || !(state & (M_NEED_BODY+M_PRE_SCT))&&!paren_nm)
				            return F_DCLINIT;
			          if ((from_wh+good_class) || !(state & M_AFTER_CC))
				        { if (state & M_IN_SCT)
				            return F_FLD;
				          if ((state & M_NEED_BODY) == 0 &&
						          (paren_dpth == 0 || sparen_nc < 0))
								    return F_OMETH;
					      }
				      }
				      loglog("C =");
					    return F_NF;
					  }
				    word_ct = -127;
					}
					else
					{ word_ct = M_NOGO;
					  if (state & M_AFTER_NM)
					  { loglog("ANM");
				      return F_NF;
				   	}
				  }			/* transient states :*/
          state &= ~(M_AFTER_NM+M_AFTER_NM0+M_AFTER_RP+M_AFTER_CC+M_AFTER_KW);
        }
      }
    } /* loop */
//  for(int ct = 10001; (--ct > 0 && lp->linetext[lp->linelength-1] == '\\');)
//    lp = meLineGetNext(lp);
  }

  loglog("Ret 0");
  return 0;
}}}} /* doit_forward */

#define SLEN 133
#if SLEN < NFILEN + 35
# undef SLEN
# define SLEN NFILEN+35
#endif

#define Q_NEED_RP 80
//#define Q_GOT_LP  40

				 /* result: 0 => not found, -1 => aborted, else
					    type of occurrence */
int Sinc::srchdeffile(int depth, const char * fname)
  	
{ Paren_t sparen = paren;
  char lbuf[SLEN+2];
  char mybuf[SLEN];
  int rcc;
  meBuffer * bp;
  char * src;
#if MEOPT_TYPEAH
  if (TTahead())
    return 0;
#endif
#if GOTTYPAH
  if (typahead())
    return 0;
#endif

  for (src = namelist; src != null; src = *(char**)src)
  { if (strcmp(fname, &src[sizeof(char*)]) == 0)
      return 0;
  }    
{
#if AM_ME
   static char incldr[] = "$incldirs";
  meWindow * wp = frameCur->windowCur;
  meBuffer * origbp = frameCur->bufferCur;
#else
	WINDOW * wp = curwp;
  meBuffer * origbp = curbp;
#endif
  meBuffer * ebp;
  
#if AM_ME
  ebp = fname[0] == 0 ? origbp : bfind(fname, 0);
	bp = ebp;

  while (bp == NULL)		      /* yuk, newfile writes curbp ! */
  { char * ffile;
    src = getval(incldr);
    if (src == NULL || *src == 0)
    { src = meGetenv("INCLUDE");
      if (src == NULL)
        src = "";
        
      setVar(incldr, src, NULL);
    }

    ffile = fLookup(&lbuf[0], fname, NULL, 
			    					meFL_CHECKDOT | meFL_EXEC,
			    					origbp->fileName, src);
    if (ffile != NULL)
    { loglog1("fLookup %s", src);
      bp = bfind(src, BFND_CREAT);
      bp->fileName = meStrdup(ffile) ;
      bp->intFlag |= BIFFILE ;
    }

    if (src == NULL)
		 src = "";
  { int idlen = strlen(src);
    if (idlen >= SLEN-6 || Sinc::ask_type != 0)
      return 0;
      
    if (meGetString(strcat(strcpy(lbuf,fname),TEXT224),
    								MLSEARCH,0,&lbuf[idlen+1],SLEN-3-idlen) != meTRUE ||
        lbuf[idlen+1] == 0
       )
			return 0;
    strcpy(&lbuf[0], src)[idlen] = PATHCHR;
    setVar(incldr, lbuf, NULL);
  }}

  swbuffer(wp, bp);
  
  if (bp->intFlag & BIFSRCH)
    return 0;
#else
  ebp = fname[0] == 0 ? origbp : gotfile(fname);
	bp = ebp;

  loglog3("WHILE SBP %x BP %x CBP %x", origbp, bp, curbp);

  while (bp == NULL)		      /* yuk, newfile writes curbp ! */
  {	const char * fcp = (char*)flook('I', fname);
  
    if (fcp == NULL)
      loglog1("flook failed %s", fname);
    else
    { bp = bufflink(fcp, TRUE);
      loglog3("SBP %x BP %x CBP %s", origbp, bp, curbp);
    }

    if (bp == NULL)
    { const char * ids = pd_incldirs;
      int idlen = strlen(ids);
      if (idlen >= SLEN-6 || Sinc::ask_type != 0 ||
					mlreply(concat(&lbuf[0], fname, TEXT224, null),
									&lbuf[idlen+1], SLEN-3-idlen) != TRUE)
				return 0;
      strcpy(&lbuf[0], ids)[idlen] = PATHCHR;
			remallocstr(&pd_incldirs, lbuf, 0);
    }
  }

  swbuffer(bp);
  paren = sparen;
  loglog2("origbp %x bp %x", origbp, bp);
 
  if (bp->b_flag & MDSRCHC)
    return 0;
#endif

  if (Sinc::ask_type == '1')
  { /*setcline();*/
    return 1;
  }

{ Lpos_t cand;
	cand.curline = NULL;
{ meLine * prev;
  char ch;
  int good_fld = false;
  int got_prochead = 0;											// detects ( ) { 
  int word_ct = 2;
  int sword_ct = 2;
  char gs_buf [NSTRING];

#if AM_ME  
  int cand_lnno;  
  meUShort save_o = wp->dotOffset;
  meLine * save_l = wp->dotLine;
        
  meLine * lp = meLineGetPrev(bp->baseLine);
  int lnno = bp->lineCount;

  if (fname[0] == 0)
  { lp = save_l;
    lnno = bp->dotLineNo;
  }
    
  bp->intFlag |= BIFSRCH;

  for (rcc = 0; Sinc::best_nest > 0 && lp != bp->baseLine;
                lp = prev)
#else
  Lpos_t save = *(Lpos_t*)&wp->w_dotp;
  LINE * lp = lback(&bp->b_baseline);

  loglog3("cwp %x lp %x ? %x", wp, save.curline, 0);
  if (fname[0] == 0)
  	lp = save.curline;
    
  bp->b_flag |= MDSRCHC;

  for (rcc = 0; Sinc::best_nest > 0 && (lp->l_props & L_IS_HD) == 0;
                lp = prev)
#endif
  {	int cplen = lp->l_used;
    prev = meLineGetPrev(lp);
      
#if AM_ME  
    --lnno;
#endif
    if (/*prev->length > 0 && */ prev->linetext[prev->linelength-1] == CHQ_ESC)
      continue;

    if (cplen == 0)
      continue;

    if (cplen > SLEN)
      cplen = SLEN;

  { char * cpstt = &lp->linetext[0];
    char * cplim = &cpstt[cplen - 1];		// inclusive
    char * cp = skipspaces(&cpstt[0], cplim);
    int not_directive = cp[0] - '#';
	  
    if (! not_directive)
		{	cp = skipspaces(cp+1, cplim);

      if (strxct_mtch(upper_index(ops)-1, cp, cplim) >= 0 &&	 // define, include
          (paren.in_mode & (Q_IN_STR+Q_IN_CHAR+Q_IN_CMT+Q_IN_EOL)) == 0)
      { ch = *cp;
        cp = skipspaces(&cp[7], cplim);
    
        if (ch == 'd')
        { if (strxct_mtch(-1, cp, cplim+1) >= 0 && Sinc::g_outbuffer == NULL)
				  { cand.curline = lp;
				    cand.curoff = cp - &lp->linetext[0];
				    set_cand_lnno(lnno);  
				    rcc = 100;
				    Sinc::best_nest = -1;
				  }
        }
        else if (rcc == 0)
        { char * fn = (char*)meMalloc(strlen(fname)+sizeof(char *)+2);
				  if (fn != null)
				  { *(char**)fn = namelist;
				    namelist = fn;
				  	strcpy(&fn[sizeof(char *)], fname);
				  }

/*#define fnb ((char*)gs_buf)*/
#define fnb mybuf
				  cp = strpcpy(&fnb[0], cp+1, 128);
				  
				  while ((ch = *++cp) != 0 && ch != '"' && ch != '>')
				    ;
				  
				  *cp = 0;
				  if (Sinc::ask_type == 0)
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
				    { if (b[0] == 0 || b[0] == ' ' || b[0] == '1')
				      { ch = meFALSE;
								Sinc::ask_type = b[0];
	    			  }
				    }
					  if	(ch < FALSE)		// Abort
					  { rcc = -1;
					    Sinc::best_nest = -1;
					    continue;
					  }
					  if (ch > FALSE)
					  	continue;
				  }

				  /*char buff[80];
				    char spaces[] = "                    ";
				    concat(&buff[0], &spaces[20-srchdpth], "SI ", &fnb[0], null);
				    logstr(buff); */
#if AM_ME
				  wp->dotLine = save_l;
				  wp->dotOffset = save_o;
#else
				  rest_l_offs(&save);
#endif
				/*++srchdpth;*/
			    loglog2("srchdeffile %d %s", paren.nest, fnb);
				  rcc = srchdeffile(depth+1, &fnb[0]);
				  if (rcc > 0)
				    cand.curline = NULL;
          loglog3("SRCHDEFFILE %d %d %s", paren.nest, rcc, fnb);
					(void)Sinc::bufappline(depth * 3, fnb);
        /*--srchdpth;*/
        }
      }
      continue;
    }
				      												/* deal with double slash this ' . */
	{ int offs = scan_for_sl(lp);
		cp = cpstt + offs;
    while (--offs >= 0 && Sinc::best_nest > 0)
    { ch = *--cp;
      if (paren.in_mode || !not_directive)
scan:   
        scan_paren(ch);
      else        
      { switch (ch)
        { case ' ':
          case '\t':
	  			when '=': ccont_need_eq = false;
				  case ',': if (ccont_need_eq)
				  						ccontext = 0;
				  					word_ct = 0;
				  when '+':
				  case '-': ccont_need_eq = true;

				  when '*':
				  case '&': word_ct -= 1;
				  
          when '}':{ Cc rc = FALSE;
          					 Lpos_t here = *(Lpos_t*)&wp->w_dotp;
          					 wp->w_dotp = lp;
          					 wp->w_doto = cp - cpstt;
									 
          					 if (paren.nest <= 0)
          					 { if (getfence(0,1))
          					   { Sinc::ext_c = *(Lpos_t*)&wp->w_dotp;
          					 	   rc = nextword(0,-1);
          					 	   if (rc)
	          					   { int offs = wp->w_doto;
	          					   	 LINE * tlp = wp->w_dotp;
  	        					 	   int rem = tlp->l_used - offs - 3;
    	      					 	   if (rem < 0 || tlp->l_text[offs]  != '"'
    	      					 	 						   || tlp->l_text[offs+2]!= '"'
    	      					 	 						   || tlp->l_text[offs+1]!= 'C')
    	      					 	 	   rc = false;
    	      					   }
          					   }
          					 }

          					 rest_l_offs(&here);
    	      				 if (rc)						// dont scan
    	      				 	 break;
	    	      			 Sinc::ext_c.curline = NULL;
          				 }	
          case '/': case '\\': 
          case '"': case '\'': 
          					goto scan;

				  when ';': ccontext = 0;
				  					word_ct = 0;
				  					got_prochead = 0;
						  		/*if (Sinc::best_nest != NO_BEST)
          					  loglog1("CBRACE %d", paren.nest);*/
          when ']': sword_ct = word_ct;
          when '[': word_ct = sword_ct;
          when ')': word_ct = 0;
          					if (ccontext >= Q_NEED_RP)
						        	ccontext += 1;
      
          when '(': word_ct = 0;
          					if (ccont_need_eq)
				  						ccontext = 0;

          					if (ccontext >= Q_NEED_RP)
										{ ccontext -= 1;
						        	if (ccontext == Q_NEED_RP)
						        		got_prochead = 1;
										}
				  when ':': if (ccontext == Q_NEED_RP)
						  	    { if (offs > 0 && cp[-1]==':')
	  	    					  { 
						  	        if      (good_class > 0 ?  good_class < paren.nest
	  	    														    			: -good_class < paren.nest)
	  	          					good_class = paren.nest;
						  	        else if (good_class = paren.nest)
						  	          good_class = -paren.nest;
	  	    					  //mbwrite("[from class]");
	  	      					}
	  	    					}
          when '{': word_ct = 0;
          				/*if (Sinc::best_nest != NO_BEST) 
                      loglog1("OBRACE %d", paren.nest);*/
										if (Sinc::ext_c.curline != lp ||
												Sinc::ext_c.curoff  != cp - cpstt)
	            				scan_paren(ch);
      		  			/*if (paren.nest < 0)
                      paren.nest = 0;*/
               				      /* structure definitions */
							    	if (paren.nest <= min_paren_nest)
			    					{ min_paren_nest = paren.nest;
								      ccontext = Q_NEED_RP;
			    					}
																	    					/* is the candidate good? */
								    if (paren.nest < sparen_nest &&
								      /*paren.nest >= 0          && */Sinc::best_nest != NO_BEST) 
		    						{ int ct;									/* look back for class or struct */
								      wp->wLine = lp;
								      wp->wOffset = offs;
							 	      loglog3("TRYDEC %d %d %d", best_nest, paren.nest, rcc);

								      for (ct = 40; --ct > 0 && backWord(1, 1); )
								      { int wh;
								        char * wd = (char *)&wp->wLine->linetext[wp->wOffset];
								      /*if      (wd[0] == '(')
												  rcc = -1;*/
											      /*else if (wd[0] == ':')
												  rcc = 2;
												else */ if (wd[0] == ';' || wd[0] == '}')
											  { ct = 0;
												  break;
												}
								        wh = strxct_mtch(0, wd,
													 							 &wd[wp->wLine->linelength - wp->wOffset]);
								        if (!in_range(wh, 0, 4) )
												  continue;

												ct = 0;			/* end of looping */
								        if      (wh == 4)	/* "C" */
												  ;
												else if (wh == 3)	/* enum */
												{ if (from_wh & 1)/* field */
												    break;
												} 
												else if (from_wh || good_class)
												{ //mbwrite("CSU");
												  good_fld = true;
												}
											/*else if (wh == 2)
												  break;*/
												else
												/* if (!from_class) */
												    break;
												Sinc::best_nest -= 1;		/* "C", enum, class unnest */
												loglog4("dec bn to %d, %d %d %s", best_nest, wh, rcc, wd);
												ct = -1;
							  	    }
							  	    if (ct == 0)
								      { loglog1("lost best %s", cp);
						//		      mbwrite("Lost Best");
												Sinc::best_nest = NO_BEST;
												/*best_bp = NULL;*/
												cand.curline = NULL;
												rcc = 0;
								      }
								    }

				  otherwise if (ccontext == Q_NEED_RP &&
												strxct_mtch(0, cp, cplim) > 3)
											got_prochead = 0;

                    if ((offs == 0 || ! isword(cp[-1])) && isword(ch))
                    { word_ct += 1;
      	              if      (ch != *target ||
      	            					 strxct_mtch(-1, cp, cplim + 1) < 0)
				                 ; 		 							   /* unique code point for match */ 
								      else if (Sinc::g_outbuffer != NULL)	/* word matched */
		  					       ;
						   	      else if (srch_exact > 0 /* && fname[0] != 0 */)
		      						{
#if AM_ME
								        strncpy(&lbuf[0], cpstt, cplen+2);
								        ch = meGetString(lbuf,MLSEARCH,0,lbuf,80);
#else
				                ch = getstring(lbuf,80,strpcpy(&lbuf[0],cpstt,cplen+1));
#endif
								        if      (ch == meABORT)
												  rcc = -1;
											  else if (ch == meFALSE || lbuf[0] == 0)
											    continue;
											  else
												{	cand.curline = lp;
													cand.curoff = offs;
											    set_cand_lnno(lnno);
												  rcc = 100;
												  Sinc::best_nest = 0;
												}
											}
											else if (word_ct <= 1 && /*paren.nestclamped<=2 &&*/ 
															(ccontext <= Q_NEED_RP || 
															 got_prochead <= 0)
															&& Sinc::best_nest == NO_BEST)
											{ sparen_nc = paren.nestclamped;
											  sparen_nest = paren.nest;
											  loglog2("TRY %d %s", sparen_nest, cpstt);
											  loglog2("Try %d %s", sparen_nc, cp);
//											sprintf(buf, "Try %d %d %50.50s", sparen_nest, sparen_nc, cp);
//											mbwrite(buf);
												SETBL(bp->b_baseLine);
											{ int wh = doit_forward(lp, cp);
								        if (wh != F_NF &&
								        		( sparen_nc <= 0 ||
								        			sparen_nc == 1 && wh < 0))
											  {	rcc = wh;
												  cand.curline = lp;
		        							cand.curoff = offs;
										      set_cand_lnno(lnno);  
			   									Sinc::best_nest = sparen_nc;
			   
													if (Sinc::best_nest == 0 && (from_wh & 3))
												  {//mbwrite("Equal nest ff");
												    ++Sinc::best_nest;
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
								      }}
		    						}
				            goto scan;
				}
      }  
    } /* for (over the line) */
    scan_paren('\n');
    if (cand.curline != NULL && from_wh && !(good_fld+good_class) 
    												 && Sinc::best_nest != 1 && rcc != 100)
    { cand.curline = NULL;
      rcc = 0;
      Sinc::best_nest = NO_BEST;
    }
  }}} /* while "lines" */

#if AM_ME
  bp->intFlag &= ~BIFSRCH;
 
  if (cand.curline != NULL && Sinc::best_bp == NULL/*&& Sinc::best_nest<=0)*/)
  { loglog2("SETTING doto %d bn %d", cand.curoff, best_nest);
    Sinc::best_bp = bp;
    wp->dotLine = cand.curline;
    wp->dotOffset = cand.curoff;
    wp->dotLineNo = cand_lnno ;

    wp->updateFlags |= WFMOVEC | WFMOVEL;
  }
  else
  { wp->dotLine = save_l;
    wp->dotOffset = save_o;
  }
#else
  bp->b_flag &= ~MDSRCHC;
 
  if (cand.curline != NULL && Sinc::best_bp == NULL/*&& Sinc::best_nest<=0)*/)
  { loglog2("SETTING doto %d bn %d", cand.curoff, best_nest);
    Sinc::best_bp = bp;
    wp->w_flag |= WFMOVE;
    rest_l_offs(&cand);
	  setcline();
  }
  else
  { rest_l_offs(&save);
    loglog3("Cwp %x lp %x ? %x", wp, wp->w_dotp, 0);
  }
  
  loglog3("Sbp %x bp %x cbp %x", origbp, bp, curbp);
#endif
  if (bp != origbp)
  { swbuffer(EXTRA_SW_ARG(wp) origbp);
		if (ebp == NULL && Sinc::best_bp != bp)
      zotbuf(bp EXTRA_ZOT_ARG);
  }

  return rcc;
}}}}

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

{ char  tgt_[92];
	char * tgt;
#if AM_ME
  meWindow * curwp = frameCur->windowCur;
  meLine * lp = curwp->dotLine;
  int g_nosharebuffs;
#else
  LINE * lp = curwp->w_dotp;
#endif
  int slpu = lp->linelength;
  int ix;

  if (Sinc::g_outbuffer != NULL)
  { tgt_[0] = '?';
    tgt_[1] = 0;
		tgt = tgt_;
  }
  else
  { int ix = curwp->wOffset;
    while (--ix > 0 && isspace(lp->linetext[ix]))
      ;

    Sinc::good_class = 0;
    Sinc::
     from_wh = ix <= 0 								 ? 0 :
     					 lp->linetext[ix-1] == '-' && lp->linetext[ix] == '>' ||
		  	       lp->linetext[ix] == '.' ? 1 :
               lp->linetext[ix-1] == ':' && lp->linetext[ix] == ':'
               												 ? 2 : 0;
		to_kill_buff(-2, 0);
		tgt = getkill();
  }
  Sinc::target = (const char*)tgt;
  Sinc::srch_exact = n - 1;
  Sinc::ask_type = 0;
  Sinc::ccontext = 0;
	Sinc::ccont_need_eq = 0;
  Sinc::min_paren_nest = 0;
  Sinc::best_nest = NO_BEST;
  Sinc::best_bp = NULL;

  init_paren("}",-1);						// set nest and nestclamped to 0
//paren.nest = 0;
//paren.nestclamped = 0;				// <= 0 => visible

  /* namelist = null;*/
  /* srchdpth = 0;*/
  lp->linelength = curwp->wOffset;	/*we dont need an Me 0 terminator here*/
  g_nosharebuffs = true;

  int cc = Sinc::srchdeffile(0, "");

  if (Sinc::g_outbuffer == NULL)

  { if (cc != 0)
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
    				          TEXT85, tgt);
    }
 #else
    { mlwrite("Found typ %d", cc);
      if (Sinc::best_bp != NULL)
        swbuffer(Sinc::best_bp);
    }
    else
    {
      mlwrite(
 #if GOTTYPAH
              typahead() ? TEXT220 : 
 #endif
				     TEXT85, tgt);
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

{ char bi_nm[20];
  int at_end = curwp->w_dotp - &curbp->b_baseline;
	if (at_end == 0)
		curwp->w_dotp = curwp->w_dotp->l_bp;
	
  BUFFER * bp = bfind(strcpy(bi_nm, "file incls"), TRUE, 0);
  if (bp == NULL)
    return 0;

  if (bp->b_fname == NULL)
    bp->b_fname = strdup(bi_nm+5) ;

{ int cc = bclear(bp);
  if (cc == TRUE)
	{	Sinc::g_outbuffer = bp;

	  cc = searchIncls(f, n);
	  if (bp->b_baseline.l_fp == &bp->b_baseline)
	  	linstr(TEXT79);		/* "Not found" */
	  bp->b_flag |= BFACTIVE;
	  swbuffer(bp);

	  Sinc::g_outbuffer = NULL;
	  if (at_end == 0)
			curwp->w_dotp = curwp->w_dotp->l_fp;
	}
  return cc;
}}

}

#endif
