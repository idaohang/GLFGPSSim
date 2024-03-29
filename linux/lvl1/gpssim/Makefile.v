#=======================================================================
#@V@:Note: File automatically generated by VIDE - 2.00/10Apr03 (gcc).
# Generated 07:40:04 PM 14 Mar 2011
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
LIBS	=	-lgftermio -lcalensub -lobsolete -lgflib -lm
C_FLAGS	=	-O

SRCS	=\
	gpssim.c

EXOBJS	=\
	$(oDir)/gpssim.o

ALLOBJS	=	$(EXOBJS)
ALLBIN	=	$(Bin)/lxgpssim
ALLTGT	=	$(Bin)/lxgpssim

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

$(Bin)/lxgpssim: $(EXOBJS)
	$(LD) -o $(Bin)/lxgpssim $(EXOBJS) $(incDirs) $(libDirs) $(LD_FLAGS) $(LIBS)

$(oDir)/gpssim.o: gpssim.c ../../clibrary/gflib.h ../../clibrary/calensub.h \
 ../../clibrary/obsolete.h ../../clibrary/gftermio.h
	$(CC) $(C_FLAGS) $(incDirs) -c -o $@ $<
