/*tab 4*/
/* -*- c -*- */
/*
 * JASSPA MicroEmacs - www.jasspa.com
 * tag.c - Find tag positions.
 *
 * Copyright (C) 1988-2005 JASSPA (www.jasspa.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#define	__TAGC			/* Define filename */

#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable : 4996)

extern "C"
{
#define STANDALONE 0

#if STANDALONE == 0

#include "estruct.h"
#include "edef.h"
#include "etype.h"
#include "elang.h"

#else
									/* test rig */
#include  <sys/stat.h>
#include  <string.h>

#define Bool int
#define false 0
#define true 1

#define is_space(ch) (ch <= ' ')

char * strpcpy(char * t_, const char * s_, int n_)
{ short n = n_;
  const char * s = s_;
  char * t = t_;
 
  if (t != NULL)
  { while (--n > 0 && (*t++ = *s++) != 0)
      ;
  
    if (n == 0)
      *t = 0;
  }
  return t_;
}

#if 0
void ml write(const char * msg)

{ printf("%s\n", msg);
}


int name_mode(const char * s)

{	struct stat fstat_;
	if (stat(s, &fstat_) != 0)
	  return 0;
	return fstat_.st_mode;
}


int fexist(char * fname)	/* does <fname> exist on disk? */
				/* file to check for existance */
{
#if S_BSD | S_UNIX5 | S_XENIX | S_SUN | S_HPUX
  char tfn[NFILEN+2];
  /* nmlze_fname(&tfn[0], fname); */
  return access(fname, 0) == 0;
#else
  return name_mode(fname);
#endif
}

#endif

/* replace the last entry in dir by file */
/* t can equal dir */

char * pathcat(char * t, int bufsz, char * dir, char * file)
	
{ register int tix;

  if (dir[0] == '.' && dir[1] == '/' && dir[2] == '.' && dir[3] == '.' && dir[4] == '/')
    dir += 2;

  if (file[0] == '/' || file[0] == '\\')
    strpcpy(t, file, bufsz);
  else
  { for (tix = -1; ++tix < bufsz - 1 && dir[tix] != 0; )
      t[tix] = dir[tix];

    t[tix] = 0;
   
    while (1)
    { while (--tix >= 0 && t[tix] != '/' && t[tix] != '\\')	/* strip back to / */
        ;

      if (tix > 0)
      { if (t[tix-1] != '.' &&
            file[0] == '.'  && file[1] == '.' && file[2] == '/')
        { for (file = &file[2]; *++file == '/';)		/* strip forward to non / */
            ;
          continue;
        }
      }

      break;
    }
  
  { int six = 0;
    for (; ++tix < bufsz - 1 && file[six] != 0; ++six)
      t[tix] = file[six];

    t[tix] = 0;
  }}
  return t;
}
										/* end test rig */
#endif
}

#if MEOPT_TAGS || STANDALONE

#define TAGBUFFSZ 10000

#if NFILEN < NPAT 
#define TAGFILEN (NPAT+2)
#else
#define TAGFILEN (NFILEN)
#endif

class Tag
{
#define LOOK_BACK 2048
#define TAG_MIN_LINE 6
	static int	  g_lastClamp;
	static int    g_LastStart;
	static char * g_LastName;
	static char * g_tagline;
	static char * g_tagfile;
	static char * g_alt_root;

	static FILE * g_fp;

	static int get_to_newline(void);
	static int findTagInFile(const char *key, const char * tagfile);
 public:
	static int USE_FAST_CALL findTagExec(const char key[]);
};

int	  Tag::g_lastClamp;
int   Tag::g_LastStart;
char *Tag::g_LastName = NULL;
char *Tag::g_tagline;
char *Tag::g_tagfile;
char *Tag::g_alt_root;

FILE *Tag::g_fp;

int Tag::get_to_newline(void)

{	char ch;
	char * ln = fgets(g_tagline, TAGBUFFSZ-1, g_fp);
	if (ln == NULL || ln[0] == 0)
		return 0;
	while ((ch = *++ln) != 0)
	  if (ch <= '\r')				// TAB, CR, LF
		*ln = 0;

	return ln - g_tagline;
}
							// A challenge with this code is to reduce the
							// number of get_to_newline invocation points

/* findTagInFile
 * 
 * Search for a tag in the give tag filename
 * 
 * returns
 *   -1 - Found nothing
 *    0 - Found tag
 *    1 - Found tag file but not tag
 */
