AEGIS_PATH=$(pkgdatadir)

INCLUDES = \
-DAEGIS_PATH=\"$(AEGIS_PATH)\" \
$(CAIRO_CFLAGS) \
$(ICONV_CFLAGS) \
$(EXPAT_CFLAGS) \
$(THREAD_CFLAGS)\
$(X_CFLAGS) \
$(SHAPE_CFLAGS) \
$(XINERAMA_CFLAGS) \
$(RANDR_CFLAGS) \
$(LIBPNG_CFLAGS) \
$(LIBSVG_CAIRO_CFLAGS) \
$(XCURSOR_CFLAGS) \
$(DEBUG_CFLAGS)

aegis_LDADD = \
$(CAIRO_LIBS) \
$(LOCALE_LIBS) \
$(ICONV_LIBS) \
$(EXPAT_LIBS) \
$(THREAD_LIBS) \
$(X_LIBS) \
$(XEXT_LIBS) \
$(XINERAMA_LIBS) \
$(RANDR_LIBS) \
$(LIBPNG_LIBS) \
$(LIBSVG_CAIRO_LIBS) \
$(XCURSOR_LIBS)

bin_PROGRAMS = aegis

aegis_SOURCES = \
	main.cpp \
	aegis.cpp \
	aegis.h \
	screen.cpp \
	screen.h \
	window.cpp \
	window.h \
	resources.cpp \
	resources.h \
	event.cpp \
	event.h \
	menu.cpp \
	menu.h \
	net.cpp \
	net.h \
	dockapp.cpp \
	dockapp.h \
	timer.cpp \
	timer.h \
	aegis_regex.cpp \
	aegis_regex.h \
	parser.cpp \
	parser.h \
	style.cpp \
	style.h \
	action.cpp \
	action.h \
	cursor.cpp \
	cursor.h \
	render.cpp \
	render.h \
	png.cpp \
	png.h \
	util.cpp \
	util.h \
	refcounted.h \
	tst.h
