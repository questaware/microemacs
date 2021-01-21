/*	LOCK:	File locking command routines for MicroEMACS
		written by Daniel Lawrence
 */
#include <stdio.h>
#include "estruct.h"
#include "edef.h"
#include "etype.h"
#include "elang.h"

#if	FILOCK
#if	S_BSD
#include <sys/errno.h>

extern int sys_nerr;		/* number of system error messages defined */
extern char *sys_errlist[];	/* list of message texts */
extern int errno;		/* current error */

extern char *dolock();
extern char *undolock();


char *lname[NLOCKS];	/* names of all locked files */
int numlocks;		/* # of current locks active */


			/* check a file for locking and add it to the list */
int lockchk(fname)
	char *fname;	/* file to check for a lock */
{
	register int i;
#define cc i
			/* check to see if that file is already locked here */
	for (i=0; i < numlocks; ++i)
	  if (strcmp(fname, lname[i]) == 0)
	    return TRUE;

			/* if we have a full locking table, bitch and leave */
	if (numlocks == NLOCKS)
	{ mlwrite(TEXT173);
/*                      "LOCK ERROR: Lock table full" */
	  return ABORT;
	}
						/* next, try to lock it */
	cc = lock(fname);
	if (cc == ABORT)	/* file is locked, no override */
	  return ABORT;
	if (cc == FALSE)	/* locked, overriden, dont add to table */
	  return TRUE;
				/* we have now locked it, add it to our table */
	lname[++numlocks - 1] = (char *)strdup(fname);
	if (lname[numlocks - 1] == NULL)
	{ undolock(fname);		/* free the lock */
	  --numlocks;
	  return ABORT;
	}
				/* everthing is cool, add it to the table */
	strcpy(lname[numlocks-1], fname);
	return TRUE;
}


			/* release all the file locks so others may edit */
int lockrel()

{	register int i;
	register int cc = TRUE;
	register int s;			/* status of one unlock */

	for (i=0; i < numlocks; ++i)
	{ s = unlock(lname[i]);
	  if (s != TRUE)
	    cc = s;
	  free(lname[i]);
	}
	numlocks = 0;
	return cc;
}

		/*	    Check and lock a file from access by others
		   returns TRUE =>  files was not locked and now is
			   FALSE => file was locked and overridden
			   ABORT => file was locked, abort command
		*/
int lock(fname)
	char *fname;	/* file name to lock */
{
		char msg[NSTRING];		/* message string */
	register char *locker = dolock(fname);	/* lock error message */

					/* attempt to lock the file */
	if (locker == NULL)	/* we win */
	  return TRUE;
						/* file failed...abort */
	if (strcmp_right(locker, TEXT175) == 0)
	{											/* "LOCK" */
	  lckerror(locker);
	  return ABORT;
	}
					/* someone else has it....override? */
	strcat(strcat(strcpy(msg, TEXT176), 
/*                  "File in use by " */
				  locker),
				  TEXT177);
/*                  ", overide?" */
	return mlyesno(msg) == TRUE ? FALSE : ABORT;	/* ask them */
}

			/* Unlock a file this only warns the user if it fails */
int unlock(fname)
	char *fname;	/* file to unlock */
{
	register char *locker = undolock(fname);   /* undolock return string */
	if (locker == NULL)
	  return TRUE;
					/* report the error and come back */
	lckerror(locker);
	return FALSE;
}


int lckerror(errstr)	/* report a lock error */
	char *errstr;		/* lock error string to print out */
{
	char obuf[NSTRING];	/* output buffer for error message */

	strcat(strcpy(obuf, errstr), " - ");
	if (errno < sys_nerr)
	  strcat(obuf, sys_errlist[errno]);
	else
	  strcat(obuf, TEXT178);
/*                           "[can not get system error message]" */
	mlwrite(obuf);
}
#endif
#else
#endif
