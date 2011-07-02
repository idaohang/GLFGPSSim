# This is the small makefile for the linux conio.h

CURSESDIR = /usr/local
LATEXDIR = /usr/bin
DVIPSDIR = /usr/bin
VERSION = 1.02

all: library doc

library:
	@echo Mental EXPlosion presents:
	@echo
	@echo 'Linux-conio.h - the conio.h for Linux'
	@echo '*************************************'
	@echo 'ATTENTION: This package needs ncurses !'
	@echo 'If you are sure you have installed it and it is not'
	@echo 'found - edit the Makefile and change the variable'
	@echo '"CURSESDIR" - It is now set to:' $(CURSESDIR)
	@echo
	@echo 'Trying to compile...'
	cc -O -c -I. -I$(CURSESDIR)/include/ncurses conio.c -o conio.o 
	ar rv libconio.a conio.o 
	ranlib libconio.a
	@echo '...success !'
	@echo
	@echo
	@echo 'libconio.a made - please take a look at the file README now !'
	@echo 'Type "make doc" to generate a nicely formatted version of the README' 
	@echo

install:
	@echo
	@echo 'There are so many different system configurations'
	@echo 'So I think it is better you install these things'
	@echo 'by hand.'
	@echo
	@echo 'The best thing to do is to copy conio.h to your'
	@echo 'standard header files directory and libconio.a to your'
	@echo 'standard library directory.'
	@echo 'copy conio.c and README to your standard source'
	@echo 'directory.'
	@echo
	@echo 'But if you want me to do it for you type "make install_default"'
	@echo 'The files will then be installed under '$(CURSESDIR)
	@echo 'to the following subdirectories: include, lib, src/linux-conio'
	@echo	

install_default:
	cp conio.h $(CURSESDIR)/include
	cp libconio.a $(CURSESDIR)/lib 
	mkdir $(CURSESDIR)/src
	mkdir $(CURSESDIR)/src/linux-conio
	cp conio.h $(CURSESDIR)/src/linux-conio
	cp conio.c $(CURSESDIR)/src/linux-conio
	cp README $(CURSESDIR)/src/linux-conio
	cp Makefile $(CURSESDIR)/src/linux-conio

doc: 
	@echo 'Compiling README.tex...'
	@echo 'I hope latex is in your PATH and works well ;)'
	$(LATEXDIR)/latex README.tex
	$(DVIPSDIR)/dvips README.dvi
	
clean:
	rm -f *.o core *~ libconio.a *.bak *.dvi *.ps *.log *.aux

