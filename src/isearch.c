/* The functions in this file implement commands that perform incremental
 * searches in the forward and backward directions.  This "ISearch" command
 * is intended to emulate the same command from the original EMACS 
 * implementation (ITS).	Contains references to routines internal to
 * SEARCH.C.
 *
 * HISTORY:
 *
 *	D. R. Banks 9-May-86
 *	- added ITS EMACSlike ISearch
 *
 *	John M. Gamble 5-Oct-86
 *	- Made iterative search use search.c's scanner() routine.
 *		This allowed the elimination of bakscan().
 *	- Put  isearch constants into estruct.h
 *	- Eliminated the passing of 'status' to scanmore() and
 *		checknext(), since there were no circumstances where
 *		it ever equalled FALSE.
 */

#include	<stdio.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"

#if ISRCH

extern void Pascal reeat(int ch);


/* A couple more "own" variables for the command string */

#if CMDBUFLEN * 2 > NSTRING
 cmdbuflen too big
#endif


/*#define cmd_buff ((int*)gs_buf) ** Save the command args here */

typedef struct
{ short cmd_buff[CMDBUFLEN];
	int 	cmd_offset; 						/* Current offset into command buff */
	int 	cmd_reexecute;					/* > 0 if re-executing command */
} T_is;

static T_is g_isb;

/* routine to echo i-search characters */

static
int Pascal USE_FAST_CALL echochar(int c, int col)
					/* character to be echoed */
					/* column to be echoed in */
{
	tcapmove(term.t_nrowm1,col);

	switch (c)
	{
#if 0
	case '\r':
		col += 3;
		mlputs(4,"<NL>");
#endif
	when '\t':
//	if ((col += 4) > 0)
		mlputs(4,"<TAB>");

	when 0x7F:
		c = '?' - 'A' + 1;
					/* drop through */
	default:
		if (c < ' ')
		{ c ^= 0x40;
			++col;
			mlout('^');
		}
		mlout(c);
	}
#if S_MSDOS == 0
	TTflush();
#endif
	return col + 1;
}

	/*
	 * Routine to get the next character from the input stream.  If we're reading
	 * from the real terminal, force a screen update before we get the char. 
	 * Otherwise, we must be re-executing the command string, so just return the
	 * next character.
	 */
static
int Pascal get_char()

{		int c;
//	T_is * is = &g_isb;
								/* See if we're re-executing: */

		if (g_isb.cmd_reexecute >= 0) 	/* Is there an offset?					*/
			if ((c = g_isb.cmd_buff[g_isb.cmd_reexecute++]) != 0)
				return c; 		/* Yes, return any character	*/

				 /* We're not re-executing (or aren't any more).	Try for a real char */

		update(FALSE);										/* Pretty up the screen 				*/

		g_isb.cmd_reexecute = -1; 				/* Say we're in real mode again */
		if (++g_isb.cmd_offset >= CMDBUFLEN)
		{// mlwrite(TEXT166);
		 // 		/* " too long" */
			return 0;
		}
		g_isb.cmd_buff[g_isb.cmd_offset] = '\0';		/* terminate the buffer 	 */
									 															/* Save char for next time */
		return g_isb.cmd_buff[g_isb.cmd_offset-1] = getkey();
}


/* Subroutine to do an incremental search.	In general, this works similarly
 * to the older micro-emacs search function, except that the search happens
 * as each character is typed, with the screen and cursor updated with each
 * new search character.
 *
 * While searching forward, each successive character will leave the cursor
 * at the end of the entire matched string.  Typing a Control-S or Control-X
 * will cause the next occurrence of the string to be searched for (where the
 * next occurrence does NOT overlap the current occurrence).	A Control-R will
 * change to a backwards search, META will terminate the search and Control-G
 * will abort the search.  Rubout will back up to the previous match of the
 * string, or if the starting point is reached first, it will delete the
 * last character from the search string.
 *
 * While searching backward, each successive character will leave the cursor
 * at the beginning of the matched string.	Typing a Control-R will search
 * backward for the next occurrence of the string.	Control-S or Control-X
 * will revert the search to the forward direction.  In general, the reverse
 * incremental search is just like the forward incremental search inverted.
 *
 * In all cases, if the search fails, the user will be beeped, and the search
 * will stall until the pattern string is edited back into something that
 * exists (or until the search is aborted).
 */
static
int Pascal isearch(int n)

