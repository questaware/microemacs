/* -*- C -*- ****************************************************************/
/*	OSWIN.C:	Operating specific I/O and Spawning functions
			under the Win32 operating system
			for MicroEMACS 3.10
*/
//#include        <windows.h>
/*#include        <winuser.h>*/
#include   <stdio.h>
#include   <conio.h>
#include   <windows.h>
#include   <process.h>

#include  "estruct.h"

#include  "../src/edef.h"
#include	"../src/etype.h"
#define Short short

#include	"../src/elang.h"
#include	"../src/logmsg.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#if USE_UBAR
#define stat _stat
#define O_RDONLY _O_RDONLY
#endif

extern char *getenv();
extern void ClearScreen(void);

#define MLIX 3

extern char lastline[MLIX+1][NSTRING];

#define P_AWAIT_PROMPT 1

			/* The Mouse driver only works with typeahead defined */
#if	MOUSE
static int mexist;	/* is the mouse driver installed? */
static int nbuttons;	/* number of buttons on the mouse */
static int oldbut;	/* Previous state of mouse buttons */
#endif

int g_chars_since_shift;
int g_timeout_secs;

int flagerr(const char * fmt)

{ DWORD ec = GetLastError();
  mlwrite(fmt, ec);
  tcapbeep();
  ttgetc();
  return ec;
}

int ErrorMessage(const char *str)  //display detailed error info

{
#if _DEBUG
	LPVOID msg;
  FormatMessage(
              FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
              NULL,
              GetLastError(),
              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
              (LPTSTR) &msg,
              0,
              NULL
               );
  mlwrite("%s: %s\n",str,msg);
  LocalFree(msg);
	mbwrite(NULL);
	return -1;
#else
	char buf[100];
	return -flagerr(strcat(strcpy(buf,str)," %d"));
#endif
}


#if 0
/*
sprintf()

{ char ch;
  flagerr("printf?");
}

fprintf()

{ sprintf();
}

printf()

{ sprintf();
}


char * memcpy(t_, s_, len_)
      char * t_, *s_;
      int    len_;
{ register int len = len_;
  register char * t = t_;
  register char * s = s_;
  
  if (t <= s)
    while (--len >= 0)
      *t++ = *s++;
  else 
  { t = &t[len];
    s = &s[len];
    
    while (--len >= 0)
      *--t = *--s;
  }
  return t_;
}

*/
#endif

#if 0
X
X#define	IBUFSIZE  4		/* must be a power of 2 */
X
Xunsigned char in_buf[IBUFSIZE];	/* input character buffer */
XShort in_next = 0;		/* pos to retrieve next input character */
XShort in_last = 0;		/* pos to place most recent input character */
X
X
X#define in_init() in_next = in_last = 0   /* initialize the input buffer */
X
X#define in_check() (in_next != in_last)   /* is the input buffer non-empty? */
X
X
Xvoid Pascal in_put(event)
X	int event;	/* event to enter into the input buffer */
X{ in_buf[in_last & (IBUFSIZE - 1)] = event;
X  in_last += 1;
X}
X
X
X#define in_get() in_buf[in_next++ & (IBUFSIZE - 1)]
X				/* get an event from the input buffer */
#endif

	/*
	 * This function is called once to set up the terminal device streams.
	 */
#if	MOUSE

void ttopen()

{
/*if (gflags & MD_NO_MMI)
	  return; */
	long miaddr;	/* mouse interupt routine address */

			/* we are not sure of the initial cursor position */
#if 0
	ttrow = 2;
	ttcol = 2;
#endif

	mexist = FALSE;  /* provisionally */
				/* check if the mouse drive exists first */
#if MSC | TURBO | DTL | LATTICE | MWC
	rg.x.ax = 0x3533;	/* look at the interrupt 33 address */
	int86x(0x21, &rg, &rg, &segreg);
	miaddr = (((long)segreg.es) << 16) + (long)rg.x.bx;
	if (miaddr == 0 || *(char * far)miaddr == 0xcf)
#endif
	  return;
				/* and then check for the mouse itself */
	rg.x.ax = 0;		/* mouse status flag */
	int86(0x33, &rg, &rg);	/* check for the mouse interupt */
	nbuttons = rg.x.bx;
     /* in_init(); */
	mexist = 0;
	rg.x.dx = 0;		                /* top row */
	if (rg.x.ax == 0)
	  return;
       
	mexist = 1;
		        		/* put it in the upper right corner */
	rg.x.ax = 4;			/* set mouse cursor position */
	rg.x.cx = (term.t_ncol - 1) << 3;	/* last col of display */
	int86(0x33, &rg, &rg);
					/* and set its attributes */
	rg.x.ax = 10;		/* set text cursor */
	rg.x.bx = 0;		/* software text cursor please */
	rg.x.cx = 0x77ff;	/* screen mask */
	rg.x.dx = 0x7700;	/* cursor mask */
	int86(0x33, &rg, &rg);
}


void maxlines(lines)		/* set number of vertical rows for mouse */
	int lines;						/* # of vertical lines */
{
	if (mexist)
	{ rg.x.ax = 8;		/* set min/max vertical cursor position */
	  rg.x.cx = 0;		/* start at 0 */
	  rg.x.dx = (lines - 1)<<3; /* end at the end */
	  int86(0x33, &rg, &rg);
	}
}

#endif  /* MOUSE */


#if 0
/*
 * Flush terminal buffer. Does real work where the terminal output is buffered
 * up. A no-operation on systems where byte at a time terminal I/O is done.
 */
Pascal ttflush()
{
}

#endif


