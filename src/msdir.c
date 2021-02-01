/*
#define _POSIX_SOURCE
#define __EXTENSIONS__
*/
				/* NOPUSHPOP:DONT OFFER msd_push(), msd_pop()*/
/*#include        <stdio.h>*/
#include        <string.h>
#include        <ctype.h>
#include        <fcntl.h>
#include        <errno.h>
#include        <sys/types.h>
#include        <sys/stat.h>
# include       <io.h>
#include        "build.h"
#include        "logmsg.h"

#if   S_WIN32
# include        <windows.h>
#if S_CYGWIN
#include 	<unistd.h>
#else
# include        <direct.h>
#endif
/*# include        <dir.h>*/
#elif S_VMS
#include         <stdio.h>
#include         <stdlib.h>
#include         <dirent.h>
#include         <unistd.h>
#elif S_MSDOS
# include        <dos.h>
# include        <io.h>
#else
# include        <stdlib.h>
# if NONDIR == 0
#  include       <dirent.h>
# else
#  if	S_XENIX
#   include	<sys/ndir.h>
#  else
#   include	<ndir.h>
#  endif
# endif
#endif

extern char * strpcpy(char * tgt, const char * src, int mlen);

#include        "msdir.h"
/*#include 	"h/msera.h"*/
/*#include	"h/eprintf.h"*/

/*extern char * strchr();*/

#define NOPUSHPOP 1

#if S_WIN32
#define write _write
#define chdir _chdir
#define getcwd _getcwd
#endif


extern char * getcwd();


#if NONDIR
   #define DIRENT direct
#else
   #define DIRENT dirent
#endif



#define staticc static

#define READAHEAD 0
#define MSD_NOCHDIR (S_BORLAND)
#define MSD_FEWCHDIR 1

#define NOLK (-32000)


#ifndef S_IREAD
# define S_IREAD  S_IRUSR
# define S_IWRITE (S_IRUSR >> 1)
# define S_IFDIR _S_IFDIR
#endif

#if S_MSDOS
 #define DOS_FFILE (0x4e00)
 #define DOS_NFILE (0x4f00)
#else
 #define DOS_FFILE 0
 #define DOS_NFILE 1
#endif

#if S_MSDOS && S_WIN32 == 0

union REGS rg;

Cc ms_intdosx(Char * str, Short code)

{ struct SREGS segreg;

  rg.x.ax = code;
  segreg.ds = ((unsigned long)str) >> 16;
  rg.x.dx = (unsigned int)str;
  intdosx(&rg, &rg, &segreg);
  return rg.x.cflag == 0 ? OK : -rg.x.ax;
}

#endif




void printf2(const char * msg, const char * val)

{ 
#if S_VMS
  mlwrite("%s%s", msg,val);
  ttgetc();
#else
  write(2, msg, strlen(msg));
  write(2, val, strlen(val));
  write(2, "\n", 1);;
#endif
}

#if 0
X
XChar * match_fn_re(tlt_, pat)
X	Char *   tlt_;
X	Char *   pat;
X{ register Char * tlt = tlt_;
X  register Char ch;
X
X  while (*tlt != 0)
X  { if (*pat == '*')
X    { ++pat;
X      while ((ch = *tlt) != 0 && ch != *pat)
X        ++tlt;
X      if (ch == 0)
X        break;
X    { char * tlt__ = tlt;
X      tlt = match_fn_re(tlt, pat);
X      --pat;
X      if (tlt__ == tlt)
X        continue;
X      pat = "";
X      break;
X    }}
X    ch = *tlt;
X    if (ch == 0 || ch != *pat && *pat != '?')
X      break;
X    ++tlt;
X    ++pat;
X  }
X
X  return *pat == 0 ? tlt : tlt_;
X}

#endif


Char * match_fn_re_ic(
	Char *       tlt_,
	Char *       pat,
	int /*Bool*/ ic)
{ register Char * tlt = tlt_;
  register Char ch;

  for ( ; (ch=*tlt) != 0; ++tlt)
  { 						/* be stricter later */
    if (*pat == '*')
    { register Char pch = pat[1];
      register Char upch = toupper(pch);
      while ((ch = *tlt) != 0 && (toupper(ch) != upch || !ic && ch != pch))
        ++tlt;
      if (ch == 0 || tlt[1] == 0)
      { ++pat;
        break;
      }
    /*eprintf(null, "Down %s.%s\n", tlt+1, pat+2);*/
    { char * t = match_fn_re_ic(tlt+1, pat+2, ic);
    /*eprintf(null, "Up %s\n", t);*/
      if (*t != 0)
        continue;
      return t;
    }}

    if (ch != *pat)
    { if (! (ic && toupper(ch) == toupper(*pat)) && *pat != '?')
        break;
    }
/*  if (ch == 0 || (! ic ? ch != *pat : toupper(ch) != *pat)
		   && *pat != '?')
      break; */
    ++pat;
  }

  return *pat == 0 || *pat == '*' ? tlt : tlt_;
}

