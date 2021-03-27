  *The names of keys*
  (The name of any key and the command it is bound to can be displayed
   by pressing CTRL-X ? x
   where x is the key in question.
  )
  
 FN1 to FN0   The function keys
 FN:, FN;   Function keys 11, 12
 FNC      The insert key
 FND      The delete key
 FN<      The home key
 FN>      The end key
 FNZ      The page-up key
 FNV      The page-down key
 FNB      The left key
 FNP      The up key
 FNF      The right key
 FNN      The down key

 Suffixing ^ to FN, or prefixing ^ to any key denotes
 the control version of that key.

 Prefixing S- denotes the shifted version of that key.

 Prefixing M- denotes prefixing Esc before a key.
 
 Prefixing ^X denotes prefixing ^X before a key.
 
 All of the above combinations are keys, e.g. ^XS-FN6

  *The Conventional Key Bindings*
  M-?   Help
  FN1   Continue search in the same direction as last time
  FN2   Go down a line and then execute the keyboard macro.
  FN^2  Execute the keyboard macro.
  FN3
  FN4   Toggle Insert Mode
  FN9   Beginning of line
  FN10  Beginning of word
  FN11  End of word
  FN12  End of Line
  ^Z    Copy a character from the previous line

  *Key Bindings in the Command Line*
  These are described by pressing ^A in the command line.
	(The bindings can be avoided by prefixing ^Q -- literal next character).
  
  ^B  kill Buffer contents
	^F  The longest match to a file name
  ^G  Abort
  ^K  Chars to eol
  ^N  File Name of buffer
  ^S  Search string
  ^W  Chars to eow
  Arrows    Previous, left, right
	Home
	End
  .^$*[\    Magic

  Pressing Home at the start of the input moves the cursor to the start of the
  command line and thus includes the prompt in the string returned.
  This is useful for the command set %res @"Return this as well:".

  *Invocation Arguments*
  Flags may be interleaved with file names
      -b**name**
            Name the buffer **name**. Useful to override file suffix attributes
      -e    Process error.rc after the startup file
      -m    Show failing macros in a visible buffer
      -i{%$}var value   Set variable with value before startup
      -Kkey   Use encryption key key
      -p    search for the file in $PATH
      -r    Forbid access to the OS
      -s pattern  Set the search pattern; the space is optional
      -/ pattern  Same but also search
      -v    Open files in view mode
            viewflag = TRUE;
      -w#   exit the editor after # seconds inactivity (default 900)
      -x    Keep ^M characters in files being editted/viewed
      -z    select the newset file matching the pattern
      @filename   Name of the startup file; can be relative to emacs diry.
      filename    Filenames may be specified with patterns using *,?
                  Any file name may be **user**@**host**:**file**
                  The first filename may be followed by :**linenumber**

  *Fundamentals*
   The current position can be set as a (zero) mark by the set-mark command. 
   A region is the area from the current position to the zero mark.

   The modes of operation can be added/deleted by add-mode/delete-mode
   and are:
     Ms      file contain(ed) ^M
     AbC     distinguish upper and lower case.
     RE      treat ^$.<asterix>[ in pattern as parts of regular expressions.
						 The regular expressions are POSIX regular expressions.
             (In them \0ab where a and b are hex digits can be used)
     Overstrike  Typing overstrikes existing text.
     View    Disallow editing.
     Crypt   Encrypt when saving, decrypt when reading.
     Ink colours.
        black, red, green, yellow, blue, magenta, cyan, grey, gray, 
        lred, lgreen, lyellow, lblue, lmagenta, lcyan, white
     Paper colours.
        BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, GREY, GRAY, 
        LRED, LGREEN, LYELLOW, LBLUE, LMAGENTA, LCYAN, WHITE
     /       When searching forwards do not ignore text in comments

   A buffer holds text ready to be saved/viewed in a window.
   A window is an area of the screen in which to view a buffer.
   A macro is a program written in .rc script; 
   A key macro is different in kind, being only a sequence of key strokes.

   The only parameter to a command/macro is the count supplied to it
   as a number in scripts or by the prefix Esc n where n is a number.

  *The Colours*
  Setting the above colour modes colour the current window.
  There is also foreground colour applied to text by the 
  comment sequences \/\* \*/  // and the control characters ^A to ^G.
  The variable $cmtcolour specifies an ansii colour.
  The control characters between $col1ch and $col2ch select a palette
  The palette is mapped to colours in $palette.
  On MSDOS the colours are ibmpc colours in pairs of hex digits
  and elsewhere vt100 sequences separated by dots.

  *Tabs*
  The variable $hardtab specifies the distance between tab stops;
  it is stored in each buffer.
  The variable $fileprof specifies $hardtab for file suffixes (starting .).
	The syntax is **suffix** = [^][**type**]#
  The presence of a ^ signifies that the mode of operation is not AbC
	The **type** is one of cpfqPm signifying C/C++,perl,Fortran,SQL,Pascal,markup.
	The number # is the tab size.
  Example : set $fileprof ".sql=^q4.h=c2.c=c2.cpp=c2.py=2"
	The variable $softtab when not zero signifies that tabs are expanded before
  inserting in the buffer; it is stored in each buffer. 
  The command handle-tab (bound to C-I) with an argument also expands the tab..

  *The File Names*
 EMACS decides that two files by different paths are the different.  
 When deciding this hard or symbolic links in the file system are ignored.
 Before comparing paths all sequences abc/../ are removed iteratively.
 Note that any file name starting ./ is treated as different from any 
 file not so starting.

  *Completion*
  The system can (partially) complete certain names; it does this by
  showing the characters which are the same in all forms; it is caused 
  by typing space.
  
  The names that can be completed:
    . File names in Find File, Insert File, (But not New file).
    . EMACS commands
    . EMACS buffer names in select-buffer

  *The Commands*
  (The commands may be listed by pressing M-A CR.)
  (All commands can be aborted by pressing ^G)

abort-command                 abort to the command line
add-global-mode          M-M  Add colours or modes to the default mode
add-mode                 ^XM  Add colours or modes to the buffer/window mode
apropos                  M-A  list commands
backward-character       ^B
                         FNB
backward-delete-character^H
backward-line            ^P
                         FNP
backward-page            FNZ
                         M-V
backward-paragraph       M-P
                         FN^Z
backward-word            M-B
                         FN^B
                         FN0
begin-macro              ^X(  Start collecting keys to be executed by execute-macro
beginning-of-file        M-<
                         FN<
                         M-FNZ
beginning-of-line        ^A
                         FN9
bind-to-key              M-K  (takes command then key)
btk                      M-K  short for bind-to-key
buffer-position          ^X=  gives information
case-region-lower        ^X^L
case-region-upper        ^X^U
case-word-capitalize     M-C  Make the first letter of word upper case
case-word-lower          M-L
case-word-upper          M-U
change-file-name         ^XN  Change the file name of the current buffer
clear-and-redraw         M-^L
clear-message-line
copy-region              ^XC  Copy the region into the n.th kill buffer.
copy-word                A-W  Copy the word into the n.th kill buffer.
count-words              M-^C
ctlx-prefix              ^X
delete-buffer            ^XK  
delete-global-mode       M-^M
delete-mode              ^X^M
delete-next-character    FND
                         
delete-next-word         M-D
delete-other-windows     ^X1
delete-backward-word     M-^H Delete spaces back to the previous word.
delete-window            ^X0
demote-buffer                 Treat the current buffer as if not yet shown
detab-region             ^X^D Expand tabs
display                  ^XG  Display a variable
end-macro                ^X)  End collecting keys to be executed by execute macro.
                         A-FNx (this is alt backslash)
end-of-file              M->
                         M-FNV
end-of-line              ^E
                         FN;
end-of-word
entab-line               ^X^E
entab-region             ^X^E
exchange-point-and-mark  ^X^X
execute-buffer                Execute the .rc commands in a buffer
execute-command-line     M-^X Accept one of these commands on the bottom line.
execute-file             M-^S Execute the .rc commands in a file
execute-macro            ^W   Execute the key macro
                         ^XE
execute-named-command    M-X  Same as execute-command-line
execute-procedure        M-^E Execute named, not numbered macro
execute-program          ^X$  Execute an OS command in the current task
exit-emacs               ^X^C
                         A-3
fetch-file                    The current line contains args <tab> password <tab> opt.
                              The text of args following '/' is the file name.
                              If opt is present this means the file is not encrypted.
                              The command cmd is from the environment variable scp;
                              It must include the option for the password.
                              The default is c:\bin\pscp -pw.
                              Executes command cmd args $(TEMP)/fname.
                              This command is permitted to fail.
                              If opt and the current file is encrypted then
                              the same key decrypts $(TEMP)/fname.
                              The result is in a new buffer.
                              If the command succeeded then save-file saves the file
                              by the command cmd password $(TEMP)/fname args.
															The remote passord must be typed again.
                              Only one save is allowed and obliterates keys.
fill-paragraph           M-Q
filter-buffer            ^X#  Write the buffer to a temp, execute a command
                         ^X\       reading the contents, read in the output.
                         ^X|
                               In Windows for filter-buffer, pipe-command, 
                               shell-command if the command can be found 
                               on %Path% or with suffix .exe added do not 
															 use cmd.exe otherwise use it.
find-file                ^X^F Read a file into a buffer, new if unique in name
find-tag                 M-FN> Searches all files named tags up from the directory
                               of the file in the buffer.
                               The search is for the word under the cursor.
                               The file in the tag file line is put in a buffer.
                               Repeating the command continues the search if
                               the word is the same.
forward-character        ^L
                         FNF
goto-line                M-G
goto-mark                M-^G
goto-matching-fence      M-^F Goto a matching bracket, parenthesis, etc
															If arg != 1 and on left brace use a right brace.
                         ^T
grow-window              ^X^
                         ^XZ
handle-tab               ^I   See above.
help                     M-?
                         A-1
hunt-backward            A-R
hunt-forward             A-S
i-shell                  ^X%  Execute an OS Shell command
incremental-search       ^XS
insert-file              ^X^I Insert a file at the current point
insert-space
insert-string
kill-kill                ^F   Clear the kill buffer
                         ^G
kill-region              ^XD  Kill removes text to the n.th kill buffer
                         FN^D
kill-to-end-of-line      ^K
                         FN^;
last-buffer              M-FNB Show the previous buffer shown
list-bindings        		 Like apropos
list-buffers             ^X^B
list-key                 ^X?
list-variables                 List values of variables
meta-prefix              ^[    Not usable
move-window-down         ^X^N
move-window-up           ^X^P
name-buffer              M-^N
narrow-to-region
newline
newline-and-indent       ^J
                         ^M
next-buffer              ^XX  Show the next buffer, possibly not yet shown
                         M-FNF
next-line                FNN
next-page                ^V
                         FNV
                         A-8
next-paragraph           M-N
                         FN^V
next-window              ^XO
                         M-FNN
next-word                M-F
                         FN^F
                         FN:
nop
overwrite-string
pipe-command             ^X@  Execute a command and use a new buffer for output
                         ^X<  For command see filter-buffer.
previous-window          ^XP
print
query-replace-string     ^R   An illegal response shows legal choices
                         M-^R
quick-exit               M-Z
quote-character          ^Q   Take the following keypress as a letter
read-file                     Clear the current buffer and read in a file
redraw-display           ^L   Redraw with current position in middle of buffer
remove-mark
replace-string           M-R
resize-window            ^XW
reverse-incremental-search^XR
run                      M-^E
save-file                ^X^S Write out a file
scroll-next-down         M-^V
scroll-next-up           M-^Z
search-forward           ^S
search-incls             M-FNP  Search for a define,variable through C++ includes
        When asking about include files:
          SP   Search it and ask no more even on failure.
          CR   Search it
          N    Dont search it
          1    Stop at line 1 of the include file
          ^G   Abort
        The variable $incldirs is initialised from 
        environment variable INCLUDE. Include files are
        searched for using path $incldirs 
        (; (or : in Unix) separates directories.)

				When asking and an include file is not found then an opportunity is
				offered to append new directories to $incldirs.

search-reverse           M-S
select-buffer                 Show the buffer in the current window
set                      ^XA  Set a variable
set-encryption-key       M-E	Per buffer; 
                              For all encryption keys pressing and releasing the
                              control key at a specific point affects the key.
                              Assists in keeping the key secret. (Windows only)
set-fill-column          ^XF  For use in word processing
set-mark                 M- 
                         M-.
shell-command            ^X!  Execute an OS command in a new task
                         A-6
shrink-window            ^X^Z
source                   M-^S execute-file
split-current-window     ^X2
                         A-2
store-macro
store-procedure
trim-line                ^X^T
trim-region              ^X^T
unbind-key               M-^K
universal-argument       ^U
unmark-buffer            M-~
update-screen
view-file                ^X^V
widen-from-region        ^X>
word-search              A-Z  Word to the left of cursor: 
                               Find backwards a word so starting
                               Insert it replacing word to the left of cursor.
wrap-word
write-file               ^X^W
write-message
yank                     ^Y Insert the contents of the kill buffer here 
                         M-FNC  (n>0 => insert n times, -n>0=> nth kill buffer)

 *DEFAULT ASSIGNMENTS OF THE MACROS FROM EMACS.RC*

 The argument ESC n to these macros is the number of times to execute the macro.

execute-macro-1          S-FN1  Change case one character
execute-macro-2          S-FN2  Drop the buffer, %goon: take new one, none: exit
                         M-\
                         M-`
                         ^]
execute-macro-3          S-FN3  Save buffer then execute-macro-2
                         M-/
execute-macro-4          ^X^R Drop the current buffer, then find-file
                         S-FN4
execute-macro-5          ^N Search in the last direction
                         S-FN5
                         FN1
execute-macro-6          S-FN6  Toggle msb of character
                         ^^
execute-macro-7          S-FN7  Search for NN; replace it by %nn+1, increment %nn
                         FN4
execute-macro-8          FN>  Copy one character from previous line
                         S-FN8
                         ^Z
execute-macro-9          ^C Copy one line to the kill buffer
                         S-FN9
execute-macro-10         S-FN0  Go down one line then execute-macro
                         FN2
execute-macro-11         ^D Cut entire line to kill buffer
execute-macro-13         M-I  Bring in more macros, e.g. in xmacs.rc
execute-macro-12
execute-macro-14         FNC  Insert contents of the kill buffer, clear it.
execute-macro-15         A-Q  Customise a user variable
execute-macro-16         M-FND Delete region
execute-macro-17         FN^P	search backwards for word right of cursor
                         FN^N	search forwards for word right of cursor
execute-macro-18
execute-macro-19         ^O Display command for editting the pipe-command it
														See above for what Home key does.
execute-macro-20         ^P search for pw: from eol, copy text to clipboard
execute-macro-21         
execute-macro-22         
execute-macro-23         
execute-macro-24         
execute-macro-25
execute-macro-26
execute-macro-27
execute-macro-28
execute-macro-29
execute-macro-30
execute-macro-31
execute-macro-32
execute-macro-33
execute-macro-34
execute-macro-35
execute-macro-36
execute-macro-37
execute-macro-38
execute-macro-39
execute-macro-40

Note that many macros are overloaded and use the keycode which invoked them.

For example macro-18 is bound to both A-FNB and A-FNF. (Alt the arrow keys).
These bindings search for the word to the right of the cursor backwards resp. forwards.

  The Variables
  -------------
  (The variables can be set using the set command.)
  (All but termcap variables can be shown with the describe-variables command.)

 The following bits are used in flags

 BFINVS   0x01    /* Internal invisable buffer  */
 BFCHG    0x02    /* Changed since last write */
 BFTRUNC  0x04    /* buffer was truncated when read */
 BFNAROW  0x08    /* buffer has been narrowed */
 MDSTT    0x0010
 MDWRAP   0x0010    /* word wrap      */
 MDCMOD   0x0020    /* C indentation and fence match*/
 MDMS     0x0040    /* File to have CRLF    */
 MDEXACT  0x0080    /* Exact matching for searches  */
 MDVIEW   0x0100    /* read-only buffer   */
 MDOVER   0x0200    /* overwrite mode   */
 MDMAGIC  0x0400    /* regular expresions in search */
 MDCRYPT  0x0800    /* encrytion mode active  */
 MDASAVE  0x1000    /* auto-save mode   */
 MDSRCHC  0x2000    /* search comments also */
 MDDIR    0x4000		/* this file is a directory	*/
 BFACTIVE 0x8000		/* this buffer is active (read in) */

 The System variables:
 --------------------
$acount        
$asave         
$bufhook       
$cbflags       Flags of current buffer.
$cbufname      Current buffer name (read only).
$cfname        Name of file in current buffer
$cliplife      Cut/paste buffer entries older than this are silently deleted!!
$cmdhook       
$cmode         The mode on the current buffer
Colours: 
               On IBMPC BG_COLOUR * 16 + FG_COLOUR + (8:intense)
               Elsewhere selects from the palette library $palette
$cmtcolour     Comment colour
$col1ch        Lowest palette selector character
$col2ch        Highest palette selector character
               If a character is within the range of the two characters
               above then a colour is selected from $palette
$curchar       The current character code. &chr $curchar is the current char.
$curcol        The current column
$curline       The Current line number
$curwidth      The width of the screen
$cwline        The line number in the window
$debug         When set, make available debug information, if compiled in
$diagflag      
$discmd        Display command output
$disinp        Inhibit all output
$exbhook       
$fcol          Column in window column 0 
$fileprof      See above
$fillcol       Right margin for text wrapping.
$gflags        Global Flags
$gmode         The default (i.e. global) mode for buffers
$hardtab       The tab stop interval
$highlight     The first character is a digit selecting a colour from $palette
               All sequences the same as the rest of the characters are
               highlighted in that colour.
$hjump         Horizontal scrolling jump
$hscroll       Enable $hjump
$incldirs      Path of directories to search with srch-incls
$keycount      Keys pressed since last command
$kill          The kill buffer
$language      english
$lastdir       Last direction of search
$lastkey       The last key pressed
$lastmesg      The last message shown
$line          The line of the current buffer (read only)
$match         The last string matched 
$msflag        1: file had ^M on input
$pagelen       Lines on the screen. Changing this changes the physical length
$pagewidth		 Width of the screen. Changing this changes the physical width
$palette       A dot separated list of strings str giving colours ESC [ <str> m 
               On msdos it is not this but a sequence of hex digit pairs
               describing colours on the MSDOS screen
$pending       Read only; typahead text 
$popup				 Write only; Raise a pop-up with this text.
$ram           Not implemented
$readhook      
$region        A prefix of the text in the marked region
$replace       The last replace string typed 
$rval
$search        The last search string typed
$seed          The random number seed
$sres          display type
$ssave         Save files to an intermediate temporary
$sscroll       Smooth scroll
$status        Status of the last editor command
$sterm         Terminator of command input, usually ^M
$uarg          ESC arg to command
$usesofttab    != 0: Expand tabs to to spaces
$version       This program version
$wintitle			 Write only; Change the window title
$wline         Number of lines in the current window
$wraphook      
$writehook     
$xpos          X position on the screen
$ypos          Y position on the screen
$zcmd          The last command

 The User variables:
 --------------------
  (Only used by Emacs macros!)
%goon   When dropping a buffer, go (0: back to last, 1: on to next)
%nn   The previous number replacing NN when generating number 
          sequences.

 The Terminal Control Variables
 ------------------------------
   (These variables control the way the terminal is controlled.
    They apply only to Unix.)
   
#cols   The number of columns on the screen
#colors   Not in use
#csr    The sequence to define the scroll region
#cup    The sequence to position the cursor
#dl1    The sequence to go down a line (only used if #csr = "")
#ed   The sequence to erase to end of the screen
#el   The sequence to erase to end of the line
#il1    The sequence to go up a line (only used if #csr = "")
#ind    The sequence to scroll the window down, text up
#is1    The sequence to initialise the terminal for EMACS
#kich1    Another sequence to initialise the terminal.
#lines    The number of lines on the screen to be used
#rev    
#ri   The sequence to scroll the window up, text down
#rs1    The sequence to restore the terminal after EMACS use
#sgr0   The sequence to unset reverse video (only after the modeline)

   (Currently the variables giving the sequences sent by the keyboard keys
    are not readable/writable under EMACS but may be changed by recompilation
    of terminal.c
   )

 The .rc Functions
 -----------------
  (The following functions can be used in .rc files.)

	MONAMIC, RSTR, "@",			/* Prompts for a value using value */
  MONAMIC, RINT, "abs",   /* absolute value of a number */
  DYNAMIC, RINT, "add",   /* add two numbers together */
  DYNAMIC, RSTR, "and",   /* logical and */
  MONAMIC, RINT, "asc",   /* char to integer conversion */
  DYNAMIC, RINT, "ban",   /* bitwise and   9-10-87  jwm */
  MONAMIC, RINT, "bin",   /* loopup what function name is bound to a key */
  MONAMIC, RINT, "bno",   /* bitwise not */
  DYNAMIC, RINT, "bor",   /* bitwise or  9-10-87  jwm */
  DYNAMIC, RINT, "bxo",   /* bitwise xor   9-10-87  jwm */
  DYNAMIC, RINT, "cat",   /* concatinate string */
  MONAMIC, RINT, "chr",   /* integer to char conversion */
  DYNAMIC, RINT, "dir", /* replace tail of filename with filename */
  NILNAMIC, RINT, "dit",  /* the character in the line above */
  DYNAMIC, RINT, "div",   /* division */
  MONAMIC, RINT, "env",   /* retrieve a system environment var */
  DYNAMIC, RSTR, "equ",   /* logical equality check */
  MONAMIC, RSTR, "exi",   /* check if a file exists */
  MONAMIC, RINT, "fin",   /* look for a file on the path... */
  DYNAMIC, RSTR, "gre",   /* logical greater than */
  NILNAMIC, RINT, "gtc",  /* get 1 emacs command */
  NILNAMIC, RINT, "gtk",  /* get 1 charater */
  MONAMIC, RINT, "ind",   /* evaluate indirect value */
  DYNAMIC, RINT, "lef",   /* left string(string, len) */
  MONAMIC, RINT, "len",   /* string length */
  DYNAMIC, RSTR, "les",   /* logical less than */
  MONAMIC, RINT, "low",   /* lower case string */
  TRINAMIC, RINT, "mid",  /* mid string(string, pos, len) */
  DYNAMIC, RINT, "mod",   /* mod */
  MONAMIC, RINT, "neg",   /* negate */
  MONAMIC, RSTR, "not",   /* logical not */
  DYNAMIC, RSTR, "or",  /* logical or */
  DYNAMIC, RINT, "rig",   /* right string(string, pos) */
  MONAMIC, RINT, "rnd",   /* get a random number */
  DYNAMIC, RSTR, "seq",   /* string logical equality check */
  DYNAMIC, RSTR, "sgr",   /* string logical greater than */
  DYNAMIC, RINT, "sin",   /* find the index of one string in another */
  DYNAMIC, RSTR, "sle",   /* string logical less than */
  DYNAMIC, RINT, "slo", /* set lower to upper char translation */
  DYNAMIC, RINT, "sub",   /* subtraction */
  DYNAMIC, RINT, "sup", /* set upper to lower char translation */
  DYNAMIC, RINT, "tim",   /* multiplication */
  MONAMIC, RINT, "tri", /* trim whitespace off the end of a string */
  MONAMIC, RINT, "upp",   /* uppercase string */
  TRINAMIC,RINT, "xla", /* XLATE character string translation */

