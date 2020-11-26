/* The functions in this file implement commands that perform incremental
 * searches in the forward and backward directions.  This "ISearch" command
 * is intended to emulate the same command from the original EMACS 
 * implementation (ITS).  Contains references to routines internal to
 * SEARCH.C.
 *
 * HISTORY:
 *
 *	D. R. Banks 9-May-86
 *	- added ITS EMACSlike ISearch
 *
 *	John M. Gamble 5-Oct-86
 *	- Made iterative search use search.c's scanner() routine.
 *	  This allowed the elimination of bakscan().
 *	- Put isearch constants into estruct.h
 *	- Eliminated the passing of 'status' to scanmore() and
 *	  checknext(), since there were no circumstances where
 *	  it ever equalled FALSE.
 */

#include	<stdio.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"

#if	ISRCH

/*
 * Incremental search defines.
 */
#define IS_ABORT	0x07	/* Abort the isearch */
#define IS_BACKSP	0x08	/* Delete previous char */
#define	IS_TAB		0x09	/* Tab character (allowed search char) */
#define IS_NEWLINE	0x0D	/* New line from keyboard (Carriage return) */
#define	IS_QUOTE	0x11	/* Quote next character */
#define IS_REVERSE	0x12	/* Search backward */
#define	IS_FORWARD	0x13	/* Search forward */
#define	IS_VMSQUOTE	0x16	/* VMS quote character */
#define	IS_VMSFORW	0x18	/* Search forward for VMS */
#define	IS_QUIT		0x1B	/* Exit the search */
#define	IS_RUBOUT	0x7F	/* Delete previous character */

/* IS_QUIT is no longer used, the variable metac is used instead */

void Pascal reeat(int ch);


/* A couple more "own" variables for the command string */

#if CMDBUFLEN * 2 > NSTRING
 cmdbuflen too big
#endif

NOSHARE char pat[NPAT+10];		/* Search pattern	*/


/*#define cmd_buff ((int*)gs_buf) ** Save the command args here */

typedef struct
{ short cmd_buff[CMDBUFLEN];
  int   cmd_offset;		/* Current offset into command buff */
  int   cmd_reexecute;	        /* > 0 if re-executing command */
} T_is;

static T_is isb;

/* routine to echo i-search characters */

int Pascal echochar(int c, int col)
					/* character to be echoed */
					/* column to be echoed in */
{
  tcapmove(term.t_nrowm1,col);
  switch (c)
  {
	case '\r':
	  col += 3;
	  mlputs("<NL>");

	when '\t':
	  col += 4;
	  mlputs("<TAB>");

	when 0x7F:
          c = '?' - 'A' + 1;
					/* drop through */
	default:
          if (c < ' ')
	  { c += 'A' - 1;
	    col++;
	    mlout('^');
	  }
	  mlout(c);
  }
#if S_MSDOS == 0
  TTflush();
#endif
  return col+1;
}



/* Subroutine to do incremental search.
 */

int Pascal fisearch(int f, int n)

{ /*char  pat_save[NPAT+1];         * Saved copy of the old pattern str */
    Lpos_t save = *(Lpos_t*)&curwp->w_dotp;/* Save the current position */
    T_is * is = &isb;
				    /* Initialize starting conditions */
    is->cmd_reexecute = -1; 	/* We're not re-executing (yet?)      */
    is->cmd_offset = 0;		/* Start at the beginning of the buff */
    is->cmd_buff[0] = '\0'; 	/* Init the command buffer	      */
  /*strpcpy(pat_save, pat, NPAT);* Save the old pattern string	      */

{   int srchres;
    while ((srchres = isearch(f, n)) <= 0)
    {	rest_l_offs(&save);		/* Reset the position	      */
        if (srchres == 0) break;
      /*strpcpy(pat, pat_save, NPAT);	 * Restore the old search str */
	is->cmd_reexecute = 0;		/* Start the whole mess over  */
    }
    if (srchres)
        mlerase();			/* If happy, just erase the cmd line  */
    else 
    {   curwp->w_flag |= WFMOVE;
	update(FALSE);
	mlwrite (TEXT164);
/*		 "[search failed]" */
    }
    return srchres;
}}



