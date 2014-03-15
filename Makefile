VERSION=20111109

##########################
## Parameters for Users ##
##########################

# Install directory (used by CVMLCPP's build system)
#TARGET=/usr/local
TARGET=$(DESTDIR)/usr

# Number of jobs to launch in parallel (used by CVMLCPP's build system)
NCPU=2

# Installation directory of CVMLCPP
export CVMLCPP_PREFIX=/usr/
#export CVMLCPP_PREFIX=$(DESTDIR)

# Comment this line out to disable the use of GNU Scientific Library
BACKEND=gsl

##########################################
## Users: Do not change anything below! ##
##########################################

EXTRA=BACKEND=${BACKEND} CVMLCPP_PREFIX=${CVMLCPP_PREFIX}

# Include the CVMLCPP build system's Makefile part
include ${CVMLCPP_PREFIX}/share/cvmlcpp/build/Makefile.build

# We need to provide our own "uninstall" target
uninstall:
	rm -f $(DESTDIR)/lib/libshapes.a $(DESTDIR)/lib/libshapes.so
	rm -rf $(DESTDIR)/include/shapes $(DESTDIR)/share/doc/shapes

allclean: clean
	rm -f .sconsign .sconsign.dblite

post-install:
	mv $(TARGET)/lib/libshapes.so $(TARGET)/lib/libshapes.so.$(VERSION)
	ln -s libshapes.so.$(VERSION) $(TARGET)/lib/libshapes.so

