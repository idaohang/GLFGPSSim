#=======================================================================
#@V@:Note: File automatically generated by VIDE - 2.00/10Apr03 (gcc).
# Generated 02:43:23 PM 17 Feb 2010
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
LIBS	=	-lm
C_FLAGS	=	-O

SRCS	=\
	obsolete.c

EXOBJS	=\
	$(oDir)/obsolete.o

ALLOBJS	=	$(EXOBJS)
ALLBIN	=	$(Bin)/libobsolete.a
ALLTGT	=	$(Bin)/libobsolete.a

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

$(Bin)/libobsolete.a: $(EXOBJS)
	rm -f $(Bin)/libobsolete.a
	ar cr $(Bin)/libobsolete.a $(EXOBJS)
	ranlib $(Bin)/libobsolete.a

$(oDir)/obsolete.o: obsolete.c obsolete.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<
