//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "bemacs.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#include    <stdio.h>
#include    <stdlib.h>
#include    <VCL/sysutils.hpp>

#include	"estruct.h"
#define in_bemacs
#include	"edef.h"
#include	"etype.h"
#include    "epredef.h"
#include	"elang.h"
extern "C" {
#include "logmsg.h"
};
#include "keyboard.h"
using namespace Keyboard;

extern LPSTR wm_param;
extern void SemLock(void);
extern void SemUnlock(void);

#define BG_COLOUR (clGrayText+0x300000)
#define BG_INVERSE clWhite


static short sctop = 0;		/* zero based */
static short scbot = 25;  	/* zero based */ /* set by tcapopen */


TColor	cfcolor = clBlack;		/* current forground color */
TColor	cbcolor = clLtGray;		/* current background color */

const TColor ctrans[] =		/* ansi to Win32 color translation table */
	{clBlack, clRed, clBlue, clGreen, clYellow, clPurple, clAqua, clWhite,
	 /*8, 12, 10, 14, 9, 13, 11, 15*/};

/* editor variable: cmt_colour;		   comment colour in ibm */

//---------------------------------------------------------------------------
TForm1 *Form1;

//---------------------------------------------------------------------------

static char consoletitle[120];

void Pascal NEAR setconsoletitle(char * title)

{ if      (title == null or title[0] == 0)
    consoletitle[0] = 0;
  else if (consoletitle[0] == 0)
   strpcpy(&consoletitle[0], title, sizeof(consoletitle));
  else
   return;

 /* SetConsoleTitle( consoletitle ); */
}


char * Pascal NEAR getconsoletitle()

{ return consoletitle;
}



void Pascal NEAR tcapsetsize(int wid, int dpth)

{ 
/*
  COORD size;
  size.X = wid;
  size.Y = dpth;
  
  SetConsoleScreenBufferSize( hConsoleOut, size);*/
}


int Pascal NEAR tcapbeep(void)

{ Beep();
}


void Pascal NEAR tcapopen()

{
  if (Form1 != NULL)
  { scbot = Form1->noo_row - 1;

    newdims(Form1->noo_col, scbot+1);
  }
}


int Pascal NEAR tcapclose()

{ return OK;
}


int Pascal NEAR tcapcres(char * res) /* change screen resolution */

{
  return TRUE;
}



#if	COLOR

void Pascal NEAR tcapbfcol(unsigned int color)/* set the current output color */

{ /*cbcolor = ctrans[(color>>8) & 7];
  cfcolor = ctrans[(color & 0xf) & 7];*/
}
#endif


void Pascal NEAR tcapscreg(int row1, int row2)

{ sctop = row1;
  scbot = row2;
}



void Pascal NEAR tcapmove(int row, int col)

{ if (Form1 != NULL)
  { Form1->textrow = row;
    Form1->textcol = col;
  }
}



void Pascal NEAR ttscupdn(int sdist)          /* direction the window moves */

{ if (Form1 != NULL)
  { Form1->Scroll(sdist);
  }
}



void __fastcall TForm1::Scroll(int sdist)

{ int from = 0;

#if 0
  char keyString[30];
  sprintf(&keyString[0], "SCROLL %d %d", sdist, Row);
  Application->MessageBox(keyString , "Message Box", MB_OK);
#endif

  int rowct = Height / y_step;

  int tgt_ = (sdist < 0 ? 1 - sdist : 1) + from;
  int src_ = (sdist < 0 ? 1         : 1 + sdist) + from;
  int len_ = (sdist < 0 ? rowct + sdist -1
                        : rowct - sdist -1) - from;
 /*MemoWrite(String("D ")+ celldpth +"."+ tgt_ +":"+ src_ +"="+ len_);*/

  int tgt = tgt_;
  int src = src_;

  TRect torect;
  torect.Top = 4 + tgt * y_step;
  torect.Left = 4;
  torect.Right =torect.Left + Width - 16;
  torect.Bottom = 4 + (tgt + len_) * y_step;

  TRect fmrect;
  fmrect.Top = 4 + src * y_step;
  fmrect.Left = 4;
  fmrect.Right =fmrect.Left + Width - 16;
  fmrect.Bottom = 4 + (tgt + len_) * y_step;
  
  Canvas->CopyRect(torect, this->Canvas, fmrect);
}


