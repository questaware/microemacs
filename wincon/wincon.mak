# Microsoft Developer Studio Generated NMAKE File, Based on wincon.dsp
!IF "$(CFG)" == ""
CFG=wincon - Win32 Debug
!MESSAGE No configuration specified. Defaulting to wincon - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "wincon - Win32 Release" && "$(CFG)" != "wincon - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wincon.mak" CFG="wincon - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wincon - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "wincon - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "wincon - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\wincon.exe"


CLEAN :
	-@erase "$(INTDIR)\basic.obj"
	-@erase "$(INTDIR)\bind.obj"
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\char.obj"
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\display.obj"
	-@erase "$(INTDIR)\dolock.obj"
	-@erase "$(INTDIR)\msemacs.obj"
	-@erase "$(INTDIR)\eval.obj"
	-@erase "$(INTDIR)\exec.obj"
	-@erase "$(INTDIR)\execasm.obj"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\isearch.obj"
	-@erase "$(INTDIR)\line.obj"
	-@erase "$(INTDIR)\logmsg.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\malloc.obj"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\mouse.obj"
	-@erase "$(INTDIR)\msdir.obj"
	-@erase "$(INTDIR)\msexec.obj"
	-@erase "$(INTDIR)\oswin.obj"
	-@erase "$(INTDIR)\random.obj"
	-@erase "$(INTDIR)\re.obj"
	-@erase "$(INTDIR)\region.obj"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\sinc.obj"
	-@erase "$(INTDIR)\tag.obj"
	-@erase "$(INTDIR)\vc70.idb"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\word.obj"
	-@erase "$(OUTDIR)\wincon.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /W3 /O1 /Os /Og /GF /Zp4 /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /c 

#.c{$(INTDIR)}.obj::
#   $(CPP) $(CPP_PROJ) $< 

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

#.c{$(INTDIR)}.sbr::
#   $(CPP) @<<
#   $(CPP_PROJ) $< 
#<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

# release

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wincon.bsc" 
BSC32_SBRS= \

LINK32=cl
#LINK32_FLAGS=kernel32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\wincon.pdb" /machine:I386 /out:"$(OUTDIR)\wincon.exe" 
LINK32_FLAGS=/nodedefaultlib /ML /Fe"$(OUTDIR)\wincon.exe" 
#LINK32_FLAGS=/nodefaultlib /nologo /subsystem:console /incremental:no /machine:I386 /out:"$(OUTDIR)\wincon.exe" 
LINK32_OBJS= \
	"$(INTDIR)\basic.obj" \
	"$(INTDIR)\bind.obj" \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\char.obj" \
	"$(INTDIR)\crypt.obj" \
	"$(INTDIR)\display.obj" \
	"$(INTDIR)\dolock.obj" \
	"$(INTDIR)\eval.obj" \
	"$(INTDIR)\exec.obj" \
	"$(INTDIR)\execasm.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\isearch.obj" \
	"$(INTDIR)\line.obj" \
	"$(INTDIR)\logmsg.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\malloc.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\mouse.obj" \
	"$(INTDIR)\msdir.obj" \
	"$(INTDIR)\msexec.obj" \
	"$(INTDIR)\oswin.obj" \
	"$(INTDIR)\random.obj" \
	"$(INTDIR)\re.obj" \
	"$(INTDIR)\region.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\sinc.obj" \
	"$(INTDIR)\tag.obj" \
	"$(INTDIR)\window.obj" \
	"$(INTDIR)\word.obj" \
	"$(INTDIR)\msemacs.obj" \


"$(OUTDIR)\wincon.exe" : "$(OUTDIR)" $(LINK32_OBJS)
    cl @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\wincon.exe" "$(OUTDIR)\wincon.bsc"


