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

CONSOLE_SCREEN_BUFFER_INFO csbiInfo;   /* Console information */
//CONSOLE_SCREEN_BUFFER_INFO csbiInfoO;  /* Orig Console information */
//CONSOLE_CURSOR_INFO        ccInfo;


#define BG_GREY (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)


long unsigned int thread_id(void)

{ return (int)GetCurrentProcess() + 29 * GetCurrentThreadId();
}


int init_wincon()

{	
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
																							// reduces memory but slows startup
	SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);

											      /* Get display screen information, clear the screen.*/
	g_ConsOut = GetStdHandle( STD_OUTPUT_HANDLE );

#if 0
  if (SetConsoleMode(g_ConsOut, ENABLE_PROCESSED_OUTPUT) == 0)
    flagerr("SCMO %d");
#endif

	GetConsoleScreenBufferInfo( g_ConsOut, &csbiInfo );
//GetConsoleScreenBufferInfo( g_ConsOut, &csbiInfoO );

	SetConsoleTextAttribute(g_ConsOut, BG_GREY);
		 
//tcapepage();
//SetConsoleTextAttribute(g_ConsOut, BG_GREY);
}


void Pascal tcapeeol()

{ COORD     Coords;
  Coords.Y = ttrow;
  Coords.X = ttcol;
{ int sz = csbiInfo.dwSize.X-ttcol;
	DWORD     Dummy;
  
  FillConsoleOutputCharacter(g_ConsOut, ' ',sz,Coords,&Dummy );
  return OK;
}}

/*
void Pascal tcapepage()

{ tcapbeeol(-1,0);
}
*/


#if 0

/* This function gets called just before we go back home to the command
 * interpreter.
 */
int Pascal tcapclose(int lvl)

