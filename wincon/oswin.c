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
//#define Short short

#include	"../src/elang.h"
#include	"../src/logmsg.h"

#include <fcntl.h>
#include <sys/types.h>

extern char *getenv();

#define P_AWAIT_PROMPT 1

			/* The Mouse driver only works with typeahead defined */
#if	MOUSE
static int mexist;	/* is the mouse driver installed? */
static int nbuttons;	/* number of buttons on the mouse */
static int oldbut;	/* Previous state of mouse buttons */
#endif

#define millisleep(n) Sleep(n)


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

*/
#endif

	/* This function is called once to set up the terminal device streams.
	 */
#if	MOUSE

void ttopen()

{
/*if (pd_gflags & MD_NO_MMI)
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
	rg.x.ax = 10;			/* set text cursor */
	rg.x.bx = 0;			/* software text cursor please */
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

static
BOOL WINAPI MyHandlerRoutine(DWORD dwCtrlType)

{ INPUT_RECORD rec_;
  rec_.EventType = KEY_EVENT;
  rec_.Event.KeyEvent.bKeyDown = true;
  rec_.Event.KeyEvent.wRepeatCount = 1;
  rec_.Event.KeyEvent.uChar.AsciiChar = 'C';
  rec_.Event.KeyEvent.wVirtualKeyCode = 'C';
  rec_.Event.KeyEvent.dwControlKeyState = RIGHT_CTRL_PRESSED;
  rec_.Event.KeyEvent.wVirtualScanCode = 0x20;

{ DWORD ct;
  int rc = WriteConsoleInputA(GetStdHandle( STD_INPUT_HANDLE ), &rec_, 1, &ct);
#if _DEBUG
  if (rc == 0 || ct != 1)
  { g_got_ctrl = true;
    flagerr("Err");
  }
#endif
  return true;
}}

#endif


int flagerr(const char *str)  //display detailed error info

{	DWORD ec = GetLastError();
#if _DEBUG
	LPVOID msg;
  FormatMessage(
              FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
              NULL,
              ec,
              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
              (LPTSTR) &msg,
              0,
              NULL
               );
  mlwrite("%p%s(%d): %s\n",str,ec,msg);
  LocalFree(msg);
#else
  mlwrite("%p%s %d", str, ec);
#endif
	return -(int)ec;
}


static HANDLE  g_ConsOut;                   /* Handle to the console */

CONSOLE_SCREEN_BUFFER_INFO g_csbi;   /* Console information */
//CONSOLE_CURSOR_INFO        ccInfo;


#define BG_GREY  (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)
#define FG_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

#if 0

long unsigned int thread_id(void)

{ return (int)GetCurrentProcess(); // + 29 * GetCurrentThreadId();
}

#endif

static UINT g_codepage;

void USE_FAST_CALL setcp(int v)

{ SetConsoleCP(v);
  SetConsoleOutputCP(v);
}


static void USE_FAST_CALL FCOC(int sz)

{	DWORD     Dummy;
	if (sz == 0)
		sz = g_csbi.dwMaximumWindowSize.X * g_csbi.dwMaximumWindowSize.Y;

  FillConsoleOutputCharacter(g_ConsOut, ' ',sz,g_coords,&Dummy );
}


void SetBufferWindow(HANDLE h, int wid, int dpth)

{	SMALL_RECT rect = { 0, 0, wid-1, dpth-1 };
	COORD size;
  size.X = wid-1;
  size.Y = dpth;

{	int rc = SetConsoleScreenBufferSize( h, size );// size);
#if _DEBUG
	if (rc == 0)
		flagerr("SCSBS");

	if (h != GetStdHandle( STD_OUTPUT_HANDLE ))
    mbwrite("Bad Hdl_");

//mlwrite("%p Doing %d", dpth);
	rc = GetConsoleScreenBufferInfo( h, &g_csbi); // Do we need this?
  if (rc == 0)
    flagerr("GCSB %d");
//if (g_csbi.dwSize.X != wid || g_csbi.dwSize.Y != dpth)
//	mlwrite("%p %d %d %d %d", g_csbi.srWindow.Left, g_csbi.srWindow.Top, g_csbi.srWindow.Right, g_csbi.srWindow.Bottom);
#endif

	if (rect.Bottom > g_csbi.srWindow.Bottom)
		rect.Bottom = g_csbi.srWindow.Bottom;

//mlwrite("%pH %d", rect.Bottom);
//mbwrite(int_asc(dpth-1));
																	// set the screen buffer to be big enough
  rc = SetConsoleWindowInfo(h, 1, &rect);
#if _DEBUG
  if (rc == 0)
    flagerr("2 Big");
#endif
	SetFocus(h);
}}

void init_wincon()

{	DWORD     Dummy;
//Sleep(1000*8);
																							// reduces memory but slows startup
	SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);

//Sleep(1000*8);
											      /* Get display screen information, clear the screen.*/
{	HANDLE h = GetStdHandle( STD_OUTPUT_HANDLE );
	g_ConsOut = h;

#if 0
  if (SetConsoleMode(h, ENABLE_PROCESSED_OUTPUT) == 0)
    flagerr("SCMO %d");
#endif

	GetConsoleScreenBufferInfo( h, &g_csbi );

	SetConsoleTextAttribute(h, BG_GREY);
		 
//tcapepage();
//SetConsoleTextAttribute(h, BG_GREY);
	g_codepage = GetConsoleOutputCP();
  setcp(1252);
{ int plen = g_csbi.srWindow.Bottom-g_csbi.srWindow.Top+1;
  int pwid = g_csbi.srWindow.Right -g_csbi.srWindow.Left+2;	/* why 2 ? */
	SetBufferWindow(h, pwid, plen);

  g_coords.X = 0;
  g_coords.Y = 0;
	tcapmove(0,0);
	FCOC(0);
}}}


