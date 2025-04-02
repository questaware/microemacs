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
#define EVINCLD		 27
#define EVKEYCT		 28
#define EVKILL     29
#define EVLANG     30
#define EVLASTDIR	 31
#define EVLASTKEY  32
#define EVLASTMESG 33
#define EVLINE     34
#define EVMATCH    35
#define EVNOINDENT 36
#define EVPAGELEN  37
#define EVPAGEWIDTH 38
#define EVPALETTE  39
#define EVPENDING  40
#define EVPOPUP    41
#define EVREADHK   42
#define	EVREGION	 43
#define EVREPLACE  44
#define EVSEARCH   45
#define EVSEED     46
#define EVSSAVE    47
#define EVSTATUS	 48
#define EVUARG		 49
#define EVUNDOS		 50
#define EVVERSION	 51
#define EVWINEW	   52
#define EVWINTITLE 53
#define EVWLINE 	 54
#define EVWORK		 55
#define EVWRAPHK	 56
#define	EVWRITEHK	 57
#if 0
#define EVXPOS		 58
#define EVYPOS		 59
#define EVZCMD		 60
#else
#define EVZCMD		 58
#endif
#define	EVHLIGHT1  (EVZCMD+1)

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
#define pd_focus_count predefvars[EVLANG].i
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


