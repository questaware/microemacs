/*	DOLOCK.C:	Machine specific code for File Locking
#include	"elang.h"
			for MicroEMACS
			(C)opyright 1987 by Daniel M Lawrence
*/

#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"


#if	0
/*	dolock: MDBS specific Unix 4.2BSD file locking mechinism
		this is not to be distributed generally 	*/

#include	<mdbs.h>
#include	<mdbsio.h>
#include	<sys/types.h>
#include	<sys/stat.h>

/* included by port.h: mdbs.h, mdbsio.h, sys/types.h, sys/stat.h */


#ifndef bsdunix
char *dolock(){return NULL;}
char *undolock(){return NULL;}
#else

#include <pwd.h>
#include <errno.h>

extern int errno;

#define LOCKDIR ".xlk"

#define LOCKMSG TEXT36
/*		"LOCK ERROR -- " */
#define LOCKMSZ sizeof(LOCKMSG)
#define LOCKERR(s) { strcat(lmsg,s); oldumask = umask(oldumask); return lmsg; }

/**********************
 *
 * dolock -- lock the file fname
 *
 * if successful, returns NULL 
 * if file locked, returns username of person locking the file
 * if other error, returns "LOCK ERROR: explanation"
 *
 * Jon Reid, 2/19/86
 *
 *********************/

BOOL parent = FALSE;
BOOL tellall = FALSE;

extern char * rindex();

char *gtname(filespec)		/* get name component of unix-style filespec */
	char *filespec;
{
  char * rname = rindex(filespec,'/');

  return rname != NULL ? rname : filespec;
}

char *getpath(filespec)
	char *filespec;
{
	static char rbuff[LFILEN];
	char *rname = rindex(strcpy(rbuff,filespec), '/');

	if (rname == NULL)
	  return NULL;
	else
	{ *(++rname) = '\0';
	  return rbuff;
	}

}

char *dolock(fname)
	char *fname;
{
	       char lockname[LFILEN];
	static char username[12];
	struct stat statblk;
	int oldumask = umask(0);     /* maximum access allowed to lock files */
        char * t = lockname[0];
        char * sep = "/"
        if (*fname != '/')
        { strcpy(t,"./");
          t += 2;
          ++sep;
        }
        strcpy(t,getpath(fname));
        strcat(t,sep);
        strcat(t,LOCKDIR);

	if (tellall) printf(TEXT37,lockname);
/*			      "checking for existence of %s\n" */

	if (stat(lockname,&statblk))
	{  if (tellall) printf(TEXT38,lockname);
/*				     "making directory %s\n" */
	   mkdir(lockname,0777); 
	}

	strcat(strcat(lockname,"/"),gtname(fname));

	if (tellall) printf(TEXT37,lockname);
/*			      "checking for existence of %s\n" */

	if (stat(lockname,&statblk))
	{
makelock:  if (tellall) printf(TEXT39,lockname);
/*				    "creating %s\n" */

	{  FILE * lf = fopen(lockname,FOP_TW);
	   if (lf == NULL)
		  LOCKERR(TEXT40)
/*			  "could not create lock file" */
	   else
	   {  Int pid = parent ? getppid() : getpid();

	      if (tellall) printf(TEXT41, pid); 
/*				 "pid is %ld\n" */

	      fprintf(lf,"%ld",pid); /* write pid to lock file */

	      fclose(lf);
	      oldumask = umask(oldumask);
	      return NULL;
	   }
	}}
	else
	{  if (tellall) printf(TEXT42,lockname);
/*				    "reading lock file %s\n" */
	   lf = fopen(lockname,FOP_TR);
	   if (lf == NULL)
		  LOCKERR(TEXT43)
/*			  "could not read lock file" */
	   else
	   {  fscanf(lf,"%ld",&pid); /* contains current pid */
	      fclose(lf);
	      if (tellall) printf(TEXT44,lockname, pid);
/*				"pid in %s is %ld\n" */
	      if (tellall) printf(TEXT45, pid);
/*				"signaling process %ld\n" */
	      if (kill(pid,0))
		switch (errno)
		{ when ESRCH:	/* process not found */
				goto makelock;
		  when EPERM:	/* process exists, not yours */
				if (tellall) puts(TEXT46);
/*					      "process exists" */
		  otherwise     LOCKERR(TEXT47)
/*						"kill was bad" */
		}
	      else
		if (tellall) puts(TEXT48);
/*					   "kill was good; process exists" */
	    }
	  { struct passwd *pblk = getpwuid(statblk.st_uid)
	    if (pblk == NULL)
	      sprintf(username, "uid %d", atoi(statblk.st_uid));
	    else
	      strcpy(username,pblk->pw_name);

	    oldumask = umask(oldumask);
	    return username;
	  }}
}

/**********************
 *
 * undolock -- unlock the file fname
 *
 * if successful, returns NULL 
 * if other error, returns "LOCK ERROR: explanation"
 *
 * Jon Reid, 2/19/86
 *
 *********************/

char *undolock(fname)
	char *fname;
{
	static char lockname[LFILEN] = LOCKDIR;
	static char lmsg[40] = LOCKMSG;

	sprintf(lockname,*fname != '/' ? "./%s%s" : "%s/%s",
		   getpath(fname), LOCKDIR);

	sprintf(lockname,"%s/%s",lockname,gtname(fname));

	if (tellall) printf(TEXT49,lockname);
/*			      "attempting to unlink %s\n" */

	if (!unlink(lockname))
	  return NULL;
	strcat(lmsg,TEXT50); 
/*			    "could not remove lock file" */
	return lmsg; 
}

#endif

/******************
 * end dolock module
 *******************/

#else
void dolhello()
{ 
}
#endif

#if 0
  OS     SN   ltd500223    AK  krmjiioh
 S5 DS   SN   ltd013391    AK  ckdjvbap
 OSISupp SN   ltd015964    AK  uvwljill
 S5ISupp SN   ltd002768    AK  cmbrzqae
  Graphics SN ltd014204    AK  kuenxcrz
#endif
