/* The functions in this file implement commands that search in the forward
 * and backward directions.
 *
 * Aug. 1986 John M. Gamble:
 *	Made forward and reverse search use the same scan routine.
 *
 *	Added a number of regular expressions - 'any', 'character class',
 *	'closure', 'beginning of line', and 'end of line'.
 *
 *	Replacement metacharacters will have to wait for a re-write of
 *	the replaces function, and a new variation of ldelchrs().
 *
 *	For those curious as to my references, i made use of
 *	Kernighan & Plauger's "Software Tools."
 *	I deliberately did not look at any published grep or editor
 *	source (aside from this one) for inspiration.  I did make use of
 *	Allen Hollub's bitmap routines as published in Doctor Dobb's Journal,
 *	June, 1985 and modified them for the limited needs of character class
 *	matching. 
 *
 *	4 November 1987 Geoff Gibbs
 *
 *	Fast version using simplified version of Boyer and Moore
 *	Software-Practice and Experience, vol 10, 501-506 (1980).
 *	Mods to scanner() and readpattern(), and added fbound() and
 *	setjtable().  Scanner() should be callable as before, provided
 *	setjtable() has been called first.
 *
 *	December 1987: John M. Gamble
 *	Made replaces() beep at you and stop if an empty string is replaced.
 *      This is possible in MAGIC mode, if you pick your pattern incorrectly.
 *	Fixed a subtle bug in the new Boyer-Moore routines, which caused
 *	searches to fail occasionally.
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"base.h"

extern int inhibit_scan;		/* also used by replace */

int Pascal delins(int, char *, int);


#define CHQ_ESC '\\'
#define CHQ_DQUOTE '"'
#define CHQ_QUOTE '\''
#define CHQ_BELL '\007'


#define DIFCASE  ('a'-'A')

       char deltaf[NSTRING+2];		/* available for sharing */

/*
 * Defines for the metacharacters in the regular expression
 * search routines.
 */
#define NOT_PAT		16
#define MASKPAT		(NOT_PAT-1)
#define	CLOSURE		32    /* An or-able value.*/
#define	MASKCL		(CLOSURE-1)

#define MCNIL	0	/* Like the '\0' for strings.*/
#define	LITCHAR	1	/* Literal character, or string.*/
#define	ANY	2
#define	CCL	3
#define	NCCL	(NOT_PAT+CCL)
#define	BOL	4
#define	EOL	5
#define	DITTO	6

#define MC_ANY		'.'	/* 'Any' character (except newline).*/
#define	MC_CCL		'['	/* Character class.*/
#define	MC_NCCL		'^'	/* Negate character class.*/
#define	MC_RCCL		'-'	/* Range in character class.*/
#define	MC_ECCL		']'	/* End of character class.*/
#define	MC_BOL		'^'	/* Beginning of line.*/
#define	MC_EOL		'$'	/* End of line.*/
#define	MC_CLOSURE	'*'	/* Closure - does not extend past newline.*/
#define	MC_DITTO	'&'	/* Use matched string in replacement.*/
#define	MC_ESC		'\\'	/* Escape - suppress meta-meaning.*/

#define BIT(n)		(1 << (n))	/* An integer with one bit set.*/

/* Typedefs that define the bitmap type for searching (BITMAP),
 * and the meta-character structure for MAGIC mode searching (MC).
 */

typedef char BITMAP;

typedef struct
{	char   mc_type;
	char   lchar;
} MC;

NOSHARE char * cclarray[NPAT / 16 /* say */ ];
NOSHARE int cclarr_ix = 0;
NOSHARE MC pats[2][NPAT+2];
#define mcpat pats[0]		/* the magic pattern		*/
#define tapcm pats[1]		/* the reversed magic pattern	*/

NOSHARE unsigned int Dmatchlen = 0;
NOSHARE unsigned int Dmlenold  = 0;


/* nextch -- retrieve the next/previous character in the buffer,
 *					 and advance/retreat the point.
 *      		 The asymmetry of forward and back is mysterious!!
 */
static int Pascal nextch(Lpos_t * lpos, int dir)
{
	   int	c = '\n';
  register LINE	*curline = lpos->curline;
  register int	curoff = lpos->curoff;

  if (dir == FORWARD)
  { if (curoff != llength(curline)) 	     /* if at EOL */
      c = lgetc(curline, curoff++);	/* get the char */
    else
    { curline = lforw(curline);	/* skip to next line */
      curoff = 0;
      lpos->line_no += 1;
    }
  }
  else		       /* Reverse.*/
  { if (curoff != 0)			/* use != -1 if wanting 16 bits */
      c = lgetc(curline, --curoff);
    else
    { curline = lback(curline);
      curoff = llength(curline);
      lpos->line_no -= 1;
    }
  }
  lpos->curline = curline;
  lpos->curoff = curoff;
  return c;
}


