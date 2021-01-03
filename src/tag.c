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
/*
 * Created:     Unknown
 * Synopsis:    Find tag positions.
 * Authors:     Mike Rendell of ROOT Computers Ltd, Jon Green & Steven Phillips.
 * Description:
 *     Find tags from a tag file. Requires a 'tags' file listing all defined
 *     tgas in the form:
 *         tag<tab>file<tab>search-pattern
 */

#define	__TAGC			/* Define filename */

#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable : 4996)


#define STANDALONE 0

#if STANDALONE == 0

#include "estruct.h"
#include "edef.h"
#include "etype.h"

#else
									/* test rig */
#include  <sys/stat.h>
#include  <string.h>

#define Bool int
#define false 0
#define true 1


char * strpcpy(char * t_, const char * s_, int n_)
{ register short n = n_;
  register const char * s = s_;
  register char * t = t_;
 
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


static char fspec[256+2];	/* full path spec to search */

					/* the text in dir following the last / is ignored*/
char * fex_up_dirs(const char * dir, const char * file)

{ if (dir != NULL)
  { register int clamp;

    for (clamp = -1; ++clamp < sizeof(fspec)-2 && dir[clamp] != 0; )
      fspec[clamp] = dir[clamp];

    fspec[clamp] = 0;

    for (clamp = 8; --clamp >= 0; )
    { char * pc = pathcat(&fspec[0], sizeof(fspec), fspec, file);
      loglog1("Trytag %s", pc);
      if (fexist(pc))
        return fspec;
      (void)pathcat(fspec, sizeof(fspec), fspec, "../a");
    }
  }
  
  return NULL;  
}

										/* end test rig */
#endif

#if MEOPT_TAGS || STANDALONE

#define TAG_MIN_LINE 6
static char * g_tagLastFile = NULL;
static int    g_tagLastStart = 0;
static char * g_tagLastName = NULL;

/* findTagInFile
 * 
 * Search for a tag in the give tag filename
 * 
 * returns
 *    0 - Found tag
 *    1 - Found tag file but not tag
 *    2 - Found nothing
 */
static int
findTagInFile(const char *tags, const char *key, int * stt_ref, char * tagline)

{  int fd_cc = 1;

  char keybuf[100];
  int iter;
  FILE * fp = fopen((char *)tags, "rb");
  if (fp == NULL)
    return 2 ;

  strpcpy(keybuf, key, sizeof(keybuf));

  for (iter = 2; --iter >= 0; )
                    	     /* Read in the tags file */
  { fseek(fp, 0L, 2);

  { char linebuf[1025];
    int end = ftell(fp);						/* points after newline */
    int start = *stt_ref;						/* points after newline */
    int pos = (start+end) >> 1;
    linebuf[0] = 0;

    for (;;)
    { /*mlwrite("%d Seek %s from %d in %d\n", start, key, pos, end);ttgetc();*/
      fseek(fp, pos, 0);
      if (pos > start)
      { while (++pos < end && (fgetc(fp) != '\n'))
	      ;
	  }
	  if (pos >= end)
	  {/*mlwrite("B %d %d %d\n", pos, start, end); ttgetc();*/
	    if (pos == start)
	      break ;
		pos = start ;
	  }
	  else				     /* Get line of info */
	  { int ix;
	    char * ln = fgets(linebuf, sizeof(linebuf)-1, fp);
	    if (ln == NULL || ln[0] == 0)
		  break;

		if (ln[0] == '\n')
		{ pos += 1;
		  if (pos >= end)
		    pos = start;
		  continue;
		}

	  /*mlwrite("Try %d %s\n", pos, ln);ttgetc();*/
		for (ix = -1; linebuf[++ix] != 0 && linebuf[ix] != '\t'; )
		  ;
		if (linebuf[ix] != 0)
		{ linebuf[ix] = 0;
		  for (; linebuf[++ix] != 0; )
		    ;
		}
		if (linebuf[ix-1] == '\n')
		  linebuf[ix-1] = 0;
	  { int tmp = strcmp(keybuf, linebuf);
	  /*mlwrite("%d Cmp %s,%s\n", tmp, key, linebuf);ttgetc();*/
	    if (!tmp)						/* found */
	    { end = pos;
	      fd_cc = 0;
	    /*mlwrite("Fd %s\n", linebuf+strlen(linebuf)+1);ttgetc();*/
		  memcpy(tagline, linebuf, ix+2);
		  *stt_ref = pos+ix;			/* point after newline */
		}
		else if (tmp > 0)				/* forward */
		  start = pos+ix;
		else 
		  end = pos;
		pos = ((start+end) >> 1) ;
	  }}
    } /* loop */
    if (fd_cc == 0)
      break;
  { int ix;
    char * ln;
	if (linebuf[0] == 0)
	  break;
	ln = fgets(linebuf, sizeof(linebuf)-1, fp);
	if (ln == NULL || ln[0] == 0)
	  break;

    for (ix = -1; linebuf[++ix] != 0 && linebuf[ix] != ':'; )
	  ;
		
    if (linebuf[ix] == 0 || keybuf[ix] != 0)
      break;
    linebuf[ix] = 0;
    strpcpy(keybuf+ix, "::", 100-ix);
    strpcpy(keybuf+ix+2, linebuf, 98-ix);
  /*mbwrite(keybuf);*/
  }}}

  fclose(fp);
  return fd_cc;
}


#if STANDALONE

#define MLWRITE(a,b) mlwrite("cant find tag")

#else

#define MLWRITE(a,b) mlwrite(a,b)

#endif
/*
 * Find the function in the tag file.  From Mike Rendell.
 * Return the line of the tags file, and let do_tag handle it from here.
 * Note file must be an array meBUF_SIZE_MAX big
 */
static Bool
findTagSearch(const char * fromfile, const char *key, char *tagline, int tlsz)
{
    static char tagf[] = "../tags";
           char * tagFile;
    	   int clamp = 4;

    if (g_tagLastName != NULL && strcmp(g_tagLastName, key) == 0)
      tagFile = g_tagLastFile;
    else
    { g_tagLastStart = 0;
      tagFile = fex_up_dirs(fromfile, tagf+3);
      if (tagFile == NULL && fexist(tagf+3))
        tagFile = tagf+3;
      if (tagFile == NULL)
	  { mlwrite("[no tag file]");
	    return false;
	  }
	  if (g_tagLastFile != NULL)
	    free(g_tagLastFile);
	  g_tagLastFile = NULL;
	  g_tagLastName = NULL;
    }
   
    for (; tagFile != NULL && --clamp >= 0; )
    {	        /* Get the current directory location of our file and use this
                 * to locate the tag file. */
	/*mbwrite(tagFile);*/
    { int ii = findTagInFile(tagFile, key, &g_tagLastStart, tagline);
      if (ii == 0)			/* found tag */
      {	int sl_tf = strlen(tagFile);
		int sl_tl = strlen(tagline);
		g_tagLastFile = malloc(sl_tf+sl_tl+260*2);
		strcpy(g_tagLastFile,tagFile);
		g_tagLastName = g_tagLastFile+sl_tf+260;
		strcpy(g_tagLastName,tagline);
		return true;
      }
      else	  /* continue search. Ascend tree by getting the directory path */
      {           /* component of our current path position */
        pathcat(tagFile, 260, tagFile, tagf);

		tagFile = fex_up_dirs(tagFile, tagf+3);
		if (tagFile == NULL)
		  break;
		g_tagLastStart = 0;
	  /*printf("Next tag file %s\n", tagFile);*/
	  }
    }}
    
    MLWRITE(g_tagLastFile != null ? "No More Tags" : "[tag %s not in tagfiles]", key);
    return false;
}

#if STANDALONE

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
    
    while (findTagSearch(".", tag, tagline, sizeof(tagline-1)) && --clamp >= 0)
	{	char * tl = tagline+strlen(tagline);
		while (*++tl != 0 && isspace(*tl))
		  ;
	    printf("FOUND %s %s\n", tagline, tl);
	}
	return 0;
}}