CLEAN :
	-@erase "$(INTDIR)\basic.obj"
	-@erase "$(INTDIR)\basic.sbr"
	-@erase "$(INTDIR)\bind.obj"
	-@erase "$(INTDIR)\bind.sbr"
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\buffer.sbr"
	-@erase "$(INTDIR)\char.obj"
	-@erase "$(INTDIR)\char.sbr"
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\crypt.sbr"
	-@erase "$(INTDIR)\display.obj"
	-@erase "$(INTDIR)\display.sbr"
	-@erase "$(INTDIR)\dolock.obj"
	-@erase "$(INTDIR)\dolock.sbr"
	-@erase "$(INTDIR)\eval.obj"
	-@erase "$(INTDIR)\eval.sbr"
	-@erase "$(INTDIR)\exec.obj"
	-@erase "$(INTDIR)\exec.sbr"
	-@erase "$(INTDIR)\execasm.obj"
	-@erase "$(INTDIR)\execasm.sbr"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\file.sbr"
	-@erase "$(INTDIR)\fileio.obj"
	-@erase "$(INTDIR)\fileio.sbr"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\input.sbr"
	-@erase "$(INTDIR)\isearch.obj"
	-@erase "$(INTDIR)\isearch.sbr"
	-@erase "$(INTDIR)\line.obj"
	-@erase "$(INTDIR)\line.sbr"
	-@erase "$(INTDIR)\logmsg.obj"
	-@erase "$(INTDIR)\logmsg.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\malloc.obj"
	-@erase "$(INTDIR)\malloc.sbr"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\map.sbr"
	-@erase "$(INTDIR)\mouse.obj"
	-@erase "$(INTDIR)\mouse.sbr"
	-@erase "$(INTDIR)\msdir.obj"
	-@erase "$(INTDIR)\msdir.sbr"
	-@erase "$(INTDIR)\msexec.obj"
	-@erase "$(INTDIR)\msexec.sbr"
	-@erase "$(INTDIR)\oswin.obj"
	-@erase "$(INTDIR)\oswin.sbr"
	-@erase "$(INTDIR)\random.obj"
	-@erase "$(INTDIR)\random.sbr"
	-@erase "$(INTDIR)\re.obj"
	-@erase "$(INTDIR)\re.sbr"
	-@erase "$(INTDIR)\region.obj"
	-@erase "$(INTDIR)\region.sbr"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\search.sbr"
	-@erase "$(INTDIR)\sinc.obj"
	-@erase "$(INTDIR)\sinc.sbr"
	-@erase "$(INTDIR)\tag.obj"
	-@erase "$(INTDIR)\tag.sbr"
	-@erase "$(INTDIR)\vc70.idb"
	-@erase "$(INTDIR)\vc70.pdb"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\window.sbr"
	-@erase "$(INTDIR)\word.obj"
	-@erase "$(INTDIR)\word.sbr"
	-@erase "$(OUTDIR)\wincon.bsc"
	-@erase "$(OUTDIR)\wincon.exe"
	-@erase "$(OUTDIR)\wincon.ilk"
	-@erase "$(OUTDIR)\wincon.pdb"
	-@erase "$(INTDIR)\msemacs.obj"
	-@erase "$(INTDIR)\msemacs.sbr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /W3 /Zi /Od /I "../src" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) $(CPP_PROJ) $< 

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wincon.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\basic.sbr" \
	"$(INTDIR)\bind.sbr" \
	"$(INTDIR)\buffer.sbr" \
	"$(INTDIR)\char.sbr" \
	"$(INTDIR)\crypt.sbr" \
	"$(INTDIR)\display.sbr" \
	"$(INTDIR)\dolock.sbr" \
	"$(INTDIR)\eval.sbr" \
	"$(INTDIR)\exec.sbr" \
	"$(INTDIR)\execasm.sbr" \
	"$(INTDIR)\file.sbr" \
	"$(INTDIR)\fileio.sbr" \
	"$(INTDIR)\input.sbr" \
	"$(INTDIR)\isearch.sbr" \
	"$(INTDIR)\line.sbr" \
	"$(INTDIR)\logmsg.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\malloc.sbr" \
	"$(INTDIR)\map.sbr" \
	"$(INTDIR)\mouse.sbr" \
	"$(INTDIR)\msdir.sbr" \
	"$(INTDIR)\msexec.sbr" \
	"$(INTDIR)\oswin.sbr" \
	"$(INTDIR)\random.sbr" \
	"$(INTDIR)\re.sbr" \
	"$(INTDIR)\region.sbr" \
	"$(INTDIR)\search.sbr" \
	"$(INTDIR)\sinc.sbr" \
	"$(INTDIR)\tag.sbr" \
	"$(INTDIR)\window.sbr" \
	"$(INTDIR)\word.sbr" \
	"$(INTDIR)\msemacs.sbr"

"$(OUTDIR)\wincon.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=cl.exe
LINK32_FLAGS=/Zi /MLd /Fe"$(OUTDIR)\wincon.exe"
LINK32_OBJS= \
	"$(INTDIR)\basic.obj" \
	"$(INTDIR)\bind.obj" \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\char.obj" \
	"$(INTDIR)\crypt.obj" \
	"$(INTDIR)\display.obj" \
	"$(INTDIR)\dolock.obj" \
	"$(INTDIR)\eval.obj" \
	"$(INTDIR)\exec.obj" \
	"$(INTDIR)\execasm.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\fileio.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\isearch.obj" \
	"$(INTDIR)\line.obj" \
	"$(INTDIR)\logmsg.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\malloc.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\mouse.obj" \
	"$(INTDIR)\msdir.obj" \
	"$(INTDIR)\msexec.obj" \
	"$(INTDIR)\oswin.obj" \
	"$(INTDIR)\random.obj" \
	"$(INTDIR)\re.obj" \
	"$(INTDIR)\region.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\sinc.obj" \
	"$(INTDIR)\tag.obj" \
	"$(INTDIR)\window.obj" \
	"$(INTDIR)\word.obj" \
	"$(INTDIR)\msemacs.obj"

