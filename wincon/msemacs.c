
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


HANDLE  hConsoleOut;                   /* Handle to the console */

CONSOLE_SCREEN_BUFFER_INFO csbiInfo;   /* Console information */
CONSOLE_SCREEN_BUFFER_INFO csbiInfoO;  /* Orig Console information */
//CONSOLE_CURSOR_INFO        ccInfo;


#define BG_GRAY (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)


void ClearScreen( void )
{
          DWORD    dummy;
    const COORD    Home = { 0, 0 };
    int len = csbiInfo.dwSize.X * csbiInfo.dwSize.Y;
    FillConsoleOutputAttribute( hConsoleOut, BG_GRAY, len, Home, &dummy );
    FillConsoleOutputCharacter( hConsoleOut, ' ',     len, Home, &dummy );
}


/* This function gets called just before we go back home to the command
 * interpreter.
 */
int Pascal tcapclose()

{ 
/*CloseHandle(hConsoleIn);
  hConsoleIn = NULL;  cannot do this */
  tcapsetsize(csbiInfoO.dwSize.X, csbiInfoO.dwSize.Y);
  return OK;
}


void Pascal tcap_init()

{ char * v = getenv("LINES");
  if (v != NULL)
  { int vv = atoi(v);
    if (vv != 0 && vv < 91)
    { term.t_nrowm1 = vv - 1;			
      term.t_mrowm1 = vv - 1;
    }
  }
  v = getenv("COLS");
  if (v != NULL)
  { int vv = atoi(v);
    if (vv != 0 && vv < 136)
    { term.t_ncol = vv;
      term.t_mcol = vv;
      term.t_margin = vv / 10;
    }
  }
}

/*
char * argv__[] = { "wincon", "tt", null};
int argc__ = 2;
*/

int main(int argc, char * argv[])

{
    char modulename[129];
    HINSTANCE hInstance = GetModuleHandle(NULL);		// Grab An Instance For Our Window
    GetModuleFileName(hInstance, modulename, sizeof(modulename)-1);
    flook_init(modulename);

    /* Get display screen information & clear the screen.*/
    hConsoleOut = GetStdHandle( STD_OUTPUT_HANDLE );

    GetConsoleScreenBufferInfo( hConsoleOut, &csbiInfo );
    GetConsoleScreenBufferInfo( hConsoleOut, &csbiInfoO );

    SetConsoleTextAttribute(    hConsoleOut, BG_GRAY);
         
    ClearScreen();
    SetConsoleTextAttribute(hConsoleOut, BG_GRAY);

    main_(argc, argv);

 /* tcapclose(); */
  /*CloseHandle( hConsoleOut );*/
}

#if 0

int	cfcolor = C_WHITE;		/* current forground color */
int	cbcolor = C_BLACK;		/* current background color */
extern const short ctrans_[];		/* ansi to ibm color translation table */

/* editor variable: cmt_colour;		   comment colour in ibm */


#if	COLOR

void Pascal tcapbfcol(color)	/* set the current output color */
	unsigned int color;
{ cbcolor = ctrans_[color>>8];
  cfcolor = ctrans_[color & 0xf];
}
#endif
#endif


void Pascal setconsoletitle(char * title)

{ /*
  char consoletitle[100];
	if      (title == null || title[0] == 0)
    consoletitle[0] = 0;
  else
   strpcpy(&consoletitle[0], title, sizeof(consoletitle));
  */
  SetConsoleTitle( title );
}


// char * Pascal getconsoletitle()

// { return consoletitle;
// }



void Pascal tcapsetsize(int wid, int dpth)

{ int ct = 1;
  COORD size;
  size.X = wid;
  size.Y = dpth;
  
  SetConsoleScreenBufferSize( hConsoleOut, size);
  GetConsoleScreenBufferInfo( hConsoleOut, &csbiInfo );
	// set the screen buffer to be big enough

  while (--ct >= 0 && csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top < dpth)
  { int rc;
//  mlwrite("%d Ws %d Cp %d MWS %d dpth %d", ct, csbiInfo.dwSize.Y, 
//                                csbiInfo.dwCursorPosition.Y,
//                                csbiInfo.dwMaximumWindowSize.Y, dpth
//         );
    csbiInfo.srWindow.Top = 0;
    csbiInfo.srWindow.Bottom = dpth-1;
    csbiInfo.dwCursorPosition.Y = dpth - 1;
    
    rc = SetConsoleWindowInfo(hConsoleOut, 1, &csbiInfo.srWindow);
//  if (rc == 0)
//    tcapbeep();
//    mlwrite("Failed %d MWS %d %d", rc, csbiInfo.dwMaximumWindowSize.X, csbiInfo.dwMaximumWindowSize.Y);
  }
}


static Bool  cursor_on = true;
static COORD oldcur;
static WORD  oldattr = BG_GRAY;



Bool Pascal cursor_on_off(Bool on)

{ Bool res = cursor_on;
  cursor_on = on;
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
  
{ WORD   MyAttrib = row == term.t_mrowm1 ? BG_GRAY
					 : window_bgfg(curwp) | BACKGROUND_INTENSITY;
  DWORD  Dummy;
  COORD  Coords;
  ttrow = row;
  Coords.Y = ttrow;
  ttcol = col;
  Coords.X = ttcol;

  if (cursor_on)
  { WriteConsoleOutputAttribute( hConsoleOut, &oldattr, 1, oldcur, &Dummy );
    WriteConsoleOutputAttribute( hConsoleOut, &MyAttrib, 1, Coords, &Dummy );
   
    
    oldattr = attr;
    oldcur = Coords;
  }
  SetConsoleCursorPosition( hConsoleOut, Coords);
}}}