#if 0
X
Xstatic const char exttbl1[] = 
X{ 
X	'Q',
X	'W',
X	'E',
X	'R',
X	'T',  /* 20 */
X	'Y',
X	'U',
X	'I',
X	'O',
X	'P',  /* 25 */
X	26,
X	27,
X	28,
X	29,
X	'A',	/* 30 */
X	'S',
X	'D',
X	'F',
X	'G',
X	'H',
X	'J',
X	'K',
X	'L',	/* 38 */
X	39,
X	40,
X	41,
X	42,
X	43,	
X	'Z',	/* 44 */
X	'X',
X	'C',
X	'V',
X	'B',
X	'N',
X	'M',	/* 50 */
X	51,
X	52,
X	53,
X	54,
X	55,
X	56,
X	57,
X	58,
X};
X
Xstatic const char exttbl2[] = 
X{
X        '<',	/* HOME */
X	'P',	/* cursor up */
X	'Z',	/* page up */
X	74,
X	'B',	/* cursor left */
X	75,
X	'F',	/* cursor right */
X	76,
X	'>',	/* end */
X	'N',	/* cursor down */
X	'V',	/* page down */
X	'C',	/* insert */
X	'D',	/* delete */
X};
X
Xstatic const char exttbl3[] = 
X{     	'B',	/* control left */
X	'F',	/* control right */
X	'>',	/* control END */
X	'V',	/* control page down */
X	'<',	/* control HOME */
X};
X
Xstatic const short exttbl4[] = 
X{	ALTD | 131,
X        SPEC | CTRL | 'Z',	/* control page up */
X      	SPEC | ':',		/* F11 */
X	SPEC | ';',		/* F12 */
X      	SPEC | SHFT | ':',	/* SHFT + F11 */
X	SPEC | SHFT | ';',	/* SHFT + F12 */
X      	SPEC | CTRL | ':',	/* CTRL + F11 */
X	SPEC | CTRL | ';',	/* CTRL + F12 */
X      	SPEC | ALTD | ':',	/* ALT + F11 */
X	SPEC | ALTD | ';',	/* ALT + F12 */
X};
X
Xstatic const char mod10[] = "1234567890";
X
X/*	extcode:	resolve MSDOS extended character codes
X			encoding the proper sequences into emacs
X			printable character specifications
X*/
Xint extcode(c_)
X	unsigned c_;	/* byte following a zero extended char byte */
X{ register unsigned char c = c_;
X
X  if (c < 15)
X    return ALTD | c;
X					/* backtab */
X  if (c == 15)
X    return SHFT | CTRL | 'I';
X					/* ALTed letter keys */ 
X  if (c <= 58)
X    return ALTD | exttbl1[c - 16]; 
X        				/* function keys 1 through 10 */
X  if (c < 69)
X    return SPEC | mod10[c - 59];
X					/* movement keys */
X  if (c < 84)
X    return SPEC | exttbl2[c-71];
X					/* shifted function keys */
X  if (c < 94)
X    return SPEC | SHFT | mod10[c - 84];
X					/* control function keys */
X  if (c < 104)
X    return SPEC | CTRL | mod10[c - 94];
X					/* ALTed function keys */
X  if (c < 114)
X    return SPEC | ALTD | mod10[c - 104];
X					/* control movement keys */
X  if (c < 120)
X    return SPEC | CTRL | exttbl3[c-115];
X					/* ALTed number keys */
X  if (c <= 130)
X    return ALTD | mod10[c - 120];
X					/* F10, F11 keys */  
X  if (c < 141)
X    return exttbl4[c - 131];
X
X  return ALTD | c;
X}
X
#else

static const unsigned char scantokey[] =
{	'1',		/* 3b */
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',		/* 44 */
	'W',		/* pause */
	'L',		/* Scroll Lock */
	'<',		/* Home */
	'P',		/* Up */
	'Z',		/* Page Up */
	'J',		/* key not known *//* 4a */
	'B',		/* Left */
	'K',		/* key not known */
	'F',		/* Right */
	'L',		/* key not known */
	'>',		/* End */
	'N',		/* Down */
	'V',		/* Page Down */
	'C',		/* Insert */
	'D',		/* Delete */
	'Q',		/* key not known *//* 54 */
	'q',		/* key not known */
	'x',		/* key not known */
	':',
	';',		/* 58 */
};
        
#if 0

static const short exttbl[] = 
{ 
	SHFT | CTRL | 'I',
	ALTD | 'Q',
	ALTD | 'W',
	ALTD | 'E',
	ALTD | 'R',
	ALTD | 'T',  /* 20 */
	ALTD | 'Y',
	ALTD | 'U',
	ALTD | 'I',
	ALTD | 'O',
	ALTD | 'P',  /* 25 */
	ALTD | 26,
	ALTD | 27,
	ALTD | 28,
	ALTD | 29,
	ALTD | 'A',	/* 30 */
	ALTD | 'S',
	ALTD | 'D',
	ALTD | 'F',
	ALTD | 'G',
	ALTD | 'H',
	ALTD | 'J',
	ALTD | 'K',
	ALTD | 'L',	/* 38 */
	ALTD | 39,
	ALTD | 40,
	ALTD | 41,
	ALTD | 42,
	ALTD | 43,	
	ALTD | 'Z',	/* 44 */
	ALTD | 'X',
	ALTD | 'C',
	ALTD | 'V',
	ALTD | 'B',
	ALTD | 'N',
	ALTD | 'M',	/* 50 */
	ALTD | 51,
	ALTD | 52,
	ALTD | 53,
	ALTD | 54,
	ALTD | 55,
	ALTD | 56,
	ALTD | 57,
	ALTD | 58,
	SPEC | '1',
	SPEC | '2',
	SPEC | '3',
	SPEC | '4',
	SPEC | '5',
	SPEC | '6',
	SPEC | '7',
	SPEC | '8',
	SPEC | '9',
	SPEC | '0',
  ALTD | 69,
	ALTD | 70,
  SPEC | '<',	/* HOME */
	SPEC | 'P',	/* cursor up */
	SPEC | 'Z',	/* page up */
	SPEC | 74,
	SPEC | 'B',	/* cursor left */
	SPEC | 75,
	SPEC | 'F',	/* cursor right */
	SPEC | 76,
	SPEC | '>',	/* end */
	SPEC | 'N',	/* cursor down */
	SPEC | 'V',	/* page down */
	SPEC | 'C',	/* insert */
	SPEC | 'D',	/* delete */
	SPEC | SHFT | '1',
	SPEC | SHFT | '2',
	SPEC | SHFT | '3',
	SPEC | SHFT | '4',
	SPEC | SHFT | '5',
	SPEC | SHFT | '6',
	SPEC | SHFT | '7',
	SPEC | SHFT | '8',
	SPEC | SHFT | '9',
	SPEC | SHFT | '0',
	SPEC | CTRL | '1',
	SPEC | CTRL | '2',
	SPEC | CTRL | '3',
	SPEC | CTRL | '4',
	SPEC | CTRL | '5',
	SPEC | CTRL | '6',
	SPEC | CTRL | '7',
	SPEC | CTRL | '8',
	SPEC | CTRL | '9',
	SPEC | CTRL | '0',
	SPEC | ALTD | '1',
	SPEC | ALTD | '2',
	SPEC | ALTD | '3',
	SPEC | ALTD | '4',
	SPEC | ALTD | '5',
	SPEC | ALTD | '6',
	SPEC | ALTD | '7',
	SPEC | ALTD | '8',
	SPEC | ALTD | '9',
	SPEC | ALTD | '0',
	ALTD | 114,
  SPEC | CTRL | 'B',	/* control left */
	SPEC | CTRL | 'F',	/* control right */
	SPEC | CTRL | '>',	/* control END */
	SPEC | CTRL | 'V',	/* control page down */
	SPEC | CTRL | '<',	/* control HOME */
	ALTD | '1',
	ALTD | '2',
	ALTD | '3',
	ALTD | '4',
	ALTD | '5',
	ALTD | '6',
	ALTD | '7',
	ALTD | '8',
	ALTD | '9',
	ALTD | '0',
	ALTD | 130,
	ALTD | 131,
	ALTD | 132,
	SPEC | ':',		/* F11 */
	SPEC | ';',		/* F12 */
	SPEC | SHFT | ':',	/* SHFT + F11 */
	SPEC | SHFT | ';',	/* SHFT + F12 */
	SPEC | CTRL | ':',	/* CTRL + F11 */
	SPEC | CTRL | ';',	/* CTRL + F12 */
	SPEC | ALTD | ':',	/* ALT + F11 */
	SPEC | ALTD | ';',	/* ALT + F12 */
	ALTD | 141,
	ALTD | 142,
	ALTD | 143,
	ALTD | 144,
	ALTD | 145,
	SPEC | CTRL | 'C',	/* insert */
	SPEC | CTRL | 'D',	/* delete */
};