#else


static	int
findTagExec(const char tag[])
{
    char * file;	/* File name */
    int  ix;
    char tagline[1025];
    char fullfilename[256+1];
    char * fn = curbp->b_fname == NULL ? "." : curbp->b_fname;
    
    if (!findTagSearch(fn, tag, tagline, sizeof(tagline)-1))
        return false ;
    
    file = skipspaces(tagline + strlen(tagline) + 1, tagline+1024);

    if (file[0] == '=')
    {   for (ix = -1; file[++ix] > 0xd; )
          ;
        file[ix] = 0;
     	mlwrite("%s = %s", tagline, file+2);
        return true;
    }

	for (ix = -1; file[++ix] != 0 && !isspace(file[ix]); ) /* to tab */
	  ;
		      
	if (file[ix] == 0)
	  file[ix+1] = 0;
	else
	  file[ix] = 0;

{	char * fn = pathcat(fullfilename, sizeof(fullfilename), g_tagLastFile, file);
    BUFFER * bp = bufflink(fn, true);
    if (bp == NULL)
    {	mlwrite("Cannot find file %s", file);
    	return false;
    }

	swbuffer(bp);
			    /* now convert the tag file search pattern into a magic search string */
{
	char cc, *dd, ee ;
    char * ss = skipspaces(file+ix+1, tagline+sizeof(tagline)-1);	/* magic pattern */

        /* if the first char is a '/' then search forwards, '?' for backwards */
    ee = *ss++ ;
    if (ee == '?')
		gotoeob(0, 0);
    else
    {   gotobob(0, 0);
		if (ee != '/')
		{
		    ss = "";
		}
	}

    strpcpy(tagline,pat,1024);

	if (ss[0] == 0)
	{ strpcpy(pat, tag, sizeof(pat)-20);
	  strcat(pat, "[^A-Za-z0-9_]");
	}
	else			/* look for the end '/' or '?' - start at the end and look backwards */
	{	dd = ss + strlen(ss) ;
	    while (--dd != ss)
	      if (*dd == ee)
	      { *dd = 0;
	        break ;
	      }
		    
		dd = pat;
		
		while ((cc=*ss++) != 0 && dd - pat < sizeof(pat) - 3)
		{
			if (cc == '\\')
	    	{   *dd++ = '\\' ;
	        	*dd++ = *ss++ ;
	    	}
	    	else
	    	{/*	if (cc == '[' || cc == '*' || cc == '+' ||
				    cc == '.' || cc == '?' || cc == '$')
					*dd++ = '\\' ;*/
				*dd++ = cc ;
			}
		}
	
		if (dd[-1] == '$')
		    (--dd)[-1] = '$';
		*dd = 0 ;
	}

    mcstr(MDMAGIC);

    ix = hunt(ee != '?' ? 1 : -1, 1);
    strcpy(pat,tagline);
    mcstr(-1);
    return ix;
}}}