/*	FILE Directory routines		*/

Bool msd_empty_dir = false;

#if S_WIN32
 staticc HANDLE msd_curr = 0;
 
//#if _MSC_VER < 1900
#undef VS_CHAR8
#define VS_CHAR8 1
//#endif 

#if VS_CHAR8
 WIN32_FIND_DATA  msd_sct;
#else
 extern  wchar_t * char_to_wchar(char const *, int, wchar_t *);
 WIN32_FIND_DATAA msd_sct;
#endif

#elif S_MSDOS
 unsigned char dta[128];	    /* return file buffer */
 char * msd_curr;		    /* so it compiles */

#else
 struct DIRENT *msd_dp;		  /* directory entry pointer */
 staticc DIR *msd_curr = null;    /* the current directory being searched */
#endif


staticc Char msd_startdir[FILENAMESZ+2];

staticc Char * msd_relpath;
staticc Char   msd_path_[FILENAMESZ+4] = "?/";
#define msd_path ((char*)&msd_path_[2])	/* printed path to current file */

staticc Short g_pathend;          /* end of path part in msd_path (after /)*/

#if S_MSDOS == 0 || S_WIN32 != 0
staticc Char msd_pat[120];		/* pattern of last component */
#else
staticc Char msd_pat[10];		/* pattern of last component */
#endif

#if S_MSDOS
staticc Char  rbuf[FILENAMESZ+2];
#else
#define rbuf msd_path
#endif

        Vint    msd_iter;		/* FIRST then NEXT */
staticc Set16   msd_props;		/* which files to use */
        Bool    msd_ic = S_MSDOS;	/* Ignore case in file names */

        Set16   msd_attrs;		/* attributes of result: MSD_xxx */

        Cc msd_cc;
staticc Short msd_ix = 0;

struct stat msd_stat;


#define MAX_TREE 18

staticc int   msd_nlink[MAX_TREE+1];		/* stack of number of dirs */

#if NOPUSHPOP == 0
					/* preincrement with 0 unoccupied */

staticc Short msd_lenstk[MAX_TREE+1];		/* stack of filename lengths */
						/* includes trailing / or // */
#if   S_WIN32
 staticc HANDLE msd_stk[MAX_TREE+1];
 staticc int   msd_slink[MAX_TREE+1];		/* to resolve the symbol only */
 staticc int   msd_ochd[MAX_TREE+1];		/* previous msd_chd */
 staticc Short msd_slnest;			/* depth of symbolic links */
staticc Short msd_chd = -1;		/* last chdir MINUS one*/

#elif S_MSDOS == 0
 staticc DIR * msd_stk[MAX_TREE+1];		/* stack of opendir handles */
 staticc int   msd_slink[MAX_TREE+1];
 staticc int   msd_ochd[MAX_TREE+1];		/* previous msd_chd */
 staticc Short msd_slnest;			/* depth of symbolic links */
staticc Short msd_chd = -1;		/* last chdir MINUS one*/
#endif

#if S_MSDOS+S_VMS == 0
 staticc ino_t  msd_inos[MAX_TREE];

/*static Bool in_ino_stk(const ino_t ino)

 { Vint i;
   for (i = msd_ix+1; --i > 0; )
     if (msd_inos[i] == ino)
       return true;
   return false;
 }*/
 
#endif

#endif

staticc int/*bool*/ msd_nochdir = 0; /* dont use chdir */


#if READAHEAD == 0

# define ra_pop()

#else
# define RA_STACK 40000

typedef struct			/* must be congruent to struct dirent */
{ ino_t   	  ino;
  off_t   	  d_off_not_used;
  unsigned short  d_reclen_not_used;
  char    fname[72];
} Msd;

#define APPROX_SIZEOF_INO_T 8

staticc char msd_readahead[RA_STACK + APPROX_SIZEOF_INO_T + 64];

staticc int msd_ra_top = 0;
staticc int msd_ra_bot = 0;

static ino_t rr_head;



static void ra_pop()