#define ERR_SYN   (-1)
#define ERR_OOMEM (-2)

/* cclmake -- create the bitmap for the character class.
 *	patix is left pointing to the end-of-character-class character,
 *	so that a loop may automatically increment with safety.
 *	uses pat[].
 */
static int Pascal cclmake(int patix, MC * mcptr)
	
{
	BITMAP  * p_bmap;
	
        register int to = cclarr_ix;
	if (to >= sizeof(cclarray)/sizeof(char*) || 
	    (p_bmap = aalloc(HICHAR >> 3)) == NULL)
	{ 
	  return ERR_OOMEM;
	}

	cclarray[ to ] = p_bmap;
	mcptr->lchar = to;
	cclarr_ix = to + 1;
				/* Test the initial character(s) in ccl for
				 * special cases - negate ccl, or an end ccl
				 * character as a first character.  Anything
				 * else gets set in the bitmap. */
	mcptr->mc_type = CCL;

{	register int from = -1;

	for ( ; (to = pat[++patix]) != MC_ECCL && to != 0; )
	{
	  if      (to == MC_RCCL && from >= 1)
	  { to = pat[++patix];
	    if (to == MC_ECCL)
	    { to = MC_RCCL;
	      from = to;
	    }
	  }
	  else if (to == MC_NCCL && from < 0)
	  { mcptr->mc_type = NCCL;
	    ++from;
	    continue;
	  }
	  else if (to == MC_ESC)
	    to = pat[++patix];
	  else
	    from = to;

	  for ( --from ; ++from <= to; )
	    p_bmap[from >> 3] |= (1 << (from & 7));
	}

        if (from < 0 || to == 0)
	{ 
	  mlwrite(TEXT97);
             /*	 "%%Missing ]" */
	/*free(p_bmap);
	  --cclarr_ix;*/
          return ERR_SYN;
	}
	return patix;
}}


int Pascal get_hex2(int * res_ref, char * s)
	
{ register  char res = 0;
  register int adv = -1;
  
  while (++adv < 2)
  { char chr = *s++ -'0';
    if      (in_range(chr, 0, 9))
      ;
    else if (in_range(chr, 'a' - '0', 'f' - '0'))
      chr -= 'a' - '0' + 10;
    else
      break;
    res = (res << 4) + chr;
  }
  *res_ref = res;
  return adv;
}


/*
 * mcclear -- Free up any CCL bitmaps, and MCNIL the MC search arrays.
 */
void Pascal mcclear()

{	while (--cclarr_ix >= 0)
	  free(cclarray[cclarr_ix]);

	cclarr_ix = 0;
	mcpat[0].mc_type = tapcm[0].mc_type = MCNIL;
}


/* mcstr -- Set up the 'magic' array.  The closure symbol is taken as
 *	a literal character when (1) it is the first character in the
 *	pattern, and (2) when preceded by a symbol that does not allow
 *	closure, such as a newline, beginning of line symbol, or another
 *	closure symbol.
 */
int Pascal mcstr(int mode)

{	  MC *  mcptr = &mcpat[0];
	  int   magical = FALSE;
 register int   patix;
	  int	pchr;
#define DOES_CL 2

	mcpat[NPAT].mc_type = -1;	/* a barrier */

			    /* If we had metacharacters in the MC array previously,
			     * free up any bitmaps that may have been allocated, and
			     * reset magical.
			     */
	mcclear();

	for (patix = -1; (pchr = pat[++patix]) != 0; )
	{
		if ((mode & MDMAGIC) == 0)
		  goto litcase;

	  switch (pchr)
	  {
		  case MC_CCL:	patix = cclmake(patix, mcptr);
				magical = DOES_CL | 1;
		  
		  when MC_BOL:	if (mcptr != &mcpat[0])
				  goto litcase;

				mcptr->mc_type = BOL;
				magical = TRUE;
			        
		  when MC_EOL:  if (pat[patix + 1] != '\0')
				  goto litcase;

				mcptr->mc_type = EOL;
				magical = TRUE;
			        
		  when MC_ANY:	mcptr->mc_type = ANY;
				magical = DOES_CL | 1;
			        
		  when MC_CLOSURE:
				/* Does the closure symbol mean closure here?
				 * If so, back up to the previous element
				 * and indicate it is enclosed.
				 */
				if ((magical & DOES_CL) == 0)
				  goto litcase;
				mcptr--;
				mcptr->mc_type |= CLOSURE;
				magical = TRUE;

		  when MC_ESC:	pchr = pat[patix+1];
				if (pchr == 0)
				  pchr = MC_ESC;
				else
				{ ++patix;
		                  if (pchr == '0') 
		              	    patix += get_hex2(&pchr, &pat[patix+1]);
				  magical = TRUE;
				}
		  default:				/* drop through */
litcase:mcptr->mc_type = LITCHAR;
				mcptr->lchar   = pchr;
				magical &= 1;
				if (pchr != '\n')
				  magical |= DOES_CL;
		}
		mcptr++;
	 	if (mcptr->mc_type < 0)
		  patix = ERR_OOMEM;

	  if (patix < 0)
	 	  break;
	}	     /* End of while.*/

		/* Set up the reverse array, if the status is good.  Please note
		 * the structure assignment - your compiler may not like that.
		 * If the status is not good, nil out the meta-pattern.
		 * The only way the status would be bad is from the cclmake()
		 * routine, and the bitmap for that member is guarenteed to be
		 * freed.  So we stomp a MCNIL value there, and call mcclear()
		 * to free any other bitmaps.
		 */
	if (patix < 0)
	{ if (patix == ERR_OOMEM)
	    mlwrite(TEXT99); /* Out of Memory */

	  mcclear();
	  return false;
	}
	else
	{ mcptr->mc_type = MCNIL; 	/* Close off the meta-string */

	  patix = mcptr - &mcpat[0];
	  tapcm[patix].mc_type = MCNIL;
	  pchr = -1;
        
	  for ( ; --patix >= 0; )
	    tapcm[patix] = mcpat[++pchr];
	  return true;
	}
}



