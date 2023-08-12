#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void explain()

{ puts("fixctagsbug {filename}\n");
	exit(1);
}


int main(int argc, char * argv[])

{ char * fn = argc < 2 ? "tags" : argv[1];

  char * buf = (char *)malloc(strlen(fn) * 3 + 10);

#ifdef _MSC_VER
  sprintf(buf, "copy %s %s.old", fn, fn);
#else
  sprintf(buf, "cp %s %s.old", fn, fn);
#endif
  system(buf);
  
  char * old = strcat(strcpy(buf, fn), ".old");
  
  FILE * ip = fopen(old, "r");
  FILE * op = fopen(fn, "w");
  
  if (ip == NULL || op == NULL)
  	explain();
  	
  while (1)
  { int ch = fgetc(ip);
  	if (ch < 0)
  		break;
  	if (ch == '*')
  		fputc('[', op);
  	fputc(ch, op);
    if (ch == '*')
  		fputc(']', op);
  }
  
  return 0;
}