void Pascal tcapeeol()

{ FCOC(g_csbi.dwSize.X-ttcol);
}

/*
void Pascal tcapepage()

{ tcapbeeol(-1,0);
}
*/


#if 1

/* This function gets called just before we go back home to the command
 * interpreter.
 */
void Pascal tcapclose(int lvl)

{ setcp(g_codepage);
/*CloseHandle(hConsoleIn);
  hConsoleIn = NULL;  cannot do this */
}

#endif

/*
char * argv__[] = { "wincon", "tt", null};
int argc__ = 2;
*/
//#if _MSC_VER < 1900
#undef VS_CHAR8
#define VS_CHAR8 1
//#endif 

#if VS_CHAR8
#define SC_CHAR char
#define SC_WORD WORD

#else
#define SC_CHAR wchar_t
#define SC_WORD DWORD
																					// Overwrites the source
char * wchar_to_char(SC_CHAR * src)

{ char * t = (char*)src-1;
	SC_CHAR * s;
	for (s = src-1; *++s != 0; )
		*++t = *s;
	*++t = 0;

	return (char*)src;
}

wchar_t * char_to_wchar(char const * src, int sz, wchar_t * tgt)

{ wchar_t * t = tgt -1;
	char const * s;
	for (s = src-1; *++s != 0 && --sz > 0; )
		*++t = *s;
	*++t = 0;

	return tgt;
}

#endif


void Pascal setconsoletitle(char * title)

{
#if VS_CHAR8 == 0
	wchar_t buf[100];
	swprintf(buf, 100, L"%S", title == null ? "" : title);
  SetConsoleTitle(buf);
#else
  SetConsoleTitle(title);
#endif
}


// char * Pascal getconsoletitle()

// { return consoletitle;
// }


void Pascal tcapsetsize(int wid, int dpth)

{
#if 0
	int decw = g_csbi.dwSize.X - wid;
	int decd = g_csbi.dwSize.Y - dpth;
	if (decd < 0)
		decd = 0;
	if (decw < 0)
		decw = 0;
	if (clamp*(decw + decd) > 0)			// Must be done twice
	{ decw = g_csbi.dwSize.X - decw - 1;
		if (decw < 0)
			decw = -decw;
		tcapsetsize(decw, g_csbi.dwSize.Y - decd - 1, 0);
	}
#endif
{
#if 1
  HANDLE h = g_ConsOut;
	SetBufferWindow(h, wid, dpth);
#else
 	SMALL_RECT rect = { 0, 0, wid-1, dpth }; // was dpth
	COORD size;
  size.X = wid-1;
  size.Y = dpth;

{ int rc;	
  HANDLE h = g_ConsOut;
#if _DEBUG
  if (h != GetStdHandle( STD_OUTPUT_HANDLE ))
    mbwrite("Bad Hdl");
#endif

#if _DEBUG || 1
  rc = SetConsoleScreenBufferSize( h, size );// size);
	if (rc == 0)
		flagerr("SCSBS");
  if (h != GetStdHandle( STD_OUTPUT_HANDLE ))
    mbwrite("Bad Hdl_");
#endif
// GetConsoleMode(h, &mode);
// mode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
// HANDLE consin = GetStdHandle(STD_INPUT_HANDLE);
// SetConsoleMode(g_ConsIn, ENABLE_WINDOW_INPUT);

#if _DEBUG
//mlwrite("%p Doing %d", dpth);
	rc = GetConsoleScreenBufferInfo( h, &g_csbi);
  if (rc == 0)
    flagerr("GCSB %d");
//if (g_csbi.dwSize.X != wid || g_csbi.dwSize.Y != dpth)
//	mlwrite("%p %d %d %d %d", g_csbi.srWindow.Left, g_csbi.srWindow.Top, g_csbi.srWindow.Right, g_csbi.srWindow.Bottom);
#endif
	if (rect.Bottom > g_csbi.srWindow.Bottom)
		rect.Bottom = g_csbi.srWindow.Bottom;

//mlwrite("%pH %d", rect.Bottom);
																	// set the screen buffer to be big enough
  rc = SetConsoleWindowInfo(h, 1, &rect);
#if _DEBUG
  if (rc == 0)
    flagerr("2 Big");
#endif
// SetFocus(h);								// Sometimes focus is lost?
}
#endif
}}

#if 0
//		 int   g_cursor_on = 0;
static COORD g_oldcur;
static WORD  g_oldattr = BG_GREY;
#endif

/*
Bool Pascal cursor_on_off(Bool on)

{ Bool res = g_cursor_on;
  g_cursor_on = on;
  return res;
}*/


void Pascal USE_FAST_CALL tcapmove(int row, int col)

{ COORD  coords;
	coords.X = col;

	if (row > term.t_nrowm1)
  { // tcapbeep();
    row = term.t_nrowm1;
  }

	coords.Y = row;
	
{ HANDLE h = g_ConsOut;
#if 0
	if (row < term.t_nrowm1 && 0/* && g_cursor_on >= 0 */)
  {	DWORD  Dummy;
		WriteConsoleOutputAttribute( h, &g_oldattr, 1, g_oldcur, &Dummy );
	  g_oldattr = refresh_colour(row, col);
	  g_oldcur = coords;
	{ WORD MyAttr = // row == term.t_nrowm1 ? BG_GREY :
                  BACKGROUND_INTENSITY;
                  // COMMON_LVB_REVERSE_VIDEO;

	  WriteConsoleOutputAttribute( h, &MyAttr, 1, coords, &Dummy );	
	}}
#endif
	g_coords = coords;
	SetConsoleCursorPosition( h, coords);
}}