/* readpattern -- Read a pattern.  Stash it in apat.  If it is the
 *	search string, create the reverse pattern and the magic
 *	pattern, assuming we are in MAGIC mode (and defined that way).
 *	Apat is not updated if the user types in an empty line.  If
 *	the user typed an empty line, and there is no old pattern, it is
 *	an error.  Display the old pattern, in the style of Jeff Lomicka.
 *	There is some do-it-yourself control expansion.  Change to using
 *	<META> to delimit the end-of-pattern to allow <NL>s in the search
 *	string. 
 */

/*static const Char * rp_prompts[] = {TEXT80, TEXT81, TEXT84, TEXT85, TEXT86};*/

//static
int Pascal readpattern(char * prompt, char apat[])
	
{   register int cc;
    char tpat[NSTRING+20];	
    
    concat(&tpat[0], prompt, " [", null);
    cc = strlen(tpat); 
							/* add old pattern */
    expandp(&tpat[cc], strlen(apat) < 30 ? apat : "\"", "] ", NSTRING+16-cc);

	     /* Read a pattern.  Either we get one,
	      * or we just get the META charater, and use the previous pattern.
	      * Then, if it's the search string, make a reversed pattern.
	      * *Then*, make the meta-pattern, if we are defined that way.
	      */ 
    cc = mltreply(tpat, tpat, NSTRING);
    if (cc != ABORT)
    { extern int gs_keyct;		 /* from getstring */
      if (cc || gs_keyct > 0)
      { /*cc = TRUE;*/
        strcpy(apat, tpat);
      }

/*    if (curwp->w_bufp->b_flag & MDMAGIC) */
      cc = TRUE;
      if (apat == pat)
        cc = mcstr(curwp->w_bufp->b_flag);
/*    else
        mcclear();*/
    }

    return cc;
}

int last_was_srch()

{ return lastfnc == forwsearch || lastfnc == backsearch ||
         lastfnc == backhunt   || lastfnc == forwhunt;
}


int Pascal hunt(int n, int again)

{		 int dir = FORWARD;
	register int cc;
								/* Make sure a pattern exists, or that we didn't switch
								 * into MAGIC mode until after we entered the pattern.
								 */
//paren.sdir = 1;

  if (kbd_record(kbdmode))
    g_got_search = TRUE;

  if (pat[0] == '\0')
	{ mlwrite(TEXT78);
/*			"No pattern set" */
	  return FALSE;
	}

	if (n < 0)
	{ n = -n;
	  dir = REVERSE;
//	  paren.sdir = -1;
	}

	if (/*(mdexact & MDMAGIC) && */pats[dir][0].mc_type == MCNIL)
	{ if (!mcstr(curwp->w_bufp->b_flag))
	    return FALSE;
	}

  if (dir == FORWARD && (curwp->w_bufp->b_flag & MDSRCHC) == 0)
    dir = -1;
      
  lastdir = (-2*dir)+1;

	while ((cc = scanner(dir, again)) && --n > 0)
	  again |= true;
			   /* Save away the match, or complain if not there. */
	if (cc != TRUE)
	  mlwrite(TEXT79);
/*			"Not found " */
	return cc;
}

/*
 * forwhunt -- Search forward for a previously acquired search string.
 *	If found, reset the "." to be just after the match string,
 *	and (perhaps) repaint the display.
 */
Pascal forwhunt(int f, int n)

{ 
	return hunt(n, TRUE);
}

