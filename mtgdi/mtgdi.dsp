# Microsoft Developer Studio Project File - Name="MtGdi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MtGdi - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mtgdi.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\UniRelease"
# PROP Intermediate_Dir ".\UniRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\UniDebug"
# PROP Intermediate_Dir ".\UniDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\src" /I "..\mtgdi" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_MBCS" /Yu"Stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386

!ENDIF 

# Begin Target

# Name "MtGdi - Win32 Release"
# Name "MtGdi - Win32 Debug"
# Name "MtGdi - Win32 Unicode Release"
# Name "MtGdi - Win32 Unicode Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\cppsrc\basic.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\bind.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\buffer.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\char.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\crypt.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\display.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\dolock.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\eval.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\exec.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\execasm.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\file.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\fileio.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\input.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\isearch.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\line.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\logmsg.c

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# ADD CPP /Gm-
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\main.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Mainfrm.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\malloc.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\map.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\mouse.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\msdir.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Mtgdi.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Mtgdi.rc
# End Source File
# Begin Source File

SOURCE=.\Mtgdidoc.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Mtgdivw.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\random.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\re.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\region.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\search.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Stdafx.cpp
# ADD CPP /Yc"Stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Threads.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\window.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\winpipe.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\cppsrc\word.cpp

!IF  "$(CFG)" == "MtGdi - Win32 Release"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Debug"

# ADD CPP /I "../src" /I "../mtgdi"
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "MtGdi - Win32 Unicode Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;inl;fi;fd"
# Begin Source File

SOURCE=.\Mainfrm.h
# End Source File
# Begin Source File

SOURCE=.\Mtgdi.h
# End Source File
# Begin Source File

SOURCE=.\Mtgdidoc.h
# End Source File
# Begin Source File

SOURCE=.\Mtgdivw.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Stdafx.h
# End Source File
# Begin Source File

SOURCE=.\Threads.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\cursora.cur
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Mtgdi.ico
# End Source File
# Begin Source File

SOURCE=.\Res\Mtgdi.rc2
# End Source File
# End Group
# End Target
# End Project