int Pascal NEAR millisleep(unsigned int n)

{ return OK;
}




int Pascal NEAR tcapbeeol(int row, int col)	/* erase to the end of the line */

{ if (Form1 == NULL)
    return -1;

  Form1->EEol(row, col);
  return OK;
}



void Pascal NEAR tcapeeol()	/* erase to the end of the line */

{ tcapbeeol(ttrow, ttcol);
}

//---------------------------------------------------------------------------
int argc = 0;
char * * argv;

int callmain(LPSTR param_)

{ AnsiString param = param_;
  char * src_ = (char*)malloc(param.Length()+1);
  bool inspace = true;

  strcpy(&src_[0], param.c_str());
  argc = 1;

  char * src;
  for (src = src_; *src != 0; ++src)
  { if (*src > ' ')
    { if (inspace)
        ++argc;
      inspace = false;
    }
    else
    { inspace = true;
      *src = 0;
    }
  }

  argv = (char**)calloc(sizeof(char *), argc + 1);
  argv[0] = strdup("emacs");
  int aix = 0;

  for (src = src_; *src != 0; ++src)
  { if (*src > ' ')
    { if (!inspace)
      { argv[++aix] = src;
      }
      inspace = false;
    }
    else
      inspace = true;
  }

  return OK;
}

//---------------------------------------------------------------------------

static int Pascal NEAR usehost(wh, line)
	char 	 wh;
	char *   line;
{
  int cc;
  if (!clexec)
    TTmove(term.t_nrowm1, 0);           /* Seek to last line.   */
/*  mlwrite(""); */
  tcapclose();
  ttputc('\n');
  /* write(0, "\n", 1);*/
  sgarbf = TRUE;
#if MOUSE == 0
  return system(line);
    /* shellprog(wh, line);	** wrapper to execprg() */
#else
  /*if (wh == '!')*/
  cc = shellprog(wh, line);	/* wrapper to execprg() */
#if MOUSE
  ttopen();
#endif
  return cc;
#endif
}


static Pascal NEAR gen_spawn(f, n, prompt)
	char *   prompt;
{          char line [150];
  register int s;

  if (restflag)
    return resterr();
  
  s=mlreply(prompt, line, NLINE);
  if (s != TRUE)
    return s;

  ttputc('\r'); /* wont work!*/

  s = usehost(*prompt, line);

  ttcol = 0;  		   /* otherwise we shall not see the message */
			   /* if we are interactive, pause here */
  if (!clexec)
  { mlwrite(s ? TEXT188 : "[Failed]");
/*             "[End]" */
    while ((s = tgetc()) != '\r' && s != ' ')
      ;
    mlerase();
  }
  return TRUE;
#undef line
}

	/*
	 * Create a subjob with a copy of the command intrepreter. When the
	 * command interpreter exits, mark the screen as garbage so that you 
	 * do a full repaint. Bound to "^X C".
	 */
Pascal NEAR spawncli(int f, int n)

{	if (restflag)		/* don't allow this command if restricted */
	  return resterr();

        usehost('!', "");
        return TRUE;
}

	 /* Run a one-liner in a subjob. When the command returns, wait for 
	  * a single character to be typed, then mark the screen as garbage 
	  * so a full repaint is done. Bound to "C-X !".
	  */
Pascal NEAR spawn(int f, int n)

{ return gen_spawn(f, n, "!");
}

	/* Run an external program with arguments. When it returns, wait 
	 * for a single character to be typed, then mark the screen as 
	 * garbage so a full repaint is done. Bound to "C-X $".
	 */
Pascal NEAR execprg(int f, int n)

{ return gen_spawn(f, n, "$");
}

//---------------------------------------------------------------------------

        char prompt[2];

	/* Pipe a one line command into a window
	 * Bound to ^X @ or ^X #
	 */
static Pascal NEAR pipefilter(char wh)

