#=======================================================================
#@V@:Note: File automatically generated by VIDE - 2.00/10Apr03 (gcc).
# Generated 03:11:49 PM 17 Feb 2010
# This file regenerated each time you run VIDE, so save under a
#    new name if you hand edit, or it will be overwritten.
#=======================================================================

# Standard defines:
CC  	=	gcc
LD  	=	gcc
WRES	=	windres
HOMEV	=	
VPATH	=	$(HOMEV)/include
oDir	=	.
Bin	=	.
libDirs	=	-L../../clibrary

incDirs	=	-I../../clibrary

LD_FLAGS =	-s
LIBS	=	-lobsolete -lgflib -lm
C_FLAGS	=	-O

SRCS	=\
	searches.c

EXOBJS	=\
	$(oDir)/searches.o

ALLOBJS	=	$(EXOBJS)
ALLBIN	=	$(Bin)/libsearches.a
ALLTGT	=	$(Bin)/libsearches.a

# User defines:

#@# Targets follow ---------------------------------

all:	$(ALLTGT)

objs:	$(ALLOBJS)

cleanobjs:
	rm -f $(ALLOBJS)

cleanbin:
	rm -f $(ALLBIN)

clean:	cleanobjs cleanbin

cleanall:	cleanobjs cleanbin

#@# User Targets follow ---------------------------------


#@# Dependency rules follow -----------------------------

$(Bin)/libsearches.a: $(EXOBJS)
	rm -f $(Bin)/libsearches.a
	ar cr $(Bin)/libsearches.a $(EXOBJS)
	ranlib $(Bin)/libsearches.a

$(oDir)/searches.o: searches.c ../../clibrary/gflib.h ../../clibrary/obsolete.h \
 searches.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<