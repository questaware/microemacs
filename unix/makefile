#CC=c89
#LK=c89
CC=gcc
LK=g++
CFLAGS = -g -I. -I../$(SRC)
LFLAGS = -g -Dhp
SRC=src
USRC=unix
#CFLAGS=		-g -I../$(USRC)/ -I../$(SRC)
#CFLAGS=		-O -I../$(USRC)/ -I../$(SRC)
LFLAGS=-km

OFILES=	basic.o bind.o buffer.o char.o crypt.o display.o eval.o \
	exec.o file.o fileio.o input.o isearch.o line.o main.o malloc.o \
	mouse.o msdir.o random.o region.o search.o sinc.o tag.o window.o word.o

UOFILES= unix.o terminal.o lock.o
MOFILES= map.o


CFILES=	../$(SRC)/basic.c ../$(SRC)/bind.c ../$(SRC)/buffer.c ../$(SRC)/char.c \
        ../$(SRC)/crypt.c ../$(SRC)/display.c ../$(SRC)/eval.c \
	../$(SRC)/exec.c ../$(SRC)/file.c ../$(SRC)/fileio.c ../$(SRC)/input.c \
	../$(SRC)/isearch.c ../$(SRC)/line.c ../$(SRC)/main.c ../$(SRC)/malloc.c \
	../$(SRC)/mouse.c ../$(SRC)/random.c ../$(SRC)/region.c ../$(SRC)/search.c \
	../$(SRC)/tag.c ../$(SRC)/window.c ../$(SRC)/word.c

HFILES=	estruct.h ../$(SRC)/edef.h ../$(SRC)/efunc.h ../$(SRC)/epath.h \
          ../$(SRC)/ebind.h ../$(SRC)/evar.h ../$(SRC)/etype.h

all:	microemacs
microemacs:	$(OFILES) $(UOFILES) $(MOFILES)
#	$(CC)  -L/opt/FSFlibg++/lib -o emacs $(OFILES) $(MOFILES) $(UOFILES) /opt/FSFlibg++/lib/libstdc++.a
#	$(CC) $(CFLAGS) $(OFILES) $(MOFILES) $(UOFILES) -lncurses -o emacs
	$(LK) $(CFLAGS) $(OFILES) $(MOFILES) $(UOFILES)  -o microemacs

line.i:   ../$(SRC)/line.c
	$(CC) -E -I../$(USRC)/ -I../$(SRC) ../$(SRC)/line.c

makekeys: makekeys.c
	$(CC) $(CFLAGS) -o makekeys makekeys.c

keys.h:	makekeys
	./makekeys | sort > keys.h
# Can be combined using
# cat keys1.h keys2.h | sort | uniq > keys.h

map.o : ../src/map.c
	$(CC) -c $(CFLAGS) ../src/map.c

#$(OFILES):	$(HFILES)

unix.o:         unix.c
		$(CC) $(CFLAGS) -c unix.c

unix.i:		unix.c
		$(CC) -E $(CFLAGS) unix.c
terminal.o:     terminal.c
		$(CC) $(CFLAGS) -c terminal.c
lock.o:         lock.c
		$(CC) $(CFLAGS) -c lock.c

basic.o:  ../$(SRC)/basic.c
	$(CC) -c $(CFLAGS) ../$(SRC)/basic.c
bind.o:  ../$(SRC)/bind.c
	$(CC) -c $(CFLAGS) ../$(SRC)/bind.c
buffer.o:  ../$(SRC)/buffer.c
	$(CC) -c $(CFLAGS) ../$(SRC)/buffer.c
char.o:  ../$(SRC)/char.c
	$(CC) -c $(CFLAGS) ../$(SRC)/char.c
crypt.o:  ../$(SRC)/crypt.c
	$(CC) -c $(CFLAGS) ../$(SRC)/crypt.c
display.o:  ../$(SRC)/display.c
	$(CC) -c $(CFLAGS) ../$(SRC)/display.c
display.i:  ../$(SRC)/display.c
	$(CC) -E $(CFLAGS) ../$(SRC)/display.c
eval.o:  ../$(SRC)/eval.c
	$(CC) -c $(CFLAGS) ../$(SRC)/eval.c
exec.o:  ../$(SRC)/exec.c
	$(CC) -c $(CFLAGS) ../$(SRC)/exec.c
file.o:  ../$(SRC)/file.c
	$(CC) -c $(CFLAGS) ../$(SRC)/file.c
file.i:  ../$(SRC)/file.c
	$(CC) -E $(CFLAGS) ../$(SRC)/file.c
fileio.o:  ../$(SRC)/fileio.c
	$(CC) -c $(CFLAGS) ../$(SRC)/fileio.c
input.o:  ../$(SRC)/input.c
	$(CC) -c $(CFLAGS) ../$(SRC)/input.c
input.i:  ../$(SRC)/input.c
	$(CC) -E $(CFLAGS) ../$(SRC)/input.c
isearch.o:  ../$(SRC)/isearch.c
	$(CC) -c $(CFLAGS) ../$(SRC)/isearch.c
line.o:  ../$(SRC)/line.c
	$(CC) -c $(CFLAGS) ../$(SRC)/line.c
main.o:  ../$(SRC)/main.c
	$(CC) -c $(CFLAGS) ../$(SRC)/main.c
malloc.o:  ../$(SRC)/malloc.c
	$(CC) -c $(CFLAGS) ../$(SRC)/malloc.c
mouse.o:  ../$(SRC)/mouse.c
	$(CC) -c $(CFLAGS) ../$(SRC)/mouse.c
msdir.o:  ../$(SRC)/msdir.c
	$(CC) -c $(CFLAGS) ../$(SRC)/msdir.c
msdir.i:  ../$(SRC)/msdir.c
	$(CC) -E $(CFLAGS) ../$(SRC)/msdir.c
random.o:  ../$(SRC)/random.c
	$(CC) -c $(CFLAGS) ../$(SRC)/random.c
region.o:  ../$(SRC)/region.c
	$(CC) -c $(CFLAGS) ../$(SRC)/region.c 
search.o:  ../$(SRC)/search.c
	$(CC) -c $(CFLAGS) ../$(SRC)/search.c
search.i:  ../$(SRC)/search.c
	$(CC) -c -E $(CFLAGS) ../$(SRC)/search.c > search.i
sinc.o:  ../$(SRC)/sinc.cpp
	$(CC) -c $(CFLAGS) ../$(SRC)/sinc.cpp
tag.o: ../$(SRC)/tag.cpp
	$(CC) -c $(CFLAGS) ../$(SRC)/tag.cpp
window.o:  ../$(SRC)/window.c
	$(CC) -c $(CFLAGS) ../$(SRC)/window.c
word.o: ../$(SRC)/word.c
	$(CC) -c $(CFLAGS) ../$(SRC)/word.c
        
rpm:
	rm -rf ~/rpmbuild
	mkdir -p ~/rpmbuild/BUILD
	mkdir -p ~/rpmbuild/RPMS
	mkdir -p ~/rpmbuild/SRPMS
	mkdir -p ~/rpmbuild/SPECS
	cp microemacs.spec ~/rpmbuild/SPECS
	mkdir -p ~/rpmbuild/SOURCES
	cd ..;tar cfz ~/rpmbuild/SOURCES/microemacs-2.9.tar.gz -T index

clean:
	rm -f *.o
	rm makekeys
