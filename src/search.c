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

// extern int g_inhibit_scan;		/* also used by replace */

static int Pascal delins(int, char *, int);


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
#define	EOL			2
#define	CCL	    3
#define	NCCL (NOT_PAT+CCL)
#define	BOL	  4
#define	DITTO	6

#define MC_ANY		'.'	/* 'Any' character (except newline).*/
#define	MC_CCL		'['	/* Character class.*/
#define	MC_NCCL		'^'	/* Negate character class.*/
#define	MC_RCCL		'-'	/* Range in character class.*/
#define	MC_ECCL		']'	/* End of character class.*/
#define	MC_BOL		'^'	/* Beginning of line.*/
#define	MC_EOL		'$'	/* End of line.*/
#define	MC_CLOSURE '*'/* Closure - does not extend past newline.*/
#define	MC_DITTO	'&'	/* Use matched string in replacement.*/
#define	MC_ESC		'\\'/* Escape - suppress meta-meaning.*/

#define BIT(n)		(1 << (n))	/* An integer with one bit set.*/

/* Typedefs that define the bitmap type for searching (BITMAP),
 * and the meta-character structure for MAGIC mode searching (MC).
 */

typedef unsigned short Bitmap;

typedef struct
{	char   mc_type;
	char   lchar;
} MC;

static Bitmap * g_cclarray[NPAT / 16 /* say */ + 1];
static int g_cclarr_ix;

static MC   g_pats[NPAT+2];

//unsigned int g_Dmatchlen = 0;

#define ERR_SYN   (0)
#define ERR_OOMEM (-1)

/* cclmake -- create the bitmap for the character class.
 *	patix is left pointing to the end-of-character-class character,
 *	so that a loop may automatically increment with safety.
 *	uses pat[].
 */
static int Pascal cclmake(int patix, MC *  mcptr)
	
{ int to = ++g_cclarr_ix;
	if (to >= sizeof(g_cclarray)/sizeof(Bitmap*))
	  return ERR_OOMEM;

	mcptr->lchar = to;
	mcptr->mc_type = CCL;

{	Bitmap * p_bmap = (Bitmap *)remallocstr((char**)&g_cclarray[ to ], NULL, HICHAR >> 3);
	if (p_bmap == NULL)
	  return ERR_OOMEM;							/* Test the initial character(s) in ccl for
																	 * special cases - negate ccl, or an end ccl
																	 * character as a first character.
																	 * Anything else gets set in the bitmap. */
{	int from;
	while ((from = pat[++patix]) != 0)
	{	if (from == MC_ECCL)
			return patix;								// at least 1

		if (from == MC_NCCL)		/* '^' */
		{ mcptr->mc_type |= NOT_PAT;
			continue;
		}
		to = pat[patix + 2];
	  if (to != MC_ECCL && pat[patix + 1] == MC_RCCL)
			patix += 2;
	  else
	    to = from;

	  for ( --from ; ++from <= to; )
	    p_bmap[from >> 4] |= (1 << (from & 15));
	}
	
	return ERR_SYN;
}}}


static
int Pascal get_hex2(int * res_ref, const char * s)
	
{ int res = 0;
  int adv = -2;
  
  while (++adv <= 0)
  { signed char chr = *s++ -'0';
  	if (chr < 0)
  		break; 
    if      (chr <= 9)
      ;
    else if (in_range((chr | 0x30), 'a' - '0', 'f' - '0'))
      chr = (chr | 0x30) - 0x30 + 9;
    else
      break;
    res = (res << 4) + chr;
  }

  *res_ref = adv >= 0 ? res : s[-1];

  return adv + 2;
}

/* mcstr -- Set up the 'magic' array.  The closure symbol is taken as
 *	a literal character when (1) it is the first character in the
 *	pattern, and (2) when preceded by a symbol that does not allow
 *	closure, such as a newline, beginning of line symbol, or another
 *	closure symbol.
 */
static
int Pascal mk_magic()

