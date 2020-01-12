INETDIR = $(HOME)/inet4
SLAWDIR = $(HOME)/slaw

BUILD_OPTIONS = -f --deep -I$(INETDIR)/src/inet/applications/base \
                 -I$(INETDIR)/src/inet/transportlayer/contract \
                 -I$(INETDIR)/src/inet/common \
                 -I$(SLAWDIR)/src/common \
                 -I$(SLAWDIR)/contract -I$(SLAWDIR)/src/observer

all: checkmakefiles
	cd src && $(MAKE)

clean: checkmakefiles
	cd src && $(MAKE) clean

cleanall: checkmakefiles
	cd src && $(MAKE) MODE=release clean
	cd src && $(MAKE) MODE=debug clean
	rm -f src/Makefile

makefiles:
	cd src && opp_makemake -f --deep

checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '======================================================================='; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '======================================================================='; \
	echo; \
	exit 1; \
	fi
