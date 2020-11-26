DEBUG=1
DIAG=1
C_HOME=C:\Program Files (x86)\Microsoft Visual Studio\VC98

HERE:=$(CURDIR)/..

include ../Makefile

vp :
	echo $(VPATH)

db:
	dumpbin /headers e.exe
sb:
	editbin /stack:0x200000,0x2000 e.exe