/*
 * backhunt -- Reverse search for a previously acquired search string,
 *	starting at "." and proceeding toward the front of the buffer.
 *	If found "." is left pointing at the first character of the pattern
 *	(the last character that was matched).
 */
Pascal backhunt(int f, int n)

{	
	return hunt(n <= 0 ? -1 : -n, TRUE);
}


/* forwsearch -- Search forward.  Get a search string from the user, and
 *	search for the string.	If found, reset the "." to be just after
 *	the match string, and (perhaps) repaint the display.
 */
Pascal forwsearch(int f, int n)

{
	/* Ask the user for the text of a pattern.  If the
	 * response is TRUE (responses other than FALSE are possible), 
	 * search for the pattern for as long as n is positive
	 * (n == 0 will go through once, which is just fine).
	 */
	
	int cc = readpattern(n < 0 ? TEXT81 : TEXT80, &pat[0]);
/*				  "Search" */
	if (cc == TRUE)
	  cc = hunt(n, FALSE);
	return cc;
}


/* backsearch -- Reverse search.  Get a search string from the user, and
 *	search, starting at "." and proceeding toward the front of the buffer.
 *	If found "." is left pointing at the first character of the pattern
 *	(the last character that was matched).
 */
Pascal backsearch(int f, int n)

{
	if (n == 0)
	  n = 1;
	return forwsearch(f, -n);
}

/*
 * eq -- Compare two characters.  The "bc" comes from the buffer, "pc"
 *	from the pattern.  If we are not in EXACT mode, fold out the case.
 */
int Pascal myeq(int a, int b)

{
  return a==b ||
        (curwp->w_bufp->b_flag & MDEXACT) == 0 && isletter(a)
	   && ((a ^ b) & ~0x20) == 0;
}
 
 
#define eq(a, b) (((a ^ b) & ~0x20) == 0 && myeq(a,b))


/*
 * biteq -- is the character in the bitmap?
 */
static int Pascal biteq(int bc, int ix)
	
{
  return /*bc <= HICHAR && */(cclarray[ix][bc >> 3] >> (bc & 7)) & 1;
}

/* mceq -- meta-character equality with a character.  In Kernighan & Plauger's
 *	Software Tools, this is the function omatch(), but i felt there
 *	were too many functions with the 'match' name already.
 */
static int Pascal mceq(int bc, MC * mt)
{
	register int result;

	switch (mt->mc_type & MASKPAT)
	{ case LITCHAR: if (eq(bc, (int)mt->lchar))
	                  return true;

	  when ANY:	if (bc != '\n')
	                  return true;

	  when CCL:	result = biteq(bc, mt->lchar);
			if (!result)
			  if ((curwp->w_bufp->b_flag & MDEXACT) == 0 && isletter(bc))
			    result = biteq(CHCASE(bc), mt->lchar);

			if (mt->mc_type & NOT_PAT)
			{ result -= 1;
			  result &= 1;
			}
	                return result;
	}

        return false;
}

/*
 * amatch -- Search for a meta-pattern in either direction.
 *      Based on the recursive routine amatch() (for "anchored match") in
 *	Kernighan & Plauger's "Software Tools". (Private to scanner)
 */
int Pascal amatch(MC * mcptr, int direct, Lpos_t * pcwlpos)
		 /* string to scan for */
	 	/* which way to go.*/
{
			/* Set up local scan pointers to ".", and get
			 * the current character.  Then loop around
			 * the pattern pointer until success or failure. */
        Lpos_t lpos = *pcwlpos;
			/* The beginning-of-line and end-of-line metacharacters
			 * do not compare against characters, they compare
			 * against positions.
			 * BOL is guaranteed to be at the start of the pattern
			 * for forward searches, and at the end of the pattern
			 * for reverse searches.  The reverse is true for EOL.
			 * So, for a start, we check for them on entry.
			 */
	register int type;
        for (; (type = mcptr->mc_type) != MCNIL; mcptr++)
	{
		        /* The only way we'd get a BOL metacharacter here
			 * is at the end of the reversed pattern.
			 * The only way we'd get an EOL metacharacter here 
			 * is at the end of a regular pattern.
			 * So if we match one or the other, and are at
			 * the appropriate position, we are guaranteed success
			 * (since the next pattern character has to be MCNIL).
			 * Before we report success, however, we back up by
			 * one character, so as to leave the cursor in the
			 * correct position.  For example, a search for ")$"
			 * will leave the cursor at the end of the line, while
			 * a search for ")<NL>" will leave the cursor at the
			 * beginning of the next line. Thus the meta-characters
			 * '$', and '^' match positions, not characters.
			 */
	  if       (type == BOL)
	  { if (lpos.curoff != 0)
	      return FALSE;
          }
	  else if (type == EOL)
	  { if (lpos.curoff != llength(lpos.curline))
	      return FALSE;
          }
          else if (type == LITCHAR)
	  { type = nextch(&lpos, direct);
	    if (! eq(type, (int)mcptr->lchar))
	      return FALSE;
	    Dmatchlen++;
	  }
	  else if ((type & CLOSURE) == 0)
	  { if (!mceq(nextch(&lpos, direct), mcptr))
	      return FALSE;
	    Dmatchlen++;
	  }
	  else
	  {		/* Try to match as many characters as possible
			 * against the current meta-character.	A
			 * newline never matches a closure. */
	    type = 0;
	    while (mceq(nextch(&lpos, direct), mcptr))
	      type++;
			/* We are now at the character that made us fail.
			 * Try to match the rest of the pattern, shrinking
			 * the closure by one for each failure.
			 * Since closure matches *zero* or more occurences
			 * of a pattern, a match may start even if the
			 * previous loop matched no characters. */
	    for (;;--type)
	    { nextch(&lpos, direct ^ REVERSE);

	      if (amatch(mcptr+1, direct, &lpos))
	      { Dmatchlen += type;
	        goto success;
	      }

	      if (type == 0)
	        return FALSE;
	    }
	  }
	}		    /* End of mcptr loop.*/

					/* A SUCCESSFULL MATCH!!!  */
success:*pcwlpos = lpos;		/* Reset the "." pointers. */
	return TRUE;
}

