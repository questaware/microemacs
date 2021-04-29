#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include	"estruct.h"
#include	"edef.h"
#include	"etype.h"
#include	"elang.h"
#include	"map.h"
#include	"logmsg.h"


#if	S_UNIX5 | S_HPUX
#include	<time.h>
#endif
#if	S_BSD | S_XENIX
#include	<sys/types.h>
#include        <sys/select.h>
#include	<time.h>
#endif


extern serialopen();
extern serialclose();
extern ttgetraw();
int Pascal get1key();




#define C_WHITE 7 
#define C_BLACK 0

#define CHROM_OFF (unsigned)0x8000

#define MARGIN	8
#define SCRSIZ	64
#define NPAUSE	10			/* # times thru update to pause */
#define BEL	0x07


#define Char char

 extern	char *getenv();
 extern char sres[];		/* current screen resolution	*/

static char * mytgoto(char * cmd, int p1, int p2);

#define FTRACE 0
#if FTRACE
  static FILE * ftrace;
#endif

/*	Termcap Sequence definitions	*/

typedef struct TBIND
{	char  p_tiname[6]; /* terminfo name */
	short p_name;      /* sequence name, then used by sequence matching */
	short p_code;	   /* resulting keycode of sequence */
	char  p_seq[8];	   /* terminal escape sequence */
} TBIND;

			/* The numbers in column 1 are terminfo numbers
			   and are not fixed universally */
TBIND keytbl[] = {
#define BS_O 0
#if S_LINUX || S_VMS
{ "kbs",/* bspace*/        102,	CTRL | 'H', "\177"},
#else
error error
{ "kbs",/* bspace*/        102,	CTRL | 'H', "\007"},
#endif

#include "keys.h"

#if 0
{ "",	/* backtab */0,	SHFT | CTRL | 'i', ""},
{ "",/* num pad 5 */0,  SPEC | 'E',	"\033[E" },
{ "kind", /* scroll down */ 0,	CTRL | 'V', "\033[G"},
{ "",	/* scroll down */             0,  CTRL | 'Z',	""},
{ "",	/* clear screen */ 	      0,  CTRL | 'L',	""},
{ "",	/* delete line */	      0,  CTRL | 'K',	""}, 
#endif
};

#define	NTBINDS	sizeof(keytbl)/sizeof(TBIND)

typedef struct Scrcap_s
{	char  p_tiname[8]; /* terminfo name */
	char  p_seq[44];	   /* terminal escape sequence */
} Scrcap_t, *Scrcap;


