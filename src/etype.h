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
											/* ALL global fuction declarations */
extern int rubbish;

Char * ClipAlloc(int);
Char * ClipRef(void);
Cc ClipSet(char * src);
char * ClipPasteStart(void);
void ClipPasteEnd(void);
Cc ClipDelete(void);

/*extern int atoi(char * s);*/

int Pascal millisleep(unsigned int n);
char * mallocz(int n);
void adb(int n);
void app_kbdm(int key);
int Pascal arith(int, int);
int Pascal addnewbind(int c, int (Pascal *func)(int, int));
char * Pascal allocate(unsigned nbytes);
char * Pascal callocate(unsigned nbytes);
void Pascal customise_buf(BUFFER * bp);
char * Pascal duplicate(const char * src);
void cls(void);
BUFFER *Pascal bfind(const char * bname, int cflag, int bflag);
int Pascal getIncls(int f, int n);
BUFFER *Pascal getdefb(void);
BUFFER *Pascal getcbuf(char *, char *, int);
BUFFER *Pascal bufflink(const char *, int);
char * Pascal repl_bfname(BUFFER*, const char *);
char *Pascal bytecopy();
char *Pascal complete();
char *Pascal envval();
void Pascal expandp(char *, char *, char *, int);
const char *Pascal fixnull(const char * s);
const char * Pascal flook(char, const char *);
char * Pascal flookdown(char *, char *);
char *Pascal flooknear();
char *Pascal funval();
char *Pascal getctext(char *);
const char *Pascal getfname(int);
char *Pascal getkill(void);
char *Pascal getreg(char * t);
extern char getvalnull[];
char *Pascal getval(char *, char *);

#if S_HPUX == 0 || 1
 const
#endif
       char *Pascal gtenv(const char *);
char *Pascal gtfilename(char *);
#if S_HPUX == 0 || 1
 const
#endif
       char *Pascal gtusr(char*);
char *Pascal ilook();
void  Pascal ibefore(LINE*, LINE*);
char * Pascal io_message(char *, int, int);
char *Pascal int_asc(int);
#if S_HPUX == 0 || 1
 const
#endif
	 char *Pascal ltos(int);
