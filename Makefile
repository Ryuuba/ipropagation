INETDIR = $(HOME)/inet4

GENERAL_BUILD_OPTIONS = -f --deep \
				                -I$(INETDIR)/src/ \
								        -L$(INETDIR)/src/ \
								        -o info_propg
												-std=c++14

.PHONY: all clean cleanall makefiles makefiles-dbg checkmakefiles

all: checkmakefiles
	cd src && $(MAKE)

clean: checkmakefiles
	cd src && $(MAKE) clean

cleanall: checkmakefiles
	cd src && $(MAKE) MODE=release clean
	cd src && $(MAKE) MODE=debug clean
	rm -f src/Makefile

makefiles:
	cd src && opp_makemake $(GENERAL_BUILD_OPTIONS) -lINET

makefiles-dbg:
	cd src && opp_makemake $(GENERAL_BUILD_OPTIONS) -lINET_dbg

checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '======================================================================='; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '======================================================================='; \
	echo; \
	exit 1; \
	fi