/*	extcode:	resolve MSDOS extended character codes
			encoding the proper sequences into emacs
			printable character specifications
*/
int extcode(c_)
	unsigned c_;	/* byte following a zero extended char byte */
{ register short c = c_ - 15;

  return c > 147 - 15 ? ALTD | c : exttbl[c];
}

#endif
#endif

#if ISRCH == 0
#undef GOTTYPAH
#endif

#if GOTTYPAH
				 /* typahead: See if any characters are already in the keyboard buffer */
int Pascal typahead()

{	return _kbhit();
}


static int g_eaten_char = 1000000;		 /* Re-eaten char */


void Pascal reeat(int c)

{	g_eaten_char = c;			/* save the char for later */
}

#endif

void flush_typah()

{ 
#if GOTTYPAH
	g_eaten_char = -1;
#endif
	while (_kbhit())
    (void)ttgetc();
}


extern CONSOLE_SCREEN_BUFFER_INFO csbiInfo;  /* Orig Console information */
extern CONSOLE_SCREEN_BUFFER_INFO csbiInfoO;  /* Orig Console information */

static HANDLE g_ConsIn;
//static HWND g_origwin = NULL;

static int g_got_ctrl = false;


BOOL WINAPI MyHandlerRoutine(DWORD dwCtrlType)

{ INPUT_RECORD rec_;
  DWORD ct;
  int rc;
 
/* Beep(400,200);*/

  rec_.EventType = KEY_EVENT;
  rec_.Event.KeyEvent.bKeyDown = true;
  rec_.Event.KeyEvent.wRepeatCount = 1;
  rec_.Event.KeyEvent.uChar.AsciiChar = 'C';
  rec_.Event.KeyEvent.wVirtualKeyCode = 'C';
  rec_.Event.KeyEvent.dwControlKeyState = RIGHT_CTRL_PRESSED;
  rec_.Event.KeyEvent.wVirtualScanCode = 0x20;

  rc = WriteConsoleInputA( g_ConsIn, &rec_, 1, &ct);
  if (rc == 0 || ct != 1)
  { /*DWORD ec = GetLastError();
    mlwrite("Err %d %d", ec, ct);
    Beep(600, 1000);*/
    g_got_ctrl = true;
  }
  return true;
}



void setMyConsoleIP()

{ g_ConsIn = GetStdHandle( STD_INPUT_HANDLE );
  if (g_ConsIn < 0)					                    /* INVALID_HANDLE_VALUE */
    flagerr("Piperr %d");

  (void)SetConsoleMode(g_ConsIn, ENABLE_WINDOW_INPUT);	// Allowed to fail
}


void Pascal MySetCoMo()

{ SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	HANDLE h = CreateFile("CONIN$",
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        &sa,
                        OPEN_EXISTING,
                        0, NULL); // ignored
	if(h == INVALID_HANDLE_VALUE || !SetStdHandle(STD_INPUT_HANDLE, h))
    flagerr("SCCFSHErr %d");

	setMyConsoleIP();

  SetConsoleCtrlHandler(MyHandlerRoutine, true);
#if 0
{ HWND mwh = GetForegroundWindow();
  if (mwh == NULL)
    flagerr("MwHerr %d");

{ Cc cc = SetWindowPos(mwh, HWND_TOP, 10,10,
//        	 ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
//	         ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2), 
        	   					 0, 0, SWP_NOSIZE /*| SWP_NOACTIVATE*/); 
  if (cc == 0)
    flagerr("SwPerr %d");
}}
#endif
//g_origwin = GetForegroundWindow();
}


/*
typedef struct _WINDOWPLACEMENT { 
    UINT  length; 
    UINT  flags; 
    UINT  showCmd; 
    POINT ptMinPosition; 
    POINT ptMaxPosition; 
    RECT  rcNormalPosition; 
} WINDOWPLACEMENT; 
*/

/*
 * Read a character from the terminal, performing no editing and doing no echo
 * at all. Also mouse events are forced into the input stream here.
 */
int Pascal ttgetc()

{
#if GOTTYPAH
	if (g_eaten_char >= 0)
	{ int c = g_eaten_char;
		g_eaten_char = -1;
		if (c != 1000000)
			return c;
//	(void)millisleep(100);
	}
#endif

#if MOUSE > 0
	while (TRUE)
	{
 #if 0
		if (in_check()) 		/* the type ahead buffer */
			return in_get();
 #endif
								/* with no mouse, this is a simple get char routine */
		if (mexist == FALSE || mouseflag == FALSE
 #if TYPEAH
			|| typahead()
 #endif
			 ) break;

			rg.x.ax = 1;						/* Show Cursor */
			int86(0x33, &rg, &rg);
								/* loop waiting for something to happen */
			while (TRUE)
			{ 
 #if TYPEAH
				if (typahead())
					break;
 #endif
				if (checkmouse())
					break;
			}
												/* turn the mouse cursor back off */
			rg.x.ax = 2;			/* Hide Cursor */
			int86(0x33, &rg, &rg);
		} /* while */
	}