int Tag::findTagInFile(const char *key, const char * tagfile)

{ FILE * fp = g_fp = fopen(tagfile, "rb");
  if (fp == NULL)
    return -1;

{ char * tagline = Tag::g_tagline;
  int fd_cc = 1;
  int last_pos = 1;
  int pos = Tag::g_LastStart;			/* points after newline */
  int seq = pos;				/* < 0 : chop, 0 : never, > 0 fseek to here */
  if (seq >= 0)
  { if (seq > 0)
	  fseek(fp, seq, 0);

  { int got = get_to_newline();
	if (got)
    { pos += got;
      fd_cc = strcmp(key, tagline);
    }
  }}
  else
  {	int got = get_to_newline();
	free(Tag::g_alt_root);
	Tag::g_alt_root = NULL;
	if (tagline[0] == ' ')
	  Tag::g_alt_root = strdup(tagline+1);

  {	int start = 0;
	fseek(fp, 0L, 2);
  {	int end = ftell(fp);				/* points after newline */
  
    while (1)
  	{ if (seq < 0)
	  { pos = (start+end) >> 1;
	    fseek(fp, pos, 0);
	  { int got = get_to_newline();
		pos += got;
	  }}
	  									/* Get line of info */
    { int got = get_to_newline();
	  pos += got;
	  if (got == 0)
	  	break;
	  fd_cc = strcmp(key, tagline);
	  if (fd_cc > 0)					/* forward */
	  { start = pos;
		continue;
	  }
	  if (fd_cc < 0)					/* backward */
	  {	if (seq >= 0)
	  	  break;
	 	if (last_pos != pos)
		{ last_pos = pos;
		  end = pos - got;
		  continue;
		}
	  }
	  if (seq < 0)
	  { int g;
	    pos -= LOOK_BACK;
		if (pos < 0)
		  pos = 0;
		while (1)
		{ fseek(fp, pos, 0);
		  pos += get_to_newline();
	  	  g = get_to_newline();
	  	  fd_cc = strcmp(key, tagline);
	  	  if (g == 0 || fd_cc <= 0)
	  	  	break;
		}
	  	pos += g;
	  }
	  break;							/* at 0 or before target */
    }} /* while */
  }}}

  Tag::g_LastStart = pos;					/* point after newline */

  fclose(fp);
  return fd_cc;
}}


#if STANDALONE

#define MLWRITE(a,b) mlwrite("cant find tag")

#else

#define MLWRITE(a,b) mlwrite(a,b)

#endif


int USE_FAST_CALL Tag::findTagExec(const char key[])

{	const int sl_ = NFILEN + 10; 	// Must allow fn to change for same key !!
	const char * const tagfname = TAGFNAME;

	if (Tag::g_LastName == NULL)
	  Tag::g_LastName = (char*)mallocz(sl_);

{	char tagfile[TAGFILEN];
	char tagline[TAGBUFFSZ+1];

	int clamp = Tag::g_lastClamp + 1;
	int state = Tag::g_lastClamp > 0 &&
				strcmp(Tag::g_LastName, key) == 0;
	if (state)
	{	strcpy(tagfile, g_tagfile);
		free(g_tagfile);
	}	
	else
	{   Tag::g_LastStart = -1;
		clamp = 6;
	}

	g_tagline = tagline;
	
	while (--clamp >= 0)
	{
        if (!fexist(state & 1  ? tagfile 		:
					clamp == 0 ? strcpy(tagfile,tagfname+5*3) : 
								pathcat(tagfile, 260, curbp->b_fname, tagfname+clamp*3)))
        	continue;
		
		state |= 2;
	{	int z = Tag::findTagInFile(key,tagfile);
		if (z == 0)			/* found tag */
			break;
    }}

	Tag::g_tagfile = strdup(tagfile);

	Tag::g_lastClamp = clamp;
	if (clamp < 0)
		return state + 4;

	strcpy(Tag::g_LastName, tagline);

	char * fn = tagline;

	BUFFER * bp;
	int iter;
	for (iter = 2; --iter >= 0; )

	{ 	char * file = skipspaces(fn + strlen(fn)+1, tagline+TAGBUFFSZ);

		if (iter > 0)
		{	if (file[0] == '.' && file[1] == '/')
				file += 2;

		    int ix;
			for (ix = -1; file[++ix] > ' '; ) /* to next tab */
	  			;

			fn = file + ix;
			*fn = 0;
			char * root = Tag::g_alt_root == NULL ? tagfile : Tag::g_alt_root;

			bp = bufflink(pathcat(tagfile,sizeof(tagfile), root, file), 1 + 64);
			if (bp == NULL)
				break;
		}
		else    			/* convert search pattern to magic search string */
	    {		  			/* if first char is '/' search forwards, '?' for backwards */
			char typ = *file;
			if (typ == '?')
				gotoeob(0, 0);
			else
				gotobob(0, 0);

#if 0
			strcpy(tagfile,pat);			/* save for restore */
#endif
	    	char * dd = strcat(strcpy(pat, key), "[^A-z0-9_]") - 1;

			if (typ != 0)	
	
						/* look for end '/' or '?' - start at end and look backwards*/
			{	char ch;
				int lim = sizeof(pat) - 3;
				int ix = 0;
				while ((ch = file[++ix]) != 0)
					if (ch == typ)
						lim = ix;

				while (--lim > 0 && (ch=*++file) != 0)
				{	*++dd = ch ;
					if (ch == '\\') 	/* backslash must be escaped */
				   	    *++dd = '\\' ;
				}

				*++dd = 0 ;
			}

			int smode = bp->b_flag;
			bp->b_flag |= MDMAGIC;

			int rc = hunt(typ != '?' ? 1 : -1, 0);
			bp->b_flag = smode;

			if (rc == FALSE)
	    		strcpy(pat,tagfile);

		    return rc;
		}
	}

	mlwrite(TEXT214, tagfile);
   	return -1;
}}