#  $(LINK32_FLAGS) $(LINK32_OBJS) /link /nodefaultlib:libc.lib 
"$(OUTDIR)\wincon.exe" : "$(OUTDIR)" $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS) /link 
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("wincon.dep")
!INCLUDE "wincon.dep"
!ELSE 
!MESSAGE Warning: cannot find "wincon.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "wincon - Win32 Release" || "$(CFG)" == "wincon - Win32 Debug"
SOURCE=..\src\basic.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\basic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP)  $(CPP_SWITCHES) $(SOURCE)


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\basic.obj"	"$(INTDIR)\basic.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_SWITCHES) $(SOURCE)


!ENDIF 

SOURCE=..\src\bind.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bind.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bind.obj"	"$(INTDIR)\bind.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\buffer.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /c 

"$(INTDIR)\buffer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\buffer.obj"	"$(INTDIR)\buffer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\char.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\char.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\char.obj"	"$(INTDIR)\char.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\crypt.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\crypt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\crypt.obj"	"$(INTDIR)\crypt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\display.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\display.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\display.obj"	"$(INTDIR)\display.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\dolock.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dolock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dolock.obj"	"$(INTDIR)\dolock.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\emacs.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\emacs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\emacs.obj"	"$(INTDIR)\emacs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\msemacs.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\msemacs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_SWITCHES) $(SOURCE)


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\msemacs.obj"	"$(INTDIR)\msemacs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\eval.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\eval.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\eval.obj"	"$(INTDIR)\eval.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\exec.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\exec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\exec.obj"	"$(INTDIR)\exec.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\execasm.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\execasm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\execasm.obj"	"$(INTDIR)\execasm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\file.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\file.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\file.obj"	"$(INTDIR)\file.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\fileio.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\fileio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\fileio.obj"	"$(INTDIR)\fileio.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\input.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\input.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\input.obj"	"$(INTDIR)\input.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\isearch.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\isearch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\isearch.obj"	"$(INTDIR)\isearch.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\line.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\line.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\line.obj"	"$(INTDIR)\line.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\logmsg.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\logmsg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\logmsg.obj"	"$(INTDIR)\logmsg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\main.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\malloc.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\malloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\malloc.obj"	"$(INTDIR)\malloc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\map.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\map.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\map.obj"	"$(INTDIR)\map.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\mouse.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mouse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mouse.obj"	"$(INTDIR)\mouse.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\msdir.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\msdir.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\msdir.obj"	"$(INTDIR)\msdir.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\msexec.c

!IF  "$(CFG)" == "wincon - Win32 Release"


CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\msexec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_SWITCHES) $(SOURCE)

!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\msexec.obj"	"$(INTDIR)\msexec.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<

!ENDIF 

SOURCE=.\oswin.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\oswin.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\oswin.obj"	"$(INTDIR)\oswin.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\random.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\random.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\random.obj"	"$(INTDIR)\random.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\re.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\re.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\re.obj"	"$(INTDIR)\re.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\region.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\region.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\region.obj"	"$(INTDIR)\region.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\search.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\search.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_SWITCHES) $(SOURCE)


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\search.obj"	"$(INTDIR)\search.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_SWITCHES) $(SOURCE)


!ENDIF 

SOURCE=..\src\sinc.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\sinc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\sinc.obj"	"$(INTDIR)\sinc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\tag.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\tag.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\tag.obj"	"$(INTDIR)\tag.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\window.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\window.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo  /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\window.obj"	"$(INTDIR)\window.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\winpipe.c

!IF  "$(CFG)" == "wincon - Win32 Release"


"$(INTDIR)\winpipe.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"


"$(INTDIR)\winpipe.obj"	"$(INTDIR)\winpipe.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\src\word.c

!IF  "$(CFG)" == "wincon - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O1 /Os /Og /GF /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\word.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wincon - Win32 Debug"

CPP_SWITCHES=/nologo /W3 /Zi /Od /I "../src" /I "../wincon" /D "WIN32" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\word.obj"	"$(INTDIR)\word.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