int Pascal makename(char *, const char *);
char *Pascal mkkey(const char *);
char *Pascal mkul(int, char *);
char *Pascal namval();
BUFFER * Pascal bmfind(int, int);
int Pascal pipefilter(char);
BUFFER * Pascal prevele(BUFFER*, BUFFER*);
char *Pascal timeset();
char *Pascal token(char * src_, char * tok, int size);
const char *Pascal transbind(char *);
char * Pascal trimstr(char * s, int * from);
char *Pascal xlat(char *, char *, char *);
int (Pascal *Pascal fncmatch(char *))(int, int);
int (Pascal *Pascal getname(char *))(int, int);
char * Pascal getconsoletitle();
int Pascal desfunc(int,int);
int Pascal desvars(int, int);
int Pascal dispvar(int,int);
int Pascal echochar(int,int);
int Pascal myeq(int, int);
int Pascal ernd(void);
int Pascal execkey(KEYTAB *, int, int);
#if S_BSD | S_UNIX5 | S_XENIX | S_SUN | S_HPUX
int Pascal fexist(const char *);
#else
#define fexist(fn) name_mode(fn)
#endif
int fex_path(const char * dir, const char * file);
char * fex_up_dirs(const char * dir, char * file);
void Pascal flook_init(char *);
int Pascal fisearch(int, int);
int Pascal getkey();
char * Pascal getwtxt(int, char *, int);
int Pascal gettyp(char *);
int Pascal getwpos();
int Pascal get_char();
int Pascal mcstr(int);
extern void Pascal mkdes();
int Pascal nmlze_fname(char *, const char *, char *);
void Pascal openwind(WINDOW *, BUFFER*);
int Pascal openwindbuf(char *);
int Pascal orwindmode(int, int);
char * Pascal pathcat(char *, int, const char *, const char *);
int Pascal reglines();
void Pascal release(char * mp);
int Pascal remmark(int, int);
int Pascal risearch(int, int);
void Pascal rpl_all(LINE*, LINE*, int, int, int);
int Pascal scan_for_sl(LINE * lp);
int Pascal scanner(int, int);
#if S_WIN32
void setMyConsoleIP(void);
#endif
void Pascal setconsoletitle(char * title);
int Pascal handletab(int, int);
int Pascal setlower(char*,char*);
int Pascal setupper(char*,char*);
int Pascal setvar(int, int);
int Pascal set_var(const char *, char *);
int Pascal sindex(char *, char *);
int Pascal stol(char * s);
int Pascal svar(int, char *);
int Pascal trim_white(int, int);
void Pascal tcap_init();
void Pascal tcapsetsize(int wid, int dpth);
int Pascal tgetc();
int Pascal topluct();
int Pascal uneat();
LINE *Pascal mk_line(const char *, int, int);
LINE *Pascal mouseline();
int Pascal adjustmode(int, int);
int Pascal anycb();
int Pascal apro(int, int);
int Pascal backchar(int, int);
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
int Pascal buildlist(const char * mstring);
int Pascal capword(int, int);
int Pascal cbuf1(int, int);
int Pascal cbuf10(int, int);
int Pascal cbuf11(int, int);
int Pascal cbuf12(int, int);
int Pascal cbuf13(int, int);
int Pascal cbuf14(int, int);
int Pascal cbuf15(int, int);
int Pascal cbuf16(int, int);
int Pascal cbuf17(int, int);
int Pascal cbuf18(int, int);
int Pascal cbuf19(int, int);
int Pascal cbuf2(int, int);
int Pascal cbuf20(int, int);
int Pascal cbuf21(int, int);
int Pascal cbuf22(int, int);
int Pascal cbuf23(int, int);
int Pascal cbuf24(int, int);
int Pascal cbuf25(int, int);
int Pascal cbuf26(int, int);
int Pascal cbuf27(int, int);
int Pascal cbuf28(int, int);
int Pascal cbuf29(int, int);
int Pascal cbuf3(int, int);
int Pascal cbuf30(int, int);
int Pascal cbuf31(int, int);
int Pascal cbuf32(int, int);
int Pascal cbuf33(int, int);
int Pascal cbuf34(int, int);
int Pascal cbuf35(int, int);
int Pascal cbuf36(int, int);
int Pascal cbuf37(int, int);
int Pascal cbuf38(int, int);
int Pascal cbuf39(int, int);
int Pascal cbuf4(int, int);
int Pascal cbuf40(int, int);
int Pascal cbuf5(int, int);
int Pascal cbuf6(int, int);
int Pascal cbuf7(int, int);
int Pascal cbuf8(int, int);
int Pascal cbuf9(int, int);
int Pascal cex(int, int);
int Pascal chk_k_range(int);
int Pascal cinsert();
int Pascal clean();
int Pascal clrmes(int, int);
char * Pascal cmdstr(char *, int);
int Pascal copyregion(int, int);
int Pascal copyword(int, int);
extern char bad_strmatch;
const char * Pascal com_match(const char *, const char *, int);
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
int Pascal ctoec(int);
int Pascal ctrlg(int, int);
Bool Pascal cursor_on_off(Bool on);
void Pascal dcline(int, char **);
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
int Pascal dobuf(BUFFER*, int);
int Pascal docmd(char *);
extern BUFFER * dofilebuff;
void Pascal double_crypt(char *, int);
int Pascal ectoc(int);
void Pascal edinit(void);
int Pascal endword(int, int);
int Pascal enlargewind(int, int);
int Pascal entab(int, int);
int Pascal execbuf(int, int);
int Pascal execcmd(int, int);
int Pascal execfile(int, int);
int Pascal execprg(int, int);
int Pascal execproc(int, int);
int Pascal fetchfile(int, int);
short execprog();
int Pascal ffisdiry();
int Pascal ffclose();
int Pascal ffgetline(int *);
int Pascal ffputline(char *, int);
int Pascal ffropen(const char *);
int Pascal ffwopen(int, char *);
int Pascal filefind(int, int);
int Pascal filename(int, int);
int Pascal fileread(int, int);
int Pascal filesave(int, int);
int Pascal filewrite(int, int);
int Pascal fillpara(int, int);
int Pascal filter(int, int);
int        findTag(int, int);
int Pascal fmatch();
int Pascal fnclabel();
int Pascal forwchar(int, int);
int Pascal forwdel(int, int);
int Pascal forwhunt(int, int);
int Pascal forwline(int, int);
int Pascal forwpage(int, int);
int Pascal forwsearch(int, int);
void Pascal freewhile();
int Pascal getccol();
int Pascal getcmd();
int Pascal getfence(int, int);
int Pascal getgoal(LINE*, int);
extern REGION region;
int Pascal getregion();
int Pascal getstring(char * buf, int nbuf, const char * prompt);
short * Pascal get_vscr_line(int row);
char    Pascal get_vscr_colors(int row);
BUFFER * Pascal gotfile(const char * fname);
int Pascal gotobob(int, int);
int Pascal gotobol(int, int);
int Pascal gotobop(int, int);
int Pascal gotoeob(int, int);
int Pascal gotoeol(int, int);
int Pascal gotoeop(int, int);
int Pascal gotoline(int, int);
int Pascal gotomark(int, int);
int Pascal help(int, int);
int Pascal hunt(int, int);
int Pascal indent(int, int);
int Pascal initchars();
int Pascal insbrace(int, int);
int Pascal insfile(int, int);
int Pascal inspound();
int Pascal insspace(int, int);
int Pascal inword();
int Pascal indentsearch(int, int);
int Pascal isearch(int, int);
int Pascal iskboard();
void Pascal ismodeline();
int Pascal isword(char);
int Pascal istring(int, int);
int Pascal hidebuffer(int, int);
int Pascal kdelete(int, int);
int Pascal killbuffer(int, int);
int Pascal killkill(int, int);
int Pascal killpara(int, int);
int Pascal killregion(int, int);
int Pascal killtext(int, int);
int Pascal kinsert(char);
int Pascal lastbuffer(int, int);
void Pascal lchange(register int);
int Pascal ldelchrs(Int, int);
int Pascal ldelnewline();
void Pascal leavewind(WINDOW *);
LINE * Pascal lfree(register LINE *, int);
int Pascal linsert(int, char);
int Pascal linstr(const char *);
int Pascal listbuffers(int, int);
int Pascal lnewline();
int Pascal lowerregion(int, int);
int Pascal lowerword(int, int);
int Pascal macarg(char * tok);
int Pascal macrotokey(int, int);
int main_(int argc, char * argv[]);
#if S_WIN32
void Pascal mbwrite(const char *);
#else
#define mbwrite(m)
#endif
void Pascal mcclear();
int Pascal meta(int, int);
Cc Pascal mlerase();
void Pascal mlforce(const char *);
void Pascal mlout(char);
void Pascal mlputf(int);
int Pascal mlputi(int);
void Pascal mlputs(const char *);
#define mlreply(a,b,c) nextarg(a,b,c)
#define mltreply(a,b,c) nextarg(a,b,c)
int Pascal mlyesno(char *);
void Pascal modeline(WINDOW *);
int Pascal mouseoffset();
int Pascal movemd();
int Pascal movemu();
int Pascal mregdown();
int Pascal mregup();
char * Pascal msd_nfile(int * fnoffs);
int Pascal mvdnwind(int, int);
int Pascal mvupwind(int, int);
int Pascal namebuffer();
int Pascal namedcmd(int, int);
int Pascal name_mode(const char * s);
int Pascal narrow(int, int);
int Pascal ins_newline(int, int);
int Pascal newdims(int, int);
int Pascal nextarg(const char * prompt, char * buffer, int size);
int Pascal nextbuffer(int, int);
int Pascal nextdown(int, int);
int Pascal nextup(int, int);
int Pascal nextwind(int, int);
int Pascal nextword(int, int);
int Pascal nullproc(int, int);
int Pascal onlywind(int, int);
#if FLUFF
int Pascal openline(int, int);
#endif
void Pascal ostring(const char *);
void Pascal outstring(const char *);
int Pascal ovstring(int, int);
int Pascal pipecmd(int, int);
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
int Pascal reframe(WINDOW *);
int Pascal refresh(int, int);
int Pascal reposition(int, int);
void Pascal resetkey(CRYPTKEY *);
void Pascal resetlcache();
int Pascal resize(int, int);
int Pascal resizm(int,int);
void 			 rest_l_offs(Lpos_t *);
int        resterr();
int Pascal restwnd(int, int);
int Pascal rmcclear();
void Pascal rvstrcpy(char *, char *);
int Pascal savematch();
int Pascal savewnd(int, int);
void Pascal scroll_vscr(void);
int Pascal scwrite(int, short *, int);
int Pascal sendfile(int, int);
int Pascal setcline(void);
int Pascal setuekey(int, int);
int Pascal setekey(CRYPTKEY *);
int Pascal setfillcol(int, int);
int Pascal setgmode(int, int);
int Pascal setjtable();
void Pascal setktkey(KEYTAB*, int, char *);
int Pascal setmark(int, int);
int Pascal setmod(int, int);
int Pascal shellprog(char wh, char * cmd);
int Pascal showcpos(int, int);
int Pascal shrinkwind(int, int);
char * Pascal skipspaces(char * s, char * limstr);
void Pascal spal();
int Pascal spawn(int, int);
int Pascal spawncli(int, int);
int Pascal splitip();
int Pascal splitwind(int, int);
int Pascal searchIncls(int, int);
int Pascal sreplace(int, int);
int Pascal startup(const char *);
int Pascal storemac(int, int);
int Pascal storeproc(int, int);
int strcmp_right(const char *, const char *);
int Pascal strinclude(const char *, const char *);
const char * Pascal strmatch(const char *, const char *);
extern char * Pascal strpcpy(char * tgt, const char * src, int mlen);
char * Pascal strpcpypfx(char *, const char *, int, char);
int Pascal swapmark(int, int);
int Pascal swbuffer(BUFFER*);
void Pascal tcapopen();
int Pascal toggmode(int, int);
int Pascal togmod(int, int);
int Pascal ttsystem(const char *, const char *);
int Pascal ttclose();
int Pascal ttflush();
int Pascal ttgetc();
void Pascal ttputc(unsigned char);
void Pascal ttscupdn(int);
void Pascal tcapscreg(int, int);

