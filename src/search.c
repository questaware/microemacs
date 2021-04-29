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
//#define	ANY	2
#define	CCL	3
#define	NCCL (NOT_PAT+CCL)
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
#define	MC_CLOSURE '*'/* Closure - does not extend past newline.*/
#define	MC_DITTO	'&'	/* Use matched string in replacement.*/
#define	MC_ESC		'\\'/* Escape - suppress meta-meaning.*/

#define BIT(n)		(1 << (n))	/* An integer with one bit set.*/

/* Typedefs that define the bitmap type for searching (BITMAP),
 * and the meta-character structure for MAGIC mode searching (MC).
 */

typedef unsigned short BITMAP;

typedef struct
{	char   mc_type;
	char   lchar;
} MC;

static BITMAP * cclarray[NPAT / 16 /* say */ ];
static int cclarr_ix = 0;

static MC   g_pats[NPAT+2];
static int	g_pats_top;

static unsigned int g_Dmatchlen = 0;

#define ERR_SYN   (0)
#define ERR_OOMEM (-1)

/* cclmake -- create the bitmap for the character class.
 *	patix is left pointing to the end-of-character-class character,
 *	so that a loop may automatically increment with safety.
 *	uses pat[].
 */
static int Pascal cclmake(int patix, MC *  mcptr)
	
{ int to = cclarr_ix;
	BITMAP * p_bmap = cclarray[to];

	if (to >= sizeof(cclarray)/sizeof(BITMAP*) ||
			 p_bmap == NULL &&
	    (p_bmap = (BITMAP*)aalloc(HICHAR >> 3)) == NULL)
	{ 
	  return ERR_OOMEM;
	}

	cclarray[ to ] = p_bmap;
	mcptr->lchar = to;
	mcptr->mc_type = CCL;
	++cclarr_ix;
																		/* Test the initial character(s) in ccl for
																		 * special cases - negate ccl, or an end ccl
																		 * character as a first character.
																		 * Anything else gets set in the bitmap. */
{	int from;
	for ( ; (from = pat[++patix]) != 0 && from != MC_ECCL; )
	{
		if (from == MC_NCCL)		/* '^' */
		{ mcptr->mc_type |= NOT_PAT;
			continue;
		}
	  if (pat[patix + 1] == MC_RCCL && pat[patix + 2] != MC_ECCL)
		{	to = pat[patix + 2];
	  	patix += 2;
	  }
	  else
	    to = from;

	  for ( --from ; ++from <= to; )
	    p_bmap[from >> 4] |= (1 << (from & 15));
	}
	
	return from == 0 ? from : patix;
}}


static
int Pascal get_hex2(int * res_ref, const char * s)
	
{ char res = 0;
  int adv = 2;
  
  while (--adv >= 0)
  { char chr = *s++ -'0';
    if      (in_range(chr, 0, 9))
      ;
    else if (in_range(chr, 'a' - '0', 'f' - '0'))
      chr -= 'a' - '0' - 10;
    else if (in_range(chr, 'A' - '0', 'F' - '0'))
      chr -= 'A' - '0' - 10;
    else
      break;
    res = (res << 4) + chr;
  }
  *res_ref = res;
  return 1 - adv;
}


/* mcstr -- Set up the 'magic' array.  The closure symbol is taken as
 *	a literal character when (1) it is the first character in the
 *	pattern, and (2) when preceded by a symbol that does not allow
 *	closure, such as a newline, beginning of line symbol, or another
 *	closure symbol.
 */
int Pascal mk_magic(int mode)