int Pascal risearch(int f, int n)

{ return fisearch(f, n == 0 ? -1 : -n);
}


/* Trivial routine to ensure that the next character in the search string is
 * still true to whatever we're pointing to in the buffer.  This routine will
 * not attempt to move the "point" if the match fails, although it will 
 * implicitly move the "point" if we're forward searching, and find a match,
 * since that's the way forward isearch works.
 *
 * If the compare fails, we return FALSE and assume the caller will call
 * scanmore or something.
 */
static int Pascal checknext(int poffs, int dir)/* Check next search string chars */
					/* Next char to look for	      */
					/* Search direction		      */
{
			       /* setup the local scan pointer to current "." */
	    Lpos_t cur = *(Lpos_t*)&curwp->w_dotp;
    register int offs = cur.curoff;
    register int buffchar;		/* character at current position      */
	     int i;

    /* dir > 0 => searching forward */
    
    for (i = dir <= 0 ? -1 : poffs-2; pat[++i] != 0;)/* for all of pattern */
    {
	if (offs != llength(cur.curline)) /* If at end of line        */
	    buffchar = lgetc(cur.curline, offs++); /* Get the next char */
	else
	{   buffchar = '\r';        /* And say the next char is NL  */
            offs = 0;
	    cur.line_no += 1;
	    cur.curline = lforw(cur.curline);
	    if (cur.curline->l_props & L_IS_HD)
		return FALSE;        /* Abort if at end of buffer    */
        }
        if (!myeq(buffchar, pat[i]))  /* Is it what we're looking for?*/
            return FALSE;            /* Nope, just punt it then        */
        if (dir > 0)
        {   cur.curoff = offs;
            rest_l_offs(&cur);        /* point to the matched character     */
            curwp->w_flag |= WFMOVE;
            break;
        }
    }
    return TRUE;
}


/* Subroutine to do an incremental search.  In general, this works similarly
 * to the older micro-emacs search function, except that the search happens
 * as each character is typed, with the screen and cursor updated with each
 * new search character.
 *
 * While searching forward, each successive character will leave the cursor
 * at the end of the entire matched string.  Typing a Control-S or Control-X
 * will cause the next occurrence of the string to be searched for (where the
 * next occurrence does NOT overlap the current occurrence).  A Control-R will
 * change to a backwards search, META will terminate the search and Control-G
 * will abort the search.  Rubout will back up to the previous match of the
 * string, or if the starting point is reached first, it will delete the
 * last character from the search string.
 *
 * While searching backward, each successive character will leave the cursor
 * at the beginning of the matched string.  Typing a Control-R will search
 * backward for the next occurrence of the string.  Control-S or Control-X
 * will revert the search to the forward direction.  In general, the reverse
 * incremental search is just like the forward incremental search inverted.
 *
 * In all cases, if the search fails, the user will be feeped, and the search
 * will stall until the pattern string is edited back into something that
 * exists (or until the search is aborted).
 */
Pascal isearch(int f, int n)