#if 0

void Pascal tcapscreg(row1, row2)
     int row1, row2;
{ 
}


static
void Pascal ttscup(int maxx, int maxy)/* direction the window moves*/

{ CHAR_INFO ci;
	SMALL_RECT rect;
//COORD doo;
//doo.Y = 0; // -1 ???;
//doo.X = 0;
  rect.Top = 0;
  rect.Left = 0;
  rect.Right = maxx;
  rect.Bottom = maxy + 1;
  
  ci.Char.AsciiChar = ' ';
  ci.Attributes = BG_GREY;
    
{ int cc = ScrollConsoleScreenBufferA( g_ConsOut, &rect, null, *(COORD*)&rect, &ci);
#if _DEBUG
	if (cc == 0)
    tcapbeep();
#endif
}}

#endif

//------------------------------------------------------------------------------

void Pascal ttputc(unsigned char ch) /* put character at the current position in
														   		      current colors */
{	HANDLE cout = g_ConsOut;

/*GetConsoleScreenBufferInfo( cout, &ccInfo );*/
	GetConsoleScreenBufferInfo( cout, &g_csbi );

{ COORD curpos = g_csbi.dwCursorPosition;
  unsigned long  Dum;
#if VS_CHAR8
#define gch (char)ch
#else
	wchar_t gch = ch;
#endif
																		/* write char to screen with current attrs */
  WriteConsoleOutputCharacter(cout, &gch,1, curpos, &Dum);

/* ttcol = col;*/

  if (ch == '\n' || ch == '\r')
  {	curpos.X = 0;
  	if (curpos.Y < g_csbi.dwSize.Y)
    { ++curpos.Y;
//   	ttscup(g_csbi.dwSize.X, g_csbi.dwSize.Y);	/* direction the window moves*/
    }
  }
  else
  { // if (ch != '\b')
		++curpos.X;
    { 
#if 0
      if (col >= g_csbi.dwSize.X)
    	{
#if _DEBUG
        mlwrite("%pRow %d Col %d Lim %d", ttrow, col, g_csbi.dwSize.X);
#endif
				return;
      }
#endif
    }
#if 0
    else
	  {
#if _DEBUG
			mbwrite("Got BS");
#endif
    	col -= 2;
      if (col < 0)
        col = 0;
    }
#endif
  }

	g_csbi.dwCursorPosition = curpos;
//ttrow = row;
  SetConsoleCursorPosition( cout, curpos);
}}



int Pascal tcapbeep()

{ Beep( 500, 250 /*millisecs*/);
//mbwrite("BEEP\n");
  return OK;
}



void Pascal scwrite(row, outstr, color)	/* write a line out */
	int 	   row; 			/* row of screen */
	short	  *outstr;		/* string to output (must be term.t_ncol long)*/
	int 	   color;		 	/* background, foreground */
{ 									/* build the attribute byte and setup the screen pointer */
	SC_CHAR buf[NCOL];
	WORD 		cuf[NCOL];
	unsigned long n_out;
	WORD attr = color;
	const SC_WORD sclen = g_csbi.dwSize.X >= NCOL ? NCOL : g_csbi.dwSize.X;
	int col;
	
	for (col = -1; ++col < sclen; )
	{
/* 0 : No special effect
 * 1 : underline
 * 2 : bold
 * 4 : Use new foreground
 * 8 : Use line foreground
 * An attribute of 0 therefore means the attributes for the line/new foreground
 */
    int wh = outstr[col] & 0xf000;
    if (wh != 0)
    { wh = wh >> 12;
      if      (wh & 4)
      {	attr = color & 0xf0 | (outstr[col] >> 8) & 0xf;
      	if (wh & 8)
					attr |= COMMON_LVB_UNDERSCORE;
      }
      else if (wh & 8)
        attr = color;

      if (wh & 1)						// Cannot do underline so bold (which is faint!)
				attr |= 0x8;
    }
		cuf[col] = attr;
		buf[col] = (outstr[col] & 0xff);
	}
	
{	COORD coords;
	coords.X = 0;
	coords.Y = row;
			 
	WriteConsoleOutputCharacter( g_ConsOut, buf, sclen, coords, &n_out );
{ int cc = WriteConsoleOutputAttribute( g_ConsOut, cuf, sclen, coords, &n_out);
#ifdef _DEBUG
	if (cc == 0 || n_out != sclen)
		adb(cc);
#endif
}}}


#if	FLABEL
int Pascal fnclabel(f, n)	/* label a function key */
	int f,n;	/* default flag, numeric argument [unused] */
{		/* on machines with no function keys...don't bother */
  return TRUE;
}
#endif


//static HANDLE g_ConsIn;
//static HWND g_origwin = NULL;


#if _DEBUG
static int g_got_ctrl = false;
#endif

#if 0

int Pascal iskboard()

{	BY_HANDLE_FILE_INFORMATION fileinfo;
	HANDLE h GetStdHandle( STD_INPUT_HANDLE );
{	DWORD rc = GetFileInformationByHandle(h, &fileinfo);
	return !rc;
}}

#endif

HANDLE setMyConsoleIP()

{// int clamp = 2;

//if (GetConsoleMode(g_ConsIn, &mode))
//{ mlwrite("%pMode before %x", mode);
//}

	HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
	//while (--clamp >= 0)
	//{
	
	Cc rc = SetConsoleMode(h,ENABLE_WINDOW_INPUT);// Allowed to fail
#if _DEBUG
	if (!rc)
	  flagerr("Ewi");
#endif
//	millisleep(50);
	//}
	return h;
}


static HANDLE USE_FAST_CALL Create(int wr, const char * fname)