{ msd_ra_top = msd_ra_bot;
  msd_ra_bot = ((ino_t*)&msd_readahead[msd_ra_bot])[0];
  rr_head = msd_ra_bot + sizeof(ino_t);
/*eprintf(null, "--RAPOP ------- bot %d top %d\n", msd_ra_bot, msd_ra_top);*/
}

#endif


#if NOPUSHPOP == 0


Cc msd_push()

{ if (msd_cc != OK)
    return msd_cc;
  ++msd_ix;
  if (msd_ix >= MAX_TREE)
  { --msd_ix;
#if S_MSDOS == 0
    closedir(msd_stk[msd_ix]);
#endif
  /*eprintf(null, "NESTED_MORE_THAN_%d:_FILES_OMITTED", MAX_TREE);*/
  }
  msd_lenstk[msd_ix] = strlen(msd_path);
/*eprintf(null, "LENSTK (%d) = %d\n", msd_ix, msd_lenstk[msd_ix]);*/
  		       /*g_pathend;*/
#if   S_WIN32
  msd_stk[msd_ix] = msd_curr;
#elif S_MSDOS == 0
  msd_stk[msd_ix] = msd_curr;
  msd_nlink[msd_ix] = msd_stat.st_nlink;
  msd_slink[msd_ix] = msd_attrs & MSD_SLINK;
  if (msd_slink[msd_ix])
  { msd_slnest += 1;
  /*eprintf(null, "+SLNEST %d\n", msd_slnest);*/
  }

/*if (msd_ix == 0)
    eprintf(null, "%s PTop %d\n", msd_path, msd_nlink[msd_ix]);*/
/*eprintf(null, "PUSH (%d) %s %x (%d) %d\n", msd_ix, msd_path,
  		msd_attrs, msd_slink[msd_ix], msd_stat.st_nlink);*/
/*eprintf(null, "PP (%d)\n", msd_slink[msd_ix]);*/
/*eprintf(null, "NL %d %d %d %d %d %d\n", 
           		msd_nlink[0], msd_nlink[1], msd_nlink[2], msd_nlink[3],
           		msd_nlink[4], msd_nlink[5]);*/
#endif

#if S_LINUX
  msd_inos[msd_ix] = msd_dp->d_ino;
#elif S_MSDOS+S_VMS == 0
  msd_inos[msd_ix] = msd_stat.st_ino;
#endif

#if READAHEAD
  if (rr_head > msd_ra_bot + sizeof(ino_t))
  { int down = rr_head - msd_ra_bot - sizeof(ino_t);
    memcpy(&msd_readahead[msd_ra_bot+sizeof(ino_t)], 
	   &msd_readahead[rr_head], msd_ra_top - rr_head + sizeof(ino_t));
    msd_ra_top -= down;
    rr_head = msd_ra_bot + sizeof(ino_t);
  /*eprintf(null, "Copied down %d rat %d\n", down, msd_ra_top);*/
  }

{ int ra_top = msd_ra_top;

  /*eprintf(null, "Reading ahead\n");**/

  while (true)
  { msd_dp = readdir(msd_curr);
    if (msd_dp == null)
      break;

  /*eprintf(null, "Rd %s\n", msd_dp->d_name);*/
  { dirent_t * eny = (dirent_t *) &msd_readahead[ra_top];
    eny->d_ino = msd_dp->d_ino;
    ra_top += strlen(strpcpy(&eny->d_name[0], msd_dp->d_name, sizeof(eny->d_name))
    							 )+1;
    ra_top += sizeof(ino_t) + sizeof(off_t) + sizeof(unsigned short)
						+ sizeof(ino_t) - 1;
    ra_top &= ~(sizeof(ino_t) - 1);
    if (ra_top >= RA_STACK-80)
      break;
  /*eprintf(null, "loop %d\n", ra_top);*/
  }}
  
/*eprintf(null, "+++++ msd_ra_bot %d ra_top %d\n", msd_ra_bot, ra_top);*/

  *(ino_t*)&msd_readahead[ra_top] = msd_ra_bot; 	/* push link to prev */
  msd_ra_bot = ra_top;
  msd_ra_top = ra_top + sizeof(ino_t);
  rr_head = msd_ra_top;

  if (msd_dp == NULL)				/* close it early */
  { closedir(msd_curr);
    /*msd_curr = NULL;*/
  }
}
/*eprintf(null, "three\n");*/
#endif
{ Set16 props = msd_props;
  Cc cc = msd_init(msd_path, msd_pat, (msd_props & ~MSD_SHOWDIR)| MSD_INTERNAL);
   
  msd_props |= (MSD_SHOWDIR & props) | MSD_REPEAT;
  return cc;
}}




