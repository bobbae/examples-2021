#
# $Revision: 1.1.1.1 $
#

MAKEDEFINES = ../include/makedef
MAKERULES = ../include/makerule

include $(MAKEDEFINES)

LIBOBJS = intr.o locore.o trap.o cpu.o
LIB = libcpu.a

default: lib

include $(MAKERULES)

