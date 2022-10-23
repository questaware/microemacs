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
#define	EVHLIGHT1  26
#define	EVHLIGHT2  27
#define	EVHLIGHT3  28
#define	EVHLIGHT4  29
#define EVHJUMP		 30
#define EVINCLD		 31
#define EVKEYCT		 32
#define EVKILL     33
#define EVLANG     34
#define EVLASTDIR	 35
#define EVLASTKEY  36
#define EVLASTMESG 37
#define EVLINE     38
#define EVMATCH    39
#define EVMSFLAG   40
#define EVPAGELEN  41
#define EVPAGEWIDTH 42
#define EVPALETTE  43
#define EVPENDING  44
#define EVPOPUP    45
#define EVREADHK   46
#define	EVREGION	 47
#define EVREPLACE  48
#define EVSEARCH   49
#define EVSEED     50
#define EVSSAVE    51
#define EVSTATUS	 52
#define EVUARG		 53
#define EVVERSION	 54
#define EVWINNEW	 55
#define EVWINTITLE 56
#define EVWLINE 	 57
#define EVWORK		 58
#define EVWRAPHK	 59
#define	EVWRITEHK	 60
#if 0
#define EVXPOS		 61
#define EVYPOS		 62
#define EVZCMD		 63
#else
#define EVZCMD		 61
#endif

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
#define pd_keyct   predefvars[EVKEYCT].i
#define pd_tabsize predefvars[EVHARDTAB].i
#define pd_hjump	 predefvars[EVHJUMP].i
#define pd_cmt_colour predefvars[EVCMTCOL].i
#define pd_macbug	    predefvars[EVDEBUG].i
#define pd_cmdstatus  predefvars[EVSTATUS].i
#define pd_got_msg predefvars[EVCFNAME].i /* stolen */
#define diagflag   predefvars[EVDIAGFLAG].i
#define pd_discmd	 predefvars[EVDISCMD].i /* 0 : nowt, -1 record only */
#define g_disinp	 predefvars[EVDISINP].i
#define pd_fcol		 predefvars[EVFCOL].i
#define pd_ssave   predefvars[EVSSAVE].i
#define pd_incldirs predefvars[EVINCLD].p
#define mouseflag  predefvars[EVMSFLAG].i
#define pd_predef  predefvars[EVLINE].i /* stolen */
#define pd_sgarbf  predefvars[EVBUFHOOK].i
#define pd_palstr  predefvars[EVPALETTE].p
#define pd_pat		 predefvars[EVSEARCH].p
#define pd_hlight1 predefvars[EVHLIGHT1].p
#define pd_hlight2 predefvars[EVHLIGHT2].p
#define pd_hlight3 predefvars[EVHLIGHT3].p
#define pd_hlight4 predefvars[EVHLIGHT4].p
#define pd_file_prof predefvars[EVFILEPROF].p
#define pd_patmatch  predefvars[EVMATCH].p
#define pd_winnew	 predefvars[EVWINNEW].i

#define pd_kbdrd   predefvars[EVCBUFNAME].i
#define pd_kbdrep  predefvars[EVCMODE].i

#define pd_nclicks    predefvars[EVPENDING]

#if 0
					/* overloading variables */
X#define xxx	  predefvars[EVREADHK]
X#define g_execlevel predefvars[EVCMDHK]
X#define abortc    predefvars[EVKILL]
X#define g_prefix    predefvars[EVLASTDIR]
X#define prenum    predefvars[EVLASTMESG]
X#define cryptflag predefvars[EVVERSION]
X#define gs_term   predefvars[EVRAM]

#endif


