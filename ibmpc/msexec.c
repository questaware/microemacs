#include <stdio.h>
#include <dos.h>
#include <process.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "estruct.h"
#include "../src/base.h"

extern Char * Pascal NEAR flook();
char * Pascal NEAR strpcpy();

#if S_MSDOS == 0
  MSDOS only
#endif

union REGS rg;
struct SREGS segreg;


#if MSC && 0
 extern int _doserrno;
#endif


/*	EXECPROG:	A function to execute a named program
			with arguments
*/

#if	LATTICE | AZTEC | MWC
#define	CFLAG	1
#endif

Cc mse_cc;

#if 0

int Pascal NEAR execprog(cmd)
	char *cmd;	/*  Incoming command line to execute  */
{
    char f1[38];	    /* FCB1 area (not initialized */
    char f2[38];	    /* FCB2 area (not initialized */
    char prog[NSTRING+10];  /* program filespec */

		    /* parse the command name from the command line */
    static const char exetyps[3][5] = { "", ".exe", ".com"};
    register char * sp;
    register short ix = 0;
    register char ch;

    for (; (ch = cmd[ix]) != 0 && ch != ' ' && ch != '\t'; )
      prog[ix++] = ch;

	     /* look up the program on the path trying various extensions */
    for (ch = 3; ; )
    { if (--ch < 0)
	return false;
      strcpy(&prog[ix], &exetyps[ch][0]);
      sp = flook(0, prog);
      if (sp != NULL)
	break;
    }
    strcpy(prog, sp);
				       /* and parse out the command tail */
    while ((ch = cmd[ix]) == ' ' || ch == '\t')
      ++ix;

    cmd += ix;

#define progl ix
#define tail sp
    progl = strlen(prog);
    tail = &prog[progl+1];
    
    tail[0] = (char)strlen(cmd);   /* record the byte length */
    if (progl + tail[0] > NSTRING + 8)
      tail[0] = NSTRING + 8 - progl;

    strpcpy(&tail[1], cmd, tail[0]+1);
    
#if  MWC == 0
		/* get a pointer to this PSPs environment segment number */
    segread(&segreg);
#endif
{ struct Pblock 		/* set up EXEC parameter block */
  {   short envptr;   /* 2 byte pointer to environment string */
      char *cline;    /* 4 byte pointer to command line */
      char *fcb1;     /* 4 byte pointer to FCB at PSP+5Ch */
      char *fcb2;     /* 4 byte pointer to FCB at PSP+6Ch */
  } pblock;
    pblock.envptr = 0;    /* make the child inherit the parents env */
    pblock.fcb1 = f1;             /* point to a blank FCB */
    pblock.fcb2 = f2;             /* point to a blank FCB */
    pblock.cline = strcat(tail, "\r"); /* parameter line pointer */

			    /* and make the call */
    rg.x.ax = 0x4b00;	    /* EXEC Load or Execute a Program */
			    /* load end execute function subcode */
    rg.x.dx = (unsigned int)prog;
    rg.x.bx = (unsigned int)&pblock;
#if	AZTEC | MWC
    rg.x.ds = ((unsigned long)prog) >> 16;    /* program name ptr */
    rg.x.es = rg.x.ds;
  /*rg.x.es = ((unsigned long)&pblock) >> 16;  * set up param block ptr */
#else
    segreg.ds = ((unsigned long)prog) >> 16;	/* program name ptr */
    segreg.es = ((unsigned long)&pblock) >> 16; /* set up param block ptr */
#endif

#if	NOVELL
    mse_cc = execpr(prog, &pblock);
#else
#if	LATTICE 
    if (intdosx(&rg, &rg, &segreg) & CFLAG)
      mse_cc = -_oserr; 	    /* failed child call */
    else
    { rg.h.ah = 0x4d;	    /* get child process return code */
      intdos(&rg, &rg);     /* go do it */
      mse_cc = rg.x.ax;     /* save child's return code */
    } 
#endif
#if	AZTEC
    if (sysint(0x21, &rg, &rg) & CFLAG)
      mse_cc = -errno;		    /* failed child call */
    else
    { rg.h.ah = 0x4d;	    /* get child process return code */
      sysint(0x21, &rg, &rg);	    /* go do it */
      mse_cc = rg.x.ax;     /* save child's return code */
    } 
#endif
#if	MWC
    intcall(&rg, &rg, DOSINT);
    if (rg.x.flags & CFLAG)
      mse_cc = -errno;		    /* failed child call */
    else
    { rg.h.ah = 0x4d;	    /* get child process return code */
      intcall(&rg, &rg, DOSINT);    /* go do it */
      mse_cc = rg.x.ax;     /* save child's return code */
    } 
#endif
#if	(TURBO | MSC | DTL)
    intdosx(&rg, &rg, &segreg);
    if (rg.x.cflag != 0)
      mse_cc = -_doserrno;  /* failed child call */
    else
    { rg.h.ah = 0x4d;	    /* get child process return code */
      intdos(&rg, &rg);     /* go do it */
      mse_cc = rg.x.ax;     /* save child's return code */
    } 
#endif
#endif
    return mse_cc >= 0;
}}

#endif



/*	SHELLPROG: Execute a command in a subshell		*/

