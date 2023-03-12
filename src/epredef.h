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
#define	EVHLIGHT10 27
#define	EVHLIGHT11 28
#define	EVHLIGHT12 29
#define	EVHLIGHT13 30
#define	EVHLIGHT14 31
#define	EVHLIGHT15 32
#define	EVHLIGHT16 33
#define	EVHLIGHT2  34
#define	EVHLIGHT3  35
#define	EVHLIGHT4  36
#define	EVHLIGHT5  37
#define	EVHLIGHT6  38
#define	EVHLIGHT7  39
#define	EVHLIGHT8  40
#define	EVHLIGHT9  41
#define EVHJUMP		 42
#define EVINCLD		 43
#define EVKEYCT		 44
#define EVKILL     45
#define EVLANG     46
#define EVLASTDIR	 47
#define EVLASTKEY  48
#define EVLASTMESG 49
#define EVLINE     50
#define EVMATCH    51
#define EVMSFLAG   52
#define EVPAGELEN  53
#define EVPAGEWIDTH 54
#define EVPALETTE  55
#define EVPENDING  56
#define EVPOPUP    57
#define EVREADHK   58
#define	EVREGION	 59
#define EVREPLACE  60
#define EVSEARCH   61
#define EVSEED     62
#define EVSSAVE    63
#define EVSTATUS	 64
#define EVUARG		 65
#define EVVERSION	 66
#define EVWINNEW	 67
#define EVWINTITLE 68
#define EVWLINE 	 69
#define EVWORK		 70
#define EVWRAPHK	 71
#define	EVWRITEHK	 72
#if 0
#define EVXPOS		 73
#define EVYPOS		 74
#define EVZCMD		 75
#else
#define EVZCMD		 73
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
#define g_gmode	    predefvars[EVGMODE].i
#define g_univct    predefvars[EVUARG].i
#define pd_gflags   predefvars[EVGFLAGS].i
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


