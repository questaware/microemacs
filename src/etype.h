/*	ETYPE:		Global function type definitions for
			MicroEMACS 3.9

                        written by Daniel Lawrence
                        based on code by Dave G. Conroy,
                        	Steve Wilhite and George Jones
*/

#if S_WIN32

#define WinChar wchar_t
#else
#define WinChar void
#endif

#define USE_FLOAT 1

#define MYFLOAT long double

											/* ALL global function declarations */
extern int rubbish;

Char * ClipAlloc(int);
Char * ClipRef(void);
Cc ClipSet(int wh);
char * ClipPasteStart(void);
void ClipPasteEnd(void);

/*extern int atoi(char * s);*/

#if S_MSDOS == 0
int millisleep(unsigned int n);
#endif
char * USE_FAST_CALL mallocz(int n);
void USE_FAST_CALL adb(int n);
int Pascal arith(int, int);
char * Pascal allocate(unsigned nbytes);
char * Pascal callocate(unsigned nbytes);
int Pascal calculator(int, int);
//int count_work(void);
//void Pascal customise_buf(BUFFER * bp);
char * Pascal duplicate(const char * src);
void cls(void);
BUFFER *Pascal bfind(const char * bname, int cflag);
int  USE_FAST_CALL ectoc(int c);
int Pascal getIncls(int f, int n);
BUFFER * getcbuf(int createflag, BUFFER * bp, const char *prompt);
BUFFER * Pascal bufflkup(const char * filename);
BUFFER *Pascal bufflink(const char *, int);
char * Pascal repl_bfname(BUFFER*, const char *);
UNDO * run_make(LINE * ln);
void run_trim(BUFFER * bp, int lim);
char *Pascal bytecopy();
Cc do_ftime(BUFFER * bp,
#if S_WIN32 == 0
						FILE * ffp,
#endif
						Bool update);
char *Pascal envval();
//void Pascal expandp(char *, char *, char *, int);
//const char * USE_FAST_CALL fixnull(const char * s);
int USE_FAST_CALL find_nch(int wh, int cix, const LINE * lp);
time_t ffiletime(FILE * ffp);
FILE* Pascal ffropen(const char * fn);
#if S_MSDOS
_CRTIMP long double __cdecl atold(const char *);
#endif
char * float_asc(MYFLOAT x);
const char * Pascal flook(char, const char *);
char * Pascal flookdown(char *, char *);
char *Pascal flooknear();
char *Pascal funval();
char *Pascal getctext(char *);
const char *Pascal getfname(int);
char *Pascal getkill(void);
const char *Pascal getreg(char * t);
extern const char getvalnull[];
const char *Pascal getval(char *, const char *);

char * gtfilename(int wh);
const char *Pascal gtusr(char * vname);			/* look up a user var's value */
char *Pascal ilook();
void  Pascal ibefore(LINE*, LINE*);
void init_fncmatch(void);
char *Pascal USE_FAST_CALL int_asc(int);
char * Pascal USE_FAST_CALL int_radix_asc(int i, int r, char fill);
int kinsstr(const char * s, int len, int bno);
//int Pascal makename(char *, const char *);
char * mkTempCommName(char suffix, /*out*/char *filename);
char *Pascal mkkey(const char *);
char *Pascal mkul(int, char *);
char *Pascal namval();
BUFFER * Pascal prevele(int, BUFFER*);
char *Pascal timeset();
char *Pascal token(char * tok, int size);
int to_kill_buff(int wh, int n);
const char *Pascal transbind(char *);
int Pascal USE_FAST_CALL trimstr(int from, char * s);
int (Pascal *Pascal USE_FAST_CALL fncmatch(const char *))(int, int);
Command getname(int wh);
char * Pascal getconsoletitle();
int Pascal desfunc(int,int);
int Pascal listvars(int, int);
int Pascal dispvar(int,int);
int USE_FAST_CALL myeq(int, int);
int Pascal ernd(void);
int Pascal name_mode(const char *);
#if S_BSD | S_UNIX5 | S_XENIX | S_SUN | S_HPUX
 int Pascal fexist(const char *);