static Scrcap_t  captbl[] = {
{ "cchrom", "0"},
#if TELNET
error error
{ "colors", "8" },
{ "cols", "106"},
{ "csr", "\033[%i%d;%dr" },
{ "cup", "\033[%i%p1%d;%p2%dH" },
{ "dl1", "\033[%p1%dM" },
{ "ed", "\033[J" },
{ "el", "\033[K" },
{ "il1", "\033[L" },
{ "ind", "\n" },
{ "is1", "" },
{ "kich1", "" },
{ "lines", SCR_LINES_STR},
#if OWN_BOLD
{ "rev", OWN_BOLD_VAL },
#else
{ "rev", "\033[7m" },
#endif
{ "ri", "\033M" },
{ "rs1", "\033[1;78r\033[41;0H\033[J" },
{ "sgr0", "\033[m" },
{ "ssf", "\033[M\033[%i%p1%dH\033[L" },
{ "ssr", "\033[%i%p1%dH\033[M\033[%p2%d;1H\033[L" },
#elif S_LINUX
error error
{ "colors", "8" },
{ "cols", "80"},
{ "csr", "\033[%i%d;%dr" },
{ "cup", "\033[%i%p1%d;%p2%dH" },
{ "dl1", "\033[%p1%dM" },
{ "ed", "\033[J" },
{ "el", "\033[K" },
{ "il1", "\033[L" },
{ "ind", "\033D" },
{ "is1", "\033[1;1H\033[30;47m" },
/*{ "is1", "" },*/
{ "kich1", "" },
{ "lines", SCR_LINES_STR},
#if OWN_BOLD
{ "rev", OWN_BOLD_VAL },
#else
{ "rev", "\033[7m" },
#endif
{ "ri", "\033M" },
{ "rs1", "\033>\033[1;3;4;5;6l\033[?7h\033[m\033[r\033[60H" },
/*{ "rs1", "" },*/
{ "sgr0", "\033[27m" },
{ "ssf", "\033[M\033[%i%p1%dH\033[L" },
{ "ssr", "\033[%i%p1%dH\033[M\033[%p2%d;1H\033[L" },
#elif 1
{ "colors", "8" },
{ "cols", "80"},/*132*/
{ "csr", "\033[%i%d;%dr" },
{ "cup", "\033[%i%p1%d;%p2%dH" },
{ "dl1", "\033[%p1%dM" },
{ "ed", "\033[J" },
{ "el", "\033[K" },
{ "il1", "\033[L" },
{ "ind", "\n" },
{ "is1", "\033[47;30m\033[8;8p" },
{ "kich1", "" },
{ "lines", SCR_LINES_STR},
{ "rev", "\033[7m" } /* no reverse */,
{ "ri", "\033M" },
{ "rsi", "\033[0m\033[7l\033[r\033[60H\033[8;0;83p"},
/*{ "rs1","\033>\033[1;3;4;5;6l\033[?7h\033[0m\033[r\033[60H\033c\033[?47l"
},*/
/*{"rs1", "\033>\033[1;3;4;5;6l\033[?7h\033[47;30m\033[r\033[60H" },*/
{ "sgr0", "\033[37;40m" },
/*{ "sgr0", "\033[47;30m" },*/
{ "ssf", "\033[M\033[%i%p1%dH\033[L" },
#if 1
{ "ssr", "\033[%i%p1%dH\033[M\033[%i%p2%d;1H\033[L" },
#else
{ "ssr", "\033[%i%p2%d;1H\033[L\033[%i%p1%dH\033[M" },
#endif
#else
{ "cols", "80"},
{ "colors", "8" },
{ "csr", "" },
{ "cup", "\033[%i%p1%d;%p2%dH" },
{ "dl1", "\033[%p1%dM" },
{ "ed", "\033[J" },
{ "el", "\033[K" },
{ "il1", "\033[L" },
{ "ind", "^J" },
{ "is1", "\033[4l" },
{ "kich1", "" },
{ "lines", SCR_LINES_STR},
{ "rev", "\033[7m" },
{ "ri", "\033M" },
{ "rs1", "\033>\033[m\033[r\033[60H" },
{ "sgr0", "\033[m" },
{ "ssf", "\033[M\033[%i%p1%dH\033[L" },
{ "ssr", "\033[%i%p1%dH\033[M\033[%p2%d;1H\033[L" },
#endif
};


#define K_CCHROM 0
#define K_COLORS 1
#define K_COLS   2
#define K_CSR    3
#define K_CUP    4
#define K_DL1    5
#define K_ED     6
#define K_EL     7
#define K_IL1    8
#define K_IND    9
#define K_IS1    10
#define K_KICH1  11
#define K_LINES  12
#define K_REV    13
#define K_RI     14
#define K_RS1    15
#define K_SGR0   16
#define K_SCR_F  17
#define K_SCR_R  18
#define NCCMDS (K_SCR_R+1)

int	cfcolor = C_WHITE;		/* current forground color */
int	cbcolor = C_BLACK;		/* current background color */
int	ctrans[] =		/* ansi to ibm color translation table */
	{0, 4, 2, 6, 1, 5, 3, 7,
	 8, 12, 10, 14, 9, 13, 11, 15};
/* editor variable cmt_colour;		   comment colour in ibm */


#define M_BOLD 1
#define M_REV  2 /* must be 2 */

  Char key_bspace;

static short tc_state;

short sctop = 0;
short scbot = SCR_LINES;  /* set by tcapopen */

short alarm_went_off = 0;

static void see_alarm(int n)

{ alarm_went_off = n;
  loginfo("Ring Ring");
  signal(SIGALRM, see_alarm);
}

/**********************************************************************/

/* Buffer Routines */
		/*  input buffers and pointers	*/

#define	IBUFSIZE  64			/* must be a power of 2 */

unsigned char in_buf[IBUFSIZE];	/* input character buffer */
int in_next = 0;		/* pos to retrieve next input character */
int in_last = 0;		/* pos to place most recent input character */



static in_init()	/* initialize the input buffer */

{ in_next = in_last = 0;
}


#define in_check() (in_next != in_last)


static in_put(int event)
                      	/* event to enter into the input buffer */
{ in_buf[in_last++] = event;
  in_last &= (IBUFSIZE - 1);
}


static int in_get()	/* get an event from the input buffer */

