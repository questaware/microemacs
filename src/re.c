#include   <stdio.h>
#include   <ctype.h>
#include   <sys/types.h>
#include   "build.h"


// Not in use

Char * match_re(Char * tlt_, Char * pat_, Bool ic)
	
{	   Char * pat = pat_;
  register Char * tlt = tlt_;
	   Char * rpt = tlt;
	   Short repeat = 0;
           Bool pass = true;

  while (*pat != 0)
  { Char * spat = pat;

    if      (*pat == '.')
      pass = true;
    else if (*pat == '\\')
    { ++pat;
      pass = *tlt != 0 && (*pat=='@' || in_range(*pat, 'A','Z') ? *pat-'@'   :
					 in_range(*pat, 'a','z') ? *pat-'a'+1 :
								   *pat)== *tlt;
    }
    else if (*pat == '[')
    { Bool compl_set = *++pat == '^';
      pass = false;

      if (! compl_set)
        --pat;
      while (*++pat != 0)
        if      (*tlt == *pat)
          pass = ! compl_set;
        else if (*pat == ']')
          break;
    }
    else if (! ic ? *pat == *tlt : toupper(*pat) == toupper(*tlt))
      pass = true;
    else
      pass = false;
    
    ++pat;
    if (pass)
    { repeat += 1;
      if      (*pat == '*' || *pat == '+' && pat[-1] == ']')
        pat = spat;
      else
      { repeat = 0;
        if (*pat == '?' && pat[-1] == ']')
	  ++pat;
      }
    }
    else
    { if      (*pat == '*' || *pat == '?' && pat[-1] == ']')
        repeat = 0;
      else if (*pat == '+' && pat[-1] == ']')
      { if (repeat == 0)
          break;
        repeat = 0;
      }
      else
	break;
      ++pat;
    }
    ++tlt;      
  }

  return ! pass ? tlt_ : tlt;
}
