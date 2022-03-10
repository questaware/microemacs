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
#define EVDISCMD   17
#define EVDISINP   18
#define EVEXBHOOK  19
#define EVFCOL		 20
#define EVFILEPROF 21
#define EVFILLCOL	 22
#define EVGFLAGS	 23
#define EVGMODE 	 24
#define	EVHARDTAB	 25
#define	EVHIGHLIGHT 26
#define EVHJUMP		 27
#define EVINCLD		 28
#define EVKEYCT		 29
#define EVKILL     30
#define EVLANG     31
#define EVLASTDIR	 32
#define EVLASTKEY  33
#define EVLASTMESG 34
#define EVLINE     35
#define EVMATCH    36
#define EVMSFLAG   37
#define EVPAGELEN  38
#define EVPAGEWIDTH 39
#define EVPALETTE  40
#define EVPENDING  41
#define EVPOPUP    42
#define EVREADHK   43
#define	EVREGION	 44
#define EVREPLACE  45
#define EVSEARCH   46
#define EVSEED     47
#define EVSSAVE    48
#define EVSTATUS	 49
#define EVUARG		 50
#define EVVERSION	 51
#define EVWINTITLE 52
#define EVWLINE 	 53
#define EVWORK		 54
#define EVWRAPHK	 55
#define	EVWRITEHK	 56
#define EVXPOS		 57
#define EVYPOS		 58
#define EVZCMD		 59

#define pd_gacount  predefvars[EVACOUNT].i
#define pd_gasave   predefvars[EVASAVE].i
#define pd_cliplife predefvars[EVCLIPLIFE].i
#define pd_prenum		predefvars[EVCURCHAR].i	/* stolen */
#define pd_fillcol  predefvars[EVFILLCOL].i
#define pd_lastdir	predefvars[EVLASTDIR].i
#define pd_col1ch   predefvars[EVCOL1CH].i
#define pd_col2ch   predefvars[EVCOL2CH].i
#define pd_lastkey  predefvars[EVLASTKEY].i
#define seed       predefvars[EVSEED].i
#define g_gmode	   predefvars[EVGMODE].i
#define g_univct   predefvars[EVUARG].i
#define pd_gflags  predefvars[EVGFLAGS].i
#define pd_xpos	   predefvars[EVXPOS].i		// For mouse
#define pd_ypos	   predefvars[EVYPOS].i
#define keyct      predefvars[EVKEYCT].i
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
#define pd_predef  predefvars[EVLINE].i /* stolen */
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