{ int event = in_buf[in_next++];
  in_next &= (IBUFSIZE - 1);
  return event;
}

/**********************************************************************/

void putpad(const char * str)		/* cheaper version */
      
{ const char * s = str;
  while (*s != 0)
    ttputc(*s++);
}

/**********************************************************************/

static Map_t keymap = mk_const_map(T_DOMCHAR0+6, 0, keytbl);
static Map_t capmap = mk_const_map(T_DOMCHAR0+6, 0, captbl);

static int first_width;
static int first_length;


int Pascal use_named_str(char * name, char * str)
			 
{ if (name != null)
	{ keymap.srch_key = name;
		capmap.srch_key = name;
	{ Vint ix = binary_const(&keymap, keytbl);
		if (ix >= OK)
			strpcpy(&keytbl[ix].p_seq[0], str, sizeof(keytbl[0].p_seq));

		ix = binary_const(&capmap, captbl);
		if (ix >= OK)
		{ strpcpy(&captbl[ix].p_seq[0], str, sizeof(captbl[0].p_seq));
		}
	}}
	
 /* strcat(&kseq[K_ED][0], kseq[K_SGR0]); */

	if (keytbl[BS_O].p_seq[0] == 0x7f and 
			keytbl[BS_O].p_seq[1] == 0)
		key_bspace = 0x7f;
	return true;
}


void Pascal restore_size()

{ if (term.t_ncol != first_width || term.t_nrowm1 + 1 != first_length)
		tcapsetsize(first_width, first_length);
}



/**********************************************************************/
ttselect()

{ putpad("\033[?47l");
}

/**********************************************************************/

ttopen(int wh)

{ /* if (pd_gflags & MD_NO_MMI)
			return; */
	const char * s;
#if USE_SCR_BDY == 0
	captbl[K_CSR].p_seq[0] = 0;
#endif
	
	serialopen();
	if (wh)
		putpad("\033[?47l");
	s = captbl[K_IS1].p_seq;
#if FTRACE
	if (ftrace != 0) fprintf(ftrace, "K_IS1: %s\n", s);
#endif
	putpad(s[0] == 0 ? "\033[m\033[2J" : s);

	if (captbl[K_CSR].p_seq[0] != 0)
	{ putpad(mytgoto(captbl[K_CSR].p_seq, sctop, scbot));
	}
 
#if FTRACE
	if (ftrace != 0) fprintf(ftrace, "K_KICH1: %s\n", captbl[K_KICH1].p_seq); 
#endif
	putpad(captbl[K_KICH1].p_seq);

	ttrow = 2;		 /* must be in range */
	ttcol = 2;
}


/**********************************************************************/

void Pascal tcap_init()

{ term.t_nrowm1 =
	term.t_mrowm1 = 47;
	term.t_ncol =
	term.t_mcol = 80;
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
}}

/**********************************************************************/

/*	Open the terminal
	put it in RA mode
	learn about the screen size
	read TERMCAP strings for function keys
*/

void Pascal tcapopen()

{
				NOSHARE char * term_type;

#if S_LINUX
	term_type = getenv("EMTERM");
	if (term_type == NULL)
#endif
		term_type = getenv("TERM");
	if (term_type == NULL)
	{ puts(TEXT182);
								 /* "Environment variable TERM not defined!" */
		exit(0);
	}

#if FTRACE
	if (ftrace == 0)
		ftrace = fopen("pjsbind", "w");
#endif
//			use_named_str(null, null);
	in_init();
				see_alarm(0);
}

			/* This function gets called just before we go
			 * back home to the command interpreter.	 */
tcapclose(int wh)

{ 		/* send end-of-keypad-transmit string if defined */
 /* if (pd_gflags & MD_NO_MMI)
		return; */
#if COLOR
	tcapchrom(0);
#endif
#if FTRACE
	if (ftrace != 0) fprintf(ftrace, "K_RS1 : %s\n", captbl[K_RS1].p_seq);
#endif
	tcapscreg(0, scbot+1);
	putpad(captbl[K_RS1].p_seq);
		 /* tcapmove(term.t_nrowm1, 0); */
	if (wh)
		putpad("\033[?47h");

	serialclose();
}

tcapkopen()

{
	strcpy(sres, "NORMAL");
}



/*	TCAPGETC: Get on character.  Resolve and setup all the
			appropriate keystroke escapes as defined in
			the comments at the beginning of input.c
*/

