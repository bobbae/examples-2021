#
# $Revision: 1.1.1.1 $
#

MAKEDEFINES = ../../include/makedef
MAKERULES = ../../include/makerule

include $(MAKEDEFINES)

FIRST = boot.o
START = 300000
LIB = 
OBJS = bsp.o

default: download

include $(MAKERULES)

a.out:	$(SRC)/libobj/$(CPU)/$(CPU).a $(FIRST) $(OBJS)
		$(LD) -o $@ -Bstatic -N -Ttext $(START) $(FIRST) $(OBJS) \
		$(SRC)/libobj/$(CPU)/$(CPU).a \

download: a.out
	$(TOHEX) -s a.out > $@
