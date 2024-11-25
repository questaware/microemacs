extern PD_VAR predefvars[]; 

				/*and its preprocesor definitions */
#define EVACOUNT	 0
#define EVASAVE 	 1
#define	EVBUFHOOK	 2
#define EVCBFLAGS	 3
#define EVCBLANG	 4
#define EVCBUFNAME 5
#define EVCFNAME	 6
#define EVCLIPLIFE 7
#define EVCMDHK 	 8
#define EVCMODE 	 9
#define EVCMTCOL	 10
#define EVCOL1CH	 11
#define EVCOL2CH	 12
#define EVCURCHAR	 13
#define EVCURCOL	 14
#define EVCURLINE  15
#define EVCWLINE	 16
#define EVDEBUG    17
#define EVDISCMD   18
#define EVDISINP   19
#define EVEXBHOOK  20
#define EVFCOL		 21
#define EVFILEPROF 22
#define EVFILLCOL	 23
#define EVGFLAGS	 24
#define	EVHARDTAB	 25
#define EVHJUMP		 26
#define	EVHLIGHT1  27
#define	EVHLIGHT10 28
#define	EVHLIGHT11 29
#define	EVHLIGHT12 30
#define	EVHLIGHT13 31
#define	EVHLIGHT14 32
#define	EVHLIGHT15 33
#define	EVHLIGHT16 34
#define	EVHLIGHT17 35
#define	EVHLIGHT18 36
#define	EVHLIGHT19 37
#define	EVHLIGHT2  38
#define	EVHLIGHT20 39
#define	EVHLIGHT21 40
#define	EVHLIGHT22 41
#define	EVHLIGHT23 42
#define	EVHLIGHT24 43
#define	EVHLIGHT25 44
#define	EVHLIGHT26 45
#define	EVHLIGHT27 46
#define	EVHLIGHT28 47
#define	EVHLIGHT29 48
#define	EVHLIGHT3  49
#define	EVHLIGHT30 50
#define	EVHLIGHT4  51
#define	EVHLIGHT5  52
#define	EVHLIGHT6  53
#define	EVHLIGHT7  54
#define	EVHLIGHT8  55
#define	EVHLIGHT9  56
#define EVINCLD		 57
#define EVKEYCT		 58
#define EVKILL     59
#define EVLANG     60
#define EVLASTDIR	 61
#define EVLASTKEY  62
#define EVLASTMESG 63
#define EVLINE     64
#define EVMATCH    65
#define EVMSFLAG   66
#define EVNOINDENT 67
#define EVPAGELEN  68
#define EVPAGEWIDTH 69
#define EVPALETTE  70
#define EVPENDING  71
#define EVPOPUP    72
#define EVREADHK   73
#define	EVREGION	 74
#define EVREPLACE  75
#define EVSEARCH   76
#define EVSEED     77
#define EVSSAVE    78
#define EVSTATUS	 79
#define EVUARG		 80
#define EVUNDOS		 81
#define EVVERSION	 82
#define EVWINEW	   83
#define EVWINTITLE 84
#define EVWLINE 	 85
#define EVWORK		 86
#define EVWRAPHK	 87
#define	EVWRITEHK	 88
#if 0
#define EVXPOS		 89
#define EVYPOS		 90
#define EVZCMD		 91
#else
#define EVZCMD		 89
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
#define pd_winnew	  predefvars[EVWINEW].i

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