{	DWORD share = GENERIC_READ | (GENERIC_WRITE*wr);
	SECURITY_ATTRIBUTES sa;
  sa.lpSecurityDescriptor = NULL;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;         //allow inheritable handles
  
{	int fattr = FILE_ATTRIBUTE_NORMAL|FILE_SHARE_READ|FILE_SHARE_WRITE |
							FILE_FLAG_BACKUP_SEMANTICS;

	HANDLE res = CreateFile(fname == NULL ? "nul" : fname,
										      share, 1, &sa,
											    OPEN_EXISTING,fattr,NULL);
//if (res < 0)
//	res =  (int)CreateFile(fname == NULL ? "nul" : fname,
//											   share, rw+1, &sa,
//												 open_create,fattr|FILE_FLAG_BACKUP_SEMANTICS,NULL);
										
//mlwrite("%pCreate %d %s -> %x", rw, fname, res);
	return res;
}}

void Pascal MySetCoMo()

{
	HANDLE h = Create(1, "CONIN$");
	(void)SetStdHandle(STD_INPUT_HANDLE, h);

	(void)setMyConsoleIP();

//SetConsoleCtrlHandler(MyHandlerRoutine, true);
#if 0
{ HWNcd \gener\omicroD mwh = GetForegroundWindow();
  if (mwh == NULL)
    flagerr("MwHerr");

{ Cc cc = SetWindowPos(mwh, HWND_TOP, 10,10,
//        	 ((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2),
//	         ((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2), 
        	   					 0, 0, SWP_NOSIZE /*| SWP_NOACTIVATE*/); 
  if (cc == 0)
    flagerr("SwPerr");
}}
#endif
//g_origwin = GetForegroundWindow();
}


int Pascal tcapopen()

{ int plen = g_csbi.srWindow.Bottom-g_csbi.srWindow.Top+1;
  int pwid = g_csbi.srWindow.Right -g_csbi.srWindow.Left+2;	/* why 2 ? */

  newdims(pwid, plen);

{ HANDLE h = setMyConsoleIP();
//SetFocus(h);								// Sometimes focus is lost

{	BY_HANDLE_FILE_INFORMATION fileinfo;
	DWORD rc = GetFileInformationByHandle(h, &fileinfo);
	return !rc;
}}}


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

//			 /* typahead: See if any characters are already in the keyboard buffer */
//int Pascal l typahead()
//
//{	return _kbhit();
//}


int g_chars_since_ctrl;
int g_timeout_secs;

static const unsigned char scantokey[] =
{	
#define SCANK_STT 0x3b
  '1',		/* 3b */
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
	0x1c,		/* CTRL-\ */
	':',
	';',		/* 58 */
};

#define EAT_LOG 7
#define EAT_SZ (1 << EAT_LOG)

typedef union
{ int pair;
  struct
  { short ct;
  	short ix;
  } both;
} Pair;

//Pair g_eaten_pair;
int g_eaten_pair;

static int g_eaten[EAT_SZ+4];	 /* four buffer entries */


void Pascal reeat(int c)

{																				/* save the char for later */
	g_eaten_pair += (1 << 8);
	g_eaten[(g_eaten_pair & 255) + (g_eaten_pair >> 8)] = c;
}


void flush_typah()

{ g_eaten_pair = 0;

	while (_kbhit())
    (void)ttgetc();
}


/*
 * Read a character from the terminal, performing no editing and doing no echo
 * at all. Also mouse events are forced into the input stream here.
 */
int ttgetc()

