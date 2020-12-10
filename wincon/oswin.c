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

#include	"winpipe.h"

#include  "estruct.h"

#include  "../src/edef.h"
#include	"../src/etype.h"
#define Short short

#include	"../src/elang.h"
#include	"../src/logmsg.h"

#include <process.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#if USE_UBAR
#define stat _stat
#define O_RDONLY _O_RDONLY
#endif

#define MLIX 3

extern char lastline[MLIX+1][NSTRING];

			/* The Mouse driver only works with typeahead defined */
#if	MOUSE
static int mexist;	/* is the mouse driver installed? */
static int nbuttons;	/* number of buttons on the mouse */
static int oldbut;	/* Previous state of mouse buttons */
#endif

int g_chars_since_shift;
int timeout_secs;

static void flagerr(const char * fmt)

{ DWORD ec = GetLastError();
  mlwrite(fmt, ec);
  Beep(1200, 2000);
  ttgetc();
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

BOOL GetFormattedError(LPTSTR dest,int size)
{
	DWORD dwLastError=GetLastError();
	if(!dwLastError)
		return 0;
	BYTE width=0;
	DWORD flags;
	flags  = FORMAT_MESSAGE_MAX_WIDTH_MASK &width;
	flags |= FORMAT_MESSAGE_FROM_SYSTEM;
	flags |= FORMAT_MESSAGE_IGNORE_INSERTS;
	return 0 != FormatMessage(flags,
														NULL,
														dwLastError,
														MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
														dest,
														size,	NULL);
	

	AllocConsole(); 
{	int hCrt = _open_osfhandle((long) GetStdHandle(STD_OUTPUT_HANDLE),_O_TEXT ); 
	FILE *hf = _fdopen( hCrt, &qout;w&qout; ); 
	*stdout = *hf; 
	i = setvbuf( stdout, NULL, _IONBF, 0 );
	return 0;
}}

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


static int  eaten_char = -1;		 /* Re-eaten char */


void Pascal reeat(int c)

{	eaten_char = c;			/* save the char for later */
}

#endif

void flush_typah()

{ 
#if GOTTYPAH
	eaten_char = -1;
#endif
	while (_kbhit())
    (void)ttgetc();
}


#define KBRD 7

extern HANDLE  g_ConsOut;                   /* Handle to the console */
extern CONSOLE_SCREEN_BUFFER_INFO csbiInfo;  /* Orig Console information */
extern CONSOLE_SCREEN_BUFFER_INFO csbiInfoO;  /* Orig Console information */

static HANDLE g_ConsIn;
static HWND   g_origwin = NULL;

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
  rec_.Event.KeyEvent.dwControlKeyState = RIGHT_CTRL_PRESSED;
  rec_.Event.KeyEvent.wVirtualScanCode = 0x20;
  rec_.Event.KeyEvent.wVirtualKeyCode = 'C';

  rc = WriteConsoleInputA( g_ConsIn, &rec_, 1, &ct);
  if (rc == 0 || ct != 1)
  { /*DWORD ec = GetLastError();
    mlwrite("Err %d %d", ec, ct);
    Beep(600, 1000);*/
    g_got_ctrl = true;
  }
  return true;
}

void cls()
               /* get the number of character cells in the current buffer */
{ 
  CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */

  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  BOOL bSuccess = GetConsoleScreenBufferInfo( hConsole, &csbi );
  if (bSuccess)
  { DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD cCharsWritten;
	  COORD coordScreen;  							/* here's where we'll home the cursor */
    coordScreen.Y = term.t_nrowm1+1;
    coordScreen.X = 0;
    bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR) ' ',
                                          dwConSize,coordScreen, &cCharsWritten);
  }
}

#if 0

static void homes()

{	const COORD coordScreen = { 0, 0 };  /* here's where we'll home the cursor */
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	(void)SetConsoleCursorPosition( hConsole, coordScreen ); /* doesnt work */
	Sleep(10);
}

#endif

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

void setMyConsoleIP()

{ g_ConsIn = GetStdHandle( STD_INPUT_HANDLE );
  if (g_ConsIn < 0)					                    /* INVALID_HANDLE_VALUE */
    flagerr("Piperr %d");

/* SetStdHandle( STD_INPUT_HANDLE, g_ConsIn ); */
  if (0 == SetConsoleMode(g_ConsIn, ENABLE_WINDOW_INPUT/*|ENABLE_PROCESSED_INPUT*/))
    flagerr("PipeC %d");
}