/*ARGSUSED*/

int
findTag(int f, int n)
{
    char  tag[1024+1];
    
    /* Determine if we are in a word. If not then get a word from the user. */
    if ((n & 0x02) || !inword())
    {
	/*---	Get user word. */
		strcpy(tag, "Tag? ");   
    {   int cc = mltreply(tag, tag, sizeof(tag)-1);
    	if (cc != true)
            return false;
    }}
    else
    {	char * ss = curwp->w_dotp->l_text;
		int  ll   = curwp->w_dotp->l_used;
        int  offs = curwp->w_doto;
													/* Go to the start of the word. */
        while (--offs >= 0 && (isword(ss[offs]) || ss[offs] == ':'))
            ;
        ++offs;
        ll -= offs;
        if (ll >= sizeof(tag))
          ll = sizeof(tag)-1;
        
    {	int len = 0;
        for (len = -1; ++len < ll && (isword(ss[offs]) || ss[offs] == ':'); )
            tag[len] = ss[offs++] ;
        tag[len] = 0 ;
        if (len > 1 && tag[len-1] == ':' && tag[len-2] != ':') tag[len-1] = 0;
    {   char * uctag;
        const char * sw = (g_clring & BCFOR)  ? "1" : 
                          (g_clring & BCCOMT) ? "0" : gtusr("uctags");
        if (!in_range(*sw, '0', '1'))
          sw = "0";
//      mlwrite("Sw is >%s<", sw);
//      ttgetc();
        if (sw != NULL && atoi(sw) != 0)
          for (uctag = tag; *uctag != 0; ++uctag)
            if (in_range(*uctag, 'a', 'z'))
              *uctag += 'A' - 'a';
    }}}

    return findTagExec(tag);
}

#endif

#endif