{	int magic = curbp->b_flag;
	int patix = -1;
	g_cclarr_ix = -1;						/* reset magical. */
	g_pats[NPAT].mc_type = -1;	/* a barrier */

{	MC * mcptr = g_pats;
	int lchr = -1;

//mcpat[0].mc_type = MCNIL;

	while (TRUE)
	{
	 	if ((++mcptr)->mc_type < 0)
		{ patix = ERR_OOMEM;
			break;
		}

		mcptr->mc_type = LITCHAR;

		lchr = pat[++patix];
	{	int pchr = lchr;
		if (pchr == 0 /* == ERR_SYN */)
			break;

		if (magic & MDMAGIC)
			
			if      (pchr == MC_ESC)		/* \ */
				patix += get_hex2(&pchr, &pat[patix+1]);

		  else if (pchr == MC_CLOSURE && patix > 0) /* * */
			{  													/* Does the closure symbol mean closure here?
																	 * If so, back up to the previous element
																	 * and indicate it is enclosed. */
				(--mcptr)->mc_type |= CLOSURE;
				continue;
			}
			else if (pchr == MC_CCL)		/* [ */
			{	patix = cclmake(patix, mcptr);
				if (patix <= 0)
					break;	  		
				continue;
			}		
			else if (pchr == MC_ANY)		/* . */
			{ mcptr->mc_type = LITCHAR | NOT_PAT;
//		  mcptr->lchar = '\n';
			  pchr = '\n';
//			continue;
			}
			else if (lchr == pchr)
			{	if (pchr == MC_BOL)
					mcptr->mc_type = BOL;
				if (pchr == MC_EOL)
				{	pchr = '\n';
					mcptr->mc_type = EOL;
				}
			}
		
		mcptr->lchar = pchr;
	}}	/* End of while.*/

		/* Set up the reverse array, if the status is good.  Please note
		 * the structure assignment - your compiler may not like that.
		 * If the status is not good, nil out the meta-pattern.
		 * The only way the status would be bad is from the cclmake()
		 * routine, and the bitmap for that member is guarenteed to be
		 * freed.  So we stomp a MCNIL value there.
		 */
	if (patix <= 0)
		mbwrite(patix == ERR_SYN ? TEXT97		/* Out of Memory */
	  												 : TEXT99);	/* "%%Missing ]" */
	else 
	{	mcptr->mc_type = MCNIL; 	/* Close off the meta-string */

	  patix = (--mcptr) - g_pats;
  }
	return patix;
}}


/*
 * expandp -- Expand control key sequences for output.
 */