Char * msd_pop()

{ msd_curr = NULL;
/*eprintf(null, "Pop (%d) %s\n", msd_ix, msd_path);*/
  
  msd_ix -= 1;

  if (in_range(msd_ix, -1, MAX_TREE-2))
  { ra_pop();
    g_pathend = msd_ix < 0 ? 0 : msd_lenstk[msd_ix];

  /*eprintf(null, "PPop (%d) %s(%d)\n", msd_ix, msd_path, msd_lenstk[msd_ix]);*/

#if S_MSDOS == 0 || S_WIN32
    msd_curr = msd_stk[msd_ix+1];
    msd_nlink[msd_ix+1] -= 1;

    if (msd_slink[msd_ix+1])
    { msd_slnest -= 1;
    /*eprintf(null, "-SLNEST %d\n", msd_slnest);*/
    }

  /*eprintf(null, "MSCD %x %d %d %s\n", msd_curr, msd_nlink[msd_ix+1], g_pathend, msd_path);*/
    if (! msd_nochdir && msd_chd > msd_ix)
    { char buf[FILENAMESZ*2+3];
      char * tgt = "..";
      msd_chd -= 1;
      if (msd_ix < 0 || (S_WIN32 == 0 && msd_slink[msd_ix+1]))
		  { Char * tt = &msd_path[g_pathend];
      	char sch = *tt;
        msd_chd = msd_ix;
      	strpcpy(&buf[0], msd_startdir, sizeof(buf));
        *tt = 0;
      	strcat(&buf[0], msd_path);
        *tt = sch;
      	tgt = buf;
      } 

    { Cc cc = chdir(tgt);
      if (cc != OK)
        printf2( "Fatal chdir error ", tgt);
      msd_relpath = msd_chd < 0 ? msd_path : &msd_path[msd_lenstk[msd_chd]];
    {/*char cwdb[300];
      eprintf(null, "POPDIR %d %d %s %s RP %s\n", msd_ix+1, msd_chd,
      			tgt, getcwd(cwdb, 299), msd_relpath);*/
    }}}
#else
    if (/*msd_ix > 0 && */ *tt != 0 && *tt != '*')
    { Char enynm[41];
      Char * res;
      Set16 save_props = msd_props;
      register Short i;
      for (i = sizeof(enynm); --i >= 0; )
      { enynm[i] = tt[i];
        if (enynm[i] == '/' || enynm[i] == '\\')
		  	  enynm[i] = 0;
      }
  
      *tt = 0;
    
      msd_init(msd_path, msd_pat, (save_props & ~MSD_SHOWDIR)|MSD_INTERNAL);
    /*eprintf(null, "TGT %s\n", &enynm[0]);*/
      
      while (true)
      { int trash;
        register Char * t = &enynm[0];
		  	Char * s = &dta[0x1e];
      /*eprintf(null, "DD %s : %s DD\n", t, s);*/
        for (; (i = *s) != 0 && toupper(*t) == toupper(i); ++s)
		  	  ++t;
        if (*t == 0)
  			  break;
        res = msd_nfile(&trash);   
        if (res == null)
        {
#if NOPUSHPOP == 0
          if (msd_ix >= 0)
            (void)msd_pop();
#endif    
          res = "FileLost";
          break;
        }
      }
    /*
      if (res == null || ((msd_attrs & MSD_DIRY) == 0)
        eprintf(stderr,"POP FAILED\n");
    */
      msd_props = save_props;
    }
#endif
  }

  msd_empty_dir = false;
  msd_attrs |= MSD_POST;
  msd_attrs &= ~MSD_DIRY;

/*eprintf(null, "pOP (%d) %s\n", msd_ix, msd_path);*/
 
  if (msd_ix < 0)
  { msd_ix = 0;
    return NULL;
  }   
  return msd_path;
}

#endif

/* CODE BEGINS HERE */

		/* do a wild card directory search (for file name completion) */

