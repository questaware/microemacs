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
#endif


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

#define TAGBUFFSZ 1024

class Tag
{
#define LOOK_BACK 2048
#define TAG_MIN_LINE 6
	static int	  g_lastClamp;
	static int    g_LastStart;
	static char * g_LastName;
	static char * g_tagline;
	static char * g_tagfile;

	static FILE * g_fp;

	static int get_to_newline(void);
	static int findTagInFile(const char *key);
 public:
	static int findTagExec(const char key[]);
};

int	  Tag::g_lastClamp;
int   Tag::g_LastStart;
char *Tag::g_LastName = NULL;
char *Tag::g_tagline;
char *Tag::g_tagfile;

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
 *    0 - Found tag
 *    1 - Found tag file but not tag
 *    2 - Found nothing
 */
int Tag::findTagInFile(const char *key)

{ FILE * fp = g_fp = fopen(g_tagfile, "rb");
  if (fp == NULL)
    return 2 ;

{ int fd_cc = 1;
  char * tagline = Tag::g_tagline;
  int start = Tag::g_LastStart;			/* points after newline */
  int end = start;
  int pos = start;
  int seq = start;				/* < 0 : chop, 0 : never, > 0 fseek to here */
  if (seq >= 0)
  { if (seq > 0)
	  fseek(fp, seq, 0);

  { int got = get_to_newline();
    pos += got;
//  Tag::g_LastStart = pos;					/* point after newline */
	if (got)
      fd_cc = strcmp(key, tagline);
  }}
  else
  {	int last_pos = -1;
	start = 0;
	fseek(fp, 0L, 2);
    end = ftell(fp);						/* points after newline */
  
    do
  	{ if (seq < 0)
	  { pos = (start+end) >> 1;
	    fseek(fp, pos, 0);
	  { int got = get_to_newline();
		pos += got;
	  }}
	  											/* Get line of info */
    { int got = get_to_newline();
	  pos += got;
      fd_cc = got == 0 					 ? 0 : 
			  last_pos == pos && seq < 0 ? 0 : strcmp(key, tagline);
	  if 		(fd_cc > 0)					/* forward */
	    start = pos;
	  else if (fd_cc < 0)					/* backward */
	  { last_pos = pos;
		end = pos - got;
	    if (seq >= 0)
	  	  break;
	  }
	  else
	  { // Tag::g_LastStart = pos;				/* point after newline */
	    if (seq >= 0)
	  	  break;

		while (1)
		{ pos =- LOOK_BACK;
		  if (pos < 0)
		    pos = 0;
	  	  fseek(fp, pos, 0);
		  if (pos > 0)
		  { pos += get_to_newline();
		  { int g = get_to_newline();
			if (g > 0 && strcmp(key, tagline) >= 0)
			  continue;
		  }}
		  break;									/* at 0 or before target */
		}

	    seq = 0;									/* No more seeking */
	  }
    }} while (1);
  }

  Tag::g_LastStart = pos;					/* point after newline */

  fclose(fp);
  return fd_cc;
}}


#if STANDALONE

#define MLWRITE(a,b) mlwrite("cant find tag")

#else

#define MLWRITE(a,b) mlwrite(a,b)

#endif


int Tag::findTagExec(const char key[])

{	const int sl_ = NFILEN + 10; 	// Must allow fn to change for same key !!
	const char tagfname[] = TAGFNAME;

	if (Tag::g_LastName == NULL)
	  Tag::g_LastName = (char*)aalloc(sl_);

{	char tagfile[NFILEN];
	char tagline[TAGBUFFSZ+1];

	int clamp = Tag::g_lastClamp + 1;
	int state = Tag::g_lastClamp > 0 &&
				strcmp(Tag::g_LastName, key) == 0;
	if (state)
		strcpy(tagfile, g_tagfile);
	else
	{   Tag::g_LastStart = -1;
		Tag::g_tagfile = tagfile;			/* Local storage */
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
	{	int z = Tag::findTagInFile(key);
		if (z == 0)			/* found tag */
			break;
    }}

	Tag::g_lastClamp = clamp;
	if (clamp < 0)
		return state + 256;
    
	if (g_tagfile != tagfile)
		free(g_tagfile);
	g_tagfile = strdup(tagfile);

	strcpy(Tag::g_LastName, tagline);

{   int ix;
	char * file = skipspaces(tagline + strlen(tagline) + 1, tagline + TAGBUFFSZ);

	for (ix = -1; file[++ix] > ' '; ) /* to next tab */
	  ;

{	char * fn = file + ix;
	fn[file[ix] == 0] = 0;
{	char filenm[256];
    BUFFER * bp = bufflink(pathcat(filenm,sizeof(filenm), tagfile, file),
				   		   1 + 64);
	if (bp == NULL)
	{	mlwrite(TEXT214, filenm);
    	return -1;
    }

	file = skipspaces(fn + strlen(fn) + 1, tagline + TAGBUFFSZ);

		    			/* convert search pattern to magic search string */
	        	/* if the first char is '/' search forwards, '?' for backwards */
{	char typ = *file;
	if (typ == '?')
		gotoeob(0, 0);
	else
		gotobob(0, 0);

	strpcpy(tagline,pat,1024);			/* save for restore */

	if (typ == 0)	
	    strcat(strpcpy(pat, key, sizeof(pat)-20), "[^A-Za-z0-9_]");
	
	else	/* look for end '/' or '?' - start at end and look backwards*/
	{	char ch;
#if 0
		char * dd = strlast(file+1,ee);
		*dd = 0;
#else
		char * dd = NULL;
		char * s = file;
		while (*++s != 0)
			if (*s == typ)
				dd = s;

		if (dd != NULL)
			*dd = 0;
#endif		
		dd = pat - 1;
		
		while ((ch=*++file) != 0 && dd < pat + sizeof(pat) - 3)
		{
			*++dd = ch ;
			if (ch == '\\') 	/* backslash must be escaped */
		   	    *++dd = '\\' ;
		}

		*++dd = 0 ;
	}

    mk_magic(MDMAGIC);

{   int rc = hunt(typ != '?' ? 1 : -1, 1);
    strcpy(pat,tagline);
    mk_magic(-1);
    return rc;
}}}}}}}

/*ARGSUSED*/

extern "C"
{

int
findTag(int f, int n)
{
    char  tag[1024+1];
					    	/* If we are not in a word get a word from the user.*/
    if ((n & 0x02) || !inword())
    {											/*---	Get user word. */
    	int cc = mltreply("Tag? ", tag, sizeof(tag)-1);
    	if (cc != true)
            return -1;
    }
    else
    {	char * ss = curwp->w_dotp->l_text;
		int  ll   = curwp->w_dotp->l_used;
        int  offs = curwp->w_doto;
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

		if ((g_clring & BCCOMT) == 0)
			if ((g_clring & BCFOR) || atoi(gtusr("uctags")))
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
			res = Tag::findTagExec(tag);
		}
	}
	if (res == true)
		return true;

	if (res > 0)
	{ 	res -= 256;
		MLWRITE(res == 0 ? TEXT142 : /* "[no tag file]" */
				res & 1  ? TEXT160 : /* "No More Tags" */ 
						   TEXT161, tag); /* "[tag %s not in tagfiles]" */
	}
	return false;
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