static 
char * expandp(int maxlength, char * srcstr, char * tailstr, char * deststr)
	/* char	*deststr;	** destination of expanded string (concat) */
	/* char	*srcstr;	** string to expand */
	/* char	*tailstr;	** string on the end */
	/* int	maxlength;	** maximum chars in destination */
{
//  static const char strs[] = "<CR\000^\000%\000";
	unsigned char c;
	char * d = deststr + strlen(deststr) - 1;
					     /* Scan through the string */
	while ((c = *srcstr++) != 0)
	{	char ich;

		if      (c == '\r')	       /* it's a newline */
		{
			*++d = '<';
			*++d = 'C';
			c = '>';
			ich = 'R';
		}
		else if (c < 0x20 /* || c == 0x7f */)       /* control character */
		{
			c ^= 0x40;
			ich = '^';
		}
		else if (c == '%')
			ich = '%';
		else
			goto just_c;
			
		*++d = ich;
just_c:		  
					      /* check for maxlength */
		if (d >= maxlength + deststr)
		{	d = maxlength + deststr;
			c = '$';
		}
		*++d = c;
	}

	strcpy(++d, tailstr);
	return deststr;
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

/*static const Char * rp_prompt[] = {TEXT80, TEXT81, TEXT82, TEXT83, TEXT84};*/

static
int Pascal readpattern(const char * prompt, char apat[])

{		extern int g_gs_keyct;		 /* from getstring */
		char tpat[NPAT+10];
    																				/* add old pattern */
		char * t = expandp(sizeof(tpat)-1, strlen(apat) < 30 ? apat : "\"", "] ",
																			 concat(tpat, prompt, " [", null)); 

						  							/* Read a pattern.  Either we get one, or just
			    									 * get the META charater and use the previous pat */ 
{		Cc cc = mlreply(t, t, NPAT);
    if (cc >= FALSE)
    { if (cc > FALSE || g_gs_keyct > 0)
        strcpy(apat, tpat);

      cc = TRUE;
    }

    return cc;
}}

/*
int last_was_srch()

{ return g_lastfnc == forwsearch || g_lastfnc == backsearch ||
         g_lastfnc == backhunt   || g_lastfnc == forwhunt;
}
*/ 

int Pascal USE_FAST_CALL hunt(int n, int again)

{											/* Make sure a pattern exists, or that we didn't switch
											 * into MAGIC mode until after we entered the pattern. */
	int cc = pat[0] - '\0';
	if (cc)
	{	setmark(-1,-1);											/* set the last mark */

	{	int dir = 1;
		if (n < 0)
		{ n = -n;
		  dir = -1;
		}
	  pd_lastdir = dir;
		pd_hlight1[1] = 0;
	  if ((curwp->w_bufp->b_flag & MDSRCHC) == 0)
	  	again |= 2;
	
		while (--n >= 0 && (cc = scanner(dir, again)))
		  again |= true;

		if (cc <= 0)
		  mlwrite(TEXT79);
					/* "Not found " */
	}}
			   /* Save away the match, or complain if not there. */
	if (cc > 0)
	{ WINDOW * wp = curwp;
		LINE * lim = wp->w_linep;
		LINE * lp = wp->w_dotp;
		int ct = wp->w_ntrows;
		while (--ct > 0 && lp != lim)
			lp = lback(lp);

		if (ct <= 0)
		{	wp->w_linep = wp->w_dotp;
			mvdnwind(1,- wp->w_ntrows / 2);
		}
	{ int pmlen = strlen(pd_patmatch) + 5;
		int plen = pmlen;
		int e = wp->w_doto + plen;
		if (e - lused(lp->l_dcr) > 0)
		{ plen -= e - lused(lp->l_dcr);
			e -= e - lused(lp->l_dcr);
		}
		if (e - term.t_ncol > 0)
#if 1
			wp->w_doto = e - plen;
#else
		{ wp->w_doto = e;
		  update(TRUE);
		  wp->w_doto -= plen;
		}
#endif
	{	char color = '2'; // pd_highlight == NULL ? '2' : pd_highlight[0];
		pd_hlight1 = remallocstr(&pd_hlight1, NULL, pmlen);
		strcpy(pd_hlight1+1, pd_patmatch)[-1] = color;
	  update(TRUE);
	}}}
	return cc > 0;
}

/*
 * forwhunt -- Search forward for a previously acquired search string.
 *	If found, reset the "." to be just after the match string,
 *	and (perhaps) repaint the display.
 */
int Pascal forwhunt(int f, int n)

{ 
	return hunt(n, TRUE);
}

/*
 * backhunt -- Reverse search for a previously acquired search string,
 *	starting at "." and proceeding toward the front of the buffer.
 *	If found "." is left pointing at the first character of the pattern
 *	(the last character that was matched).
 */
int Pascal backhunt(int f, int n)

{	
	return hunt(-n, TRUE);
}


/* forwsearch -- Search forward.  Get a search string from the user, and
 *	search for the string.	If found, reset the "." to be just after
 *	the match string, and (perhaps) repaint the display.
 */
int Pascal forwsearch(int f, int n)

{	/* Ask the user for the text of a pattern.  If the
	 * response is TRUE (responses other than FALSE are possible), 
	 * search for the pattern for as long as n is positive
	 * (n == 0 will go through once, which is just fine).
	 */
	int cc = readpattern(TEXT80 + (n >= 0 ? TEXT80_O : 0), &pat[0]);
												             /* "Search" */
	if (cc > FALSE)
	  cc = hunt(n, FALSE);

	return cc;
}


/* backsearch -- Reverse search.  Get a search string from the user, and
 *	search, starting at "." and proceeding toward the front of the buffer.
 *	If found "." is left pointing at the first character of the pattern
 *	(the last character that was matched).
 */
int Pascal backsearch(int f, int n)

{
	return forwsearch(f, -n);
}

/*
 * eq -- Compare two characters.  The "bc" comes from the buffer, "pc"
 *	from the pattern.  If we are not in EXACT mode, fold out the case.
 */
int USE_FAST_CALL myeq(int a, int b)

{
	int mask = (curbp->b_flag & MDIGCASE) && isletter(a) ? (~0x20 & 0xff) : 0xff;
  a = ((a^b) & mask) == 0;
  return a;
}
 
 
#define eq(a, b) ((((a ^ b) & (~0x20 & 0xff))) == 0 && myeq(a,b))

/* mceq -- meta-character equality with a character.  In Kernighan & Plauger's
 *	Software Tools, this is the function omatch(), but i felt there
 *	were too many functions with the 'match' name already.
 */
static int mceq(int bc, int mctype, int lchar)
{
	int result;

	if      (bc < 0)
		return bc;
	else if ((mctype & MASKPAT) <= EOL)
		result = eq(bc, (int)lchar);
	else
	{
#if _DEBUG && 0
		if ((mctype & MASKPAT) != CCL)
		{	adb(41);
			result = 0;
		}
		else
#endif
	  {	Bitmap * pbm = g_cclarray[lchar];
			result = pbm[bc >> 4];
			if ((curbp->b_flag & MDIGCASE) && isletter(bc))
				result |= pbm[(bc >> 4) ^ 2];
			result = (result >> (bc & 15)) & 1;
		}
	}

	if (mctype & NOT_PAT)
		result ^= 1;

	return result;
}

/*
 * amatch -- Search for a meta-pattern in either direction.
 *      Based on the recursive routine amatch() (for "anchored match") in
 *			Kernighan & Plauger's "Software Tools". (Private to scanner)
 *				g_Dmatchlen inherited and synthesised
 */
static
int Pascal amatch(MC * mcptr, int direct, Lpos_t * pcwlpos, int matchlen)
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
	int type;
	int ct;
	for (; (type = mcptr->mc_type) != MCNIL; mcptr += direct)
	{ 								    /* The only way we'd get a BOL metacharacter here
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
												 * '$', and '^' match positions, not characters.*/
	  if       (type == BOL)
	  { if (lpos.curoff != 0)
	      return 0;
		}
//  else if (type ==  EOL)
//  { if (lpos.curoff != llength(lpos.curline))
//      return FALSE;
//	}
	  else												/* Try to match as many characters as possible */
																/* A newline never matches a closure. */
	  {	ct = 0;
	    while (mceq(nextch(&lpos, direct), type, mcptr->lchar) > 0)
	    { ++ct;
	  		if ((type & CLOSURE) == 0)
	  			break;
	  	}
	  	if ((type & CLOSURE) == 0)
			{ if (type != EOL)
	    	  ++matchlen;
	  		if (ct == 0)
	  			return 0;
	    	continue;
	    }																	 /* We are now at the character that made us fail.
																					* Try to match the rest of the pattern, shrinking
																					* the closure by one for each failure.
																					* Since closure matches *zero* or more occurences
																					* of a pattern, a match may start even if the */
	    while (1)													 /* previous loop matched no characters. */
	    {	nextch(&lpos, -direct);
 
	      matchlen = amatch(mcptr+direct, direct, &lpos, matchlen);
	      if (matchlen)
	        goto success;

	      if (--ct < 0)
	        return 0;
	    }
	  }
	}	/* End of mcptr loop.*/

	ct = 0;
success:							/* A SUCCESSFULL MATCH!!!  */
	*pcwlpos = lpos;		/* Reset the "." pointers. */
	return matchlen += ct;
}