Cc msd_init(Char const *  diry,	/* must not be "" */
						Char const *  pat,	/* pattern to match, null => take it from diry */
						int     props)	/* msdos props + top bits set => repeat first file */
{ Char ch;
  short pe;
  short pe_last_sl = -1;
												/* msd_path often == diry */
  for ( pe = -1; ++pe < FILENAMESZ && (msd_path[pe] = (ch = diry[pe])) != 0; )
  { if (ch == '\\' || ch == '/')
    { msd_path[pe] = '/';
      pe_last_sl = pe;
    }
  } 
  				/* extract pattern, and cut back */
  if (pat == null)
  { pe = pe_last_sl + 1;
    pat = &diry[pe];
  }

  if (pe > 0 && msd_path[pe-1] != '/')
    msd_path[pe++] = '/';

  msd_path[pe] = 0;
  g_pathend = pe;

  for (pe = -1; (ch = pat[++pe]) != 0 && pe < sizeof(msd_pat)-2; )
#if S_MSDOS && S_WIN32 == 0
    msd_pat[pe] = toupper(ch);
#else
    msd_pat[pe] = ch;
#endif
  msd_pat[pe] = 0;

  loglog2("PATH %s PAT  %s", msd_path, msd_pat);
/*eprintf(null, "PATH %s\nPAT  %s\n", msd_path, msd_pat);*/

  if ((props & MSD_INTERNAL) == 0)
  {
    if (props & MSD_CHGD)			/* tie them together for now */
      props &= ~MSD_SIMPLE;
  
#if READAHEAD
    *(ino_t*)&msd_readahead[0] = 0;
    msd_ra_bot = 0;
    msd_ra_top = msd_ra_bot + sizeof(ino_t);

    rr_head = msd_ra_top;
#endif
    msd_ix = 0;
    msd_relpath = msd_path;
    msd_empty_dir = false;
    msd_ic = msd_props & MSD_IC;
    msd_nochdir = (props & (MSD_NOCHD+MSD_STAY));

#if NOPUSHPOP == 0

    msd_lenstk[0] = strlen(msd_path);
#if S_MSDOS == 0 || S_WIN32
    msd_chd = -1;
    msd_ochd[0] = -1;
    msd_nlink[0] = NOLK;
    msd_slnest = 0;
    getcwd(&msd_startdir[0], sizeof(msd_startdir)-2);
    strcat(&msd_startdir[0], "/");
  /*eprintf(null, "Startdir %s\n", msd_startdir);*/
    msd_slink[0] = 0;		/* this can be wrong ! */
#endif
#endif
  }

  msd_props = props;
  msd_attrs = 0;
  msd_iter = DOS_FFILE;
#if S_MSDOS && 0
  rbuf[0] = 0;
#endif
  msd_cc = -100;

  pe = g_pathend;

#if S_MSDOS & S_WIN32 == 0
  strpcpy(&msd_path[pe], "*.*", 4);

  ms_intdosx((Char *)dta, 0x1a00);		/*  set dta  */

  rg.x.cx = msd_props & 0x1f;			/*  allow all files */
  msd_cc = ms_intdosx(msd_path, DOS_FFILE);
  
/*eprintf(null, "DFF %d\n", msd_cc);*/
  return msd_cc;
#else
{ register char * dir;
  
#if   S_WIN32
  dir = pe == 0 ? "./*.*" : msd_relpath;
  strpcpy(&msd_path[pe], "*.*", 4);
/*eprintf(null, "FF %s\n", dir);*/

#if VS_CHAR8
  msd_curr = FindFirstFile(dir, &msd_sct);
#else
	wchar_t buf[512];
  wchar_t * nm = char_to_wchar(dir, 512, buf);
  msd_curr = FindFirstFileA(dir, &msd_sct);
#endif

/*eprintf(null, "HANDLE %x %s\n", msd_curr, (Char*)msd_sct.cFileName);*/
  msd_path[pe-1] = '/';
  msd_path[pe] = 0;
  if (msd_curr == INVALID_HANDLE_VALUE)
    return EDENIED;

  msd_cc = OK;
#else
  dir = pe == 0 ? "." : msd_relpath;
  msd_curr = opendir(dir);
/*eprintf(null, "%d %d OPen %s\n", msd_ix, msd_chd, dir);*/
  if (msd_curr == NULL)
  {/*char cwdb[160];*/
  /*eprintf(null, "OPen Empty %d %s, CWD %s\n", errno, dir, getcwd(cwdb, 159));*/
    msd_empty_dir = true;
    return EDENIED;
  }
#endif


#if NOPUSHPOP == 0
/*eprintf(null, "PTRY %d %d %d\n", msd_ix, msd_chd, msd_nlink[msd_ix]);*/
  if (!msd_nochdir && (!(msd_props & MSD_SIMPLE) 
#if S_MSDOS == 0
		      || msd_slnest == 0 && msd_slink[msd_ix] == 0
#endif
     )		      )
  { int nchd;
    int fd = -1;

  /*eprintf(null, "ptry\n");*/

    if      ((msd_props & MSD_CHGD) || msd_ix == 0)
    {
#if S_WIN32
      char cwdb[300];
      fd = chdir(dir);
    /*eprintf(null, "chdir %d %s CWD %s\n", fd, dir, getcwd(cwdb, 299));*/
#elif S_LINUX
      fd = dirfd(msd_curr);
#else
      fd = msd_curr->dd_fd;
#endif

      nchd = msd_ix;
    }
    else if (S_WIN32 == 0 && msd_nlink[msd_ix-1] > 2)
    {
#if S_LINUX
      fd = dirfd(msd_stk[msd_ix]);
#elif S_WIN32 == 0
      fd = msd_stk[msd_ix]->dd_fd;
#endif
      nchd = msd_ix-1;
    }

    if (fd >= 0)
    {/* Int orp = msd_relpath; */
#if S_WIN32 == 0
      Cc cc = fchdir(fd);
      if (cc != OK)
        printf2("CHDIR failure OK", "");
      else
#endif
      { msd_ochd[msd_ix] = msd_chd;
        msd_chd = nchd;
				msd_relpath = &msd_path[msd_lenstk[nchd]];
      }
    {/*char cn[300];
      eprintf(null,"PUSHDIR %d %d %d, %s ORP %s NRP %s\n", msd_ix, nchd,
      			msd_slink[msd_ix], getcwd(cn,299), orp, msd_relpath);*/
    }}
  }
#endif
}
#endif

  return OK;
}



