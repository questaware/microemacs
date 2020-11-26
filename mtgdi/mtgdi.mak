# Microsoft Developer Studio Generated NMAKE File, Based on mtgdi.dsp
!IF "$(CFG)" == ""
CFG=MtGdi - Win32 Release
!MESSAGE No configuration specified. Defaulting to MtGdi - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "MtGdi - Win32 Release" && "$(CFG)" != "MtGdi - Win32 Debug" && "$(CFG)" != "MtGdi - Win32 Unicode Release" && "$(CFG)" != "MtGdi - Win32 Unicode Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mtgdi.mak" CFG="MtGdi - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MtGdi - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MtGdi - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "MtGdi - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "MtGdi - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "MtGdi - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\mtgdi.exe" "$(OUTDIR)\mtgdi.pch"


CLEAN :
	-@erase "$(INTDIR)\basic.obj"
	-@erase "$(INTDIR)\bind.obj"
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\char.obj"
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\display.obj"
	-@erase "$(INTDIR)\dolock.obj"
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
	-@erase "$(INTDIR)\Mainfrm.obj"
	-@erase "$(INTDIR)\malloc.obj"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\mouse.obj"
	-@erase "$(INTDIR)\msdir.obj"
	-@erase "$(INTDIR)\Mtgdi.obj"
	-@erase "$(INTDIR)\mtgdi.pch"
	-@erase "$(INTDIR)\Mtgdi.res"
	-@erase "$(INTDIR)\Mtgdidoc.obj"
	-@erase "$(INTDIR)\Mtgdivw.obj"
	-@erase "$(INTDIR)\random.obj"
	-@erase "$(INTDIR)\re.obj"
	-@erase "$(INTDIR)\region.obj"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\Stdafx.obj"
	-@erase "$(INTDIR)\Threads.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\winpipe.obj"
	-@erase "$(INTDIR)\word.obj"
	-@erase "$(OUTDIR)\mtgdi.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Mtgdi.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mtgdi.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\mtgdi.pdb" /machine:I386 /out:"$(OUTDIR)\mtgdi.exe" 
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
	"$(INTDIR)\Mainfrm.obj" \
	"$(INTDIR)\malloc.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\mouse.obj" \
	"$(INTDIR)\msdir.obj" \
	"$(INTDIR)\Mtgdi.obj" \
	"$(INTDIR)\Mtgdidoc.obj" \
	"$(INTDIR)\Mtgdivw.obj" \
	"$(INTDIR)\random.obj" \
	"$(INTDIR)\re.obj" \
	"$(INTDIR)\region.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\Stdafx.obj" \
	"$(INTDIR)\Threads.obj" \
	"$(INTDIR)\window.obj" \
	"$(INTDIR)\winpipe.obj" \
	"$(INTDIR)\word.obj" \
	"$(INTDIR)\Mtgdi.res"

"$(OUTDIR)\mtgdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\mtgdi.exe"


CLEAN :
	-@erase "$(INTDIR)\basic.obj"
	-@erase "$(INTDIR)\bind.obj"
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\char.obj"
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\display.obj"
	-@erase "$(INTDIR)\dolock.obj"
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
	-@erase "$(INTDIR)\Mainfrm.obj"
	-@erase "$(INTDIR)\malloc.obj"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\mouse.obj"
	-@erase "$(INTDIR)\msdir.obj"
	-@erase "$(INTDIR)\Mtgdi.obj"
	-@erase "$(INTDIR)\mtgdi.pch"
	-@erase "$(INTDIR)\Mtgdi.res"
	-@erase "$(INTDIR)\Mtgdidoc.obj"
	-@erase "$(INTDIR)\Mtgdivw.obj"
	-@erase "$(INTDIR)\random.obj"
	-@erase "$(INTDIR)\re.obj"
	-@erase "$(INTDIR)\region.obj"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\Stdafx.obj"
	-@erase "$(INTDIR)\Threads.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\winpipe.obj"
	-@erase "$(INTDIR)\word.obj"
	-@erase "$(OUTDIR)\mtgdi.exe"
	-@erase "$(OUTDIR)\mtgdi.ilk"
	-@erase "$(OUTDIR)\mtgdi.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Mtgdi.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mtgdi.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\mtgdi.pdb" /debug /machine:I386 /out:"$(OUTDIR)\mtgdi.exe" 
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
	"$(INTDIR)\Mainfrm.obj" \
	"$(INTDIR)\malloc.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\mouse.obj" \
	"$(INTDIR)\msdir.obj" \
	"$(INTDIR)\Mtgdi.obj" \
	"$(INTDIR)\Mtgdidoc.obj" \
	"$(INTDIR)\Mtgdivw.obj" \
	"$(INTDIR)\random.obj" \
	"$(INTDIR)\re.obj" \
	"$(INTDIR)\region.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\Stdafx.obj" \
	"$(INTDIR)\Threads.obj" \
	"$(INTDIR)\window.obj" \
	"$(INTDIR)\winpipe.obj" \
	"$(INTDIR)\word.obj" \
	"$(INTDIR)\Mtgdi.res"