void rest_l_offs(Lpos_t* lpos)

{ *(Lpos_t*)&curwp->w_dotp = *lpos;
}

#if 0
/*
 * savematch -- We found the pattern?  Let's save it away.
 */
Pascal savematch()

{	register char	*ptr;		/* pointer to last match string */
	register int	j;

	/* Free any existing match string, then
	 * attempt to allocate a new one.
	 */
}
#endif

/*
 * scanner -- Search for a pattern in either direction.  If found,
 *	reset the "." to be at the start or just after the match string,
 *	and (perhaps) repaint the display.
 *	Fast version using simplified version of Boyer and Moore
 *	Software-Practice and Experience, vol 10, 501-506 (1980)
 */
int Pascal scanner(int direct, int again)
	/* int	direct; 	** which way to go.*/
{ Lpos_t lpos = *(Lpos_t*)&curwp->w_dotp;
  Lpos_t sm = lpos;			/* match line and offs */
  int skip = 0;
  if (direct < FORWARD)
  { direct = FORWARD;
    skip = 1;
  }

  init_paren("",0);
  
  if (again != 0 ? direct <= FORWARD : (lpos.curline->l_props & L_IS_HD))
    nextch(&lpos, direct);    /* Advance the cursor.*/
                              /* Save the old Dmatchlen length, in case it is
                               * very different (closure) from the old length.
                               * Important for the query-replace undo command. */
{ register int ch;
  LINE * bl = curbp->b_baseline;
  MC *         mcpatrn = &pats[direct][0];
  Dmlenold = Dmatchlen;

		 /* Scan each character until we hit the head link record. */
  while (true)
  { /*if (direct == FORWARD ? lpos.curoff == llength(lpos.curline) &&
				(lforw(lpos.curline)->l_props & L_IS_HD)
		 	  :  ** lpos.curoff == 0	&& */
			     /* lpos.curline  == lforw(curbp->b_baseline) && */
    if (lpos.curline == bl)
      return false;
   
	   /* Save the current position in case we need to
	     * restore it on a match, and initialize Dmatchlen to
	     * zero in case we are doing a search for replacement.
	     */
		/* This code is here as an optimisation */
    if      (mcpatrn->mc_type == LITCHAR)
    { ch = nextch(&lpos, direct);  /* Advance the cursor.*/
      if (skip)
		    scan_paren(ch);

      if (! eq(ch, (int)mcpatrn->lchar))
        continue;
      sm = lpos;
      Dmatchlen = 1;
      if (! amatch(mcpatrn+1, direct, &lpos))
        continue;

			if (skip && (paren.in_mode & Q_IN_CMT) != 0)
				continue;

      nextch(&sm, direct ^ REVERSE);  /* restore the cursor.*/
      break;
    }
    else if (mcpatrn->mc_type == BOL && lpos.curoff != 0)
    { if (direct == FORWARD) 
        lpos.curoff = llength(lpos.curline);
      else
        lpos.curoff = 1;
    }
    else if (mcpatrn->mc_type == EOL && lpos.curoff != llength(lpos.curline))
      ;
    else
    { sm = lpos;
      Dmatchlen = 0;
//  	if (skip && (paren.in_mode & Q_IN_CMT) != 0)
//			continue;
      if (amatch(mcpatrn, direct, &lpos))
        break;
    }
		if (skip == 0)
      nextch(&lpos, direct);  /* Advance the cursor.*/
		else
		  scan_paren(nextch(&lpos, direct));
  }
		   
  if (direct == FORWARD)     /* at beginning of string */
    lpos = sm;

  rest_l_offs(&lpos);
  curwp->w_flag |= WFMOVE; /* flag that we have moved */

  if (patmatch != NULL)
    free(patmatch);

  patmatch = (char*)malloc(Dmatchlen+1);

{ char * patptr = patmatch;
  if (patptr != NULL)
  {
    for (ch = Dmatchlen; --ch >= 0; )
      *patptr++ = nextch(&sm, FORWARD);

    *patptr = '\0';
  }

  return true;				/* We found a match */
}}}