staticc Cc getnext()
{
#if S_WIN32
  strpcpy(&msd_path[g_pathend], "*.*", 4);
/*eprintf(null, "FNF %x, %s\n", msd_curr, msd_path);*/

#if VS_CHAR8
  if (msd_curr == 0 || ! FindNextFile(msd_curr, &msd_sct))
#else
  if (msd_curr == 0 || ! FindNextFileA(msd_curr, &msd_sct))
#endif
  { msd_path[g_pathend] = 0;
    return ~OK;
  }

  return OK;
#elif S_MSDOS
  
  ms_intdosx((Char *)dta, 0x1a00);		/*  set dta  */

  strpcpy(&msd_path[g_pathend], "*.*", 4);

/*printf("DNF\n");*/

  rg.x.cx = msd_props & 0x1f;			/*  allow all files */
  return ms_intdosx(msd_path, DOS_NFILE);
#else
  msd_path[g_pathend] = 0;
#if READAHEAD
  if (rr_head < msd_ra_top)
  { msd_dp = (dirent_t*)&msd_readahead[rr_head];
 
  /*eprintf(null, "RDDIR %d ra_top %d\n", rr_head, msd_ra_top);*/
  /*
    if (res->d_name[0] == 0)
      eprintf(null, "RR_HEAD %d ra_top %d\n", rr_head, msd_ra_top);
  */
  /*eprintf(null, "name %s\n", res->d_name);*/
 
    rr_head = 
     (rr_head + strlen(msd_dp->d_name) +
                sizeof(ino_t) + sizeof(off_t) + sizeof(unsigned short) +
    		sizeof(ino_t)) & ~(sizeof(ino_t) - 1);
    return OK;
  }
  else
#endif
  { if (msd_curr == NULL)
    { /*eprintf(null, "ImpErr7\n");*/
      return ~OK;
    }

    msd_dp = readdir(msd_curr);		/* call for the next file */
  /*eprintf(null, "RD %x -> %x %s\n", msd_curr, msd_dp, msd_dp == null ? "()" : msd_dp->d_name);*/
    if (msd_dp != NULL)
      return OK;
    
    return ~OK;
  }
#endif
}



static Bool extract_fn(int * fnoffs)

