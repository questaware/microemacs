#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <windows.h>

#include "estruct.h"
#include "../src/edef.h"
#include "../src/etype.h"
#include "../src/logmsg.h"

extern void flagerr(const char * fmt);

HANDLE  g_ConsOut;                   /* Handle to the console */

CONSOLE_SCREEN_BUFFER_INFO g_csbi;   /* Console information */
//CONSOLE_SCREEN_BUFFER_INFO csbiInfoO;  /* Orig Console information */
//CONSOLE_CURSOR_INFO        ccInfo;


#define BG_GREY  (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)
#define FG_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)


long unsigned int thread_id(void)

{ return (int)GetCurrentProcess() + 29 * GetCurrentThreadId();
}


UINT g_codepage;

void init_wincon()

{	g_codepage = GetConsoleOutputCP();
  SetConsoleCP(1252);
  SetConsoleOutputCP(1252);
#if 0
  SC_CHAR buf[129];
	HINSTANCE hInstance = GetModuleHandle(NULL);	 // Grab An Instance For Window
	GetModuleFileName(hInstance, buf, sizeof(buf)-1);

#if VS_CHAR8
#else
	(void)wchar_to_char(buf);
#endif
	flook_init(argv[0]);
#endif

//Sleep(1000*8);
																							// reduces memory but slows startup
	SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);

//Sleep(1000*8);
											      /* Get display screen information, clear the screen.*/
	g_ConsOut = GetStdHandle( STD_OUTPUT_HANDLE );

#if 0
  if (SetConsoleMode(g_ConsOut, ENABLE_PROCESSED_OUTPUT) == 0)
    flagerr("SCMO %d");
#endif

	GetConsoleScreenBufferInfo( g_ConsOut, &g_csbi );
//GetConsoleScreenBufferInfo( g_ConsOut, &csbiInfoO );

	SetConsoleTextAttribute(g_ConsOut, BG_GREY);
		 
//tcapepage();
//SetConsoleTextAttribute(g_ConsOut, BG_GREY);
}


void Pascal tcapeeol()

{ COORD     Coords;
  Coords.Y = ttrow;
  Coords.X = ttcol;
{ int sz = g_csbi.dwSize.X-ttcol;
	DWORD     Dummy;
  
  FillConsoleOutputCharacter(g_ConsOut, ' ',sz,Coords,&Dummy );
}}

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

