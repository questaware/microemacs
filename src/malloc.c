/*	MALLOC.C: General purpose Memory Allocation routines

*/
#include	<stdio.h>
#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"

#if WRAP_MEM

#undef	malloc
#undef  calloc
#undef  strdup
#undef	free


#define MEM_TAG (0x79)

#define	RAMCHK  0       /* wrap mallocs, frees in checks */
#define	RAMSCAN 0       /* scan all areas at mallocs or frees */

#define	RAMTRCK	0	/* send debug info to MALLOC.DAT */
#define	RAMSHOW	0	/* auto dynamic RAM reporting */

#if RAMCHK == 0
#undef  RAMSCAN
#define RAMSCAN 0
#endif


struct MemCell					/* for documentation only, not used */
{ 
#if RAMSCAN
   struct MemCell * next;
#endif
#if RAMCHK
   short            size;			/* size of malloc */
#endif
#if LONG_ALGNMT <= 2
#define PAD_SZ 0
#else
#define PAD_SZ LONG_ALGNMT-2
   char		    pad[PAD_SZ];
#endif
   char		    c[1];
#if RAMCHK
  /* two bytes at &c[size] *			** tag */
#endif
};



/*	These routines will allow me to track memory usage by placing
	a layer on top of the standard system malloc() and free() calls.
	with this code defined, the environment variable, $RAM, will
	report on the number of bytes allocated via malloc.

	with SHOWRAM defined, the number is also posted on the
	end of the bottom mode line and is updated whenever it is changed.
*/

#if	RAMSHOW
void dspram()	/* display the amount of RAM currently malloced */

{	mlwrite("[%lu ]", envram);
	ttgetc();
}}
#endif


#if RAMSCAN

char * mallist;
int mallct = 0;

scan_mallist()

{ register char * lst;
  register int ct = mallct;

  for (lst = mallist; lst != null && --ct >= 0; lst = *(char**)lst)
  { short nbytes = (lst[SIZEOF_PTR] << 8) + (lst[SIZEOF_PTR+1] & 0xff);

    if (lst[SIZEOF_PTR+2+nbytes] != MEM_TAG or
	lst[SIZEOF_PTR+3+nbytes] != MEM_TAG)
      adb(7);
  }
  if (ct != 0)
    adb(74);
}



rm_mallist(cell)
    char * cell;
{ register char * lst;
           char * prv = (char*)&mallist;
  register int ct = 0;

  for (lst = mallist; lst != null; lst = *(char**)lst)
  { if (lst == cell)
    { *(char**)prv = *(char**)lst;
      return;
    }
    prv = lst;
    ++ct;
  }
  
  --mallct;
  adb(ct);
}

#endif


char * Pascal allocate(unsigned nbytes)
					/* # of bytes to allocate */
{
	char * mp = (char*)malloc(RAMSCAN*SIZEOF_PTR+4*RAMCHK+PAD_SZ+nbytes);
#if	RAMTRCK
	FILE *track = fopen("malloc.dat", "a");
	fprintf(track, "Alloc %x at %lx\n", nbytes, mp);
	fclose(track);
#endif
	if (mp == NULL)
	{ mlwrite(TEXT99);
          return NULL;
        }
#if RAMCHK
	envram += nbytes;
#else
	envram += 1;
#endif
#if	RAMSHOW
	dspram();
#endif
#if RAMCHK == 0
        return mp;
#else
#if RAMSCAN
        scan_mallist();
	*(char**)mp = mallist;
	mallist = mp;
  	++mallct;
	mp += SIZEOF_PTR;
#endif
	mp[0] = (nbytes >> 8);
	mp[1] = nbytes;
	mp[2+PAD_SZ+nbytes] = MEM_TAG;
	mp[2+PAD_SZ+1+nbytes] = MEM_TAG;
	return &mp[2+PAD_SZ];
#endif
}


char * Pascal callocate(unsigned nbytes)

{ char * res = allocate(nbytes);		/* mend this for unalligned malloc! */
  if (res != NULL)
    memset(res, 0, nbytes);
  return res;
}


char * Pascal duplicate(const char * src)

{ int sz = strlen(src);
  char * p = allocate(sz);
  if (p != NULL)
    strcpy(p, src);
  return p;
}



void Pascal release(char * mp)
	/*mp;	** chunk of RAM to release */
{
#if	RAMTRCK
	FILE *track = fopen("malloc.dat", "a");
	fprintf(track, "Free %lx\n", mp);
	fclose(track);
#endif
	if (mp == null)
	  TTbeep();
	else
	{ 
#if RAMCHK
	  mp -= 2 + PAD_SZ;
  { int size = (mp[1] & 0xff) + (unsigned)(mp[0] << 8);
	  if (mp[2+PAD_SZ+size] != MEM_TAG or
	      mp[3+PAD_SZ+size] != MEM_TAG)
            adb(mp[2+PAD_SZ+size]);

	  mp[2+PAD_SZ+size] = 0;

#if RAMSCAN
          mp -= SIZEOF_PTR;
          rm_mallist(mp);
#endif

#if RAMCHK
	  envram -= size;
#else
	  envram -= 1;
#endif
#if	RAMSHOW
	  dspram();
#endif
	}
#endif
	  free(mp);
	}
}

#endif