{
#if S_LINUX && DT_DIR == 4 && DT_LNK == 10
#define DT_TABLE
  static const Set16 dt_table[16] = 
     { 0, 0, 0, 0, MSD_DIRY, 0, 0, 0, 0, 0, MSD_DIRY+MSD_SLINK, };
#endif

#if S_WIN32
  unsigned short fat_date;
  unsigned short fat_time;
  #define s (((Char*)msd_sct.cFileName))
#elif S_MSDOS
  #define s  (&dta[0x1e])
#elif S_XENIX
  #define s (&((Char*)msd_dp)[10])
#else
  #define s (&msd_dp->d_name[0])
#endif

/*eprintf(null, "MFRI %d %s:%s\n", g_pathend, s, msd_pat);*/
  msd_attrs = 0;	/* do not allow push to . or .. */
  *fnoffs = g_pathend;
{ int msd_a;
  int i;
  char * tl = &msd_path[g_pathend];
  char * s_ = s;
  if (s_[0] == '.' && (s_[1] == '.' || s_[1] == 0))
    return false;
  for ( i = FILENAMESZ - 1 - g_pathend; --i >= 0 && (*tl++ = *s_++) != 0; )
    ;

/*loglog3("CG %d .%s. %s", g_pathend, msd_path, s);*/

  if (i < 0)
    return false;

#if S_WIN32
  msd_stat.st_size = msd_sct.nFileSizeLow;
  msd_a = msd_sct.dwFileAttributes;

/*eprintf(null, "doFatDate\n");*/

  FileTimeToDosDateTime(&msd_sct.ftLastWriteTime, &fat_date, &fat_time);
/*eprintf(null,"FatDate %lx FatTime %lx\n", fat_date, fat_time);*/
  msd_stat.st_mtime = ((Int)fat_date << 16) + (fat_time & 0xffff);
#elif S_VMS
  
#elif S_MSDOS
  msd_a =  dta[0x15] & 0x3f;

  msd_stat.st_size  = *(Int*)&dta[0x1a];
  msd_stat.st_mtime = *(Int*)&dta[0x16];
/*
   printf( "ma %x %x %x %x œ%x %x %x %x.%x %x\n"
          "   %x %x %x %x `%x %x %x %x.%x %x %x %s\n", 
   dta[0], dta[1], dta[2], dta[3], dta[4], dta[5], dta[6], dta[7], dta[8], dta[9],
   dta[10],dta[11],dta[12],dta[13],dta[14],dta[15],dta[16],dta[17],dta[18],dta[19],dta[20],
		   msd_path);
  printf( "ma fa %x td %x %x %x %x size %x %x %x %x\n",
	 dta[21],dta[22],dta[23],dta[24],dta[25],dta[26],dta[27],dta[28],dta[29]);
*/
#else
/*{ char cwdb[300];
  eprintf(null, "CWD now %s\n", getcwd(cwdb, 299));
}*/
#if S_LINUX 
#ifdef DT_TABLE
  msd_a = dt_table[msd_dp->d_type && 0xf];
#else
  msd_a = msd_dp->d_type == DT_DIR ? MSD_DIRY :
          msd_dp->d_type == DT_LNK ? MSD_DIRY + MSD_SLINK : 0;
#endif

#if NOPUSHPOP == 0
  if (! msd_nochdir && msd_slnest == 0 && msd_chd < msd_ix - 1 and
      (msd_a & MSD_DIRY) && msd_nlink[msd_ix] <= NOLK+1)/*stat the parent*/
  { char sch = msd_path[msd_lenstk[msd_ix]];
    msd_path[msd_lenstk[msd_ix]] = 0;
    if (stat(msd_relpath, &msd_stat)!= OK)
    {// char cwdb[300];
    /*eprintf(null, "%d staterr0 %s / %s\n", errno, getcwd(cwdb, 299), msd_relpath);*/
    }
    else
    { msd_nlink[msd_ix] = msd_stat.st_nlink;
    /*eprintf(null, "statP %s %d %d %d\n", msd_relpath, msd_ix, msd_chd, msd_stat.st_nlink);*/
    }
    msd_path[msd_lenstk[msd_ix]] = sch;
  }
#endif
#endif
#endif

  if ((msd_props & MSD_SIMPLE)
#if S_LINUX
      && !(msd_a & MSD_SLINK)
#elif S_MSDOS == 0
      && in_range(msd_nlink[msd_ix], 0, 2)
#endif
     )
  {
  /*eprintf(null, "//KNOWNFILE %s %x\n", s, msd_a);*/
#if S_LINUX+S_VMS == 0
    /*msd_a = 0;*/
    msd_stat.st_ino = 0;
#endif
    msd_stat.st_nlink = NOLK;
  }
#if S_WIN32 == 0
  else
  { if (stat(msd_relpath, &msd_stat)!= OK)
    { char cwdb[160];
      printf2("staterr ", msd_relpath);
      printf2("CWD now ", getcwd(cwdb, sizeof(cwdb)-1));
      msd_a = MSD_NOPERM;
    }
    else
      msd_a =
#if S_LINUX
	        (S_ISDIR(msd_stat.st_mode)	       ? MSD_DIRY   :
	         S_ISLNK(msd_stat.st_mode)	       ? MSD_DIRY+MSD_SLINK : 0)
#else
	        ((msd_stat.st_mode & S_IFMT) == S_IFDIR ? MSD_DIRY   : 0)
#endif
	      | ((msd_stat.st_mode & S_IWRITE) == 0     ? MSD_ROFILE : 0) | 
	       /* crude!*/
#if S_LINUX
	        (msd_a & MSD_SLINK) |
#else
	        (msd_stat.st_mode & S_IFLNK ? MSD_SLINK : 0) |
#endif
	        ( s[0] == '.' ? MSD_HIDFILE : 0);
	        
  /*eprintf(null, "LS %s %d %x %d INO %ld %d ATTR %d\n",
            s, msd_ix, msd_stat.st_mode, 
               msd_stat.st_mode, 
               msd_stat.st_ino, msd_stat.st_nlink,
               msd_a);*/
  }
#endif

  msd_attrs = msd_a;
  if (msd_attrs & MSD_DIRY)
  { 
    tl[-1] = '/';
    tl[0] = 0;
#if S_MSDOS == 0 | S_WIN32
    if (msd_attrs & MSD_SLINK)
    { tl[0] = '/';
      tl[1] = 0;
    }
#endif
  }

  loglog2("Match %s %s", msd_pat, s);

  if      (msd_pat[0] == 0 ||
	  !*match_fn_re_ic(s,msd_pat, S_MSDOS ? true : msd_ic)
          )
    msd_attrs |= MSD_MATCHED;
  else if (! ((msd_attrs & MSD_DIRY) && (msd_attrs & MSD_SHOWDIR)))
    return false;

#if NOPUSHPOP == 0
#if S_MSDOS+S_VMS == 0
#define myino msd_a
    myino = msd_dp->d_ino;
    for (i = msd_ix+1; --i > 0; )
      if (msd_inos[i] == myino)
        return false;
#endif
#endif
  }
    
/*eprintf(null, "M %x.%s %s\n", match[0], match, &dta[0x1e]);*/
/*eprintf(null, msd_attrs & MSD_DIRY ? "YY %s %x %x\n"
			      : "DD %s %x %x\n", msd_path, msd_attrs, 0**msd_stat.st_mode**);*/
#if S_MSDOS
  strpcpy(&rbuf[0], &msd_path[0], sizeof(rbuf));
  
#if S_WIN32 == 0
{ register int ix;
  for (ix = -1; rbuf[++ix] != 0; )
    if (in_range(rbuf[ix], 'A', 'Z'))
       rbuf[ix] += 'a' - 'A';
}
#endif
#endif
  return true;
}



