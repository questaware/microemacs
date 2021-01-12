extern NOSHARE int predefvars[]; 

				/*and its preprocesor definitions */
#define EVACOUNT	 0
#define EVASAVE 	 1
#define	EVBUFHOOK	 2
#define EVCBFLAGS	 3
#define EVCBUFNAME 4
#define EVCFNAME	 5
#define EVCMDHK 	 6
#define EVCMODE 	 7
#define EVCMTCOL	 8
#define EVCOL1CH	 9
#define EVCOL2CH	 10
#define EVCURCHAR	 11
#define EVCURCOL	 12
#define EVCURLINE	 13
#define EVCURWIDTH 14
#define EVCWLINE	 15
#define EVDEBUG    16
#define EVDIAGFLAG 17
#define EVDISCMD   18
#define EVDISINP   19
#define EVEXBHOOK  20
#define EVFCOL		 21
#define EVFILEPROF 22
#define EVFILLCOL	 23
//#define EVFLICKER	 24
#define EVGFLAGS	 25
#define EVGMODE 	 26
#define	EVHARDTAB	 27
#define	EVHIGHLIGHT 28
#define EVHJUMP		 29
#define EVHSCROLL	 30
#define EVINCLD		 31
#define EVKEYCT		 32
#define EVKILL     33
#define EVLANG     34
#define EVLASTDIR	 35
#define EVLASTKEY  36
#define EVLASTMESG 37
#define EVLINE     38
#define EVLWIDTH   39
#define EVMATCH    40
#define EVMODEFLAG 41
#define EVMSFLAG   42
#define EVPAGELEN  43
#define EVPALETTE  44
#define EVPENDING  45
#define EVPOPUP    46
#define EVRAM      47
#define EVREADHK   48
#define	EVREGION	 49
#define EVREPLACE  50
#define EVRVAL     51
#define EVSEARCH   52
#define EVSEED     53
//#define EVSOFTTAB	54
//#define EVSRES    55
#define EVSSAVE    56
#define EVSSCROLL	 57
#define EVSTATUS	 58
#define EVSTERM 	 59
#define EVTARGET	 60
#define EVTERM		 61
#define EVTIME		 62
#define EVUARG		 63
#define EVVERSION	 64
#define EVWINTITLE 65
#define EVWLINE 	 66
#define EVWRAPHK	 67
#define	EVWRITEHK	 68
#define EVXPOS		 69
#define EVYPOS		 70
#define EVZCMD		 71

#define fillcol predefvars[EVFILLCOL]
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
#define modeflag   predefvars[EVMODEFLAG]
#define sscroll    predefvars[EVSSCROLL]
#define ssave      predefvars[EVSSAVE]
#define terminal   predefvars[EVTERM]
#define hscroll    predefvars[EVHSCROLL]
#define diagflag   predefvars[EVDIAGFLAG]
#define mouseflag  predefvars[EVMSFLAG]
#define predef    predefvars[EVLINE]
#define quotec    predefvars[EVLWIDTH]
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
#define gfcolor   predefvars[EVCURCOL]
#define gbcolor   predefvars[EVCURLINE] 
#define mpresf    predefvars[EVCWLINE]
#define abortc    predefvars[EVKILL]
#define sterm     predefvars[EVLANG]
#define g_prefix    predefvars[EVLASTDIR]
#define prenum    predefvars[EVLASTMESG]
#define cryptflag predefvars[EVVERSION]
#define gs_term   predefvars[EVRAM]

#endif