{	  int magical = 0;
    int patix = -1;
	  int	pchr;
#define DOES_CL 1
		MC * mcptr = g_pats;

//mcpat[0].mc_type = MCNIL;
	g_pats[NPAT].mc_type = -1;	/* a barrier */

	cclarr_ix = 0;						/* reset magical. */

	if (mode < 0)
		mode = curbp->b_flag;

	while (TRUE)
	{
	 	if ((++mcptr)->mc_type < 0)
		{ patix = ERR_OOMEM;
			break;
		}

		pchr = pat[++patix];
		if (pchr == 0)
			break;

		if (mode & MDMAGIC)
		  switch (pchr)
		  {	case MC_CCL:		/* [ */
					magical = DOES_CL;
		  		patix = cclmake(patix, mcptr);
					if (patix <= 0)
						break;	  		
					continue;
						
			  when MC_ANY:		/* . */
					magical = DOES_CL;
			  	mcptr->mc_type = LITCHAR | NOT_PAT;
			  	mcptr->lchar = '\n';
					continue;
				        
			  when MC_CLOSURE:/* * */			/* Does the closure symbol mean closure here?
																		 * If so, back up to the previous element
																		 * and indicate it is enclosed. */
					if (magical)
					{	(--mcptr)->mc_type |= CLOSURE;
						magical = 0;
						continue;
					}
	
			  when MC_ESC:		/* \ */
					if (pat[patix+1] != 0)
					{ pchr = pat[patix+1];
					  ++patix;
						if (pchr == '0') 
							patix += get_hex2(&pchr, &pat[patix+1]);
					}
			}

		mcptr->mc_type = LITCHAR;
		mcptr->lchar   = pchr;
		magical = pchr - '\n';
	}	/* End of while.*/

		/* Set up the reverse array, if the status is good.  Please note
		 * the structure assignment - your compiler may not like that.
		 * If the status is not good, nil out the meta-pattern.
		 * The only way the status would be bad is from the cclmake()
		 * routine, and the bitmap for that member is guarenteed to be
		 * freed.  So we stomp a MCNIL value there.
		 */
	if (patix < 0)
	{ mlwrite(patix == ERR_SYN ? TEXT97		/* Out of Memory */
	  												 : TEXT99);	/* "%%Missing ]" */
	  return false;
	}
	else
	{	mcptr->mc_type = MCNIL; 	/* Close off the meta-string */

		g_pats_top = (--mcptr) - g_pats;
		if (mcptr->lchar == MC_EOL)
//		mcptr->mc_type = EOL;
			mcptr->lchar = '\n';
	  if (g_pats[1].lchar == MC_BOL)
			g_pats[1].mc_type = BOL;

	  return true;
	}
}


/*
 * expandp -- Expand control key sequences for output.
 */
