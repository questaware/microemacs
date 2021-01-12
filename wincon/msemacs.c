
//#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>

//err err;

#include <windows.h>

//more err;

#include "estruct.h"
#include "../src/edef.h"
#include "../src/etype.h"
#include "../src/logmsg.h"

extern void flagerr(const char * fmt);

HANDLE  g_ConsOut;                   /* Handle to the console */

CONSOLE_SCREEN_BUFFER_INFO csbiInfo;   /* Console information */
//CONSOLE_SCREEN_BUFFER_INFO csbiInfoO;  /* Orig Console information */
//CONSOLE_CURSOR_INFO        ccInfo;


#define BG_GRAY (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)


long unsigned int thread_id(void)

{ return GetCurrentThreadId();
}


void ClearScreen( void )
{
          DWORD    dummy;
    const COORD    Home = { 0, 0 };
    int len = csbiInfo.dwSize.X * csbiInfo.dwSize.Y;
    FillConsoleOutputAttribute( g_ConsOut, BG_GRAY, len, Home, &dummy );
    FillConsoleOutputCharacter( g_ConsOut, ' ',     len, Home, &dummy );
}

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



int main(int argc, char * argv[])

{	SC_CHAR modulename[129];
	HINSTANCE hInstance = GetModuleHandle(NULL);	 // Grab An Instance For Window
	GetModuleFileName(hInstance, modulename, sizeof(modulename)-1);

#if VS_CHAR8
#else
	(void)wchar_to_char(modulename);
#endif
	flook_init(modulename);
										    /* Get display screen information & clear the screen.*/
	g_ConsOut = GetStdHandle( STD_OUTPUT_HANDLE );

#if 0
  if (SetConsoleMode(g_ConsOut, ENABLE_PROCESSED_OUTPUT) == 0)
    flagerr("SCMO %d");
#endif

	GetConsoleScreenBufferInfo( g_ConsOut, &csbiInfo );
//GetConsoleScreenBufferInfo( g_ConsOut, &csbiInfoO );

	SetConsoleTextAttribute(g_ConsOut, BG_GRAY);
		 
	ClearScreen();
//SetConsoleTextAttribute(g_ConsOut, BG_GRAY);

	main_(argc, argv);

 /* tcapclose(); */
  /*CloseHandle( g_ConsOut );*/
}


void Pascal setconsoletitle(char * title)

{
#if VS_CHAR8
#define txt title
#else
#define txt buf
	wchar_t buf[100];
	swprintf(buf, 100, L"%S", title == null ? "" : title);
#endif
  SetConsoleTitle( title == NULL ? "" : txt);
#undef txt
}


// char * Pascal getconsoletitle()

// { return consoletitle;
// }

extern HANDLE g_ConsIn;

void setMyConsoleIP()

{ g_ConsIn = GetStdHandle( STD_INPUT_HANDLE );
  if (g_ConsIn < 0)					                    /* INVALID_HANDLE_VALUE */
    flagerr("Piperr %d");

/* SetStdHandle( STD_INPUT_HANDLE, g_ConsIn ); */
  if (0 == SetConsoleMode(g_ConsIn, ENABLE_WINDOW_INPUT))
    flagerr("PipeC %d");
}



void Pascal tcapsetsize(int wid, int dpth)

{//DWORD mode;
  COORD size;
  size.X = wid;
  size.Y = dpth;
  
  SetConsoleScreenBufferSize( g_ConsOut, size);

//GetConsoleMode(g_ConsOut, &mode);
//mode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
{// HANDLE consin = GetStdHandle(STD_INPUT_HANDLE);
 // SetConsoleMode(g_ConsIn, ENABLE_WINDOW_INPUT);

  GetConsoleScreenBufferInfo( g_ConsOut, &csbiInfo );
	// set the screen buffer to be big enough

  if (csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top < dpth)
  { int rc;
//  mlwrite("%d Ws %d Cp %d MWS %d dpth %d", ct, csbiInfo.dwSize.Y, 
//                                csbiInfo.dwCursorPosition.Y,
//                                csbiInfo.dwMaximumWindowSize.Y, dpth
//         );
    csbiInfo.srWindow.Top = 0;
    csbiInfo.srWindow.Bottom = dpth-1;
    csbiInfo.dwCursorPosition.Y = dpth - 1;
    
    rc = SetConsoleWindowInfo(g_ConsOut, 1, &csbiInfo.srWindow);
    if (rc == 0)
      flagerr("SCWI %d");
  }
}}