{ register int c;
  register int cpos = 0;
           int col = 0;

  /*char  tpat[NPAT+20]; */
  int  cc = -1;

   if (!g_clexec) 
   { /*expandp(&tpat[0],TEXT165,pat,"]<META>: ",NPAT/2-5);** add old pattern */
     col = mlwrite(TEXT165/*tpat*/);
							/*			"ISearch: " */
				      /* ask the user for the text of a pattern */
   }

   if (n < 0)
     backchar(FALSE, 1);
 /*
    Get the first character in the pattern. If we get an initial Control-S
    or Control-R, re-use the old search string and find the first occurrence
  */
   for (;;)			/* ISearch per character loop */
   {
		c = get_char();
										/* Check for special characters first: */
										/* Most cases here change the search */
		switch (c)				/* dispatch on the input char */
		{ case -1:
		    return TRUE;
		  case IS_ABORT:			/* If abort search request    */
		    return FALSE;			/* Quit searching again       */

		  case IS_FORWARD:
#if S_VMS
		  case IS_VMSFORW:
#endif
		  case IS_REVERSE:
		    n = c == IS_REVERSE ? -1 : 1;
		    if (cpos == 0)
		    { backchar (TRUE, 1); 									/* Be defensive about EOB  */
     	    for (cpos = -1; (c = pat[++cpos]) != 0; ) 	/* find the length */
					 	col = echochar(c, col); 						/* and re-echo the string */
		    }
   
		    goto sm;				/* Start the search again     */

		  case IS_QUOTE:
#if S_VMS
		  case IS_VMSQUOTE:
#endif
		    c = get_char();			/* Get the next char	      */

  		case IS_NEWLINE:
		  case IS_TAB:
		  case '\n':
		    break;				/* Make sure we use it	      */

		  case IS_BACKSP:
		  case IS_RUBOUT:
		    isb.cmd_offset -= 2;				/* Back up over the Rubout */
		    if (isb.cmd_offset <= 0)		/* Anything to delete?	   */
					return TRUE;							/* No, just exit	      */

		    isb.cmd_buff[isb.cmd_offset] = '\0';/* Yes, delete last char   */
		    return -1;

		  default:
		    if (c < ' ')			/* Is it printable?	      */
		    {					/* Nope.		      */
					reeat(c);			/* Re-eat the char	      */
					return TRUE;
		    }
		}  /* Switch */

		pat[cpos] = c;				/* put the char in the buffer */
		pat[++cpos] = 0;			/* null terminate the buffer  */
		if (cpos >= NPAT-1)			/* too many chars in string?  */
		{					/* Yup.  Complain about it    */
  		mlwrite(TEXT166);
					/* "? Search string too long" */
	    return TRUE;			/* Return an error	      */
		}
		col = echochar(c,col);			/* Echo the character	      */
		if (cc == 0 ||
	    checknext(cpos, n))			/* See if match	      	      */
	  continue;
sm:
/*
 * This hack will search for the next occurrence of <pat> in the buffer, either
 * forward or backward.  It is called with the status of the prior search
 * attempt, so that it knows not to bother if it didn't work last time.  If
 * we can't find any more matches, "point" is left where it was before.  If
 * we do find a match, "point" will be at the end of the matched string for
 * forward searches and at the beginning of the matched string for reverse
 * searches.
 */					/* search forward or back for pattern */
						/* direction to search		      */
		if (!mcstr(0))
		  return FALSE;

	 	cc = scanner(n < 0 ? REVERSE : FORWARD, false);
		if (!cc)
		  TTbeep();	/* Beep if search fails       */
	} /* for {;;} */
}


/*
 * Routine to get the next character from the input stream.  If we're reading
 * from the real terminal, force a screen update before we get the char. 
 * Otherwise, we must be re-executing the command string, so just return the
 * next character.
 */

int Pascal get_char ()
{
    register int c;				/* A place to get a character */
//  T_is * is = &isb;
    						/* See if we're re-executing: */

    if (isb.cmd_reexecute >= 0)		/* Is there an offset?		      */
	if ((c = isb.cmd_buff[isb.cmd_reexecute++]) != 0)
	    return ectoc(c);			/* Yes, return any character  */

         /* We're not re-executing (or aren't any more).  Try for a real char */

    isb.cmd_reexecute = -1; 		/* Say we're in real mode again       */
    update(FALSE);			/* Pretty up the screen 	      */
    if (isb.cmd_offset >= CMDBUFLEN-1)
    {
	mlwrite(TEXT166);
/*		 " too long" */
	return -1;
    }
    isb.cmd_buff[++isb.cmd_offset] = '\0';	/* And terminate the buffer   */
    c = getkey();                               /* Save char for next time */
    isb.cmd_buff[isb.cmd_offset-1] = c;
    return c == sterm ? -1 : ectoc(c);
}

#endif
