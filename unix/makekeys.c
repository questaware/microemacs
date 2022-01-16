
#include	<stdio.h>
#include	<stdlib.h>

#include	"estruct.h"


char * tr_esc(char * s)

{ static char buf[30];
  char * t = buf;
  int lim = sizeof(buf) - 3;
  
  for ( ; *s != 0 && --lim > 0; ++s)
  { if (*s != 27)
      *t++ = *s;
    else
    { *t++ = '\\';
      *t++ = '0';
      *t++ = '3';
      *t++ = '3';
      lim -= 3;
    }
  }
  
  *t++ = 0;
  return buf;
}


char * ktbl[] =
{
"kcub1\",/* left cursor */193,	SPEC | 'B'",
"kcud1\",/* down cursor */117,	SPEC | 'N'",
"kcuf1\",/* right cursor */211,SPEC | 'F'",
"kcuu1\",/* up cursor */247,	SPEC | 'P'",
"kdch1\",/* delete character */115,SPEC | 'D'",
"kf1\",  125,	SPEC | '1'",
"kf10\", 134,	SPEC | '0'",
"kf11\", 135,	SPEC | ':'",
"kf12\", 136,	SPEC | ';'",
"kf2\",  126,	SPEC | '2'",
"kf3\",  127,	SPEC | '3'",
"kf4\",  128,	SPEC | '4'",
"kf5\",  129,	SPEC | '5'",
"kf6\",  130,	SPEC | '6'",
"kf7\",  131,	SPEC | '7'",
"kf8\",  132,	SPEC | '8'",
"kf9\",  133,	SPEC | '9'",
"khome\",/* home */  		     190, SPEC | '<'",
"kich1\",/* insert character */191,SPEC | 'C'",
"kll\",  /* home down [END?] key */ 194, SPEC | '>'",
"knp\",  /* next page */            199, SPEC | 'V'",
"kpp\",  /* previous page */        202, SPEC | 'Z'",
};

char * kname[] =
{
"left cursor",
"down cursor",
"right cursor",
"up cursor",
"delete",
"F1",
"F10",
"F11",
"F12",
"F2",
"F3",
"F4",
"F5",
"F6",
"F7",
"F8",
"F9",
"home",
"insert",
"end",
"next page",
"previous page",
#if 0
"alt left cursor",
"alt down cursor",
"alt right cursor",
"alt up cursor",
"alt delete",
"ctrl left cursor",
"ctrl down cursor",
"ctrl right cursor",
"ctrl up cursor",
"ctrl delete",
#endif
};


int order[] = { 0,1,2,3,4,5,9,10,11,12,13,14,15,16,
 6,7,8,17,18,19,20,21
 };


int main(int argc, char *argv[])
{ 
	char buf[30];
  int lim = sizeof(order) / sizeof(order[0]);
  int i;
  
  fprintf(stderr, "Press keys followed by return (just return skips)\n");
  for (i = -1; ++i < lim; )
  { char * ln;
    fprintf(stderr, "The %s key:", kname[order[i]]);
    ln = fgets(&buf[0], 30, stdin);
    if (ln == null)
      break;

    ln[strlen(ln)-1] = 0;
    
    if (ln[0] != 0)
      printf("{ \"%s, \"%s\" },\n", ktbl[order[i]], tr_esc(ln));
  }
  
  
  exit(0);
}

