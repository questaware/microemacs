CC = \gener\c600\bin\cl

#CFLAGS = /Zi /AL /I../ibmpc /I../src
#LFLAGS = /B /ST:1600 /NOI /CODEVIEW /MAP
CFLAGS = /AL /Ocegilsa /Gs /I../ibmpc /I../src
LFLAGS = /B /ST:1800 /NOI /MAP

OBJS = ibmpc.obj msdos.obj msdir.obj msexec.obj \
       basic.obj buffer.obj bind.obj char.obj crypt.obj display.obj \
       eval.obj exec.obj execasm.obj file.obj fileio.obj line.obj \
       input.obj main.obj malloc.obj mouse.obj \
       random.obj region.obj isearch.obj search.obj window.obj word.obj map.obj

all: emacs.exe

logmsg.obj : ../src/logmsg.c
	$(CC) $(CFLAGS) /c ../src/logmsg.c

map.obj : ../src/map.c
	$(CC) $(CFLAGS) /c ../src/map.c

ibmpc.obj : ibmpc.c
	$(CC) $(CFLAGS) /c ibmpc.c

msdos.obj : msdos.c
 	$(CC) $(CFLAGS) /c msdos.c

ibmpc.i :
 	$(CC) /E $(CFLAGS) /c ibmpc.c

msexec.obj : msexec.c
 	$(CC) $(CFLAGS) /c msexec.c

env.obj : env.c
 	$(CC) $(CFLAGS) /c env.c

basic.obj : ../src/basic.c
	$(CC) $(CFLAGS) /c $?

bind.obj : ../src/bind.c ../src/efunc.h
	$(CC) $(CFLAGS) /c ../src/bind.c

buffer.obj : ../src/buffer.c
	$(CC) $(CFLAGS) /c ../src/buffer.c

char.obj : ../src/char.c
	$(CC) $(CFLAGS) /c ../src/char.c

crypt.obj : ../src/crypt.c
	$(CC) $(CFLAGS) /c ../src/crypt.c

display.obj : ../src/display.c
	$(CC) $(CFLAGS) /c ../src/display.c

display.i : ../src/display.c
	$(CC) $(CFLAGS) /E ../src/display.c

dolock.obj : ../src/dolock.c
	$(CC) $(CFLAGS) /c ../src/dolock.c

eval.obj : ../src/eval.c
	$(CC) $(CFLAGS) /c ../src/eval.c

exec.obj : ../src/exec.c
	$(CC) $(CFLAGS) /c ../src/exec.c

execasm.obj : ../src/execasm.c
	$(CC) /AL /Ocegilsa /Gs /I../ibmpc /I../src /c ../src/execasm.c

file.asm : ../src/file.c
	$(CC) $(CFLAGS) /S ../src/file.c

file.i : ../src/file.c
	$(CC) $(CFLAGS) /E ../src/file.c

file.obj : ../src/file.c
	$(CC) $(CFLAGS) /c ../src/file.c

fileio.obj : ../src/fileio.c
	$(CC) $(CFLAGS) /c ../src/fileio.c

input.obj : ../src/input.c
	$(CC) $(CFLAGS) /c ../src/input.c

input.i : ../src/input.c
	$(CC) $(CFLAGS) /E ../src/input.c

isearch.obj : ../src/isearch.c
	$(CC) $(CFLAGS) /c ../src/isearch.c

line.obj : ../src/line.c
	$(CC) $(CFLAGS) /c ../src/line.c

main.obj : ../src/main.c
	$(CC) $(CFLAGS) /c ../src/main.c

malloc.obj : ../src/malloc.c
	$(CC) $(CFLAGS) /c ../src/malloc.c

mouse.obj : ../src/mouse.c
	$(CC) $(CFLAGS) /c ../src/mouse.c

msdir.obj : ../src/msdir.c
 	$(CC) $(CFLAGS) /c ../src/msdir.c

region.obj : ../src/region.c
	$(CC) $(CFLAGS) /c ../src/region.c

random.obj : ../src/random.c
	$(CC) $(CFLAGS) /c ../src/random.c

search.obj : ../src/search.c
	$(CC) $(CFLAGS) /c ../src/search.c

search.i : ../src/search.c
	$(CC) $(CFLAGS) /E ../src/search.c

window.obj : ../src/window.c
	$(CC) $(CFLAGS) /c ../src/window.c

word.obj : ../src/word.c
	$(CC) $(CFLAGS) /c ../src/word.c

lemacs.exe : $(OBJS) logmsg.obj
	\gener\c600\bin\link $(LFLAGS) logmsg+@lk.rsp, emacs, , ;

emacs.exe : $(OBJS)
	\gener\c600\bin\link $(LFLAGS) @lk.rsp, emacs, , ;

