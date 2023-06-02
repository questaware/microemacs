/**************************************************************************
 *
 *  			Copyright 2003 <unknown>.
 *			      All Rights Reserved
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

extern "C"
{
						/* pick up MEOPT_SINC */
#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"

extern int g_bfindmade;

#if MEOPT_SINC

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

#define linetext l_text
#define wLine    w_dotp
#define wOffset  w_doto
#define linelength  l_used

#define meMalloc(n) malloc(n)
#define meBuffer    BUFFER
#define meLine      LINE
#define backWord(a, b) backword(a, b)

int USE_FAST_CALL find_nch(int wh, int cix, LINE * lp)

{ int len = llength(lp);
	
  while ( ++cix >= 0 && cix < len )
  { char ch = lp->l_text[cix];
		if (wh < 0)
		{	if (ch > ' ')
				return cix;
			continue;
		}
    if (ch == wh)
      return cix;
//  if (! isword(ch) )
//   	break;
	}

  return -1;
}

#define CHQ_ESC 0x1b

#define NO_BEST 1000


#define M_NOGO  (-255)

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
		
//	static int prev_block;		/* not in use */

		static int strxct_mtch(int stt, int offs,   // const char * s_, 
																		LINE * lp); // const char * slim);
		/*static Short srchdpth = 0;*/
	public:
	 static char * namelist;
	 static BUFFER * g_outbuffer;	/* buffer accumulating includes */
	 static char  ops[];
	 static const char * target;

	 static short good_class;				/* nesting of valid T C::p() */
	 static short ccontext; 					/* measures paren nesting p1(){ */
	 static short ccont_need_eq;

	 static short ask_type;
	 static short srch_exact;	/* <= 0 => true */
	 static short min_paren_nest;

	 static short best_nest;			/* the most unnested match */
	 static BUFFER * best_bp;
                                            /* 0 or (+ve) type found */
   static int doit_forward(LINE* lp, char* cp_, short from_wh, int obrace_dpth);
	 static int USE_FAST_CALL srchdeffile(int depth, short from_wh, const char * fname);
	 static void USE_FAST_CALL bufappline(int indent, const char * str);
};

int Sinc::sparen_nest;
int Sinc::sparen_nc;

// int Sinc::prev_block;

char * Sinc::namelist;
BUFFER * Sinc::g_outbuffer;
short Sinc::good_class;
short Sinc::ccontext;
short Sinc::ccont_need_eq;

short Sinc::ask_type;						/* response from SP,CR,N,1,^G */

short Sinc::srch_exact;
short Sinc::min_paren_nest;

short Sinc::best_nest;
BUFFER * Sinc::best_bp;

char Sinc::ops[] =
		 		"define\000 include\000"
		   	"return\000 if\000     while\000  else\000   switch\000 " // end of ops
		 	  "class\000  struct\000 union\000  enum\000   "
//	  	"public","private","protected",
				"\"C\"\000    namespace";

const char * Sinc::target;

int Pascal Sinc::strxct_mtch(int wh, int offs, LINE * lp ) // const char * src_, const char * slim)
															/* -1, 0, 1 */
{	int len = llength(lp);
	int t_ = wh * 16 - 8;
	int ct = wh + 2;				// -1: just Sinc::target
		;											// ct = 1

													// the first two    include is 0,   define is 1
													// ct = 2
	
	if (wh > 0)							// the rest         namespace is 0, return is 10
		ct = 11;

  for ( ; --ct >= 0; )
  { const char * pat = wh < 0 ? Sinc::target : &Sinc::ops[t_ += 8];
		int ix;
    for (ix = offs - 1; ++ix < len && *pat == lp->l_text[ix]; ++pat)
    	;
    if (ix >= len || ! isword(lp->l_text[ix]))
	    if (*pat == 0)
        break;
  }
										/* < 0 : not found */
  return ct;
}

/* Append a line to a buffer */

void USE_FAST_CALL Sinc::bufappline(int indent, const char * str)

{ int  sz = strlen(str);
	if (Sinc::g_outbuffer)
	{	LINE * inslp = mk_line(str, indent+sz, 1);
	   
		if (inslp != NULL)
		{	memset(&inslp->l_text[0], ' ', indent);
			strcpy( inslp->l_text+indent, str);
			ibefore(Sinc::g_outbuffer->b_baseline.l_fp, inslp);
		}
	}
}