static Bool  g_cursor_on = true;
static COORD g_oldcur;
static WORD  g_oldattr = BG_GRAY;



Bool Pascal cursor_on_off(Bool on)

{ Bool res = g_cursor_on;
  g_cursor_on = on;
  return res;
}


void Pascal tcapmove(int row, int col)

{/* if (row == ttrow && col == ttcol)
      return; */
 if (row > term.t_mrowm1)
  { tcapbeep();
    row = 0;
  }
    
{ short * scl = get_vscr_line(row);
  WORD attr = get_vscr_colors(row);
  WORD attr_ = attr;
  int icol;
  for (icol = -1; ++icol <= col; )
    if (scl[icol] & 0xff00)
      if (scl[icol] & 0x7f00)
      { attr_ = attr;		/* push */
        attr = (scl[icol] >> 8) & 0x7f;
      }
      else
      { attr = attr_;
      }
  
{ WORD MyAttr = row == term.t_mrowm1 ? BG_GRAY
                        					 : window_bgfg(curwp) | BACKGROUND_INTENSITY;
  DWORD  Dummy;
  COORD  Coords;
  ttrow = row;
  Coords.Y = ttrow;
  ttcol = col;
  Coords.X = ttcol;

  if (g_cursor_on)
  { WriteConsoleOutputAttribute( g_ConsOut, &g_oldattr, 1, g_oldcur, &Dummy );
    WriteConsoleOutputAttribute( g_ConsOut, &MyAttr, 1, Coords, &Dummy );
   
    
    g_oldattr = attr;
    g_oldcur = Coords;
  }
  SetConsoleCursorPosition( g_ConsOut, Coords);
}}}



int Pascal tcapbeeol(int row, int col) /* erase to the end of the line */

{
  DWORD     Dummy;
  COORD     Coords;
  Coords.Y = row;
  Coords.X = col;
  
  FillConsoleOutputCharacter(g_ConsOut, ' ',csbiInfo.dwSize.X-col,Coords,&Dummy );
  return OK;
}


void Pascal tcapeeol()

{ tcapbeeol(ttrow, ttcol);
}


void Pascal tcapeeop()

{ int row;
/* awaitgap(); */
  for (row = term.t_nrowm1+1; --row >= 0; )
    tcapbeeol(row, 0);
}

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
  ci.Attributes = BG_GRAY;
    
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
      { int sd = g_discmd;
      	g_discmd = 0;
				mlwrite("Row %d Col %d Lim %d", ttrow, col, csbiInfo.dwSize.X);
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
}




int Pascal scwrite(row, outstr, color)	/* write a line out */
	int 	 row; 	/* row of screen */
	short *outstr;		/* string to output (must be term.t_ncol long)*/
	int 	 color; 	/* background, foreground */
{ 			/* build the attribute byte and setup the screen pointer */
	SC_CHAR buf[512];
	WORD cuf[512];
	const SC_WORD sclen = csbiInfo.dwSize.X >= 148 ? 148 : csbiInfo.dwSize.X;
	unsigned long n_out;
	WORD attr = color;

	COORD Coords;
	int col;

	Coords.Y = row;
			 
	for (col = -1; ++col < sclen; )
	{ cuf[col] = attr;
		if (outstr[col] & 0xff00)
		{ 
			if (outstr[col] & 0x7f00)
			{ 																/* push */
				attr = (outstr[col] >> 8) & 0x7f;
				if (attr & FOREGROUND_INTENSITY)		// is 8
				{
					attr |= COMMON_LVB_UNDERSCORE;
				}
				cuf[col] = attr;
			}
			else
			{ attr = color;
			}
		}
		buf[col] = (outstr[col] & 0xff);
	}
	
	buf[++col] = 0;
	cuf[col] = 0;
	Coords.X = 0;

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