void Pascal MySetCoMo()

{ RECT rc; 
  SMALL_RECT sr;
  COORD sz;
  if (term.t_nrowm1 >= csbiInfoO.dwSize.Y)
    term.t_nrowm1 = csbiInfoO.dwSize.Y - 1;
  if (term.t_ncol > csbiInfoO.dwSize.X)
    term.t_ncol = csbiInfoO.dwSize.X;
   
  sr.Left = 0+0;
  sr.Top = 0+3;
  sr.Right = 0+term.t_ncol-1;
  sz.X = term.t_ncol;
  sr.Bottom = 0+term.t_nrowm1-3/* say */;
  sz.Y = term.t_nrowm1+1;

  g_origwin = GetForegroundWindow();
 
  GetWindowRect(g_origwin, &rc); 

  FreeConsole();
  AllocConsole();

  g_ConsOut = GetStdHandle( STD_OUTPUT_HANDLE );

  if (SetConsoleScreenBufferSize(g_ConsOut, sz) == 0)
    flagerr("SCSBerr %d");
  if (SetConsoleWindowInfo(g_ConsOut, true, &sr) == 0)
    flagerr("SCWIEerr %d");

  //SetConsoleTitle("Debug Window");

  setMyConsoleIP();

  SetConsoleCtrlHandler(MyHandlerRoutine, true);

{ HWND mwh = GetForegroundWindow();
  if (mwh == NULL)
    flagerr("MwHerr %d");

{ Cc cc = SetWindowPos(mwh, HWND_TOP, 10,10,
//        	 ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
//	         ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2), 
        	   					 0, 0, SWP_NOSIZE /*| SWP_NOACTIVATE*/); 
  if (cc == 0)
    flagerr("SwPerr %d");
}}}


/*
 * Read a character from the terminal, performing no editing and doing no echo
 * at all. Also mouse events are forced into the input stream here.
 */
Pascal ttgetc()