static 
void Pascal expandp(char * deststr, char * srcstr, char * tailstr, int maxlength)
	/* char	*deststr;	** destination of expanded string (concat) */
	/* char	*srcstr;	** string to expand */
	/* char	*tailstr;	** string on the end */
	/* int	maxlength;	** maximum chars in destination */
{
  static const char strs[] = "<CR\000^\000%\000";
	int dix = strlen(deststr) - 1;
	unsigned char c;
					     /* Scan through the string */
	while ((c = *srcstr++) != 0)
	{	int insix = 2;

		if      (c == '\r')	       /* it's a newline */
		{
			insix = -1;
			c = '>';
		}
		else if (c < 0x20 || c == 0x7f)       /* control character */
		{
			insix = 3;
			c ^= 0x40;
		}
		else if (c == '%')
			insix = 5;
			
		for ( ; strs[++insix] != 0; )
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

{   char tpat[NPAT+50];
    																				/* add old pattern */
    expandp(concat(tpat, prompt, " [", null), 
    				strlen(apat) < 30 ? apat : "\"",  "] ",  sizeof(tpat)-1);

					     /* Read a pattern.  Either we get one, or
		    			  * we just get the META charater, and use the previous pattern */ 
{		Cc cc = mltreply(tpat, tpat, NPAT);
    if (cc >= 0)
    { extern int gs_keyct;		 /* from getstring */
      if (cc || gs_keyct > 0)
        strcpy(apat, tpat);

      cc = TRUE;
//    if (apat == pat)
//      cc = mk_magic(-1);
    }

    return cc;
}}

/*
int last_was_srch()

{ return lastfnc == forwsearch || lastfnc == backsearch ||
         lastfnc == backhunt   || lastfnc == forwhunt;
}
*/ 


int Pascal hunt(int n, int again)

{								/* Make sure a pattern exists, or that we didn't switch
								 * into MAGIC mode until after we entered the pattern. */
//paren.sdir = 1;

  if (kbd_record(pd_kbdmode))
    g_got_search = TRUE;

  if (pat[0] == '\0')
	{ mlwrite(TEXT78);
				  /* "No pattern set" */
	  return FALSE;
	}

//if (/*(mdexact & MDMAGIC) && */g_pats[dir][0].mc_type == MCNIL)
	if (!mk_magic(-1))
	  return FALSE;

{	int dir = 1;
	int cc;
	if (n < 0)
	{ n = -n;
	  dir = -1;
//	paren.sdir = -1;
	}

  pd_lastdir = dir;
  if ((curwp->w_bufp->b_flag & MDSRCHC) == 0)
  	again |= 4;

	while ((cc = scanner(dir, again))
					&& --n > 0)
	  again |= true;
			   /* Save away the match, or complain if not there. */
	if (cc != TRUE)
	  mlwrite(TEXT79);
					/* "Not found " */
	else
	{ update(TRUE);
	{ WINDOW * wp = curwp;
		LINE * lim = wp->w_linep;
		LINE * lp = wp->w_dotp;
		int ct = wp->w_ntrows;
		while (--ct > 0 && lp != lim)
			lp = lback(lp);

		if (ct <= 2)
			mvdnwind(1,2 + ct);
	}}
	return cc;
}}

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

{	if (n == 0)
		n = 1;

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
	int cc = readpattern(n >= 0 ? TEXT80 : TEXT81, &pat[0]);
												             /* "Search" */
	if (cc == TRUE)
	  cc = hunt(n, FALSE);

	return cc;
}


/* backsearch -- Reverse search.  Get a search string from the user, and
 *	search, starting at "." and proceeding toward the front of the buffer.
 *	If found "." is left pointing at the first character of the pattern
 *	(the last character that was matched).
 */
int Pascal backsearch(int f, int n)

{	if (n == 0)
	  n = 1;

	return forwsearch(f, -n);
}

/*
 * eq -- Compare two characters.  The "bc" comes from the buffer, "pc"
 *	from the pattern.  If we are not in EXACT mode, fold out the case.
 */
int USE_FAST_CALL myeq(int a, int b)

{
  return a==b ||
        (curbp->b_flag & MDEXACT) == 0 && isletter(a)
     && ((a ^ b) & ~0x20) == 0;         
}
 
 
#define eq(a, b) (((a ^ b) & ~0x20) == 0 && myeq(a,b))

/* mceq -- meta-character equality with a character.  In Kernighan & Plauger's
 *	Software Tools, this is the function omatch(), but i felt there
 *	were too many functions with the 'match' name already.
 */
static int mceq(int bc, int mctype, int lchar)
{
	int result;

	if      (bc < 0)
		return 0;
	else if ((mctype & MASKPAT) == LITCHAR)
		result = eq(bc, (int)lchar);
	else
	{
#if _DEBUG
		if ((mctype & MASKPAT) != CCL)
		{	adb(41);
			result = 0;
		}
		else
#endif
	  {	BITMAP * pbm = cclarray[lchar];
			result = pbm[bc >> 4];
			if ((curbp->b_flag & MDEXACT) == 0 && isletter(bc))
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
	int type;
	for (; (type = mcptr->mc_type) != MCNIL; mcptr += direct)
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
												 * '$', and '^' match positions, not characters.*/
	  if       (type == BOL)
	  { if (lpos.curoff != 0)
	      return FALSE;
		}
//  else if (type == EOL)
//  { if (lpos.curoff != llength(lpos.curline))
//      return FALSE;
//	}
	  else												/* Try to match as many characters as possible */
	  {														/* against the current meta-character.
																 * A newline never matches a closure. */
	    int ct = 0;
	    while (mceq(nextch(&lpos, direct), mcptr->mc_type, mcptr->lchar))
	    { ++ct;
	  		if ((type & CLOSURE) == 0)
	  			break;
	  	}
	  	if ((type & CLOSURE) == 0)
	  	{	if (ct == 0)
	  			return FALSE;
	    	g_Dmatchlen++;
	    	continue;
	    }
												/* We are now at the character that made us fail.
												 * Try to match the rest of the pattern, shrinking
												 * the closure by one for each failure.
												 * Since closure matches *zero* or more occurences
												 * of a pattern, a match may start even if the
												 * previous loop matched no characters. */
	    while (1)
	    { nextch(&lpos, -direct);

	      if (amatch(mcptr+direct, direct, &lpos))
	      { g_Dmatchlen += ct;
	        goto success;
	      }

	      if (--ct < 0)
	        return FALSE;
	    }
	  }
	}	/* End of mcptr loop.*/

success:							/* A SUCCESSFULL MATCH!!!  */
	*pcwlpos = lpos;		/* Reset the "." pointers. */
	return TRUE;
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
									/* int again ; 		** 1 : again, 2: call mk_magic, 4 : skip */
	
{ Lpos_t lpos = *(Lpos_t*)&curwp->w_dotp;
  Lpos_t sm = lpos;			/* match line and offs */
  int skip = again & 4;

  init_paren("",0);
  paren.sdir = direct;
  
  (void)mk_magic(-1);
  
  if ((again & 1) ? direct > 0 : (lpos.curline->l_props & L_IS_HD))
    nextch(&lpos, direct);    /* Advance the cursor.*/
                              /* Save the old g_Dmatchlen length, in case it is
                               * very different (closure) from the old length.
                               * Important for the query-replace undo command. */
{ int ch = 0;
  MC * mcpatrn = &g_pats[direct > 0 ? 1 : g_pats_top];
  int got = 0;
									 /* Scan each character until we hit the head link record. */
  while (true)
  { /*if (direct == FORWARD ? lpos.curoff == llength(lpos.curline) &&
				(lforw(lpos.curline)->l_props & L_IS_HD)
		 	  :  ** lpos.curoff == 0	&& */
			     /* lpos.curline  == lforw(&curbp->b_baseline) && */
    if (ch < 0)
      return false;
		  	  /* Save the current position in case we need to restore it on a match. 
		    	 * Initialize g_Dmatchlen to zero for any search for replacement.
		  	   */
	{	char ptyp = mcpatrn->mc_type;
    if      (ptyp == LITCHAR)
    { ch = nextch(&lpos, direct); 		/* Advance the cursor.*/
      if (skip)
		    scan_paren(ch);

      if (! eq(ch, (int)mcpatrn->lchar))
        continue;
      sm = lpos;
      got = 1;
      g_Dmatchlen = 1;
      if (! amatch(mcpatrn+direct, direct, &lpos))
        continue;

			if ((paren.in_mode & Q_IN_CMT) != 0)
				continue;

      nextch(&sm, -direct);  /* restore the cursor.*/
      break;
    }
    else
    { sm = lpos;
    	got = 1;
      g_Dmatchlen = 0;
//  	if (skip && (paren.in_mode & Q_IN_CMT) != 0)
//			continue;
      if (amatch(mcpatrn, direct, &lpos))
        break;
    }
    ch = nextch(&lpos, direct);  /* Advance the cursor.*/
		if (skip)
		  scan_paren(ch);
  }}
		   
  if (direct > 0)     /* at beginning of string */
    lpos = sm;

  rest_l_offs(&lpos);
  curwp->w_flag |= WFMOVE; /* flag that we have moved */

  if (patmatch != NULL)
    free(patmatch);

  patmatch = (char*)malloc(g_Dmatchlen+1);

{ char * patptr = patmatch;
  if (patptr != NULL)
  {
    for (ch = g_Dmatchlen; --ch >= 0; )
      *patptr++ = nextch(&sm, FORWARD);

    *patptr = '\0';
  }
#if _DEBUG
	if (!got)											// otherwise optimised away
		adb(147); 									// Sc inherited
#endif

  return true;				/* We found a match */
}}}

static int Pascal replaces(int, int, int);
/*
 * sreplace -- Search and replace.
 */
int Pascal sreplace(int f, int n)

{
  return replaces(FALSE, f, n);
}

/*
 * qreplace -- search and replace with query.
 */
int Pascal qreplace(int f, int n)

{
  return replaces(TRUE, f, n);
}


/* replaces -- Search for a string and replace it with another
 *	string.  Query might be enabled (according to kind).
 */
static
int Pascal replaces(int kind, int f, int n)
	/* int	kind;	* Query enabled flag */
	/* int	f;	    * default flag */
	/* int	n;	    * # of repetitions wanted */
{
	int cc;
	char tpat[NPAT*2+20];

	if (curbp->b_flag & MDVIEW)
	  return rdonly();

	if      (!f)
		n = 100000000;
	else if (n < 0) 		  		/* Check for negative repetitions */
	  return FALSE;
														/* Ask the user for the text of a pattern. */
	cc = readpattern(kind == FALSE ? TEXT82 : TEXT83, &pat[0]);
																	/* "Replace" */
																	/* "Query replace" */
	if (cc != TRUE)
	  return cc;
					   /* Ask for the replacement string. */
	cc = readpattern(TEXT84, &rpat[0]);
									/* "with" */
	if (cc < 0)
	  return cc;
															/* Find the length of the replacement string. */
{ int inhib; 
														  /* Set up flags so we can make sure not to do 
														   * a recursive replace on the last line. */
  int numsub = 0;
  int lenold = 0;
  int lastins = 0;

	Lpos_t orig = *(Lpos_t*)&curwp->w_dotp;
	Lpos_t last;		/* position of last replace */

//orig.curline = lback(orig.curline);
	last.curline = NULL;
																		/* Build query replace question string */
											/* "Replace '" */
	expandp(strcpy(tpat, TEXT87), pat, TEXT88, NPAT-9);
													/* "' with '" */
	expandp(tpat, rpat, "'? ", NPAT-4);

	while (--n >= 0 && scanner(1, FALSE))
	{							     				/* Search for the pattern. */
#if 0
				  				   				/* If we search with a regular expression,
												     * g_Dmatchlen is reset to the true length of
												     * the matched string. */
														/* Check if we are on the last line */
	  if ((lforw(curwp->w_dotp)->l_props & L_IS_HD) &&
        g_Dmatchlen > 0 &&
	      pat[g_Dmatchlen - 1] == '\n')
	    n = -1;
#endif

	  if (kind)			/* Check for query */
	  {	if (getwpos() + 2 >= curwp->w_ntrows)
			{ Lpos_t save = *(Lpos_t*)&curwp->w_dotp;
			  forwline(f, 2);
			  rest_l_offs(&save);
			}
					  					/* Get the query  */
pprompt:
			mlwrite(tpat);
qprompt:
			update(TRUE);		/* show the proposed place to change */
			cc = getkey();
   /* mlwrite("");			** and clear it */

			switch (toupper(cc))		/* And respond appropriately */
			{ case 'L':   n = -1;
#if	FRENCH
		  	case 'O':			/* yes, substitute */
#endif
			  case 'Y':			/* yes, substitute */
			  case ' ':

			  when '!':	kind = FALSE;	/* yes/stop asking */
					
			  when 'N':	forwchar(FALSE, 1);	/* no, onword */
					continue;

			  when 'U':		   /* undo last and re-prompt */
					if (last.curline != NULL)
			  	{ rest_l_offs(&last);			/* Restore old position. */
					  last.curline = NULL;	   									

					  backchar(FALSE, lastins);
					  cc = delins(lastins, patmatch, 0);
					  if (cc < 0)
					    return FALSE;
						       /* Record one less substitution,
										* backup, save our place, and reprompt. */
					  --numsub;
					  backchar(FALSE, lenold);
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
					rest_l_offs(&orig);
					curwp->w_flag |= WFMOVE;

			  case CTRL|'G':
			  	mlwrite(TEXT89);  /* abort! and stay */
								/* "Aborted!" */
					return FALSE;
			}       
	  }	/* end of "if kind" */

		inhib = last.curline == curwp->w_dotp;
//	if (inhib)
//		++g_inhibit_scan;

			   /* Delete the sucker, and insert its replacement. */
		lenold = g_Dmatchlen;
	  lastins = delins(lenold, &rpat[0], MC_DITTO);
	  if (lastins < 0)
	    return FALSE;
	  			  /* Save our position, since we may undo this*/
		if (inhib)
		{ // --g_inhibit_scan;
			if (last.curline != curwp->w_dotp)
				lchange(WFHARD);
	  }
	  last = *(Lpos_t*)&curwp->w_dotp;
										  /* If we are not querying, check to make sure
										   * that we didn't replace an empty string
										   * (possible in MAGIC mode), because we'll
										   * infinite loop. */
	  numsub++;					/* increment # of substitutions */
	  if      (kind)
	    update(TRUE);			  /* show the change */
	  else if (lenold == 0)
	  { mlwrite(TEXT91);
						/* "Empty string replaced, stopping." */
	    return FALSE;
	  }
	} // Loop
						/* And report the results */
	mlwrite(TEXT92, numsub);
				/* "%d substitutions" */
	return TRUE;
}}

								/* delins -- Delete a specified length from the current point
								 *	then either insert the string directly, or make use of
								 *	replacement meta-array. */
static
int Pascal delins(int dlength, char * instr, int use_meta)
						       					
{	extern int g_overmode;
	--g_overmode;							/* Zap what we gotta, and insert its replacement. */

{	int len = 0;
  int cc = ldelchrs((Int)dlength, FALSE);
	if (cc == TRUE)
	{ int ix = -1;
	  char buf[120];

	  for ( ; ; ++instr)
	  { if (instr[0] == MC_ESC && instr[1] != 0 && use_meta)
	    { int chr = *++instr;
        if (chr == '0')
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
	      if (cc != TRUE || instr[0] == 0)
	        break;
	      ix = -1;
	      if (instr[0] == use_meta)
	      { len += strlen(patmatch);
	      	cc = linstr(patmatch);
	        if (cc != TRUE)
	          break;
	        continue;
	      }
	    }
	    
	    buf[++ix] = instr[0];
	  }
	}

	++g_overmode;

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
      char * w = &curwp->w_dotp->l_text[doto];
      int len = curwp->w_dotp->l_used - doto;
      
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
    ldelchrs(ix, 0);
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
  int fin = lp->l_used-1;
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
	int    ct = lp->l_used;
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
int Pascal indentsearch(int f, int n)

{
  LINE * lp = curwp->w_dotp;
  int tabsz = curbp->b_tabsize;
//char buff[132];
//sprintf(buff,"offs %d", offs);
//mbwrite(buff);

	int off = get_indent(lp,tabsz);

  if (off > 0 && n > 0)
  { int moved = 0;
		int offs = 0;

    do
    { lp = lback(lp);
    	offs = -(lp->l_props & L_IS_HD);
      if (offs != 0) 
				break;

      moved += 1;

 			offs = get_indent(lp, tabsz);

		} while (offs >= off);

    if (offs < 0)
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