void Pascal tcapmove(int, int);
void Pascal tcapeeol();
void Pascal tcapeeop();
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
#ifndef tcapkclose
int Pascal tcapkclose();
#endif
void Pascal tcapbfcol(unsigned int color);
int Pascal scinit();
int Pascal getboard();
int Pascal egaopen();
int Pascal egaclose();
int Pascal fnclabel();
long unsigned int thread_id(void);
#if FLUFF
int Pascal twiddle(int,int);
#endif
int Pascal typahead();
int Pascal unarg(int, int);
int Pascal unbindkey(int, int);
int Pascal unmark(int, int);
int Pascal unqname(char *);
int Pascal uniarg(int, int);
void Pascal updall(WINDOW *, int);
void Pascal updallwnd(int);
int /*Pascal*/ update(int);
void Pascal updline(int);
int Pascal updone();
void Pascal updupd(int);
void Pascal upmode();
int Pascal upperregion(int, int);
int Pascal upperword(int, int);
int Pascal upscreen(int, int);
void Pascal upwind(void);
int Pascal usebuffer(int, int);
int Pascal use_named_str(Char *,Char *);
int Pascal varclean();
int Pascal varinit();
int Pascal viewfile(int, int);
int Pascal vteeol();
int Pascal vtfree();
void Pascal vtinit();
int Pascal vtputc();
int Pascal vttidy();
int Pascal widen(int, int);
int Pascal window_bgfg(WINDOW * wp);
int Pascal wordcount(int, int);
int Pascal wordsearch(int, int);
int Pascal wrapword(int, int);
int Pascal writemsg(int, int);
int Pascal writeout(const char *);
int Pascal yank(int, int);
int Pascal zotbuf(BUFFER*);
void Pascal MySetCoMo();

int Pascal init_paren(const char *, int);
int Pascal scan_paren(char);
int Pascal scan_par_line(LINE *);

unsigned int Pascal chcase();
unsigned int Pascal getckey(int);
unsigned int Pascal stock(char *);
KEYTAB * Pascal getbind(int);

#ifndef RAWTI
WINDOW *Pascal mousewindow();
WINDOW *Pascal wpopup();
#endif

char * Pascal remallocstr(char **, const char *, int);

/*
#if	S_MSDOS & (TURBO | MSC)
int Pascal binary(char *, char *(Pascal *)(), int);
#else
int Pascal binary();
#endif
*/

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

