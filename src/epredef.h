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
#define EVHJUMP		 25
#define	EVHLIGHT1  26
#define	EVHLIGHT10 27
#define	EVHLIGHT11 28
#define	EVHLIGHT12 29
#define	EVHLIGHT13 30
#define	EVHLIGHT14 31
#define	EVHLIGHT15 32
#define	EVHLIGHT16 33
#define	EVHLIGHT17 34
#define	EVHLIGHT18 35
#define	EVHLIGHT19 36
#define	EVHLIGHT2  37
#define	EVHLIGHT20 38
#define	EVHLIGHT21 39
#define	EVHLIGHT22 40
#define	EVHLIGHT23 41
#define	EVHLIGHT24 42
#define	EVHLIGHT25 43
#define	EVHLIGHT26 44
#define	EVHLIGHT27 45
#define	EVHLIGHT28 46
#define	EVHLIGHT29 47
#define	EVHLIGHT3  48
#define	EVHLIGHT30 49
#define	EVHLIGHT4  50
#define	EVHLIGHT5  51
#define	EVHLIGHT6  52
#define	EVHLIGHT7  53
#define	EVHLIGHT8  54
#define	EVHLIGHT9  55
#define EVINCLD		 56
#define EVKEYCT		 57
#define EVKILL     58
#define EVLANG     59
#define EVLASTDIR	 60
#define EVLASTKEY  61
#define EVLASTMESG 62
#define EVLINE     63
#define EVMATCH    64
#define EVMSFLAG   65
#define EVPAGELEN  66
#define EVPAGEWIDTH 67
#define EVPALETTE  68
#define EVPENDING  69
#define EVPOPUP    70
#define EVREADHK   71
#define	EVREGION	 72
#define EVREPLACE  73
#define EVSEARCH   74
#define EVSEED     75
#define EVSSAVE    76
#define EVSTATUS	 77
#define EVUARG		 78
#define EVVERSION	 79
#define EVWINNEW	 80
#define EVWINTITLE 81
#define EVWLINE 	 82
#define EVWORK		 83
#define EVWRAPHK	 84
#define	EVWRITEHK	 85
#if 0
#define EVXPOS		 85
#define EVYPOS		 86
#define EVZCMD		 87
#else
#define EVZCMD		 86
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