{ SetConsoleCP(g_codepage);
  SetConsoleOutputCP(g_codepage);
/*CloseHandle(hConsoleIn);
  hConsoleIn = NULL;  cannot do this */
//tcapsetsize(csbiInfoO.dwSize.X, csbiInfoO.dwSize.Y, 2);
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


void Pascal tcapsetsize(int wid, int dpth, int clamp)

{	int decw = g_csbi.dwSize.X - wid;
	int decd = g_csbi.dwSize.Y - dpth;
	if (decd < 0)
		decd = 0;
	if (decw < 0)
		decw = 0;

	if (--clamp > 0 && decw + decd > 0)			// Must be done twice
	{ decw = g_csbi.dwSize.X - decw - 1;
		if (decw < 0)
			decw = -decw;
		tcapsetsize(decw, g_csbi.dwSize.Y - decd - 1, 0);
	}
{
#if 0
	int rc = Console.SetWindowSize(wid, dpth);
#if _DEBUG
  if (rc == 0)
    flagerr("SCWI %d");
#endif
#else
 	SMALL_RECT rect = { 0, 0, wid-1, dpth-1 };
	COORD size;
  size.X = wid;
  size.Y = dpth;

{ HANDLE h = g_ConsOut;
  SetConsoleScreenBufferSize( h, size);

// GetConsoleMode(g_ConsOut, &mode);
// mode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
// HANDLE consin = GetStdHandle(STD_INPUT_HANDLE);
// SetConsoleMode(g_ConsIn, ENABLE_WINDOW_INPUT);

#if 0
	GetConsoleScreenBufferInfo( h, &g_csbi );
#endif
																	// set the screen buffer to be big enough
{ int rc = SetConsoleWindowInfo(h, 1, &rect);
#if _DEBUG
  if (rc == 0)
    flagerr("SCWI %d");
#endif
}}
#endif
}}

//		 int   g_cursor_on = 0;
static COORD g_oldcur;
static WORD  g_oldattr = BG_GREY;


/*
Bool Pascal cursor_on_off(Bool on)

{ Bool res = g_cursor_on;
  g_cursor_on = on;
  return res;
}*/


void Pascal USE_FAST_CALL tcapmove(int row, int col)

{/* if (row == ttrow && col == ttcol)
      return; */
  COORD  Coords;

	if (row > term.t_nrowm1)
  { // tcapbeep();
    row = term.t_nrowm1;
  }
	ttrow = row;
	Coords.Y = row;

	ttcol = col;
	Coords.X = ttcol;

	if (row < term.t_nrowm1 /* && g_cursor_on >= 0 */)
  {	DWORD  Dummy;
		WriteConsoleOutputAttribute( g_ConsOut, &g_oldattr, 1, g_oldcur, &Dummy );
	  g_oldattr = refresh_colour(row, col);
	  g_oldcur = Coords;
	{ WORD MyAttr = // row == term.t_nrowm1 ? BG_GREY :
                       					  	  window_bgfg(curwp) | BACKGROUND_INTENSITY;

	  WriteConsoleOutputAttribute( g_ConsOut, &MyAttr, 1, Coords, &Dummy );	
	}}
	SetConsoleCursorPosition( g_ConsOut, Coords);
}



#if 0

void Pascal tcapscreg(row1, row2)
     int row1, row2;
{ 
}

#endif


static
void Pascal ttscup(int maxx, int maxy)/* direction the window moves*/

{ CHAR_INFO ci;
	SMALL_RECT rect;
	COORD doo;
  doo.Y = 0; // -1 ???;
  doo.X = 0;
  rect.Top = 0;
  rect.Left = 0;
  rect.Right = maxx;
  rect.Bottom = maxy + 1;
  
  ci.Char.AsciiChar = ' ';
  ci.Attributes = BG_GREY;
    
{ int cc = ScrollConsoleScreenBufferA( g_ConsOut, &rect, null, doo, &ci);
#if _DEBUG
	if (cc == 0)
    tcapbeep();
#endif
}}

//------------------------------------------------------------------------------

void Pascal ttputc(unsigned char ch) /* put character at the current position in
														   		      current colors */
{	HANDLE cout = g_ConsOut;

/*GetConsoleScreenBufferInfo( cout, &ccInfo );*/
	GetConsoleScreenBufferInfo( cout, &g_csbi );

{ unsigned long  Dum;
#if VS_CHAR8
#define gch (char)ch
#else
	wchar_t gch = ch;
#endif
																		/* write char to screen with current attrs */
  WriteConsoleOutputCharacter(cout, &gch,1, g_csbi.dwCursorPosition, &Dum);

/* ttcol = col;*/
{ int col = 0;

  if (ch != '\n' && ch != '\r')
  { // if (ch != '\b')
		col = g_csbi.dwCursorPosition.X + 1;
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
  else
	{	int row = g_csbi.dwCursorPosition.Y + 1;
    if (row <= g_csbi.dwSize.Y)
    { g_csbi.dwCursorPosition.Y = row;
//   	ttscup(g_csbi.dwSize.X, g_csbi.dwSize.Y);	/* direction the window moves*/
    }
  }

  g_csbi.dwCursorPosition.X = col;
//ttrow = row;
  SetConsoleCursorPosition( cout, g_csbi.dwCursorPosition);
}}}



int Pascal tcapbeep()

{ Beep( 500, 250 /*millisecs*/);
//mbwrite("BEEP\n");
  return OK;
}



void Pascal tcapopen()

{ int plen = g_csbi.srWindow.Bottom-g_csbi.srWindow.Top+1;
  int pwid = g_csbi.srWindow.Right -g_csbi.srWindow.Left+1;

  newdims(pwid, plen);
	setMyConsoleIP();
}



int Pascal scwrite(row, outstr, color)	/* write a line out */
	int 	 row; 			/* row of screen */
	short *outstr;		/* string to output (must be term.t_ncol long)*/
	int 	 color;		 	/* background, foreground */
{ 									/* build the attribute byte and setup the screen pointer */
	SC_CHAR buf[NCOL];
	WORD 		cuf[NCOL];
	unsigned long n_out;
	WORD attr = color;
	const SC_WORD sclen = g_csbi.dwSize.X >= 148 ? 148 : g_csbi.dwSize.X;

	COORD Coords;
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
	
	buf[col+1] = 0;
	cuf[col+1] = 0;
	Coords.X = 0;
	Coords.Y = row;
			 
	WriteConsoleOutputCharacter( g_ConsOut, buf, sclen, Coords, &n_out );
{ int cc = WriteConsoleOutputAttribute( g_ConsOut, cuf, sclen, Coords, &n_out);
#ifdef _DEBUG
	if (cc == 0 || n_out != sclen)
		adb(cc);
#endif
	return OK;
}}


#if	FLABEL
int Pascal fnclabel(f, n)	/* label a function key */
	int f,n;	/* default flag, numeric argument [unused] */
{		/* on machines with no function keys...don't bother */
  return TRUE;
}
#endif