"$(OUTDIR)\mtgdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

OUTDIR=.\UniRelease
INTDIR=.\UniRelease
# Begin Custom Macros
OutDir=.\UniRelease
# End Custom Macros

ALL : "$(OUTDIR)\mtgdi.exe"


CLEAN :
	-@erase "$(INTDIR)\basic.obj"
	-@erase "$(INTDIR)\bind.obj"
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\char.obj"
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\display.obj"
	-@erase "$(INTDIR)\dolock.obj"
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
	-@erase "$(INTDIR)\Mainfrm.obj"
	-@erase "$(INTDIR)\malloc.obj"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\mouse.obj"
	-@erase "$(INTDIR)\msdir.obj"
	-@erase "$(INTDIR)\Mtgdi.obj"
	-@erase "$(INTDIR)\mtgdi.pch"
	-@erase "$(INTDIR)\Mtgdi.res"
	-@erase "$(INTDIR)\Mtgdidoc.obj"
	-@erase "$(INTDIR)\Mtgdivw.obj"
	-@erase "$(INTDIR)\random.obj"
	-@erase "$(INTDIR)\re.obj"
	-@erase "$(INTDIR)\region.obj"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\Stdafx.obj"
	-@erase "$(INTDIR)\Threads.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\winpipe.obj"
	-@erase "$(INTDIR)\word.obj"
	-@erase "$(OUTDIR)\mtgdi.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Mtgdi.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mtgdi.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\mtgdi.pdb" /machine:I386 /out:"$(OUTDIR)\mtgdi.exe" 
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
	"$(INTDIR)\Mainfrm.obj" \
	"$(INTDIR)\malloc.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\mouse.obj" \
	"$(INTDIR)\msdir.obj" \
	"$(INTDIR)\Mtgdi.obj" \
	"$(INTDIR)\Mtgdidoc.obj" \
	"$(INTDIR)\Mtgdivw.obj" \
	"$(INTDIR)\random.obj" \
	"$(INTDIR)\re.obj" \
	"$(INTDIR)\region.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\Stdafx.obj" \
	"$(INTDIR)\Threads.obj" \
	"$(INTDIR)\window.obj" \
	"$(INTDIR)\winpipe.obj" \
	"$(INTDIR)\word.obj" \
	"$(INTDIR)\Mtgdi.res"

"$(OUTDIR)\mtgdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

OUTDIR=.\UniDebug
INTDIR=.\UniDebug
# Begin Custom Macros
OutDir=.\UniDebug
# End Custom Macros

ALL : "$(OUTDIR)\mtgdi.exe"