/*
 * expandp -- Expand control key sequences for output.
 */
void Pascal expandp(char * deststr, char * srcstr, char * tailstr, int maxlength)
	/* char	*deststr;	** destination of expanded string (concat) */
	/* char	*srcstr;	** string to expand */
	/* char	*tailstr;	** string on the end */
	/* int	maxlength;	** maximum chars in destination */
{
        static const char strs[] = "<CR\000^\000%\000";
	register int dix = strlen(deststr) - 1;
	register unsigned char c;
					     /* Scan through the string */
	while ((c = *srcstr++) != 0)
	{	int insix = 3;

		if      (c == '\r')	       /* it's a newline */
		{
			insix = 0;
			c = '>';
		}
		else if (c < 0x20 || c == 0x7f)       /* control character */
		{
			insix = 4;
			c ^= 0x40;
		}
		else if (c == '%')
			insix = 6;
			
		for ( ; strs[insix] != 0; ++insix)
		  deststr[++dix] = strs[insix];
		  

		deststr[++dix] = c;
					      /* check for maxlength */
		if (dix >= maxlength)
		{	dix = maxlength-1;
			deststr[dix] = '$';
			break;
		}
	}
	strcpy(&deststr[++dix], tailstr);
}

/*
 * sreplace -- Search and replace.
 */
Pascal sreplace(int f, int n)

{
  return replaces(FALSE, f, n);
}

/*
 * qreplace -- search and replace with query.
 */
Pascal qreplace(int f, int n)

{
  return replaces(TRUE, f, n);
}


/* replaces -- Search for a string and replace it with another
 *	string.  Query might be enabled (according to kind).
 */