{
 static const char bname [] = "_command";

 static const char filnam1[] = "fltinp";
 static       char filna2[] = " <fltinp >fltout";
#define filnam2 &filna2[10]

        char tmpnam_[128];
 extern char deltaf[HICHAR];
        char line[NSTRING+1];       /* command line send to shell */

	if (restflag)		/* don't allow this command if restricted */
	  return resterr();
	prompt[0] = wh;
{	int s;
	const char * tmpnam = wh == 0 ? &tmpnam_[2] : filnam2;
	wh -= '@';
	if (wh != 0 and (curbp->b_flag & MDVIEW)) /* disallow this command if */
	  return rdonly();			  /* we are in read only mode */

        if (mlreply(prompt, line, NLINE) != TRUE)
          return FALSE;

	if (wh == 0)
	{				/* get the command to pipe in */
			/* get rid of the command output buffer if it exists */
	  BUFFER * bp = bfind(bname, FALSE, 0);
          if (bp != null) 	    /* try to make sure we are off screen */
	  { BUFFER * sbp = curbp;
	    s = orwindmode(0, 1);
	    curbp = sbp;
	    if (s > 0)
	      onlywind(FALSE, 1);
				   /* get rid of the existing command buffer */
	    if (zotbuf(bp) != TRUE)
	      return FALSE;
	  }
	  concat(&tmpnam_[0], " >", fixnull(getenv("TMP")), "\\", bname, null);
          strcat(&line[0], tmpnam_);
	}
	else				/* setup the proper file names */
	{ if (writeout(filnam1) != TRUE)/* write it out, checking for errors */
	  { mlwrite(TEXT2);
   					/* "[Cannot write filter file]" */
	    return FALSE;
	  }
          strcat(&line[0], filna2);
	}
	TTmove(term.t_nrowm1, 0);
        s = usehost('!', line);
			 	       /* did the output file get generated? */
{/* 	int fid = open(tmpnam, O_RDONLY);
	if (fid < 0)
	  return FALSE;
	close(fid);
*/
	if (wh == 0)
	{ if (splitwind(FALSE, 1) == FALSE)
	    return FALSE;

        { BUFFER * bp = bufflink(&tmpnam_[2], clexec);	/* and read the stuff in */
	  if (bp == NULL)
	    return FALSE;
	  
	  swbuffer(bp);
       /* linstr(tmpnam); */
		      /* make this window in VIEW mode, update all mode lines */
	  curwp->w_bufp->b_flag |= MDVIEW;
	  (void)orwindmode(WFMODE, 0);
					/* and get rid of the temporary file */
	  s = TRUE;
	}}
	else				/* on failure, escape gracefully */	
	{ char * sfn = curbp->b_fname;
	  curbp->b_fname = null;
	  s = readin(filnam2, FALSE);
          curbp->b_fname = sfn;		/* restore name */
	  if (s == FALSE)
	  { mlwrite(TEXT3);
		/* "[Execution failed]" */
	  }
	  else						
	    curbp->b_flag |= BFCHG;	/* flag it as changed */

	  unlink(filnam1);		/* and get rid of the temporary file */
	}
	unlink(tmpnam);
	return s != FALSE;
}}}


	/* Pipe a one line command into a window
	 * Bound to ^X @
	 */
Pascal NEAR pipecmd(int f, int n)

{ return pipefilter('@');
}

	/*
	 * filter a buffer through an external DOS program
	 * Bound to ^X #
	 */
Pascal NEAR filter(int f, int n)

{ return pipefilter('#');
}

//---------------------------------------------------------------------------

__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{

   log_init("emacs.log", 400000, 0);
   loglog("***************Started***************");

   this->OnPaint = this->MyPaint;

   in_paint = false;

   hrect.Top = 4;
   hrect.Left = 4;
   hrect.Right = hrect.Left + this->Width - 16;
   hrect.Bottom = this->Height - 60;

   textrow = 0;
   textcol = 0;
   y_row = 0;
   y_coord = 0;

   this->OnKeyDown = this->DoKeyDown;
   AnsiString fname = "Courier";
   Canvas->Font->Name = fname;
   y_step = Canvas->TextHeight("A");
   x_step = Canvas->TextWidth("A");

   noo_row = this->Height / y_step;
   noo_col = this->Width / x_step;

/* AnsiString params = wm_param;*/
   Caption = wm_param;
   callmain(wm_param);

   for (int ix = upper_index(scrsrc)+1; --ix >= 0; )
   { scrsrc[ix] = 0;
     scrbackground[ix] = (TColor)(-1);
   }
   
   vtinit();
   edinit();               /* Buffers, windows */
#if CALLED
   varinit();              /* user variables */
#endif
#if DIACRIT
   initchars();            /* character set definitions */
#endif
                                /* Process command line and let the */
    
#if S_MSDOS
    tcapopen();             /* open the screen AGAIN ! */
#endif
    (void)dcline(argc, argv);
    ttopen();
    tcapopen();
    tcapkopen();            /* open the keyboard */
    tcaprev(FALSE);
    this->OnShow = TForm1::Update;
    loglog1("Curbp %s", curbp->b_bname);
    update(FALSE);		/* Fix up the screen	*/
}