void USE_FAST_CALL rest_l_offs(Lpos_t* lpos)

{ *(Lpos_t*)&curwp->w_dotp = *lpos;
}

#if 0
/*
 * savematch -- We found the pattern?  Let's save it away.
 */
void Pascal savematch()

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
									/* int again ; 		** 1 : again, 2 : skip */
{ int pats_top = mk_magic();
  if (pats_top <= 0)
	  return pats_top;

{ MC * mcptr = &g_pats[direct > 0 ? direct : pats_top];
 	Lpos_t lpos = *(Lpos_t*)&curwp->w_dotp;
  Lpos_t sm; //  = lpos;			/* match line and offs */

  init_paren("", 0);
  g_paren.sdir = direct;

  if ((again & 1) ? direct > 0 : l_is_hd(lpos.curline))
    nextch(&lpos, direct);    /* Advance the cursor.*/

{ int skip = (again & 2) * g_clring;
	int matchlen = 0;
	int ch = 0;
									 /* Scan each character until we hit the head link record. */
  while (true)
  { /*if (direct == FORWARD ? lpos.curoff == llength(lpos.curline) &&
				(lforw(lpos.curline)->l_props & L_IS_HD)
		 	  :  ** lpos.curoff == 0	&& */
			     /* lpos.curline  == lforw(&curbp->b_baseline) && */
    if (ch < 0)
      return ch;
		  	   /* Save the current position in case we need to restore it on a match. 
		    		* Initialize g_Dmatchlen to zero for any search for replacement.
		  	  	*/
	{	char ptyp = mcptr->mc_type;
    if      (ptyp != LITCHAR)
    	ptyp = 0;
    else
    { ch = nextch(&lpos, direct); 		/* Advance the cursor.*/
      if (skip)
		  { if ((scan_paren((char)ch) & Q_IN_CMT) != 0)
		  		continue;
		  }

      if (! eq(ch, (int)mcptr->lchar))
        continue;
    }
    sm = lpos;
    matchlen = amatch(mcptr+ptyp*direct, direct, &lpos, ptyp);
    if (ptyp)
		{	if (! matchlen)
        continue;

      nextch(&sm, -direct);  /* restore the cursor.*/
      break;
    }
    else
      if (matchlen)
        break;

    ch = nextch(&lpos, direct);  /* Advance the cursor.*/
		if (skip)
		  scan_paren((char)ch);
  }}
	   
	if (matchlen > 0)
	{ extern int macro_start_col;
		macro_start_col = -1;
	}

  if (direct > 0)     /* at beginning of string */
    lpos = sm;

  rest_l_offs(&lpos);

  curwp->w_flag |= WFMOVE; /* flag that we have moved */
  free(pd_patmatch);
  pd_patmatch = (char*)mallocz(matchlen+1);

{ char * patptr = pd_patmatch;
  if (patptr != NULL)
  {
    for (ch = matchlen; --ch >= 0; )
      *patptr++ = nextch(&lpos, FORWARD);
  }

//g_Dmatchlen = matchlen;
  return matchlen;				 /* We found a match */
}}}}

