SUBDIRS = actions styles menus scripts
pkgdata_DATA = waimearc waimea.dtd

configfiles = pre-action.conf \
	pre-style.conf \
	post-action.conf \
	post-style.conf

CLEANFILES = waimearc waimea.dtd $(configfiles)
EXTRA_DIST = \
	waimearc.in \
	waimea.dtd.in \
	pre-action.conf.in \
	pre-style.conf.in \
	post-action.conf.in \
	post-style.conf.in \
	Waimea.session
all-local: waimearc waimea.dtd $(configfiles)

waimearc: waimearc.in
	@regex_cmd@ -e "s,@dialogcommand@,@dialog_command@," -e "s,@pkgdatadir@,$(pkgdatadir)," $< > $@

waimea.dtd: waimea.dtd.in
	@regex_cmd@ -e "s,@pkgversion@,@VERSION@,"  $< > $@

$(configfiles): %.conf: %.conf.in
	@regex_cmd@ -e "s,@pkgdatadir@,$(pkgdatadir)," -e "s,@pkgversion@,@VERSION@," $< > $@