{ if ((g_eaten_pair >> 8) > 0)
	{ g_eaten_pair += 1 - 256;
		return g_eaten[g_eaten_pair & (EAT_SZ-1)];
	}

#if MOUSE > 0
	while (TRUE)
	{												/* with no mouse, this is a simple get char routine */
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
#endif
{ HANDLE h = GetStdHandle( STD_INPUT_HANDLE );
	int totalwait = g_timeout_secs;
 	DWORD lim = 1000;
	int oix = -1;
  static FILE * diags;

  while (1)
  { int need;
  	INPUT_RECORD rec[EAT_SZ];
  	int got = 0;
    int cc = WaitForSingleObject(h, lim);
    switch(cc)
		{	case WAIT_OBJECT_0:
							need = (EAT_SZ - 1 - oix);
							cc = ReadConsoleInput(h,&rec[0],need,(DWORD*)&got);
							if (cc && got > 0)
								break;	
#if _DEBUG
						  {	DWORD errn = GetLastError();
						   	if (errn != 6)
									mlwrite("%pError %d %d ", cc, errn);
						  }
#endif
//					  continue;
    	case WAIT_TIMEOUT: 
#if _DEBUG
    					if (g_got_ctrl)
							{ g_got_ctrl = false;
							  return (int)(CTRL | 'C');
							}
#endif
							if (--totalwait == 0)			// -w opt
								exit(2);
														// drop through
			default:if (lim != 0)
								continue;
    }
#define LOG_K 0
#if LOG_K
		if (diags == NULL)
			diags = fopen("KEYS", "w");
#endif		
	{	int ix = -1;
		while (++ix < got)
    {	INPUT_RECORD * r = &rec[ix];
#if LOG_K
			fprintf(diags, "EventType %d %x %x  %x\n", r->EventType, r->Event.KeyEvent.dwControlKeyState,
																						 r->Event.KeyEvent.wVirtualKeyCode,
																						 r->Event.KeyEvent.uChar.AsciiChar);
#endif		
			if      (r->EventType == KEY_EVENT && r->Event.KeyEvent.bKeyDown)
			{	int keystate = r->Event.KeyEvent.dwControlKeyState;
	      int ctrl = keystate & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED);
	      if (ctrl)
				{	ctrl = CTRL;
					g_chars_since_ctrl = 0;
				}
	
	    { int chr = r->Event.KeyEvent.wVirtualKeyCode;
	      if (in_range(chr, 0x10, 0x14))
	        continue;														/* shifting or caps lock key only */
	    
	      if (keystate & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED))
		 		  ctrl |= ALTD;
				else
					chr = r->Event.KeyEvent.uChar.AsciiChar & 0xff;
	
				if (/*chr !=  0x7c && */ (chr | 0x60) != 0x7c)	// | BSL < or ^ BSL
				{	int vsc = r->Event.KeyEvent.wVirtualScanCode;
					if (in_range(vsc, SCANK_STT, 0x58))
		      { ctrl |= SPEC;
						chr = scantokey[vsc - SCANK_STT];
					}
//				else if (in_range(vsc, 2, 10) && chr == 0)
//					chr = '0' - 1 + vsc;
				}
	     
	      if ((keystate & SHIFT_PRESSED) && ctrl)		// exclude e.g. SHIFT 7
	      	ctrl |= SHFT;
	      
	      g_eaten[++oix] = ctrl | (chr == 0xdd ? 0x7c : chr);
				++g_chars_since_ctrl;
	    }}
	    else if (r->EventType == MENU_EVENT)
	    { /*loglog1("Menu %x", r->Event.MenuEvent.dwCommandId);*/
	    }
		}
		
		if (lim != 0 && got == need && oix < EAT_SZ - 1)
		{	lim = 5;
			continue;
		}

		if (oix >= 0)
		{
			g_eaten_pair = oix  << 8;
#if LOG_K
			fprintf(diags, "Eaten %x\n", g_eaten[0]);
#endif		
			return g_eaten[0];
		}
  }}
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


static char * mkTempCommName(char suffix, /*out*/char *filename)
{
#ifdef _CONVDIR_CHAR
 #define DIRY_CHAR _CONVDIR_CHAR
#else
 #define DIRY_CHAR DIRSEPCHAR
#endif
	const char * td = gettmpfn();
	
{	char *ss = concat(filename,td,"/me"+(td[strlen(td)-1] == DIRY_CHAR),int_asc(_getpid()),NULL);
	int tail = strlen(ss);
	int iter; 
	ss[tail] = suffix;
	ss[tail+1] = 0;
	
	for (iter = 25; --iter >= 0; )
	{
		if (!fexist(ss))
			break;
		
		ss[tail-2] = 'A' + 24 - iter;				// File should not exist anyway
	}
	return filename;
}}

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




int Pascal name_mode(const char * s)

{	char filen[NFILEN+1];
	char * t;
	for (t = strpcpy(filen, s, NFILEN+1)-1; *++t != 0; )
		if (*t == '/')
			*t = '\\';

{	BY_HANDLE_FILE_INFORMATION fileinfo;
	int res = 0x04;
  HANDLE myfile = Create(0, filen);
	if ((int)myfile < 0)
		return 0;
	if (GetFileInformationByHandle(myfile, &fileinfo))
	{ res |= fileinfo.dwFileAttributes & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_DIRECTORY);
	  if (fileinfo.nNumberOfLinks > 1)
	  	res |= FILE_ATTRIBUTE_NORMAL;
	}
	
  CloseHandle(myfile);

	return res;
}}


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

typedef struct 
{	STARTUPINFO si;
  SECURITY_ATTRIBUTES sa;
  HANDLE write_stdin;							// pipe handles
  HANDLE read_stdout;
} SiSa;