static int replaces(int, int, int);
/*
 * sreplace -- Search and replace.
 */
int Pascal sreplace(int f, int n)

{
  return replaces(TRUE, f, n);
}

/*
 * qreplace -- search and replace with query.
 */
int Pascal qreplace(int f, int n)

{
  return replaces(FALSE, f, n);
}


/* replaces -- Search for a string and replace it with another
 *	string.  Query might be enabled (according to kind).
 */
static
int replaces(int kind, int f, int n)
	/* int	kind;	* Query enabled flag */
	/* int	f;	    * default flag */
	/* int	n;	    * # of repetitions wanted */
{
	int cc;
	char tpat[NPAT*2+20];

	if (rdonly())
	  return FALSE;
														/* Ask the user for the text of a pattern. */
	cc = readpattern(TEXT82 + (kind * TEXT82_O), &pat[0]);
																	/* "Replace" */
																	/* "Query replace" */
	if (cc <= FALSE)
	  return cc;
					   /* Ask for the replacement string. */
	cc = readpattern(TEXT84, &rpat[0]);
									/* "with" */
	if (cc < 0)
	  return cc;
															/* Find the length of the replacement string. */
														  /* Set up flags so we can make sure not to do 
														   * a recursive replace on the last line. */
{	WINDOW * wp = curwp;
  int orig_line_no = setcline();

//orig.curline = lback(orig.curline);
  int numsub = 0;
  int lenold = 0;
  int lastins = 0;
	Lpos_t last;		/* position of last replace */
	last.curline = NULL;
																		/* Build query replace question string */
											/* "Replace '" */
{	char * t = expandp(NPAT-9, pat, TEXT88, strcpy(tpat, TEXT87));
													/* "' with '" */
	expandp(NPAT-4, rpat, "'? ", t);

	while (--n >= 0 || !f)
	{ int matchlen = scanner(1, FALSE);
		if (matchlen < 0)
			break;
								     				/* Search for the pattern. */
#if 0
				  				   				/* If we search with a regular expression,
												     * g_Dmatchlen is reset to the true length of
												     * the matched string. */
														/* Check if we are on the last line */
	  if (l_is_hd(lforw(wp->w_dotp)) &&
        matchlen > 0 &&
	      pat[matchlen - 1] == '\n')
	    n = -1;
#endif

	  if (!kind)			/* Check for query */
	  {	if (getwpos() + 2 >= wp->w_ntrows)
			{ Lpos_t save = *(Lpos_t*)&wp->w_dotp;
			  forwbyline(2);
			  rest_l_offs(&save);
			}
					  					/* Get the query  */
pprompt:
			mlwrite(tpat);
qprompt:
			update(TRUE);		/* show the proposed place to change */
			cc = getkey();
   /* mlwrite("");			** and clear it */

			switch (cc | 0x20)		/* And respond appropriately */
			{ case 'l':   n = -1;
#if	FRENCH
		  	case 'o':			/* yes, substitute */
#endif
			  case 'y':			/* yes, substitute */
			  case ' ':

			  when '!':	kind = TRUE;	/* yes/stop asking */
					
			  when 'n':	goto no;

			  when 'u':		   /* undo last and re-prompt */
					if (last.curline != NULL)
			  	{ rest_l_offs(&last);			/* Restore old position. */
					  last.curline = NULL;	   									

					  backbychar(lastins);
					  cc = delins(lastins, pd_patmatch, 0);
					  if (cc < 0)
					    return FALSE;
						       /* Record one less substitution,
										* backup, save our place, and reprompt. */
					  --numsub;
					  backbychar(lenold);
					}
					goto pprompt;
			  otherwise			   /* bitch and beep */
					tcapbeep();
			  case '?':	mlwrite(TEXT90);	  /* help me */
/*"(Y)es, (N)o, (!)Do rest, (U)ndo last, (^G)Abort, (.)Abort back, (?)Help: "*/
					goto qprompt;
			  when '.':			     /* abort! and return */   /* restore old position */
//				orig.curline = lforw(orig.curline);
//				if (orig.curoff >= orig.curline->l_used)
//				  orig.curoff = 0;
					gotoline(1,orig_line_no);
//				wp->w_flag |= WFMOVE;

			  case CTRL|'g':
			  	mlwrite(TEXT89);  /* abort! and stay */
								/* "Aborted!" */
					return FALSE;
			}       
	  }	/* end of "if kind" */

	  numsub++;					/* increment # of substitutions */
		lenold = matchlen;
	  if (/*(unsigned)matchlen == strlen(rpat) && */ strcmp(pd_patmatch,rpat) == 0)
	  	goto no;

	{ LINE * curl = wp->w_dotp;

			   /* Delete the sucker, and insert its replacement. */
	  lastins = delins(lenold, &rpat[0], MC_DITTO);
//  if (lastins < 0)
//    return FALSE;
	  			  /* Save our position, since we may undo this*/
		if (last.curline == curl)
		{ // --g_inhibit_scan;
			if (last.curline != wp->w_dotp)
				lchange(WFHARD);
	  }
	  last = *(Lpos_t*)&wp->w_dotp;
										  /* If we are not querying, check to make sure
										   * that we didn't replace an empty string
										   * (possible in MAGIC mode), because we'll
										   * infinite loop. */
	  if      (!kind)
	    update(TRUE);			  /* show the change */
	  else if (matchlen == 0)
		{	mlwrite(TEXT91);				/* And report the results */
				/* "Empty string replaced, stopping." */
	  	return FALSE;
	  }
	  continue;
no:	  
	  forwbychar(1);	/* no, onword */
	}} // Loop
#if S_WIN32 == 0
	update(TRUE);
#endif						
	mlwrite(TEXT92, numsub);				/* And report the results */
				/* "%d substitutions" */
	return TRUE;
}}}

								/* delins -- Delete a specified length from the current point
								 *	then either insert the string directly, or make use of
								 *	replacement meta-array. */
