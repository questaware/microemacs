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
#define	EVHARDTAB	 24
#define	EVHLIGHT1  25
#define	EVHLIGHT10 26
#define	EVHLIGHT11 27
#define	EVHLIGHT12 28
#define	EVHLIGHT13 29
#define	EVHLIGHT14 30
#define	EVHLIGHT15 31
#define	EVHLIGHT16 32
#define	EVHLIGHT2  33
#define	EVHLIGHT3  34
#define	EVHLIGHT4  35
#define	EVHLIGHT5  36
#define	EVHLIGHT6  37
#define	EVHLIGHT7  38
#define	EVHLIGHT8  39
#define	EVHLIGHT9  40
#define EVHJUMP		 41
#define EVINCLD		 42
#define EVKEYCT		 43
#define EVKILL     44
#define EVLANG     45
#define EVLASTDIR	 46
#define EVLASTKEY  47
#define EVLASTMESG 48
#define EVLINE     49
#define EVMATCH    50
#define EVMSFLAG   51
#define EVPAGELEN  52
#define EVPAGEWIDTH 53
#define EVPALETTE  54
#define EVPENDING  55
#define EVPOPUP    56
#define EVREADHK   57
#define	EVREGION	 58
#define EVREPLACE  59
#define EVSEARCH   60
#define EVSEED     61
#define EVSSAVE    62
#define EVSTATUS	 63
#define EVUARG		 64
#define EVVERSION	 65
#define EVWINNEW	 66
#define EVWINTITLE 67
#define EVWLINE 	 68
#define EVWORK		 69
#define EVWRAPHK	 70
#define	EVWRITEHK	 71
#if 0
#define EVXPOS		 72
#define EVYPOS		 73
#define EVZCMD		 74
#else
#define EVZCMD		 72
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
#define pd_seed     predefvars[EVSEED].i
#define g_gflag     predefvars[EVGFLAGS].i
#define g_univct    predefvars[EVUARG].i
#define pd_xpos	    predefvars[EVXPOS].i		// For mouse
#define pd_ypos	    predefvars[EVYPOS].i
#define pd_keyct    predefvars[EVKEYCT].i
#define pd_tabsize  predefvars[EVHARDTAB].i
#define pd_hjump	  predefvars[EVHJUMP].i
#define pd_cmt_colour predefvars[EVCMTCOL].i
#define pd_macbug	    predefvars[EVDEBUG].i
#define pd_cmdstatus  predefvars[EVSTATUS].i
#define pd_got_msg  predefvars[EVCFNAME].i /* stolen */
#define diagflag    predefvars[EVDIAGFLAG].i
#define pd_discmd	  predefvars[EVDISCMD].i /* 0 : nowt, -1 record only */
#define g_disinp	  predefvars[EVDISINP].i
#define pd_fcol		  predefvars[EVFCOL].i
#define pd_ssave    predefvars[EVSSAVE].i
#define pd_incldirs predefvars[EVINCLD].p
#define mouseflag   predefvars[EVMSFLAG].i
#define pd_predef   predefvars[EVLINE].i /* stolen */
#define pd_sgarbf   predefvars[EVBUFHOOK].i
#define pd_palstr   predefvars[EVPALETTE].p
#define pd_pat		  predefvars[EVSEARCH].p
#define pd_hlight1  predefvars[EVHLIGHT1].p
#define pd_hlight2  predefvars[EVHLIGHT2].p
#define pd_file_prof predefvars[EVFILEPROF].p
#define pd_patmatch  predefvars[EVMATCH].p
#define pd_winnew	  predefvars[EVWINNEW].i

#define pd_kbdrd   predefvars[EVCBUFNAME].i
#define pd_kbdrep  predefvars[EVCMODE].i

#define pd_nclicks    predefvars[EVPENDING]

#if 0
					/* overloading variables */
X#define xxx	       predefvars[EVREADHK]
X#define g_execlevel predefvars[EVCMDHK]
X#define g_abortc    predefvars[EVKILL]
X#define g_prefix    predefvars[EVLASTDIR]
X#define prenum      predefvars[EVLASTMESG]
X#define cryptflag   predefvars[EVVERSION]
X#define gs_term     predefvars[EVRAM]

#endif