#endif
#if _DEBUG
  if (g_ConsIn == 0)
  	mbwrite("Int Err.7");
#endif
{	int totalwait = g_timeout_secs;
/*int cRecords = 0;
	PeekConsoleInput(g_hConIn, &ir, 1, &cRecords);*/

  while (1)
  { DWORD actual;
  	const DWORD lim = 1000;
  	INPUT_RECORD rec;
  	int keystate;

    int cc = WaitForSingleObject(g_ConsIn, lim);
    switch(cc)
    { case WAIT_TIMEOUT: 
    					if (g_got_ctrl)
							{ g_got_ctrl = false;
							  return (int)(CTRL | 'C');
							}

							if (--totalwait == 0)			// -w opt
								exit(2);

							continue;
			case WAIT_OBJECT_0:
//					  SetConsoleMode(g_ConsIn, ENABLE_WINDOW_INPUT);
							cc = ReadConsoleInput(g_ConsIn, &rec, (DWORD)1, &actual);
							if (!cc || actual < 1)
						  {	DWORD errn = GetLastError();
								millisleep(10); // _sleep(10);
						   	if (errn != 6)
								{	mlwrite("Error %d %d %d ", cc, g_ConsIn, errn);
									mbwrite(NULL);
								}
						    continue;
						  }
							break;
			default:continue;
    }

		keystate = rec.Event.KeyEvent.dwControlKeyState;
    if (keystate & LEFT_CTRL_PRESSED)
			g_chars_since_shift = 0;

    if      (rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown)
    { int ctrl = 0;
      int chr = rec.Event.KeyEvent.uChar.AsciiChar & 0xff;
      if (in_range(rec.Event.KeyEvent.wVirtualKeyCode, 0x10, 0x12))
        continue; /* shifting key only */

		{ int vsc = rec.Event.KeyEvent.wVirtualScanCode - 0x3b;

      if (keystate & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED))
        ctrl = CTRL;

      if (keystate & SHIFT_PRESSED)
      { if (ctrl || 
             (in_range(vsc, 0, 0x58 - 0x3b) &&
	            chr != 0x7c))
          ctrl |= SHFT;
      }
      if (keystate & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED))
      { chr = rec.Event.KeyEvent.wVirtualKeyCode;
	 		  if (in_range(chr, 'a', 'z'))
      		chr += 'A' - 'z';
	 		  ctrl |= ALTD;
			}
      
      if (in_range(vsc, 0, 0x58 - 0x3b) &&
	   		  chr != 0x7c && chr != '\\'
	  		 )
      { return (int)(ctrl | SPEC | scantokey[vsc]);
#if 0
        in_put(ctrl >> 8);
        in_put(ctrl & 255);
        return 0;
#endif
      }
       
			++g_chars_since_shift;
      return (int)(ctrl | (chr == 0xdd ? 0x7c : chr));
    }}
    else if (rec.EventType == MENU_EVENT)
    { /*loglog1("Menu %x", rec.Event.MenuEvent.dwCommandId);*/
    }  
  }
}}

#define LAUNCH_BUFFERNM      0x0001      /* Do not use the comspec    */
#define LAUNCH_SILENT        0x0002      /* Do not use the comspec    */
#define LAUNCH_NOCOMSPEC     0x0004      /* Do not use the comspec    */
#define LAUNCH_DETACHED      0x0008      /* Detached process launch   */
#define LAUNCH_LEAVENAMES    0x0010      /* Leave the names untouched */
#define LAUNCH_SHOWWINDOW    0x0020      /* Dont hide the new window  */
#define LAUNCH_USER_FLAGS    0x002F      /* User flags bitmask        */
#define LAUNCH_SHELL         0x0100
#define LAUNCH_SYSTEM        0x0200
#define LAUNCH_FILTER        0x0400
#define LAUNCH_PIPE          0x0800
#define LAUNCH_IPIPE         0x1000
#define LAUNCH_STDIN         0x2000
#define LAUNCH_STDERROUT     0x4000

#if CREATE_NEW_PROCESS_GROUP != LAUNCH_SYSTEM
error error
#endif

static char * mkTempName (/*out*/char *buf, const char *name)
{
#ifdef _CONVDIR_CHAR
 #define DIRY_CHAR _CONVDIR_CHAR
#else
 #define DIRY_CHAR DIRSEPCHAR
#endif
	static const char *tmpDir = NULL ;
				 char c2[2];
	const  char * td = tmpDir != NULL ? tmpDir : (char *)getenv("TEMP");

	c2[0] = c2[1] = 0;

	if (td == NULL)
#if (defined _DOS) || (defined _WIN32)
						/* the C drive : better than ./ as ./ could be on a CD-Rom etc */
		td = "c:\\" ;
#else
		td = "./" ;
#endif
	else
		if (td[strlen(td)-1] != DIRY_CHAR)
			c2[0] = DIRY_CHAR;
	
	tmpDir = td;
	concat(buf,td,c2,"me",int_asc(_getpid()),name,0);

	return &buf[strlen(buf)];
}


static char * mkTempCommName(/*out*/char *filename, char *basename)
{
	char *ss = mkTempName(filename,basename) - 3;
	int iter = 25;
	
	while (--iter >= 0 && !fexist(filename))
	{
		HANDLE hdl = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,
													  OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (hdl >= 0) 			/*INVALID_HANDLE_VALUE*/
		{  CloseHandle(hdl);
			 break ;
		}
		ss[0] = 'A' + 24 - iter;				// File in use?
		ss[1] = '~';
		ss[2] = 0;
	}
	return filename;
}

#ifdef _WIN32s
error error

