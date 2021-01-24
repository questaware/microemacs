/*	EFUNC.H:	MicroEMACS function declarations and names

	This file list all the C code functions used by MicroEMACS
	and the names to use to bind keys to them. To add functions,
	declare it here in both the extern function list and the name
	binding table.
*/
#if	ISRCH
static const char irsrchm[] = "reverse-incremental-search";
#define ISRCHM_OFFS 8
#endif

NOSHARE const NBIND names[] = 
{	{"abort-command",				ctrlg},
	{"add-global-mode",			setgmode},
	{"add-mode",						setmod},
#if	APROP
	{"apropos",							apro},
#endif
	{"arith",		        		arith},
	{"backward-character",	backchar},
	{"backward-delete-char",backdel},
	{"backward-line",				backline},
	{"backward-page",				backpage},
#if	WORDPRO
	{"backward-paragraph",	gotobop},
#endif
	{"backward-word",				backword},
	{"begin-macro",					ctlxlp},
	{"beginning-of-file",		gotobob},
	{"beginning-of-line",		gotobol},
	{"bind-to-key",					bindtokey},
	{"btk",									bindtokey},
	{"buffer-mode",					togmod},
	{"buffer-position",			showcpos},
	{"case-region-lower",		lowerregion},
	{"case-region-upper",		upperregion},
	{"case-word-capitalize",capword},
	{"case-word-lower",			lowerword},
	{"case-word-upper",			upperword},
	{"change-file-name",		filename},
	{"clear-and-redraw",		refresh},
	{"clear-message-line",	clrmes},
	{"copy-region",					copyregion},
	{"copy-word",						copyword},
#if	WORDPRO
	{"count-words",					wordcount},
#endif
	{"ctlx-prefix",					cex},
	{"delete-backward-word",delbword},
	{"delete-buffer",				killbuffer},
	{"delete-global-mode",	delgmode},
	{"delete-mode",					delmode},
	{"delete-next-character",forwdel},
	{"delete-next-word",		delfword},
	{"delete-other-windows",onlywind},
	{"delete-window",				delwind},
	{"demote-buffer",				hidebuffer},
	{"describe-bindings",		desbind},
#if	AEDIT
	{"detab-region",				detab},
#endif
#if	DEBUGM
	{"display",							dispvar},
#endif
	{"end-macro",						ctlxrp},
	{"end-of-file",					gotoeob},
	{"end-of-line",					gotoeol},
	{"end-of-word",					endword},
#if	AEDIT
	{"entab-region",				entab},
#endif
	{"exchange-point-and-mark",swapmark},
	{"execute-buffer",			execbuf},
#if FLUFF
	{"execute-command-line",execcmd},
#endif
	{"execute-file",				execfile},
	{"execute-macro",		ctlxe},
	{"execute-macro-1",	cbuf1},
	{"execute-macro-10",cbuf10},
	{"execute-macro-11",cbuf11},
	{"execute-macro-12",cbuf12},
	{"execute-macro-13",cbuf13},
	{"execute-macro-14",cbuf14},
	{"execute-macro-15",cbuf15},
	{"execute-macro-16",cbuf16},
	{"execute-macro-17",cbuf17},
	{"execute-macro-18",cbuf18},
	{"execute-macro-19",cbuf19},
	{"execute-macro-2",	cbuf2},
	{"execute-macro-20",cbuf20},
	{"execute-macro-21",cbuf21},
	{"execute-macro-22",cbuf22},
	{"execute-macro-23",cbuf23},
	{"execute-macro-24",cbuf24},
	{"execute-macro-25",cbuf25},
	{"execute-macro-26",cbuf26},
	{"execute-macro-27",cbuf27},
	{"execute-macro-28",cbuf28},
	{"execute-macro-29",cbuf29},
	{"execute-macro-3",	cbuf3},
	{"execute-macro-30",cbuf30},
	{"execute-macro-31",cbuf31},
	{"execute-macro-32",cbuf32},
	{"execute-macro-33",cbuf33},
	{"execute-macro-34",cbuf34},
	{"execute-macro-35",cbuf35},
	{"execute-macro-36",cbuf36},
	{"execute-macro-37",cbuf37},
	{"execute-macro-38",cbuf38},
	{"execute-macro-39",cbuf39},
	{"execute-macro-4",	cbuf4},
	{"execute-macro-40",cbuf40},
	{"execute-macro-5",	cbuf5},
	{"execute-macro-6",	cbuf6},
	{"execute-macro-7",	cbuf7},
	{"execute-macro-8",	cbuf8},
	{"execute-macro-9",	cbuf9},

	{"execute-named-command",	namedcmd},
#if	NMDPROC
	{"execute-procedure",		execproc},
#endif
	{"exit-emacs",					quit},
	{"fetch-file",					fetchfile},
#if	WORDPRO
	{"fill-paragraph",			fillpara},
#endif
	{"filter-buffer",				filter},
	{"find-file",						filefind},
#if MEOPT_TAGS
	{"find-tag",						findTag},
#endif
	{"forward-character",		forwchar},
	{"get-incls",						getIncls},
	{"global-mode",					toggmode},
	{"goto-line",						gotoline},
	{"goto-mark",						gotomark},
#if	CFENCE
	{"goto-matching-fence",	getfence},
#endif
	{"grow-window",					enlargewind},
	{"handle-tab",					handletab},
	{"help",								help},
	{"hunt-backward",				backhunt},
	{"hunt-forward",				forwhunt},
	{"i-shell",							spawncli},
#if	ISRCH
	{&irsrchm[ISRCHM_OFFS],	fisearch},
#endif
  {"indent-search",   		indentsearch},
	{"insert-file",					insfile},
	{"insert-space",				insspace},
	{"insert-string",				istring},
	{"kill-kill",						kdelete},
#if	WORDPRO && 0
	{"kill-paragraph",			killpara},
#endif
	{"kill-region",					killregion},
	{"kill-to-end-of-line",	killtext},
#if	FLABEL
	{"label-function-key",	fnclabel},
#endif
	{"last-buffer",					lastbuffer},
	{"list-buffers",				listbuffers},
#if	DEBUGM || 1
	{"list-functions",			desfunc},
#endif
	{"list-key",						deskey},
	{"list-variables",			desvars},
	{"macro-to-key",				macrotokey},
	{"meta-prefix",					meta},
#if	MOUSE
	{"mouse-move-down",			movemd},
	{"mouse-move-up",				movemu},
	{"mouse-region-down",		mregdown},
	{"mouse-region-up",			mregup},
	{"mouse-resize-screen",	resizm},
#endif
	{"move-window-down",		mvdnwind},
	{"move-window-up",			mvupwind},
#if 0
	{"name-buffer",					namebuffer},
#endif
	{"narrow-to-region",		narrow},
	{"newline",							ins_newline},
	{"newline-and-indent",	indent},
	{"next-buffer",					nextbuffer},
	{"next-line",						forwline},
	{"next-page",						forwpage},
#if	WORDPRO
	{"next-paragraph",			gotoeop},
#endif
	{"next-window",					nextwind},
	{"next-word",						nextword},
	{"nop",									nullproc},
#if FLUFF
	{"open-line",						openline},
#endif
	{"overwrite-string",		ovstring},
	{"pipe-command",				pipecmd},
	{"previous-window",			prevwind},
	{"print",								writemsg},
	{"query-replace-string",qreplace},
#if S_MSDOS == 0
	{"quick-exit",					quickexit},
#endif
	{"quote-character",			quote},
	{"read-file",						fileread},
	{"redraw-display",			reposition},
	{"remove-mark",					remmark},
	{"replace-string",			sreplace},
	{"resize-window",				resize},
	{"restore-window",			restwnd},
#if	ISRCH
	{irsrchm,								risearch},
#endif
#if	NMDPROC
	{"run",									execproc},
#endif
	{"save-file",						filesave},
	{"save-window",					savewnd},
	{"scroll-next-down",		nextdown},
	{"scroll-next-up",			nextup},
	{"search-forward",			forwsearch},
	{"search-incls",				searchIncls},
	{"search-reverse",			backsearch},
	{"select-buffer",				usebuffer},
	{"set",									setvar},
#if	CRYPT
	{"set-encryption-key",	setuekey},
#endif
	{"set-fill-column",			setfillcol},
	{"set-mark",						setmark},
	{"shell-command",				spawn},
	{"shrink-window",				shrinkwind},
	{"source",							execfile},
	{"split-current-window",splitwind},
	{"store-macro",					storemac},
#if	NMDPROC
	{"store-procedure",			storeproc},
#endif
#if	S_BSD
	{"suspend-emacs",				bktoshell},
#endif
#if FLUFF
	{"transpose-characters",twiddle},
#endif
#if	AEDIT
	{"trim-region",					trim_white},
#endif
	{"uarg",								uniarg},
	{"unbind-key",					unbindkey},
	{"unmark-buffer",				unmark},
	{"update-screen",				upscreen},
	{"view-file",						viewfile},
	{"widen-from-region",		widen},
	{"word-search",					wordsearch},
	{"wrap-word",						wrapword},
	{"write-file",					filewrite},
	{"write-message",				writemsg},
	{"yank",								yank},
	{"",			NULL}
};

#define	NCMDS	(sizeof(names)/sizeof(NBIND)) - 1