void __fastcall TForm1::Update(TObject* sdr)

{ update(false);
}



//---------------------------------------------------------------------------
void __fastcall TForm1::MyPaint(TObject* sdr)
{ Paint();
}


//---------------------------------------------------------------------------
void __fastcall TForm1::Paint()
{
  if (in_paint)
    return;

  in_paint = true;

  TForm::Paint();

  TColor sc = Canvas->Brush->Color;

  Canvas->Brush->Color = cbcolor;
  Canvas->FillRect(hrect);

  loglog("Paint");

  Canvas->Brush->Color = sc;

  short * src;
  int ssgarbf = sgarbf;
  sgarbf = true;

  update(true);
  sgarbf = ssgarbf;
/*
  for (int row = -1; ++row <= upper_index(scrsrc) && (src = scrsrc[row])!= NULL; )
  { PaintRow(row, src);
  }
*/
}


//---------------------------------------------------------------------------


void Pascal NEAR tcapeeop()

{ if (Form1 != NULL)
  { Form1->EEop();
  }
}



void __fastcall TForm1::EEop()

{ TRect eeoprect;
  eeoprect.Top = 10;
  eeoprect.Left = 2;
  eeoprect.Right = Width - 12;
  eeoprect.Bottom = this->Height - 60;

  TColor sc = Canvas->Brush->Color;
  Canvas->Brush->Color = cbcolor;

  Canvas->FillRect(eeoprect);
  loglog("Eop");
  Canvas->Brush->Color = sc;
}


void __fastcall TForm1::EEol(int myrow, int mycol)

{ if (myrow >= noo_row || mycol >= noo_col)
    return;

  eeolrect.Top = 10 + myrow * y_step;
  eeolrect.Left = 2 + mycol * x_step;
  eeolrect.Right = Width - 12;
  eeolrect.Bottom = eeolrect.Top + y_step;

  TColor sc = Canvas->Brush->Color;
  Canvas->Brush->Color = scrbackground[myrow];

  loglog2("Eol %d %d", myrow, mycol);

  Canvas->FillRect(eeolrect);
  Canvas->Brush->Color = sc;
}



void __fastcall TForm1::PaintRow(int myrow, const short * src,
							int xstt, int xlim)
{ /* mycol not yet implemented */
  TColor sc = Canvas->Brush->Color;
  TColor sf = Canvas->Font->Color;

  Canvas->Brush->Color = cbcolor;
  Canvas->Font->Color = cfcolor;

  int x = hrect.Left + xstt * x_step;
  int y = myrow * y_step;
  int h = 0;
  int pause = 0;
  TColor nxtclr = clBlack;

  int color = scrbackground[myrow];
  unsigned int attr = color << 8;
  unsigned int attr_ = attr;

  AnsiString str = "";
  int lim = xlim < term.t_ncol ? xlim : term.t_ncol;

  for (int mycol = xstt; ++mycol <= lim; ++src)
  { if (mycol == lim)
    { pause = 2;
      if (textrow == myrow && textcol == mycol)
        pause = 6;
    }
    else if (textrow == myrow && textcol == mycol)
      pause = 4;
    else
    { if (*src & 0x7f00)
      { attr_ = attr;		/* push */
        attr = *src & 0x7f00;
        nxtclr = ctrans[(attr>>12)&7];
        pause = 1;
      }
      str += (char)(*src & 0xff);
      if (*src & 0x8000)	/* pop */
      { attr = attr_;
        nxtclr = ctrans[(attr>>12)&7];
        pause = 1;
      }
    }
    if (pause)
    { /*loglog3("TO %d %d %s", x, y, str.c_str());*/
      if (str != "")
      { Canvas->TextOut (x, y, str);
        x += Canvas->TextWidth(str);
      }
      int xx = x;
      str = "";
      if (pause & 4)
      { if (pause == 4)
        { /*Canvas->Font->Color = clWhite;*/
          Canvas->Brush->Color = BG_INVERSE;
          str = (char)(*src & 0xff);
          Canvas->TextOut (x, y, str);
          xx += Canvas->TextWidth(str);
          str = "";
        }

        Canvas->Brush->Color = cbcolor;
      }
      pause = 0;
      x = xx;
    }
/*  Canvas->Font->Color = nxtclr; */
  } /* for */

  Canvas->Brush->Color = sc;
  Canvas->Font->Color = sf;

  in_paint = false;
}