static																						/* flags: above */
Cc WinLaunch(int flags,
						 const char *cmd,
          	 const char *ipstr, const char *infile, const char *outfile 
          	 // char *outErr
						)
{ char buff[512];           //i/o buffer
	int quote = 0;

	const char * s = cmd;
	if (*s == '"')
	{ quote = *s;
		++s;
	}
{	char * t = buff - 1;
  while (1)
  { char ch = *s++;
  	*++t = ch;
  	if (ch == quote || ch <= ' ')
			break;
  }
  *t = 0;
	
{ int ct = 2;
	const char * app;
		
	while ((app = flook(Q_LOOKP, buff)) == NULL && --ct > 0)
		strcat(buff, ".exe");
			
	if (ct > 0)
		app = NULL;
	else
	{	if ((flags & WL_SHELL))					// use comspec
		{	app = (char*)getenv("COMSPEC");
			if (app == NULL)
				app = "cmd.exe";
	
			if (cmd != NULL)														/* Create the command line */
			{	if (strlen(cmd)+5 >= sizeof(buff))
					return -1;
			{	char * dd = strcpy(buff," /c \"")+5;
				char ch;
			//char prev = 'A';
	
				for (; (ch = *cmd++); /* prev = ch */)
				{// if (ch == '/' && 										// &&!(flags & LAUNCH_LEAVENAMES)
				 //	  (in_range(toupper(prev), 'A','Z')
				 // || in_range(prev, '0', '9')
				 //	||					prev == '_'  || prev == ' '))
				 //		ch = '\\';
			
					if (ch == '"')
						*dd++ = '\\';
					*dd++ = ch;
				}
	
				*dd = '"';
				dd[1] = 0;
				cmd = buff;
			}}
		}
	}

{	PROCESS_INFORMATION pi;
	SiSa sisa;
	Cc wcc = OK;
//pi.hProcess = 0;
	memset(&sisa, 0, sizeof(sisa));
	sisa.si.cb = sizeof(sisa.si);
//sisa.sa.lpSecurityDescriptor = NULL;
  sisa.sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sisa.sa.bInheritHandle = TRUE;         //allow inheritable handles

  pd_sgarbf = TRUE;
	upwind(TRUE);
//memset(&pi, 0, sizeof(pi));

//if (flags & WL_SHOWW)
//{ sisa.si.dwFlags |= STARTF_USESHOWWINDOW;
//  sisa.si.wShowWindow = SW_SHOWNORMAL;
//}
							  
	if (!(flags & WL_SPAWN))
	{ if ((flags & WL_NOIHAND) == 0)
//		sisa.si.hStdInput = Create(infile,FILE_SHARE_READ-1);
      sisa.si.hStdInput = Create(0, infile);
//      	 == NULL ? "nul" : infile,
//                                GENERIC_READ,FILE_SHARE_READ,&sisa.sa,
//                                OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (outfile != NULL)
    { sisa.si.hStdOutput = CreateFile(outfile,GENERIC_WRITE,FILE_SHARE_WRITE,&sisa.sa,
                                      CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY,NULL);
#if _DEBUG
			if (sisa.si.hStdOutput <= 0)
				mbwrite("CFOut Failed");
#endif
		}
		sisa.si.dwFlags |= STARTF_USESTDHANDLES;
	}
	else
	{	if      (!CreatePipe(&sisa.read_stdout,&sisa.si.hStdOutput,&sisa.sa,0)) //create stdout
	  	wcc = -1000;
		else if ((ipstr != NULL || infile != NULL)
		  		&& !CreatePipe(&sisa.si.hStdInput,&sisa.write_stdin,&sisa.sa,0))  //create stdin
			wcc = -2000;
		else
		{ // mbwrite("Created WSO");
// 		GetStartupInfo(&si);      //set startupinfo for the spawned process
			sisa.si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		  sisa.si.wShowWindow = SW_HIDE;
//	  sisa.si.lpTitle = "Emsub";
//	  sisa.si.hStdInput = g_ConsIn;
	  }
	  flags |= WL_IHAND;
	}

	if (sisa.si.hStdOutput > 0)
#if 1
		sisa.si.hStdError = sisa.si.hStdOutput;
#else
	{	HANDLE cur_proc = GetCurrentProcess();
									 
		BOOL s = DuplicateHandle(cur_proc,sisa.si.hStdOutput,
														 cur_proc,&sisa.si.hStdError,0,TRUE,
														 DUPLICATE_SAME_ACCESS) ;
	}
#endif	  

//mbwrite(app == NULL ? "<no app>" : app);
//mbwrite(ca == NULL ? "<no args>" : ca);
//mbwrite(int_asc((int)sisa.si.hStdOutput));

{	DWORD exit = STILL_ACTIVE;  			//process exit code
  DWORD bread = 0, bwrote = 0;  		//bytes read/written
  DWORD avail;  										//bytes available
//int sct = 6;
 	int sentz = 0;

	if      (wcc != OK)
		wcc += flagerr("CreatePipe");
  else if (!CreateProcess(app,					//spawn the child process
                    			(char*)cmd,
                    			NULL,NULL,
													(flags & WL_IHAND),
                					flags & (WL_CNPG+WL_CNC),
                    			NULL,NULL,&sisa.si,&pi))
		wcc = -3000 + flagerr("CreateProcess");
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
 					if (!_kbhit() /* && (TTbreakFlag != 0)*/) 
						continue;
				}
			{ Cc cc = (procStatus != WAIT_FAILED);
				if (cc)
				{ cc = GetExitCodeProcess(pi.hProcess,&exit);
				  if (cc != 0)
						break;
				  flagerr("GECP");
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
			FILE * ip_ = infile == NULL ? NULL  : fopen(infile, "r");
			FILE * ip = ip_;
	  	FILE * op = outfile == NULL ? NULL_OP : fopen(outfile, "w");
			int append_nl = 1;
	  	int std_delay = 5;
		  int got_ip = 0;
	  	int delay = 0;
	  	Cc cc;
#if _DEBUG			
			if (sisa.read_stdout == 0)
			{ mbwrite("Int Err");
				return -1;
			}
#endif
		  while (TRUE)															      //main program loop
		  {	(void)millisleep(delay);											//check for data on stdout
		  	delay = std_delay;
		  	cc = PeekNamedPipe(sisa.read_stdout,buff,2,&bread,&avail,NULL);
#if _DEBUG			
			  if (!cc)
		  	 	flagerr("PNP");
#endif
				if (bread == 0)
		  	{ cc = GetExitCodeProcess(pi.hProcess,&exit); //while process exists
				  if (!cc)
				  	break;
	  			if (exit != STILL_ACTIVE)
	  				break;
	  		}
	  		else
	  		{ DWORD done = 0;
	  			buff[bread] = 0;
//	  		mbwrite(buff);
	    	  while (done < avail)
	    	  {	cc = ReadFile(sisa.read_stdout,buff+done,1023-done,&bread,NULL);
					  if (cc == 0)
	    	 			break;

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

				if (ipstr == NULL)
					ipstr = "";
				if (*ipstr == 0)
		    {	if (ip != NULL)
				  {	ipstr = fgets(&fbuff[0], sizeof(fbuff)-1-bwrote, ip);
				    if (ipstr == NULL)
				    {	ip = NULL;
							continue;
				    }
					}
				}
			{	int sl = *ipstr;
				if (sl != 0)
		    { char ch;
					char * ln = l.buf-1+bwrote;
		    	--ipstr;
		      while ((*++ln = (ch=*++ipstr)) != 0 && ch != '\n')
		        ;

		      if (ch == 0 && ip == NULL)
		      {	*++ln = ch = '\n';
		      	//mbwrite("Ends");
		      }
		      sl = (ln - l.buf) + 1;
			    if (sl > 0 && ch != '\n' && append_nl)
				  { l.buf[sl++] = '\n';
					 	append_nl = 0;
					}
		    }
		   	if (!sentz && sl == 0)
			  {	l.buf[sl++] = 'Z'-'@';
					std_delay = 50;
				 	sentz = 4000 / 50;			// Wait 4 seconds
				}

		  { Cc cc_ = WriteFile(sisa.write_stdin,l.buf+bwrote,sl,&bwrote,NULL);//send to stdin
		    if (cc_ == 0)
			  	wcc = -4000 + flagerr("WriteFile");
			  else
		   	{	if (sl - bwrote > 0)
				  {	l.buf[sl] = 0;
			  		strpcpy(l.buf, l.buf+bwrote, sizeof(l.buf));	// overlapping copy
			  		bwrote = sl - bwrote;
			  	}
//		  	mlwrite("Sent %d",bread);
//				mbwrite(lastmesg);
				}		  
				l.i[0] = 0;						// Conceal password data
				l.i[1] = 0;
				l.i[2] = 0;
	    	//delay = STD_DELAY;
	    }}} /* loop */
	    
	    if (!cc)
				flagerr("PIP");

			if (ip_ != NULL)
				fclose(ip_);
			if (op != NULL_OP)
				fclose(op);
		}
	}

//printf("Exitted %d\n", exit);
  
	if (pi.hProcess)
	{	if (exit == STILL_ACTIVE)
		{ int rc = TerminateProcess(pi.hProcess, 1);
#if _DEBUG
			if (rc == 0)
				mbwrite("Rogue Process");
#endif
		}
	  CloseHandle(pi.hProcess);
	}

  CloseHandle(sisa.si.hStdInput);
  CloseHandle(sisa.si.hStdOutput);
//CloseHandle(sisa.si.hStdError);
  CloseHandle(sisa.read_stdout);
  CloseHandle(sisa.write_stdin);
	(void)setMyConsoleIP();

  return wcc != OK ? wcc :
  			 sentz < 0 ? sentz  : (Cc)exit;
}}}}}




int ttsystem(const char * cmd, const char * data)

{ Cc cc;
#if 0
  if (data == NULL)
  { cc = system(cmd);
		setMyConsoleIP();
	}
  else
#endif
//  if (*(cmd + (t-app)) != ' ')
//   	mbwrite("No space");
  	cc = WinLaunch(WL_SPAWN+WL_CNC+WL_AWAIT_PROMPT,
  								 cmd,
  								 data,null,null);
	return cc;
}

extern Char msd_path_[NFILEN+40];

	/* Pipe a one line command into a window
	 */
int pipefilter(char wh)
{
 static int bix;
 				char 	 bname [10];
				char	 pipeInFile[NFILEN];
				char	 pipeOutFile[NFILEN];
							
				char line[NSTRING+2*NFILEN+100];			 /* command line send to shell */

	if (resterr())
		return FALSE;

{ Cc cc;
	char prompt[2];
	prompt[0] = wh;
	wh -= '<';											
	if (wh == '#'-'<' && rdonly())
		return FALSE;

#ifdef USE_SVN
	if (wh == 'e'-'<')
		strpcpy(line, g_ll.lastline[0], sizeof(line)-2*NFILEN);
	else
#endif
	if (wh == '='-'<')
		strcpy(line, msd_path_);
	else
	{ extern int g_last_cmd;
	  prompt[1] = 0;
		if (mlreply(prompt, line, NLINE) <= FALSE)
			return FALSE;
			
		g_last_cmd = g_ll.ll_ix & MLIX;
		if (line[0] == '%' || line[0] == '\'')
		{ char sch;
      int ix;
			for (ix = 0; (sch = line[++ix]) >= '0'; )
				;

			line[ix] = 0;

		{ const char * val = gtusr(line+1);
			line[ix] = sch;

			if (val != NULL)
				strcpy(line,strcat(strcpy(pipeInFile,val),line+ix));
		}}
	}

{	char * fnam1 = NULL;

	if (wh == '#'-'<' || wh == '<'-'<') 			/* setup the proper file names */
	{ 			
		fnam1 = mkTempCommName('i', pipeInFile);

		if (writeout(fnam1) <= FALSE)		/* write it out, checking for errors */
		{ // mlwrite(TEXT2);
																			/* "[Cannot write filter file]" */
			return FALSE;
		}
		curbp->b_flag |= BFCHG;
		mlwrite(TEXT159);					/* "\001Wait ..." */
	}

//tcapmove(term.t_nrowm1, 0);

{	char * fnam2 = mkTempCommName('o', pipeOutFile);

	cc = WL_IHAND + WL_HOLD + WL_SHELL;

	if      (wh == '#'-'<')
//	cc |= WL_SPAWN+WL_CNC;
		cc |= WL_SHELL+WL_CNC;
//else if	(wh >= '<'-'<')					// < @
//	cc |= WL_SHELL;
#if 0
	else if (wh == 'E' -'<')
		cc |= LAUNCH_STDERROUT;
#endif
#ifdef USE_SVN
	else if (wh == 'e' -'<')
	{ wh = 'E';
		cc |= LAUNCH_STDIN;
	}
#endif

	cc = WinLaunch(cc,line, null, fnam1, fnam2);
	if (cc != OK)
	{	if (wh == '='-'<')
			return FALSE;
		mlwrite(TEXT3, cc); 							/* "[Execution failed]" */
//	return FALSE;
	}

	if (wh >= '<'-'<')   /* <  =  @ */
	{ BUFFER * bp = bfind(strcat(strcpy(bname,"_cmd"),int_asc(++bix)), TRUE);
		if (bp == NULL)
			return FALSE;
/*	
		if (splitwind(FALSE, 1) == FALSE)
			return FALSE;
*/
		if (wh == '='-'<')
			curbp = bp;
		else
		{	swbuffer(bp);
		/*linstr(tmpnam); */
								/* make this window in VIEW mode, update all mode lines */
			curwp->w_bufp->b_flag |= MDVIEW;
		/*upmode();*/
		}
	}
{	Cc rc = FALSE;
	if (wh == '!'-'<')
	{ FILE * ip = fopen(fnam2, "rb");
		if (ip != NULL)
		{ char * ln;
			while ((ln = fgets(&line[0], NSTRING+NFILEN*2-1, ip))!=NULL)
				fputs(ln, stdout);

			fclose(ip);
			puts(TEXT6);
			ttgetc();
		/*homes();*/
			rc = TRUE;
		}
	}
	else													/* on failure, escape gracefully */ 		
	{ BUFFER * bp = curbp;
		char * sfn = bp->b_fname;
		bp->b_fname = null;									/* otherwise it will be freed */
		rc = readin(fnam2, FILE_NMSG);
		bp->b_fname = sfn; 									/* restore name */
		bp->b_flag |= BFCHG; 								/* flag it as changed */
		flush_typah();
	}
																	/* get rid of the temporary files */
	if (fnam1 != NULL)
		unlink(fnam1);							
	unlink(fnam2);
//if (wh == 'E' - '<')
//	unlink(fnam3);
	return rc;
}}}}}

	/* Pipe a one line command into a window
	 * Bound to ^X @
	 */
int Pascal pipecmd(int f, int n)

{ return pipefilter(n >= 0 ? '@' : '<');
}

	/*
	 * filter a buffer through an external DOS program
	 * Bound to ^X #
	 */
int Pascal filter(int f, int n)

{ return pipefilter('#');
}


/* Run a one-liner in a subjob. When the command returns, wait for 
	* a single character to be typed, then mark the screen as garbage 
	* so a full repaint is done. Bound to "C-X !".
	*/
int Pascal spawn(int f, int n)
{
	return pipefilter('!');
}


char * searchfile(char * result, Fdcr fdcr)

{ FILE * ip = (FILE*)fdcr->ip;
	if (ip == NULL)
	{ char buf[NFILEN+20];
//	char * basename = result+strlen(result)+1;
		
		char * cmd = concat(buf, "ffg -/ ", /* basename, " ", */ result, NULL);

		char * fnam2 = mkTempCommName('o', fdcr->name);
		Cc cc = WinLaunch(WL_IHAND+WL_HOLD+WL_SHELL,
												cmd, NULL, NULL, fnam2);
		if (cc != OK)
			return NULL;
		ip = fopen(fnam2, "rb");
		fdcr->ip = ip;
		if (ip == NULL)
			return NULL;
	}

{ char * fname = fgets(result, NFILEN, ip);
	if (fname == NULL)
	{	fclose(ip);
		unlink(fdcr->name);
	}
	else
	{ int sl = strlen(fname)-2;
		if (sl <= 0)
			fname = NULL;
		else														// strip off CR,LF
			fname[sl] = 0;
	}

	return fname;
}}

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
X return g_logm[2];
X#endif
}