int Pascal NEAR shellprog(wh, cmd)
	 char  wh;
	 char *cmd;	/*  Incoming command line to execute  */
{   extern char ebuffer[];
#define comline ebuffer			/* constructed command line */
    extern char * getenv();
    register char ch;
    register char * sp;
#define shell sp

    if (wh == '!')
    {
		/*  detect current switch character and set us up to use it */
	rg.x.ax = 0x3700;	/*  get setting data  */
				/*  al: get switch character to use */
	intdos(&rg, &rg);
     	/* get (char)rg.h.dl */
					/*  get name of system shell  */
        shell = getenv("COMSPEC");
	if (shell == NULL)
	  return false;			/*  No shell located  */

	while ((ch = *cmd) == ' ' || ch == '\t')
	  cmd++;
		 /**  If the command line is not empty, bring up the shell  **/
		 /**  and execute the command.  Otherwise, bring up the     **/
		 /**  shell in interactive mode.   **/
	if (ch == 0)
	  cmd = shell;
	else
	{ static char qc [] = " ?c ";
	  qc[1] = (char)rg.h.dl;	/* replace the ? */
	  concat(&comline[0], shell, qc, cmd, null);
	/*comline[strlen(shell) + 1] = (char)rg.h.dl;	/* replace the ? */
	  cmd = comline;
	}
    }
    
#if 0
    return execprog(cmd);
#else
{
    char f1[38];	    /* FCB1 area (not initialized */
    char f2[38];	    /* FCB2 area (not initialized */
    char prog[NSTRING+10];  /* program filespec */

		    /* parse the command name from the command line */
    static const char exetyps[3][5] = { "", ".exe", ".com"};
    register short ix = 0;

    for (; (ch = cmd[ix]) != 0 && ch != ' ' && ch != '\t'; )
      prog[ix++] = ch;

	     /* look up the program on the path trying various extensions */
    for (ch = 3; ; )
    { if (--ch < 0)
	return false;
      strcpy(&prog[ix], &exetyps[ch][0]);
      sp = flook(0, prog);
      if (sp != NULL)
	break;
    }
    strcpy(prog, sp);
				       /* and parse out the command tail */
    while ((ch = cmd[ix]) == ' ' || ch == '\t')
      ++ix;

    cmd += ix;

#define progl ix
#define tail sp
    progl = strlen(prog);
    tail = &prog[progl+1];
    
    tail[0] = (char)strlen(cmd);   /* record the byte length */
    if (progl + tail[0] > NSTRING + 8)
      tail[0] = NSTRING + 8 - progl;

    strpcpy(&tail[1], cmd, tail[0]+1);
    
#if  MWC == 0
		/* get a pointer to this PSPs environment segment number */
    segread(&segreg);
#endif
{ struct Pblock 		/* set up EXEC parameter block */
  {   short envptr;   /* 2 byte pointer to environment string */
      char *cline;    /* 4 byte pointer to command line */
      char *fcb1;     /* 4 byte pointer to FCB at PSP+5Ch */
      char *fcb2;     /* 4 byte pointer to FCB at PSP+6Ch */
  } pblock;
    pblock.envptr = 0;    /* make the child inherit the parents env */
    pblock.fcb1 = f1;             /* point to a blank FCB */
    pblock.fcb2 = f2;             /* point to a blank FCB */
    pblock.cline = strcat(tail, "\r"); /* parameter line pointer */

			    /* and make the call */
    rg.x.ax = 0x4b00;	    /* EXEC Load or Execute a Program */
			    /* load end execute function subcode */
    rg.x.dx = (unsigned int)prog;
    rg.x.bx = (unsigned int)&pblock;
#if	AZTEC | MWC
    rg.x.ds = ((unsigned long)prog) >> 16;    /* program name ptr */
    rg.x.es = rg.x.ds;
  /*rg.x.es = ((unsigned long)&pblock) >> 16;  * set up param block ptr */
#else
    segreg.ds = ((unsigned long)prog) >> 16;	/* program name ptr */
    segreg.es = ((unsigned long)&pblock) >> 16; /* set up param block ptr */
#endif

#if	NOVELL
    mse_cc = execpr(prog, &pblock);
#else
#if	LATTICE 
    if (intdosx(&rg, &rg, &segreg) & CFLAG)
      mse_cc = -_oserr; 	    /* failed child call */
    else
    { rg.h.ah = 0x4d;	    /* get child process return code */
      intdos(&rg, &rg);     /* go do it */
      mse_cc = rg.x.ax;     /* save child's return code */
    } 
#endif
#if	AZTEC
    if (sysint(0x21, &rg, &rg) & CFLAG)
      mse_cc = -errno;		    /* failed child call */
    else
    { rg.h.ah = 0x4d;	    /* get child process return code */
      sysint(0x21, &rg, &rg);	    /* go do it */
      mse_cc = rg.x.ax;     /* save child's return code */
    } 
#endif
#if	MWC
    intcall(&rg, &rg, DOSINT);
    if (rg.x.flags & CFLAG)
      mse_cc = -errno;		    /* failed child call */
    else
    { rg.h.ah = 0x4d;	    /* get child process return code */
      intcall(&rg, &rg, DOSINT);    /* go do it */
      mse_cc = rg.x.ax;     /* save child's return code */
    } 
#endif
#if	(TURBO | MSC | DTL)
    intdosx(&rg, &rg, &segreg);
    if (rg.x.cflag != 0)
      mse_cc = -_doserrno;  /* failed child call */
    else
    { rg.h.ah = 0x4d;	    /* get child process return code */
      intdos(&rg, &rg);     /* go do it */
      mse_cc = rg.x.ax;     /* save child's return code */
    } 
#endif
#endif
    return mse_cc;
}}

#endif
}