int Pascal ttgetc()

{ 			/* if there are already keys waiting.... send them */
	if (in_check())
		return in_get();
					/* otherwise... get the char for now */
			/*loginfo("GETKEY");*/
{ int c = get1key();
			/*loginfo2("GOTKEY %x %d", c, pd_kbdwr);*/

	if (CTRL & c) 		 /* unfold the control bit back into the character */
		c = (c & ~ CTRL) - '@';

		/* fold the event type into the input stream as an escape seq */
	if ((c & ~255) != 0)
	{ in_put(0);		/* keyboard escape prefix */
		in_put(c >> 8); 	/* event type */
		in_put(c & 255);	/* event code */
		return ttgetc();
	}

	return c;
}}


/*	GET1KEY:	Get one keystroke. The only prefixs legal here
			are the SPEC and CTRL prefixes.

	Note:

		Escape sequences that are generated by terminal function
		and cursor keys could be confused with the user typing
		the default META prefix followed by other chars... ie

		UPARROW  =	<ESC>A	 on some terminals...
		apropos  =	M-A

		The difference is determined by measuring the time between
		the input of the first and second character... if an <ESC>
		is types, and is not followed by another char in 1/30 of
		a second (think 300 baud) then it is a user input, otherwise
		it was generated by an escape sequence and should be SPECed.
*/


static int ecco_ct = 0;

#if FTRACE == 0
#define ecco(c) (c)
#else

int ecco(int c)

{ char buff[30];
	
	fprintf(ftrace,"k%d  = %x\n",++ecco_ct,c);
	return c;
}
#endif

/* This is an alternative algorithm for below:
	
	if (last_ch != 0)
	{ c = last_ch;
		last_ch = 0;
		return ecco(c);
	}
	c = ttgetraw();
	if (c != 27)
		return ecco(c);
	c = ttgetraw();
	if (c != '[' and c != 'O')
	{ last_ch = c;
		return ecco(CTRL | '[');
	}
*/

#define A_ESC 27

static short keybuf[8];
static short kbhd = 0;
static short kbtl = 0;

#define bgetk(c) \
	if (kbhd == kbtl) \
		c = ttgetraw(); \
	else \
	{ c = keybuf[kbhd]; \
		kbhd = (kbhd + 1) & 7; \
	}

#define bpushk(c) \
	{ keybuf[kbtl] = c; kbtl = (kbtl + 1) & 7; }


int Pascal get1key()

{	register int c; 		/* also index into termcap binding table */
					 short skbtl;
		 char cseq[10]; 	/* current sequence being parsed */
		 TBIND * btbl;

	bgetk(c);
	if (c != A_ESC)
		if (c == key_bspace)
			return ecco(CTRL | 'H');
		else	
			return ecco(c);

	bgetk(c);
/*			if (c == A_ESC)
		 return ecco(CTRL | META | '[');	*/
				
	if (c != 'O' and c != '[')					 /* it's not terminal generated */
	{ if (c != A_ESC)
		{ bpushk(c)
			return ecco(CTRL | '[');
		}
		else 
		{ alarm(1); 		/* it could be esc function key */
		{ int third_key;
			alarm_went_off = false;
			loginfo("Get wont get");
			bgetk(third_key);
			alarm(0);
			if (alarm_went_off)
			{ alarm_went_off = false;
				loginfo("alarm went off");
				return ecco(CTRL | META | '[');
			}
			bpushk(c);
			bpushk(third_key);
			return ecco(CTRL | '[');
		}}
	}

{ int mct = 0;
	int ix;
	cseq[0] = 
				cseq[2] = A_ESC;
	cseq[1] = c;
	if (c != '[')
		cseq[3] = '[';
	else
		cseq[3] = 'O';
	for (btbl = &keytbl[NTBINDS]; --btbl >= &keytbl[0];)
	{ btbl->p_name = false;
		if (*(short*)cseq == *(short*)(btbl->p_seq) or
				*(short*)&cseq[2] == *(short*)(btbl->p_seq))
		{ btbl->p_name = true;
			++mct;
		}
#if 0
		if (*(short*)(btbl->p_seq))
			fprintf(ftrace, "X%x %x ", *(short*)&cseq[2], *(short*)(btbl->p_seq));
#endif	
	}

#if FTRACE
				if (ftrace != 0)
					fprintf(ftrace, "MCT %d\n", mct);
#endif

	for (ix = 1; mct > 0 and ++ix <= 6; )
	{ bgetk(c);
		cseq[ix] = c;
#if FTRACE
		if (ftrace != 0)
			fprintf(ftrace, "STT %c ", c);
#endif
		for (btbl = &keytbl[NTBINDS]; --btbl >= &keytbl[0] and mct > 0;)
		{ if (btbl->p_name)
			{
#if FTRACE
				if (ftrace != 0)
					fprintf(ftrace, "MM %c %d\n", btbl->p_seq[ix], btbl-&keytbl[0]);
#endif
				if (btbl->p_seq[ix] != c)
				{ btbl->p_name = false;
#if FTRACE
					if (ftrace != 0)
						fprintf(ftrace, "FAILS\n");
#endif
					--mct;
				}
				else
				{	if (btbl->p_seq[ix+1] == 0)
					{
#if FTRACE
						if (ftrace != 0)
							fprintf(ftrace, "YES %d\n", btbl->p_code);
#endif
						return ecco(btbl->p_code);
					}
				}
			}
		}
	}
{ char * sp = &cseq[1];
	while (--ix >= 0)
		bpushk(*sp++);
	return ecco(CTRL | '[');
}}}