{ int cpos = 0;
	int col = 0;

	/*char	tpat[NPAT+20]; */
	int  cc = -1;

	if (!g_clexec) 
	{ /*expandp(&tpat[0],TEXT165,pat,"]<META>: ",NPAT/2-5);** add old pattern */
		 col = mlwrite(TEXT165/*tpat*/);
							/*			"ISearch: " */
							/* ask the user for the text of a pattern */
	}

	if (n < 0)
		backbychar( 1);
	  
	/* Get the first character in the pattern. If we get an initial Control-S
		 or Control-R, re-use the old search string and find the first occurrence
	 */
	for (;;)			/* ISearch per character loop */
	{ int raw = get_char();
		if (raw == sterm)
			 return TRUE;
	{ int c = ectoc(raw);
													/* Check for special characters first: */
		switch (c)						/* Most cases here change the search */
		{ 
			case IS_ABORT:			/* If abort search request		*/
				return FALSE; 		/* Quit searching again 			*/

#if S_VMS
			case IS_VMSFORW: c = IS_FORWARD;
#endif
			case IS_REVERSE:
			case IS_FORWARD: n = (c - IS_REVERSE)*2 - 1;

				if (cpos == 0)
				{ char ch;
					backbychar(1); 														/* Be defensive about EOB  */
					for (cpos = -1; (ch = pat[++cpos]) != 0; )/* find the length */
						col = echochar(ch, col);								/* and re-echo the string */
				}
	 
				goto sm;
#if 0
			case IS_QUOTE:
#if S_VMS
			case IS_VMSQUOTE:
#endif
				c = ectoc(get_char());						/* Get the next char*/
#endif
			case IS_NEWLINE:
			case IS_TAB:
			case '\n':
				break;														/* Make sure we use it */

			case IS_BACKSP:
			case IS_RUBOUT:
				g_isb.cmd_offset -= 2;				/* Back up over the Rubout */
				if (g_isb.cmd_offset <= 0)		/* Anything to delete?		 */
					return TRUE;							/* No, just exit				*/

				g_isb.cmd_buff[g_isb.cmd_offset] = '\0';/* Yes, delete last char	 */
				return -1;

			default:
				if (c < ' ')			/* Is it printable? 			*/
				{ 				/* Nope.					*/
//				reeat(c); 		/* Re-eat the char				*/
					return TRUE;
				}
		}  /* Switch */

		pat[cpos] = c;				/* put the char in the buffer */
		pat[++cpos] = 0;			/* null terminate the buffer	*/
		if (cpos >= NPAT-1) 		/* too many chars in string?	*/
		{ 																		/* Yup.  Complain about it		*/
//		mlwrite(TEXT166);
					/* "? Search string too long" */
			return TRUE;
		}
		col = echochar(c,col);								/* Echo the character 			*/
		if (cc == 0)
			continue;
		if (n > 0)
			c = 0;
		else
		{ forwchar(TRUE, cpos+1);
			cc = scanner(-1, 2);
			if (cc)
				continue;
			forwchar(TRUE, -(cpos+1));
		}
//	 ||
//			checknext(cpos, n)) 							/* See if match 						*/
//			continue;
sm:
/*
 * This hack will search for the next occurrence of <pat> in the buffer, either
 * forward or backward.  It is called with the status of the prior search
 * attempt, so that it knows not to bother if it didn't work last time.  If
 * we can't find any more matches, "point" is left where it was before.  If
 * we do find a match, "point" will be at the end of the matched string for
 * forward searches and at the beginning of the matched string for reverse
 * searches.
 */ 				/* search forward or back for pattern */
						/* direction to search					*/

		cc = scanner(n, (c == IS_FORWARD) | 2);
		if (!cc)
			TTbeep(); /* Beep if search fails 			*/
	}} /* for {;;} */
}




/* Subroutine to do incremental search.
 */

int Pascal fisearch(int f, int n)

{ /*char	pat_save[NPAT+1]; 				* Saved copy of the old pattern str */
		Lpos_t save = *(Lpos_t*)&curwp->w_dotp;/* Save the current position */
		T_is * is = &g_isb;
						/* Initialize starting conditions */
		is->cmd_reexecute = -1;						/* We're not re-executing (yet?)	*/
		is->cmd_offset = 0; 							/* Start at the beginning of the buff */
		is->cmd_buff[0] = '\0';						/* Init the command buffer		 */
	/*strpcpy(pat_save, pat, NPAT);			 * Save the old pattern string */

{ 	int srchres;
		while ((srchres = isearch(n)) <= 0)
		{ rest_l_offs(&save); 						/* Reset the position 			*/
			if (srchres == 0) break;
			/*strpcpy(pat, pat_save, NPAT);  * Restore the old search str */
			is->cmd_reexecute = 0;					/* Start the whole mess over	*/
		}
		if (srchres)
			mlerase();											/* If happy, just erase the cmd line */
		else 
		{ // curwp->w_flag |= WFMOVE;
			// update(FALSE);
			mlwrite (TEXT79);
					/* "Not found" */
		}
		return srchres;
}}



int Pascal risearch(int f, int n)

{ return fisearch(f, n ^ 0x80000000);
}

#endif