/**************************************************************************
* Function: DWORD SynchSpawn(LPTSTR, UINT)																*
* 																																				*
* Purpose: Thunk to 16-bit code. This allows a synchronous process spawn	*
* i.e. it only returns when the new process has been created. 						*
**************************************************************************/
static DWORD
				SynchSpawn( LPCSTR lpszCmdLine, UINT nCmdShow )
{
	static int doneOnce = 0;						/* Have loaded DLL once */
	UT32PROC pfnUTProc = NULL;
	BOOL fWin32s;
	DWORD Args[2];
	PVOID Translist[2];
	DWORD status;
	DWORD dwVersion = GetVersion(); 	/* Find out if we're running on Win32s */
														
	if (!(BOOL)(!(dwVersion < 0x80000000)) && (LOBYTE(LOWORD(dwVersion)) < 4))
		return 0; 											/* Not win32s */

			/* Register the 16bit DLL. We do this when we are called. This saves
				 problems with a win32s 32-bit DLL under Win 3.1 with win32s installed.*/
again:
	if (UTRegister (ttInstance, 				/* 'me32s' module handle */
									"methnk16.dll", 		/* 16-bit thunk dll */
									NULL, 							/* Nothing to do */
									"UTProc", 					/* 16-bit dispatch routine */
									&pfnUTProc, 				/* Receives thunk address */
									NULL, 							/* No callback function */
									NULL) == FALSE) 		/* no shared memroy */
	{
				/* This fails the first time !! */
		if (doneOnce == 0)
		{
			doneOnce = 1;
			goto again;
		}
		return 0;
	}

		/* Build the argument list to the 16 bit side */
	Args[0] = (DWORD) lpszCmdLine;
	Args[1] = (DWORD) nCmdShow;

	Translist[0] = &Args[0];
	Translist[1] = NULL;

	status = (* pfnUTProc)(Args, SYNCHSPAWN, Translist);

	UTUnRegister (ttInstance);			/* Unregister the DLL */
	return status;
}
#endif

//#if _MSC_VER < 1900
#undef VS_CHAR8
#define VS_CHAR8 1
//#endif 

#if _DEBUG
#define NULL_OP stdout
#else
#define NULL_OP NULL
#endif


#define WL_IHAND 1
#define WL_SHELL 2
#define WL_AWAIT_PROMPT 4
#define WL_HOLD  8
#define WL_CNC   CREATE_NEW_CONSOLE
#define WL_CNPG	 CREATE_NEW_PROCESS_GROUP
#if WL_CNC <= WL_HOLD || WL_CNPG <= WL_HOLD
error error
#endif
#define WL_SPAWN   0x1000
#define WL_SHOWW	 0x2000
#define WL_NOIHAND 0x4000

static int
																						/* flags: above */