/*ARGSUSED*/

extern "C"
{

int
findTag(int f, int n)
{
    char  tag[1024+1];
#if NPAT+22 > 1024
 error error
#endif
    Bool middle = 1;
					    	/* If we are not in a word get a word from the user.*/
    if ((n & 0x02) || !inword())
    {											/*---	Get user word. */
    	int cc = mlreply("Tag? ", tag, sizeof(tag)-1);
    	if (cc <= FALSE)
            return -1;
    }
    else
    {	char * ss = curwp->w_dotp->l_text;
		int  ll   = llength(curwp->w_dotp);
        int  offs = curwp->w_doto;
        middle = ss[offs - 1] - ':';			// preceding byte is zero
												/* Go to the start of the word.*/
        while (--offs >= 0 && (ss[offs] == ':' || isword(ss[offs])))
            ;
        ll -= offs;
        if (ll >= sizeof(tag)-1)
          ll = sizeof(tag)-2;
        
    {	char ch;
    	int len;
        for (len = -1; ++len <= ll && ((ch = ss[++offs]) == ':' || isword(ch));)
            tag[len] = ch;
        tag[len] = 0 ;
//      while (--len >= 0 && tag[len] == ':')
//       	tag[len] = 0;

		if (g_clring & BCFOR)
        	mkul(1, tag);
    }}

{	int res = Tag::findTagExec(tag);
	if (res != TRUE)
	{	char * s = tag - 1;
		char ch;
		while ((ch = *++s) != 0 && ch != ':')
			;
		if (ch != 0)
		{	*s = 0;
			if (s[1] == ':' && middle == 0)
				res = Tag::findTagExec(s+2);
			if (res != TRUE)
				res = Tag::findTagExec(tag);
		}
	}
//	if (res == TRUE)
//		return TRUE;

	if (res > TRUE)
	{	MLWRITE((res & 3) == 0 ? TEXT142 : /* "[no tag file]" */
				res & 1  	   ? TEXT160 : /* "No More Tags" */ 
						   		 TEXT161, tag); /* "[tag %s not in tagfiles]" */
		return FALSE;
	}
	return TRUE;
}}

#if STANDALONE

typedef BUFF 
{	char * b_fname;
} BUFF;

BUFF buffer;

BUFF * curbp = &buffer;

void explain()

{ printf("tag name\n");
  exit(1);
}




int main(int argc, char * argv[])

{	if (argc != 2)
    	explain();
{	int clamp = 10;
    char * tag = argv[1];
	char tagline[1025];
    strcpy(tagline, "NOVAL");
    
    while (findTagExec(tag) && --clamp >= 0)
	{	char * tl = tagline+strlen(tagline);
		while (*++tl != 0 && is_space(*tl))
		  ;
	    printf("FOUND %s %s\n", tagline, tl);
	}
	return 0;
}}

#endif

#endif

}