/*
Bool Pascal cursor_on_off(Bool on)

{ return !on;
}
*/

static char tgt_res[20];


static char * mytgoto(char * cmd, int p1, int p2)

{ int stack[8];
	int six = 1;
	register char * t = &tgt_res[0];
	register char * s = &cmd[0];
	stack[0] = p2;
	stack[1] = p1;

	while (*s != 0)
	{ if (*s != '%')
			*t++ = *s++;
		else
		{ ++s;
			switch (*s)
			{ case 'i': stack[0] += 1; p1 += 1;
									stack[1] += 1; p2 += 1;
				when 'p': ++s;
									stack[++six] = *s == '1' ? p1 :
									*s == '2' ? p2 : 0;
				when 'd': { int vv = stack[six--];
										int v = vv;
										if			(v >= 100)
										{ *t++ = '1';
											v -= 100; 	/* 200 chars max */
											*t++ = '0' + v / 10; /*copied yuk */
											v %= 10;
										}
										else if (v >= 10)
										{ *t++ = '0' + v / 10;
											v %= 10;
										}
										if (vv > 0)
											*t++ = '0' + v;
									}
			}
			++s;
		}
	}
	*t = 0;
	return &tgt_res[0];
}



void tcapmove(int row, int col)
{
#if 1
	if (col == 0 and row == ttrow + 1 /* and row < scbot*/) 
		putpad("\r\n");
	else
#endif
	if			(row == ttrow and col == ttcol)
		; 
	else if (row == ttrow and col+1 == ttcol)
		putpad("\010");
/* else if (row == 0 && col == 0)
		putpad("033[;H");*/
	else
		putpad(mytgoto(captbl[K_CUP].p_seq, row, col));
	ttrow = row;				/* display should know anyway */
	ttcol = col;	
}




void tcapmoveline(int row)
{
#if 1
	if (row == ttrow + 1 /* and row < scbot*/) 
		putpad("\n");
	else
#endif
	if			(row == ttrow)
		; 
	else
		putpad(mytgoto(captbl[K_CUP].p_seq, row, ttcol));
	ttrow = row;				/* display should know anyway */
}




void Pascal tcapscreg(int row1, int row2)
{
	if (sctop != row1 || scbot != row2)
	{ sctop = row1;
		scbot = row2;
		if (captbl[K_CSR].p_seq[0] != 0)
			putpad(mytgoto(captbl[K_CSR].p_seq, row1, row2));
	}
}



void Pascal tcapsetsize(int width, int length)

{ char buf[40];
	sprintf(buf, "\033[8;%d;%dt", length, width);
	putpad(buf);
	if (first_width == 0)
	{ first_width = width;
		first_length = length;
	}
}



void Pascal tcapeeol()

{ putpad(captbl[K_EL].p_seq);
/*tcapbeep();*/
}


void Pascal tcapepage()

{ tcapmove(0, 0);
	putpad(captbl[K_ED].p_seq);
/*millisleep(10);*/
}


tcaprev(int state)		/* change reverse video status */
	 /* state;		** TRUE => reverse video */
{ char * seq = state ? captbl[K_REV].p_seq : captbl[K_SGR0].p_seq;
	if (!state)
		tc_state &= ~M_REV;
	else
	{ if (tc_state & M_BOLD)
			putpad(captbl[K_SGR0].p_seq);
		tc_state |= M_REV;
	}

	if (seq != NULL)
		putpad(seq);
	tc_state &= ~M_BOLD;
}