static
int Pascal delins(int dlength, char * instr, int use_meta)
						       					
{	int overmode = curbp->b_flag;
	curbp->b_flag &= ~MDOVER;						/* Zap what we gotta, and insert its replacement. */

{	int len = 0;
  int cc = ldelchrs((Int)dlength, FALSE, FALSE);
	if (cc > FALSE)
	{ int ix = -1;
	  char buf[120];

	  for ( ; ; ++instr)
	  { if (instr[0] == MC_ESC && instr[1] != 0 && use_meta)
	    { int chr;
	      instr += get_hex2(&chr, instr+1);

	      buf[++ix] = chr;
	      continue;
	    }
	    if (instr[0] == use_meta || 
	        instr[0] == 0        ||
	        ix > sizeof(buf) - 4 )
	    { buf[++ix] = 0;
	    	len += ix;
	      cc = linstr(buf);
	      if (cc <= FALSE || instr[0] == 0)
	        break;
	      ix = -1;
	      if (instr[0] == use_meta)
	      { len += strlen(pd_patmatch);
	      	cc = linstr(pd_patmatch);
	        if (cc <= FALSE)
	          break;
	        continue;
	      }
	    }
	    
	    buf[++ix] = instr[0];
	  }
	}

	overmode |= curbp->b_flag & BFCHG;
	curbp->b_flag = overmode;

	return len;
}}

