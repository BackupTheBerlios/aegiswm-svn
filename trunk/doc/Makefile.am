man_MANS = waimea.1

all-local: waimea.1
CLEANFILES = waimea.1
EXTRA_DIST = waimea.1.in

waimea.1: waimea.1.in
	@regex_cmd@ -e "s,@pkgdatadir@,$(pkgdatadir)," -e "s,@pkgversion@,@VERSION@,"  $< > $@