int Pascal millisleep(unsigned int n)

{ 
  Sleep((DWORD)(n >> 8));
  return OK;
}



int Pascal tcapbeeol(int row, int col) /* erase to the end of the line */

{
  DWORD     Dummy;
  COORD     Coords;
  Coords.Y = row;
  Coords.X = col;
  
  FillConsoleOutputCharacter(hConsoleOut, ' ',csbiInfo.dwSize.X-col,Coords,&Dummy );
  return OK;
}


void Pascal tcapeeol()

{ tcapbeeol(ttrow, ttcol);
}


void Pascal tcapeeop()

{ register int row;
/* awaitgap(); */
  for (row = term.t_nrowm1+1; --row >= 0; )
    tcapbeeol(row, 0);
}

int Pascal tcapcres(char * res) /* change screen resolution */

{ return OK;
}

#if 0

void Pascal tcapscreg(row1, row2)
     int row1, row2;
{ 
}

#endif


void Pascal ttscupdn(n)                  /* direction the window moves */
        int   n;   
{ COORD doo;
  SMALL_RECT rect;
  CHAR_INFO ci;
  rect.Right = csbiInfo.dwSize.X;
  rect.Bottom = csbiInfo.dwSize.Y;
  rect.Top = 0;
  rect.Left = 0;
  doo.Y = 0;
  doo.X = 0;
  
  if (n < 0)
    rect.Top += n;
  else 
  { rect.Bottom += n;
    doo.Y -= n;
  }

  ci.Char.AsciiChar = ' ';
  ci.Attributes = BG_GRAY;
    
  if (ScrollConsoleScreenBufferA( hConsoleOut, &rect, null, doo, &ci) == 0)
    tcapbeep();
}

//------------------------------------------------------------------------------

void Pascal ttputc(ch) /* put a character at the current position in the
		   		current colors */
	char ch;
{			/* if its a newline, we have to move the cursor */
  DWORD     Dummy;
  register int col;

/*GetConsoleScreenBufferInfo( hConsoleOut, &ccInfo );*/
  GetConsoleScreenBufferInfo( hConsoleOut, &csbiInfo );
  col = csbiInfo.dwCursorPosition.X;
/* ttcol = col;*/
  ttrow = csbiInfo.dwCursorPosition.Y;

  if (ch != '\n' && ch != '\r')
  { if (ch != '\b')
    { col += 1; 
      if (col >= csbiInfo.dwSize.X)
      { mbwrite("screen oflo");
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
  WriteConsoleOutputCharacter(hConsoleOut, &ch,1, csbiInfo.dwCursorPosition, &Dummy);
  csbiInfo.dwCursorPosition.X = col;
  csbiInfo.dwCursorPosition.Y = ttrow;
  SetConsoleCursorPosition( hConsoleOut, csbiInfo.dwCursorPosition);
/* rg.x.bx = cfcolor; */
}



int Pascal tcapbeep()

{ 
  Beep( 500, 250 /*millisecs*/);
  return OK;
}


void Pascal tcapopen()

{ screxist = TRUE;
  revexist = TRUE;
{ int plen = csbiInfo.srWindow.Bottom-csbiInfo.srWindow.Top+1;
  int pwid = csbiInfo.srWindow.Right -csbiInfo.srWindow.Left+1;
  if (plen > 25)
    newdims(pwid, plen);
  else				/* it is the default XP window: pick up LINES */
    tcapsetsize(term.t_ncol,term.t_mrowm1+1);
}}




int Pascal scwrite(row, outstr, color)	/* write a line out */
	int    row;		/* row of screen */
	short *outstr;		/* string to output (must be term.t_ncol long)*/
	int    color;		/* background, foreground */
{		    /* build the attribute byte and setup the screen pointer */
  char buf[149];
  WORD cuf[149];

	const int sclen = csbiInfo.dwSize.X >= 148 ? 148 : csbiInfo.dwSize.X;
  DWORD  Dummy;
  WORD attr = color;
  WORD attr_ = attr;

  COORD     Coords;

  register int col;

  Coords.Y = row;
  	   
  for (col = -1; ++col < sclen; )
  { cuf[col] = attr;
    if (outstr[col] & 0xff00)
    { 
      if (outstr[col] & 0x7f00)
      { attr_ = attr;		/* push */
        attr = (outstr[col] >> 8) & 0x7f;
				cuf[col] = attr;
      }
      else
      { attr = attr_;
      }
    }
    buf[col] = (outstr[col] & 0xff);
  }
  
  buf[++col] = 0;
  cuf[col] = 0;
  Coords.X = 0;
 /*loglog("TO %d %s", row, buf);*/
  WriteConsoleOutputCharacter( hConsoleOut, buf, sclen, Coords, &Dummy );
  WriteConsoleOutputAttribute( hConsoleOut, cuf, sclen, Coords, &Dummy);
  return OK;
}


#if	FLABEL
int Pascal fnclabel(f, n)	/* label a function key */
	int f,n;	/* default flag, numeric argument [unused] */
{		/* on machines with no function keys...don't bother */
  return TRUE;
}
#endif


