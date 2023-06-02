/*	ENGLISH.H:	English language text strings for
                        MicroEMACS 3.10

			(C)opyright 1988 by Daniel Lawrence
*/

#ifdef	maindef
  const   char TEXT8[] = "%b[Aborted]";
/*const*/ char TEXT79[] = "Not found";
  const   char TEXT94[] = "[TABLE OVERFLOW]";
/*const*/ char TEXT23[] = "Out of range";
 
const char TEXTS_[] = "[OUT OF MEMORY]\000Read \000Inserted \000I/O ERROR, ";

#else
extern   const   char TEXT8[];
extern /*const*/ char TEXT79[];
extern   const   char TEXT94[];
extern /*const*/ char TEXT23[];
extern   const   char TEXTS_[];
#endif

#define TEXT99 TEXTS_
#define	TEXT140_O 16
#define	TEXT154_O 22
#define	TEXT141_O 32


#ifdef IN_RANDOM_C

const char attrnames [][8] = {		
 "VW ",    "WRAP ", "OVER ",	"Aa ",	  "RE ",     // names of modes */
 "// ",    "MS ",   "CRYPT ", "ASAVE ", "CHGD", "INVS",
	"BLACK", "RED",     "GREEN", "YELLOW", 					// names of colors */
	"BLUE",  "MAGENTA", "CYAN",  "GREY",
	"GRAY",  "LRED",    "LGREEN","LYELLOW", 
	"LBLUE", "LMAGNTA",	"LCYAN", "WHITE"};

#define NCOLORS 16

static	
const char TEXT64a [][8] = { "delete:", "add:", "toggle:",};

#endif

#define TEXT119 TEXT99