Char * msd_nfile(int * fnoffs)

{ 
  while (true)
  {
#if NOPUSHPOP == 0
    if ((msd_attrs & MSD_DIRY) && ! (msd_props & MSD_STAY))
    { msd_cc = OK; 
      msd_push();
    }
#endif
#if S_MSDOS
    if (msd_iter != DOS_FFILE)
#endif
      msd_cc = getnext();		/* call for the next file */
    msd_iter = DOS_NFILE;
    
  /*eprintf(null, "dff %d\n", msd_cc);*/
    
    if (msd_cc != OK)
    {/*eprintf(null, "No next\n");*/
#if S_MSDOS == 0
      if (msd_curr != 0)
        closedir(msd_curr);			/* close it early */
#endif
    /*eprintf(null, "CLose %x\n", msd_curr);*/
    /*msd_curr = NULL;*/
#if NOPUSHPOP
      return null;
#else
      if ((msd_props & MSD_STAY) || msd_pop() == null)
      	return null;
      msd_attrs = 0;
#endif
    }
    else if (extract_fn(fnoffs))
    {
      return rbuf;
    }

    msd_cc = -100;
  }
}

#if 0

int msd_getprops(Char * fn)

#if S_MSDOS && S_WIN32 == 0

{ msd_cc = ms_intdosx(fn, 0x4300);
  return rg.h.cl & 0x3f;
}

#else

{ struct stat mstat;
  return ( stat(fn, &mstat) != OK             ? MSD_NOPERM :
	  (mstat.st_mode & S_IFMT) == S_IFDIR ? MSD_DIRY   :
	  (mstat.st_mode & S_IWRITE) == 0     ? MSD_ROFILE : 0) | 
	       /* crude!*/
	  ( s[0] == '.' ? MSD_HIDFILE : 0);
}

#endif
#endif