int Pascal replaces(int kind, int f, int n)
	/* int	kind;	* Query enabled flag */
	/* int	f;	    * default flag */
	/* int	n;	    * # of repetitions wanted */
{
	register int cc;	/* success flag on pattern inputs */
	char tpat[NPAT];	/* temporary to hold search pattern */

	if (curbp->b_flag & MDVIEW)
	  return rdonly();

	if (f && n < 0) 		  /* Check for negative repetitions */
	  return FALSE;
				/* Ask the user for the text of a pattern.
				 */
	cc = readpattern(kind == FALSE ? TEXT84 : TEXT85, &pat[0]);
/*			     "Replace" */
/*			     "Query replace" */
	if (cc != TRUE)
	  return cc;
					   /* Ask for the replacement string. */
	cc = readpattern(TEXT86, &rpat[0]);
/*				  "with" */
	if (cc == ABORT)
	  return cc;
				/* Find the length of the replacement string. */
{ 	int inhib; 
				  /* Set up flags so we can make sure not to do 
				   * a recursive replace on the last line. */
	int nlrepl = FALSE;
  	int numsub = 0;
  	int numleft = n;

	Lpos_t orig = *(Lpos_t*)&curwp->w_dotp;
	Lpos_t last;		/* position of last replace */

        orig.curline = lback(orig.curline);
	last.curline = NULL;

	strcpy(tpat, TEXT87);       /* Build query replace question string */
/*			     "Replace '" */
	expandp(&tpat[0], &pat[0], TEXT88, NPAT-9);
/*			     "' with '" */
	expandp(&tpat[0], &rpat[0], "'? ", NPAT-4);

	while ((! f || --numleft >= 0) && ! nlrepl && scanner(FORWARD, FALSE))
	{			     /* Search for the pattern.
				      * If we search with a regular expression,
				      * Dmatchlen is reset to the true length of
				      * the matched string. */

					/* Check if we are on the last line */
	  if ((lforw(curwp->w_dotp)->l_props & L_IS_HD) &&
              Dmatchlen > 0 &&
	      pat[Dmatchlen - 1] == '\n')
	    nlrepl = true;

	  if (kind)			/* Check for query */
	  {	if (getwpos() + 2 >= curwp->w_ntrows)
		{ Lpos_t save = *(Lpos_t*)&curwp->w_dotp;
		  forwline(f, 2);
		  rest_l_offs(&save);
		}
	  					/* Get the query  */
pprompt:	mlwrite(&tpat[0], &pat[0], &rpat[0]);
qprompt:
		update(TRUE);	/* show the proposed place to change */
		cc = getkey();
             /* mlwrite("");			** and clear it */

		switch (toupper(cc))		/* And respond appropriately */
		{ case 'L':   nlrepl = true;
#if	FRENCH
		  case 'O':			/* yes, substitute */
#endif
		  case 'Y':			/* yes, substitute */
		  case ' ':

		  when '!':	kind = FALSE;	/* yes/stop asking */
				

		  when 'N':	forwchar(FALSE, 1);	/* no, onword */
				continue;

		  when 'U':		   /* undo last and re-prompt */
				if (last.curline == NULL)
						 /* There is nothing to undo.*/
				  TTbeep();
				else		    /* Restore old position. */
		  		{ rest_l_offs(&last);
				  last.curline = NULL;
						    /* Delete the new string. */
				  cc = strlen(&rpat[0]);
				  backchar(FALSE, cc);
				  cc = delins(cc, patmatch, FALSE);
				  if (cc != TRUE)
				    return cc;
					       /* Record one less substitution,
						* backup, save our place, and
						* reprompt. */
				  --numsub;
				  backchar(FALSE, Dmlenold);
				}
				goto pprompt;
		  otherwise			   /* bitch and beep */
				tcapbeep();
		  case '?':	mlwrite(TEXT90);	  /* help me */
/*"(Y)es, (N)o, (!)Do rest, (U)ndo last, (^G)Abort, (.)Abort back, (?)Help: "*/
				goto qprompt;
		  when '.':			     /* abort! and return */
						     /* restore old position */
				orig.curline = lforw(orig.curline);
				if (orig.curoff >= orig.curline->l_used)
				  orig.curoff = 0;
				rest_l_offs(&orig);
				curwp->w_flag |= WFMOVE;

		  case CTRL|'G':mlwrite(TEXT89);  /* abort! and stay */
/*						"Aborted!" */
				return FALSE;
		}       
	  }	/* end of "if kind" */

          inhib = last.curline == curwp->w_dotp;
          if (inhib)
             ++inhibit_scan;

			   /* Delete the sucker, and insert its replacement. */
	  cc = delins(Dmatchlen, &rpat[0], TRUE);
	  if (cc != TRUE)
	    return cc;
	  			  /* Save our position, since we may undo this*/
          if (inhib)
          { --inhibit_scan;
            if (last.curline != curwp->w_dotp)
              lchange(WFHARD);
	  }
	  last = *(Lpos_t*)&curwp->w_dotp;
				  /* If we are not querying, check to make sure
				   * that we didn't replace an empty string
				   * (possible in MAGIC mode), because we'll
				   * infinite loop. */
	  numsub++;	/* increment # of substitutions */
	  if      (kind)
	    update(TRUE);			  /* show the change */
	  else if (Dmatchlen == 0)
	  { mlwrite(TEXT91);
/*				"Empty string replaced, stopping." */
	    return FALSE;
	  }
	}
						/* And report the results */
	mlwrite(TEXT92, numsub);
/*		"%d substitutions" */
	return TRUE;
}}

/*
 * delins -- Delete a specified length from the current point
 *	then either insert the string directly, or make use of
 *	replacement meta-array.
 */
Pascal delins(int dlength, char * instr, int use_meta)
	
{
					       /* Zap what we gotta,
						* and insert its replacement. */
 char buf[120];
 register int cc;
 extern int overmode;
        int sov = overmode;

        for (cc = -1; ! use_meta && instr[++cc] != 0; )
          if (instr[cc] == '\n' || instr[cc] == MC_ESC || instr[cc] == MC_DITTO)
          { cc = -1;
            break;
          }
        overmode = cc == dlength;

        cc = TRUE;
        if (! overmode)
          cc = ldelchrs((Int)dlength, FALSE);
	if (cc != TRUE)
	  mlwrite(TEXT93);
/*			"%%ERROR while deleting" */
	else
	{ cc = -1;
	  for ( ; ; ++instr)
	  { if (instr[0] == MC_ESC && instr[1] != 0 && use_meta)
	    { int chr = *++instr;
              if (chr == '0')
	      { instr += get_hex2(&chr, instr+1);
	      }
	      buf[++cc] = chr;
	      continue;
	    }
	    if (instr[0] == MC_DITTO && use_meta || 
	        instr[0] == 0                    ||
	        cc > sizeof(buf) - 4 )
	    { buf[++cc] = 0;
	      cc = linstr(buf);
	      if (cc != TRUE || instr[0] == 0)
	        break;
	      cc = -1;
	      if (instr[0] == MC_DITTO && use_meta)
	      { cc = linstr(patmatch);
	        if (cc != TRUE)
	          break;
	        cc = -1;
	        continue;
	      }
	    }
	    
	    buf[++cc] = instr[0];
	  }
	  cc = TRUE;
	}
	  
        overmode = sov;
	return cc;
}