#define	TEXT1	  "[Starting new CLI]"
#define	TEXT2	  "Int Err 1"
#define	TEXT3	  "Return code %d"
#define	TEXT4	  "Shell variable TERM not defined!"
#define	TEXT5	  "Terminal type not 'vt100'!"
#define	TEXT6	  "[End]"
#define	TEXT7	  "Line to GOTO: "
#define	TEXT9	  "Mark %d set"
#define	TEXT10	"Mark %d removed"
#define	TEXT11	"No mark %d here"
#define	TEXT12	"Help file missing"
#define	TEXT13	": describe-key "
#define	TEXT14	"Not Bound"
#define	TEXT15	" \000Command to use "
#define	TEXT16	"No such function"
#define	TEXT17	"Binding table FULL!"
#define	TEXT18	": unbind-key "
#define	TEXT19	"\b[Key not bound]"
#define	TEXT20	"Apropos string: "
#define	TEXT21	"Binding list"
#define	TEXT24	"Use buffer"
#define	TEXT26	"Drop buffer"
#define	TEXT28	"Buffer in use"
//#define	TEXT29	"Change buffer name to: "
#define	TEXT30	"ACT   Modes      Size Buffer          File"
#define	TEXT31	"         Global Modes"
#define	TEXT32	"Discard changes"
#define	TEXT33	"Encryption Key: "
#define	TEXT34	"Cant find THEN"
#define	TEXT35	"%bType Char"
#define	TEXT36	"LOCK ERROR -- "
#define	TEXT37	"checking for existence of %s\n"
#define	TEXT38	"making directory %s\n"
#define	TEXT39	"creating %s\n"
#define	TEXT40	"could not create lock file"
#define	TEXT41	"pid is %ld\n"
#define	TEXT42	"reading lock file %s\n"
#define	TEXT43	"could not read lock file"
#define	TEXT44	"pid in %s is %ld\n"
#define	TEXT45	"signaling process %ld\n"
#define	TEXT46	"process exists"
#define	TEXT47	"kill was bad"
#define	TEXT48	"kill was good; process exists"
#define	TEXT49	"attempting to unlink %s\n"
#define	TEXT50	"could not remove lock file"
#define	TEXT51	"Variable to set: "
#define	TEXT52	"%%No such variable as '%s'%w"
#define	TEXT53	"Value: "
#define	TEXT54	"[Macro aborted]"
#define	TEXT55	"Variable to display: "
#define	TEXT56	"Variable list"
#define	TEXT57	"Can not display variable list"
#define	TEXT59	"[Fill column is %d]"
#define	TEXT60	"Line %d/%d Col %d/%d Char %d/%d (%d%%) char = 0x%x"
#define	TEXT61	"%%Negative argument to kill is illegal"
#define	TEXT62	"Global mode to "
#define	TEXT63	"Mode to "
#define	TEXT64	"Found typ %d"
#define	TEXT65	"Type {([])}"
#define	TEXT66	"No such mode!"
#define	TEXT67	"Message to write: "
#define	TEXT68	"String to insert:"
#define	TEXT69	"String to overwrite:"
#define	TEXT70	"[region copied]"
#define	TEXT71	"%%buffer already narrowed"
#define	TEXT72	"%%Too narrow"
#define	TEXT73	"[Buffer is narrowed]"
#define	TEXT74	"%%buffer not narrowed"
#define	TEXT75	"[Buffer is widened]"
#define	TEXT76	"No mark set in this window"
#define	TEXT77	"Bug:lost mark"
#define	TEXT78	"No pattern set"
#define	TEXT80	"Reverse Search"
#define	TEXT80_O 8
#define	TEXT82  "Query Replace"
#define	TEXT82_O 6
#define	TEXT84  "with"
#define	TEXT85  "[Not Found] %s"
#define	TEXT86  "Earliest Visited"
#define	TEXT87	"Replace '"
#define	TEXT88	"' with '"
#define	TEXT89	"Aborted!"
#define	TEXT90	"(Y)es,(N)o,(L)ast time,(!)Do rest,(U)ndo last,(^G)Abort,(.)Abort back:"
#define	TEXT91	"Empty string replaced, stopping."
#define	TEXT92	"%d substitutions"
#define	TEXT93	"%%ERROR deleting"
//#define	TEXT95	"%%mceq: what is %d?"
#define	TEXT97	"%%Missing ]"
#define	TEXT98	"No fill column set"
#define	TEXT100	"Words %D Chars %D Lines %d Avg chars/word %f"
#define TEXT101 "Password? "
#define TEXT102 "Only one save allowed"
#define	TEXT103	"[Saving %s]\n"
#define	TEXT104	"Modified buffers exist. Leave anyway"
#define	TEXT105	"%%Macro already active"
#define	TEXT106	"[Start macro]"
#define	TEXT107	"%%Macro not active"
#define	TEXT108	"[End macro]"
#define	TEXT109	"%bVIEW mode"
#define	TEXT110	"%b[command is RESTRICTED]"
#define	TEXT111	"No macro specified"
#define	TEXT112	"Macro number oo range"
#define	TEXT113	"Can not create macro"
#define	TEXT114	"Procedure name: "
#define	TEXT115	"Execute procedure: "
#define	TEXT116	"No such procedure"
#define	TEXT117	"Execute buffer: "
#define	TEXT118	"No such buffer"
#define TEXT121 "In %s mismatched:%s%p"
#define	TEXT124	"%%Unknown Directive%p"
#define	TEXT125	"Oo memory storing macro"
#define	TEXT126	"%%While loop int.error%p"
#define	TEXT127	"%%No such label %s%p"
#define	TEXT128	"(e)val exp,(c/x)ommand,(t)rack exp,(^G)abort,<SP>exec,<META> stop debug"
#define	TEXT129	"File to execute: "
#define	TEXT130	"Macro not defined"
#define	TEXT131	"File"
#define	TEXT132	"Insert file"
#define	TEXT133
#define	TEXT134	"View file"
#define	TEXT135	"[Old buffer]"
#define	TEXT136	"Buffer ("
#define	TEXT137	"Cannot create buffer"
#define	TEXT138	"[New file]"
#define	TEXT139	"[Reading file]"
#define	TEXT142	"[no tag file]"
#define	TEXT143	" line"
#define	TEXT144	"Write file: "
#define	TEXT145	"No file name"
#define	TEXT146	"Truncated file..output it"
#define	TEXT147	"Narrowed Buffer..output it"
#define	TEXT148	"[Writing...]"
#define	TEXT149	"Wrote "
#define	TEXT150	", saved as "
#define	TEXT151	"Name: "
#define	TEXT152	"[No such file]"
#define	TEXT153	"[Inserting file]"
#define	TEXT155	"Cannot write file"
#define	TEXT156	"Error closing file"
#define	TEXT157	"Write I/O error"
#define	TEXT158	"File read error"
#define	TEXT159	"\001Wait ..."
#define	TEXT160	"No More Tags"
#define	TEXT161	"[tag %s not in tagfiles]"
#define	TEXT162	" [y/n]? "
#define	TEXT163	""								// not in use
#define	TEXT164	"[search failed]"
#define	TEXT165	"ISearch: "
#define	TEXT166	"too long"
#define	TEXT167	"cmd too long"
#define	TEXT169	"Inserted"	/* this not used anymore */
#define	TEXT170	"bug:linsert"
#define	TEXT171	"Replaced"	/* this not used anymore */
#define	TEXT172	""								// not in use
#define	TEXT173	"LOCK ERROR: Lock table full"
#define	TEXT174	""								// not in use
#define	TEXT175	"LOCK"
#define	TEXT176	"File in use by "
#define	TEXT177	", overide?"
#define	TEXT178	"[can not get system error message]"
#define	TEXT179	" Diry:"
#define	TEXT180	"Use whole file?"
#define	TEXT181	""								// not in use
#define	TEXT182	"Environment variable TERM not defined!"
#define	TEXT183	""
#define	TEXT184	""
#define	TEXT185	""
#define	TEXT186	""
#define	TEXT187	""
#define	TEXT188	""
#define	TEXT189	""
#define	TEXT190	""
#define	TEXT191	""
#define	TEXT192	""
#define	TEXT193	""
#define	TEXT194	""
#define	TEXT195	""
#define	TEXT196	""
#define	TEXT197	""
#define	TEXT198	""
#define	TEXT199	"["
#define	TEXT200	"[Calling DCL]\r\n"
#define	TEXT201	"[Not available yet under VMS]"
#define	TEXT202	"Terminal type not 'vt52'or 'z19' !"
#define	TEXT203	"Nothing there"
#define	TEXT204	"Cannot delete window"
#define	TEXT205	"Cannot split a %d line window"
#define	TEXT206	""
#define	TEXT207	"Impossible change"
#define	TEXT208	"[No such window exists]"
#define	TEXT209	"Impossible screen size"
#define	TEXT211	"Function list"
#define	TEXT212	""
#define	TEXT213	""
#define	TEXT214 "%%No such file as %s"
#define	TEXT215	": macro-to-key "
#define TEXT216 ""
#define TEXT217 ""
#define TEXT218 "Multiple links, unlink result"
#define TEXT219 ""
#define TEXT220 ""
#define TEXT221 "Write to RO file?"
#define TEXT222 "No Password"
#define TEXT223 "Cannot save .e2"
#define TEXT224 " ? extra INCLS path or CR:"
																				/* Must have 5 ../ */
#define TAGFNAME "../../../../../tags"
#define TAGFNLEN 4