/* modeline put character
 */
Pascal NEAR ttputc(char c)

{ short buf[2];
  buf[0] = c;
  buf[1] = 0;
  Form1->PaintRow(ttrow, static_cast<const short*>(buf), ttcol);
}



int Pascal NEAR scwrite(row, outstr, color)	/* write a line out */
	int    row;	/* row of screen to place outstr on */
	short *outstr;	/* string to write out (must be term.t_ncol long) */
	int    color;
{		    /* build the attribute byte and setup the screen pointer */
  if (Form1 == NULL)
    return -1;

  /*loglog1("SCWRITE %d", row);*/

  Form1->scrbackground[row] = ctrans[(color >> 4) & 7];
  Form1->PaintRow(row, outstr);
  return OK;
}

//---------------------------------------------------------------------------

void MySetCoMo()

{
#if 0
  hConsoleIn = GetStdHandle( STD_INPUT_HANDLE );
  if (hConsoleIn == INVALID_HANDLE_VALUE)
  { DWORD ec = GetLastError();
    mlwrite("Pipeerr %d", ec);
    Beep(1200, 2000);
  }

#if 0
  if (0 == SetConsoleMode(hConsoleIn, ENABLE_WINDOW_INPUT/*|ENABLE_PROCESSED_INPUT*/))
  { DWORD ec = GetLastError();
    mlwrite("PipeC %d", ec);
    Beep(1200, 2000);
  }
#endif
  SetConsoleCtrlHandler(MyHandlerRoutine, true);
#endif
}


bool in_modal = false;

#define	IBUFSIZE  512		/* must be a power of 2 */

unsigned char in_buf[IBUFSIZE];	/* input character buffer */
Short in_next = 0;		/* pos to retrieve next input character */
Short in_last = 0;		/* pos to place most recent input character */


#define in_init() in_next = in_last = 0   /* initialize the input buffer */

#define in_check() (in_next != in_last)   /* is the input buffer non-empty? */


void Pascal in_put(event)
	int event;	/* event to enter into the input buffer */
{ in_buf[in_last & (IBUFSIZE - 1)] = event;
  in_last += 1;
}


#define in_get() in_buf[in_next++ & (IBUFSIZE - 1)]
				/* get an event from the input buffer */


void __fastcall TForm1::DoKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
   WORD k = Key;
   loglog1("Key %d", k);

   if (in_modal)	/* not STRICTLY atomic but who cares? */
   { in_put((int)k);
     SemUnlock();
   }
   else
   { /*in_put(k);*/
     lastflag = 0;		/* Fake last flags.*/
	   			/* execute the "command" macro...normally null*/
     execkey(&cmdhook, FALSE, 1);	/* used to push/pop lastflag */
		
     update(FALSE);		/* Fix up the screen	*/

     editloop(k);
   }
}
//---------------------------------------------------------------------------


static int  eaten_char = -1;		 /* Re-eaten char */


void Pascal NEAR reeat(int ch)

{
    eaten_char = ch;			/* save the char for later */
}



Pascal NEAR ttgetc()

{ if (eaten_char != -1)
  { int ch = eaten_char;
    eaten_char = -1;
    return ch;
  }
  
  if (!in_check())
  { in_modal = true;
    SemLock();
    in_modal = false;
  }

  return in_get();
}


#if	TYPEAH
	/* typahead:	See if any characters are already in the keyboard buffer
	*/
Pascal NEAR typahead()

{ return FALSE;
}

#endif


