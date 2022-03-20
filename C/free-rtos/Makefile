#
# $Revision: 1.1.1.1 $
#

#
# sub-directories to make
#
DIRS = app kern io lib bsp
#
# supported architectures
#
ARCHS = m68k
#
# supported CPU's
#
CPUS = m68020

default: $(CPUS)

m68020: utilities
	for i in m68k $(DIRS) ;\
	do \
		if [ -d $$i ] ; \
		then \
			cd $$i ; \
			make default ; \
			cd .. ; \
		fi ; \
	done ; \

utilities:
	cd util ; \
	make default; \
	cd .. ;

util-clean:
	cd util ; \
	make clean; \
	cd .. ;

util-depend:
	cd util ; \
	make depend; \
	cd .. ;


depend: util-depend
	@-for i in $(ARCHS) $(DIRS) ;\
	do  \
		if [ -d $$i ] ; \
		then \
			cd $$i ; \
			make depend ; \
			cd .. ; \
		fi ; \
	done

clean: util-clean
	@-for i in $(ARCHS) $(DIRS) ;\
	do  \
		if [ -d $$i ] ; \
		then \
			cd $$i ; \
			make clean ; \
			cd .. ; \
		fi ; \
	done
	rm libobj/*/*
