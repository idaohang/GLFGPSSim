#=======================================================================
#@V@:Note: File automatically generated by VIDE - 2.00/10Apr03 (gcc).
# Generated 02:51:43 PM 17 Feb 2010
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
	osdep.c

EXOBJS	=\
	$(oDir)/osdep.o

ALLOBJS	=	$(EXOBJS)
ALLBIN	=	$(Bin)/libosdep.a
ALLTGT	=	$(Bin)/libosdep.a

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

$(Bin)/libosdep.a: $(EXOBJS)
	rm -f $(Bin)/libosdep.a
	ar cr $(Bin)/libosdep.a $(EXOBJS)
	ranlib $(Bin)/libosdep.a

$(oDir)/osdep.o: osdep.c osdep.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<