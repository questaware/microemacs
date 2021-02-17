ifndef C_FLG
C_FLG=/ML
endif

ifneq ($(DEBUG),1)

CPP=cl.exe
CPP_PROJ=/nologo /W3 /O1 /Os /Og /Oy /GF $(C_FLG) /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "NDEBUG" /D "_CONSOLE" /c 
RSC=rc.exe

LINK32=cl.exe
LINK32_FLAGS=/nologo /Fm $(C_FLG) /Fe

else

CPP=cl.exe
#CPP_PROJ=/nologo /Yd /W3 /Zi /MDd /Od /I "../src" /D "WIN32_LEAN_AND_MEAN" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c 
CPP_PROJ=/nologo /YX /Yd /W3 /Zi /MDd /Od /I "../src" /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /c 
RSC=rc.exe
LINK32=link
LINK32_FLAGS=/nologo /debug /out:

endif

VPATH+=$(HERE)/../src $(HERE)

ifndef C_HOME
#C_HOME=C:/Program Files/Microsoft Visual Studio .NET 2003/VC7
C_HOME=C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC
endif
ifndef C_KIT
C_KIT=C:/Program Files (x86)/Windows Kits
endif

#INCLUDE=$(C_HOME)/ATLMFC/INCLUDE;$(C_HOME)/INCLUDE;$(C_HOME)/PlatformSDK/include/prerelease;$(C_HOME)/PlatformSDK/include;$(C_HOME)/../SDK/v1.1/include;$(C_HOME)/../SDK/v1.1/include/
INCLUDE=$(C_HOME)\INCLUDE;$(C_HOME)\ATLMFC\INCLUDE;$(C_KIT)\10\include\10.0.10150.0\ucrt;$(C_KIT)\NETFXSDK\4.6\include\um;$(C_KIT)\8.1\include\shared;$(C_KIT)\8.1\include\um;$(C_KIT)\8.1\include\winrt;
export INCLUDE

LBC=$(C_HOME)/LIB
SDKLIB=$(LBC);$(C_HOME)/PlatformSDK/Lib
LIB=$(LBC)
LIB=$(LBC);$(C_KIT)/8.1/Lib/winv6.3/um/x86;$(C_KIT)/10/Lib/10.0.10150.0/ucrt/x86
export LIB

Path :=c:/bin;$(C_HOME)/bin;$(C_HOME)/lib;\$(C_HOME)/../Common/msdev98/bin/;\
export Path

.SUFFIXES: .c .o .asm


LINK32_OBJS= \
basic.obj bind.obj buffer.obj \
char.obj crypt.obj \
display.obj \
eval.obj exec.obj \
file.obj fileio.obj \
input.obj isearch.obj \
line.obj \
main.obj map.obj msdir.obj \
random.obj region.obj \
search.obj sinc.obj \
tag.obj \
window.obj word.obj \
msemacs.obj \
oswin.obj

.c.obj :
	$(CPP) -I../ -I../../src/ $(CPP_PROJ) $< 

.c.exp :
	$(CPP) /E -I../ -I../../src/ $(CPP_PROJ) $< 

%.obj : %.c
	$(CPP) -I../ -I../../src/ $(CPP_PROJ) $< 

%.obj : %.cpp
	$(CPP) -I../ -I../../src/ $(CPP_PROJ) $< 

e.exe : $(LINK32_OBJS)
	spath -a $(LINK32)
	$(LINK32) $(LINK32_FLAGS)./e.exe $(LINK32_OBJS)  winmm.lib kernel32.lib user32.lib

main.obj : main.c
bind.obj : bind.c efunc.h

testpipe.exe : testpipe.obj sleep.obj
	$(LINK32) $(LINK32_FLAGS)./testpipe.exe testpipe.obj sleep.obj winmm.lib kernel32.lib user32.lib

testshell.exe : testshell.obj
	$(LINK32) $(LINK32_FLAGS)./testshell.exe testshell.obj sleep.obj winmm.lib kernel32.lib user32.lib

boldbug.exe : boldbug.obj
	$(LINK32) $(LINK32_FLAGS)./boldbug.exe boldbug.obj

clean :
	-rm *.obj
	-rm *.pch
	-rm *.pdb
	-rm e.exe

.c.asm :
	$(CPP) -I../ -I../../src/ /Fait.asm $(CPP_PROJ) $< 

inc :
	rem $(INCLUDE)

test :
	$(CPP) -o doit ../test.c