CLEAN :
	-@erase "$(INTDIR)\basic.obj"
	-@erase "$(INTDIR)\bind.obj"
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\char.obj"
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\display.obj"
	-@erase "$(INTDIR)\dolock.obj"
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
	-@erase "$(INTDIR)\Mainfrm.obj"
	-@erase "$(INTDIR)\malloc.obj"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\mouse.obj"
	-@erase "$(INTDIR)\msdir.obj"
	-@erase "$(INTDIR)\Mtgdi.obj"
	-@erase "$(INTDIR)\mtgdi.pch"
	-@erase "$(INTDIR)\Mtgdi.res"
	-@erase "$(INTDIR)\Mtgdidoc.obj"
	-@erase "$(INTDIR)\Mtgdivw.obj"
	-@erase "$(INTDIR)\random.obj"
	-@erase "$(INTDIR)\re.obj"
	-@erase "$(INTDIR)\region.obj"
	-@erase "$(INTDIR)\search.obj"
	-@erase "$(INTDIR)\Stdafx.obj"
	-@erase "$(INTDIR)\Threads.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\window.obj"
	-@erase "$(INTDIR)\winpipe.obj"
	-@erase "$(INTDIR)\word.obj"
	-@erase "$(OUTDIR)\mtgdi.exe"
	-@erase "$(OUTDIR)\mtgdi.ilk"
	-@erase "$(OUTDIR)\mtgdi.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Mtgdi.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mtgdi.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\mtgdi.pdb" /debug /machine:I386 /out:"$(OUTDIR)\mtgdi.exe" 
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
	"$(INTDIR)\Mainfrm.obj" \
	"$(INTDIR)\malloc.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\mouse.obj" \
	"$(INTDIR)\msdir.obj" \
	"$(INTDIR)\Mtgdi.obj" \
	"$(INTDIR)\Mtgdidoc.obj" \
	"$(INTDIR)\Mtgdivw.obj" \
	"$(INTDIR)\random.obj" \
	"$(INTDIR)\re.obj" \
	"$(INTDIR)\region.obj" \
	"$(INTDIR)\search.obj" \
	"$(INTDIR)\Stdafx.obj" \
	"$(INTDIR)\Threads.obj" \
	"$(INTDIR)\window.obj" \
	"$(INTDIR)\winpipe.obj" \
	"$(INTDIR)\word.obj" \
	"$(INTDIR)\Mtgdi.res"

"$(OUTDIR)\mtgdi.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mtgdi.dep")
!INCLUDE "mtgdi.dep"
!ELSE 
!MESSAGE Warning: cannot find "mtgdi.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MtGdi - Win32 Release" || "$(CFG)" == "MtGdi - Win32 Debug" || "$(CFG)" == "MtGdi - Win32 Unicode Release" || "$(CFG)" == "MtGdi - Win32 Unicode Debug"
SOURCE=..\cppsrc\basic.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\basic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\basic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\basic.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\basic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\bind.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bind.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bind.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bind.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bind.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\buffer.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\buffer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\buffer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\buffer.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\buffer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\char.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\char.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\char.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\char.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\char.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\crypt.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\crypt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\crypt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\crypt.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\crypt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\display.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\display.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\display.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\display.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\display.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\dolock.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dolock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dolock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dolock.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dolock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\eval.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\eval.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\eval.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\eval.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\eval.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\exec.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\exec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\exec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\exec.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\exec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\execasm.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\execasm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\execasm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\execasm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\execasm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\file.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\file.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\file.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\file.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\file.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\fileio.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\fileio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\fileio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\fileio.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\fileio.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\input.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\input.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\input.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\input.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\input.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\isearch.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\isearch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\isearch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\isearch.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\isearch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\line.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\line.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\line.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\line.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\line.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\src\logmsg.c

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\logmsg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\logmsg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\logmsg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm- /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\logmsg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\main.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Mainfrm.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mainfrm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\malloc.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\malloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\malloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\malloc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\malloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\map.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\map.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\map.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\map.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\map.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\mouse.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mouse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mouse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mouse.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mouse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\msdir.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\msdir.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\msdir.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\msdir.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\msdir.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Mtgdi.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mtgdi.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mtgdi.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mtgdi.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mtgdi.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Mtgdi.rc

"$(INTDIR)\Mtgdi.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\Mtgdidoc.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mtgdidoc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mtgdidoc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mtgdidoc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mtgdidoc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Mtgdivw.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mtgdivw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mtgdivw.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mtgdivw.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Mtgdivw.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\random.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\random.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\random.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\random.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\random.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\re.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\re.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\re.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\re.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\re.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\region.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\region.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\region.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\region.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\region.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\search.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\search.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\search.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\search.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\search.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Stdafx.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Stdafx.obj"	"$(INTDIR)\mtgdi.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Stdafx.obj"	"$(INTDIR)\mtgdi.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Stdafx.obj"	"$(INTDIR)\mtgdi.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yc"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Stdafx.obj"	"$(INTDIR)\mtgdi.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Threads.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Threads.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Threads.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Threads.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Threads.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\window.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\window.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\window.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\window.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\window.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\winpipe.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\winpipe.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\winpipe.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\winpipe.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\winpipe.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\cppsrc\word.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "../src" /I "../mtgdi" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\word.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /I "../mtgdi" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\word.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\mtgdi.pch" /Yu"Stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\word.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mtgdi.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\word.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

