extern NOSHARE int predefvars[]; 

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
#define EVHSCROLL	 29
#define EVINCLD		 30
#define EVKEYCT		 31
#define EVKILL     32
#define EVLANG     33
#define EVLASTDIR	 34
#define EVLASTKEY  35
#define EVLASTMESG 36
#define EVLINE     37
#define EVMATCH    38
#define EVMSFLAG   39
#define EVPAGELEN  40
#define EVPAGEWIDTH 41
#define EVPALETTE  42
#define EVPENDING  43
#define EVPOPUP    44
#define EVREADHK   45
#define	EVREGION	 46
#define EVREPLACE  47
#define EVSEARCH   48
#define EVSEED     49
#define EVSSAVE    50
#define EVSSCROLL	 51
#define EVSTATUS	 52
#define EVSTERM 	 53
#define EVUARG		 54
#define EVUSESOFTTAB 55
#define EVVERSION	 56
#define EVWINTITLE 57
#define EVWLINE 	 58
#define EVWRAPHK	 59
#define	EVWRITEHK	 60
#define EVXPOS		 61
#define EVYPOS		 62
#define EVZCMD		 63

#define pd_fillcol predefvars[EVFILLCOL]
#define gasave  predefvars[EVASAVE]
#define gacount predefvars[EVACOUNT]
#define col1ch  predefvars[EVCOL1CH]
#define col2ch  predefvars[EVCOL2CH]
#define lastkey predefvars[EVLASTKEY]
#define seed    predefvars[EVSEED]
#define g_gmode	predefvars[EVGMODE]
#define univct 	predefvars[EVUARG]
#define gflags 	predefvars[EVGFLAGS]
#define rval	predefvars[EVRVAL]
#define xpos	predefvars[EVXPOS]
#define ypos	predefvars[EVYPOS]
#define keyct   predefvars[EVKEYCT]
#define tabsize	 predefvars[EVHARDTAB]
//#define stabsize predefvars[EVSOFTTAB]
#define hjump	     predefvars[EVHJUMP]
#define cmt_colour predefvars[EVCMTCOL]
//#define flickcode predefvars[EVFLICKER]
#define macbug	   predefvars[EVDEBUG]
#define cmdstatus  predefvars[EVSTATUS]
#define g_discmd	 predefvars[EVDISCMD]  /* 0 : nowt, -1 record only */
#define g_disinp	 predefvars[EVDISINP]
#define sscroll    predefvars[EVSSCROLL]
#define ssave      predefvars[EVSSAVE]
#define terminal   predefvars[EVTERM]
#define hscroll    predefvars[EVHSCROLL]
#define diagflag   predefvars[EVDIAGFLAG]
#define mouseflag  predefvars[EVMSFLAG]
#define predef    predefvars[EVLINE] /* stolen */
#define quotec    predefvars[EVPOPUP]
#define saveflag  predefvars[EVMATCH]
#define sgarbf    predefvars[EVBUFHOOK]
#define minfcol   predefvars[EVFCOL]

#define kbdmode   predefvars[EVCBFLAGS]
#define kbdrd     predefvars[EVCBUFNAME]
#define kbdwr     predefvars[EVCFNAME]
#define kbdrep    predefvars[EVCMODE]

#define nclicks   predefvars[EVPENDING]

#if 0
					/* overloading variables */
#define xxx	  predefvars[EVREADHK]
#define g_execlevel predefvars[EVCMDHK]
#define mpresf    predefvars[EVCWLINE]
#define abortc    predefvars[EVKILL]
#define sterm     predefvars[EVLANG]
#define g_prefix    predefvars[EVLASTDIR]
#define prenum    predefvars[EVLASTMESG]
#define cryptflag predefvars[EVVERSION]
#define gs_term   predefvars[EVRAM]

#endif