{ 
/*CloseHandle(hConsoleIn);
  hConsoleIn = NULL;  cannot do this */
  tcapsetsize(csbiInfoO.dwSize.X, csbiInfoO.dwSize.Y);
  return OK;
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



#if VS_CHAR8 == 0

void Pascal setconsoletitle(char * title)

{
	wchar_t buf[100];
	swprintf(buf, 100, L"%S", title == null ? "" : title);
  SetConsoleTitle(buf);
}

#endif

// char * Pascal getconsoletitle()

// { return consoletitle;
// }


void Pascal tcapsetsize(int wid, int dpth)

{//DWORD mode;
	if (wid < 0)
		wid = -wid;
	else
	{	int decw = csbiInfo.dwSize.X - wid;
		int decd = csbiInfo.dwSize.Y - dpth;
		if (decd < 0)
			decd = 0;
		if (decw < 0)
			decw = 0;
		
		if (decw > 0 || decd > 0)
			tcapsetsize(-(csbiInfo.dwSize.X - decw - 1), csbiInfo.dwSize.Y - decd - 1);
	}
{
#if 0
	int rc = Console.SetWindowSize(wid, dpth);
#if _DEBUG
  if (rc == 0)
    flagerr("SCWI %d");
#endif
#else
 	SMALL_RECT rect = { 0, 0, wid - 1, dpth-1 };
	COORD size;
  size.X = wid;
  size.Y = dpth;

{ HANDLE h = g_ConsOut;
  SetConsoleScreenBufferSize( h, size);

// GetConsoleMode(g_ConsOut, &mode);
// mode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
// HANDLE consin = GetStdHandle(STD_INPUT_HANDLE);
// SetConsoleMode(g_ConsIn, ENABLE_WINDOW_INPUT);

	GetConsoleScreenBufferInfo( h, &csbiInfo );
																	// set the screen buffer to be big enough
#if 1
{ int rc = SetConsoleWindowInfo(h, 1, &rect);
  if (rc == 0)
    flagerr("SCWI %d");
}
#endif
#endif
}}}

			 int   g_cursor_on = 0;
static COORD g_oldcur;
static WORD  g_oldattr = BG_GREY;


/*
Bool Pascal cursor_on_off(Bool on)

{ Bool res = g_cursor_on;
  g_cursor_on = on;
  return res;
}*/


void Pascal tcapmove(int row, int col)

{/* if (row == ttrow && col == ttcol)
      return; */
 if (row > term.t_mrowm1)
  { tcapbeep();
    row = 0;
  }
  
{ WORD MyAttr = row == term.t_mrowm1 ? BG_GREY
                        					   : window_bgfg(curwp) | BACKGROUND_INTENSITY;
  DWORD  Dummy;
  COORD  Coords;
  ttrow = row;
  Coords.Y = ttrow;
  ttcol = col;
  Coords.X = ttcol;

  if (g_cursor_on >= 0)
  { WriteConsoleOutputAttribute( g_ConsOut, &g_oldattr, 1, g_oldcur, &Dummy );
    WriteConsoleOutputAttribute( g_ConsOut, &MyAttr, 1, Coords, &Dummy );

    g_oldattr = refresh_colour(row, col);
    g_oldcur = Coords;
  }
  SetConsoleCursorPosition( g_ConsOut, Coords);
}}



#if 0

void Pascal tcapscreg(row1, row2)
     int row1, row2;
{ 
}

#endif


void Pascal ttscupdn(n)                  /* direction the window moves */
        int   n;   
{ CHAR_INFO ci;
	SMALL_RECT rect;
	COORD doo;
  doo.Y = 0;
  doo.X = 0;
  rect.Top = 0;
  rect.Left = 0;
  rect.Right = csbiInfo.dwSize.X;
  rect.Bottom = csbiInfo.dwSize.Y;
  
  if (n < 0)
    rect.Top += n;
  else 
  { rect.Bottom += n;
    doo.Y -= n;
  }

  ci.Char.AsciiChar = ' ';
  ci.Attributes = BG_GREY;
    
  if (ScrollConsoleScreenBufferA( g_ConsOut, &rect, null, doo, &ci) == 0)
    tcapbeep();
}

//------------------------------------------------------------------------------

void Pascal ttputc(unsigned char ch) /* put character at the current position in
														   		      current colors */
{			/* if its a newline, we have to move the cursor */
  unsigned long  Dummy;
  register int col;
#if VS_CHAR8
#define gch (char)ch
#else
	wchar_t gch = ch;
#endif

/*GetConsoleScreenBufferInfo( g_ConsOut, &ccInfo );*/
	GetConsoleScreenBufferInfo( g_ConsOut, &csbiInfo );
  col = csbiInfo.dwCursorPosition.X;
/* ttcol = col;*/
  ttrow = csbiInfo.dwCursorPosition.Y;

  if (ch != '\n' && ch != '\r')
  { if (ch != '\b')
    { col += 1; 
      if (col >= csbiInfo.dwSize.X)
      { mlwrite("Row %d Col %d Lim %d", ttrow, col, csbiInfo.dwSize.X);
				mbwrite(lastmesg);
				return;
      }
    }
    else
    { col -= 1;
      if (col < 0)
        col = 0;
    }
  }
  else
  { ttrow += 1;
    if (ttrow >= csbiInfo.dwSize.Y)
    { ttscupdn(1);                 /* direction the window moves */
      ttrow -= 1;
    }
  }
				/* write char to screen with current attrs */
  WriteConsoleOutputCharacter(g_ConsOut, &gch,1, csbiInfo.dwCursorPosition, &Dummy);
  csbiInfo.dwCursorPosition.X = col;
  csbiInfo.dwCursorPosition.Y = ttrow;
  SetConsoleCursorPosition( g_ConsOut, csbiInfo.dwCursorPosition);
/* rg.x.bx = cfcolor; */
}



int Pascal tcapbeep()

{ Beep( 500, 250 /*millisecs*/);
//mbwrite("BEEP\n");
  return OK;
}



void Pascal tcapopen()

{ int plen = csbiInfo.srWindow.Bottom-csbiInfo.srWindow.Top+1;
  int pwid = csbiInfo.srWindow.Right -csbiInfo.srWindow.Left+1;

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
	const SC_WORD sclen = csbiInfo.dwSize.X >= 148 ? 148 : csbiInfo.dwSize.X;
	unsigned long n_out;
	WORD attr = color;

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
	
	buf[++col] = 0;
	cuf[col] = 0;
	Coords.X = 0;
	Coords.Y = row;
			 
	WriteConsoleOutputCharacter( g_ConsOut, buf, sclen, Coords, &n_out );
{ int cc = WriteConsoleOutputAttribute( g_ConsOut, cuf, sclen, Coords, &n_out);
	if (cc == 0 || n_out != sclen)
		adb(cc);
	return OK;
}}


#if	FLABEL
int Pascal fnclabel(f, n)	/* label a function key */
	int f,n;	/* default flag, numeric argument [unused] */
{		/* on machines with no function keys...don't bother */
  return TRUE;
}
#endif


