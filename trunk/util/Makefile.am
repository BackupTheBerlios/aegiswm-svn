WAIMEA_PATH=$(pkgdatadir)

INCLUDES =\
-DWAIMEA_PATH=\"$(WAIMEA_PATH)\"\
$(X_CFLAGS)\
$(DEBUG_CFLAGS)

waimeactl_LDADD =\
$(X_LIBS)

bin_PROGRAMS = waimeactl

waimeactl_SOURCES = \
	waimeactl.c