spal(int dummy) /* change palette string */

{
	/*	Does nothing here */
}

#if COLOR
void Pascal tcapbfcol(unsigned int color) /* no colors here, ignore this */

{
	tcapchrom((color | 0x80) << 8);
}
#endif

tccadb()

{
	/*putc('T', stderr);*/
}



tcapchrom(short chroms)

{ static char chromattrs[20] = "\033[";
	static short last_chroms;
	static short lastl_chroms;
	static short last_was_chrom;
	register int tix = 2;
					 short chm = (chroms >> 8) & 0x7f;	/* background,foreground */
					 short chmbg = ((chm & 0x70)>> 4);
	int pix;

	if (chroms != last_chroms)
	{ pix = last_was_chrom;
		last_was_chrom = 0;

		while (true)	/* once only */

		{ if (chm == 0 || (chroms & CHROM_OFF) || chm == C_WHITE * 256 + C_BLACK)
			{ if (pix)
				{ chm = !(last_chroms & CHROM_OFF) and 
								(lastl_chroms & 0x7700) ? (lastl_chroms >> 8) & 0x7f :
								                          0x7;		/* background,foreground */
					last_was_chrom = 1; /* select 0 */
				}
				else
				{
#if REVERSE_VIDEO
					strcpy(&chromattrs[2], &"27"[~(tc_state & M_REV)]);
#else
					strcpy(&chromattrs[2], &"27"[(tc_state & M_REV)]);
#endif
					tix += tc_state & M_REV;
					break;
				}
			}

			if (((chm & 0x70)>> 4) == (chm & 0x7))
			{ chm = 0x70;
				tccadb();
			}

/*		if ((chm & 7) == 0 && chmbg == 7)
				;
			else */
			
#if REVERSE_VIDEO
			sprintf(&chromattrs[2],"4%c;3%c", '0'+((chm & 0x70)>> 4),'0'+(chm & 0x7));
#else
			sprintf(&chromattrs[2],"3%c;4%c", '0'+((chm & 0x70)>> 4),'0'+(chm & 0x7));
#endif
			tix += 5;
			last_was_chrom ^= 1;
			break;
		}
		chromattrs[tix++] = 'm';
		chromattrs[tix] = 0;
		putpad(chromattrs);
		lastl_chroms = last_chroms;
		last_chroms = chroms;
	}
}


tcapbeep()
{ 
	ttputc(BEL);
}

addb(int n)

{ ttputc(BEL);
}

			/* only 1 at present */ /* UP = window UP, text DOWN */
void Pascal ttscupdn(int n)

{ 
	if (n > 0)					/* reverse */
	{
		if (captbl[K_CSR].p_seq[0] != 0)
		{ tcapmove(sctop, 0);
			putpad(captbl[K_RI].p_seq);
/*
		tcapmove(scbot,0);
		tcapeeol();
*/
		}
		else
#if USE_SCR_BDY
			putpad(mytgoto(captbl[K_SCR_R].p_seq, scbot, sctop));
#else
		{ tcapmove(scbot,0);
			putpad(mytgoto(captbl[K_DL1].p_seq, 1, 0)); 	/* delete line*/
			tcapmove(sctop,0);
			putpad(captbl[K_IL1].p_seq);				/* insert line*/
		}
#endif
	}
	else						/* forward */
	{/*tcapbeep();*/
		if (captbl[K_CSR].p_seq[0] != 0)
		{ tcapmove(scbot, 0);
			putpad(captbl[K_IND].p_seq);
			putpad(captbl[K_EL].p_seq);
			/*tcapbeep();*/
		}
		else
		{ tcapmove(sctop,0);
#if USE_SCR_BDY
			putpad(mytgoto(captbl[K_SCR_F].p_seq, scbot, scbot));
#else
			putpad(mytgoto(captbl[K_DL1].p_seq, 1, 0)); 	/* delete line*/
			tcapmove(scbot,0);
			putpad(captbl[K_IL1].p_seq);				/* insert line*/
#endif
		}
	}
}

#if FLABEL
fnclabel(f, n)		/* label a function key */

int f,n;	/* default flag, numeric argument [unused] */

{
	/* on machines with no function keys...don't bother */
	return TRUE;
}
#endif



