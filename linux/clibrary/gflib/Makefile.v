#=======================================================================
#@V@:Note: File automatically generated by VIDE - 2.00/10Apr03 (gcc).
# Generated 05:22:40 PM 18 Feb 2010
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
libDirs	=
incDirs	=
LD_FLAGS =	-s
LIBS	=	-lm
C_FLAGS	=	-O

SRCS	=\
	gflib.c

EXOBJS	=\
	$(oDir)/gflib.o

ALLOBJS	=	$(EXOBJS)
ALLBIN	=	$(Bin)/libgflib.a
ALLTGT	=	$(Bin)/libgflib.a

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

$(Bin)/libgflib.a: $(EXOBJS)
	rm -f $(Bin)/libgflib.a
	ar cr $(Bin)/libgflib.a $(EXOBJS)
	ranlib $(Bin)/libgflib.a

$(oDir)/gflib.o: gflib.c gflib.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<