#else
#define fexist(fn) (name_mode(fn) >= 0)
#endif
char * fex_up_dirs(const char * dir, const char * file);
void Pascal flook_init(char *);
int Pascal fisearch(int, int);
int Pascal getkey();
const char * gettmpfn(void);
int Pascal gettyp(char *);
int Pascal getwpos();
void init_wincon();
int Pascal mkdes();
int Pascal nmlze_fname(char *, const char *, char *);
void Pascal openwind(WINDOW *);
void openwindbuf(char *);
int Pascal orwindmode(int);
char * Pascal pathcat(char *, int, const char *, const char *);
#if S_MSDOS
unsigned short USE_FAST_CALL refresh_colour(int row, int col);
#endif
void Pascal release(char * mp);
int reload_buffers(void);
int Pascal remmark(int, int);
int Pascal risearch(int, int);
void Pascal rpl_all(int, int, int, LINE*, LINE*);
int Pascal USE_FAST_CALL scan_for_sl(LINE * lp);
int Pascal scanner(int, int);
#if VS_CHAR8 && DEBUG == 0
#define setconsoletitle(t) SetConsoleTitle(t)
#else
void Pascal setconsoletitle(char * title);
#endif
int Pascal setlower(char*,char*);
int Pascal setupper(char*,char*);
int Pascal setvar(int, int);
int Pascal set_var(char *, const char *);
int Pascal shiftkill(int f, int n);
int USE_FAST_CALL stol(const char * s);
int Pascal svar(int var, const char * value);
int Pascal trim_white(int, int);
void Pascal tcap_init();
void tcapsetfgbg(int chrom);
void Pascal tcapsetsize(int wid, int dpth);
int Pascal tgetc();
int Pascal topluct();
int Pascal uneat();
LINE * mk_line(const char *, int, int, int);
LINE *Pascal mouseline();
int Pascal apro(int, int);
int ask_region();
int Pascal backchar(int, int);
#define backbychar(x) backchar(x,x)
int Pascal backdel(int, int);
int Pascal backhunt(int, int);
int Pascal backline(int, int);
int Pascal backpage(int, int);
int Pascal backsearch(int, int);
int Pascal backword(int, int);
int Pascal bclear(BUFFER *);
int Pascal bindtokey(int, int);
void Pascal blankpage(void);
int Pascal bktoshell(int, int);
int Pascal buildlist(int wh);
int Pascal capword(int, int);
int Pascal cex(int, int);
int Pascal USE_FAST_CALL chk_k_range(int);
int Pascal cinsert();
int Pascal clean();
int Pascal clrmes(int, int);
char * Pascal cmdstr(char *, int);
int Pascal copyregion(int, int);
int Pascal copyword(int, int);
int Pascal ucrypt(char *, int);
#if CRYPT == 0
#define cryptremote(X)
#else
void Pascal cryptremote(char *);
#endif
WinChar * char_to_wchar(char const *, int, WinChar *);
int Pascal ctlxe(int, int);
int Pascal ctlxlp(int, int);
int Pascal ctlxrp(int, int);
int Pascal USE_FAST_CALL ctoec(int);
int Pascal ctrlg(int, int);
Bool Pascal cursor_on_off(Bool on);
int Pascal debug(BUFFER*,char*);
int Pascal delbword(int, int);
int Pascal delfword(int, int);
int Pascal delgmode(int, int);
int Pascal delmode(int, int);
int Pascal delwind(int, int);
int Pascal desbind(int, int);
int Pascal deskey(int, int);
int Pascal detab(int, int);
char *Pascal dir_entry_name();
Cc doeither(int f, int n, Command fnc);
extern BUFFER * g_dofilebuff;
void Pascal double_crypt(char *, int);
int Pascal dropbuffer(int, int);
void Pascal edinit(void);
int Pascal endword(int, int);
int Pascal enlargewind(int, int);
int Pascal entab(int, int);
int Pascal execbuf(int, int);
int Pascal execcmd(int, int);
int Pascal execfile(int, int);
int Pascal execporb(int isp, int n);
int Pascal execproc(int, int);
int        execwrap(int wh);
int extract_line(char * s, char alt);
int Pascal fetchfile(int, int);
short execprog();
int Pascal ffisdiry(FILE *);
FILE * ffwopen(int mode, char * fn);
int Pascal ffclose();
FILE* Pascal ffropen(const char *);
int Pascal filefind(int, int);
int Pascal filename(int, int);
int Pascal fileread(int, int);
int Pascal filesave(int, int);
int Pascal filewrite(int, int);
int Pascal fillpara(int, int);
int Pascal filter(int, int);
int        findTag(int, int);
void flush_typah();
int Pascal fmatch();
int Pascal fnclabel();
int Pascal forwchar(int, int);
#define forwbychar(x) forwchar(x,x)
int Pascal forwdel(int, int);
int Pascal forwhunt(int, int);
int Pascal forwline(int, int);
#define forwbyline(x) forwline(x,x)
int Pascal forwpage(int, int);
int Pascal forwsearch(int, int);
int Pascal getccol();
int Pascal getcmd(int mode);
int Pascal getfence(int, int);
int Pascal USE_FAST_CALL getgoal(int, LINE*);
REGION * Pascal getregion();
int Pascal getstring(char * buf, int nbuf, const char * prompt);
short * Pascal get_vscr_line(int row);
char    Pascal get_vscr_colors(int row);
BUFFER * Pascal gotfile(void);
int Pascal gotobob(int, int);
int Pascal gotobol(int, int);
int Pascal gotobop(int, int);
int Pascal gotoeob(int, int);
int Pascal gotoeol(int, int);
int Pascal gotoeop(int, int);
int Pascal gotoline(int, int);
int Pascal gotomark(int, int);
int Pascal help(int, int);
int Pascal USE_FAST_CALL hunt(int, int);
int Pascal initchars();
int Pascal insbrace(int, int);
int Pascal insfile(int, int);
int Pascal inspound();
int Pascal insspace(int, int);
int Pascal inword();
int Pascal indentsearch(int, int);
int Pascal iskboard();
void Pascal ismodeline();
int Pascal USE_FAST_CALL isword(char);
int Pascal istring(int, int);
int Pascal hidebuffer(int, int);
int Pascal kdelete(int, int);
int Pascal killkill(int, int);
int Pascal killpara(int, int);
int Pascal killregion(int, int);
int Pascal killtext(int, int);
int Pascal USE_FAST_CALL kinsert(char);
int Pascal lastbuffer(int, int);
int Pascal lchange(register int);
int Pascal USE_FAST_CALL ldelchrs(Int, int, int);
WINDOW * USE_FAST_CALL leavewind(int, WINDOW *);
LINE * Pascal USE_FAST_CALL lfree(int, LINE *);
int Pascal linsert(int, char);
int Pascal linstr(const char *);
int Pascal listbuffers(int, int);
int Pascal USE_FAST_CALL lnewline(int);
int Pascal lowerregion(int, int);
int Pascal lowerword(int, int);
int Pascal macarg(char * tok);
//int Pascal macrotokey(int, int);
int main_(int argc, char * argv[]);
void Pascal mbwrite(const char *);
int Pascal meta(int, int);
void Pascal mlerase();
void Pascal mlforce(const char *);
#define UNSIGNED
void Pascal mlout(UNSIGNED char);
void Pascal mlputf(int);
int Pascal mlputi(int);
void Pascal mlputs(int, const UNSIGNED char *);
#define mlreply(a,b,c) nextarg(a,b,c)
int Pascal USE_FAST_CALL mlyesno(const char *);
int Pascal mouseoffset();
int Pascal movemd();
int Pascal movemu();
int Pascal mregdown();
int Pascal mregup();
char * msd_tidy();
char * Pascal msd_nfile(void);
int Pascal mvdnwind(int, int);
int Pascal mvupwind(int, int);
int Pascal namebuffer();
int Pascal namedcmd(int, int);
int Pascal nextarg(const char * prompt, char * buffer, int size);
int Pascal replace_file(const char * s);
int Pascal narrow(int, int);
int Pascal ins_newline(int, int);
void Pascal USE_FAST_CALL newdims(int, int);
int Pascal nextbuffer(int, int);
int Pascal nextch(Lpos_t * lpos, int dir);
int Pascal nextdown(int, int);
int Pascal nextup(int, int);
int Pascal nextwind(int, int);
int Pascal nextword(int, int);
int Pascal nullproc(int, int);
int Pascal onlywind(int, int);
#if FLUFF
int Pascal openline(int, int);
#endif
int Pascal ovstring(int, int);
int Pascal pipecmd(int, int);
int pipefilter(char);
char * USE_FAST_CALL point_curr(Lpos_t * spos_ref);
int Pascal prevwind(int, int);
int Pascal putctext(char *);
void       putpad(const char *);
int Pascal qreplace(int, int);
int Pascal quickexit(int, int);
int Pascal quit(int, int);
int Pascal quote(int, int);
int        rdonly();
/*extern Int readin_lines;*/
int Pascal readin(const char *, int);
int Pascal refresh(int, int);
int Pascal reposition(int, int);
void Pascal resetkey(CRYPTKEY *);
void Pascal resetlcache();
int Pascal resize(int, int);
int Pascal resizm(int,int);
void USE_FAST_CALL rest_l_offs(Lpos_t*);
int        resterr();
int Pascal rmcclear();
void Pascal rvstrcpy(char *, char *);
int Pascal savematch();
//int Pascal savewnd(int, int);
void Pascal scroll_vscr(void);
void Pascal scwrite(int, short *, int);
int Pascal sendfile(int, int);
int Pascal setcline(void);
int Pascal setuekey(int, int);
int Pascal setekey(CRYPTKEY *);
int Pascal setfillcol(int, int);
int Pascal setgmode(int, int);
int Pascal setjtable();
void Pascal setktkey(int, char *, KEYTAB*);
int Pascal setmark(int, int);
int Pascal setmod(int, int);
int Pascal shellprog(char wh, char * cmd);
int Pascal bufferposition(int, int);
int Pascal shrinkwind(int, int);
char * Pascal skipspaces(char * s, char * limstr);
void Pascal spal();
int Pascal spawn(int, int);
int Pascal splitip();
int Pascal splitwind(int, int);
int Pascal searchIncls(int, int);
char * searchfile(char * result, Fdcr fdcr);
int Pascal sreplace(int, int);
Cc Pascal startup(const char *);
#if S_LINUX
void stdin_close();
#endif
int Pascal storemac(int, int);
int Pascal storeproc(int, int);
char * strlast(const char * src, char ch);
int strcmp_right(const char *, const char *);
int Pascal strinclude(const char *, const char *);
extern char bad_strmatch;
const char * Pascal strmatch(const char *, const char *);
const char * Pascal strsame(const char *, const char *);
extern char * Pascal strpcpy(char * tgt, const char * src, int mlen);
char * Pascal strpcpypfx(char *, const char *, int, char);
int Pascal swapmark(int, int);
int Pascal USE_FAST_CALL swbuffer(BUFFER*);
int Pascal tcapopen();
int Pascal toggmode(int, int);
int Pascal togmod(int, int);
int Pascal ttsystem(const char *, const char *);
int Pascal ttclose();
int Pascal ttflush();
int Pascal ttgetc();
void Pascal ttputc(unsigned char);
void Pascal ttscupdn(int);
void Pascal tcapscreg(int, int);

