# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.4-p6 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999, 2001 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = ../../..
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/aegiswm
pkglibdir = $(libdir)/aegiswm
pkgincludedir = $(includedir)/aegiswm

top_builddir = ../../..

ACLOCAL = aclocal-1.4
AUTOCONF = autoconf
AUTOMAKE = automake-1.4
AUTOHEADER = autoheader

INSTALL = /bin/install -c
INSTALL_PROGRAM = ${INSTALL} $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = foo
host_triplet = 
CAIRO_CFLAGS = -I/usr/include/freetype2 -I/usr/X11R6/include  
CAIRO_LIBS = -L/usr/X11R6/lib -lcairo -lm -lfontconfig -lpixman -lXrender -lX11 -lXext  
CC = gcc
CPP = gcc -E
CXX = g++
DEBUG_CFLAGS = 
EXPAT_CFLAGS = 
EXPAT_LIBS = -lexpat
HAVE_EXPAT = 1
HAVE_LIB = @HAVE_LIB@
HAVE_XMLPARSE_H = 
ICONV_CFLAGS = 
ICONV_LIBS = 
LIB = @LIB@
LIBICONV = 
LIBPNG_CFLAGS = -DPNG -I/usr/include/libpng12  
LIBPNG_CONFIG = 
LIBPNG_LIBS = -lpng12 -lz -lm  
LIBSVG_CAIRO_CFLAGS = -DSVG -I/usr/include/freetype2 -I/usr/include/libxml2 -I/usr/X11R6/include  
LIBSVG_CAIRO_LIBS = -L/usr/X11R6/lib -lsvg-cairo -lsvg -lpng -ljpeg -lcairo -lxml2 -lpthread -lz -lm -lfontconfig -lpixman -lXrender -lX11 -lXext  
LOCALE_LIBS = 
LTLIB = @LTLIB@
LTLIBICONV = 
MAINT = #
MAKEINFO = makeinfo
PACKAGE = aegiswm
PKG_CONFIG = /usr/bin/pkg-config
RANDR_CFLAGS = -DRANDR
RANDR_LIBS = -lXrandr
SHAPE_CFLAGS = -DSHAPE
THREAD_CFLAGS = 
THREAD_LIBS = 
VERSION = 0.5.0
XCURSOR_CFLAGS = -DXCURSOR 
XCURSOR_LIBS = -L/usr/X11R6/lib -lXcursor -lXrender -lX11 -lXext  
XEXT_LIBS = -lXext
XINERAMA_CFLAGS = -DXINERAMA
XINERAMA_LIBS = -lXinerama
X_CFLAGS =  -I/usr/X11R6/include
X_LIBS =  -L/usr/X11R6/lib -lX11
dialog_command = zenity --title \"AegisWM: - warning -\" --info --text \"%m\"
kdialog_cmd = 
perl_cmd = /usr/bin/perl
regex_cmd = sed
xdialog_cmd = 
zenity_cmd = zenity

sloppyfocusnonopaquedir = $(pkgdatadir)/actions/sloppyfocusnonopaque
sloppyfocusnonopaque_DATA = action.conf

configfiles = $(sloppyfocusnonopaque_DATA)
CLEANFILES = $(configfiles)
EXTRA_DIST = action.conf.in
mkinstalldirs = $(SHELL) $(top_srcdir)/config/mkinstalldirs
CONFIG_HEADER = ../../../config.h
CONFIG_CLEAN_FILES = 
DATA =  $(sloppyfocusnonopaque_DATA)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP_ENV = --best
all: all-redirect
.SUFFIXES:
$(srcdir)/Makefile.in: # Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --gnu data/actions/sloppyfocusnonopaque/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status $(BUILT_SOURCES)
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


install-sloppyfocusnonopaqueDATA: $(sloppyfocusnonopaque_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(sloppyfocusnonopaquedir)
	@list='$(sloppyfocusnonopaque_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(sloppyfocusnonopaquedir)/$$p"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(sloppyfocusnonopaquedir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(DESTDIR)$(sloppyfocusnonopaquedir)/$$p"; \
	    $(INSTALL_DATA) $$p $(DESTDIR)$(sloppyfocusnonopaquedir)/$$p; \
	  fi; fi; \
	done

uninstall-sloppyfocusnonopaqueDATA:
	@$(NORMAL_UNINSTALL)
	list='$(sloppyfocusnonopaque_DATA)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(sloppyfocusnonopaquedir)/$$p; \
	done
tags: TAGS
TAGS:


distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = data/actions/sloppyfocusnonopaque

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	distdir=`cd $(distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --gnu data/actions/sloppyfocusnonopaque/Makefile
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(distdir)/$$file; \
	  else \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file || :; \
	  fi; \
	done
info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am:
install-exec: install-exec-am

install-data-am: install-sloppyfocusnonopaqueDATA
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-sloppyfocusnonopaqueDATA
uninstall: uninstall-am
all-am: Makefile $(DATA) all-local
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(sloppyfocusnonopaquedir)


mostlyclean-generic:

clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-generic mostlyclean-am

clean: clean-am

distclean-am:  distclean-generic clean-am

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-generic distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: uninstall-sloppyfocusnonopaqueDATA \
install-sloppyfocusnonopaqueDATA tags distdir info-am info dvi-am dvi \
check check-am installcheck-am installcheck install-exec-am \
install-exec install-data-am install-data install-am install \
uninstall-am uninstall all-local all-redirect all-am all installdirs \
mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean

all-local: $(configfiles)

$(configfiles): %.conf: %.conf.in
	sed -e "s,@pkgdatadir@,$(pkgdatadir)," -e "s,@pkgversion@,0.5.0," $< > $@

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