WinLaunch(Cc *sysRet, int flags,
					const char *app, const char * ca, 
          const char *in_data, const char *infile, const char *outfile 
          	 // char *outErr
				 )
{ char buff[1024];           //i/o buffer
	const char * fapp = NULL;
	if (app != NULL)
	{ int ct = 2;
		char * app_ = (char*)app;
		
		while ((fapp = flook('P', app_)) == NULL && --ct > 0)
			app_ = strcat(strcpy(buff, app_), ".exe");
	}
	
	if      (fapp != NULL)					// never use comspec
		app = fapp;
	else if (flags & WL_SHELL)
	{	const char * comSpecName = (char*)getenv("COMSPEC");
		if (comSpecName == NULL)
			comSpecName = "cmd.exe";

		if (ca == NULL)
		{ app = NULL;
			ca = comSpecName;
		}
		else									/* Create the command line */
		{ int len = strlen(concat(buff," /c \"", app == NULL ? "" : app,0));
			char * dd = buff+len;
			char ch;
			char prev = 'A';
			const char * ss = ca;
			if (len+strlen(ss) >= sizeof(buff))
				return -1;

			for (; (ch = *ss++); prev = ch)
			{	if (ch == '/' && 										// &&!(flags & LAUNCH_LEAVENAMES)
					  (in_range(toupper(prev), 'A','Z')
				  || in_range(prev, '0', '9')
					||					prev == '_'  || prev == ' '))
					ch = '\\';
		
				if (ch == '"')
					*dd++ = '\\';
				*dd++ = ch;
			}

			*dd = '"';
			dd[1] = 0;
			ca = buff;
			app = comSpecName;
		}
	}
	
	if (app != NULL && ca != NULL && ca[0] != ' ')
		return -1000;

{ char	dummyInFile[NFILEN] = ""; 		// Dummy input file
	HANDLE newstdout = 0;								// pipe handles
  HANDLE write_stdin = 0;
  HANDLE read_stdout = 0;
	Cc wcc = OK;
  STARTUPINFO si;
//SECURITY_DESCRIPTOR sd;               //security information for pipes
  PROCESS_INFORMATION pi;
  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = NULL;
  sa.bInheritHandle = TRUE;         //allow inheritable handles
	pi.hProcess = 0;

	sgarbf = TRUE;
//memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	if (flags & WL_SHOWW)
	{ si.dwFlags |= STARTF_USESHOWWINDOW;
	  si.wShowWindow = SW_SHOWNORMAL;
	}
							  
	if (!(flags & WL_SPAWN))
	{ if ((flags & WL_NOIHAND) == 0)
		{	si.hStdInput = // infile == NULL ? CreateF(dummyInFile)
										 CreateFile(infile == NULL ? "nul" : infile,
										 						GENERIC_READ,FILE_SHARE_READ,NULL,
																OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		}
		if (outfile != NULL)
		{	si.hStdOutput = CreateFile(outfile,GENERIC_WRITE,FILE_SHARE_WRITE,&sa,
																 CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY,NULL);
			if (si.hStdOutput < 0)
				mbwrite("CFOut Failed");
		}
		if (si.hStdInput > 0 || si.hStdOutput > 0)
			si.dwFlags |= STARTF_USESTDHANDLES;
	}
	else
	{	if (!CreatePipe(&read_stdout,&newstdout,&sa,0))  //create stdout pipe
	  	return -1000 + ErrorMessage("CreatePipe");
	
		if (!CreatePipe(&si.hStdInput,&write_stdin,&sa,0))   //create stdin pipe
			wcc = -2000 + ErrorMessage("CreatePipe");
		else
		{ // mbwrite("Created WSO");

// 		GetStartupInfo(&si);      //set startupinfo for the spawned process
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		  si.wShowWindow = SW_HIDE;
		  si.hStdOutput = newstdout;
		  si.hStdError = newstdout;     //set the new handles for the child process
//	  si.lpTitle = "Emsub";
#if 0
			if (si.hStdOutput != 0)
			{	HANDLE cur_proc = GetCurrentProcess();
									 
				BOOL s = DuplicateHandle(cur_proc,si.hStdOutput,
																 cur_proc,&si.hStdError,0,TRUE,
																 DUPLICATE_SAME_ACCESS) ;
			}
	  // mbwrite("IHAND");
#endif	  
	  }
	  flags |= WL_IHAND;
	}

//mbwrite(app == NULL ? "<no app>" : app);
//mbwrite(ca == NULL ? "<no args>" : ca);
//mbwrite(int_asc((int)si.hStdOutput));

{	DWORD exit = STILL_ACTIVE;  			//process exit code
  DWORD bread = 0, bwrote = 0;  		//bytes read/written
  DWORD avail;  										//bytes available
  int got_ip = 0;
  int clamp = 4;
  int iter = 0;
  int end = 0;
	int sct = 6;
 	int sentz = 0;

	if      (wcc != OK)
		;
  else if (!CreateProcess(app,					//spawn the child process
                    			(char*)ca,
                    			NULL,NULL,
													(flags & WL_IHAND),
                					flags & (WL_CNPG+WL_CNC),
                    			NULL,NULL,&si,&pi))
	{	pi.hProcess = 0;
  	wcc = -3000 + ErrorMessage("CreateProcess");
  }
	else
	{	CloseHandle(pi.hThread);
  
		if (!(flags & WL_SPAWN))
		{
//  int ct = 200000;
			for (;;)
			{ DWORD procStatus = WaitForSingleObject(pi.hProcess, 200);
				if (procStatus == WAIT_TIMEOUT)
				{ // millisleep(100);
//			if (--ct >= 0)
					if (!typahead() /* && (TTbreakFlag != 0)*/) 
						continue;
				}
			{ Cc cc = (procStatus != WAIT_FAILED);
				if (cc)
				{ cc = GetExitCodeProcess(pi.hProcess,&exit);
				  if (cc == 0)
				  	flagerr("GECP %d");
					else
					{//mbwrite("Exitting");
						break;
					}
				}
			}}
			
			//mbwrite("Exit Whole");
//		if (dummyInFile[0] != 0)
//			unlink(dummyInFile);
		}
		else 
		{ char fbuff[512];
			union 
			{ int	 i[64];
				char buf[512];
			} l;
			FILE * ip = infile == NULL ? NULL : fopen(infile, "r");
	  	FILE * op = outfile == NULL ? NULL_OP : fopen(outfile, "w");
			const char * ipstr = in_data == NULL ? "" : in_data;
	  	int append_nl = *ipstr != 0;
	  	int std_delay = 10;
	  	int delay = 0;
			
			if (read_stdout == 0)
			{ mbwrite("Int Err");
				return -1;
			}

		  for(;;++iter)      //main program loop
		  {	(void)millisleep(delay);											//check for data on stdout
		  	delay = std_delay;
		  {	Cc cc = PeekNamedPipe(read_stdout,buff,2,&bread,&avail,NULL);
			  if (!cc)
		  	 	flagerr("PNP %d");
				if (bread == 0)
		  	{ 
	  			if (exit == STILL_ACTIVE)
					{ cc = GetExitCodeProcess(pi.hProcess,&exit); //while process exists
				    if 			(!cc)
				    {	flagerr("GECP %d");
				    	if (--clamp <= 0)
				    		break;
				    }
	  				else if (exit != STILL_ACTIVE)
	  					break;
	  			}
	  		}
	  		else
	  		{ DWORD done = 0;
	  			buff[bread] = 0;
//	  		mbwrite(buff);
	    	  while (done < avail)
	    	  {	cc = ReadFile(read_stdout,buff+done,1023-done,&bread,NULL);
					  if (cc == 0)
	  				{	flagerr("PNP %d");
	    	 			break;
	    	 		}
	    		  done += bread;
	    		  buff[done] = 0;
	    	 		if (bread == 0)
	    	 			break;
						got_ip = 1;
	  		    if (op != NULL)
	  		      fputs(buff, op);
//						printf("In: %s",buff);
	   			}
				}

				if (sentz)
				{ if (--sentz <= 0)
					{ --sentz;
						break;
					}
					continue;
				}

		    if ((flags & WL_AWAIT_PROMPT) && !got_ip && !_kbhit())
		      continue;

				//mbwrite("Getting");

//			if (ipstr == 0)
//				continue;

				if (*ipstr == 0)
		    {	if (ip != 0)
			    { ipstr = fgets(&fbuff[0], sizeof(fbuff)-1-bwrote, ip);
			    	if (ipstr == NULL)
			    	{	fclose(ip);
			    		ip = NULL;
			    		ipstr = "";
							continue;
						}
			    }
				}
			{	int sl = *ipstr;
				if (sl != 0)
		    { char * ln = l.buf-1+bwrote;
		    	--ipstr;
		      while (*++ipstr != 0 && *ipstr != '\r' && *ipstr != '\n')
		        *++ln = *ipstr;

					if (*ipstr == '\r' && ipstr[1] == '\n')
						++ipstr;
		      if      (*ipstr != 0)
		      	*++ln = *ipstr++;
		      else if (ip == 0)
		      {	*++ln = '\n';
		      	//mbwrite("Ends");
		      }
		      sl = (ln - l.buf) + 1;
			    if (sl > 0 && append_nl)
			    {	if (l.buf[sl-1] != '\n')
				    { l.buf[sl++] = '\n';
					   	append_nl = 0;
		  	  	}
					}
		    }
		   	if (!sentz && sl == 0 || l.buf[sl-1] == 'Z' - '@')
			  {	if (!sentz)
			  		l.buf[sl++] = 'Z' -'@';
			  	std_delay = 100;
			  	sentz = 4000 / 100;			// Wait 4 seconds
		   	}
#if 0
		    --sct;
				if (sct >= 0)
			  {	char sch = l.buf[sl];
			    l.buf[sl] = 0;
			  	mlwrite("Sending %d %x %s",sl, l.buf[sl-1], l.buf);
					mbwrite(NULL);
			    l.buf[sl] = sch;
			  }
#endif
		    cc = WriteFile(write_stdin,l.buf+bwrote,sl,&bwrote,NULL); //send to stdin
		    if (cc == 0)
			  	wcc = -4000 + ErrorMessage("WriteFile");
			  else
			  {	if (sl - bwrote > 0)
				  {	l.buf[sl] = 0;
			  		strpcpy(l.buf, l.buf+bwrote, sizeof(l.buf));
			  	}
			  	bwrote = sl - bwrote;
//		  	mlwrite("Sent %d",bread);
//				mbwrite(lastmesg);
				}		  
				l.i[0] = 0;						// Conceal password data
				l.i[1] = 0;
				l.i[2] = 0;
	    	//delay = STD_DELAY;
	    }}}
			if (ip != NULL)
				fclose(ip);
			if (op != NULL_OP)
				fclose(op);
		}
	}
  
//printf("Exitted %d\n", exit);
  
  CloseHandle(pi.hProcess);
  CloseHandle(si.hStdInput);
  CloseHandle(si.hStdOutput);
  CloseHandle(read_stdout);
  CloseHandle(write_stdin);
	setMyConsoleIP();

  *sysRet = sentz < 0 ? -1 : (Cc)exit;
  return wcc != OK ? wcc :
  			 sentz < 0 ? -1  : OK;
}}}




int ttsystem(const char * cmd, const char * data)

{ Cc cc;
  if (data == NULL)
  { cc = system(cmd);
		setMyConsoleIP();
	}
  else
  { char app[140];
    strpcpy(app, cmd, sizeof(app));
  { char * t = app - 1;
    while (*++t != 0 && *t != ' ' && *t != '\t')
      ;
	  *t = 0;
//  if (*(cmd + (t-app)) != ' ')
//   	mbwrite("No space");
  	cc = WinLaunch(&cc, WL_SPAWN+WL_CNC+WL_AWAIT_PROMPT,
  										app, cmd+(t-app),
  										data,null,null);
  }}

	return cc;
}

/*
 * Create a subjob with a copy of the command intrepreter in it. When the
 * command interpreter exits, mark the screen as garbage so that you do a full
 * repaint. Bound to "^X %". The message at the start in VMS puts out a newline.
 * Under some (unknown) condition, you don't get one free when DCL starts up.
 */
int spawncli (int f, int n)
{
  Cc rc;
	return WinLaunch(&rc, WL_SHELL+WL_CNPG+WL_NOIHAND,   // +WL_SHOWW,
										NULL, NULL, NULL, NULL, NULL);
//return WinLaunchProgram(NULL, LAUNCH_SHELL, NULL, NULL, NULL, &rc EXTRA_ARG);
}

	/* Pipe a one line command into a window
	 * Bound to ^X @ or ^X #
	 */
int pipefilter(wh)
	 char 	 wh;
{
 static int bix;
 				char 	 bname [10];
				char	 pipeInFile[NFILEN];
				char	 pipeOutFile[NFILEN];
				char	 app[80];
//			char	 pipeEFile[NFILEN];
				char * fnam1 = NULL;
//			char * fnam3 = NULL;
							
				char line[NSTRING+2*NFILEN+100];			 /* command line send to shell */

	if (restflag) 					/* don't allow this command if restricted */
		return resterr();

{ Cc sysRet;
	Cc cc;
	char prompt[2];
	prompt[0] = wh;
	wh -= '@';											
	if (wh != 0 && wh != '!'-'@' && (curbp->b_flag & MDVIEW)) /* disallow if*/
		return rdonly();															/* we are in read only mode */

	if (wh == 'E'-'@' || wh == 'e'-'@')
		strpcpy(line, lastline[0], sizeof(line)-2*NFILEN);
	else
	{ prompt[1] = 0;
		if (mlreply(prompt, line, NLINE) != TRUE)
			return FALSE;
			
		if (line[0] == '%' || line[0] == '\'')
		{ char sch;
      int ix;
			for (ix = 0; line[++ix] != 0 && isalpha(line[ix]); )
				;

			sch = line[ix];
			line[ix] = 0;

		{ const char * val = gtusr(line+1);
			line[ix] = sch;

			if (val != NULL && strcmp(val,"ERROR") != 0)
			{ 
				strcpy(line,strcat(strcpy(pipeInFile,val),line+ix));
			}
		}}
	}

	if (wh == '@'-'@')
	{ strcat(strcpy(bname,"_cmd"),int_asc(++bix)); /* get the command to pipe in */
													/* get rid of the command output buffer if it exists */
	{ BUFFER * bp = bfind(bname, FALSE, 0);
		if (bp != null) 					/* try to make sure we are off screen */
		{ BUFFER * sbp = curbp;
			cc = orwindmode(0, 1);
			curbp = sbp;
			if (cc > 0)
				onlywind(FALSE, 1);
														 /* get rid of the existing command buffer */
			if (bp == curbp)
				nextbuffer(0,0);
			if (zotbuf(bp) != TRUE)
				return FALSE;
		}
	}}
	else if (wh == '#'-'@') 						 /* setup the proper file names */
	{ 			
		fnam1 = mkTempCommName(pipeInFile,"si");

		if (writeout(fnam1) != TRUE)		/* write it out, checking for errors */
		{ mlwrite(TEXT2);
																			/* "[Cannot write filter file]" */
			return FALSE;
		}
	}

//char * fnam2 = wh == '!' - '@' ? NULL : mkTempCommName(pipeOutFile,"so");
{	char * fnam2 = mkTempCommName(pipeOutFile,"so");
	char * s = line;

	tcapmove(term.t_nrowm1, 0);

	app[0] = 0;

	if  (wh == '#'-'@' || wh == '@'-'@')
	{	int sz = sizeof(app) - 1;
		char * t = app -1;
		for (--s; --sz >= 0 && *++s != 0 && *s != ' '; )
			*++t = *s;
		*++t = 0;
	}

	cc = WL_IHAND + WL_HOLD;

	if      (wh == '#'-'@')
		cc |= WL_SPAWN+WL_CNC;
	else if	(wh <= 0)					// %@
		cc |= WL_SHELL;
#if 0
	else if (wh == 'E' -'@')
		cc |= LAUNCH_STDERROUT;
	else if (wh == 'e' -'@')
	{ wh = 'E';
		cc |= LAUNCH_STDIN;
	}
#endif
	cc = WinLaunch(&sysRet, cc,
								 app[0] == 0 ? NULL : app, s, NULL, fnam1, fnam2);
	if (cc != OK)
	{	if (cc == -1)
			mbwrite("Gave up");
		sysRet = -1;
	}

{/*int fid = open(tmpnam, O_RDONLY);			// did the output file get generated?
	if (fid < 0)
		return FALSE;
	close(fid);
*/
	if (/*sysRet == OK && */ wh == '@'-'@')
	{ BUFFER * bp = bfind(bname, TRUE, 0);
		if (bp == NULL)
			return FALSE;
/*	
		if (splitwind(FALSE, 1) == FALSE)
			return FALSE;
*/
		swbuffer(bp);
	/*linstr(tmpnam); */
								/* make this window in VIEW mode, update all mode lines */
		curwp->w_bufp->b_flag |= MDVIEW;
		upmode();
																	/* and get rid of the temporary file */
	}
	cc = FALSE;
	if (wh == '!'-'@')
	{ FILE * ip = fopen(fnam2, "rb");
		if (ip != NULL)
		{ char * ln;
			while ((ln = fgets(&line[0], NSTRING+NFILEN*2-1, ip))!=NULL)
			{ int len = strlen(ln);
				if (len > 0)
					ln[len-1] = 0;
				puts(ln);
			}
			if (sysRet == OK)
			{ puts("[End]");
				ttgetc();
			/*homes();*/
				sgarbf = TRUE;
			}
			fclose(ip);
			cc = TRUE;
		}
	}
	else													/* on failure, escape gracefully */ 		
	{ char * sfn = curbp->b_fname;
		curbp->b_fname = null;
		cc = readin(fnam2, 0);
		curbp->b_fname = sfn; 							/* restore name */
		curbp->b_flag |= BFCHG; 		/* flag it as changed */
//  if (fnam3 != null)
// 	  cc = readin(fnam3, FILE_INS);
	}
	if (sysRet != 0 || !cc)
		mlwrite(TEXT3); 							/* "[Execution failed]" */
/*else																						
		mbwrite("ExecSucc");*/
																	/* get rid of the temporary files */
	if (fnam1 != NULL)
		unlink(fnam1);							
	unlink(fnam2);
//if (wh == 'E' - '@')
//	unlink(fnam3);
	return cc != FALSE;
}}}}

	/* Pipe a one line command into a window
	 * Bound to ^X @
	 */
Pascal pipecmd(int f, int n)

{ return pipefilter('@');
}

	/*
	 * filter a buffer through an external DOS program
	 * Bound to ^X #
	 */
Pascal filter(int f, int n)

{ return pipefilter('#');
}


/* Run a one-liner in a subjob. When the command returns, wait for 
	* a single character to be typed, then mark the screen as garbage 
	* so a full repaint is done. Bound to "C-X !".
	*/
Pascal spawn(int f, int n)
{
	return pipefilter('!');
}

#if 0
X 			/* return a system dependant string with the current time */
Xchar *Pascal timeset()
X
X{
X#if	MWC | TURBO | MSC
X register char *sp;	/* temp string pointer */
X char buf[16]; 	/* time data buffer */
Xextern char *ctime();
X
X time(buf);
X sp = ctime(buf);
X sp[strlen(sp)-1] = 0;
X return sp;
X#else
X return errorm;
X#endif
}

#endif

// ******************************************************************

// Clipboard Functions

HANDLE	m_hClipData;

//Char * ClipRef()

//{
//	if (m_lpData != NULL)
//		return m_lpData;
//	
//	return m_hData == NULL ? NULL : (m_lpData = (char*)GlobalLock(m_hData));
//}


Cc ClipSet(char * src)

{ Int len = strlen(src);
	HWND mwh = GetTopWindow(NULL);
	if (mwh == NULL)
		return -1;

{	HANDLE m_hData = GlobalAlloc(GMEM_DDESHARE, len + KBLOCK*20 + 10);
	if (!m_hData)  
		return -1;

{	char * m_lpData = (char*)GlobalLock(m_hData);
	if (m_lpData == NULL)
		return -1;
	
	strcpy(&m_lpData[0], src);
	GlobalUnlock(m_hData);

	if (OpenClipboard(mwh))
	{ 								/* Clear the current contents of the clipboard, and set
										 * the data handle to the new string.*/
		EmptyClipboard();
		SetClipboardData(CF_TEXT, m_hData);
		CloseClipboard();
	}
	return OK;
}}}


#if 0

Cc ClipDelete()

{ 
	HWND mwh = GetTopWindow(NULL);
	if (mwh == NULL)
		return -1;

	if (!OpenClipboard(/*m_pMainWnd->*/mwh))
		return -1;

	EmptyClipboard();
	CloseClipboard();
/*
	if (m_hData)
		GlobalFree(m_hData);
*/
	m_hData = 0;
	return OK;
}
#endif



char * ClipPasteStart()

{ HWND mwh = GetTopWindow(NULL);
	if (mwh != NULL &&
			OpenClipboard(mwh))
	{ m_hClipData = GetClipboardData(CF_TEXT);
		if (m_hClipData)
			return (char *)GlobalLock(m_hClipData);
	}

	loglog("PasteFailed");
	return null;
}



void ClipPasteEnd()

{ 
	if (m_hClipData)
	{ GlobalUnlock(m_hClipData);
		m_hClipData = NULL;
	}
	CloseClipboard();
}




void Pascal mbwrite(const char * msg)

{
#if S_WIN32
		char * em_ = "Emacs";
#if VS_CHAR8
		char * m = (char*)(msg == NULL ? lastmesg : msg);
		char * em = em_;
#else
		wchar_t buf[256], ebuf[256];
		LPTSTR m = char_to_wchar((char*)(msg == NULL ? lastmesg : msg), 256, buf);
		LPTSTR em = char_to_wchar(em_, 256, ebuf);
#endif
		HWND mwh = /*GetTopWindow(NULL);*/GetFocus();
		MessageBox(mwh, m, em, MB_OK | MB_SYSTEMMODAL);
		SetActiveWindow(mwh);
#endif
}

#if 0

#define IDR_MYMENU 101
#define IDI_MYICON 201

#define ID_FILE_EXIT 9001
#define ID_STUFF_GO 9002

void Pascal mbmenu(const char * msg)

{ HICON hIcon, hIconSm;

	HMENU hMenu = CreateMenu();

	HMENU hSubMenu = CreatePopupMenu();
	AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, "E&xit");
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");

	hSubMenu = CreatePopupMenu();
	AppendMenu(hSubMenu, MF_STRING, ID_STUFF_GO, "&Go");
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Stuff");

{ HWND hwnd = /*GetTopWindow(NULL);*/GetFocus();
	SetMenu(hwnd, hMenu);


{ hIcon = LoadImage(NULL, "menu_two.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	if(hIcon)
			SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	else
			MessageBox(hwnd, "Could not load large icon!", "Error", MB_OK | MB_ICONERROR);

	hIconSm = LoadImage(NULL, "menu_two.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
	if(hIconSm)
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
	else
			MessageBox(hwnd, "Could not load small icon!", "Error", MB_OK | MB_ICONERROR);
}}}


void Pascal mbwrite2(const char * diag, const char * msg)

{
#if S_WIN32
		int len = strlen(diag) + strlen(msg) + 3;
		char * t = strcat(strcpy(malloc(len),diag),msg);

		mbwrite(t);
		free(t);
}
#endif
}

#endif

#if 0

void Pascal SetParentFocus()

{ 
	if (g_origwin != NULL)
	{ HWND mwh = GetForegroundWindow();
		g_origwin = mwh;
		if (g_origwin != NULL)
			mwh = GetParent(g_origwin);
		if (mwh == NULL)
			flagerr("SPF %d");
		else
			SetActiveWindow(mwh);
	}
}
#endif