static int get_word(char * t, char * s, int ct)

{ char * t_ = t;

  while (--ct >= 0 && (isalnum(*s) || *s == '_'))
	{ *t++ = *s++; 
	}
	*t = 0;
	
	return t - t_;
}



/* wordsearch -- Reverse search for a word starting with a prefix.
 * Insert the result at .
 */
int Pascal wordsearch(int f, int n)

{ Lpos_t sv = *(Lpos_t*)&curwp->w_dotp;	/* original line pointer */

  char * stt = lgets(sv.curline, 0);
  char * s = stt + sv.curoff;
  
  while (--s >= stt)
  { char ch = *s;
  	if (!isalnum(ch) && ch != '_')
    	break;
  }

{	char buff[132];
  int len = stt - ++s + sv.curoff;
  if (len >= sizeof(buff))
		len = sizeof(buff)-1;

{ int ix = get_word(buff, s, len);

//mbwrite(buff);
  if (ix > 0)
  { backword(1, 1);
    while (backword(1, 1))
    {
    	int doto = curwp->w_doto;
      int len = lused(curwp->w_dotp->l_dcr) - doto;
      char * w = &curwp->w_dotp->l_text[doto];
      
      if (w[0] == buff[0])
        if (strcmp_right(w,buff) == 0)
        { 
		      if (len >= sizeof(buff)) 
						len = sizeof(buff) - 1;
					get_word(buff, w, len);
				  ix = -ix;
          break;
        }
    }
  }

	rest_l_offs(&sv);
  
  if (ix < 0)
  {
#if S_WIN32 && 0
    mbwrite(buff);
#endif
    linstr(buff-ix);
  }

  return OK;
}}}

#if 0

static int get_indt(LINE * lp)

{ char * s = lgets(lp, 0);
  int fin = lused(lp->l_dcr)-1;
  int ix = -1;
  int offs = 0;
  
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


static int get_indent(LINE * lp, int tabsz)

{ char * s = lgets(lp, 0) - 1;
	int    ct = lused(lp->l_dcr);
	int offs = 0;

  while (--ct >= 0)
  { char ch = *++s;
    if      (ch == ' ')
      offs += 1;
    else if (ch == 9)
	    offs = ((offs + tabsz-1)/tabsz)*tabsz;
    else
      break;
  }
  
  return offs;
}


/*
 * indentsearch -- Reverse search for an indent less than current.
 */
int Pascal indentsearch(int notused, int n)

{
  LINE * lp = curwp->w_dotp;
  int tabsz = curbp->b_tabsize;
//char buff[132];
//sprintf(buff,"offs %d", offs);
//mbwrite(buff);

  if ( n > 0)
  { int moved = 0;
		int offs = 0;
		int off = -1;

    do
 		{	offs = get_indent(lp,tabsz);
 			if      (off < 0)
 			{ if (offs <= 0)
 					break;
 				off = offs;
 			}
 			else if (offs < off)
 				break;
      lp = lback(lp);
    	offs = -(l_is_hd(lp));
      if (offs != 0) 
				break;

      moved += 1;
    } while (TRUE);

    if (offs < 0)
      tcapbeep();
    else
    { curwp->w_dotp  = lp;
      curwp->w_doto  = offs;
      curwp->w_line_no -= moved;
      curwp->w_flag |= WFMOVE; /* flag that we have moved */
    }
  }

  return OK;
}