int Sinc::doit_forward(LINE* lp, char * cp_, short from_w_good_cl, int obrace_dpth)

{ int  word_ct = M_NOGO;

  for (int ix = BEFORE_WINDOW_LEN+1; --ix > 0; )
  {
    if (l_is_hd(lp))
    { word_ct = 0;
      break;
    } 
    lp = meLineGetPrev(lp);
  }
  //Sinc::prev_block = -1;
{ int  rp_line = -1;		/* line containing a () */
  int  no_esc = -1;			/* line after \ */
  int  paren_dpth = 0;		/* always >= 0 */
  int  brace_dpth = 0;		/* always >= 0, relevant only to structures */
  int  paren_nm = 0;			/* always >= 0 */
//int  brace_nm;
//int  tok_ct = 0;

{ int save_a_cc = 0;
  int state = 0;
  int lines_left;
  init_paren("[", 0);

  for (lines_left = 2 * BEFORE_WINDOW_LEN; --lines_left >= 0; )
  { lp = meLineGetNext(lp);

    if (l_is_hd(lp))
      break;

    int directive = 0;		// undecided

//  char * cpstt = &lp->linetext[0];
    int ixlim = lused(lp->l_dcr);
    int nesc = lp->l_text[ixlim-1] - '\\';

    if (!no_esc)
    { no_esc = nesc;
      continue;
    }

	{//char * cplim = cpstt+ixlim;	/* exclusive */
//  const char * cp;
    int cix;

//  for (cp = cpstt-1; ++cp <= cplim; )
    for (cix = -1; ++cix <= ixlim; )
    { int ch, dch;
    	if (cix == ixlim)
    	{ ch = '\n';
    		dch = 1;
    	}
    	else
    	{ ch = lp->l_text[cix];
    		dch = lp->l_text[cix+1] - ch;
    	}
      
      if (! directive )
			{	--directive; // -1
        if (ch == '#')
        { directive += 2;
          no_esc = nesc;			// Trigger the continues above
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
      else if (directive <= 0 && g_paren.nest == 0)
      { if (paren_dpth < 2 && isword((char)ch))
        {
          if ((state & M_IN_W) == 0)
          {
            ch = strxct_mtch(1, cix, lp);
            if (ch >= 0)
				    { if (ch > 5)
				    	{ state |= M_AFTER_KW;
				        word_ct = M_NOGO;
				      }
//			      else if (ch >= 5)								/* protected, public or private */
//			      { cp += strlen(ops[ch])-1;			/* ignore following ch */
//			        /*word_ct = 0;*/
//			        continue;
//    			  }
	      			else			/*class, union, struct, enum, C, namespace */
				      { // prev_block = ch;
	    		    /*if ((state & M_IN_SCT) == 0)*/
	        		  state |= M_PRE_SCT;
	        			if (ch == 2)						// enum
				          state |= M_IN_ENUM;
	      			}
	    			}
            state |= M_IN_W;
#if 0
            if ((state & M_AFTER_NM))
				      if (paren_dpth==0 && brace_dpth <= brace_nm)
	    			  { 
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
         	  	if (lp->l_text + cix == cp_)
//             	brace_nm = brace_dpth;
							{	if (sparen_nc > 1)
				        	return F_NF;

	            	paren_nm = paren_dpth;		// is >= 0
							
		      			if (paren_dpth * from_w_good_cl) /* p (^here) */
		      			{ loglog1("pdrej %20.20s", cp_);
				        	return F_NF;
	    			  	}

	           		state |= M_GOT_NM | M_AFTER_NM;
	      				if ((state & M_IN_P) && sparen_nest < 0 ||
	          				word_ct < 0		 /* set back */ 			||
		          		  rp_line == lines_left  /* a (x)..tgt; ignored */)
	      	      { loglog("Arft RP");
									return F_NF;
	      				}

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
        { int t;
          state &= ~M_IN_W;
          if (ch <= ' ' || ch == ']')						// [xxx] is invisible
            continue;
//        tok_ct += 1;
          if      (ch == '*' || ch == '&')
          {	if (state & M_AFTER_NM)
          		return F_NF;
          }
          else if (ch == ':' && dch == 0)	// cix < ixlim && lp->l_text[cix+1] == ':'
          { --word_ct;
            ++cix;
          }
          else if (ch == '<' && (t = find_nch('>', cix, lp)) > 0)
          { cix = t;
            state |= M_IN_W;
          }
          else if (ch == '(' && !(state & M_AFTER_KW))
				  { save_a_cc |= state;
	    			if (state & M_BAN_LP)
	    			{ loglog("BAN");
				      return F_NF;
	    			}
	    			paren_dpth += 1;
            if (paren_dpth < 2 &&
               (state & M_AFTER_NM))
            {
				      if ((state & M_IN_SCT) == 0 && paren_dpth > 0)
                state |= M_NEED_BODY;
              if (word_ct - 1 > paren_nm)
              { if (!(state & M_AFTER_CC) || from_w_good_cl)
       	        {/*char bufff[50];
       	          sprintf(bufff, "word_ct %d", word_ct);
       	          mbwrite(bufff);*/
                  return F_CLASS;
                }
              }
            }
            if (word_ct > 0)
              word_ct = 0;
          }
          else if (ch == ',' || ch == ')' /* rparen */) 
				  { if (state & M_AFTER_NM0)
	    			{ loglog("ANM0");
				      return F_NF;
	    			}
	    		  if (paren_dpth * obrace_dpth < 0 &&
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
								++word_ct; 
				      else if (paren_dpth != 1)  // || word_ct <= -127)
								word_ct = M_NOGO;
	    			}
				    else
	    			{ state |= (save_a_cc & M_AFTER_CC);
				      if (--paren_dpth == 0)
	    			  { save_a_cc = 0;
				        state &= ~(M_IN_W+M_AFTER_NM+M_AFTER_NM0+M_IN_P);
								state |= M_AFTER_RP;
								if (state & M_NEED_BODY)
								{ state |= M_NEED_B2;
									rp_line = lines_left;
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
	      			if (sparen_nc < 0 || !(state & (M_NEED_BODY+M_PRE_SCT)) && !paren_nm)
	      			{ int res = word_ct > 1 ? F_SCT : F_NF;
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
	    			{ state &= ~(M_NO_BODY+M_PRE_SCT);
							word_ct = 1;
				      brace_dpth -= 1;
	    			  if (brace_dpth <= 0)
				      { state &= ~(M_IN_P+M_IN_SCT);
								brace_dpth = 0;
								// prev_block = -1;
				      }
	    			}
				    else 
	    			{ if (word_ct == 0 || brace_dpth <= 0)
								state &= ~M_PRE_SCT;
				      if (word_ct < 2)
								state &= ~M_IN_P;
				      word_ct = 0;
	    			}
	  			}
				  else if (ch == '{')
				  { if (state & M_GOT_NM)
				    { 
				      return (state & (M_NEED_BODY+M_PRE_SCT)) && 
				              ((state & M_NEED_BODY)|| !(state & M_AFTER_CC) ||
				               from_w_good_cl)                 && paren_nm <= 1
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
				  else if (dch == 0)
				  { ++cix;
				    if (ch == ':')
				    { word_ct -= 1;
				      state |= M_AFTER_CC;
				      continue;
				    }
				    state |= M_NO_BODY;
				    state &= ~M_NEED_BODY;
				    word_ct = M_NOGO; // -state;				// very negative 
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
				  else 
					{ if (ch == '=')
						{ if (state & M_AFTER_NM)
					    { if (state & M_IN_ENUM)
					        return -F_ENUM;
					      if (word_ct > 1)
					      { if (paren_dpth == 0)
					          if (sparen_nc < 0 || !((state & (M_NEED_BODY+M_PRE_SCT))||paren_nm))
					            return F_DCLINIT;
				          if (from_w_good_cl || !(state & M_AFTER_CC))
					        { if (state & M_IN_SCT)
					            return F_FLD;
					          if ((state & M_NEED_BODY) == 0 &&
							          (paren_dpth == 0 || sparen_nc < 0))
									    return F_OMETH;
						      }
					      }
						    return F_NF;
						  }
						}
						else if (state & M_AFTER_NM)
						{ loglog("ANM");
					    return F_NF;
					  }			/* transient states :*/
						word_ct = M_NOGO;
					}
          state &= ~(M_AFTER_NM+M_AFTER_NM0+M_AFTER_RP+M_AFTER_CC+M_AFTER_KW);
        }
      }
    } /* loop */
//  for(int ct = 10001; (--ct > 0 && lp->linetext[lp->linelength-1] == '\\');)
//    lp = meLineGetNext(lp);
  }}

  return 0;
}}} /* doit_forward */

#define SLEN 133
#if SLEN < NFILEN + 35
# undef SLEN
# define SLEN NFILEN+35
#endif

#define Q_NEED_RP 80
//#define Q_GOT_LP  40

				 /* result: 0 => not found, -1 => aborted, else
					    type of occurrence */
int Sinc::srchdeffile(int depth, short from_wh, const char * fname)
  	
{ char lbuf[SLEN+2];
  char mybuf[SLEN];

#if MEOPT_TYPEAH
  if (TTahead())
    return 0;
#endif
#if GOTTYPAH
  if (typahead())
    return 0;
#endif
{
	short best_nest = Sinc::best_nest;
	int bf_made = 0;
	
	WINDOW * wp = curwp;
  BUFFER * origbp = curbp;

  BUFFER * bp = fname[0] == 0 ? origbp : NULL;

  while (bp == NULL)		      /* yuk, newfile writes curbp ! */
  {	const char * fcp = (char*)flook('I', fname);
  
    if (fcp != NULL)
    {	bp = bufflink(fcp, TRUE);
			bf_made = g_bfindmade;
    }

    if (bp == NULL)
    { const char * ids = pd_incldirs;
      int idlen = ids == NULL ? 0 : strlen(ids);
      if (idlen >= SLEN-6 || Sinc::ask_type != 0 ||
					mlreply(concat(&lbuf[0], fname, TEXT224, null),
									&lbuf[idlen+1], SLEN-3-idlen) <= FALSE)
				return 0;
      if (idlen > 0)
      	strcpy(&lbuf[0], ids)[idlen] = PATHCHR;
			remallocstr(&pd_incldirs, lbuf, 0);
    }
  }

  if (bp->b_flag & MDSRCHC)
    return 0;

{ Paren_t sparen = g_paren;
  pd_discmd -= 2;
//pd_discmd = 0;
  swbuffer(bp);
  pd_discmd += 2;
  g_paren = sparen;
}

  if (Sinc::ask_type == '1')
  { /*setcline();*/
    return 1;
  }

{ char gs_buf [NSTRING];
  char ch;
  int word_ct = 2;
  int sword_ct = 2;
  int good_fld = false;
//int line_ct = 0;
  int got_prochead = 0;											// detects ( ) { 
	Lpos_t ext_c;
  Lpos_t cand;
	ext_c.curline = NULL;
	cand.curline = NULL;

{ Lpos_t save = *(Lpos_t*)&wp->w_dotp;
  bp->b_flag |= MDSRCHC;

{ int rcc;
	LINE * lp;
	LINE * prev = lback(&bp->b_baseline);

  if (fname[0] == 0)
  	prev = lback(save.curline);
  
  for (rcc = 0; best_nest > 0 && !l_is_hd(prev); )
  {	lp = prev;
  { int cplen = lused(lp->l_dcr);
    prev = lback(lp);
      
    if (/*prev->length > 0 && */ prev->linetext[lused(prev->l_dcr)-1] == CHQ_ESC)
      continue;

//  if (cplen == 0)
//    continue;

    if (cplen > SLEN)
      cplen = SLEN;

//++line_ct;
  {//char * cpstt = &lp->linetext[0];
#define cpstt (lp->linetext)
//  char * cplim = &cpstt[cplen - 1];	// inclusive
		int cix = find_nch(-1, -1, lp);		// skipspaces
		if (cix < 0)
			continue;
    cplen -= cix;
	  
    if (lp->l_text[cix] == '#')
		{	cix = find_nch(-1, cix, lp);		// skipspaces(cp+1, cplim);

      if (strxct_mtch(0, cix, lp) >= 0 &&	 					// define, include
          (g_paren.in_mode & (Q_IN_STR+Q_IN_CHAR+Q_IN_CMT+Q_IN_EOL)) == 0)
      { ch = lp->l_text[cix];
        cix = find_nch(-1, cix+6, lp);
        if (cix < 0)
        	continue;
    
        if (ch == 'd')			// compare to target
				{	if (strxct_mtch(-1, cix, lp) >= 0 && Sinc::g_outbuffer == NULL)
				  { cand.curline = lp;
						cand.curoff = cix;
				    rcc = 100;
				    best_nest = -1;
				  }
        }
        else if (rcc == 0)
				{ char q = lp->l_text[cix];
					if (q == '<')
						q = '>';
					int t = find_nch(q, cix, lp);
					if (t < 0)
						continue;
#define fnb mybuf
				  strpcpy(&fnb[0], lp->l_text+cix+1, t - cix);
				  
				{ char * fn;
				  for (fn = namelist; fn != NULL && strcmp(fnb, &fn[sizeof(char*)]);
				  		 fn = *(char**)fn)
						;
				  
				  if (fn != NULL)
				  	continue;

        	fn = (char*)meMalloc(strlen(fnb)+sizeof(char *)+2);
				  if (fn != null)
				  { *(char**)fn = namelist;
				    namelist = fn;
				  	strcpy(&fn[sizeof(char *)], fnb);
				  }

				  if (Sinc::ask_type == 0)
				  {
/*			    strcpy(&b[0], "Search at (N)");
				    b[11] = '1' + depth;
				    if (g_paren.in_mode != 0)
				      strcat(&b[0], "NP!");
*/
#define b ((char*)&gs_buf[128])

				    ch = getstring(b, 80, concat(&b[0], &fnb[0], " SP,CR,N,1,^G", null));
					  if	(ch < FALSE)		// Abort
					  { rcc = -1;
					    best_nest = -1;
					    continue;
					  }
				    if (ch > FALSE)
				    { if (!(b[0] == 0 || b[0] == ' ' || b[0] == '1'))
				    		continue;
							Sinc::ask_type = b[0];
				    }
				  }

				  /*char buff[80];
				    char spaces[] = "                    ";
				    concat(&buff[0], &spaces[20-srchdpth], "SI ", &fnb[0], null);
				    logstr(buff); */
				  rest_l_offs(&save);

				/*++srchdpth;*/
			    loglog2("srchdeffile %d %s", g_paren.nest, fnb);
				  rcc = srchdeffile(depth+1, from_wh, &fnb[0]);
				  if (rcc > 0)
				    cand.curline = NULL;
					(void)Sinc::bufappline(depth * 3, fnb);
        /*--srchdpth;*/
        }}
      }
      continue;
    }
  {	short ccontext = Sinc::ccontext;				      												
		int offs = scan_for_sl(lp);					/* deal with double slash this ' . */
		char * cp = lp->linetext + offs;
    while (--offs >= 0 && best_nest > 0)
    { ch = *--cp;
      if (g_paren.in_mode)
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
				  
          when '}':{ Lpos_t here = *(Lpos_t*)&wp->w_dotp;
          					 wp->w_dotp = lp;
          					 wp->w_doto = offs;
									 
          				 { int rc = - g_paren.nest;
          					 if (rc >= 0)
          					 { if (getfence(-1,1))
          					   { ext_c = *(Lpos_t*)&wp->w_dotp;
          					   	 int iter = 2;
          					   	 while (--iter >= 0)
          					   	 { rc = nextword(0,-1);
	          					 	   if (rc)
		          					   { int off = wp->w_doto;
	  	        					   	 LINE * tlp = wp->w_dotp;
        	  					 	   	 int wh = strxct_mtch(1, off, tlp);
          					 	   
    	      					 	   	 if (wh == 1 || wh == 0)		// "C", namespace
    	      					 	   	 	 break;
														 rc = 0;
													 }
    	      					   }
          					   }
          					 }

          					 rest_l_offs(&here);
    	      				 if (rc > 0)						// dont scan
    	      				 	 break;
	    	      			 ext_c.curline = NULL;
          				 }}	
          case '/': case '\\': 
          case '"': case '\'': 
          					goto scan;

				  when ';': ccontext = 0;
				  					word_ct = 0;
				  					got_prochead = 0;
						  		/*if (Sinc::best_nest != NO_BEST)
          					  loglog1("CBRACE %d", g_paren.nest);*/
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
						  	        if 		 (good_class == g_paren.nest)
						  	          good_class = -g_paren.nest;
						  	        else if ((good_class > 0 ?  good_class : -good_class)
						  	        					 < g_paren.nest)
	  	          					good_class = g_paren.nest;
	  	    					  //mbwrite("[from class]");
	  	      					}
	  	    					}
          when '{': word_ct = 0;
          				/*if (Sinc::best_nest != NO_BEST) 
                      loglog1("OBRACE %d", g_paren.nest);*/
										if (ext_c.curline != lp ||
												ext_c.curoff  != offs)
	            				scan_paren(ch);
      		  			/*if (g_paren.nest < 0)
                      g_paren.nest = 0;*/
               				      /* structure definitions */
							    	if (g_paren.nest <= min_paren_nest)
			    					{ min_paren_nest = g_paren.nest;
								      ccontext = Q_NEED_RP;
			    					}
																	    					/* is the candidate good? */
								    if (g_paren.nest < sparen_nest &&
								      /*g_paren.nest >= 0          && */best_nest != NO_BEST) 
		    						{ int ct;									/* look back for class or struct */
								      wp->wLine = lp;
								      wp->wOffset = offs;

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
								        wh = strxct_mtch(1, wp->wOffset, wp->wLine);
								        if (!in_range(wh, 0, 5))	// class to namespace
												  continue;

												ct = 0;			/* end of looping */
								        if      (wh == 1)	/* "C" */
												  ;
												else if (wh == 2)	/* enum */
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
												best_nest -= 1;		/* "C", enum, class, unnest */
							  	    }
							  	    if (ct == 0)
								      { // mbwrite("Lost Best");
												best_nest = NO_BEST;
												/*best_bp = NULL;*/
												cand.curline = NULL;
												rcc = 0;
								      }
								    }

				  otherwise if (ccontext == Q_NEED_RP &&
												strxct_mtch(1, offs, lp) > 5)
											got_prochead = 0;

									{	int mynest = -128;
                    if ( ! isword(cp[-1]) && isword(ch))
                    { word_ct += 1;
      	              if      (ch != *Sinc::target ||
      	            					 strxct_mtch(-1, offs, lp) < 0)
				                ; 							 							   /* unique code point for match */ 
								      else if (Sinc::g_outbuffer != NULL)	/* word matched */
		  					        ;
						   	      else if (srch_exact > 0 /* && fname[0] != 0 */)
		      						{
				                ch = getstring(lbuf,80,strpcpy(&lbuf[0],cpstt,cplen+1));
								        if      (ch < OK)
												  rcc = -1;
											  else if (ch > OK && lbuf[0] != 0)
												{	mynest = 0;
												  rcc = 100;
												}
											}
											else if (word_ct <= 1 /*&& line_ct > 3 */&& /*g_paren.nestclamped<=2 &&*/ 
															(ccontext <= Q_NEED_RP || 
															 got_prochead <= 0)
															&& best_nest == NO_BEST)
											{ 
											  sparen_nest = g_paren.nest;
												sparen_nc = g_paren.nestclamped;
//											sprintf(buf, "Try %d %d %50.50s", sparen_nest, sparen_nc, cp);
//											mbwrite(buf);
											{ int wh = doit_forward(lp, cp, from_wh + good_class, sparen_nc);
								        if (wh != F_NF &&
								        		( sparen_nc <= 0 ||
								        			sparen_nc == 1 && wh < 0))
											  {	rcc = wh;
											  	mynest = sparen_nc;
													if (mynest == 0 && (from_wh & 3))
												  {//mbwrite("Equal nest ff");
												    ++mynest;
			   									}
												{ char * tp; 
//			   								char buff[50];
												  for (tp = cp; --tp > cpstt+1 && *tp <= ' '; )
												    ;
//			   								sprintf(buff, "BN %d %20.20s", best_nest, tp); mbwrite(buff);
												  if (tp[0] == ':' && tp[-1] == ':')
												    good_fld = true;
		        						}}
		        						init_paren("}", 0);
	      								g_paren.nest = sparen_nest;
												g_paren.nestclamped = sparen_nc;
								      }}
		    						}
		    						if (mynest > -128)
										{ best_nest = mynest;
		    							cand.curline = lp;
											cand.curoff = offs;
										}

				            goto scan;
				          }
				}
      }  
    } /* for (over the line) */
    Sinc::ccontext = ccontext;

    scan_paren('\n');
    if (cand.curline != NULL && from_wh && !(good_fld+good_class) 
    												 && best_nest != 1 && rcc != 100)
    { cand.curline = NULL;
      rcc = 0;
      best_nest = NO_BEST;
    }
  }}}} /* for "lines" */

  bp->b_flag &= ~MDSRCHC;
 
  if (cand.curline != NULL && Sinc::best_bp == NULL/*&& best_nest<=0)*/)
  { loglog2("SETTING doto %d bn %d", cand.curoff, best_nest);
    Sinc::best_bp = bp;
    bf_made = false;
    wp->w_flag |= WFMOVE;
	  setcline();
    rest_l_offs(&cand);
  }
  else
  { rest_l_offs(&save);
  }
  
  if (bp != origbp)
  { swbuffer(EXTRA_SW_ARG(wp) origbp);
		if (bf_made)
      zotbuf(bp EXTRA_ZOT_ARG);
  }

	Sinc::best_nest = best_nest;
  return rcc;
}}}}}

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
  tgt_[0] = '?';
  tgt_[1] = 0;
	tgt = tgt_;
{
  LINE * lp = curwp->w_dotp;
	int from_wh = 0;

  if (f >= 0)		// Sinc::g_outbuffer == NULL
  { int ix = curwp->wOffset;
  	char ch;
    while (--ix > 0)
    { ch = lp->linetext[ix];
    	if (ch == ':' && lp->linetext[ix-1] == ':')
     		from_wh = 2;
     	if (ch == '>' && lp->linetext[ix-1] == '-' ||
		  	  ch == '.')
		  	from_wh = 1;
			if (ch > ' ')
				break;
		}
		to_kill_buff(-2, 0);
		tgt = getkill();
  }
  Sinc::target = (const char*)tgt;
  Sinc::srch_exact = n - 1;
  Sinc::good_class = 0;
  Sinc::ccontext = 0;
	Sinc::ccont_need_eq = 0;
  Sinc::ask_type = 0;
  Sinc::min_paren_nest = 0;
  Sinc::best_bp = NULL;
  Sinc::best_nest = NO_BEST;

  init_paren("}", -1);						// set nest and nestclamped to 0
//g_paren.nest = 0;
//g_paren.nestclamped = 0;				// <= 0 => visible

  /* namelist = null;*/
  /* srchdpth = 0;*/
  
  setmark(-1,-1);		// set the highest mark

  int cc = Sinc::srchdeffile(0, from_wh, "");

  if (f >= 0)		// Sinc::g_outbuffer == NULL)
  { if (cc > 0)
    {
#if S_WIN32 == 0
    	pd_sgarbf = TRUE;
#endif
    	mlwrite(TEXT64, cc);
						/* "Found typ %d" */
      swbuffer(Sinc::best_bp);
    }
    else
    {
      mlwrite(
 #if GOTTYPAH && 0
              typahead() ? TEXT220 : 
 #endif
											     TEXT85, tgt);
      curwp->w_flag &= ~(WFFORCE|WFMODE|WFHARD);	     /* stay put */
    }
  }

	for ( lp = (meLine*)Sinc::namelist; lp != null; lp = (meLine*)Sinc::namelist)
  { Sinc::namelist = *(char**)lp;
    free(reinterpret_cast<char*>(lp));
  }

  return cc != 0;
}}



int Pascal getIncls(int f, int n)

{ char bi_nm[20];
  int at_end = curwp->w_dotp - &curbp->b_baseline;
	if (at_end == 0)
		curwp->w_dotp = curwp->w_dotp->l_bp;
	
  BUFFER * bp = bfind(strcpy(bi_nm, "file incls"), TRUE);
  if (bp == NULL)
    return 0;

	Sinc::g_outbuffer = bp;

{	Cc cc = searchIncls(-1, n);

  if (bp->b_baseline.l_fp == &bp->b_baseline)
  	linstr(TEXT79);		/* "Not found" */

  bp->b_flag |= BFACTIVE;			/* code added */
	bp->b_fname = strdup(bi_nm+5);	// allow the leakage
  swbuffer(bp);

  Sinc::g_outbuffer = NULL;
  gotobob(0,0);

  return cc;
}}

}

#endif
