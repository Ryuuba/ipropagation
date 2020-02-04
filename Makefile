INETDIR = $(HOME)/inet4
SLAWDIR = $(HOME)/slaw
BUILD_OPTIONS = -f --deep \
								-I$(INETDIR)/src/ \
                -I$(INETDIR)/src/inet/common/ \
								-L$(INETDIR)/out/gcc-release/src/ \
                -I$(SLAWDIR)/src/common/ \
                -I$(SLAWDIR)/contract/ \
								-I$(SLAWDIR)/src/observer/ \
								-lINET \
								-o info_propg

all: checkmakefiles
	cd src && $(MAKE)

clean: checkmakefiles
	cd src && $(MAKE) clean

cleanall: checkmakefiles
	cd src && $(MAKE) MODE=release clean
	cd src && $(MAKE) MODE=debug clean
	rm -f src/Makefile

makefiles:
	cd src && opp_makemake $(BUILD_OPTIONS)

checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '======================================================================='; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '======================================================================='; \
	echo; \
	exit 1; \
	fi