{
#if GOTTYPAH
	if (eaten_char != -1)
	{ int c = eaten_char;
		eaten_char = -1;
		return c;
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
  if (g_ConsIn == 0)
  {/*FreeConsole();
     AllocConsole();*/
     setMyConsoleIP();
  }

{	int totalwait = timeout_secs;
/*int cRecords = 0;
	PeekConsoleInput(g_hConIn, &ir, 1, &cRecords);*/

  while (1)
  { DWORD actual;
  	const DWORD lim = 1000;
  	INPUT_RECORD rec;
  	int keystate;

    int cc = WaitForSingleObject(g_ConsIn, lim);
    if (cc == WAIT_TIMEOUT)
    { if (g_got_ctrl)
      { g_got_ctrl = false;
        return (int)(CTRL | 'C');
      }
			totalwait -= 1;

			if (totalwait == 0 && timeout_secs > 0)
			{ exit(2);
			}
			_sleep(10);
      continue;
    }

    SetConsoleMode(g_ConsIn, ENABLE_WINDOW_INPUT);
    if (!ReadConsoleInput(g_ConsIn, &rec, (DWORD)1, &actual) || actual < 1)
    { _sleep(10);
	    continue;
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

#define NEW_PIPE 2

#if NEW_PIPE == 2

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


char * mkTempName (/*out*/char *buf, char *name, char *ext)
{
#ifdef _CONVDIR_CHAR
 #define DIRY_CHAR _CONVDIR_CHAR
#else
 #define DIRY_CHAR DIRSEPCHAR
#endif
	static char *tmpDir = NULL ;
				 char c2[2];
				 char * td = tmpDir;

	c2[0] = c2[1] = 0;

	if (td == NULL)
	{	td = (char *)getenv("TEMP");
		if (td != NULL)
		{
			if (td[strlen(td)-1] != DIRY_CHAR)
				c2[0] = DIRY_CHAR;
		}
	  else
#if (defined _DOS) || (defined _WIN32)
						/* the C drive : better than ./ as ./ could be on a CD-Rom etc */
			td = "c:\\" ;
#else
			td = "./" ;
#endif
		}
		tmpDir = td;
		concat(buf,td,c2,"me",int_asc(_getpid()),name,ext,0);

		return &buf[strlen(buf)];
}


static char * mkTempCommName(/*out*/char *filename, char *basename)
{
	char *ss = mkTempName(filename,basename,NULL) - 3;
	int ch;
	
	for (ch = 'A'-1 ; ++ch <= 'Z'  && !fexist(filename); )
	{
		HANDLE hdl = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,
													  OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (hdl >= 0) 			/*INVALID_HANDLE_VALUE*/
		{  CloseHandle(hdl);
			 break ;
		}
		*ss = ch;				// File in use?
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

int platformId = -1;

#define COMMAND_FILE				 "stdout.~~~"
#define DUMMY_STDIN_FILE		 "stdin.~~~" 

#if MEOPT_IPIPES
#define EXTRA_ARG ,NULL
#else
#define EXTRA_ARG
#endif


HANDLE CreateF(char * filenm, DWORD gen, DWORD share, SECURITY_ATTRIBUTES * sb,
							 DWORD cmd, BOOL temp)
{
#if _MSC_VER < 1900
	char * nm = filenm;
#else
	wchar_t buf[512];
  wchar_t * nm = char_to_wchar(filenm, 512, buf);
#endif
	DWORD sh = share != 0 					? share :
						 gen == GENERIC_WRITE ? FILE_SHARE_WRITE :
																		FILE_SHARE_READ;
	DWORD attr = temp ? FILE_ATTRIBUTE_TEMPORARY : FILE_ATTRIBUTE_NORMAL;
	return CreateFile(nm,gen,sh,sb,
											cmd,attr,NULL);
}

HANDLE CreateRetry(char * filenm)

{ int gen = GENERIC_WRITE;
	int share = FILE_SHARE_WRITE;
	int cmd = CREATE_ALWAYS;
	HANDLE Hdl;
	int ct = 2;

#if _MSC_VER < 1900
	char * nm = filenm;
#else
	wchar_t buf[512];
  wchar_t * nm = char_to_wchar(filenm, 512, buf);
#endif

	for (; ;)
	{	Hdl = CreateFile(nm,gen,share,NULL,
											cmd,FILE_ATTRIBUTE_NORMAL,NULL);
		if (--ct == 0)
			break;
		if (Hdl >= 0) 					/*INVALID_HANDLE_VALUE*/
			CloseHandle(Hdl);

		gen = GENERIC_READ;
		share = FILE_SHARE_READ;
		cmd = OPEN_ALWAYS;
	}

	return Hdl;
}


/*
 * WinLaunchProgram
 * Launches an external program using the DOS shell.
 *
 * Returns TRUE if all went well, FALSE if wait cancelled and FAILED if
 * failed to launch.
 *
 * Cmd is the command string to launch.
 *
 * DOSApp is TRUE if the external program is a DOS program to be run
 * under a DOS shell. If DOSApp is FALSE, the program is launched
 * directly as a Windows application. In that case, the InFile parameter
 * is ignored, and the value of the OutFile parameter is used only to
 * determine if the program should be monitored. the text of the string
 * referenced by OutFile is irrelevant.
 *
 * InFile is the name of the file to pipe into stdin (if NULL, nothing
 * is piped in)
 *
 * OutFile is the name of the file where stdout is expected to be
 * redirected. If it is NULL or an empty string, stdout is not redirected
 *
 * If Outfile is NULL, LaunchPrg returns immediately after starting the
 * DOS box.
 *
 * If OutFile is not NULL, the external program is monitored.
 * LaunchPrg returns only when the external program has terminated or
 * the user has cancelled the wait (in which case LaunchPrg returns
 * FALSE).
 *
 * NOTE: Jon 14/05/97:
 *
 * Encountering problems with utilies such as 'grep' locking up the system,
 * this occurs when the command line has been goofed up by the user (me in
 * this case) and forgot to add some files as arguments. grep then takes it's
 * input from 'stdin'. If stdin is NULL (as was the case) then grep hangs
 * since there is no 'stdin', even worse we seem to kill off a windows .dll
 * from which we can never recover and grep never works again.
 *
 * To get round this problem create a empty file as the stdin input file,
 * utilities such as grep then have a source of stdin, which will immediatly
 * terminate safely. The stdin file is deleted once the command has been
 * executed.
 */
#define meBUF_SIZE_MAX 300					/* Buffer for the command line */

int
							/* flags: LAUNCH_SYSTEM, LAUNCH_SHELL */
WinLaunchProgram(char *cmd, int flags, char *inFile, char *outFile, char *outErr,
								 int *sysRet
#if MEOPT_IPIPES
								,meIPipe *ipipe
#endif
								)
{
	PROCESS_INFORMATION mePInfo ;
	STARTUPINFO meSuInfo ;
	char	cmdLine[meBUF_SIZE_MAX+102];			 /* Buffer for the command line */
	char	dummyInFile[NFILEN] ; 						 /* Dummy input file */
	char	pipeOutFile[NFILEN] ; 						 /* Pipe output file */
	int 	status = 1;
#ifndef _WIN32s
	HANDLE inHdl, outHdl, dumHdl ;
#else
	char *endOfComString = NULL;						 /* End of the com string */
	static int pipeStderr = 0;
	if (pipeStderr == 0)
		pipeStderr = getenv("ME_PIPE_STDERR") != NULL ? 1 : -1 ;
#endif
																	/*set the startup window size*/
	memset(&mePInfo, 0, sizeof (PROCESS_INFORMATION));
	memset(&meSuInfo, 0, sizeof (STARTUPINFO));
	meSuInfo.cb = sizeof(STARTUPINFO);

	/*if ((flags & LAUNCH_SHOWWINDOW) == 0)*/  /* always 0 */
	{ 						/*Only a shell needs to be visible, hide the rest*/
		meSuInfo.wShowWindow = SW_HIDE;
		meSuInfo.dwFlags |= STARTF_USESHOWWINDOW ;
	}
{
#if _MSC_VER < 1900
	char * cmd_ = cmd;
#else
	wchar_t buf[1000];
	LPTSTR cmd_ = char_to_wchar(cmd, 1000, buf);
//meSuInfo.pipeOutFile = NULL;
#endif
	meSuInfo.lpTitle = cmd_;
//meSuInfo.hStdInput	= INVALID_HANDLE_VALUE ;
//meSuInfo.hStdOutput = INVALID_HANDLE_VALUE ;
//meSuInfo.hStdError	= INVALID_HANDLE_VALUE ;

	dummyInFile[0] = 0;

	if (platformId < 0)
	{
		OSVERSIONINFO os;
		os.dwOSVersionInfoSize = sizeof(os);
		GetVersionEx(&os);
		platformId = os.dwPlatformId;
		if (platformId == VER_PLATFORM_WIN32_NT)
			platformId = 0;
	}
																						/* Get the comspec */
	/*if ((flags & LAUNCH_NOCOMSPEC) == 0)*/	/* always 0 */
{ char * compSpecName = (char*)getenv("COMSPEC");
	if (compSpecName == NULL)
																					 /* If no COMSPEC setup the default */
		compSpecName = platformId == 0 ? "cmd.exe"
																	 : "command.com";

	if ((flags & LAUNCH_SHELL) == 0)	 /* Create the command line */
	{ register
		char c2 ;
		char * ss;
		char delim;
		char * dd = cmdLine;
		concat(dd,compSpecName," /c ",0);
		compSpecName = dd;

	{ int len = strlen(dd);
		dd += len;
#ifdef _WIN32s
		endOfComString = dd;						/* End of the COM string */
#endif
		if (platformId == 0)
			*dd++ = '"';

		ss = cmd ;
		if (len+strlen(ss) >= meBUF_SIZE_MAX+99)
			return FALSE ;

		delim = *ss;
		if (delim != '"')
			delim = ' ' ;

		while ((c2=*ss++))
		{ if (c2 == delim && ss > cmd+1)
				delim = 0;																		/* always 0 */
			if (c2 == '/' && delim != 0 /* &&!(flags & LAUNCH_LEAVENAMES)*/)
				c2 = '\\';
		
			if (c2 == '"')
				*dd++ = '\\';
			*dd++ = c2;
		}

		*dd = 0;
		dd[1] = 0;

		if (platformId == 0 /*&&!(flags & LAUNCH_NOCOMSPEC)*/)
			*dd = '"';
							/*If its a system call we don't care about input or output*/
#if 0
C 	if (flags & LAUNCH_SYSTEM)
C 	{
#ifndef _WIN32s
C 		if (platformId == VER_PLATFORM_WIN32_WINDOWS)
C 		{
C 					/* For some reason Win98 shell-command start-up path is incorrect
C 					 * unless a dummy input file is used, no idea why but doing the
C 					 * following (taken from above) works! */
C 			error error
C 																						/* Re-open the file for reading */
C 			meSuInfo.hStdInput = CreateRetry(dummyInFile);
C 			if (meSuInfo.hStdInput < 0) 	/* INVALID_HANDLE_VALUE */
C 			{
C 					DeleteFile(dummyInFile) ;
C 					CloseHandle(meSuInfo.hStdOutput) ;
C 					return FALSE;
C 			}
C 			meSuInfo.dwFlags |= STARTF_USESTDHANDLES ;
C 		}
#endif
C 	}
C 	else
#endif
		{ SECURITY_ATTRIBUTES sbuts ;
			sbuts.nLength = sizeof(SECURITY_ATTRIBUTES) ;
			sbuts.lpSecurityDescriptor = NULL ;
			sbuts.bInheritHandle = TRUE ;
/*
			if ((flags & LAUNCH_FILTER) && 0) 	 //this facility not used here
			{ 
#ifdef _WIN32s
				strcat (cmdLine, " <");
				strcat (cmdLine, inFile);
				if (outFile != NULL)
				{ strcat (cmdLine, " >");
					strcat (cmdLine, outFile);
				}
#else
				if((meSuInfo.hStdInput=CreateFile(inFile,GENERIC_READ,FILE_SHARE_READ,&sbuts,
																					OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL)) == INVALID_HANDLE_VALUE)
					return FALSE ;
				if((meSuInfo.hStdOutput=CreateFile(outFile,GENERIC_WRITE,FILE_SHARE_READ,&sbuts,
																					 OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL)) == INVALID_HANDLE_VALUE)
				{ CloseHandle(meSuInfo.hStdInput) ;
					return FALSE ;
				}

				if (outErr == NULL)
				{ if (CreatePipe(&meSuInfo.hStdError,&h,&sbuts,0) != 0)
						CloseHandle(h) ;
				}
				else if((meSuInfo.hStdError=CreateFile(outErr,GENERIC_WRITE,FILE_SHARE_READ,&sbuts,
																							 OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL)) == INVALID_HANDLE_VALUE)
				{ CloseHandle(meSuInfo.hStdError) ;
					return FALSE ;
				}
#endif
			}
			else 
*/
					 if (flags & LAUNCH_IPIPE)						 /*this facility not used here*/
			{
																					/* Its an IPIPE so create the pipes */
				if (CreatePipe(&meSuInfo.hStdInput,&inHdl,&sbuts,0) == 0)
					return FALSE ;
				if (CreatePipe(&outHdl,&meSuInfo.hStdOutput,&sbuts,0) == 0)
				{
					CloseHandle(meSuInfo.hStdInput) ;
					status = 0;
				}
				else
					status = -1;
			}
			else
			{ 				/* Under Windows 95 (and I assume win32s) if there is no
								 * standard input then create an empty file as stdin. This
								 * allows commands such as Grep to not lock up when they
								 * have been mis-typed on the command line with no file.
								 * Otherwise the grep command hangs on the input stream.
								 *
								 * Found that some launched programs return before sub programs
								 * have finished, this leaves the "stdin.~~~" file locked.
								 * This is not a problem in this case because the file will be
								 * empty and we should still be able to open it, so don't fail
								 * if we fail to create the file, only fail if we fail to open it.
								 */
								/* Create a dummy input file to stop the process from locking up.
								 *
								 * Construct the dummy input file */
									 /* if an output file name is given, use it, else create one*/
				dumHdl = 0;

				if (outFile == NULL)
					outFile = mkTempCommName(pipeOutFile,COMMAND_FILE) ;

#ifdef _WIN32s
				strcat (cmdLine, pipeStderr > 0 ? " >& " : " > ");
				strcat (cmdLine, outFile);
#else
				meSuInfo.hStdOutput = CreateF(outFile,GENERIC_WRITE,0,&sbuts,
																			CREATE_ALWAYS,1);
				if			(meSuInfo.hStdOutput < 0)
					status = 0;
#endif
				else if (inFile != NULL)
				{ 
					dumHdl = CreateF(inFile,GENERIC_READ,0,NULL,OPEN_ALWAYS,0);
					/*mbwrite(inFile);*/
				}
				else if ((flags & LAUNCH_STDIN) == 0)
				{	(void)mkTempName(dummyInFile, DUMMY_STDIN_FILE,NULL);
					dumHdl = CreateRetry(dummyInFile);
				}
				
				if			(dumHdl == 0)
					;
				else if (dumHdl < 0)
				{ //DeleteFile(dummyInFile) ;
					CloseHandle(meSuInfo.hStdOutput) ;
					status = 0;
				}
				else 
				{ meSuInfo.hStdInput = dumHdl;
					if (outErr != NULL)
					{ meSuInfo.hStdError = CreateF(outErr,GENERIC_WRITE,0, /*FILE_SHARE_READ*/
																				 &sbuts,
																				 OPEN_ALWAYS,0);
						if (meSuInfo.hStdError < 0)
							status = 0;
					}
					else if (flags & LAUNCH_STDERROUT)
						status = -1;
				}
			}
#ifndef _WIN32s
			meSuInfo.dwFlags |= STARTF_USESTDHANDLES ;
#endif
		}
	}}

	if (status != 0)
	{ if (status < 0)					/* Duplicate stdout=>stderr, dont care if this fails*/
		{	HANDLE cur_proc = GetCurrentProcess();
								 
			BOOL s = DuplicateHandle(cur_proc,meSuInfo.hStdOutput,
															 cur_proc,&meSuInfo.hStdError,0,TRUE,
															 DUPLICATE_SAME_ACCESS) ;
			if (s == 0)
				mbwrite("DuptoSEf");
		}
		cls();

#ifdef _WIN32s
C		char curDir[1024];
C		char cmd [1024];
C		char batname [1024];
C		FILE *fp;
C
C		if (endOfComString == NULL)
C			compSpecName = cmd;
C		else					 /*Get the current directory in the 32-bit world*/
C		{ strcpy(curDir,".:\ncd "
C			_getcwd (curDir+6, sizeof(curDir)-6);
C
C			(void)mkTempName(batname,"", ".bat"); /*Create a BAT file to hold the cmd*/
C
C			if ((fp = fopen (batname, "w")) == NULL)
C				compSpecName = NULL;
C			else
C			{ curDir[0] = curDir[6];
C				strcat(strcat(strcat(curDir,"\n"),endOfComString),"\n");
C				fputs(curDir,fp);
C				fclose (fp);
C												 /* Append the command to run the exec file to 
C														the end of the command string */
C				strcpy (endOfComString, batname);
C			}
C		}
C		status = compSpecName == NULL ? 0 :
C														  SynchSpawn (compSpecName,/*SW_HIDE*/SW_SHOWNORMAL);
C														/* Correct the status from the call */
C		if (status != 1)
C			status = 0;
C			
C		error error
#else 																												/* ! _WIN32s */
	/*mbwrite(cp);*/
		loglog1("CrePre %s", compSpecName);
	{
#if _MSC_VER < 1900
		char * app = NULL;
		char * csn = compSpecName;
#else
		wchar_t abuf[256], cbuf[256];
		LPTSTR app = NULL;
		LPTSTR csn = char_to_wchar(compSpecName, 256, abuf);
#endif
													 /* start the process and get a handle on it */
		status = CreateProcess(app,
													 csn,
													 NULL,NULL,
													 !(flags & LAUNCH_SHELL),
											 /*(flags & LAUNCH_DETACHED) ? DETACHED_PROCESS : CREATE_NEW_CONSOLE,*/
													 CREATE_NEW_CONSOLE,
													 NULL,
													 NULL,
													 &meSuInfo,
													 &mePInfo);
		if (status)
		{
			status = TRUE ;
#if 0
											/* this causes problems for win95 ipipes on network drives*/
			WaitForSingleObject(GetCurrentProcess(), 50);
			WaitForInputIdle(mePInfo.hProcess, 5000);
#endif
			CloseHandle(mePInfo.hThread);

									/* Ipipes need the process handle and we dont wait for it */
			if ((flags & LAUNCH_IPIPE) == 0)
			{
									 /* For shells close the process handle but dont wait for it */
				if ((flags & LAUNCH_SHELL) == 0)
				{
															 /* Wait for filter, system, pipe process to end */
					loglog1("Wait For %d", mePInfo.hProcess);
					for (;;)
					{ DWORD procStatus = WaitForSingleObject(mePInfo.hProcess, 200);
						if (procStatus == WAIT_TIMEOUT)
						{
							if (!typahead() /* && (TTbreakFlag != 0)*/) 
								continue;
						}
						status = (procStatus != WAIT_FAILED);
													/* If we're interested in the result, get it */
						if (sysRet != NULL)
							GetExitCodeProcess(mePInfo.hProcess,sysRet) ;
						break;
					}
				}
										/* Close the process */
				CloseHandle (mePInfo.hProcess);
			}
		}
									/* Close the file handles */
		if (meSuInfo.hStdInput > 0)
			CloseHandle(meSuInfo.hStdInput);
		if (meSuInfo.hStdOutput > 0)
			CloseHandle(meSuInfo.hStdOutput);
		if (meSuInfo.hStdError > 0)
			CloseHandle(meSuInfo.hStdError);
#endif /* WIN32s */

#if MEOPT_IPIPES
		if (flags & LAUNCH_IPIPE)
		{
			if (!status)
			{
				CloseHandle(inHdl);
				CloseHandle(outHdl);
			}
			else
			{ ipipe->pid = 1 ;
				ipipe->rfd = outHdl ;
				ipipe->outWfd = inHdl ;
				ipipe->process = mePInfo.hProcess ;
				ipipe->processId = mePInfo.dwProcessId ;
						
						/* attempt to create a new thread to wait for activity,
						 * this is because windows pipes are crap and doing a Wait on
						 * them fails. so us poor programmers have to jump through lots
						 * of hoops just to make Bils crap usable, and what really hurts
						 * is that after doing so every non-programmer thinks Bills stuff
						 * is wonderful! Sometimes the world sucks.
						 * Set the childActive event to manual so we can do the global
						 * MsgWait and then after its Set do a SingleWait on each reset
						 * those that are set.
						 */
				ipipe->threadContinue = NULL ;
				ipipe->thread = NULL ;
				if(!(ipipe->childActive=CreateEvent(NULL, TRUE, FALSE, NULL))||
					 !(ipipe->threadContinue=CreateEvent(NULL, FALSE, FALSE, NULL)))
					;
				else
#ifndef USE_BEGINTHREAD
					ipipe->thread = CreateThread(NULL,0,childActiveThread,ipipe,0,&ipipe->threadId) ;
#else 							 
				{ unsigned long thread=_beginthread(childActiveThread,0,ipipe);
					if (thread != -1)
						ipipe->thread = (HANDLE) thread ;
				}
#endif
			}
		}
#endif
	}}
	if (dummyInFile[0] != 0)													
		DeleteFile(dummyInFile);	/* Delete the dummy stdin file if there is one. */

	return status ;
}}}


#ifdef _DOS
#define AM_DOS 1
#else
#define AM_DOS 0
#endif


/*
 * Create a subjob with a copy of the command intrepreter in it. When the
 * command interpreter exits, mark the screen as garbage so that you do a full
 * repaint. Bound to "^X %". The message at the start in VMS puts out a newline.
 * Under some (unknown) condition, you don't get one free when DCL starts up.
 */
int spawncli (int f, int n)
{
	int rc;
	return WinLaunchProgram(NULL, LAUNCH_SHELL, NULL, NULL, NULL, &rc EXTRA_ARG);
}
#endif

#if NEW_PIPE == 0

static Cc Pascal usehost(wh, line)
	char	 wh;
	char *	 line;
{
	int cc;
	if (!g_clexec)
	{
		tcapmove(term.t_nrowm1, 0); 					/* Seek to last line. 	*/
 /* mlwrite(""); */
 /* tcapclose(); */
 /* ttputc('\n'); */
#if S_WIN32
		tcapeeol();
		ttputc('\n');
#endif
	}
	/* write(0, "\n", 1);*/
	sgarbf = TRUE;
	cc = system(line);	/* wrapper to execprg() */
#if MOUSE
	ttopen();
#endif
	tcapopen();
	if (0 == SetConsoleMode(g_ConsIn, ENABLE_WINDOW_INPUT/*|ENABLE_PROCESSED_INPUT*/))
		Beep(1400, 200);
	
	ttcol = 0;				 /* otherwise we shall not see the message */
				 /* if we are interactive, pause here */
	if (!g_clexec)
	{ int ch;
		mlwrite(cc == 0 ? TEXT188 : "[Failed]");
/*						 "[End]" */
		while ((ch = ttgetc()) != '\r' && ch != ' ')
			;
		mlerase();
	}

	return cc;
}

#endif


	/* Pipe a one line command into a window
	 * Bound to ^X @ or ^X #
	 */
int pipefilter(wh)
	 char 	 wh;
{
				char prompt[2];
 static int bix;
 				char 	 bname [10];
				char	 pipeInFile[NFILEN];
				char	 pipeOutFile[NFILEN];
//			char	 pipeEFile[NFILEN];
				char * filnam1 = NULL;
				char * filnam3 = NULL;
							
				char line[NSTRING+2*NFILEN+100];			 /* command line send to shell */

	if (restflag) 					/* don't allow this command if restricted */
		return resterr();

{ int sysRet;
	int s;
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
			for (s = 0; line[++s] != 0 && isalpha(line[s]); )
				;

			sch = line[s];
			line[s] = 0;

		{ const char * val = gtusr(line+1);
			line[s] = sch;

			if (val != NULL && strcmp(val,"ERROR") != 0)
			{ 
				strcat(strcpy(pipeInFile,val),line+s);
				strcpy(line,pipeInFile);
#if 0
				mbwrite(line);
#endif
			}
		}}
	}

	if (wh == 0) 						/* '@' */
	{ strcat(strcpy(bname,"_cmd"),int_asc(++bix)); /* get the command to pipe in */
													/* get rid of the command output buffer if it exists */
	{ BUFFER * bp = bfind(bname, FALSE, 0);
		if (bp != null) 					/* try to make sure we are off screen */
		{ BUFFER * sbp = curbp;
			s = orwindmode(0, 1);
			curbp = sbp;
			if (s > 0)
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
		filnam1 = mkTempCommName(pipeInFile,"si");

		if (writeout(filnam1) != TRUE)		/* write it out, checking for errors */
		{ mlwrite(TEXT2);
																			/* "[Cannot write filter file]" */
			return FALSE;
		}
#if 0
		strcat(&line[0], "<");
		strcat(&line[0], filnam1);
#endif
	}
{ char * filnam2 = mkTempCommName(pipeOutFile,"so") ;
	loglog1("Launch Out %s", filnam2);
#if 0
	strcat(&line[0], ">");
	strcat(&line[0], filnam2);			/* WinLaunchProgram cannot cope */

	if (wh == 'E' - '@')
	{ 
		filnam3 = mkTempCommName(pipeEFile,"se") ;
#if 0
		strcat(&line[0], " 2>");
		strcat(&line[0], filnam3);		/* WinLaunchProgram cannot cope */
#endif
	}
#endif
/*mbwrite(line);*/
	tcapmove(term.t_nrowm1, 0);

	s = 0;
	if			(wh == 'E' -'@')
		s |= LAUNCH_STDERROUT;
	else if (wh == 'e' -'@')
	{ wh = 'E';
		s |= LAUNCH_STDIN;
	}  
	s = WinLaunchProgram(line, s, filnam1, filnam2, filnam3,&sysRet EXTRA_ARG);

	loglog1("Unlaunched %d", s);
	if (!s)
		sysRet = -1;
																 /* did the output file get generated? */
{/*int fid = open(tmpnam, O_RDONLY);
	if (fid < 0)
		return FALSE;
	close(fid);
*/
	if (/*sysRet == OK && */ wh == 0) 			/* '@' */
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
	s = FALSE;
	if (wh == '!'-'@')
	{ FILE * ip = fopen(filnam2, "rb");
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
			s = TRUE;
		}
	}
	else													/* on failure, escape gracefully */ 		
	{ char * sfn = curbp->b_fname;
		curbp->b_fname = null;
		s = readin(filnam2, FALSE);
		curbp->b_fname = sfn; 							/* restore name */
		curbp->b_flag |= BFCHG; 		/* flag it as changed */
//	if (wh == 'E' - '@')
//		s = readin(filnam3, -1);
	}
	if (sysRet != 0 || !s)
		mlwrite(TEXT3); 							/* "[Execution failed]" */
/*else																						
		mbwrite("ExecSucc");*/
																	/* get rid of the temporary files */
	if (filnam1 != NULL)
		unlink(filnam1);							
	unlink(filnam2);
//if (wh == 'E' - '@')
//	unlink(filnam3);
	return s != FALSE;
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
/* Run an external program with arguments. When it returns, wait 
 * for a single character to be typed, then mark the screen as 
 * garbage so a full repaint is done. Bound to "C-X $".
 */
Pascal execprg(int f, int n)
				
{ return spawn(f,n);
}

#if 0
X
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




void Pascal mbwrite(char const * msg)

{
#if S_WIN32
		char * em_ = "Emacs";
#if _MSC_VER < 1900
		char * m = (char*)msg;
		char * em = em_;
#else
		wchar_t buf[256], ebuf[256];
		LPTSTR m = char_to_wchar(msg, 256, buf);
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
		char * t = malloc(len);

		strcat(strcpy(t,diag),msg);

		mbwrite(t);

		free(t);
}
#endif
}

#endif

void Pascal SetParentFocus()

{ 
	if (g_origwin != NULL)
	{ HWND mwh = GetForegroundWindow();
		if (mwh != NULL)
			mwh = GetParent(mwh);
		if (mwh == NULL)
			Beep(1500, 200);
		else
			SetActiveWindow(mwh);
	}
/*else
		Beep(1500, 200);*/
}