#endif

// ******************************************************************

// Clipboard Functions

static HANDLE	g_hClipData;

static int g_clix = 0;


static
Cc OpenClip()

{	HWND mwh = GetTopWindow(NULL);
	if (mwh == NULL)
		return -1;
{ Cc cc = OpenClipboard(mwh);
	if (cc == 0)
		return 0;
//{ DWORD ec = GetLastError();
//	return ec == ERROR_ACCESS_DENIED ? 0 : -1;
//}
	
	return 1;
}}


static 
DWORD WINAPI ClipSet_(int is_thread)

{ return ClipSet(is_thread);
}


Cc ClipSet(int clix)

{	if (clix > 0)
	{	millisleep(pd_cliplife * 1000);

//	if (mwh != GetTopWindow(NULL))
//		return -1;

		if (clix != g_clix)
			return 0;
	}

{ Cc cc = OpenClip();
	if (cc > 0)
	{	
		EmptyClipboard();
		if (clix == 0)
		{	char * data = getkill();
			int len = strlen(data);
			HANDLE m_hData = GlobalAlloc(GMEM_DDESHARE, len + KBLOCK*20 + 10);
			if (!m_hData)  
				return -1;
	
		{	char * m_lpData = (char*)GlobalLock(m_hData);
			if (m_lpData == NULL)
				return -1;
	
			strcpy(&m_lpData[0], data);
			SetClipboardData(CF_TEXT, m_hData);
			GlobalUnlock(m_hData);
		}}

		CloseClipboard();
	}

#if 1
	if (clix > 0 && pd_cliplife != 0)
  {	HANDLE thread = CreateThread(NULL, 0, ClipSet_, (void*)(++g_clix),0,NULL);
  }
#endif
	return OK;
}}


#if 0

Cc ClipDelete()

{ Cc cc = OpenClip();
	if (cc <= 0)
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

{ Cc cc = OpenClip();
	if (cc > 0)
	{	g_hClipData = GetClipboardData(CF_TEXT);
		if (g_hClipData)
			return (char *)GlobalLock(g_hClipData);
	}

	if (cc < 0)
		loglog("PasteFailed");
	return null;
}



void ClipPasteEnd()

{ 
	if (g_hClipData)
	{ GlobalUnlock(g_hClipData);
		g_hClipData = NULL;
	}
	CloseClipboard();
}

void Pascal mbwrite(const char * msg)

{
#if S_WIN32
		const char * em_ = "Emacs";
#if VS_CHAR8
		char * m = (char*)(msg == NULL ? lastmesg : msg);
		const char * em = em_;
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
			flagerr("SPF");
		else
			SetActiveWindow(mwh);
	}
}
#endif
