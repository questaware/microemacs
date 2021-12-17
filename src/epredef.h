extern PD_VAR predefvars[]; 

				/*and its preprocesor definitions */
#define EVACOUNT	 0
#define EVASAVE 	 1
#define	EVBUFHOOK	 2
#define EVCBFLAGS	 3
#define EVCBUFNAME 4
#define EVCFNAME	 5
#define EVCLIPLIFE 6
#define EVCMDHK 	 7
#define EVCMODE 	 8
#define EVCMTCOL	 9
#define EVCOL1CH	 10
#define EVCOL2CH	 11
#define EVCURCHAR	 12
#define EVCURCOL	 13
#define EVCURLINE  14
#define EVCWLINE	 15
#define EVDEBUG    16
#define EVDIAGFLAG 17
#define EVDISCMD   18
#define EVDISINP   19
#define EVEXBHOOK  20
#define EVFCOL		 21
#define EVFILEPROF 22
#define EVFILLCOL	 23
#define EVGFLAGS	 24
#define EVGMODE 	 25
#define	EVHARDTAB	 26
#define	EVHIGHLIGHT 27
#define EVHJUMP		 28
#define EVINCLD		 29
#define EVKEYCT		 30
#define EVKILL     31
#define EVLANG     32
#define EVLASTDIR	 33
#define EVLASTKEY  34
#define EVLASTMESG 35
#define EVLINE     36
#define EVMATCH    37
#define EVMSFLAG   38
#define EVPAGELEN  39
#define EVPAGEWIDTH 40
#define EVPALETTE  41
#define EVPENDING  42
#define EVPOPUP    43
#define EVREADHK   44
#define	EVREGION	 45
#define EVREPLACE  46
#define EVSEARCH   47
#define EVSEED     48
#define EVSSAVE    49
#define EVSTATUS	 50
#define EVSTERM 	 51
#define EVUARG		 52
#define EVVERSION	 53
#define EVWINTITLE 54
#define EVWLINE 	 55
#define EVWRAPHK	 56
#define	EVWRITEHK	 57
#define EVXPOS		 58
#define EVYPOS		 59
#define EVZCMD		 60

#define pd_gacount  predefvars[EVACOUNT].i
#define pd_gasave   predefvars[EVASAVE].i
#define pd_cliplife predefvars[EVCLIPLIFE].i
#define pd_fillcol  predefvars[EVFILLCOL].i
#define pd_lastdir	predefvars[EVLASTDIR].i
#define pd_col1ch   predefvars[EVCOL1CH].i
#define pd_col2ch   predefvars[EVCOL2CH].i
#define lastkey   predefvars[EVLASTKEY].i
#define seed      predefvars[EVSEED].i
#define g_gmode	  predefvars[EVGMODE].i
#define g_univct  predefvars[EVUARG].i
#define pd_gflags predefvars[EVGFLAGS].i
#define xpos	    predefvars[EVXPOS].i
#define ypos	    predefvars[EVYPOS].i
#define keyct     predefvars[EVKEYCT].i
#define pd_tabsize predefvars[EVHARDTAB].i
#define pd_hjump	 predefvars[EVHJUMP].i
#define cmt_colour predefvars[EVCMTCOL].i
#define macbug	   predefvars[EVDEBUG].i
#define pd_cmdstatus predefvars[EVSTATUS].i
#define pd_got_msg predefvars[EVCFNAME].i /* stolen */
#define diagflag   predefvars[EVDIAGFLAG].i
#define pd_discmd	 predefvars[EVDISCMD].i /* 0 : nowt, -1 record only */
#define g_disinp	 predefvars[EVDISINP].i
#define minfcol    predefvars[EVFCOL].i
#define ssave      predefvars[EVSSAVE].i
#define pd_incldirs predefvars[EVINCLD].p
#define mouseflag  predefvars[EVMSFLAG].i
#define predef     predefvars[EVLINE].i /* stolen */
#define quotec     predefvars[EVPOPUP].i
#define pd_sgarbf  predefvars[EVBUFHOOK].i
#define pd_palstr  predefvars[EVPALETTE].p
#define pd_pat		 predefvars[EVSEARCH].p
#define pd_highlight predefvars[EVHIGHLIGHT].p
#define pd_file_prof predefvars[EVFILEPROF].p
#define pd_patmatch  predefvars[EVMATCH].p

#define pd_kbdrd   predefvars[EVCBUFNAME].i
#define pd_kbdrep  predefvars[EVCMODE].i

#define pd_nclicks    predefvars[EVPENDING]

#if 0
					/* overloading variables */
X#define xxx	  predefvars[EVREADHK]
X#define g_execlevel predefvars[EVCMDHK]
X#define abortc    predefvars[EVKILL]
X#define sterm     predefvars[EVLANG]
X#define g_prefix    predefvars[EVLASTDIR]
X#define prenum    predefvars[EVLASTMESG]
X#define cryptflag predefvars[EVVERSION]
X#define gs_term   predefvars[EVRAM]

#endif