/*
 * wordsearch -- Reverse search for a word starting with a prefix.
 * Insert the result at .
 */
Pascal wordsearch(int f, int n)

{ Lpos_t sv = *(Lpos_t*)&curwp->w_dotp;	/* original line pointer */

  char buff[132];
  int ix = -1;
  LINE * ln = curwp->w_dotp;
  char * lp = lgets(ln, 0);
  int fin = curwp->w_doto;
  int offs =  fin;
  
  while (--offs >= 0 && 
         (isalpha(lp[offs]) || isdigit(lp[offs]) || lp[offs] == '_'))
    ;

  if (fin - offs >= sizeof(buff)) fin = offs + sizeof(buff) - 2;

  while (++offs < fin && 
         (isalpha(lp[offs]) || isdigit(lp[offs]) || lp[offs] == '_'))
  { buff[++ix] = lp[offs];
  }

  buff[++ix] = 0;

#if S_WIN32 && 0
  mbwrite(buff);
#endif
#if S_WIN32 && 0
  buff[100] = '0'+ix;
  buff[101] = 0;
  mbwrite(buff+100);
#endif

{ int ixx = ix;

  if (ix > 0)
  { 
    backword(1, 1);
    while (backword(1, 1))
    {
      int doto =curwp->w_doto;
      char * s = &curwp->w_dotp->l_text[doto];
      int len = curwp->w_dotp->l_used - doto;
      if (len >= sizeof(buff)) len = sizeof(buff) - 1;
      
      if (s[0] == buff[0])
        if (strncmp(buff,s,ix) == 0)
        { 
          while (ixx < len && 
                 (isalpha(s[ixx]) || isdigit(s[ixx]) || s[ixx] == '_'))
          { buff[ixx] = s[ixx];
            ++ixx;
          }
          
          buff[ixx] = 0;

          break;
        }
    }
  }

  rest_l_offs(&sv);
  
  if (ixx > ix)
  {
#if S_WIN32 && 0
//  mbwrite(buff);
#endif
#if S_WIN32 && 0
//  sprintf(buff+50,"del %d", fin - offs);
//  mbwrite(buff+50);
#endif
    ldelchrs(fin - offs, 0);
#if S_WIN32 && 0
    mbwrite(buff+ix);
#endif
    linstr(buff+ix);
  }

  return OK;
}}

#if 0

static int get_indt(LINE * lp)

{ char * s = lgets(lp, 0);
  int fin = lp->l_used-1;
  int ix = -1;
  register int offs = 0;
  
  while (ix < fin)
  { ix += 1;
    if      (s[ix] == ' ')
      offs += 1;
    else if (s[ix] == 9)
      offs = ((offs + curbp->b_tabsize-1)/curbp->b_tabsize)*curbp->b_tabsize;
    else
      break;
  }
  
  return offs;
}

#endif

/*
 * indentsearch -- Reverse search for an indent less than current.
 */
Pascal indentsearch(int f, int n)

{
//char buff[132];
  LINE * lp = curwp->w_dotp;

//sprintf(buff,"offs %d", offs);
//mbwrite(buff);

  char * s = lgets(lp, 0);
  int fin = lp->l_used-1;
  int ix = -1;
  int          origoffs;
  register int offs = 0;
  
  while (ix < fin)
  { ix += 1;
    if      (s[ix] == ' ')
      offs += 1;
    else if (s[ix] == 9)
      offs = ((offs + curbp->b_tabsize-1)/curbp->b_tabsize)*curbp->b_tabsize;
    else
      break;
  }

  origoffs = offs;

  if (n > 0 && origoffs > 0)
  { 
    int moved = 0;

    while (1)
    {
      lp = lback(lp);
      if ((lp->l_props & L_IS_HD) != 0) break;

      moved += 1;

      s = lgets(lp, 0);
      fin = lp->l_used-1;
      ix = -1;
      offs = 0;
      
      while (ix < fin)
      { ix += 1;
        if      (s[ix] == ' ')
          offs += 1;
        else if (s[ix] == 9)
          offs = ((offs + curbp->b_tabsize-1)/curbp->b_tabsize)*curbp->b_tabsize;
        else
          break;
      }

      if (offs < origoffs) break;
    } 

    if ((lp->l_props & L_IS_HD) != 0)
      TTbeep();
    else
    { curwp->w_dotp  = lp;
      curwp->w_doto  = offs;
      curwp->w_line_no -= moved;
      curwp->w_flag |= WFMOVE; /* flag that we have moved */
    }
  }

  return OK;
}

