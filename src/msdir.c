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
#include        "build.h"
#include        "logmsg.h"

#if   S_WIN32
# include       <windows.h>
#if S_CYGWIN
#include 				<unistd.h>
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

#if S_WIN32 == 0

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

#endif


static
Bool match_fn_re_ic(Char * tlt,
										Char * pat
#if S_WIN32 == 0
									 ,int /*Bool*/ fn_ic
#else
#define fn_ic 1
#endif
									 )
{	Char ch,pch;
	--pat;
	--tlt;
  while (1)
  { pch = *++pat;
	  ch = *++tlt;
		if (pch == 0 || ch == 0)
			break;
#if S_WIN32 == 0
		if (ch == pch)
    	continue;
#endif
    if (toupper(ch) == toupper(pch) && fn_ic)
      continue;
//  if (pch == '?')											/* not enabled */
//   	continue;
    
    if (pch != '*')
    	break;
		if (pat[1] == 0)
			return 1;

  { Bool rc = match_fn_re_ic(tlt, pat+1 /*,*/ msd_ignore(fn_ic));
   	if (rc)
   		return rc;
#if S_WIN32 == 0
    if (toupper(ch) == toupper(pat[1]) && fn_ic)
#else
   	if (ch == pat[1])
#endif
    	++pat;
    else
    	--pat;
  }}

  return (ch | pch) == 0;						/* ?* ?? */
}

#undef fn_ic

/*	FILE Directory routines		*/

// Bool msd_empty_dir = false;

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


staticc Char   msd_path_[NFILEN+4] = "?/";
#define msd_path ((char*)&msd_path_[2])	/* printed path to current file */
#define msd_relpath msd_path

staticc Short g_pathend;          /* end of path part in msd_path (after /)*/

staticc Char msd_pat[100];		/* pattern of last component */


#if S_MSDOS && S_WIN32 == 0
staticc Char  rbuf[FILENAMESZ+2];
#else
#define rbuf msd_path
#endif

staticc Vint    msd_iter;		/* FIRST then NEXT */
staticc Set16   msd_props;		/* which files to use */
static  Set16   msd_attrs;		/* attributes of result: MSD_xxx */
#if S_MSDOS			
#define msd_ic 1
#else
        Bool    msd_ic;				/* Ignore case in file names */
#endif

struct stat msd_stat;


#define MAX_TREE 18

#if S_MSDOS == 0
staticc int   msd_nlink[MAX_TREE+1];		/* stack of number of dirs */
#endif

#define msd_ix 0

/* CODE BEGINS HERE */

		/* do a wild card directory search (for file name completion) */

Cc msd_init(Char const *  diry,	/* must not be "" */
						int     props)	/* msdos props + top bits set => repeat first file */
{ msd_props = props;
  msd_attrs = 0;
  msd_iter = DOS_FFILE;

{ Char ch;
  short pe;
  short pe_last_sl = -1;

// msd_relpath = msd_path;
												/* msd_path often == diry */
  for ( pe = -1; ++pe < FILENAMESZ && (msd_path[pe] = (ch = diry[pe])) != 0; )
  { if (ch == '\\' || ch == '/')
    { msd_path[pe] = '/';
      pe_last_sl = pe;
    }
  } 
							  				/* extract pattern, and cut back */
	msd_pat[0] = 0;
  if (props & MSD_USEPATH)
  { pe = pe_last_sl + 1;
    strpcpy(msd_pat, &diry[pe], sizeof(msd_pat)-1);
  }

  if (pe > 0 && msd_path[pe-1] != '/')
    msd_path[pe++] = '/';

  g_pathend = pe;

  loglog2("PATH %s PAT  %s", msd_path, msd_pat);

{	char * dir;
  
#if   S_WIN32
	const char * const stars = "./*.*";
  dir = pe == 0 ? stars : msd_relpath;
  strpcpy(&msd_path[pe], stars+1, 4);
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
//msd_path[pe] = 0;
  if (msd_curr == INVALID_HANDLE_VALUE)
    return EDENIED;

#else
  dir = pe == 0 ? "." : msd_relpath;
  msd_curr = opendir(dir);
/*eprintf(null, "%d %d OPen %s\n", msd_ix, msd_chd, dir);*/
  if (msd_curr == NULL)
  {/*char cwdb[160];*/
  /*eprintf(null, "OPen Empty %d %s, CWD %s\n", errno, dir, getcwd(cwdb, 159));*/
//  msd_empty_dir = true;
    return EDENIED;
  }
#endif

  return OK;
}}}



staticc Cc getnext()
{
#if S_WIN32
  strcpy(&msd_path[g_pathend], "*.*");
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

  strcpy(&msd_path[g_pathend], "*.*");

/*printf("DNF\n");*/

  rg.x.cx = msd_props & 0x1f;			/*  allow all files */
  return ms_intdosx(msd_path, DOS_NFILE);
#else
  msd_path[g_pathend] = 0;

  if (msd_curr == NULL)
  { /*eprintf(null, "ImpErr7\n");*/
    return ~OK;
  }

  msd_dp = readdir(msd_curr);		/* call for the next file */
  /*eprintf(null, "RD %x -> %x %s\n", msd_curr, msd_dp, msd_dp == null ? "()" : msd_dp->d_name);*/
  if (msd_dp != NULL)
    return OK;
    
  return ~OK;
#endif
}



static Bool extract_fn()

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
//*fnoffs = g_pathend;
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
    //msd_stat.st_ino = 0;
#endif
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
  if (msd_a & MSD_DIRY)
  { 
    tl[-1] = '/';
    tl[0] = 0;
  }

  loglog2("Match %s %s", msd_pat, s);

  if      (msd_pat[0] == 0 ||
				   match_fn_re_ic(s,msd_pat /*,*/msd_ignore(msd_ic))
          )
    ; // msd_attrs |= MSD_MATCHED;
  else if (! ((msd_a & MSD_DIRY) && (msd_a & MSD_SHOWDIR)))
    return false;
}
    
/*eprintf(null, "M %x.%s %s\n", match[0], match, &dta[0x1e]);*/
/*eprintf(null, msd_attrs & MSD_DIRY ? "YY %s %x %x\n"
			      : "DD %s %x %x\n", msd_path, msd_attrs, 0**msd_stat.st_mode**);*/
#if S_MSDOS && S_WIN32 == 0
  strpcpy(&rbuf[0], &msd_path[0], sizeof(rbuf));
	mkul(0, rbuf);													/* make string lower case */
#endif
  return true;
}



Char * msd_nfile()

{ int fnoffs;

  while (true)
  { 
		Cc msd_cc = OK;
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
      return null;
    }
    else if (extract_fn())
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

#define STANDALONE 0
#if STANDALONE

#include        <stdio.h>*/

int main(int argc, char * argv[])

{ char * pat = NULL;
  char * file = NULL;
  int carg;
  for (carg = argc; --carg > 0; )
	{ char * filev = argv[argc-carg];
		if (pat == NULL)
			pat = filev;
		else
			file = filev;
	}
	
{	Bool rc = match_fn_re_ic(file, pat /*,*/ msd_ignore(0));
	printf("Res %d\n", rc);
}}

#endif