void Pascal USE_FAST_CALL tcapmove(int, int);
void Pascal tcapeeol();
void Pascal tcapepage();
#ifndef tcaprev
void Pascal tcaprev(int st);
#endif
void       tcapchrom(short);
int Pascal tcapbeep(void);
int Pascal tcapbeep();
#if MOUSE || S_MSDOS == 0
void Pascal ttopen();
#else
#define ttopen()
#endif
int Pascal tcapclose(int lvl);
#ifndef tcapkopen
void Pascal tcapkopen();
#endif
void Pascal tcapbfcol(unsigned int color);
int Pascal scinit();
int Pascal getboard();
int Pascal egaopen();
int Pascal egaclose();
int Pascal fnclabel();
int Pascal to_kill(int, int);
#if S_WIN32
#define thread_id() (int)GetCurrentProcess()
#else
long unsigned int thread_id(void);
#endif
#if FLUFF
int Pascal twiddle(int,int);
#endif
#if S_MSDOS
#define typahead() (g_eaten_pair.both.ct || _kbhit())
#else
int Pascal typahead();
#endif
int Pascal unarg(int, int);
int Pascal undochange(int notused, int n);
int Pascal unbindkey(int, int);
int Pascal uniarg(int, int);
//void Pascal USE_FAST_CALL updall(WINDOW *, int);
void Pascal updallwnd(int);
int /*Pascal*/ update(int);
int Pascal updone();
void Pascal updupd(int);
int Pascal upmode();
int Pascal upperregion(int, int);
int Pascal upperword(int, int);
int Pascal upscreen(int, int);
#if S_MSDOS
int Pascal upwind_(void);
#define upwind(x) upwind_()
#else
int Pascal upwind(int garbage);
#endif
int Pascal usebuffer(int, int);
int Pascal use_named_str(Char *,Char *);
int Pascal varclean();
void Pascal varinit();
int Pascal viewfile(int, int);
int Pascal vteeol();
int Pascal vtfree();
void Pascal vtinit(int cols, int dpthm1);
int Pascal vtputc();
int Pascal vttidy();
int Pascal widen(int, int);
int Pascal window_bgfg(WINDOW * wp);
WINDOW * Pascal window_ct(BUFFER* bp);
int Pascal wordcount(int, int);
int Pascal wordsearch(int, int);
int Pascal wrapword(int, int);
int Pascal writemsg(int, int);
int Pascal writeout(const char * fn);
int Pascal yank(int, int);
int Pascal zotbuf(BUFFER*);
void Pascal MySetCoMo();

