/* Resources.cc

   Copyright © 2003 David Reveman.

   This file is part of Aegis.

   Aegis is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2, or (at your option) any later
   version.

   Aegis is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.

   You should have received a copy of the GNU General Public License
   along with Aegis; see the file COPYING. If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA. */

#ifdef    HAVE_CONFIG_H
#  include "../config.h"
#endif // HAVE_CONFIG_H

extern "C" {
#include <X11/Xlib.h>

#ifdef    HAVE_STDIO_H
#  include <stdio.h>
#endif // HAVE_STDIO_H

#ifdef    STDC_HEADERS
#  include <stdlib.h>
#  include <string.h>
#endif // STDC_HEADERS

#ifdef    HAVE_SCHED_H
#  include <sched.h>
#endif // HAVE_SCHED_H
}

#include "resources.h"

ResourceHandler::ResourceHandler(Aegis *wa, char **_options) {
	aegis = wa;
	display = aegis->display;
	options = _options;
}

void ResourceHandler::loadConfig(Aegis *aegis) {
	XrmValue value;
	char *value_str;
	char *value_type;

	database = (XrmDatabase) 0;

	char *rc_file;
	bool rc_forced = false;
	if (options[ARG_RCFILE]) {
		rc_file = options[ARG_RCFILE];
		rc_forced = true;
	} else {
		char *homedir = getenv("HOME");
		if (! homedir) homedir = "";
		rc_file = new char[strlen(homedir) + strlen("/.aegisrc") + 1];
		sprintf(rc_file, "%s/.aegisrc", homedir);
	}
	if (! (database = XrmGetFileDatabase(rc_file))) {
		if (rc_forced)
			snprintf(aegis->config_info, 256,
					"can not open rcfile %s for reading.", rc_file);
		else
			if (! (database = XrmGetFileDatabase(DEFAULTRCFILE)))
				snprintf(aegis->config_info, 256,
						"can not open system default rcfile %s for reading.",
						DEFAULTRCFILE);
	}

	if (! rc_forced)
		delete [] rc_file;

	aegis->screenmask = 0;
	value_str = NULL;
	if (options[ARG_SCREENMASK])
		value_str = options[ARG_SCREENMASK];
	else {
		if (XrmGetResource(database, "screenMask", "ScreenMask",
					&value_type, &value))
			value_str = value.addr;
	}

	if (value_str) {
		if (! strcasecmp("all", value_str))
			aegis->screenmask = 0xffffffff;
		else {
			value_str = AE_STRDUP(value_str);
			char *token = strtok(value_str, ", \t");
			if (token) aegis->screenmask |= (1L << atoi(token));
			while ((token = strtok(NULL, ", \t")))
				aegis->screenmask |= (1L << atoi(token));
			delete [] value_str;
		}
	} else
		aegis->screenmask = 0xffffffff;

	char *path = getenv("PATH");
	if (! path) path = "";
	value_str = NULL;
	if (options[ARG_SCRIPTDIR])
		value_str = options[ARG_SCRIPTDIR];
	else {
		if (XrmGetResource(database, "scriptDir", "ScriptDir",
					&value_type, &value))
			value_str = value.addr;
	}

	if (value_str) {
		char *sdir = environment_expansion(AE_STRDUP(value_str));
		aegis_pathenv = new char[strlen(path) + strlen(sdir) + 7];
		sprintf(aegis_pathenv, "PATH=%s:%s", sdir, path);
		delete [] sdir;
	} else {
		aegis_pathenv =
			new char[strlen(path) + strlen(DEFAULTSCRIPTDIR) + 7];
		sprintf(aegis_pathenv, "PATH=%s:%s", DEFAULTSCRIPTDIR, path);
	}

	value_str = NULL;
	if (options[ARG_DOUBLECLICK])
		value_str = options[ARG_DOUBLECLICK];
	else {
		if (XrmGetResource(database, "doubleClickInterval",
					"DoubleClickInterval", &value_type, &value))
			value_str = value.addr;
	}

	if (value_str) {
		if (sscanf(value_str, "%lu", &aegis->double_click) != 1)
			aegis->double_click = 300;
	} else
		aegis->double_click = 300;

	if (aegis->double_click > 999) aegis->double_click = 999;


	value_str = NULL;
	if (options[ARG_CLIENTSIDE])
		value_str = options[ARG_CLIENTSIDE];
	else {
		if (XrmGetResource(database, "clientSideRendering",
					"ClientSideRendering", &value_type, &value))
			value_str = value.addr;
	}

	aegis->client_side_rendering = false;

	if (value_str)
		if (! strcasecmp("true", value_str))
			aegis->client_side_rendering = true;

#ifdef    THREAD
	value_str = NULL;
	if (options[ARG_THREADS])
		value_str = options[ARG_THREADS];
	else {
		if (XrmGetResource(database, "multiThreading",
					"MultiThreading", &value_type, &value))
			value_str = value.addr;
	}

	__render_thread_count = 0;

	if (value_str) {
		if (! strcasecmp("true", value_str))
			__render_thread_count = 1;

		struct sched_param param;
		int policy = sched_getscheduler(0);
		sched_getparam(0, &param);

		value_str = NULL;
		if (options[ARG_THREADPRIO])
			value_str = options[ARG_THREADPRIO];
		else {
			if (XrmGetResource(database, "renderingThreadPriority",
						"RenderingThreadPriority", &value_type, &value))
				value_str = value.addr;
		}

		if (value_str) {
			if (sscanf(value_str, "%i", &aegis->render_thread_prio) != 1)
				aegis->render_thread_prio = param.sched_priority;
		} else
			aegis->render_thread_prio = param.sched_priority;

		if (aegis->render_thread_prio > sched_get_priority_max(policy))
			aegis->render_thread_prio = sched_get_priority_max(policy);
		else if (aegis->render_thread_prio < sched_get_priority_min(policy))
			aegis->render_thread_prio = sched_get_priority_min(policy);
#endif // THREAD

	}

	void ResourceHandler::loadConfig(AegisScreen *wascreen) {
		XrmValue value;
		char *value_str;
		char *value_type;
		char rc_name[256], rc_class[256];
		int sn = wascreen->screen_number;
		ScreenConfig *sc = &wascreen->config;

		sc->action_file = AE_STRDUP((char *) DEFAULTACTIONFILE);
		sc->menu_file = AE_STRDUP((char *) DEFAULTMENUFILE);
		sc->style_file = AE_STRDUP((char *) DEFAULTSTYLEFILE);

		if (options[ARG_ACTIONFILE]) {
			delete [] sc->action_file;
			sc->action_file = AE_STRDUP(options[ARG_ACTIONFILE]);
		} else {
			snprintf(rc_name, 256, "screen%d.actionFile", sn);
			snprintf(rc_class, 256, "Screen%d.ActionFile", sn);
			if (XrmGetResource(database, rc_name, rc_class, &value_type, &value)) {
				delete [] sc->action_file;
				sc->action_file = AE_STRDUP(value.addr);
			}
		}

		if (options[ARG_STYLEFILE]) {
			delete [] sc->style_file;
			sc->style_file = AE_STRDUP(options[ARG_STYLEFILE]);
		} else {
			snprintf(rc_name, 256, "screen%d.styleFile", sn);
			snprintf(rc_class, 256, "Screen%d.StyleFile", sn);
			if (XrmGetResource(database, rc_name, rc_class, &value_type, &value)) {
				delete [] sc->style_file;
				sc->style_file = AE_STRDUP(value.addr);
			}
		}

		if (options[ARG_MENUFILE]) {
			delete [] sc->menu_file;
			sc->menu_file = AE_STRDUP(options[ARG_MENUFILE]);
		} else {
			snprintf(rc_name, 256, "screen%d.menuFile", sn);
			snprintf(rc_class, 256, "Screen%d.MenuFile", sn);
			if (XrmGetResource(database, rc_name, rc_class, &value_type, &value)) {
				delete [] sc->menu_file;
				sc->menu_file = AE_STRDUP(value.addr);
			}
		}

		value_str = NULL;
		if (options[ARG_NRDESKTOPS])
			value_str = options[ARG_NRDESKTOPS];
		else {
			snprintf(rc_name, 256, "screen%d.numberOfDesktops", sn);
			snprintf(rc_class, 256, "Screen%d.NumberOfDesktops", sn);
			if (XrmGetResource(database, rc_name, rc_class, &value_type, &value))
				value_str = value.addr;
		}

		if (value_str) {
			if (sscanf(value_str, "%u", &sc->desktops) != 1) {
				sc->desktops = 1;
			} else {
				if (sc->desktops < 1) sc->desktops = 1;
				if (sc->desktops > 16) sc->desktops = 16;
			}
		} else
			sc->desktops = 1;

		value_str = NULL;
		if (options[ARG_DESKTOPNAMES])
			value_str = options[ARG_DESKTOPNAMES];
		else {
			snprintf(rc_name, 256, "screen%d.desktopNames", sn);
			snprintf(rc_class, 256, "Screen%d.DesktopNames", sn);
			if (XrmGetResource(database, rc_name, rc_class, &value_type, &value))
				value_str = value.addr;
		}
		if (value_str)
			wascreen->net->setDesktopNames(wascreen, value_str);

		value_str = NULL;
		if (options[ARG_VIRTUALSIZE])
			value_str = options[ARG_VIRTUALSIZE];
		else {
			snprintf(rc_name, 256, "screen%d.virtualSize", sn);
			snprintf(rc_class, 256, "Screen%d.VirtualSize", sn);
			if (XrmGetResource(database, rc_name, rc_class, &value_type, &value))
				value_str = value.addr;
		}

		if (value_str) {
			if (sscanf(value_str, "%ux%u", &sc->virtual_x, &sc->virtual_y) != 2) {
				sc->virtual_x = sc->virtual_y = 3;
			}
		} else
			sc->virtual_x = sc->virtual_y = 3;

		if (sc->virtual_x > 20) sc->virtual_x = 20;
		if (sc->virtual_y > 20) sc->virtual_y = 20;
		if (sc->virtual_x < 1) sc->virtual_x = 1;
		if (sc->virtual_y < 1) sc->virtual_y = 1;

		value_str = NULL;
		if (options[ARG_MENUSTACKING])
			value_str = options[ARG_MENUSTACKING];
		else {
			snprintf(rc_name, 256, "screen%d.defaultMenuStacking", sn);
			snprintf(rc_class, 256, "Screen%d.DefaultMenuStacking", sn);
			if (XrmGetResource(database, rc_name, rc_class, &value_type, &value))
				value_str = value.addr;
		}

		if (value_str) {
			if (! strcasecmp("AlwaysAtBottom", value_str))
				sc->menu_stacking = AlwaysAtBottomStackingType;
			else if (! strcasecmp("Normal", value_str))
				sc->menu_stacking = NormalStackingType;
			else
				sc->menu_stacking = AlwaysOnTopStackingType;
		} else
			sc->menu_stacking = AlwaysOnTopStackingType;

		value_str = NULL;
		if (options[ARG_DOCKAPPHOLDERSTACKING])
			value_str = options[ARG_DOCKAPPHOLDERSTACKING];
		else {
			snprintf(rc_name, 256, "screen%d.defaultDockappHolderStacking", sn);
			snprintf(rc_class, 256, "Screen%d.DefaultDockappHolderStacking", sn);
			if (XrmGetResource(database, rc_name, rc_class, &value_type, &value))
				value_str = value.addr;
		}

		if (value_str) {
			if (! strcasecmp("AlwaysAtBottom", value_str))
				sc->dock_stacking = AlwaysAtBottomStackingType;
			else if (! strcasecmp("Normal", value_str))
				sc->dock_stacking = NormalStackingType;
			else
				sc->dock_stacking = AlwaysOnTopStackingType;
		} else
			sc->dock_stacking = AlwaysOnTopStackingType;

		value_str = NULL;
		if (options[ARG_TRANSIENTABOVE])
			value_str = options[ARG_TRANSIENTABOVE];
		else {
			snprintf(rc_name, 256, "screen%d.transientNotAbove", sn);
			snprintf(rc_class, 256, "Screen%d.TransientNotAbove", sn);
			if (XrmGetResource(database, rc_name, rc_class, &value_type, &value))
				value_str = value.addr;
		}

		if (value_str) {
			if (! strcasecmp("true", value_str))
				sc->transient_above = true;
			else
				sc->transient_above = false;
		} else
			sc->transient_above = false;

		value_str = NULL;
		if (options[ARG_FOCUSREVERTTOWINDOW])
			value_str = options[ARG_FOCUSREVERTTOWINDOW];
		else {
			snprintf(rc_name, 256, "screen%d.focusRevertToRoot", sn);
			snprintf(rc_class, 256, "Screen%d.focusRevertToRoot", sn);
			if (XrmGetResource(database, rc_name, rc_class, &value_type, &value))
				value_str = value.addr;
		}

		if (value_str) {
			if (! strcasecmp("true", value_str))
				sc->revert_to_window = true;
			else
				sc->revert_to_window = false;
		} else
			sc->revert_to_window = false;

		value_str = NULL;
		if (options[ARG_EXTERNALBG])
			value_str = options[ARG_EXTERNALBG];
		else {
			snprintf(rc_name, 256, "screen%d.externalBackground", sn);
			snprintf(rc_class, 256, "Screen%d.externalBackground", sn);
			if (XrmGetResource(database, rc_name, rc_class, &value_type, &value))
				value_str = value.addr;
		}

		if (value_str) {
			if (! strcasecmp("true", value_str))
				sc->external_bg = true;
			else
				sc->external_bg = false;
		} else
			sc->external_bg = false;

		value_str = NULL;
		if (options[ARG_INFOCOMMAND])
			value_str = options[ARG_INFOCOMMAND];
		else {
			snprintf(rc_name, 256, "screen%d.infoCommand", sn);
			snprintf(rc_class, 256, "Screen%d.infoCommand", sn);
			if (XrmGetResource(database, rc_name, rc_class, &value_type, &value))
				value_str = value.addr;
		}

		sc->info_command = NULL;

		if (value_str) {
			int i;
			for (i = 0; value_str[i] == ' ' || value_str[i] == '\t'; i++);
			if (value_str[i] != '\0') {
				char *command = AE_STRDUP(value_str);
				sc->info_command =
					preexpand(command, &sc->info_command_dynamic);
			}
		}

		value_str = NULL;
		if (options[ARG_WARNINGCOMMAND])
			value_str = options[ARG_WARNINGCOMMAND];
		else {
			snprintf(rc_name, 256, "screen%d.warningCommand", sn);
			snprintf(rc_class, 256, "Screen%d.warningCommand", sn);
			if (XrmGetResource(database, rc_name, rc_class, &value_type, &value))
				value_str = value.addr;
		}

		sc->warning_command = NULL;

		if (value_str) {
			int i;
			for (i = 0; value_str[i] == ' ' || value_str[i] == '\t'; i++);
			if (value_str[i] != '\0') {
				char *command = AE_STRDUP(value_str);
				sc->warning_command =
					preexpand(command, &sc->warning_command_dynamic);
			}
		}
	}