int init_paren(const char *, int);
int Pascal USE_FAST_CALL scan_paren(char);
int Pascal scan_par_line(LINE *);

unsigned int Pascal chcase();
unsigned int Pascal getckey(int);
unsigned int Pascal stock(char *);
KEYTAB * getbind(int);

#ifndef RAWTI
WINDOW *Pascal mousewindow();
WINDOW *Pascal wpopup();
#endif

char * Pascal remallocstr(char **, const char *, int);

#if	DIACRIT
int Pascal islower();
int Pascal isupper();
int Pascal isletter();
#endif

#if S_MSDOS && MSC || S_UNIX5
 int Cdecl mlwrite(const char *, ...);
#else
 int Cdecl mlwrite( va_alist );
#endif

/* some library redefinitions */

/* char *strcat();
char *strcpy(); */


#define CTOEC(c) (CTRL | ((int)(c)+'@'))

#if S_MSDOS && MSC || S_WIN32 || S_UNIX5
 char * concat(char * tgt,  ...);
#else
 char *  concat( );
#endif

#if S_MSDOS && S_WIN32 == 0

#define LFN_to_83   1
#define LFN_from_83 2

Char * Pascal LFN_to_8dot3(int dir/* 1 to 8.3; else 2*/, int up,
				char *from, char *to);

#endif

int TermThreadView(int cc);
void Pascal SetParentFocus(void);

#define FILE_INS  1
#define FILE_REST 2
#define FILE_LOCK 4
#define FILE_NMSG 8
